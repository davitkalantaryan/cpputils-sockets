//
// repo:			cpputils-sockets
// file:            cpputils_sockets_core_tcp_server.cpp
// path:			src/core/cpputils_sockets_core_tcp_server.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cpputils/sockets/internal_header.h>

#ifndef cinternal_gettid_needed
#define cinternal_gettid_needed
#endif

#ifndef cinternal_unnamed_sema_wait_ms_needed
#define cinternal_unnamed_sema_wait_ms_needed
#endif

#include <cpputils/sockets/tcp_server.hpp>
#include "cpputils_sockets_core_tcp_socket_p.hpp"
#include <cinternal/unnamed_semaphore.h>
#include <cinternal/bistateflags.h>
#include <cinternal/signals.h>
#include <cinternal/gettid.h>
#include <cinternal/disable_compiler_warnings.h>
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <cinternal/undisable_compiler_warnings.h>

namespace cpputils { namespace sockets{


#define CPPUTILS_SOCKS_INTERNAL_CHK_STR         "__internalSocket"
#define CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN     sizeof(CPPUTILS_SOCKS_INTERNAL_CHK_STR)

#define CPPUTILS_SOCKS_INTERNAL_STP_SRV         "__stopServer"
#define CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN     sizeof(CPPUTILS_SOCKS_INTERNAL_STP_SRV)


#ifdef _MSC_VER
#pragma warning (disable:5039)
#pragma warning (disable:4820)
#endif
typedef void (*SignalHandlerPointer)(int);


static void SigHandlerFunction(int a_signo) noexcept {CPPUTILS_STATIC_CAST(void, a_signo);}


class CPPUTILS_DLL_PRIVATE tcp_data_base_server_p
{
public:
	tcp_server_base::TypeConnectClbk    clbk;
	socket_t					        serv;
    struct StopperData                  stpData;
    sockaddr_in					        servAddr;
    int64_t                             serverTid;
	CPPUTILS_BISTATE_FLAGS_UN(
        shouldRun,
        tryingToCreate,
        serverRunning,
        isCreated,
        hasError
    )	flags;

public:
    tcp_data_base_server_p();
    int  CreateServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse);
    int  CreateServerRaw(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse) noexcept;
	void RunServer();
    int64_t StopServer() noexcept;
    int getPortNumber() const noexcept;
    int GetStopperData(StopperData* CPPUTILS_ARG_NN a_pStpData, size_t a_count);
private:
    inline void ServerAcceptInline(struct sockaddr_in* a_bufForRemAddress);
    inline void RunServerInline();
private:
    tcp_data_base_server_p(const tcp_data_base_server_p&) = delete;
    tcp_data_base_server_p(tcp_data_base_server_p&&) = delete;
    tcp_data_base_server_p& operator=(const tcp_data_base_server_p&) = delete;
    tcp_data_base_server_p& operator=(tcp_data_base_server_p&&) = delete;
};


class CPPUTILS_DLL_PRIVATE tcp_server_p : public tcp_data_base_server_p
{
public:
    tcp_server::TypeExtraCleanClbk  ecClbk;
    ::std::thread				    server_thread;
public:
    tcp_server_p();
private:
    tcp_server_p(const tcp_server_p&) = delete;
    tcp_server_p(tcp_server_p&&) = delete;
    tcp_server_p& operator=(const tcp_server_p&) = delete;
    tcp_server_p& operator=(tcp_server_p&&) = delete;
};


/*--------------------------------------------------------------------------------------------------------------*/

tcp_server_base::~tcp_server_base()
{
    delete m_serv_base_data_p;
}


tcp_server_base::tcp_server_base()
	:
	m_serv_base_data_p(new tcp_data_base_server_p())
{
}


tcp_server_base::tcp_server_base(tcp_server_base&& a_mM) noexcept
	:
	m_serv_base_data_p(a_mM.m_serv_base_data_p)
{
	a_mM.m_serv_base_data_p = nullptr;
}


tcp_server_base& tcp_server_base::operator=(tcp_server_base&& a_mM) noexcept
{
    tcp_data_base_server_p* const pThisData = m_serv_base_data_p;
	m_serv_base_data_p = a_mM.m_serv_base_data_p;
	a_mM.m_serv_base_data_p = pThisData;
	return *this;
}


void tcp_server_base::ChangeAcceptCallback(const TypeConnectClbk& a_clbk)
{
    m_serv_base_data_p->clbk = a_clbk;
}


const sockaddr_in* tcp_server_base::getSockAddr() const noexcept
{
    return &(m_serv_base_data_p->servAddr);
}


int tcp_server_base::getPortNumber() const noexcept
{
    return m_serv_base_data_p->getPortNumber();
}


int tcp_server_base::GetStopperData(StopperData* CPPUTILS_ARG_NN a_pStpData, size_t a_count)
{
    return m_serv_base_data_p->GetStopperData(a_pStpData, a_count);
}


/*--------------------------------------------------------------------------------------------------------------*/

tcp_server::~tcp_server()
{
    delete m_serv_async_data_p;
}


tcp_server::tcp_server()
    :
    m_serv_async_data_p(new tcp_server_p())
{
}


tcp_server::tcp_server(tcp_server&& a_mM) noexcept
	:
    m_serv_async_data_p(a_mM.m_serv_async_data_p)
{
    m_serv_async_data_p = a_mM.m_serv_async_data_p;
	a_mM.m_serv_base_data_p = nullptr;
    a_mM.m_serv_async_data_p = nullptr;
}


tcp_server& tcp_server::operator=(tcp_server&& a_mM) noexcept
{
    tcp_data_base_server_p* const pThisDataBase = m_serv_base_data_p;
    m_serv_base_data_p = a_mM.m_serv_base_data_p;
    a_mM.m_serv_base_data_p = pThisDataBase;
    tcp_server_p* const pThisData = m_serv_async_data_p;
    m_serv_async_data_p = a_mM.m_serv_async_data_p;
    a_mM.m_serv_async_data_p = pThisData;
    return *this;
}


void tcp_server::StoptServer()
{
    if (m_serv_base_data_p->flags.rd.shouldRun_false) {
        return;
    }
    
    if ((m_serv_base_data_p->serverTid) != m_serv_base_data_p->StopServer()) {
        m_serv_async_data_p->server_thread.join();
    }
    else {
        m_serv_async_data_p->server_thread.detach();
    }
}


int tcp_server::StartAsyncServerOnOtherThreadAndReturn(
    int a_nPort, const TypeConnectClbk& a_clbk,
	bool a_bOnlyLocalHost, bool a_bReuse, 
    const TypeExtraCleanClbk& a_ecclb)
{
    if (m_serv_base_data_p->flags.rd.shouldRun_true) {
        return 1;  // server already created
    }

    cinternal_unnamed_sema_t sema_for_start;
    if (cinternal_unnamed_sema_create(&sema_for_start, 0)) {
        // log on semaphore creation failure
        return -1;
    }

    m_serv_base_data_p->clbk = a_clbk;
    m_serv_async_data_p->ecClbk = a_ecclb ? (a_ecclb) : ([]()->void {});

    m_serv_async_data_p->server_thread = ::std::thread([this, a_nPort,a_bOnlyLocalHost, a_bReuse, &sema_for_start]() {
        if (m_serv_base_data_p->CreateServer(a_nPort, a_bOnlyLocalHost, a_bReuse)) {
            m_serv_base_data_p->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
            cinternal_unnamed_sema_post(&sema_for_start);
            return;
        }

        m_serv_base_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        cinternal_unnamed_sema_post(&sema_for_start);
        m_serv_base_data_p->RunServer();
        m_serv_async_data_p->ecClbk();
	});

	cinternal_unnamed_sema_wait(&sema_for_start);
    cinternal_unnamed_sema_destroy(&sema_for_start);

	return m_serv_base_data_p->flags.rd.hasError_false ? 0 : (-1);
}


/*--------------------------------------------------------------------------------------------------------------*/

// stops and waits to stop
// can be stopped from accept callback, or from any thread
void blocking_tcp_server::StoptServer()
{
    if (m_serv_base_data_p->flags.rd.shouldRun_false) {
        return;
    }
    m_serv_base_data_p->StopServer();
}


// creates server socket, but does not start server
// returns port number, or -1 on error
// also one can ask getSockAddr
int blocking_tcp_server::CreateBlockingServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse)
{
    const int rtn = m_serv_base_data_p->CreateServer(a_nPort, a_bOnlyLocalHost, a_bReuse);
    if (rtn < 0) {
        return rtn;
    }
    return m_serv_base_data_p->getPortNumber();
}


