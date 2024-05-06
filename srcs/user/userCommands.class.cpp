/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   userCommands.class.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljerinec <ljerinec@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 10:22:51 by ljerinec          #+#    #+#             */
/*   Updated: 2024/05/06 10:25:35 by ljerinec         ###   ########.fr       */
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
	if (_nickname.size() != 0 && _username.size() != 0 && _cap_passed == true)
	{
		send_message(":ft_irc 001 " + _nickname + " :Welcome to the 'ft_irc' Network, " + _nickname + "\r\n");
		send_message(":ft_irc 002 " + _nickname + " :Your host is " + server.get_servername() + ", running version 1.0" + "\r\n");
		send_message(":ft_irc 003 " + _nickname + " :This server was created " + server.get_dt() + "\r\n");
		send_message(":ft_irc 004 " + _nickname + " :" + server.get_servername() + " version 1.0\r\n");
		send_message(":ft_irc 005 " + _nickname + " NETWORK=FT_IRC NICKLEN=30 CHANNELLEN=32 CHANTYPES=# PREFIX=(o)@ :are available on this server\r\n");
		send_message(":ft_irc 005 " + _nickname + " CASEMAPPING=ascii CHANMODES=,ko,l,it :are available on this server\n");
		ascii_sam(this);
		change_status(CONNECTED);
	}
	else
	{
		send_message("Cannot register if negotiation is not done: will be disconnected\r\n");
		server.disconnect(this, "NULL");
		throw Error("Cannot register if negotiation is not done.");
	}
}

void    User::change_status(int status)
{
	if (this->_status == 0 && status == 2)
		throw Error("can't skip registration");
	if (this->_status == 2 && status == 1)
		throw Error("user is already registered");
	this->_status = status;
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
		server.disconnect(this, " TIMED OUT");
		std::cout << "Client TIMEOUT" << std::endl;
	}
}
