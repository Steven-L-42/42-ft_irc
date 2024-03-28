/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:07:03 by slippert          #+#    #+#             */
/*   Updated: 2024/03/28 13:06:57 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Helper.hpp"

std::string Helper::getCurTime()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);

	std::ostringstream ssHour;
	ssHour << (ltm->tm_hour);
	std::ostringstream ssMin;
	ssMin << (ltm->tm_min);
	std::ostringstream ssSec;
	ssSec << (ltm->tm_sec);
	std::string hour((ltm->tm_hour < 10 ? "0" + ssHour.str() : ssHour.str()));
	std::string min((ltm->tm_min < 10 ? "0" + ssMin.str() : ssMin.str()));
	std::string sec((ltm->tm_sec < 10 ? "0" + ssSec.str() : ssSec.str()));
	std::string localTime = "[" + hour + ":" + min + ":" + sec + "] ";
	return (localTime);
}

std::string Helper::itoa(int i)
{
	std::ostringstream oss;
	oss << i;
	if ((oss.fail()))
		throw(std::runtime_error("Error: oss"));
	return (oss.str());
}

std::string Helper::trim_whitespace(std::string str)
{
	size_t start = str.find_first_not_of(" \n");
	if (start == std::string::npos)
		return str;
	size_t end = str.find_last_not_of(" \n");
	return str.substr(start, end - start + 1);
}

std::vector<std::string> Helper::split_buffer(const char *buffer)
{
	std::vector<std::string> commands;
	std::stringstream strStream(buffer);
	std::string command;
	while (std::getline(strStream, command, ' '))
		commands.push_back(trim_whitespace(command));
	return (commands);
}

std::vector<std::string> Helper::splitString(const std::string &str)
{
	std::istringstream iss(str);
	std::vector<std::string> tokens;
	std::string token;

	while (iss >> token)
	{
		tokens.push_back(token);
	}

	return tokens;
}