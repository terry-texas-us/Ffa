#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "StringExtra.h"

// Format: 
// line 1: // <name of the .ffa file>
// line 2: //
// line 3 - end: <4 char key> // <var number of printable chars describing key>
// The key identifies the .jb1 to use. The description becomes its .peg name.

// TODO: test ffa read when open pass non peg files

void ffaReadFile(const CString& strPathName)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CStdioFile fl;
						
	if (fl.Open(strPathName, CFile::modeRead | CFile::typeText))
	{
		char szLine[256];
		while (fl.ReadString(szLine, sizeof(szLine) - 1) != 0)
		{
			CString strLine(szLine);
			int nComment = strLine.Find("//");
			
			if (nComment == 0 || nComment == - 1) continue;
			
			CString strName = strLine.Left(nComment);
			strName.TrimRight();
			strName += ".jb1";

			if (pDoc->LayersLookup(strName) < 0)
			{
				pDoc->TracingMap(strName);
				pDoc->TracingFuse(strName);
				CLayer* pLayer = pDoc->LayersGet(strName);
				if (pLayer != 0)
				{
					strName = strLine.Mid(nComment + 2);
					strName.Trim();
					pLayer->SetName(strName);
				}
			}
		}
	}
}