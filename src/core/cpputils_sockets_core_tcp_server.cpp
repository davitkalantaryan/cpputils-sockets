//
// repo:			cpputils-sockets
// file:            cpputils_sockets_core_tcp_server.cpp
// path:			src/core/cpputils_sockets_core_tcp_server.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cpputils/sockets/tcp_server.hpp>
#include "cpputils_sockets_core_tcp_socket_p.hpp"
#define cinternal_unnamed_sema_wait_ms_needed
#include <cinternal/unnamed_semaphore.h>
#include <cinternal/bistateflags.h>
#include <cinternal/disable_compiler_warnings.h>
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <cinternal/undisable_compiler_warnings.h>

namespace cpputils { namespace sockets{


#ifdef _MSC_VER
#pragma warning (disable:5039)
#pragma warning (disable:4820)
#endif


class CPPUTILS_DLL_PRIVATE tcp_server_p
{
public:
	tcp_server::TypeConnectClbk	clbk;
	tcp_server::TypeExtraCleanClbk ecClbk;
	socket_t					serv;
	::std::thread				server_thread;
	cinternal_unnamed_sema_t	sema_for_start;
	CPPUTILS_BISTATE_FLAGS_UN(threadStarted,shouldRun,runs, hasError)	flags;

public:
	tcp_server_p();
	tcp_server_p(const tcp_server_p&) = delete;
	tcp_server_p(tcp_server_p&&) = delete;
	tcp_server_p& operator=(const tcp_server_p&) = delete;
	tcp_server_p& operator=(tcp_server_p&&) = delete;
	void ServerThreadFunction(int a_nPort, int a_lnTimeoutMs,bool a_bOnlyLocalHost, bool a_bReuse);
	int  CreateServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse);
	void RunServer(int a_lnTimeoutMs);
	void ServerAccept(int a_lnTimeoutMs, struct sockaddr_in* a_bufForRemAddress);
};



/*--------------------------------------------------------------------------------------------------------------*/


const tcp_server::TypeConnectClbk	tcp_server::s_defClbk = [](tcp_socket& a_incommingSocket, const sockaddr_in*) {
	a_incommingSocket.Close();
};


tcp_server::~tcp_server()
{
}


tcp_server::tcp_server()
	:
	m_serv_data_p(new tcp_server_p())
{
}


tcp_server::tcp_server(tcp_server&& a_mM) noexcept
	:
	m_serv_data_p(a_mM.m_serv_data_p)
{
	a_mM.m_serv_data_p = new tcp_server_p();
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


int tcp_server::StartServer(
	int a_nPort, const TypeConnectClbk& a_clbk, int a_lnTimeoutMs,
	bool a_bOnlyLocalHost, bool a_bReuse, const TypeExtraCleanClbk& a_ecclb)
{
	m_serv_data_p->clbk = a_clbk;
	m_serv_data_p->ecClbk = a_ecclb ? (a_ecclb) : ([]() {});

	StoptServer();

    if (cinternal_unnamed_sema_create(&(m_serv_data_p->sema_for_start), 0)) {
        // log on semaphore creation failure
        return -1;
    }
	m_serv_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;

	m_serv_data_p->server_thread = ::std::thread([this, a_nPort, a_lnTimeoutMs,a_bOnlyLocalHost, a_bReuse]() {
		m_serv_data_p->ServerThreadFunction(a_nPort, a_lnTimeoutMs,a_bOnlyLocalHost, a_bReuse);
	});

	m_serv_data_p->flags.wr.threadStarted = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
	//m_serv_data_p->sema.Wait();
	cinternal_unnamed_sema_wait(&(m_serv_data_p->sema_for_start));
    cinternal_unnamed_sema_destroy(&(m_serv_data_p->sema_for_start));

	return m_serv_data_p->flags.rd.runs_true ? 0 : (-1);
}


void tcp_server::ChangeAcceptCallback(const TypeConnectClbk& a_clbk)
{
    m_serv_data_p->clbk = a_clbk;
}


void tcp_server::StoptServer()
{
	if (m_serv_data_p->flags.rd.shouldRun_false) {return;}

	m_serv_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
	if (m_serv_data_p->flags.rd.threadStarted_false) { return; }

	if (m_serv_data_p->server_thread.get_id() == ::std::this_thread::get_id()) { 
        m_serv_data_p->server_thread.detach();
        return; 
    }

#ifdef _WIN32
	QueueUserAPC([](_In_ ULONG_PTR) {}, m_serv_data_p->server_thread.native_handle(), 0);
#else
	pthread_kill(m_serv_data_p->server_thread.native_handle(), SIGPIPE);
#endif
    
    m_serv_data_p->server_thread.join();
}


/*--------------------------------------------------------------------------------------------------------------*/


tcp_server_p::tcp_server_p()
{
	this->flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
	this->clbk = tcp_server::s_defClbk;
	this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
}


#ifndef _WIN32
static void SigHandlerFunction(int a_signo){
    CPPUTILS_STATIC_CAST(void,a_signo);
}
#endif


void tcp_server_p::ServerThreadFunction(int a_nPort, int a_lnTimeoutMs, bool a_bOnlyLocalHost, bool a_bReuse)
{

#ifndef _WIN32
	struct sigaction newAction;
    memset(&newAction,0,sizeof(struct sigaction));
	//newAction.sa_flags = 0; // because of memset
	sigemptyset(&newAction.sa_mask);
	//newAction.sa_restorer = nullptr;  // because of memset
	newAction.sa_handler = &SigHandlerFunction;
	sigaction(SIGPIPE, &newAction, nullptr);
#endif

	if (CreateServer(a_nPort, a_bOnlyLocalHost, a_bReuse)) {
		this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
		this->flags.wr.runs = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
		//this->sema.Post();
		cinternal_unnamed_sema_post(&(this->sema_for_start));
		return;
	}

	this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
	this->flags.wr.runs = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
	//this->sema.Post();
	cinternal_unnamed_sema_post(&(this->sema_for_start));

	RunServer(a_lnTimeoutMs);

	closesocketn(this->serv);
	this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
	this->ecClbk();

}


#define MAX_HOSTNAME_LENGTH_MIN_1		127
static constexpr size_t		MAX_HOSTNAME_LENGTH = MAX_HOSTNAME_LENGTH_MIN_1 + 1;

int tcp_server_p::CreateServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse)
{
	char l_host[MAX_HOSTNAME_LENGTH];

	this->serv = ::socket(AF_INET, SOCK_STREAM, 0);
	if (CHECK_FOR_SOCK_INVALID(this->serv)) {
		return(-1);  // no socket
	}

	if (a_bReuse) { int i(1); setsockopt(this->serv, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i)); }

