#pragma once
#ifndef JC_NETWORKING_SOCK_LIB_H
#define JC_NETWORKING_SOCK_LIB_H

/*
	Exposes raw os socket library headers
*/


#include "JCNetworkingConfig.h"


#ifdef JCN_WINSOCK

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#undef min
#undef max

namespace JCN_NAMESPACE::slib
{
	using socket_t = ::SOCKET;

	using ::addrinfo;
	using ::fd_set;
	using ::timeval;

	using ::accept;
	using ::bind;
	using ::closesocket;
	using ::connect;
	using ::freeaddrinfo;
	using ::getaddrinfo;
	using ::gethostname;
	using ::listen;
	using ::recv;
	using ::recvfrom;
	using ::select;
	using ::send;
	using ::sendto;
	using ::shutdown;
	using ::socket;
	using ::ioctlsocket;

};

#else
#error "Missing socket library inclusions for target OS"
#endif







#endif