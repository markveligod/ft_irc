/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leweathe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/19 13:08:21 by leweathe          #+#    #+#             */
/*   Updated: 2020/12/19 13:08:22 by leweathe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

int main()
{
	Client client = Client();

	client.create_socket();
	client.connection();
	client.chat();
	client.close_connection();
	
	return (0);
}
