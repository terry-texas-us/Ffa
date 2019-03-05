#pragma once

class CJob2State
{
	public:
		CJob2State();

		double m_dVersion;

		RECT m_rectClip;
		CPnt m_pntOrigin;
		CString m_strFilePath;
};