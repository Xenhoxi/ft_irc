/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smunio <smunio@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 14:49:47 by smunio            #+#    #+#             */
/*   Updated: 2024/04/30 18:51:27 by smunio           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libs.hpp"

Server::Server()
{
    time_t  t               = time(0);
    this->_datetime         = ctime(&t);
	this->_server_name		= "Vitesse a la team de puants";
    return ;
}

Server::~Server()
{
	std::map<std::string, Channel *>::iterator	it;
	for (it = _channel_list.begin(); it != _channel_list.end(); it++)
	{
		delete (*it).second;
	}
	_channel_list.clear();
    return ;
}

void	Server::socket_init(int port, std::string pass)
{
	int 	fd_socket;
    User    *server_socket  = new User(); // leak

	this->_password = pass;
	server_socket->change_status(MAIN_SOCKET);
	fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Socket init: " << fd_socket << std::endl;
	if (fd_socket < 0) 
		std::cerr << "Socket exception !" << std::endl;
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_port = htons(port);
	this->_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd_socket, (struct sockaddr *) &this->_addr, sizeof(this->_addr)) < 0)
		throw Error("Bind info: Address already in use");
	else
		std::cout << "Server ON !" << std::endl;
	listen(fd_socket, 5);
    this->_usr_list.push_back(server_socket);
	this->_usr_list.front()->set_fds(fd_socket);
}

std::string Server::find_ch_name(std::string line)
{
	char *tok = strtok((char *)line.c_str(), " ");
	while (tok != NULL)
	{
		if (tok[0] == '#')
			return (static_cast<std::string>(tok));
		tok = strtok(NULL, " ");
	}
	throw Error("can't find channel");
}
