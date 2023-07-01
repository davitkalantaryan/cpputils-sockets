//
// repo:			cpputils-sockets
// file:            main_tcp_server_test.cpp
// path:			src/tests/main_tcp_server_test.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cpputils/sockets/export_symbols.h>
#include <cpputils/sockets/socket_data.hpp>
#include <cpputils/sockets/tcp_server.hpp>

int main(void)
{
	cpputils::sockets::tcp_server aServer;
	aServer.StartServer(9030);
	SWITCH_SCHEDULING(10000);
	aServer.StoptServer();

	return 0;;
}
