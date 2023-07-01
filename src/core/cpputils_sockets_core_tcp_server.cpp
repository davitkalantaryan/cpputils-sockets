//
// repo:			cpputils-sockets
// file:            cpputils_sockets_core_tcp_server.cpp
// path:			src/core/cpputils_sockets_core_tcp_server.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cpputils/sockets/tcp_server.hpp>
#include "cpputils_sockets_core_tcp_socket_p.hpp"
#include <string.h>
#include <stdlib.h>

namespace cpputils { namespace sockets{


class CPPUTILS_DLL_PRIVATE tcp_server_p
{
public:
	tcp_server::TypeConnectClbk	clbk;
	socket_t					serv;

public:
	int CreateServer(int a_nPort, bool a_bReuse, bool a_bOnlyLocalHost);
};



/*--------------------------------------------------------------------------------------------------------------*/


tcp_server::~tcp_server()
{
}


tcp_server::tcp_server()
	:
	m_serv_data_p(new tcp_server_p())
{
	m_serv_data_p->clbk = [](tcp_socket* a_pIncommingSocket) {
		delete a_pIncommingSocket;
	};
}


tcp_server::tcp_server(tcp_server&& a_mM) noexcept
	:
	m_serv_data_p(a_mM.m_serv_data_p)
{
	a_mM.m_serv_data_p = new tcp_server_p();
	a_mM.m_serv_data_p->clbk = [](tcp_socket* a_pIncommingSocket) {
		delete a_pIncommingSocket;
	};
}


tcp_server& tcp_server::operator=(tcp_server&& a_mM) noexcept
{
	tcp_server_p* pThisData = m_serv_data_p;
	m_serv_data_p = a_mM.m_serv_data_p;
	a_mM.m_serv_data_p = pThisData;
	return *this;
}


void tcp_server::ReplaceWithOtherServer(tcp_server* CPPUTILS_ARG_NN a_pMM) noexcept
{
	tcp_server_p* pThisData = m_serv_data_p;
	m_serv_data_p = a_pMM->m_serv_data_p;
	a_pMM->m_serv_data_p = pThisData;
}


void tcp_server::StartServer(
	const TypeConnectClbk& a_clbk, int a_nPort,
	bool a_bOnlyLocalHost)
{
#ifndef _WIN32
	struct sigaction newAction;

	m_serverThread = pthread_self();

	newAction.sa_flags = SA_SIGINFO;
	sigemptyset(&newAction.sa_mask);
	newAction.sa_restorer = NULL;
	newAction.sa_sigaction = SigActionFunction;

	sigaction(SIGPIPE, &newAction, NULL);
#endif
}


/*--------------------------------------------------------------------------------------------------------------*/

#define MAX_HOSTNAME_LENGTH_MIN_1		127
static constexpr size_t		MAX_HOSTNAME_LENGTH = MAX_HOSTNAME_LENGTH_MIN_1 + 1;

int tcp_server_p::CreateServer(int a_nPort, bool a_bReuse, bool a_bOnlyLocalHost)
{
	
	char l_host[MAX_HOSTNAME_LENGTH];

	this->serv = ::socket(AF_INET, SOCK_STREAM, 0);
	if (CHECK_FOR_SOCK_INVALID(this->serv)) {
		return(-1);  // no socket
	}

	if (a_bReuse) { int i(1); setsockopt(this->serv, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i)); }

	if (gethostname(l_host, MAX_HOSTNAME_LENGTH_MIN_1) < 0) {
		return -1;  // this is not possible (hope so)
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = a_bOnlyLocalHost ? AF_UNIX:AF_INET;
	addr.sin_port = htons(a_nPort);
	addr.sin_addr.s_addr = htonl((a_bOnlyLocalHost ? INADDR_LOOPBACK : INADDR_ANY));

	MakeSocketNonBlockingInline(this->serv);

	const socklen_t addr_len = sizeof(addr);
	int rtn = bind(this->serv, (struct sockaddr*)&addr, addr_len);
	if (CHECK_FOR_SOCK_ERROR(rtn)) {
		return(-1);  // bind error
	}

	rtn = ::listen(this->serv, 64);
	if (CHECK_FOR_SOCK_ERROR(rtn)) {
		return (-1); // listen error
	}

	return 0;
}


}}  //  namespace cpputils { namespace sockets{
