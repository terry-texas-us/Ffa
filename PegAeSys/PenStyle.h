#pragma once

class CPenStyle : public CObject
{
private:
	CString		m_strName;		// linetype name
								// standard flag values (not implemented)
	CString		m_strDesc; 		// descriptive text for linetype
	WORD		m_wLens;		// number of dash length items
	double*		m_dLen; 		// pointer to zero or more dash lengths

public:

	CPenStyle& CPenStyle::operator= (const CPenStyle& src)
	{
		m_strName = src.m_strName;

		m_strDesc = src.m_strDesc;
		m_wLens = src.m_wLens;
		//m_dLen = src.m_dLen;

		if (m_wLens != src.m_wLens)
		{
			if (m_wLens != 0)
				delete [] m_dLen;

			m_wLens = src.m_wLens;
			
			m_dLen = (m_wLens == 0) ? 0 : new double[m_wLens];
		}
		for (WORD w = 0; w < m_wLens; w++) m_dLen[w] = src.m_dLen[w]; 

		return *this;
	}

	CPenStyle(const CString& strName, const CString& strDesc, WORD, double*);
	
	~CPenStyle();
	
	WORD		GetDefLen() const {return m_wLens;}
	void		GetDashLen(double* dDash) const {for (int i = 0; i < m_wLens; i++) dDash[i] = m_dLen[i];}
	CString		GetDescription() const {return m_strDesc;}
	CString		GetName() const {return m_strName;}
	double		GetPatternLen() const;
};

typedef CTypedPtrArray<CObArray, CPenStyle*> CPenStyles;
