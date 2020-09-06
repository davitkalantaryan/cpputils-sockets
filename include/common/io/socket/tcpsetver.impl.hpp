
// common_servertcp.impl.hpp
// 2017 Aug 02

#ifndef COMMON_IO_SOCKET_TCPSERVER_IMPL_HPP
#define COMMON_IO_SOCKET_TCPSERVER_IMPL_HPP

#ifndef COMMON_IO_SOCKET_TCPSERVER_HPP
//#error do not include this file directly
#include "tcpserver.hpp"
#endif


template <typename Type>
int common::io::socket::TcpServer::StartServer(
	Type* a_owner,
	void(Type::*a_fpAddClient)(common::io::socket::Tcp& clientSock, const sockaddr_in*remoteAddr),
        int a_nPort, bool a_bReuse, bool a_bLoopback, int a_lnTimeout, int* a_pnRetCode)
{
	return StartServerS(
		//(TypeAccept)GetFuncPointer_common(1, a_fpAddClient), 
		*(reinterpret_cast<TypeAccept*>(&a_fpAddClient)),
		(void*)a_owner,
                a_nPort, a_bReuse, a_bLoopback,a_lnTimeout,a_pnRetCode);
}


#endif  // #ifndef __impl_common_servertcp_hpp__
