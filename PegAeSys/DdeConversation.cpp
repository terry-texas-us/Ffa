#include "stdafx.h"

#include "dde.h"

using namespace dde;

//<summary>Add a conversation to our list.</summary>
bool dde::ConversationAdd(HCONV hConv, HSZ hszTopic)
{
	PCONVERSATIONINFO pCI = (PCONVERSATIONINFO) new char[sizeof(CONVERSATIONINFO)];	
	if (!pCI) return false;
	
	::ZeroMemory(pCI, sizeof(CONVERSATIONINFO));
	pCI->hConv = hConv;
	pCI->hszTopicName = hszTopic;
	pCI->pNext = ServerInfo.pConvList;
	ServerInfo.pConvList = pCI;
	return true;
}

///<summary>Find the first occurence of a topic conversation in our list</summary>
PCONVERSATIONINFO dde::ConversationFind(HSZ hszTopic)
{
	PCONVERSATIONINFO pCI = ServerInfo.pConvList;

	while (pCI) 						// Try to find the info in the list
	{
		if (DdeCmpStringHandles(pCI->hszTopicName, hszTopic) == 0) 
			return pCI;
		
		pCI = pCI->pNext;
	}
	return 0;
}

//<summary>Remove a conversation from our list.</summary>
bool dde::ConversationRemove(HCONV hConv, HSZ hszTopic)
{
	PCONVERSATIONINFO pCI = ServerInfo.pConvList;
	PCONVERSATIONINFO pPrevCI = 0;
	
	// Try to find the info in the list
	while (pCI)
	{
		if ((pCI->hConv == hConv) && (DdeCmpStringHandles(pCI->hszTopicName, hszTopic) == 0))
		{	 // Found it. Unlink it from the list
			if (pPrevCI) 
				pPrevCI->pNext = pCI->pNext;
			else 
				ServerInfo.pConvList = pCI->pNext;
			delete [] pCI;			  
			return true;
		}
		pPrevCI = pCI;
		pCI = pCI->pNext;
	}
	return false;			// Not in the list
}