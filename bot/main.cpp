/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:00:36 by agunczer          #+#    #+#             */
/*   Updated: 2024/05/06 15:17:20 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "bot.hpp"
#include <sstream>
#include <vector>
#include <signal.h>
#include <sys/select.h>

bool signalState = false;
int sockfd;

void send_message(int sockfd, const char *message)
{
    std::string msg = std::string(message) + "\r\n";
    send(sockfd, msg.c_str(), msg.length(), 0);
}

std::vector<std::string> splitString(const std::string &str)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}

bool checkPassword(Bot *botInstance, std::pair<std::string, std::string> credentials)
{
    if (credentials.second == botInstance->getBOTPassword())
        return true;
    else
        return false;
}
// command: 'opme <nickname> <botpassword>'
std::string checkOPME(Bot *botInstance, std::string input)
{
    std::vector<std::string> commandExtract = splitString(input);
    if (commandExtract.size() == 6)
        commandExtract.erase(commandExtract.begin(), commandExtract.end() - 4);
    else
        return "";

    if (commandExtract[0] == "bitchbot")
    {
        if (commandExtract[1] == ":opme")
        {
            std::pair<std::string, std::string> credentials(commandExtract[2], commandExtract[3]);
            if (checkPassword(botInstance, credentials))
            {
                return ("MODE " + botInstance->getChannel() + " +o " + credentials.first + "\r\n");
            }
        }
    }
    return "";
}

int handleReceive(Bot *bot, int bytesReceived, char buffer[1024])
{
    if (bytesReceived < 0)
    {
        std::cerr << "Error receiving data" << std::endl;
        close(sockfd);
        return 1;
    }
    else if (bytesReceived == 0)
    {
        std::cout << "Disconnected from server" << std::endl;
        close(sockfd);
        return 1;
    }
    else
    {
        // Process received data (parse IRC messages, handle commands, etc.)
        buffer[bytesReceived] = '\0'; // Null-terminate the received data
        std::string received = buffer;
        std::cout << "Received: " << received << std::endl;
        std::string opString = checkOPME(bot, received);
        if (opString != "")
            send_message(sockfd, opString.c_str());
        return 0;
    }
}

void startConnection(Bot *bot)
{
    char BUFFER[1024];
    send_message(sockfd, ("CAP LS\r\n"));
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0)
    {
        sleep(1);
    }
    bzero(BUFFER, 1024);
    send_message(sockfd, ("CAP REQ :multi-prefix\r\n"));
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0)
    {
        sleep(1);
    }
    bzero(BUFFER, 1024);
    send_message(sockfd, ("CAP END\r\n"));
    bzero(BUFFER, 1024);
    // Send NICK and USER commands to identify with the server
    send_message(sockfd, (("PASS :" + bot->getPassword() + "\nNICK " + std::string(bot->getNick()) + "\n" + "USER " + std::string(bot->getUser()) + " 0 irc.localhost.net :" + std::string(bot->getRealName())) + "\r\n").c_str());
    bzero(BUFFER, 1024);
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0)
    {
        sleep(1);
    }
    bzero(BUFFER, 1024);

    // Join the channel
    bzero(BUFFER, 1024);
    send_message(sockfd, (("JOIN " + std::string(bot->getChannel()) + " " + bot->getCHPassword() + "\r\n").c_str()));
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0)
    {
        sleep(1);
    }
    std::cout << BUFFER << std::endl;
    bzero(BUFFER, 1024);
    // Send a message to the channel
    send_message(sockfd, (("PRIVMSG " + std::string(bot->getChannel()) + " :Hello, I'm a bot!") + "\r\n").c_str());
}

// bitchbot sends: MODE #wedogreat +o test
// Received: :test!agunczer@irc.localhost.net PRIVMSG bitchbot :opme steven password
// QUIT :KVIrc 5.0.0 Aria http://www.kvirc.net/

static void signalHandler(int signum)
{
    std::cout << signum << std::endl;
    (void)signum;
    // signalState = true;
}

#define CRLF "/r/n"
int main()
{
    // signalState = false;
    signal(SIGINT, signalHandler);
    Bot bot("127.0.0.1", 3000, "#wedogreat");
    // Bot bot;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(bot.getPort());
    serverAddr.sin_addr.s_addr = inet_addr(bot.getIpAddr().c_str());

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error connecting to server" << std::endl;
        close(sockfd);
        return 1;
    }

    // Sets up connection to server
    startConnection(&bot);
    // Receive and process messages (loop)
    // INITIALIZE SET TO MONITOR FOR READABILITY
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    FD_SET(STDIN_FILENO, &readfds);

    int maxfd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;

    char buffer[1024];
    while (true)
    {
        bzero(buffer, 1024);

        // Wait for activity on any of the monitored file descriptors
        int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0)
        {
            if (errno == EINTR)
            {
                // This indicates that the select call was interrupted by a signal
                std::cout << "Received signal, exiting gracefully..." << std::endl;
                send_message(sockfd, "QUIT :KVIrc 5.0.0 Aria http://www.kvirc.net/");
                close(sockfd);
                return 0;
            }
            else
            {
                // Another error occurred
                std::cerr << "Error in select: " << strerror(errno) << std::endl;
                close(sockfd);
                return 1;
            }
        }
        if (FD_ISSET(sockfd, &readfds))
        {
            // Receive and process data from the socket
            // (Code for receiving and processing data from the socket goes here)
            int bytesReceived = recv(sockfd, buffer, 1024, 0);
            int receiveStatus = handleReceive(&bot, bytesReceived, buffer);
            if (receiveStatus == 1)
                return 1;
        }

        // Clear the file descriptor set for the next iteration
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
    }

    close(sockfd);
    return 0;
}
