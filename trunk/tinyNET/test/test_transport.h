/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou@yahoo.fr>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*	
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/
#ifndef TNET_TEST_TRANSPORT_H
#define TNET_TEST_TRANSPORT_H
#define REMOTE_IPV4 "192.168.0.15"
#define SIP_MESSAGE \
	"REGISTER sip:micromethod.com SIP/2.0\r\n" \
	"Via: SIP/2.0/%s %s:%d;rport;branch=z9hG4bK1245420841406%d\r\n" \
	"From: <sip:mamadou@micromethod.com>;tag=29358\r\n" \
	"To: <sip:mamadou@micromethod.com>\r\n" \
	"Call-ID: M-fa53180346f7f55ceb8d8670f9223dbb\r\n" \
	"CSeq: 201 REGISTER\r\n" \
	"Max-Forwards: 70\r\n" \
	"Contact: <sip:mamadou@%s:%d;transport=%s>\r\n" \
	"Expires: 20\r\n" \
	"\r\n"


static int tnet_tcp_data_read(const void* data, size_t size)
{
	/* Do quick job. */
	TSK_DEBUG_INFO("--- TCP ---\n%s\n", data);
	return 0;
}

static int tnet_udp_data_read(const void* data, size_t size)
{
	/* Do quick job. */
	TSK_DEBUG_INFO("--- UDP ---\n%s\n", data);
	return 0;
}

void test_transport_tcp_ipv4(tnet_transport_handle_t *transport)
{
	tnet_socket_type_t type = tnet_socket_type_tcp_ipv4;
	tnet_ip_t ip;
	tnet_port_t port;
	tnet_fd_t fd = INVALID_SOCKET;

	if(tnet_transport_start(transport))
	{
		TSK_DEBUG_ERROR("Failed to create TCP/IPv4 transport.");
		return;
	}

	while(!tnet_transport_isready(transport))
	{
		tsk_thread_sleep(500);
	}

	//tsk_thread_sleep(500);

	/* Connect to the SIP Registrar */
	if((fd = tnet_transport_connectto(transport, REMOTE_IPV4, 5060)) == INVALID_SOCKET)
	{
		TSK_DEBUG_ERROR("Failed to connect TCP/IPv4 transport.");
		return;
	}

	/* Set our callback function */
	tnet_transport_set_callback(transport, tnet_tcp_data_read);

	//while(!tnet_transport_get_isconnected(transport))
	{
		/* Connecto succeed but not connected yet.*/
		tsk_thread_sleep(500);
	}

	/* Send our SIP message */
	{
		char* message = 0;
		tnet_transport_get_ip_n_port(transport, fd, &ip, &port);
		tsk_sprintf(&message, SIP_MESSAGE, "TCP", ip, port, port, ip, port, "tcp");

		if(!tnet_transport_send(transport, fd, message, strlen(message)))
		{
			TSK_DEBUG_ERROR("Failed to send data using TCP/IPv4 transport.");
			TSK_FREE(message);
			return;
		}
		TSK_FREE(message);
	}
}


int test_transport_udp_ipv4(tnet_transport_handle_t *transport)
{
	tnet_socket_type_t type = tnet_socket_type_udp_ipv4;
	tnet_ip_t ip;
	tnet_port_t port;
	tnet_fd_t fd = INVALID_SOCKET;
	
	if(tnet_transport_start(transport))
	{
		TSK_DEBUG_ERROR("Failed to create UDP/IPv4 transport.");
		return -1;
	}

	//tsk_thread_sleep(2000);

	/* Connect to our SIP REGISTRAR */
	if((fd = tnet_transport_connectto(transport, REMOTE_IPV4, 5060)) == INVALID_SOCKET)
	{
		TSK_DEBUG_ERROR("Failed to connect UDP/IPv4 transport.");
		//tnet_transport_shutdown(transport);
		return -2;
	}

	//tsk_thread_sleep(2000);

	/* Set our callback function */
	tnet_transport_set_callback(transport, tnet_udp_data_read);

	/* Send our SIP message */
	/*while(1)*/{
		char* message = 0;
		tnet_transport_get_ip_n_port(transport, fd, &ip, &port);
		tsk_sprintf(&message, SIP_MESSAGE, "UDP", ip, port, port, ip, port, "udp");

		if(!tnet_transport_send(transport, fd, message, strlen(message)))
		{
			TSK_DEBUG_ERROR("Failed to send data using UDP/IPv4 transport.");
			//tnet_transport_shutdown(transport);
			TSK_FREE(message);
			return -3;
		}
		TSK_FREE(message);
	}

	return 0;
}

void test_transport()
{
#define TEST_TCP 1
#define TEST_UDP 0

#if TEST_UDP
	tnet_transport_handle_t *udp = TNET_TRANSPORT_CREATE(TNET_SOCKET_HOST_ANY, TNET_SOCKET_PORT_ANY, tnet_socket_type_udp_ipv4, "UDP/IPV4 TRANSPORT");
	test_transport_udp_ipv4(udp);
#endif

#if TEST_TCP
	tnet_transport_handle_t *tcp = TNET_TRANSPORT_CREATE(TNET_SOCKET_HOST_ANY, TNET_SOCKET_PORT_ANY, tnet_socket_type_tcp_ipv4, "TCP/IPV4 TRANSPORT");
	test_transport_tcp_ipv4(tcp);
#endif	

	tsk_thread_sleep(2000);

#if TEST_UDP
	TNET_TRANSPORT_SAFE_FREE(udp);
#endif

#if TEST_TCP
	tnet_transport_shutdown(tcp);
#endif
}


#endif /* TNET_TEST_TRANSPORT_H*/