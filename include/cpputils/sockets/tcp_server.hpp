//
// repo:			cpputils-sockets
// file:            tcp_server_async.hpp
// path:			include/cpputils/sockets/tcp_server_async.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <cpputils/sockets/export_symbols.h>
#include <cpputils/sockets/tcp_socket.hpp>
#include <cinternal/disable_compiler_warnings.h>
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4365)
#endif
#include <functional>
#ifdef _MSC_VER
#pragma warning (pop)
#endif
#include <cinternal/undisable_compiler_warnings.h>


struct pollfd;

namespace cpputils { namespace sockets{


struct StopperData;
class CPPUTILS_DLL_PRIVATE tcp_server_base_p;
class CPPUTILS_DLL_PRIVATE CStprSocks_p;


class CSOCKETS_EXPORT tcp_server_base
{
public:
	typedef ::std::function<void(tcp_socket&,const sockaddr_in* CPPUTILS_ARG_NN)>	TypeConnectClbk;

public:
    ~tcp_server_base();
    tcp_server_base();
    tcp_server_base(tcp_server_base&& a_mM) noexcept;
    tcp_server_base& operator=(tcp_server_base&& a_mM) noexcept;

    // creates server socket, but does not start server
    // returns port number, or -1 on error
    // also one can ask getSockAddr or getPortNumber to get actual port number being used by server
    int CreateServer(int a_nPort=0, bool a_bOnlyLocalHost = false, bool a_bReuse = true);
    // Destroy the server
    // before calling this one should stop the server
    void DestroyServer() noexcept;
    void ChangeAcceptCallback(const TypeConnectClbk& a_clbk);
    const sockaddr_in* getSockAddr() const noexcept;  // returns server socket address,
    int getPortNumber() const noexcept; // this is usefull when port number is 0, and one wants to know which port was assigned by OS
    int GetStopperData(StopperData* CPPUTILS_ARG_NN a_pStpData, size_t a_count);

protected:
    tcp_server_base_p*     m_serv_base_data_p;

protected:
    tcp_server_base(tcp_server_base_p* CPPUTILS_ARG_NN a_serv_base_data_p);
    tcp_server_base(const tcp_server_base&) = delete;
    tcp_server_base& operator=(const tcp_server_base&) = delete;
};


class CSOCKETS_EXPORT tcp_server_async : public tcp_server_base
{
public:
    typedef ::std::function<void(void)>	TypeExtraCleanClbk;

public:
    ~tcp_server_async();
    tcp_server_async();
    tcp_server_async(tcp_server_async&& a_mM) noexcept = default;
    tcp_server_async& operator=(tcp_server_async&& a_mM) noexcept = default;

    // stops and waits to stop and then clean server
    // can be stopped from accept callback, or from any thread
    void StopServer();
    // server will start in the different thread
    // Server shoulb be created in advance
    int StartAsyncServerOnOtherThreadAndReturn(const TypeConnectClbk& a_clbk,const TypeExtraCleanClbk& a_ecclb = CPPUTILS_NULL);
    // server will start in the different thread
    // if server is not created in advance, then will be created here
    int CreateAndStartAsyncServerOnOtherThreadAndReturn(
        int a_nPort, const TypeConnectClbk& a_clbk,
        bool a_bOnlyLocalHost = false, bool a_bReuse = true,
        const TypeExtraCleanClbk& a_ecclb = CPPUTILS_NULL);

private:
    tcp_server_async(const tcp_server_async&) = delete;
    tcp_server_async& operator=(const tcp_server_async&) = delete;
};


class CSOCKETS_EXPORT tcp_server_sync : public tcp_server_base
{
public:
    ~tcp_server_sync();
    tcp_server_sync() = default;
    tcp_server_sync(tcp_server_sync&& a_mM) noexcept = default;
    tcp_server_sync& operator=(tcp_server_sync&& a_mM) noexcept = default;

    // stops and waits to stop
    // can be stopped from accept callback, or from any thread
    void StopServer() noexcept;
    // server will run in the same thread, 
    // one can stop server by using signal, or from accept callback
    int StartSyncServer(const TypeConnectClbk& a_clbk);
    // server will start in the same thread
    // if server is not created in advance, then will be created here
    // this call will block the thread
    int CreateAndStartSyncServerOnThisThread(
        int a_nPort, const TypeConnectClbk& a_clbk,
        bool a_bOnlyLocalHost = false, bool a_bReuse = true);

private:
    tcp_server_sync(const tcp_server_sync&) = delete;
    tcp_server_sync& operator=(const tcp_server_sync&) = delete;
};


class CPPUTILS_DLL_PRIVATE CStprSocks
{
public:
    ~CStprSocks() noexcept;
    CStprSocks(const StopperData& a_stpDt);

    void interrupBlockingSocksCall() const noexcept;
    // >0  returns revent of the poll fd for argument socket
    // 0   means timeout,
    // -1  poll error, probably EINTR, or
    // -2  stopping was done
    int  waitForAction(ptrdiff_t a_otherRawSock, int a_timeoutMs=-1) const noexcept;
    int  waitForAction(const tcp_socket& a_otherSock, int a_timeoutMs=-1) const noexcept;
    //int  waitForAction(size_t a_rawSocksCount, ptrdiff_t* a_otherRawSocks_p, int a_timeoutMs=-1) const noexcept;
    // >0  returns number of FDs has read data
    // 0   means timeout,
    // <0  poll error, probably EINTR, or
    int  waitForAction(size_t a_rawSocksCount, ptrdiff_t* a_otherRawSocks_p, pollfd* CPPUTILS_ARG_NN a_pollfdBuff_p, bool* CPPUTILS_ARG_NN a_isStp_p, int a_timeoutMs=-1) const noexcept;
    // struct pollfd

private:
    CStprSocks_p*   m_data_p;

private:
    CStprSocks(const CStprSocks&)=delete;
    CStprSocks(CStprSocks&&)=delete;
    CStprSocks& operator=(const CStprSocks&)=delete;
    CStprSocks& operator=(CStprSocks&&)=delete;
};


}}  //  namespace cpputils { namespace sockets{
