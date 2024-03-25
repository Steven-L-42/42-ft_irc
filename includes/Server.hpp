/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:42 by slippert          #+#    #+#             */
/*   Updated: 2024/03/25 10:42:40 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../includes/Includes.hpp"
#include "../includes/Helper.hpp"
#include "../includes/Client.hpp"

class Server
{
private:
	std::map<int, Client> clients;

	int _socket;
	const int _port;
	const std::string _password;
	static const int _maxConnections = 128;
	struct sockaddr_in _ServerAddress;
	struct sockaddr_in _ClientAddress;
	socklen_t _ClientAddrLen;

	static bool Signal;
	static void SignalHandler(int signum);
	void srvInit();
	void srvLstn();
	void srvRecv();
	void srvCheck();
	void srvSend(int _clientSocket, const std::string &message);
	void srvRemv();
	void checkCommand(int _clientSocket);
	bool checkCAP(int _clientSocket, const std::string &msg);
	bool checkJOIN(int _clientSocket, const std::string &msg);
	bool checkPART(int _clientSocket, const std::string &msg);
	bool checkPRIVMSG(int _clientSocket, const std::string &msg);
	bool checkNICK(int _clientSocket, const std::string &msg);
	bool checkWHO(int _clientSocket, const std::string &msg);
	bool checkWHOIS(int _clientSocket, const std::string &msg);
	bool checkMODE(int _clientSocket, const std::string &msg);
	bool checkPING(int _clientSocket, const std::string &msg);
	bool checkQUIT(int _clientSocket, const std::string &msg);
	bool unknownCMD(int _clientSocket, const std::string &msg);

public:
	Server(int port, std::string password);
	~Server();
	void start();
};
