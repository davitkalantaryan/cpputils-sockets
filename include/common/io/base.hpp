
// common_iodevice.hpp
// 2017 Jul 6

#ifndef __common_iodevice_hpp__
#define __common_iodevice_hpp__

#include <common_sockets_internal.h>
#include <stddef.h>


namespace common{ namespace io{

class COMMON_SOCKETS_HANDLER_EXPORT Base
{
public:
	Base();
	Base(const Base& cM);
	virtual ~Base();

	virtual bool	isOpenC(void)const { return false; }
	virtual void	closeC(void);

	virtual int		readC(void* buffer, int bufferLen)const=0;
	virtual int		writeC(const void* buffer, int bufferLen)=0;

	virtual int		setTimeout(int timeoutMS) = 0;

	Base& operator=(const Base& aM);
	virtual Base* Clone()const;

protected:
	virtual void	closeHard(void) {}
	virtual void	cloneFromOther(const Base& other);

protected:
	const Base*		m_pPrev;
	mutable Base*	m_pNext;
};

}} // namespace common{ namespace io{


#endif  // #ifndef __common_iodevice_hpp__
