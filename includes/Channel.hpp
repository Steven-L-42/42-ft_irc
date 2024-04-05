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
	std::string	Modes;
	std::string	Topic;
	bool		invite_only;
	bool		join_invite_only;
};