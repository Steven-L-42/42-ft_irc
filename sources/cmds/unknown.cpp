#include "../../includes/Commands.hpp"

// received command is not known by us, we will send an error back
void Commands::unknown(int socket, const std::string &msg)
{
	replyMsg = ERR_UNKNOWNCOMMAND(clients[socket].Nickname, msg);
	srv->Send(socket, replyMsg);
}
