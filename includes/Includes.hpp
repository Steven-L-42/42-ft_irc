/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Includes.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:22:15 by slippert          #+#    #+#             */
/*   Updated: 2024/03/27 14:17:03 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <algorithm>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <poll.h>
#include <vector>
#include <stack>
#include <map>

#define magenta "\033[35m"
#define green "\033[32m"
#define blue "\033[34m"
#define red "\033[31m"
#define res "\033[0m"

#define forever for (; /*ever*/;)

#define CRLF "\r\n"

//// REPLY CODES

// 381	RPL_YOUREOPER	 	:You are now an IRC operator	– RPL_YOUREOPER is sent back to a client which has just successfully issued an OPER message and gained operator status.

// send Welcome after answering to CAP LS and setup whole user with received information
#define RPL_WELCOME(nickname) (std::string(": 001 ") + nickname + " :Welcome to the IRC server of 42 Heilbronn 🖥 Team: .:GUNSANSLIP:. 👋😎🍉🏝" + CRLF)

// send Quit approvel with message
#define RPL_QUIT(nickname, username, hostname, message) (":" + nickname + "!" + username + "@" + hostname + " QUIT :" + message + " ; User " + nickname + " has left the Server: " + message + "." + CRLF)

// send leaving a Channel approvel
#define RPL_PART(nickname, username, hostname, channel) (":" + nickname + "!" + username + "@" + hostname + " PART " + channel + " ; " + nickname + " is leaving the channel " + channel + CRLF)

// send Message of the Day
#define RPL_MOTD() (std::string(": 372 :This is our Message of the Day 🌇") + CRLF)

// send Join approvel after user requested joining a channel
#define RPL_JOINMSG(nickname, ipaddress, channelname) (std::string(":") + nickname + "!" + nickname + "@" + ipaddress + " JOIN " + channelname + CRLF)

// send Message to all other users there on same channel or a single privat user
#define RPL_MESSAGE(nickname, username, hostname, recipient, message) (std::string(":") + nickname + "!" + username + "@" + hostname + " PRIVMSG " + recipient + " " + message + CRLF)

// send nickname change approvel after user changed it with command /nick <username>
#define RPL_NICKCHANGE(oldnickname, nickname) (std::string(":") + oldnickname + " NICK " + nickname + CRLF)

// send PONG on incoming PING
#define RPL_PONG(nickname, hostname) (std::string("PONG ") + nickname + " " + hostname + CRLF)


// Send Information about requested User (nicknameWho)
// :server 311 <nick> <client> <username> <hostname> * <realname>
// <nick>: Nickname of the connected user.
// <client>: Nickname of the user being WHOIS‘d.
// <username>: Username of the user.
// <hostname>: Hostname of the user.
// <realname>: Realname (or “gecos”) of the user.
#define RPL_WHOIS(nickname, nicknameWho, usernameWho, hostnameWho, realnameWho) (": 311 " + nickname + " " + nicknameWho + " " + usernameWho + " " + hostnameWho + " * :" + realnameWho + CRLF)

// Set Channel Topic
#define RPL_SETCHTOPIC(nickname, username, hostname, channel, message) (std::string(":") + nickname + "!" + username + "@" + hostname + " TOPIC " + channel + " :" + message + CRLF)
// Send Channel Topic
#define RPL_CHTOPIC(nickname, channelname, topic) (": 332 " + nickname + " " + channelname + " :" + topic + CRLF)
// Send UserList
#define RPL_USERLIST(nickname, channelname, clientslist) (": 353 " + nickname + " @ " + channelname + " :" + clientslist + CRLF)
// Send End of UserList
#define RPL_ENDOFUSERLIST(nickname, channelname) (": 366 " + nickname + " " + channelname + " :END of /NAMES list" + CRLF)

// Send Channel Creationtime
#define RPL_CREATIONTIME(nickname, channelname, creationtime) (": 329 " + nickname + " #" + channelname + " " + creationtime + CRLF)

// send Channelmodes as answer to "MODE #channelname"
#define RPL_CHANNELMODEIS(nickname, channelname, modes) (": 324 " + nickname + " #" + channelname + " " + modes + CRLF)

#define RPL_UMODEIS(hostname, channelname, mode, user) ":" + hostname + " MODE " + channelname + " " + mode + " " + user + CRLF
#define RPL_CHANGEMODE(hostname, channelname, mode, arguments) (":" + hostname + " MODE #" + channelname + " " + mode + " " + arguments + CRLF)

//// ERROR CODES

// send Password incorrect if password does not match
#define ERR_PASSWDMISMATCH(nickname) (": 464 " + nickname + " :Password incorrect" + CRLF)

// send Error that this command is unknown
#define ERR_UNKNOWNCOMMAND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command!" + CRLF)

// send Error if user is not Channel Operator
#define ERR_CHANOPRIVSNEEDED(nickname, channel) (": 482 " + nickname + " " + channel + " :You're not channel operator." + CRLF)

#define ERR_NEEDMODEPARM(channelname, mode) (": 696 #" + channelname + " * You must specify a parameter for the key mode. " + mode + CRLF)
#define ERR_INVALIDMODEPARM(channelname, mode) ": 696 #" + channelname + " Invalid mode parameter. " + mode + CRLF
#define ERR_KEYSET(channelname) ": 467 #" + channelname + " Channel key already set. " + CRLF
#define ERR_UNKNOWNMODE(nickname, channelname, mode) ": 472 " + nickname + " #" + channelname + " " + mode + " :is not a recognised channel mode" + CRLF
#define ERR_NOTENOUGHPARAM(nickname) (": 461 " + nickname + " :Not enough parameters." + CRLF)
#define ERR_CHANNELNOTFOUND(nickname, channelname) (": 403 " + nickname + " " + channelname + " :No such channel" + CRLF)
#define ERR_NOSUCHNICK(channelname, name) (": 401 #" + channelname + " " + name + " :No such nick/channel" + CRLF)
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You may not reregister !" + CRLF)
#define ERR_NONICKNAME(nickname) (": 431 " + nickname + " :No nickname given" + CRLF)
#define ERR_NICKINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + CRLF)
#define ERR_ERRONEUSNICK(nickname) (": 432 " + nickname + " :Erroneus nickname" + CRLF)
#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered!" + CRLF)
