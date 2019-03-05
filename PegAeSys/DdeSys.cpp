#include "stdafx.h"

#include "ddeSys.h"

using namespace dde;

///<summary>Return the Help info</summary>
HDDEDATA dde::SysReqHelp(UINT wFmt, HSZ , HSZ hszItem)
{
	static char sz[] = 
		"DDE Help for the PegAEsys Service.\r\n\t"
		"Request or advise on 'General!<item>' to get the current value.\r\n\t"
		"Items are:  RelPosX, RelPosY, RelPosZ,...";

	return MakeCFText(wFmt, sz, hszItem);
}

///<summary>Return the system item list.</summary>
HDDEDATA dde::SysReqItems(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
	// The data is returned in CF_TEXT format as a tab delimited list of names.
	int cb;

	PTOPICINFO pTopic = TopicFind(hszTopic);
	
	if (!pTopic)												// No system topic
		return 0;

	HDDEDATA hData = DdeCreateDataHandle(ServerInfo.dwInstance, 0, 0, 0, hszItem, wFmt, 0); // Empty data object to fill
	int cbOffset = 0;
	PITEMINFO pItem = pTopic->pItemList;
	
	while (pItem)												// Walk the item list
	{
		if (cbOffset != 0)										// Put in a tab delimiter
		{
			DdeAddData(hData, (LPBYTE) "\t", 1, cbOffset);
			cbOffset++;
		}
		cb = lstrlen(pItem->pszItemName);						// the string name of the item
		DdeAddData(hData, (LPBYTE) pItem->pszItemName, cb, cbOffset);
		cbOffset += cb;
		pItem = pItem->pNext;
	}
	DdeAddData(hData, (LPBYTE) "", 1, cbOffset);							// Put a 0 on the end
	// Return a DDE data handle to the data object containing the return data.
	return hData;
}

// Process a request for the system format list.  The Format list is the union of all
// formats supported for all topics of the service.
HDDEDATA dde::SysReqFormats(UINT wFmt, HSZ , HSZ hszItem)
{
	PITEMINFO pItem;
	WORD wFormats[MAXFORMATS];

	wFormats[0] = 0; 										// Start with an empty list

	DdeCreateDataHandle(ServerInfo.dwInstance, 0, 0, 0, hszItem, wFmt, 0); // Empty data object to fill
	
	PTOPICINFO pTopic = ServerInfo.pTopicList;
	while (pTopic)												// Walk the topic list
	{
		pItem = pTopic->pItemList;
		while (pItem)											// Walk the item list for this topic
		{
			AddFormatsToList(wFormats, MAXFORMATS, pItem->pFormatList); // Add unique formats to the main list
			pItem = pItem->pNext;
		}
		pTopic = pTopic->pNext;
	}
	// Return a DDE data handle to the data object containing the return data.
	return MakeDataFromFormatList((LPWORD) wFormats, (WORD) wFmt, hszItem);
}

// Process a request for the list of protocols supported by the server.  The default is to return the
// 'Execute Control 1' protocol.
HDDEDATA dde::SysReqProtocols(UINT , HSZ , HSZ hszItem)
{
	static char sz[] = "Execute Control 1";
	
	return DdeCreateDataHandle(ServerInfo.dwInstance, (LPBYTE) sz, lstrlen(sz) + 1, 0, hszItem, CF_TEXT, 0);
// Return a DDE data handle to the data object containing the return data.
}

// Process a request for the topics list.  Typically the data is returned in CF_TEXT format as a tab
// delimited list of names.
HDDEDATA dde::SysReqTopics(UINT wFmt, HSZ, HSZ hszItem)
{
	int cb;

	HDDEDATA hData = DdeCreateDataHandle(ServerInfo.dwInstance, 0, 0, 0, hszItem, wFmt, 0); // Empty data object to fill
	int cbOffset = 0;
	PTOPICINFO pTopic = ServerInfo.pTopicList;
	
	while (pTopic) 
	{
		if (cbOffset != 0)										// Put in a tab delimiter
		{
			DdeAddData(hData, (LPBYTE) "\t", 1, cbOffset);
			cbOffset++;
		}
		cb = lstrlen(pTopic->pszTopicName); 					// the string name of the topic
		DdeAddData(hData, (LPBYTE) pTopic->pszTopicName, cb, cbOffset);
		cbOffset += cb;
		pTopic = pTopic->pNext;
	}
	DdeAddData(hData, (LPBYTE) "", 1, cbOffset);							// Put a 0 on the end
	// Return a DDE data handle to the data object containing the return data.
	return hData;
}