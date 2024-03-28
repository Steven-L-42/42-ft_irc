#include "../../includes/Commands.hpp"

void Commands::ping(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	replyMsg = RPL_PONG(clients[socket].Nickname, clients[socket].Hostname);
	srv->Send(socket, replyMsg);
}