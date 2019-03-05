#include "stdafx.h"

#include "Job2State.h"

CJob2State::CJob2State()
{
	m_dVersion = 1.0;

	m_rectClip.bottom = 5;
	m_rectClip.top = 30;
	m_rectClip.left = 5;
	m_rectClip.right = 40;

	m_pntOrigin[0] = 0;
	m_pntOrigin[1] = 0;

	m_strFilePath = "Filename - <empty>";
}