#pragma once

#include <iostream>

#define MAX_USERS 1024

class Channel
{
private:
public:
	std::string	Password;
	size_t			max_users;
	size_t			user_count;
	bool		has_password;
	std::string	Modes;
	std::string	Topic;
	bool		invite_only;
	bool		join_invite_only;
	bool		restricted_topic;
};