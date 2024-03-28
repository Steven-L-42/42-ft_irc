#include "../../includes/Server.hpp"

bool checkPort(char *_port, int *port)
{
	*port = static_cast<int>(std::atoi(_port));
	if (*port >= 1024 && *port <= 49151)
		return (true);
	else
		return (false);
}

int main(int argc, char **argv)
{
	int port = 0;
	if (argc != 3 || (argc == 3 && !checkPort(argv[1], &port)))
		return (std::cerr << "usage ./ircserv <port> <password>\n", 1);

	std::string password(argv[2]);
	Server server(port, password);
	try
	{
		server.start();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
