#pragma once

#include <iostream>

#define CRLF "\r\n"

//// REPLY CODES

// 381	RPL_YOUREOPER	 	:You are now an IRC operator	– RPL_YOUREOPER is sent back to a client which has just successfully issued an OPER message and gained operator status.

// send Welcome after answering to CAP LS and setup whole user with received information
#define RPL_WELCOME(nickname) (std::string(": 001 ") + nickname + " :Welcome to the IRC server of 42 Heilbronn 🖥 Team: .:GUNSANSLIP:. 👋😎🍉🏝" + CRLF)

// send Quit approvel with message
#define RPL_QUIT(nickname, username, hostname, reason) (":" + nickname + "!" + username + "@" + hostname + " QUIT :Reason: " + reason + CRLF)

// send leaving a Channel approvel
#define RPL_PART(nickname, username, hostname, channel, reason) (":" + nickname + "!" + username + "@" + hostname + " PART " + channel + " :Reason: " + reason + CRLF)

// kick user from channel
#define RPL_KICKUSER(nickname, username, hostname, channel, kickedUser, reason) (":" + nickname + "!" + username + "@" + hostname + " KICK " + channel + " " + kickedUser + " :Reason: " + reason + CRLF)

// send Message of the Day
#define RPL_MOTD() (std::string(": 372 :This is our Message of the Day 🌇") + CRLF)

// send Join approvel after user requested joining a channel
#define RPL_JOINMSG(nickname, username, hostname, channel) (std::string(":") + nickname + "!" + username + "@" + hostname + " JOIN " + channel + CRLF)

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
// <realname>: Realname of the user.
#define RPL_WHOIS(nickname, nicknameWho, usernameWho, hostnameWho, realnameWho) (": 311 " + nickname + " " + nicknameWho + " " + usernameWho + " " + hostnameWho + " * :" + realnameWho + CRLF)

// Set Channel Topic
#define RPL_SETCHTOPIC(nickname, username, hostname, channel, message) (std::string(":") + nickname + "!" + username + "@" + hostname + " TOPIC " + channel + " :" + message + CRLF)
// Send Channel Topic
#define RPL_CHTOPIC(nickname, channel, topic) (": 332 " + nickname + " " + channel + " :" + topic + CRLF)
// Send UserList
#define RPL_USERLIST(nickname, channel, clientslist) (": 353 " + nickname + " @ " + channel + " :" + clientslist + CRLF)
// Send End of UserList
#define RPL_ENDOFUSERLIST(nickname, channel) (": 366 " + nickname + " " + channel + " :END of /NAMES list" + CRLF)

/////// currently not used
// // Send Channel Creationtime
// #define RPL_CREATIONTIME(nickname, channel, creationtime) (": 329 " + nickname + " " + channel + " " + creationtime + CRLF)

// send and set Channelmodes as answer to "/mode <channel> <mode>"
#define RPL_CHANNELMODEIS(nickname, channel, modes) (": 324 " + nickname + " " + channel + " " + modes + CRLF)

#define RPL_UMODEIS(hostname, channel, mode, user) ":" + hostname + " MODE " + channel + " " + mode + " " + user + CRLF
// #define RPL_CHANGEMODE(hostname, channel, mode, arguments) (":" + hostname + " MODE " + channel + " " + mode + " " + arguments + CRLF)
///////

//// ERROR CODES

// send Password incorrect if password does not match
#define ERR_PASSWDMISMATCH(nickname) (": 464 " + nickname + " :Password incorrect." + CRLF)

// send Error that this command is unknown
#define ERR_UNKNOWNCOMMAND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command!" + CRLF)

// send Error if user is not Channel Operator
#define ERR_CHANOPRIVSNEEDED(nickname, channel) (": 482 " + nickname + " " + channel + " :You're not channel operator." + CRLF)

// send Error if nickname is already in use (format have to be exact like this ': 433 ERR_NICKNAMEINUSE ')
#define ERR_NICKNAMEINUSE(nickname, newNickname) (": 433 ERR_NICKNAMEINUSE " + nickname + " :Nickname '" + newNickname + "' is already in use." + CRLF)

// if the command needs more parameters
#define ERR_NEEDMOREPARAMS(nickname) (": 461 " + nickname + " :Not enough parameters." + CRLF)

// if user is not found
#define ERR_NOSUCHNICK(invitedUser, nickname) (": 401 " + invitedUser + " " + nickname + " :'" + invitedUser + "' is not a known user." + CRLF)

// if user is not found
#define ERR_NOTSAMECHANNEL(invitedUser, nickname, channel) (": 401 " + invitedUser + " " + nickname + " :'" + invitedUser + "' hasn't joined to '" + channel + "'." + CRLF)

// if channel is not found
#define ERR_NOSUCHCHANNEL(channel, nickname) (": 401 " + channel + " " + nickname + " :'" + channel + "' is not a known channel." + CRLF)

// if invited user is already on channel
#define ERR_USERONCHANNEL(nickname, channel) (": 443 ERR_USERONCHANNEL " + nickname + " " + channel + CRLF)

// if user send a unknownmode which we dont handle
#define ERR_UNKNOWNMODE(nickname, channel, mode) ": 472 " + nickname + " " + channel + " :'" + mode + "' is a unknown channel mode" + CRLF

// if channel is full
#define ERR_CHANNELISFULL(nickname, channel) ": 471 " + nickname + " " + channel + " :Cannot join channel (+l)" + CRLF

// if channel is invite only
#define ERR_INVITEONLYCHAN(nickname, channel) ": 473 " + nickname + " " + channel + " :Cannot join channel (+i)" + CRLF

// #define ERR_UNKNOWNMODE(nickname, channel, mode) ": 472 " + mode + " " + nickname + " :is unknown mode char to me for " + channel + CRLF

/////// currently not used
// #define ERR_NEEDMODEPARM(channel, mode) (": 696 " + channel + " * You must specify a parameter for the key mode. " + mode + CRLF)
// #define ERR_INVALIDMODEPARM(channel, mode) ": 696 " + channel + " Invalid mode parameter. " + mode + CRLF
// #define ERR_KEYSET(channel) ": 467 " + channel + " Channel key already set. " + CRLF
// #define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You may not reregister !" + CRLF)
// #define ERR_NONICKNAME(nickname) (": 431 " + nickname + " :No nickname given" + CRLF)
// #define ERR_ERRONEUSNICK(nickname) (": 432 " + nickname + " :Erroneus nickname" + CRLF)
// #define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered!" + CRLF)
///////