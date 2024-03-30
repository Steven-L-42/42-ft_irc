#include "../../includes/Commands.hpp"

void Commands::unknown(int socket, const std::string &msg)
{
	replyMsg = ERR_UNKNOWNCOMMAND(clients[socket].Nickname, msg);
	srv->Send(socket, replyMsg);
}
