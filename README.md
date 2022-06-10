# common-sockets
Implemented classes for network sockets. 
There are simple classes that can be used as an example during designing code, that need to use raw TCP 
server/client connection. 
  
# Code snippets  
In order to have clue on using classes, pleae have a look to [tcp.hpp](include/common/io/socket/tcp.hpp)  
  
``` c++  
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
```  

![tcp.hpp](docs/pics/tcp_socket_header_2020.09.06.PNG)