// server will run in the same thread, 
// one can stop server by using signal, or from accept callback
void blocking_tcp_server::RunBlockingServer(const TypeConnectClbk& a_clbk)
{
    m_serv_base_data_p->clbk = a_clbk;
    m_serv_base_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    m_serv_base_data_p->RunServer();
}


/*--------------------------------------------------------------------------------------------------------------*/

tcp_data_base_server_p::tcp_data_base_server_p()
{
	this->flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    this->clbk = [](tcp_socket&, const sockaddr_in*) {};
	this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
    this->stpData = { {CPPUTILS_SOCKS_CLOSE_SOCK}, {CPPUTILS_SOCKS_CLOSE_SOCK} };
    this->servAddr = {};
    this->serverTid = 0;
}


inline void tcp_data_base_server_p::ServerAcceptInline(struct sockaddr_in* a_bufForRemAddress)
{
    struct pollfd vPollFd[4];
    cpputils_poll_arg2 nPollFdCount = 1;
    vPollFd[0].fd = this->serv;
    vPollFd[0].events = POLLIN | POLLRDNORM | POLLRDBAND;
    vPollFd[0].revents = 0;

    if ((this->stpData.pol.sock) != CPPUTILS_SOCKS_CLOSE_SOCK) {
        nPollFdCount = 2;
        vPollFd[1].fd = this->stpData.pol.sock;
        vPollFd[1].events = POLLIN | POLLRDNORM | POLLRDBAND;
        vPollFd[1].revents = 0;
    }

    const int pollRes = CpputilsPoll(vPollFd, nPollFdCount, -1);
    if (this->flags.rd.shouldRun_false) {
        return;
    }  //  if (this->flags.rd.shouldRun_true) {
    
    if (pollRes > 0) {
        if (vPollFd[0].revents & POLLIN) {
            cpputils_socklen_t addr_len = sizeof(struct sockaddr_in);
            const socket_t clntSockDescrpt = accept(this->serv, (struct sockaddr*)a_bufForRemAddress, &addr_len);
            if (!CHECK_FOR_SOCK_INVALID(clntSockDescrpt)) {
                const struct SysSocket clientSocket = { clntSockDescrpt };
                tcp_socket aIncommingSocket(&clientSocket);
                aIncommingSocket.MakeSocketBlocking();
                this->clbk(aIncommingSocket, a_bufForRemAddress);
                aIncommingSocket.Close();
            }  //  if (!CHECK_FOR_SOCK_INVALID(clntSockDescrpt)) {
        }  //  if (vPollFd[0].revents & POLLIN) {
    }  //  if (pollRes > 0) {	
}


