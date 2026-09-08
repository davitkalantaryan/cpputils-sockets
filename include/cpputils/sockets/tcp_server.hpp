//
// repo:			cpputils-sockets
// file:            tcp_server.hpp
// path:			include/cpputils/sockets/tcp_server.hpp
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


namespace cpputils { namespace sockets{


struct StopperData;
class CPPUTILS_DLL_PRIVATE tcp_server_p;
class CPPUTILS_DLL_PRIVATE tcp_data_base_server_p;


class CSOCKETS_EXPORT tcp_server_base
{
public:
	typedef ::std::function<void(tcp_socket&,const sockaddr_in* CPPUTILS_ARG_NN)>	TypeConnectClbk;

public:
    ~tcp_server_base();
    tcp_server_base();
	tcp_server_base(tcp_server_base&& a_mM) noexcept;	
    tcp_server_base& operator=(tcp_server_base&& a_mM) noexcept;

    void ChangeAcceptCallback(const TypeConnectClbk& a_clbk);
    const sockaddr_in* getSockAddr() const noexcept;  // returns server socket address,
    int getPortNumber() const noexcept; // this is usefull when port number is 0, and one wants to know which port was assigned by OS
    int GetStopperData(StopperData* CPPUTILS_ARG_NN a_pStpData, size_t a_count);

protected:
    tcp_data_base_server_p*     m_serv_base_data_p;

private:
    tcp_server_base(const tcp_server_base&) = delete;
    tcp_server_base& operator=(const tcp_server_base&) = delete;
};


class CSOCKETS_EXPORT tcp_server : public tcp_server_base
{
public:
    typedef ::std::function<void(void)>	TypeExtraCleanClbk;

public:
    ~tcp_server();
    tcp_server();
    tcp_server(tcp_server&& a_mM) noexcept;
    tcp_server& operator=(tcp_server&& a_mM) noexcept;

    // stops and waits to stop and then clean server
    // can be stopped from accept callback, or from any thread
    void StoptAndCleanServer();
    // server will start in the different thread
    int StartAsyncServerOnOtherThreadAndReturn(
        int a_nPort, const TypeConnectClbk& a_clbk,
        bool a_bOnlyLocalHost = false, bool a_bReuse = true,
        const TypeExtraCleanClbk& a_ecclb = CPPUTILS_NULL);

protected:
    tcp_server_p*   m_serv_async_data_p;

private:
    tcp_server(const tcp_server&) = delete;
    tcp_server& operator=(const tcp_server&) = delete;
};


class CSOCKETS_EXPORT blocking_tcp_server : public tcp_server_base
{
public:
    blocking_tcp_server() = default;
    blocking_tcp_server(blocking_tcp_server&& a_mM) noexcept = default;
    blocking_tcp_server& operator=(blocking_tcp_server&& a_mM) noexcept = default;

    // stops and waits to stop
    // can be stopped from accept callback, or from any thread
    void StopAndCleanServer();

    // stops and waits to stop
    // can be stopped from accept callback, or from any thread
    void StopButNotCleanServer();

    // creates server socket, but does not start server
    // returns port number, or -1 on error
    // also one can ask getSockAddr
    int  CreateBlockingServer(int a_nPort=0, bool a_bOnlyLocalHost = false, bool a_bReuse = true);

    // server will run in the same thread, 
    // one can stop server by using signal, or from accept callback
    void RunBlockingServer(const TypeConnectClbk& a_clbk);

private:
    blocking_tcp_server(const blocking_tcp_server&) = delete;
    blocking_tcp_server& operator=(const blocking_tcp_server&) = delete;
};


}}  //  namespace cpputils { namespace sockets{
