
// common_sockettcp.hpp
// 2017 Jul 06

#ifndef __common_sockettcp_hpp__
#define __common_sockettcp_hpp__

#include <common/io/socket/base.hpp>


namespace common{ namespace io{ namespace socket{

class COMMON_SOCKETS_HANDLER_EXPORT Tcp : public Base
{
public:
	Tcp() {}
	Tcp(int a_sock) {m_socket=a_sock;}
	virtual ~Tcp();

	virtual int		connectC(const char *svrName, int port, int connectionTimeoutMs = 1000);
	virtual int		readC(void* buffer, int bufferLen)const;
	virtual int		writeC(const void* buffer, int bufferLen);

protected:
	virtual common::io::Base* Clone()const;
};

}}}  // namespace common{ namespace io{ namespace socket{


#endif  // #ifndef __common_sockettcp_hpp__
