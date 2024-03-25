/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:06:52 by slippert          #+#    #+#             */
/*   Updated: 2024/03/23 13:03:09 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <vector>

class Helper
{
public:
	static std::string itoa(int i);
	static std::string getCurTime();
	static std::vector<std::string> split_buffer(const char *buffer);
	static std::string trim_whitespace(std::string str);
	static std::vector<std::string> splitString(const std::string &str);
};