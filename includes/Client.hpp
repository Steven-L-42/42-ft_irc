/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/22 09:30:22 by slippert          #+#    #+#             */
/*   Updated: 2024/03/23 14:38:43 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

class Client
{
private:
public:
	int socket;
	std::string Nickname;
	std::string Username;
	std::string Realname;
	std::string UserMode;
	std::string Hostname;
	std::string Password;
	std::string recvMsg;
	std::vector<std::string> channel;
	bool Connected;
};