void tcp_data_base_server_p::RunServerInline()
{
    this->serverTid = CinternalGetCurrentTid();
    sockaddr_in remoteAddress;
    this->flags.wr.serverRunning = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    while (this->flags.rd.shouldRun_true && this->flags.rd.hasError_false) {
        ServerAcceptInline(&remoteAddress);
    }
    this->flags.wr.serverRunning = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    this->serverTid = 0;
}


int tcp_data_base_server_p::CreateServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse)
{
    int rtn = this->CreateServerRaw(a_nPort, a_bOnlyLocalHost, a_bReuse);
    if (rtn) {
        return rtn;
    }

    rtn = this->GetStopperData(&(this->stpData), 1);
    if (rtn) {
        CpputilsCloseSocket(this->serv);
        this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
        this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        this->flags.wr.isCreated = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
        return rtn;
    }

    return 0;
}


int tcp_data_base_server_p::CreateServerRaw(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse) noexcept
{
    this->serv = ::socket(AF_INET, SOCK_STREAM, 0);
    if (CHECK_FOR_SOCK_INVALID(this->serv)) {
        this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
        this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        return(-1);  // no socket
    }
    if (a_bReuse) { int i(1); setsockopt(this->serv, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i)); }

    //char vcHNameBfr[MAX_HOSTNAME_LENGTH];
    //if (gethostname(vcHNameBfr, MAX_HOSTNAME_LENGTH_MIN_1) < 0) {
    //    CpputilsCloseSocket(this->serv);
    //    this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
    //    return -1;  // this is not possible (hope so)
    //}

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
		CpputilsCloseSocket(this->serv);
		this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
        this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
		return(-1);  // bind error
	}

	rtn = ::listen(this->serv, 64);
	if (CHECK_FOR_SOCK_ERROR(rtn)) {
		CpputilsCloseSocket(this->serv);
		this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
        this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
		return (-1); // listen error
	}

    cpputils_socklen_t sock_name_addr_len = static_cast<cpputils_socklen_t>(sizeof(struct sockaddr_in));
    rtn = ::getsockname(this->serv, (struct sockaddr*)&(this->servAddr), &sock_name_addr_len);
    if (CHECK_FOR_SOCK_ERROR(rtn)) {
        CpputilsCloseSocket(this->serv);
        this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
        this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        return (-1); // listen error
    }

    this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    this->flags.wr.isCreated = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
	return 0;
}


