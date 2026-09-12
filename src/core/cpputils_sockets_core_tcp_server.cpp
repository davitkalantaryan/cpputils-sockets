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
#ifndef MakeSocketNonBlockingInline_needed
#define MakeSocketNonBlockingInline_needed
#endif

#include <cpputils/sockets/tcp_server.hpp>
#include "cpputils_sockets_core_tcp_socket_p.hpp"
#include <cinternal/unnamed_semaphore.h>
#include <cinternal/bistateflags.h>
#include <cinternal/signals.h>
#include <cinternal/gettid.h>
#include <cinternal/logger.h>
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

#define cpputilsAsynSrvDt(_data)                static_cast<tcp_server_async_p*>(_data)


#ifdef _MSC_VER
#pragma warning (disable:5039)
#pragma warning (disable:4820)
#endif
typedef void (*SignalHandlerPointer)(int);


static void SigHandlerFunction(int a_signo) noexcept {
#ifdef _WIN32
    signal(a_signo, &SigHandlerFunction);
#endif
    CPPUTILS_STATIC_CAST(void, a_signo);
}


class CPPUTILS_DLL_PRIVATE CStprSocks_p
{
public:
    CStprSocks_p() = default;
    ::cpputils::sockets::tcp_socket     wUp;
    ::cpputils::sockets::SysSocket      pol;
private:
    CStprSocks_p(const CStprSocks_p&) = delete;
    CStprSocks_p& operator=(const CStprSocks_p&) = delete;
};


class CPPUTILS_DLL_PRIVATE tcp_server_base_p
{
public:
    tcp_server_base::TypeConnectClbk    clbk;
	socket_t					        serv;
    struct StopperData                  stpData;
    sockaddr_in					        servAddr;
    int64_t                             serverTid;
	CPPUTILS_BISTATE_FLAGS_UN(
        shouldRun,
        serverRunning,
        isCreated,
        hasError
    )	flags;

public:
    virtual ~tcp_server_base_p() = default;
    tcp_server_base_p();

    int  CreateServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse);
    int  CreateServerRaw(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse) noexcept;
    void DestroyServer() noexcept;
    int getPortNumber() const noexcept;
    int GetStopperData(StopperData* CPPUTILS_ARG_NN a_pStpData, size_t a_count);
    inline int64_t StopServerInline() noexcept;
    inline void RunServerInline() noexcept;

private:
    inline void ServerAcceptInline(struct sockaddr_in* a_bufForRemAddress) noexcept;

private:
    tcp_server_base_p(const tcp_server_base_p&) = delete;
    tcp_server_base_p(tcp_server_base_p&&) = delete;
    tcp_server_base_p& operator=(const tcp_server_base_p&) = delete;
    tcp_server_base_p& operator=(tcp_server_base_p&&) = delete;
};


class CPPUTILS_DLL_PRIVATE tcp_server_async_p : public tcp_server_base_p
{
public:
    tcp_server_async::TypeExtraCleanClbk    ecClbk;
    ::std::thread                           server_thread;
public:
    tcp_server_async_p();
    void StartAsyncServerOnOtherThreadAndReturn(const tcp_server_base::TypeConnectClbk& a_clbk, const tcp_server_async::TypeExtraCleanClbk& a_ecclb);
private:
    tcp_server_async_p(const tcp_server_async_p&) = delete;
    tcp_server_async_p(tcp_server_async_p&&) = delete;
    tcp_server_async_p& operator=(const tcp_server_async_p&) = delete;
    tcp_server_async_p& operator=(tcp_server_async_p&&) = delete;
};


/*--------------------------------------------------------------------------------------------------------------*/

inline int64_t tcp_server_base_p::StopServerInline() noexcept
{
    const int64_t currentThreadTid = CinternalGetCurrentTid();
    if (this->flags.rd.shouldRun_false) {
        return currentThreadTid;
    }
    this->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;

    if (currentThreadTid == (this->serverTid)) {
        this->flags.wr.serverRunning = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
        return currentThreadTid;
    }

    tcp_socket stpSockS(&this->stpData.stp);
    stpSockS.sendSimple(CPPUTILS_SOCKS_INTERNAL_STP_SRV, CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
    while ((this->flags.rd.serverRunning_true) && (stpSockS.isValid())) {
        CinternalSleepInterruptableMs(2);
        stpSockS.sendSimple(CPPUTILS_SOCKS_INTERNAL_STP_SRV, CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
    }
    stpSockS.Release();
    return currentThreadTid;
}


/*--------------------------------------------------------------------------------------------------------------*/

tcp_server_base::~tcp_server_base()
{
    delete m_serv_base_data_p;
}


tcp_server_base::tcp_server_base()
	:
    m_serv_base_data_p(new tcp_server_base_p())
{
}


tcp_server_base::tcp_server_base(tcp_server_base_p* CPPUTILS_ARG_NN a_serv_base_data_p)
    :
    m_serv_base_data_p(a_serv_base_data_p)
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
    tcp_server_base_p* const pThisData = m_serv_base_data_p;
	m_serv_base_data_p = a_mM.m_serv_base_data_p;
	a_mM.m_serv_base_data_p = pThisData;
	return *this;
}


int tcp_server_base::CreateServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse)
{
    const int rtn = m_serv_base_data_p->CreateServer(a_nPort, a_bOnlyLocalHost, a_bReuse);
    if (rtn < 0) {
        return rtn;
    }
    return m_serv_base_data_p->getPortNumber();
}


