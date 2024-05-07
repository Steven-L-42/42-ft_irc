#include "../../includes/Commands.hpp"

std::vector<std::string> NetCat(std::string msg)
{
	std::vector<std::string> output;
	std::string::iterator it_msg = msg.begin();
	std::string temp = "";
	for (; it_msg != msg.end(); it_msg++)
	{
		if (*it_msg == '|' || *it_msg == ' ')
		{
			output.push_back(temp);
			temp = "";
			continue;
		}
		temp += *it_msg;
	}

	return output;
}

// CAP is the login procedure, it is the first thing that
// the Server receives after getting a incoming connection request
void Commands::cap(int socket, const std::string &msg)
{
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
		// received END of REQ and set LoginProcess to END to receive now the PASS and USER Data to register
		clients[socket].LoginProcess = "END";
		return;
	}
	if (clients[socket].LoginProcess == "END")
	{
		clients[socket].LoginProcess = "DONE";
		std::vector<std::string> capTokens = Helper::splitString(msg);
		if (capTokens.size() < 9)
			capTokens = NetCat(msg);
		std::vector<std::string>::iterator itCap = capTokens.begin();

		// set <password>
		if (itCap != strTokens.end() && *itCap == "PASS")
		{
			// std::cout << "1." << *itCap << std::endl;
			if (++itCap != strTokens.end() && !itCap->empty())
				itCap->erase(0, 1);
			// std::cout << "2." << *itCap << std::endl;
			if (*itCap == srv->getPassword())
				clients[socket].Password = *itCap;
			++itCap;
		}
		// std::cout << "3." << *itCap << std::endl;
		// std::cout << "4." << clients[socket].Password << ":" << srv->getPassword() << std::endl;
		if (clients[socket].Password != srv->getPassword())
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
				nick(socket, "NICK " + *itCap + CRLF);
			++itCap;
		}
		// if naming wasnt successfull
		if (clients[socket].Accepted == false)
		{
			clients[socket].Connected = false;
			return;
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
		clients[socket].Registred = true;
		return;
	}
}
