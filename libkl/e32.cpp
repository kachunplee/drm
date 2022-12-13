#include "e32.h"

u_int64_t
E32_t::toU64 ()
{
	u_int64_t m = m_value&E32_M_MASK;
	m <<= ((m_value>>E32_M) * E32_E_BIT);
	return m;
}

E32_t&
E32_t::operator= (E32_t arg)
{
	m_value = arg.m_value;
	return *this;
}

E32_t
E32_t::operator+ (E32_t arg)
{
	u_int32_t size1 = m_value;
	u_int32_t size2 = arg.m_value;
	int e1 = size1>>E32_M;
	int e2 = size2>>E32_M;
	u_int32_t m1 = size1&E32_M_MASK;
	u_int32_t m2 = size2&E32_M_MASK;

	int d = e1 - e2;
	if (d > 0)
	{
		m2 >>= (d*E32_E_BIT);
		e2 = e1;
	}
	else if (d < 0)
	{
		m1 >>= ((-d)*E32_E_BIT);
		e1 = e2;
	}

	m1 += m2;
	while (m1 > E32_M_MASK)
	{
		m1 >>= E32_E_BIT;
		e1++;
	}

	if (e1 > E32_E_MASK)
	{
		e1 = E32_E_MASK;
		m1 = E32_M_MASK;
	}

	return E32_t((e1<<E32_M) + m1);
}
