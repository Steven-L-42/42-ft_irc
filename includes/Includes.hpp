#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <algorithm>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <limits>
#include <poll.h>
#include <vector>
#include <stack>
#include <map>

#define magenta "\033[35m"
#define green "\033[32m"
#define blue "\033[34m"
#define red "\033[31m"
#define res "\033[0m"

#define forever for (; /*ever*/;)