void tcp_server_base::DestroyServer() noexcept
{
    m_serv_base_data_p->DestroyServer();
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

tcp_server_async::~tcp_server_async()
{
    StopServer();
    m_serv_base_data_p->DestroyServer();
}


tcp_server_async::tcp_server_async()
    :
    tcp_server_base(new tcp_server_async_p())
{
}


void tcp_server_async::StopServer()
{
    if (m_serv_base_data_p->flags.rd.shouldRun_false) {
        return;
    }
    
    if ((m_serv_base_data_p->serverTid) != m_serv_base_data_p->StopServerInline()) {
        cpputilsAsynSrvDt(m_serv_base_data_p)->server_thread.join();
    }
    else {
        cpputilsAsynSrvDt(m_serv_base_data_p)->server_thread.detach();
    }
}


int tcp_server_async::StartAsyncServerOnOtherThreadAndReturn(const TypeConnectClbk& a_clbk,const TypeExtraCleanClbk& a_ecclb)
{
    if(m_serv_base_data_p->flags.rd.isCreated_false){
        return 1;
    }
    cpputilsAsynSrvDt(m_serv_base_data_p)->StartAsyncServerOnOtherThreadAndReturn(a_clbk,a_ecclb);
    return 0;
}


int tcp_server_async::CreateAndStartAsyncServerOnOtherThreadAndReturn(
    int a_nPort, const TypeConnectClbk& a_clbk,
    bool a_bOnlyLocalHost, bool a_bReuse,
    const TypeExtraCleanClbk& a_ecclb)
{
    if(m_serv_base_data_p->flags.rd.isCreated_false){
        const int cnCrtRes = m_serv_base_data_p->CreateServer(a_nPort,a_bOnlyLocalHost,a_bReuse);
        if(cnCrtRes){
            return -1;
        }
    }  //  if(m_serv_base_data_p->flags.rd.isCreated_false){
    cpputilsAsynSrvDt(m_serv_base_data_p)->StartAsyncServerOnOtherThreadAndReturn(a_clbk,a_ecclb);
    return 0;
}


/*--------------------------------------------------------------------------------------------------------------*/

tcp_server_sync::~tcp_server_sync()
{
    StopServer();
    m_serv_base_data_p->DestroyServer();
}


// stops and waits to stop
// can be stopped from accept callback, or from any thread
void tcp_server_sync::StopServer() noexcept
{
    if (m_serv_base_data_p->flags.rd.shouldRun_false) {
        return;
    }
    m_serv_base_data_p->StopServerInline();
}


// server will run in the same thread, 
// one can stop server by using signal, or from accept callback
int tcp_server_sync::StartSyncServer(const TypeConnectClbk& a_clbk)
{
    if(m_serv_base_data_p->flags.rd.isCreated_false){
        return 1;
    }
    m_serv_base_data_p->clbk = a_clbk;
    m_serv_base_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    m_serv_base_data_p->RunServerInline();
    return 0;
}


// server will start in the same thread
// if server is not created in advance, then will be created here
// this call will block the thread
int tcp_server_sync::CreateAndStartSyncServerOnThisThread(
    int a_nPort, const TypeConnectClbk& a_clbk,
    bool a_bOnlyLocalHost, bool a_bReuse)
{
    if(m_serv_base_data_p->flags.rd.isCreated_false){
        const int cnCrtRes = m_serv_base_data_p->CreateServer(a_nPort,a_bOnlyLocalHost,a_bReuse);
        if(cnCrtRes<1){
            return -1;
        }
    }  //  if(m_serv_base_data_p->flags.rd.isCreated_false){
    m_serv_base_data_p->clbk = a_clbk;
    m_serv_base_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    m_serv_base_data_p->RunServerInline();
    return 0;
}


/*--------------------------------------------------------------------------------------------------------------*/

tcp_server_base_p::tcp_server_base_p()
{
	this->flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    this->clbk = [](tcp_socket&, const sockaddr_in*) {};
	this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
    this->stpData = { {CPPUTILS_SOCKS_CLOSE_SOCK}, {CPPUTILS_SOCKS_CLOSE_SOCK} };
    this->servAddr = {};
    this->serverTid = 0;
}


inline void tcp_server_base_p::ServerAcceptInline(struct sockaddr_in* a_bufForRemAddress) noexcept
{
    struct pollfd vPollFd[4];
    cpputils_poll_arg2 nPollFdCount = 1;
    vPollFd[0].fd = this->serv;
    vPollFd[0].events = POLLIN | POLLRDNORM | POLLRDBAND;
    vPollFd[0].revents = 0;

    vPollFd[1].revents = 0; // we will analyze this, and if POLLIN, we have to read it to prevent socket kernel buffer filling
    if ((this->stpData.pol.sock) != CPPUTILS_SOCKS_CLOSE_SOCK) {
        nPollFdCount = 2;
        vPollFd[1].fd = this->stpData.pol.sock;
        vPollFd[1].events = POLLIN | POLLRDNORM | POLLRDBAND;
    }

    const int pollRes = CpputilsPoll(vPollFd, nPollFdCount, -1);
    if (this->flags.rd.shouldRun_false) {
        return;
    }  //  if (this->flags.rd.shouldRun_true) {
    
    if (pollRes > 0) {
        if (vPollFd[1].revents & POLLIN) {
            char vcBuff[CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN + 10];
            tcp_socket  aSock(&this->stpData.pol);
            aSock.receiveAll(vcBuff, CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
            aSock.Release();
        }
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


void tcp_server_base_p::RunServerInline() noexcept
{
#ifdef _WIN32
    const SignalHandlerPointer initialSigintPointer = signal(SIGINT, &SigHandlerFunction);
#else
    struct sigaction initialSigpipeAction;
    struct sigaction newAction;
    memset(&newAction, 0, sizeof(struct sigaction));
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    newAction.sa_handler = &SigHandlerFunction;
    sigaction(SIGPIPE, &newAction, &initialSigpipeAction);
#endif

    if (!this->clbk) {
        this->clbk = [](tcp_socket&, const sockaddr_in*) ->void {};
    }

    this->serverTid = CinternalGetCurrentTid();
    sockaddr_in remoteAddress;
    this->flags.wr.serverRunning = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    while (this->flags.rd.shouldRun_true && this->flags.rd.hasError_false) {
        ServerAcceptInline(&remoteAddress);
    }
    this->flags.wr.serverRunning = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    this->serverTid = 0;

#ifdef _WIN32
    signal(SIGFPE, initialSigintPointer);
#else
    sigaction(SIGPIPE, &initialSigpipeAction, nullptr);
#endif

}


void tcp_server_base_p::DestroyServer() noexcept
{
    if(this->flags.rd.isCreated_false){
        return;
    }

    if(this->flags.rd.serverRunning_true){
        CInternalLogCritical("Before Destroying server, one should stop it");
        return;
    }

    this->servAddr = {};
    this->serverTid = 0;

    if((this->stpData.pol.sock)!=CPPUTILS_SOCKS_CLOSE_SOCK){
        CpputilsCloseSocket(this->stpData.pol.sock);
        this->stpData.pol.sock = CPPUTILS_SOCKS_CLOSE_SOCK;
    }

    if((this->stpData.stp.sock)!=CPPUTILS_SOCKS_CLOSE_SOCK){
        CpputilsCloseSocket(this->stpData.stp.sock);
        this->stpData.stp.sock = CPPUTILS_SOCKS_CLOSE_SOCK;
    }

    if((this->serv)!=CPPUTILS_SOCKS_CLOSE_SOCK){
        CpputilsCloseSocket(this->serv);
        this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
    }

    this->flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
}


int tcp_server_base_p::CreateServer(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse)
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


int tcp_server_base_p::CreateServerRaw(int a_nPort, bool a_bOnlyLocalHost, bool a_bReuse) noexcept
{
    this->serv = ::socket(AF_INET, SOCK_STREAM, 0);
    if (CHECK_FOR_SOCK_INVALID(this->serv)) {
        this->serv = CPPUTILS_SOCKS_CLOSE_SOCK;
        this->flags.wr.hasError = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        return(-1);  // no socket
    }
    if (a_bReuse) { int i(1); setsockopt(this->serv, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i)); }

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
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


int tcp_server_base_p::GetStopperData(StopperData* CPPUTILS_ARG_NN a_pStpData, size_t a_count)
{
    if (this->flags.rd.isCreated_false) {
        return -1;
    }

    static int nIter =0;
    const int curTid = (int)CinternalGetCurrentTid();
    CInternalLogDebug("In: %d, tid: %d",++nIter,curTid);

    cinternal_unnamed_sema_t* const sema_for_to_finish_p = new cinternal_unnamed_sema_t();
    if (cinternal_unnamed_sema_create(sema_for_to_finish_p, 0)) {
        // log on semaphore creation failure
        delete sema_for_to_finish_p;
        return -1;
    }

    const int cnPort = ntohs(this->servAddr.sin_port);
    const uint64_t inShouldRun = this->flags.wr.shouldRun;
    this->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    ::std::thread* const pTmpThread = new ::std::thread([a_pStpData, a_count,cnPort]() {
        tcp_socket pollSocket;
        int rtn = -1;
        for (size_t ind(0); ind < a_count; ) {
            rtn = pollSocket.Connect("localhost", cnPort, -1);
            if (rtn) {
                pollSocket.Close();
                CinternalSleepInterruptableMs(10);
                continue;
            }
            rtn = pollSocket.sendSimple(CPPUTILS_SOCKS_INTERNAL_CHK_STR, CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN);
            if (rtn != CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN) {
                pollSocket.Close();
                CinternalSleepInterruptableMs(10);
                continue;
            }
            SysSocket aSysSock;
            pollSocket.GetSysSocketAndRelease(&aSysSock);
            a_pStpData[ind++].stp.sock = aSysSock.sock;
        }  //  while (rtn) {
    });  //  ::std::thread tmpThread([&stpSocket,cnPort]() {
    
    const tcp_server_base::TypeConnectClbk* const aClbkIn_p = new tcp_server_base::TypeConnectClbk(this->clbk);
    typedef ::std::function<bool(tcp_socket&)>	TypeConnectExtraClbk;
    size_t* const ind_p = new size_t(0);
    const TypeConnectExtraClbk* const clbkExtra_p = new TypeConnectExtraClbk([this,a_pStpData,a_count, aClbkIn_p, inShouldRun,ind_p,sema_for_to_finish_p](tcp_socket& a_sock) ->bool{
        const int curTid = (int)CinternalGetCurrentTid();
        CInternalLogDebug("Clbk: %d, tid: %d",nIter,curTid);
        a_sock.MakeSocketBlocking();
        a_sock.SetTimeout(1000);
        char vcBuffer[CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN+10];
        const int rdRet = a_sock.receiveSngl(vcBuffer, CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN);
        if (rdRet == CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN) {
            if (memcmp(vcBuffer, CPPUTILS_SOCKS_INTERNAL_CHK_STR, CPPUTILS_SOCKS_INTERNAL_CHK_STR_LEN) == 0) {
                SysSocket aSysSock;
                a_sock.GetSysSocketAndRelease(&aSysSock);
                a_pStpData[(*ind_p)++].pol.sock = aSysSock.sock;
                if ((*ind_p) >= a_count) {
                    this->clbk = *aClbkIn_p;
                    this->flags.wr.shouldRun = inShouldRun;
                    cinternal_unnamed_sema_post(sema_for_to_finish_p);
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
    }  //  if (this->flags.rd.serverRunning_false) {

    pTmpThread->join();
    delete pTmpThread;

    cinternal_unnamed_sema_wait(sema_for_to_finish_p);
    cinternal_unnamed_sema_destroy(sema_for_to_finish_p);
    delete sema_for_to_finish_p;

    delete ind_p;
    delete clbkExtra_p;
    delete aClbkIn_p;

    CInternalLogDebug("Out: %d",nIter);

    return 0;
}


int tcp_server_base_p::getPortNumber() const noexcept
{
    return ntohs(this->servAddr.sin_port);
}


/*--------------------------------------------------------------------------------------------------------------*/

tcp_server_async_p::tcp_server_async_p()
    :
    ecClbk([]() {})
{
}


void tcp_server_async_p::StartAsyncServerOnOtherThreadAndReturn(const tcp_server_base::TypeConnectClbk& a_clbk, const tcp_server_async::TypeExtraCleanClbk& a_ecclb)
{
    if (this->flags.rd.shouldRun_true) {
        return;  // server already started
    }

    this->clbk = a_clbk;
    this->ecClbk = a_ecclb ? (a_ecclb) : ([]()->void {});
    this->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;

    this->server_thread = ::std::thread([this]() {
        this->RunServerInline();
        this->ecClbk();
    });
}

/*--------------------------------------------------------------------------------------------------------------*/

CStprSocks::~CStprSocks() noexcept
{
    if(m_data_p){
        m_data_p->wUp.Close();
        tcp_socket sctPoll(&(m_data_p->pol));
        sctPoll.Close();
    }  //  if(m_data_p){
}


CStprSocks::CStprSocks(const ::cpputils::sockets::StopperData& a_stpDt)
:
    m_data_p(new CStprSocks_p())
{
    m_data_p->pol = a_stpDt.pol;
    m_data_p->wUp.ResetFromSysSock(&(a_stpDt.stp));
    tcp_socket sctPoll(&(a_stpDt.pol));
    sctPoll.MakeSocketNonBlocking();
    sctPoll.Release();
}


void CStprSocks::interrupBlockingSocksCall() const noexcept
{
    m_data_p->wUp.send(CPPUTILS_SOCKS_INTERNAL_STP_SRV,CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
}


// >0  returns revent of the poll fd for argument socket
// 0   means timeout,
// -1  poll error, probably EINTR, or
// -2  stopping was done
int CStprSocks::waitForAction(ptrdiff_t a_otherRawSock, int a_timeoutMs) const noexcept
{
    struct pollfd vPollFd[4];
    ::cpputils::sockets::cpputils_poll_arg2 nPollFdCount = 1;
    vPollFd[0].fd = m_data_p->pol.sock;
    vPollFd[0].events = POLLIN | POLLRDNORM | POLLRDBAND;
    vPollFd[0].revents = 0;

    if (a_otherRawSock >= 0) {
        nPollFdCount = 2;
        vPollFd[1].fd = (socket_t)a_otherRawSock;
        vPollFd[1].events = POLLIN | POLLRDNORM | POLLRDBAND;
        vPollFd[1].revents = 0;
    }

    const int pollRes = CpputilsPoll(vPollFd, nPollFdCount, a_timeoutMs);
    if(pollRes<1){
        return pollRes ? (-1) : 0;
    }

    if(vPollFd[0].revents & POLLIN){
        // we have to read buffer for preventing socket kernel buffer overflow
        char vcBuff[CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN + 10];
        tcp_socket  aSock(&(m_data_p->pol));
        aSock.receiveAll(vcBuff, CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
        aSock.Release();
    }

    if(nPollFdCount<2){
        return -2;
    }

    return vPollFd[1].revents;
}


int CStprSocks::waitForAction(const tcp_socket& a_otherSock, int a_timeoutMs) const noexcept
{
    const ptrdiff_t rawSock = a_otherSock.getRawSock();
    return waitForAction(rawSock,a_timeoutMs);
}


// >0  returns number of FDs has read data
// 0   means timeout,
// <0  poll error, probably EINTR, or
int CStprSocks::waitForAction(size_t a_rawSocksCount, ptrdiff_t* a_otherRawSocks_p, pollfd* CPPUTILS_ARG_NN a_pollfdBuff_p, bool* CPPUTILS_ARG_NN a_isStp_p, int a_timeoutMs) const noexcept
{
    for(size_t ind(0); ind<a_rawSocksCount; ++ind){
        a_pollfdBuff_p[ind].fd = (socket_t)a_otherRawSocks_p[ind];
        a_pollfdBuff_p[ind].events = POLLIN | POLLRDNORM | POLLRDBAND;
        a_pollfdBuff_p[ind].revents = 0;
    }

    a_pollfdBuff_p[a_rawSocksCount].fd = m_data_p->pol.sock;
    a_pollfdBuff_p[a_rawSocksCount].events = POLLIN | POLLRDNORM | POLLRDBAND;
    a_pollfdBuff_p[a_rawSocksCount].revents = 0;

    const int pollRes = CpputilsPoll(a_pollfdBuff_p,(cpputils_poll_arg2)(a_rawSocksCount+1), a_timeoutMs);

    if(a_pollfdBuff_p[a_rawSocksCount].revents & POLLIN){
        // we have to read buffer for preventing socket kernel buffer overflow
        char vcBuff[CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN + 10];
        tcp_socket  aSock(&(m_data_p->pol));
        aSock.receiveAll(vcBuff, CPPUTILS_SOCKS_INTERNAL_STP_SRV_LEN);
        aSock.Release();
        *a_isStp_p = true;
    }
    else{
        *a_isStp_p = false;
    }

    return pollRes;
}


}}  //  namespace cpputils { namespace sockets{
