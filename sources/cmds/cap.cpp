#include "../../includes/Commands.hpp"

void Commands::cap(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;

	if (*itToken == "LS")
	{
		replyMsg = "CAP * LS :multi-prefix sasl\r\n";
		srv->Send(socket, replyMsg);
		return;
	}
	if (*itToken == "REQ")
	{
		// substract all after "CAP REQ "
		itToken++;
		replyMsg = "CAP * ACK " + *itToken + CRLF;
		srv->Send(socket, replyMsg);
		return;
	}
	if (*itToken == "END")
	{
		std::string incomingMsg;
		// wait for receiving NICK <username and USER <username> <usermode> <hostname> <clientcomment>
		size_t breakit = 0;
		while (srv->getSignal() == false)
		{
			char buffer[1024];
			ssize_t bytes_received = recv(socket, buffer, sizeof(buffer), 0);
			if (bytes_received < 1)
			{
				// uncool solution ^^
				if (++breakit >= 99999999)
					break;
				continue;
			}

			buffer[bytes_received] = '\0';
			incomingMsg = std::string(buffer);
			break;
		}
		std::cout << magenta << "srvRecv: " << incomingMsg << res;

		// create tokens, split string on each space ' '.
		strTokens = Helper::splitString(incomingMsg);
		itToken = strTokens.begin();

		bool foundPASS = false;
		// set <password>
		if (itToken != strTokens.end() && *itToken == "PASS")
		{
			if (++itToken != strTokens.end() && !itToken->empty())
				itToken->erase(0, 1);
			if (*itToken == srv->getPassword())
			{
				foundPASS = true;
				clients[socket].Password = *itToken;
			}
			++itToken;
		}
		if (!foundPASS)
		{
			replyMsg = ERR_PASSWDMISMATCH(clients[socket].Nickname);
			srv->Send(socket, replyMsg);
			clients[socket].Connected = false;
			return;
		}

		// set <nickname> and send reply to client
		if (itToken != strTokens.end() && *itToken == "NICK")
		{
			if (++itToken != strTokens.end())
				nick(socket, "NICK " + *itToken + "\r\n");
			++itToken;
		}
		if (itToken != strTokens.end() && *itToken == "USER")
		{
			// set <username>
			if (++itToken != strTokens.end())
				clients[socket].Username = *itToken;
			// set <usermode>
			if (++itToken != strTokens.end())
				clients[socket].UserMode = *itToken;
			// set <hostname>
			if (++itToken != strTokens.end())
				clients[socket].Hostname = *itToken;
			// set <realname>
			// skip ':1,8' (kvIRC comment)
			if (++itToken != strTokens.end() && !itToken->empty())
				itToken->erase(0, 5);
			while (itToken != strTokens.end())
			{
				clients[socket].Realname += *itToken;
				if (++itToken != strTokens.end())
					clients[socket].Realname += " ";
			}
		}

		// send welcome message and login
		replyMsg = RPL_WELCOME(clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		// send message of the day
		replyMsg = RPL_MOTD();
		srv->Send(socket, replyMsg);
	}
}
