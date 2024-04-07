#pragma once

#include <iostream>

#define MAX_USERS 1024

class Channel
{
private:
public:
	std::string	Password;
	int			max_users;
	int			user_count;
	bool		has_password;
	std::string	Topic;
	bool		join_invite_only;
	std::string	Modes;
	std::string	Modes_Params;
	bool		user_limit;
	bool		invite_only;
	bool		restricted_topic;
};