	if (gethostname(l_host, MAX_HOSTNAME_LENGTH_MIN_1) < 0) {
		closesocketn(this->serv);
		this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
		return -1;  // this is not possible (hope so)
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	//addr.sin_family = (a_bOnlyLocalHost ? ((unsigned short)AF_UNIX):((unsigned short)AF_INET));
	addr.sin_family = (unsigned short)AF_INET;
	addr.sin_port = htons((u_short)a_nPort);
	addr.sin_addr.s_addr = htonl((a_bOnlyLocalHost ? INADDR_LOOPBACK : INADDR_ANY));

	MakeSocketNonBlockingInline(this->serv);

	const socklen_t addr_len = sizeof(addr);
	int rtn = bind(this->serv, (struct sockaddr*)&addr, addr_len);
	if (CHECK_FOR_SOCK_ERROR(rtn)) {
		closesocketn(this->serv);
		this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
		return(-1);  // bind error
	}

	rtn = ::listen(this->serv, 64);
	if (CHECK_FOR_SOCK_ERROR(rtn)) {
		closesocketn(this->serv);
		this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
		return (-1); // listen error
	}

	return 0;
}


void tcp_server_p::RunServer(int a_lnTimeout)
{
	sockaddr_in remoteAddress;

	while (this->flags.rd.shouldRun_true && this->flags.rd.hasError_false) {
		ServerAccept(a_lnTimeout, &remoteAddress);
	}
}


void tcp_server_p::ServerAccept(int a_lnTimeoutMs, struct sockaddr_in* a_bufForRemAddress)
{
	int rtn = WaitForDataOnSocketInline(this->serv, a_lnTimeoutMs, DeskType::read);

	switch (rtn) {
	case -1:
		this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
		return;
	case 0:  // timeout
		return;
	default:
		break;
	}  //  switch (rtn) {

	cpputils_socklen_t addr_len = sizeof(struct sockaddr_in);
	const struct SysSocket clientSocket = { accept(this->serv, (struct sockaddr*)a_bufForRemAddress, &addr_len) };

	if (CHECK_FOR_SOCK_INVALID(clientSocket.sock)){
		return;
	}

	tcp_socket aIncommingSocket(&clientSocket);
	aIncommingSocket.MakeSocketBlocking();
	this->clbk(aIncommingSocket, a_bufForRemAddress);
	aIncommingSocket.Close();
}


}}  //  namespace cpputils { namespace sockets{
