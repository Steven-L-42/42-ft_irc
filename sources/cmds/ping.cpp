#include "../../includes/Commands.hpp"

// reply to every ping with a pong
void Commands::ping(int socket, const std::string &msg)
{
	replyMsg = RPL_PONG(clients[socket].Nickname, clients[socket].Hostname);
	srv->Send(socket, replyMsg);
}