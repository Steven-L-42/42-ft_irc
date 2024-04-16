/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agunczer <agunczer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 15:50:30 by agunczer          #+#    #+#             */
/*   Updated: 2024/04/16 14:52:27 by agunczer         ###   ########.fr       */
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
};


#endif