int tcp_data_base_server_p::GetStopperData(StopperData* CPPUTILS_ARG_NN a_pStpData, size_t a_count)
{
    if (this->flags.rd.isCreated_false) {
        return -1;
    }

    cinternal_unnamed_sema_t sema_for_to_finish = (cinternal_unnamed_sema_t)0;
    if (this->flags.rd.serverRunning_true) {
        const int64_t currentThreadTid = CinternalGetCurrentTid();
        if (currentThreadTid != (this->serverTid)) {
            if (cinternal_unnamed_sema_create(&sema_for_to_finish, 0)) {
                // log on semaphore creation failure
                return -1;
            }
        }
    }  //  if (this->flags.rd.serverRunning_false) {

    const int cnPort = ntohs(this->servAddr.sin_port);
    const uint64_t inShouldRun = this->flags.wr.shouldRun;
    this->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    ::std::thread* const pTmpThread = new ::std::thread([a_pStpData, a_count,cnPort,&sema_for_to_finish]() {
        tcp_socket pollSocket;
        int rtn = -1;
        for (size_t ind(0); ind < a_count; ) {
            rtn = pollSocket.Connect("localhost", cnPort, -1);
            if (rtn) {
                pollSocket.Close();
                CinternalSleepInterruptableMs(10);
                continue;
            }
            rtn = pollSocket.SendSimple(CPPUTILS_SOCKS_INTERNAL_CHK_STR, CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN);
            if (rtn != CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN) {
                pollSocket.Close();
                CinternalSleepInterruptableMs(10);
                continue;
            }
            SysSocket aSysSock;
            pollSocket.GetSysSocketAndReset(&aSysSock);
            a_pStpData[ind++].stp.sock = aSysSock.sock;
        }  //  while (rtn) {
        if (sema_for_to_finish) {
            cinternal_unnamed_sema_post(&sema_for_to_finish);
        }
    });  //  ::std::thread tmpThread([&stpSocket,cnPort]() {
    
    const tcp_server_base::TypeConnectClbk* const aClbkIn_p = new tcp_server_base::TypeConnectClbk(this->clbk);
    typedef ::std::function<bool(tcp_socket&)>	TypeConnectExtraClbk;
    size_t ind(0);
    const TypeConnectExtraClbk* const clbkExtra_p = new TypeConnectExtraClbk([this,a_pStpData,a_count, aClbkIn_p, inShouldRun, pTmpThread,&ind](tcp_socket& a_sock) ->bool{
        a_sock.MakeSocketBlocking();
        a_sock.SetTimeout(1000);
        char vcBuffer[CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN+10];
        const int rdRet = a_sock.receiveSngl(vcBuffer, CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN);
        if (rdRet == CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN) {
            if (memcmp(vcBuffer, CPPUTILS_SOCKS_INTERNAL_CHK_STR, CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN) == 0) {
                SysSocket aSysSock;
                a_sock.GetSysSocketAndReset(&aSysSock);
                a_pStpData[ind++].pol.sock = aSysSock.sock;
                if (ind >= a_count) {
                    this->clbk = *aClbkIn_p;
                    pTmpThread->join();
                    delete pTmpThread;
                    this->flags.wr.shouldRun = inShouldRun;
                }  //  if (ind >= a_count) {
                return true;
            }
        }  //  if (rdRet == CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN) {
        return false;
    });  //  this->clbk = [](tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN) {

    this->clbk = [clbkExtra_p, aClbkIn_p](tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_pAddr)->void{
        if ((*clbkExtra_p)(a_sock)) {
            return;
        }
        (*aClbkIn_p)(a_sock,a_pAddr);
    };

    if (this->flags.rd.serverRunning_false) {
        RunServerInline();
        delete clbkExtra_p;
        delete aClbkIn_p;
        return 0;
    }  //  if (this->flags.rd.serverRunning_false) {

    if (sema_for_to_finish) {
        cinternal_unnamed_sema_wait(&sema_for_to_finish);
        cinternal_unnamed_sema_destroy(&sema_for_to_finish);
    }

    delete clbkExtra_p;
    delete aClbkIn_p;

    return 0;
}


