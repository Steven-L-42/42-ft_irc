#include "../../includes/Commands.hpp"
#include "../../includes/Helper.hpp"
#include "../../includes/Channel.hpp"



bool	Commands::test_jan(std::string msg)
{
	std::vector<std::string> splitted = Helper::split_buffer(msg.c_str());

	if (splitted.size() < 2)
		return false;
	if (splitted[1] == "+k" || splitted[1] == "-k") {
		if (splitted.size() < 3 || splitted[1] == "-k") {
			channels[channelName].has_password = false;
			channels[channelName].Password = "";
			return true;
		}
		else {
			std::string temp = Helper::trim_whitespace(splitted[2]);
			channels[channelName].Password = temp;
			channels[channelName].has_password = true;
			return true;
		}
	}
	return false;
}


bool	Commands::topic(std::string msg) {
	std::vector<std::string> splitted = Helper::split_buffer(msg.c_str());

	if (splitted.size() < 2)
		return false;
	if (splitted[1] == "+t" || splitted[1] == "-t") {
		if (splitted.size() < 3 || splitted[1] == "-k") {
			channels[channelName].Topic = "";
			return true;
		}
		else {
			std::string newTopic = splitted[2];
			for (std::vector<std::string>::iterator it = splitted.begin() + 3; it != splitted.end(); it++) {
				newTopic += " ";
				newTopic += *it;
			}
			channels[channelName].Topic = newTopic;
			return true;
		}
	}
	return false;
}


bool	Commands::invite_only(std::string msg) {
	std::vector<std::string> splitted = Helper::split_buffer(msg.c_str());

	if (splitted.size() < 2)
		return false;
	if (splitted[1] == "+i" || splitted[1] == "-i") {
		if (splitted[1] == "+i")
			channels[channelName].invite_only = true;
		else
			channels[channelName].invite_only = false;
		return true;
	}
	return false;
}


bool	Commands::operator_priv(std::string msg, int socket) {
	std::vector<std::string> splitted = Helper::split_buffer(msg.c_str());

	if (splitted.size() < 2)
		return false;
	if (splitted[1] == "+o" || splitted[1] == "-o") {
		if (splitted[1] == "+o")
			clients[socket].channels[channelName].isOp = true;
		else
			clients[socket].channels[channelName].isOp = false;
		return true;
	}
	return false;
}


bool	Commands::set_user_limit(std::string msg) {
	std::vector<std::string> splitted = Helper::split_buffer(msg.c_str());

	if (splitted.size() < 2)
		return false;
	if (splitted[1] == "+l" || splitted[1] == "-l") {
		if (splitted[1] == "+l" && splitted.size() > 3) {
			std::string temp = splitted[2];
			channels[channelName].max_users = std::atoi(temp.c_str());
		}
		else
			channels[channelName].max_users = MAX_USERS;
		return true;
	}
	return false;
}
// MODE (Operator Command) - Change the channel’s mode:
// Done ! i: Set/remove Invite-only channel
// Done ! t: Set/remove the restrictions of the TOPIC command to channel operators
// Done ! k: Set/remove the channel key (password)
// Done ! o: Give/take channel operator privilege 
// Done ! l: Set/remove the user limit to channel
void Commands::mode(int socket, const std::string &msg)
{
	if (clients[socket].channels[channelName].isOp == false) {
		replyMsg = ERR_CHANOPRIVSNEEDED(clients[socket].Nickname, channelName);
		srv->Send(socket, replyMsg);
		return ;
	}
	(void)msg;
	if (test_jan(msg) == true || topic(msg) == true || invite_only(msg) == true
		|| operator_priv(msg, socket) == true)
		return ;

	replyMsg = ": 421 /mode command not found " + std::string(CRLF);
	srv->Send(socket, replyMsg);
	return;
}

	// clients[socket].recvMsg = "";
	// // substract channel name without # and without \r\n
	// std::string substrCHANNEL = msg.substr(modeSize, msg.size() - modeSize - 2);
	//

	// // send Channel modes
	// replyMsg = RPL_CHANNELMODEIS(clients[socket].Nickname, substrCHANNEL, " ");
	// srv->Send(socket, replyMsg);
	// replyMsg = ": 421 " + clients[socket].Nickname + " MODE :Command is currently deactivated by Steven!" + CRLF;