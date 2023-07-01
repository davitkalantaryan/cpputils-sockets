//
// repo:			cpputils-sockets
// file:            main_tcp_client_test.cpp
// path:			src/tests/main_tcp_client_test.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cpputils/sockets/export_symbols.h>
#include <cpputils/sockets/tcp_socket.hpp>
#include <stdio.h>

int main(void)
{
	cpputils::sockets::tcp_socket aSocket;
	if (aSocket.Connect("localhost", 9030, 1000)) {
		fprintf(stderr, "unable to connect\n");
		return 1;
	}
	//aServer.StoptServer();

	return 0;;
}
