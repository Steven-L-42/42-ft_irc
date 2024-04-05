/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsanger <jsanger@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:07:03 by slippert          #+#    #+#             */
/*   Updated: 2024/04/04 18:10:06 by jsanger          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Helper.hpp"

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

std::string Helper::trim_whitespace_jan(std::string str)
{
	int start = 0;
	for (; str[start]; start++) {
		if (str[start] > 32 && str[start] < 126)
			break ;
	}

	int end = str.length();
	for (; end > 0; end--) {
		if (end - 1 < 0 || (str[end - 1] > 32 && str[end - 1] < 126))
			break ;
		
	}
	std::string sub = str.substr(start, end);
	return sub;
}