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
#include <functional>

namespace cpputils { namespace sockets{

class CPPUTILS_DLL_PRIVATE tcp_server_p;

class CSOCKETS_EXPORT tcp_server
{
public:
	typedef ::std::function<void(tcp_socket*)>	TypeConnectClbk;

public:
	virtual ~tcp_server();
	tcp_server();
	tcp_server(tcp_server&& a_mM) noexcept;
	tcp_server(const tcp_server&)=delete;
	
	tcp_server& operator=(tcp_server&& a_mM) noexcept;
	tcp_server& operator=(const tcp_server&) = delete;
	void ReplaceWithOtherServer(tcp_server* CPPUTILS_ARG_NN a_pMM) noexcept;

	void StartServer(
						const TypeConnectClbk& a_clbk, int a_nPort,
						bool a_bOnlyLocalHost=false);

protected:
	tcp_server_p* m_serv_data_p;
};

}}  //  namespace cpputils { namespace sockets{
