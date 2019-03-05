#include "stdafx.h"

#include "dde.h"

using namespace dde;

// Add a new item.
PITEMINFO dde::ItemAdd(LPSTR lpszTopic, LPSTR lpszItem, LPWORD lpFormatList, PREQUESTFN lpReqFn, PPOKEFN lpPokeFn)
{
	PITEMINFO pItem = 0;
	PTOPICINFO pTopic = TopicFind(lpszTopic);

	if (!pTopic)						// We need to add this as a new topic
		pTopic = TopicAdd(lpszTopic, 0, 0, 0);

	if (!pTopic) return 0;				// failed
	
	pItem = ItemFind(pTopic, lpszItem);

	if (pItem)
	{	 // Already have this item.  Just update the info in it.
		pItem->pfnRequest = lpReqFn;
		pItem->pfnPoke = lpPokeFn;
		pItem->pFormatList = lpFormatList;
	} 
	else														
	{	// Create a new item
		pItem = (PITEMINFO) new char[sizeof(ITEMINFO)];
		if (!pItem) return 0;

		::ZeroMemory(pItem, sizeof(ITEMINFO));
		pItem->pszItemName = lpszItem;
		pItem->hszItemName = DdeCreateStringHandle(ServerInfo.dwInstance, lpszItem, CP_WINANSI);
		pItem->pTopic = pTopic;
		pItem->pfnRequest = lpReqFn;
		pItem->pfnPoke = lpPokeFn;
		pItem->pFormatList = lpFormatList;
		
		// Add it to the existing item list for this topic
		pItem->pNext = pTopic->pItemList;
		pTopic->pItemList = pItem;
	}
	return pItem;
}

///<summary>Find an item by its name in a topic </summary>
PITEMINFO dde::ItemFind(PTOPICINFO pTopic, LPSTR lpszItem)
{
	PITEMINFO pItem = pTopic->pItemList;
	while (pItem) 
	{
		if (lstrcmpi(pItem->pszItemName, lpszItem) == 0) 
			break;
		
		pItem = pItem->pNext;
	}
	return pItem;
}

///<summary>Find an item by its HSZ in a topic </summary>
PITEMINFO dde::ItemFind(PTOPICINFO pTopic, HSZ hszItem)
{
	PITEMINFO pItem = pTopic->pItemList;
	while (pItem) 
	{
		if (DdeCmpStringHandles(pItem->hszItemName, hszItem) == 0) 
			break;
		
		pItem = pItem->pNext;
	}
	return pItem;
}

// Remove an item from a topic.
bool dde::ItemRemove(LPSTR lpszTopic, LPSTR lpszItem)
{
	PTOPICINFO pTopic = TopicFind(lpszTopic);
	PITEMINFO pItem, pPrevItem;
	
	// See if we have this topic
	if (!pTopic) 
		return false;
	
	// Walk the topic item list looking for this item.
	
	pPrevItem = 0;
	pItem = pTopic->pItemList;
	while (pItem) 
	{
		if (lstrcmpi(pItem->pszItemName, lpszItem) == 0)		// Found it.  Unlink it from the list.
		{
			if (pPrevItem) 
				pPrevItem->pNext = pItem->pNext;
			else 
				pTopic->pItemList = pItem->pNext;
			
			// Release its string handle
			DdeFreeStringHandle(ServerInfo.dwInstance, pItem->hszItemName);

			// Free the memory associated with it
			delete [] pItem;
			return true;
		}
		pPrevItem = pItem;
		pItem = pItem->pNext;
	}
	// We don't have that one
	return false;
}