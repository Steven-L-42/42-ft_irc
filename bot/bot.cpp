/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agunczer <agunczer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 15:50:35 by agunczer          #+#    #+#             */
/*   Updated: 2024/04/16 12:17:56 by agunczer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"

// my name is bot ..., ...bot

Bot::Bot()
{
    std::cout << "Server to connect: ";
    std::cin >> this->_IP_ADDR;;
    std::cout << "At port: ";
    std::cin >> this->_PORT;;
    std::cout << "To channel: ";
    std::cin >> this->_CHANNEL;;
    this->_NICK = "bitchbot";
    this->_USER = "bitchbot";
    this->_REALNAME = "bitchbot";
    this->_PASSWORD = "bitchbotisthebest";
    
    std::cout << "Bot class has been created" << std::endl;
}

Bot::Bot(std::string IP_ADDR, int PORT, std::string CHANNEL)
{
    this->_IP_ADDR = IP_ADDR;
    this->_PORT = PORT;
    this->_CHANNEL = CHANNEL;
    this->_NICK = "bitchbot";
    this->_USER = "bitchbot";
    this->_REALNAME = "bitchbot";
    this->_PASSWORD = "bitchbotisthebest";
}

Bot::~Bot()
{

}

Bot::Bot(const Bot &copy)
{
    *this = copy;
}

const Bot &Bot::operator=(const Bot &assignment)
{
    *this = assignment;
    
    return *this;
}

const std::string &Bot::getIpAddr()
{
    return this->_IP_ADDR;
}

const int &Bot::getPort()
{
    return this->_PORT;
}

const std::string &Bot::getChannel()
{
    return this->_CHANNEL;
}

const std::string &Bot::getNick()
{
    return this->_NICK;
}

const std::string &Bot::getUser()
{
    return this->_USER;
}

const std::string &Bot::getRealName()
{
    return this->_REALNAME;
}

const std::string &Bot::getPassword()
{
    return this->_PASSWORD;
}