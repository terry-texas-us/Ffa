#include "stdafx.h"

#include "dde.h"
#include "ddeSys.h"

using namespace dde;

///<summary>Add a new topic and default processing for its item list and formats</summary>
PTOPICINFO dde::TopicAdd(LPSTR lpszTopic, PEXECFN pfnExec, PREQUESTFN pfnRequest, PPOKEFN pfnPoke)   
{
	PTOPICINFO pTopic = TopicFind(lpszTopic);

	// See if we already have this topic
	if (pTopic) 						// We already have this one so just update its info
	{
		pTopic->pfnExec = pfnExec;
		pTopic->pfnRequest = pfnRequest;
		pTopic->pfnPoke = pfnPoke;
	} 
	else														
	{	// Create a new topic
		pTopic = (PTOPICINFO) new char[sizeof(TOPICINFO)];
		if (!pTopic) return 0;
		
		::ZeroMemory(pTopic, sizeof(TOPICINFO));
		pTopic->pszTopicName = lpszTopic;
		pTopic->hszTopicName = DdeCreateStringHandle(ServerInfo.dwInstance, lpszTopic, CP_WINANSI);
		pTopic->pItemList = 0;
		pTopic->pfnExec = pfnExec;
		pTopic->pfnRequest = pfnRequest;
		pTopic->pfnPoke = pfnPoke;
		pTopic->pCmdList = 0;
		
		// Add it to the list
		pTopic->pNext = ServerInfo.pTopicList;
		ServerInfo.pTopicList = pTopic;

		// Add handlers for its item list and formats.
		ItemAdd(lpszTopic, SZDDE_ITEM_ITEMLIST, dde::SysFormatList, SysReqItems, 0);
		ItemAdd(lpszTopic, SZDDESYS_ITEM_FORMATS, dde::SysFormatList, TopicReqFormats, 0);
	}
	return pTopic;
}

///<summary>Find a topic by its name</summary>
PTOPICINFO dde::TopicFind(LPSTR lpszName)
{
	PTOPICINFO pTopic = ServerInfo.pTopicList;
	
	while (pTopic) 
	{
		if (lstrcmpi(pTopic->pszTopicName, lpszName) == 0) 
			break;
		
		pTopic = pTopic->pNext;
	}
	return pTopic;
}

///<summary>Find a topic by its HSZ</summary>
PTOPICINFO dde::TopicFind(HSZ hszName)
{
	PTOPICINFO pTopic = ServerInfo.pTopicList;
	
	while (pTopic) 
	{
		if (DdeCmpStringHandles(pTopic->hszTopicName, hszName) == 0) 
			break;
		
		pTopic = pTopic->pNext;
	}
	return pTopic;
}

///<summary>
//Remove a topic and all its items.  If there is an active conversation on the topic then disconnect it.
///</summary>
bool dde::TopicRemove(LPSTR lpszTopic)
{
	PTOPICINFO pTopic = ServerInfo.pTopicList;
	PTOPICINFO pPrevTopic = 0;
	PCONVERSATIONINFO pCI;

	// See if we have this topic by walking the list
	while (pTopic) 
	{
		if (lstrcmpi(pTopic->pszTopicName, lpszTopic) == 0) 
		{
			// Found it. Disconnect any active conversations on this topic
			pCI = ConversationFind(pTopic->hszTopicName);
			while (pCI) 
			{
				// Tell DDEML to disconnect it
				DdeDisconnect(pCI->hConv);
			
				// We don't get notified until later that it's gone
				// so remove it from the list now so we won't keep
				// finding it in this loop.
				// When DDEML sends the disconnect notification later
				// it won't be there to remove but that doesn't matter.
				ConversationRemove(pCI->hConv, pCI->hszTopicName);
				pCI = ConversationFind(pTopic->hszTopicName);
			}
			while (pTopic->pCmdList)	// Remove all the execute command handlers in the topic
			{
				if (!ExecCmdRemove(lpszTopic, pTopic->pCmdList->pszCmdName)) // Some error
					return false; 				
			}
			while(pTopic->pItemList)	// Free all the items in the topic
			{
				if (!ItemRemove(lpszTopic, pTopic->pItemList->pszItemName)) // some error
					return false;
			}
			// Unlink it from the list.
			if (pPrevTopic) 
				pPrevTopic->pNext = pTopic->pNext;
			else 
				ServerInfo.pTopicList = pTopic->pNext;
			
			// Release its string handle
			DdeFreeStringHandle(ServerInfo.dwInstance, pTopic->hszTopicName);
		
			// Free the memory associated with it
			delete [] pTopic;		
			return true;
		}
		pTopic = pTopic->pNext;
	}
	// We don't have this topic
	return false;
}

///<summary>Process a request for the list of formats supported for a given topic.</summary>
// Notes:	Typically the data is returned in CF_TEXT format as a tab
//			delimited list of names.  The Format list is the union of all
//			formats supported for all topics of the service.
// Returns: A DDE data handle to the data object containing the return data.
HDDEDATA dde::TopicReqFormats(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
	WORD wFormats[MAXFORMATS];

	PTOPICINFO pTopic = TopicFind(hszTopic);
	
	if (!pTopic)						// No such topic
		return 0;

	wFormats[0] = 0; 					// Start with an empty list

	DdeCreateDataHandle(ServerInfo.dwInstance, 0, 0, 0, hszItem, wFmt, 0); // Empty data object to fill
	PITEMINFO pItem = pTopic->pItemList;

	while (pItem)						// Walk the item list for this topic
	{
		AddFormatsToList(wFormats, MAXFORMATS, pItem->pFormatList); // Add unique formats to the main list
		pItem = pItem->pNext;
	}
	return MakeDataFromFormatList((LPWORD) wFormats, (WORD) wFmt, hszItem);
}