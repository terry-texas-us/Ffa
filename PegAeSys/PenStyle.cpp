#include "stdafx.h"

CPenStyle::CPenStyle(const CString& strName, const CString& strDesc, WORD wLens, double* dLen)
{
	m_strName = strName;
	m_strDesc = strDesc;
	m_wLens = wLens;
	if (m_wLens == 0)
	{
		m_dLen = 0;
	}
	else
	{
		m_dLen = new double[m_wLens];
		for (int i = 0; i < m_wLens; i++)
			m_dLen[i] = Max(- 99., Min(dLen[i], 99.));
	}
}

CPenStyle::~CPenStyle()
{
	delete [] m_dLen;
}

double CPenStyle::GetPatternLen() const
{
	double dLen = 0.;

	for (int i = 0; i < m_wLens; i++) 
		dLen += fabs(m_dLen[i]);
	
	return (dLen);
}