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

class CPPUTILS_DLL_PRIVATE tcp_server_p;

class CSOCKETS_EXPORT tcp_server
{
public:
	typedef ::std::function<void(tcp_socket&,const sockaddr_in* CPPUTILS_ARG_NN)>	TypeConnectClbk;
	typedef ::std::function<void(void)>	TypeExtraCleanClbk;
	static const TypeConnectClbk	s_defClbk;

public:
	virtual ~tcp_server();
	tcp_server();
	tcp_server(tcp_server&& a_mM) noexcept;
	tcp_server(const tcp_server&)=delete;
	
	tcp_server& operator=(tcp_server&& a_mM) noexcept;
	tcp_server& operator=(const tcp_server&) = delete;
	void ReplaceWithOtherServer(tcp_server* CPPUTILS_ARG_NN a_pMM) noexcept;

	// server will start in the different thread
	int StartServer(
						int a_nPort, const TypeConnectClbk& a_clbk= s_defClbk, int a_lnTimeoutMs = 1000,
						bool a_bOnlyLocalHost=false, bool a_bReuse=true, const TypeExtraCleanClbk& a_ecclb = CPPUTILS_NULL);
    void ChangeAcceptCallback(const TypeConnectClbk& a_clbk);
	void StoptServer();  // stops and waits to stop

protected:
	tcp_server_p* m_serv_data_p;
};

}}  //  namespace cpputils { namespace sockets{
