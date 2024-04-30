/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   userCommands.class.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljerinec <ljerinec@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 10:22:51 by ljerinec          #+#    #+#             */
/*   Updated: 2024/04/30 13:39:04 by ljerinec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libs.hpp"

void    User::negotiation(Server &server)
{
	std::vector<std::string> parsed;
	std::string tmp;
	size_t closest;
	
	while (_data.size() && (_data.find('\r') != _data.npos || _data.find('\n') != _data.npos))
	{
		if (_data.find('\r') < _data.find('\n'))
			closest = _data.find('\r');
		else
			closest = _data.find('\n');
		tmp = _data.substr(0, closest);
		this->parse_negotiation(tmp, server);
		_data.erase(0, closest + 1);
	}
	_data.clear();
}

void    User::registration(Server &server)
{
	send_message(":ft_irc 001 " + _nickname + " :Welcome to the 'ft_irc' Network, " + _nickname + "\r\n");
	send_message(":ft_irc 002 " + _nickname + " :Your host is " + server.get_servername() + ", running version 1.0" + "\r\n");
	send_message(":ft_irc 003 " + _nickname + " :This server was created " + server.get_dt() + "\r\n");
	send_message(":ft_irc 004 " + _nickname + " :" + server.get_servername() + " version 1.0\r\n");
	send_message(":ft_irc 005 " + _nickname + " NETWORK=FT_IRC CHANLIMIT=#:25 NICKLEN=30 TOPICLEN=307 KICKLEN=307 CHANNELLEN=32 CHANTYPES=# PREFIX=(o)@ :are available on this server\r\n");
	send_message(":ft_irc 005 " + _nickname + " CASEMAPPING=ascii CHANMODES=,ko,l,it :are available on this server\n");
	change_status(CONNECTED);
}

void    User::change_status(int status)
{
	if (this->_status == 0 && status == 2)
		throw Error("can't skip registration");
	if (this->_status == 2 && status == 1)
		throw Error("user is already registered");
	this->_status = status;
}

void	User::change_nick(std::string new_nick, Server &server)
{
	std::string old_nick = this->_nickname;
	this->_nickname = new_nick;
	while (_nickname.find("#") != _nickname.npos)
		_nickname.erase(_nickname.find("#"), 1);
	if (_nickname.size() == 0 || nick_isalnum() == 0)
		_nickname = "Guest";
	if (is_nick_used(server) == true || new_nick == old_nick)
	{
		send_message(": NOTICE " + this->_nickname + " :This nick is owned by someone else. Please choose another.\r\n");
		send_message(": NOTICE " + this->_nickname + " :Your nick will be changed if you like it or not.\r\n");
		_nickname = old_nick;
	}
	else
		send_message(":" + old_nick + " NICK :" + _nickname + "\r\n");	
}

void	User::send_message(std::string msg)
{
	write(this->get_fds()->fd, msg.c_str(), msg.size());
	std::cout << ">> " << msg.substr(0, msg.size() - 2) << std::endl;
}

void	User::ping(Server &server)
{
	time_t  t = time(0);

	if (t - _last_pong > 20 && _pinged == false)
	{
		send_message("PING ft_irc\r\n");
		_pinged = true;
	}
	if (t - _last_pong > 25 && _pinged == true)
	{
		server.disconnect(this, "NULL");
		std::cout << "Client TIMEOUT" << std::endl;
	}
}
