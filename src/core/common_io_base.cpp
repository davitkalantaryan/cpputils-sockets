
// common_iodevice.cpp
// 2017 Jul 6

#include <common/io/base.hpp>

using namespace common;

io::Base::Base()
	:
	m_pPrev(NULL),
	m_pNext(NULL)
{
}


io::Base::Base(const Base& a_cM)
{
	cloneFromOther(a_cM);
}


io::Base::~Base()
{
	closeC();
}


void io::Base::closeC(void)
{
	if(isOpenC()){
		if (!m_pPrev && !m_pNext) {
			this->closeHard();
		}
		else {
			if (m_pPrev) { m_pPrev->m_pNext = m_pNext; }
			if (m_pNext) { m_pNext->m_pPrev = m_pPrev; }
		}
		m_pPrev = m_pNext = NULL;
	}
}


io::Base& io::Base::operator=(const Base& a_cM)
{
	cloneFromOther(a_cM);
	return *this;
}


void io::Base::cloneFromOther(const Base& a_cM)
{
	if (a_cM.isOpenC()) {
		Base* pNext = a_cM.m_pNext;
		a_cM.m_pNext = this;
		m_pNext = pNext;
		if (pNext) { pNext->m_pPrev = this; }
		m_pPrev = &a_cM;
	}
}


io::Base* io::Base::Clone()const
{
	return NULL;
}
