/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channelMode.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljerinec <ljerinec@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 10:22:07 by smunio            #+#    #+#             */
/*   Updated: 2024/05/14 11:02:19 by ljerinec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libs.hpp"

void Channel::mode(std::string &line, User &caller, Server &server)
{
	(void)server;

	if (line.find('#') + this->_name.size() == line.size())
	{
		std::stringstream	nb;
		nb << _created_at;
		caller.send_message(": 324 " + caller.get_nick() + " " + this->_name + " +" + "\r\n");
		caller.send_message(": 329 " + caller.get_nick() + " " + this->_name + " " + nb.str() + "\r\n");
		return ;
	}
	char *opt = (char *)line.c_str();
		opt = strtok(NULL, " ");
		opt = strtok(NULL, " ");
	if ((opt[1] != 'o' && opt[1] != 'i' && opt[1] != 'l' && opt[1] != 't' && opt[1] != 'k'
		&& opt[0] != '+' && opt[0] != '-') || strlen(opt) > 2)
		return ;
	if (is_operator(caller.get_nick()) == false)
	{
		caller.send_message(":ft_irc 482 " + caller.get_host_info() + " " + _name + " :You're not channel operator\r\n");
		throw Error("not channel op");
	}
	if (opt != NULL && opt[1] == 'o')
		this->mode_o(line, opt, caller);
	else if (opt != NULL && opt[1] == 'l')
		this->mode_l(line, opt, caller);
	else if (opt != NULL && opt[1] == 't')
		this->mode_t(opt, caller);
	else if (opt != NULL && opt[1] == 'i')
		this->mode_i(opt, caller);
	else if (opt != NULL && opt[1] == 'k')
		this->mode_k(opt, line, caller);
}

void Channel::mode_o(std::string &line, char *opt, User &caller)
{
	if (line.find("MODE") + 9 + this->_name.size() >= line.size())
		throw Error("no target for MODE o");
	std::string	tar = line.substr(line.find("MODE") + 9 + this->_name.size(), line.size());
	std::list<User *>::iterator it;

	if (opt[0] == '-')
	{
		for (it = this->_operators.begin(); it != this->_operators.end(); it++)
		{
			if ((*it)->get_nick() == tar)
			{
					this->_operators.erase(it);
					this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + tar + "\r\n");
					break ;
			}
		}
	}
	else
	{
		for (it = this->_userInChannel.begin(); it != this->_userInChannel.end(); it++)
		{
			if ((*it)->get_nick() == tar)
			{
					this->_operators.push_back(*it);
					this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + tar + "\r\n");
					break ;
			}
		}
		if (it == this->_userInChannel.end())
			caller.send_message(":ft_irc 401 " + caller.get_host_info()  + " " + tar + ":No such nick/channel");
	}
}

void    Channel::mode_l(std::string &line, char *opt, User &caller)
{
	if (opt[0] == '+')
	{
		if (line.find(opt) + 3 >= line.size())
		{
			caller.send_message(": 461 " + caller.get_host_info() + " " + _name + " MODE +l :Not enough parameters\r\n");
			throw Error("not enough parameters mode +l");
		}
		std::string count = line.substr(line.find(opt) + 3, line.size());
		if (!isdigit(count[0]))
			throw Error("wrong MODE +l opt");
		this->_max_users += atoi(count.c_str());
		std::stringstream	nb;
		nb << _max_users;
		this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + nb.str() + "\r\n");
	}
	if (opt[0] == '-')
	{
		this->_max_users = 0;
		this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + "\r\n");
	}
}

void    Channel::mode_t(char *opt, User &caller)
{
	if (opt[0] == '+')
	{
		this->_topic_mode = TOPIC_OP;
		this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + "\r\n");
	}
	if (opt[0] == '-')
	{
		this->_topic_mode = TOPIC_ALL;
		this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + "\r\n");
	}

}

void    Channel::mode_i(char *opt, User &caller)
{
	if (opt[0] == '+')
	{
		this->_invite_mode = ON_INVITE;
		this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + "\r\n");
	}
	if (opt[0] == '-')
	{
		this->_invite_mode = NO_INVITE;
		this->send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + "\r\n");
	}
}

void    Channel::mode_k(char *opt, std::string &line, User &caller)
{
	if (strlen(opt) > 2)
	{
		caller.send_message(": NOTICE " + caller.get_host_info() + " :We do not handle those MODE options.\r\n");
		throw Error("wrong mode opt");
	}
	if (opt[0] == '+')
	{
		if (line.find('+') + 3 >= line.size())
			throw Error("missing password");
		std::string pass = line.substr(line.find('+') + 3, line.size());
		this->_password = pass;
		this->_need_pass = true;
		send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + pass + "\r\n");
	}
	if (opt[0] == '-' && _need_pass == true)
	{
		std::string pass = line.substr(line.find('-') + 3, line.size());
		if (pass != this->_password)
			throw Error("can't remove password: Wrong password");
		this->_password.clear();
		this->_need_pass = false;
		send_to_all_user(":" + caller.get_host_info() + " MODE " + this->_name + " " + opt + " " + pass + "\r\n");
	}
}
