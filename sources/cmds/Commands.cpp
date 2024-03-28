#include "../../includes/Commands.hpp"

Commands::Commands(Server *_srv) : srv(_srv), clients(srv->getClients()), channels(srv->getChannels())
{
}

Commands::~Commands()
{
}
