/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 15:50:30 by agunczer          #+#    #+#             */
/*   Updated: 2024/05/06 15:07:27 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifndef BOT_HPP
#define BOT_HPP

#pragma once

class Bot {
    private:
        std::string _IP_ADDR;
        int _PORT;
        std::string _CHANNEL;
        std::string _NICK;
        std::string _USER;
        std::string _REALNAME;
        std::string _PASSWORD;
        std::string _CHPASSWORD;
        std::string _BOTPASSWORD;

    public:
        Bot();
        Bot(std::string IP_ADDR, int PORT, std::string CHANNEL);
        ~Bot();
        Bot(const Bot &copy);
        const Bot &operator=(const Bot &assignment);
        const std::string &getIpAddr();
        const int &getPort();
        const std::string &getChannel();
        const std::string &getNick();
        const std::string &getUser();
        const std::string &getRealName();
        const std::string &getPassword();
        const std::string &getCHPassword();
        const std::string &getBOTPassword();
};


#endif