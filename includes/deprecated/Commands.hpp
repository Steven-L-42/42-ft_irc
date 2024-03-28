#pragma once

#include "commands/CAP.hpp"
#include "commands/JOIN.hpp"
#include "commands/PART.hpp"
#include "commands/PRIVMSG.hpp"
#include "commands/NICK.hpp"
#include "commands/WHO.hpp"
#include "commands/WHOIS.hpp"
#include "commands/MODE.hpp"
#include "commands/PING.hpp"
#include "commands/KICK.hpp"
#include "commands/QUIT.hpp"
#include "commands/TOPIC.hpp"
#include "commands/UNKNOWN.hpp"

class Commands : public CAP, public JOIN, public PART, public PRIVMSG, public NICK, public WHO, public WHOIS, public MODE, public PING, public KICK, public QUIT, public TOPIC, public UNKNOWN
{
};
