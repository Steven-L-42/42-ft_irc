#include "../../includes/Commands.hpp"

// CAP is the login procedure, it is the first thing that
// the Server receives after getting a incoming connection request
void Commands::cap(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;

	if (*itToken == "LS")
	{
		std::cout << "TEST" << std::endl;
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
		// strTokens = Helper::splitString(incomingMsg);
		// itToken = strTokens.begin();
		std::__1::vector<std::__1::string> capTokens = Helper::splitString(incomingMsg);
		std::vector<std::string>::iterator itCap = capTokens.begin();

		bool foundPASS = false;
		// set <password>
		if (itCap != strTokens.end() && *itCap == "PASS")
		{
			if (++itCap != strTokens.end() && !itCap->empty())
				itCap->erase(0, 1);
			if (*itCap == srv->getPassword())
			{
				foundPASS = true;
				clients[socket].Password = *itCap;
			}
			++itCap;
		}
		if (!foundPASS)
		{
			replyMsg = ERR_PASSWDMISMATCH(clients[socket].Nickname);
			srv->Send(socket, replyMsg);
			clients[socket].Connected = false;
			return;
		}

		// set <nickname> and send reply to client
		if (itCap != capTokens.end() && *itCap == "NICK")
		{
			if (++itCap != capTokens.end())
				nick(socket, "NICK " + *itCap + "\r\n");
			++itCap;
		}

		if (itCap != capTokens.end() && *itCap == "USER")
		{

			// set <username>
			if (++itCap != capTokens.end())
				clients[socket].Username = *itCap;

			// set <usermode>
			if (++itCap != capTokens.end())
				clients[socket].UserMode = *itCap;

			// set <hostname>
			if (++itCap != capTokens.end())
				clients[socket].Hostname = *itCap;

			// set <realname>
			// skip ':1,8' (kvIRC comment)
			if (++itCap != capTokens.end() && !itCap->empty())
				itCap->erase(0, 5);
			while (itCap != capTokens.end())
			{
				clients[socket].Realname += *itCap;
				if (++itCap != capTokens.end())
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
