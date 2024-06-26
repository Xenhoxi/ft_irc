/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljerinec <ljerinec@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 17:10:01 by smunio            #+#    #+#             */
/*   Updated: 2024/05/14 12:49:27 by ljerinec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

#include "libs.hpp"

class Server;

class Channel;

class User
{
public:
	User();
	~User();
	int					get_status() const;
	std::string 		get_nick();
	struct pollfd   	*get_fds() const;
	void				add_data(std::string new_data);
	void            	set_fds(int server_socket);
	std::string			get_host_info() const;
	void				add_channel(Channel *channel);
	void				set_addr(char *addr);
	
	void			parsing(Server &server);
	void			parse_negotiation(std::string line, Server &server);
	void			parse_command(std::string line, Server &server);
	void			be_my_guest(Server &server);

	void			ping(Server &server);
	void			negotiation(Server &server);
	void			registration(Server &server);
	void			send_message(std::string msg);
	void			change_status(int status);
	void			remove_channel(Channel *channel);

private:
	void			change_nick(std::string new_nick, Server &server);
	int				nick_isalnum(void);
	bool			is_nick_used(Server &server);
	void			check_nick_validity(Server &server);

	std::string				_data;
	std::string     		_nickname;
	std::string				_username;
	std::string				_password;
	std::list<Channel *>	_channels;
	time_t					_last_pong;
	int						_status;
	bool					_pinged;
	bool					_cap_passed;
	std::string				_client_addr;
	struct pollfd			*_fds;
};


#endif