void tcp_data_base_server_p::RunServer()
{

#ifdef _WIN32
    SignalHandlerPointer                initialSigintPointer;
    initialSigintPointer = signal(SIGINT, &SigHandlerFunction);
#else
    struct sigaction                    initialSigpipeAction;
    struct sigaction newAction;
    memset(&newAction, 0, sizeof(struct sigaction));
    sigemptyset(&newAction.sa_mask);
    new_action.sa_flags = 0;
    newAction.sa_handler = &SigHandlerFunction;
    sigaction(SIGPIPE, &newAction, &initialSigpipeAction);
#endif

    if (!this->clbk) {
        this->clbk = [](tcp_socket&, const sockaddr_in*) ->void {};
    }

    RunServerInline();

    this->servAddr = {};
    this->serverTid = 0;
    CpputilsCloseSocket(this->serv);
    this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
    CpputilsCloseSocket(this->stpData.pol.sock);
    this->stpData.pol.sock = CPPUTILS_SOCKS_CLOSE_SOCK;
    this->flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;

#ifdef _WIN32
    signal(SIGFPE, initialSigintPointer);
#else
    sigaction(SIGPIPE, &initialSigpipeAction, nullptr);
#endif

}


int tcp_data_base_server_p::getPortNumber() const noexcept
{
    return ntohs(this->servAddr.sin_port);
}


int64_t tcp_data_base_server_p::StopServer() noexcept
{
    const int64_t currentThreadTid = CinternalGetCurrentTid();
    if (this->flags.rd.shouldRun_false) {
        return currentThreadTid;
    }
    this->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;

    tcp_socket stpSockS(&this->stpData.stp);

    if (currentThreadTid == (this->serverTid)) {
        this->servAddr = {};
        this->stpData.stp.sock = CPPUTILS_SOCKS_CLOSE_SOCK;
        stpSockS.Close();
        return currentThreadTid;
    }

    stpSockS.SendSimple(CPPUTILS_SOCKS_INTERNAL_STP_SRV, CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
    while ((this->flags.rd.serverRunning_true) && (stpSockS.isValid())) {
        CinternalSleepInterruptableMs(2);
        stpSockS.SendSimple(CPPUTILS_SOCKS_INTERNAL_STP_SRV, CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
    }
    this->servAddr = {};
    this->stpData.stp.sock = CPPUTILS_SOCKS_CLOSE_SOCK;
    stpSockS.Close();
    return currentThreadTid;    
}


/*--------------------------------------------------------------------------------------------------------------*/

tcp_server_p::tcp_server_p()
    :
    ecClbk([]() {})
{
}


}}  //  namespace cpputils { namespace sockets{
