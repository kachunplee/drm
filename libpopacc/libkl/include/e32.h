#ifndef __E32_H__

#define __E32_H__

#include <sys/types.h>

#define E32_M 29
#define E32_E (32-E32_M)
#define E32_M_MASK ((1<<E32_M)-1)
#define E32_E_MASK ((1<<E32_E)-1)
#define E32_E_BIT 6

class E32_t
{
public:
	E32_t (u_int32_t v = 0) 	{ m_value = v; }
	E32_t &operator=(E32_t);
	E32_t operator+(E32_t);

	u_int64_t toU64();
	u_int32_t GetV ()		{ return m_value; }

protected:
	u_int32_t	m_value;
};

#endif // __E32_H__
