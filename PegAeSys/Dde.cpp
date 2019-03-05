#include "stdafx.h"

#include "PegAEsys.h"

#include "dde.h"
#include "ddeCmds.h"
#include "ddeGItms.h"
#include "ddeSys.h"
#include "lex.h"

using namespace dde;

// Standard format name lookup table
// String names for standard Windows Clipboard formats
CFTAGNAME dde::CFNames[] = 
{
	CF_TEXT,		"TEXT",		 
	CF_BITMAP,		"BITMAP",	 
	CF_METAFILEPICT,"METAFILEPICT",
	CF_SYLK,		"SYLK",		 
	CF_DIF, 		"DIF",		 
	CF_TIFF,		"TIFF",		 
	CF_OEMTEXT, 	"OEMTEXT",	 
	CF_DIB, 		"DIB",		 
	CF_PALETTE, 	"PALETTE",	 
	CF_PENDATA, 	"PENDATA",	 
	CF_RIFF,		"RIFF",		 
	CF_WAVE,		"WAVE",		 
	0,			0
};
// Local data 
SERVERINFO dde::ServerInfo;
// Format lists
WORD dde::SysFormatList[] = {CF_TEXT, 0};
WORD dde::MyFormats[] = {CF_TEXT, 0};
// DDE Instance value
DWORD dde::dwInstance = 0;					

///<summary>Startup our DDE services and do the basic initialization.</summary>
void dde::Setup(HINSTANCE)
{
	ServerInfo.pfnStdCallback = (PFNCALLBACK) MakeProcInstance((FARPROC) StdCallback, hInst);
	ServerInfo.pfnCustomCallback = 0;
	
	DWORD dwFilterFlags = CBF_FAIL_SELFCONNECTIONS;
		
	UINT uiResult = DdeInitialize(&ServerInfo.dwInstance, ServerInfo.pfnStdCallback, dwFilterFlags, 0L);
	if (uiResult != DMLERR_NO_ERROR) 
	{
		msgWarning(IDS_MSG_DDE_INIT_FAILURE, "PegAEsys", app.GetSafeHwnd());
		::DestroyWindow(app.GetSafeHwnd());
		return;
	}
	dwInstance = ServerInfo.dwInstance;
	
	ServerInfo.lpszServiceName = "PegAEsys";
	ServerInfo.hszServiceName = DdeCreateStringHandle(ServerInfo.dwInstance, "PegAEsys", CP_WINANSI);

	// Register the name of the service
	DdeNameService(ServerInfo.dwInstance, ServerInfo.hszServiceName, (HSZ) 0, DNS_REGISTER);

	// Add each System!Item pair
	ItemAdd(SZDDESYS_TOPIC, SZDDESYS_ITEM_FORMATS, SysFormatList, SysReqFormats, 0);
	ItemAdd(SZDDESYS_TOPIC, SZDDESYS_ITEM_HELP, SysFormatList, SysReqHelp, 0);
	ItemAdd(SZDDESYS_TOPIC, SZDDESYS_ITEM_SYSITEMS, SysFormatList, SysReqItems, 0);
	ItemAdd(SZDDESYS_TOPIC, SZDDE_ITEM_ITEMLIST, SysFormatList, SysReqItems, 0);
	ItemAdd(SZDDESYS_TOPIC, SZDDESYS_ITEM_TOPICS, SysFormatList, SysReqTopics, 0);
	
	ItemAdd(SZDDESYS_TOPIC, "Protocols", SysFormatList, SysReqProtocols, 0);

//	ExecCmdAdd(SZDDESYS_TOPIC, "TracingOpen", ExecTracingOpen, 1, 2);
	ExecCmdAdd(SZDDESYS_TOPIC, "TracingMap", ExecTracingMap, 1, 2);
	ExecCmdAdd(SZDDESYS_TOPIC, "TracingView", ExecTracingView, 1, 2);

	// Add each General!Item pair
	DimAngZInfo = ItemAdd("General", "DimAngZ", MyFormats, DimAngZRequest, DimAngZPoke);
	DimLenInfo  = ItemAdd("General", "DimLen",   MyFormats, DimLenRequest, DimLenPoke);
	EngAngZInfo = ItemAdd("General", "EngAngZ", MyFormats, EngAngZRequest, 0);
	EngLenInfo  = ItemAdd("General", "EngLen", MyFormats, EngLenRequest, 0);
	ExtNumInfo  = ItemAdd("General", "ExtNum", MyFormats, ExtNumRequest, 0);
	ExtStrInfo  = ItemAdd("General", "ExtStr", MyFormats, ExtStrRequest, 0);
	RelPosZInfo = ItemAdd("General", "RelPosZ", MyFormats, RelPosZRequest, 0);
	RelPosYInfo = ItemAdd("General", "RelPosY", MyFormats, RelPosYRequest, 0);
	RelPosXInfo = ItemAdd("General", "RelPosX", MyFormats, RelPosXRequest, 0);
	ScaleInfo = ItemAdd("General", "Scale", MyFormats, ScaleRequest, ScalePoke);
	
	// Add Topic for command execute connections
	TopicAdd("Commands", 0, 0, 0);
	
	// Add each Command!Item pair
	ExecCmdAdd("Commands", "TracingBlank", ExecTracingBlank, 1, 2);
	ExecCmdAdd("Commands", "TracingMap", ExecTracingMap, 1, 2);
//	ExecCmdAdd("Commands", "TracingOpen", ExecTracingOpen, 1, 2);
	ExecCmdAdd("Commands", "TracingView", ExecTracingView, 1, 2);
	ExecCmdAdd("Commands", "FileGet", ExecFileGet, 1, 2);
	ExecCmdAdd("Commands", "GotoPoint", ExecGotoPoint, 1, 1);
	ExecCmdAdd("Commands", "Line", ExecLine, 1, 1);
	ExecCmdAdd("Commands", "Pen", ExecPen, 1, 1);
	ExecCmdAdd("Commands", "Note", ExecNote, 1, 1);
	ExecCmdAdd("Commands", "Send", ExecSend, 1, 1);
	ExecCmdAdd("Commands", "SetPoint", ExecSetPoint, 1, 1);
	ExecCmdAdd("Commands", "DimAngZ", ExecDA, 1, 1);
	ExecCmdAdd("Commands", "DimLen", ExecDL, 1, 1);
	ExecCmdAdd("Commands", "Scale", ExecScale, 1, 1);
	ExecCmdAdd("Commands", "Fill", ExecFill, 1, 1);
	ExecCmdAdd("Commands", "NoteHT", ExecNoteHT, 1, 1);

	return;
}
///<summary>Tidy up and close down DDEML.</summary>
void dde::Uninitialize()
{
	PITEMINFO pItem;

	// Unregister the service name
	DdeNameService(ServerInfo.dwInstance, ServerInfo.hszServiceName, 0, DNS_UNREGISTER);
	// Free the name handle
	DdeFreeStringHandle(ServerInfo.dwInstance, ServerInfo.hszServiceName);

	// Walk the server topic tree, freeing all the other string handles
	PTOPICINFO pTopic = ServerInfo.pTopicList;
	while (pTopic) 
	{
		DdeFreeStringHandle(ServerInfo.dwInstance, pTopic->hszTopicName);
		pTopic->hszTopicName = 0;
		
		// Free any item handles owned by topic
		pItem = pTopic->pItemList;
		while (pItem) 
		{
			DdeFreeStringHandle(ServerInfo.dwInstance, pItem->hszItemName);
			pItem->hszItemName = 0;
		
			pItem = pItem->pNext;
		}
		pTopic = pTopic->pNext;
	}
	TopicRemove("Commands");
	TopicRemove("General");
	TopicRemove(SZDDESYS_TOPIC);
	
	// Release DDEML
	DdeUninitialize(ServerInfo.dwInstance);					
	ServerInfo.dwInstance = 0;

	// Free any proc instances we created
	if (ServerInfo.pfnCustomCallback) 
	{
		ServerInfo.pfnCustomCallback = 0;
	}
	ServerInfo.pfnStdCallback = 0;
}
// DDE callback function called from DDEML
HDDEDATA WINAPI dde::StdCallback(UINT wType, UINT wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2)
{
	HDDEDATA hDdeData = 0;

	switch (wType) 
	{
		case XTYP_CONNECT_CONFIRM:			// Confirm that a conversation has been established with a
											// client and provide the server with the conversation handle
			ConversationAdd(hConv, hsz1);
			break;

		case XTYP_DISCONNECT:				// Remove a conversation from the list
			ConversationRemove(hConv, hsz1);
			break;

		case XTYP_WILDCONNECT:				// Sent when service name and/or topic name is NULL
			if ((hsz2 == NULL) || !DdeCmpStringHandles(hsz2, ServerInfo.hszServiceName))
				return (DoWildConnect(hsz1));
			break;

		// For all other messages we see if we want them here and if not, 
		// they get passed on to the user callback if one is defined.

		case XTYP_ADVSTART:
		case XTYP_CONNECT:
		case XTYP_EXECUTE:
		case XTYP_REQUEST:
		case XTYP_ADVREQ:
		case XTYP_ADVDATA:
		case XTYP_POKE:

		// Try and process them here first.
			if (DoCallback(wType, wFmt, hConv, hsz1, hsz2, hData, &hDdeData)) 
				return hDdeData;

		// Fall Through to allow the custom callback a chance
		default:
			if (ServerInfo.pfnCustomCallback != 0) 
				return(ServerInfo.pfnCustomCallback(wType, wFmt, hConv, hsz1, hsz2, hData, dwData1, dwData2));
	}
	return (HDDEDATA) 0;
}
//<summary>Process a generic callback.</summary>
bool dde::DoCallback(UINT wType, UINT wFmt, HCONV hConv, HSZ hszTopic, HSZ hszItem, HDDEDATA hData, HDDEDATA *phReturnData)
{
	PTOPICINFO pTopic = TopicFind(hszTopic);

	if (!pTopic)						// Don't know the topic
		return false;
	
	if (wType == XTYP_EXECUTE)									
	{	// Execute request for the topic
		if (pTopic->pfnExec)									
		{	// User supplied a function to handle this
			if ((*pTopic->pfnExec)(wFmt, hszTopic, hData))		
			{	// Call the exec function to process it
				*phReturnData = (HDDEDATA) (DWORD) DDE_FACK;
				return true;
			}
		}
		else if (pTopic->pCmdList)		
		{	// Try to parse and execute the request
			if (ProcessExecRequest(pTopic, hData))
			{
				*phReturnData = (HDDEDATA) (DWORD) DDE_FACK;
				return true;
			}
		}
		else
		{
			char sz[32];
			DdeGetData(hData, (LPBYTE) sz, (DWORD) sizeof(sz), (DWORD) 0);
			::PostMessage(app.GetSafeHwnd(), WM_CHAR, (WPARAM) sz[0], (LPARAM) 1); 
			*phReturnData = (HDDEDATA) (DWORD) DDE_FACK;
			return true;
		}		 
		*phReturnData = (HDDEDATA) DDE_FNOTPROCESSED;			// Either no function or it didn't get handled by the function
		return true;
	}
	if (wType == XTYP_CONNECT)
	{	// Connect request.  
		*phReturnData = (HDDEDATA) (DWORD) TRUE;				// Conversation established
		return true;
	}
	// For any other transaction we need to be sure this is an item we support and in some cases,
	// that the format requested is supported for that item.
	
	PITEMINFO pItem = ItemFind(pTopic, hszItem);
	if (!pItem) 												// Not an item we support
		return false;
	
	// See if this is a supported format
	
	LPWORD pFormat = pItem->pFormatList;
	while (*pFormat) 
	{
		if (*pFormat == wFmt) 
			break;
		pFormat++;
	}
	if (! *pFormat) 		// not a format we support
		return false;

	PPOKEFN		pfnPoke;
	CONVINFO		ci;
	PREQUESTFN	pfnRequest;
	
	switch (wType)			// Do whatever is required for each specific transaction
	{
		case XTYP_ADVSTART: // Start an advise request.  Topic/item and format are ok.
			*phReturnData = (HDDEDATA) (DWORD) TRUE;
			break;

		case XTYP_POKE: 	// We did the data change ourself.
		case XTYP_ADVDATA:	// It came from elsewhere. 
			*phReturnData = (HDDEDATA) DDE_FNOTPROCESSED;
			pfnPoke = pItem->pfnPoke;
			if (!pfnPoke)				// No poke function for this item
				pfnPoke = pTopic->pfnPoke;	// Use poke function for this topic in general.
			if (pfnPoke) 
			{
				if ((*pfnPoke)(wFmt, hszTopic, hszItem, hData)) // Data at the server has changed.
				{
					ci.cb = sizeof(CONVINFO);
					if (DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, &ci)) 
					{
						if (! (ci.wStatus & ST_ISSELF)) 		// It came from elsewhere (not a poke)
							DdePostAdvise(ServerInfo.dwInstance, hszTopic, hszItem); // Advise ourself of the change.
					}
					*phReturnData = (HDDEDATA) (DWORD) DDE_FACK; // Say we took it
				}
			}
			break;

		case XTYP_ADVREQ:
		case XTYP_REQUEST:	// Attempt to start an advise or get the data on a topic/item
							// See if we have a request function for this item or
							// a generic one for the topic
			pfnRequest = pItem->pfnRequest;
			if (!pfnRequest) 
				pfnRequest = pTopic->pfnRequest;
			if (pfnRequest) 
				*phReturnData = (*pfnRequest)(wFmt, hszTopic, hszItem);
			else 
				*phReturnData = (HDDEDATA) 0;
			break;

		default:
			break;
	}
	// Say we processed the transaction in some way
	return true;
}
///<summary>Add a list of formats to main list ensuring that each item only exists in the list once.</summary>
void dde::AddFormatsToList(LPWORD pMain, int iMax, LPWORD pList)
{
	LPWORD pFmt;
	
	if (!pMain || !pList) 
		return;

	int iCount = 0;
	LPWORD pLast = pMain;
	
	while (*pLast)						// Count what we have to start with
	{
		pLast++;
		iCount++;
	}
	while ((iCount < iMax) && *pList)	// Walk the new list adding unique items if there is room
	{
		pFmt = pMain;
		while (*pFmt) 
		{
			if (*pFmt == *pList)		// Already got this one
				goto next_fmt; 
			
			pFmt++;
		}
		*pLast++ = *pList;				// Put it on the end of the list
		iCount++;

next_fmt:
		pList++;
	}
	*pLast = 0;							// Stick a null on the end to terminate the list
}
///<summary>
///Process a wild connect request. Since we only support one service, this is much simpler.  
///If hszTopic is 0 we supply a list of all the topics we currently support.  If it's not 0, 
///we supply a list of topics (zero or one items) which match the requested topic. 
///The list is terminated by a 0 entry.
///</summary>
HDDEDATA dde::DoWildConnect(HSZ hszTopic)
{
	PTOPICINFO pTopic;
	HDDEDATA hData;
	PHSZPAIR pHszPair;
	
	int iTopics = 0;
	
	if (hszTopic == 0)					// Count all the topics we have
	{
		pTopic = ServerInfo.pTopicList;
		while (pTopic)
		{
			iTopics++;
			pTopic = pTopic->pNext;
		}

	} 
	else								// Look for specific topic
	{
		pTopic = ServerInfo.pTopicList;
		while (pTopic) 
		{
			if (DdeCmpStringHandles(pTopic->hszTopicName, hszTopic) == 0) 
			{
				iTopics++;
				break;
			}
			pTopic = pTopic->pNext;
		}
	}
	if (!iTopics)						// No match or no topics at all. No Connect.
		return (HDDEDATA) 0;

	// Big enough for all the HSZPAIRS we'll be sending back plus space for a 0 entry on the end
	hData = DdeCreateDataHandle(ServerInfo.dwInstance, 0, (iTopics + 1) * sizeof(HSZPAIR), 0, 0, 0, 0);

	if (!hData) 						// Failed to create mem object!
		return (HDDEDATA) 0;

	pHszPair = (PHSZPAIR) DdeAccessData(hData, 0);
	if (hszTopic == 0)		// all the topics (includes the system topic)
	{
		pTopic = ServerInfo.pTopicList;
		while (pTopic) 
		{
			pHszPair->hszSvc = ServerInfo.hszServiceName;
			pHszPair->hszTopic = pTopic->hszTopicName;
			pHszPair++;
			pTopic = pTopic->pNext;
		}
	} 
	else						// the one topic asked for
	{
		pHszPair->hszSvc = ServerInfo.hszServiceName;
		pHszPair->hszTopic = hszTopic;
		pHszPair++;
	}
	pHszPair->hszSvc = 0;				// Terminator on the end
	pHszPair->hszTopic = 0;
	DdeUnaccessData(hData);

	return hData;
}
PEXECCMDFNINFO dde::ExecCmdAdd(LPSTR pszTopic, LPSTR pszCmdName, PEXECCMDFN pExecCmdFn, UINT uiMinArgs, UINT uiMaxArgs)
{
	PEXECCMDFNINFO pCmd = 0;
	PEXECCMDFNINFO pHead;
	
	PTOPICINFO pTopic = TopicFind(pszTopic);

	if (!pTopic)						// Do Not already have this topic.	We need to add this as a new topic
		pTopic = TopicAdd(pszTopic, 0, 0, 0);
	
	if (!pTopic) return 0;				// failed

	pCmd = ExecCmdFind(pTopic, pszCmdName);

	if (pCmd)
	{	// Already have this command.  Just update the info in it
		pCmd->pFn = pExecCmdFn;
		pCmd->uiMinArgs = uiMinArgs;
		pCmd->uiMaxArgs = uiMaxArgs;
	} 
	else														
	{	// New command item
		pCmd = (PEXECCMDFNINFO) new char[sizeof(EXECCMDFNINFO)];
		if (!pCmd) return 0;
		
		::ZeroMemory(pCmd, sizeof(EXECCMDFNINFO));
		pCmd->pszCmdName = pszCmdName;
		pCmd->pTopic = pTopic;
		pCmd->pFn = pExecCmdFn;
		pCmd->uiMinArgs = uiMinArgs;
		pCmd->uiMaxArgs = uiMaxArgs;
		// Add it to the existing cmd list for this topic
		pHead = pTopic->pCmdList;
		pCmd->pNext = pTopic->pCmdList;
		pTopic->pCmdList = pCmd;

		// If this was the first command added to the list, add the 'Result' command too.
		// This supports the Execute Control 1 protocol.

		ExecCmdAdd(pszTopic, "Result", SysResultExecCmd, 1, 1);
	}
	return pCmd;
}
///<summary>Find a DDE execute command from its string name.</summary>
PEXECCMDFNINFO dde::ExecCmdFind(PTOPICINFO pTopic, LPSTR lpszCmd)
{
	PEXECCMDFNINFO pCmd = pTopic->pCmdList;
	
	while (pCmd)
	{
		if (lstrcmpi(pCmd->pszCmdName, lpszCmd) == 0)
			break;

		pCmd = pCmd->pNext;
	}
	return pCmd;
}		
bool dde::ExecCmdRemove(LPSTR pszTopic, LPSTR pszCmdName)
{
	PTOPICINFO pTopic = TopicFind(pszTopic);

	if (!pTopic)						// See if we have this topic
		return false;
	
	PEXECCMDFNINFO pPrevCmd = 0;
	PEXECCMDFNINFO pCmd = pTopic->pCmdList;
	
	while (pCmd)												
	{	// Walk the topic item list looking for this cmd.
		if (lstrcmpi(pCmd->pszCmdName, pszCmdName) == 0)		
		{	// Found it.  Unlink it from the list.
			if (pPrevCmd) 
				pPrevCmd->pNext = pCmd->pNext;
			else 
				pTopic->pCmdList = pCmd->pNext;

			// Free the memory associated with it
			delete [] pCmd;
			return true;
		}
		pPrevCmd = pCmd;
		pCmd = pCmd->pNext;
	}
	return false;					// We don't have that one
}
///<summary>Get the text name of a Clipboard format from its id</summary>
LPSTR dde::GetCFNameFromId(WORD wFmt, LPSTR lpBuf, int iSize)
{
	PCFTAGNAME pCTN = CFNames;

	// Try for a standard one first
	while (pCTN->wFmt) 
	{
		if (pCTN->wFmt == wFmt) 
		{
			strncpy(lpBuf, pCTN->pszName, iSize);
			return lpBuf;
		}
		pCTN++;
	}
	if (GetClipboardFormatName(wFmt, lpBuf, iSize) == 0)		// It's unknown (not registered)
		*lpBuf = '\0';
	
	return lpBuf;
}

// Return a string in CF_TEXT format
HDDEDATA dde::MakeCFText(UINT wFmt, LPSTR lpszStr, HSZ hszItem)
{
	if (wFmt != CF_TEXT) 
		return 0;

	return (DdeCreateDataHandle(dwInstance, (LPBYTE) lpszStr, lstrlen(lpszStr) + 1, 0, hszItem, CF_TEXT, 0));
}
///<summary>Create a data item containing the names of all the formats supplied in a list.</summary>
// Returns: A DDE data handle to a list of the format names.
HDDEDATA dde::MakeDataFromFormatList(LPWORD pFmt, WORD wFmt, HSZ hszItem)
{
	int 	cb;
	char	buf[256];

	HDDEDATA hData = DdeCreateDataHandle(ServerInfo.dwInstance, 0, 0, 0, hszItem, wFmt, 0); // Empty data object to fill
	int cbOffset = 0;
	
	while (*pFmt)						// Walk the format list
	{
		if (cbOffset != 0)				// Put in a tab delimiter 
		{
			DdeAddData(hData, (LPBYTE) "\t", 1, cbOffset);
			cbOffset++;
		}
		GetCFNameFromId(*pFmt, buf, sizeof(buf));		// the string name of the format
		cb = lstrlen(buf);
		DdeAddData(hData, (LPBYTE) buf, cb, cbOffset);
		cbOffset += cb;
		pFmt++;
	}
	DdeAddData(hData, (LPBYTE) "", 1, cbOffset);							// Put a 0 on the end
	return hData;
}
///<summary>Post an advise request about an item</summary>
void dde::PostAdvise(PITEMINFO pItemInfo)
{
	if (pItemInfo && pItemInfo->pTopic) 
		DdePostAdvise(ServerInfo.dwInstance, pItemInfo->pTopic->hszTopicName, pItemInfo->hszItemName);
}
// DDE Execute command parser

///<summary>Process a DDE execute command line.</summary>
// Notes:	Support for the 'Execute Control 1' protocol is provided allowing
//			return information to be sent back to the caller.
// Returns: true if no errors occur in parsing or executing the commands.  
//			false if any error occurs.
bool dde::ProcessExecRequest(PTOPICINFO pTopic, HDDEDATA hData)
{
	bool	bResult = false;
	POP 	OpTable[MAXOPS];
	PPOP	ppOp, ppArg;
	UINT	uiNargs;
	LPSTR	pArgBuf = 0;
	PCONVERSATIONINFO pCI;
	char	szResult[MAXRESULTSIZE];

	if (!hData) return false;
	
	LPSTR pData = (LPSTR) DdeAccessData(hData, 0);
	
	if (!pData) return false;
	
	// Allocate double required size we might need so we can avoid doing any space tests.
	pArgBuf = (LPSTR) new char[2 * strlen(pData)];
	if (!pArgBuf) goto PER_exit;
	
	::ZeroMemory(pArgBuf, 2 * strlen(pData));
	pCI = ConversationFind(pTopic->hszTopicName);	// Get a pointer to the current conversation
	
	while (pData && *pData) 							
	{	// Parse and execute each command in turn
		szResult[0] = '\0';
		bResult = ParseCmd(&pData, pTopic, szResult, sizeof(szResult), OpTable, MAXOPS, pArgBuf); // Parse a single command
	
		if (!bResult) 
		{
			if (pCI && pCI->pResultItem)		
				// Current conversation has a results item to pass the error string back in
				pCI->pResultItem->hData = DdeCreateDataHandle(ServerInfo.dwInstance, (LPBYTE) szResult, lstrlen(szResult)+1, 0, pCI->pResultItem->hszItemName, CF_TEXT, 0);
			
			goto PER_exit;
		}
		ppOp = OpTable;

		while (*ppOp)					
		{	// Execute the op list
			uiNargs = 0;				
			ppArg = ppOp + 1;
			while (*ppArg) 
			{	// Count the number of args
				uiNargs++;
				ppArg++;
			}
			ppArg = ppOp + 1;			// Call the function, passing the address of the first arg
			szResult[0] = '\0';
			bResult = (*((PEXECCMDFN)*ppOp))(pTopic, szResult, sizeof(szResult), uiNargs, (LPSTR *)ppArg);
			
			if (pCI && pCI->pResultItem)
				// Current conversation has a results item to pass the result string back in
				pCI->pResultItem->hData = DdeCreateDataHandle(ServerInfo.dwInstance, (LPBYTE) szResult, lstrlen(szResult)+1, 0, pCI->pResultItem->hszItemName, CF_TEXT, 0);
			
			if (!bResult) 
				goto PER_exit;
			
			while (*ppOp)				// move on to the next function
				ppOp++;
			ppOp++;
		}
	}
	bResult = true; 					// if we get this far we're done

PER_exit:

	DdeUnaccessData(hData);
	if (pArgBuf)
		delete [] pArgBuf;

	return bResult;
}
///<summary>Parses a single command.</summary>
// Notes:	Error information may be set in the error return buffer.
// Returns: true if there are no errors, else it is false.
//	ppszCmdLine	Pointer to a a pointer which addresses the command line to be parsed.
//	pTopic		Pointer to the topic info structure.
//	pszError	Pointer to a buffer to receive the error string.
//	uiErrorSize	Size of the error return buffer.
//	pOpTable	Pointer to a table in which the operator and operands are to be inserted.
//				The size of the op table.
//	pArgBuf		Pointer to a buffer in which the arguments are to be constructed.
bool dde::ParseCmd(LPSTR *ppszCmdLine, PTOPICINFO pTopic, LPSTR pszError, UINT uiErrorSize, PPOP pOpTable, UINT , LPSTR pArgBuf)
{
	LPSTR	pArg;
	PPOP	ppOp = pOpTable;
	PEXECCMDFNINFO pExecFnInfo;
	UINT	uiNargs;
	char	cTerm;

	*ppOp = 0;
	LPSTR pCmd = lex::SkipWhiteSpace(*ppszCmdLine);
	
	if (!lex::ScanForChar('[', &pCmd))
	{	// Scan for a command leading
		strncpy(pszError, "Missing '['", uiErrorSize - 1);
		return false;
	}
	
	pExecFnInfo = ScanForCommand(pTopic->pCmdList, &pCmd);
	if (!pExecFnInfo)				
	{	// Not a valid command
		strncpy(pszError, "Invalid Command", uiErrorSize - 1);
		return false;
	}
	*ppOp++ = pExecFnInfo->pFn; 		// Add the function pointer to the opcode list
	
	uiNargs = 0;
	if (lex::ScanForChar('(', &pCmd))		
	{	// Command has arguments
		do
		{	// each argument to the op list
			pArg = lex::ScanForString(&pCmd, &cTerm, &pArgBuf);
			if (pArg) 
			{
				*ppOp++ = pArg;
				uiNargs++;
			}
		} while (cTerm == ',');

		if ((cTerm != ')') && (!lex::ScanForChar(')', &pCmd)))
		{	// Do not have a terminating ) char
			strncpy(pszError, "Missing ')'", uiErrorSize - 1);
			return false;
		}
	}	
	if (!lex::ScanForChar(']', &pCmd))
	{	// Do not have a terminating ] char
		strncpy(pszError, "Missing ']'", uiErrorSize - 1);
		return false;
	}
	if (uiNargs < pExecFnInfo->uiMinArgs) 
	{
		strncpy(pszError, "Too few arguments", uiErrorSize - 1);
		return false;
	}
	if (uiNargs > pExecFnInfo->uiMaxArgs) 
	{
		strncpy(pszError, "Too many arguments", uiErrorSize - 1);
		return false;
	}
	*ppOp++ = 0; 					// Terminate this op list with a 0
	pCmd = lex::SkipWhiteSpace(pCmd);
	*ppOp = 0;						// Put a final 0 on the op list
	*ppszCmdLine = pCmd;				// Update the buffer pointer
	
	return true;
}
///<summary>Process a DDE Execute 'Result' command.</summary>
// Notes:	This command creates a temporary item under the current topic
//			which will contain the result of the next command to be executed.
// Returns: true if the command executes with no errors, otherwise it is false.
//	pTopic	Pointer to a topic info structure.
//			Pointer the the buffer to receive the result string.
//			Size of the return buffer.
//			Number of arguments in the argument list.
//	ppArgs	A list of pointers to the arguments.
bool dde::SysResultExecCmd(PTOPICINFO pTopic, LPSTR , UINT , UINT , LPSTR *ppArgs)
{
	PCONVERSATIONINFO pCI = ConversationFind(pTopic->hszTopicName);
	
	if (!pCI) 
		return false; // internal error
	
	if (pCI->pResultItem)				// Already have a temporary result item. Get rid of it
		ItemRemove(pTopic->pszTopicName, pCI->pResultItem->pszItemName);
	
	pCI->pResultItem = ItemAdd(pTopic->pszTopicName, ppArgs[0], SysFormatList, SysReqResultInfo, 0); // Add a new temporary result item to the current conversation
	
	return true;
}
///<summary>Return the 'result' info for a given item and delete the item.</summary>
// Notes:	The item is deleted after the data is returned.
// Returns: A DDE data handle to an object containing the return string.
HDDEDATA dde::SysReqResultInfo(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
	PTOPICINFO	pTopic = TopicFind(hszTopic);
	PITEMINFO	pItem = ItemFind(pTopic, hszItem);
	HDDEDATA		hData = pItem->hData;

	if (!hData) 						// No data to return. Send back an empty string
		hData = DdeCreateDataHandle(ServerInfo.dwInstance, (LPBYTE) "", 1, 0, hszItem, CF_TEXT, wFmt);
	
	ItemRemove(pTopic->pszTopicName, pItem->pszItemName);

	return hData;
}
///<summary>Scan for a valid comamnd.</summary>
// Notes:	If found, the scan pointer is updated.
// Returns: Pointer to the command info if found, 0 if not.
//	pCmdInfo	Pointer to the current command list.
//	ppStr		Pointer to the current scan pointer.
PEXECCMDFNINFO dde::ScanForCommand(PEXECCMDFNINFO pCmdInfo, LPSTR *ppStr)			
{
	LPSTR pStart = lex::SkipWhiteSpace(*ppStr);
	LPSTR p = pStart;
	
	if (!isalpha(*p))					// First char is not alpha
		return 0;
	
	while (isalnum(*p)) 				// Collect alpha-num chars until we get to a non-alpha.
		p++;

	char cSave = *p;					// Terminate the source temporarily with a null
	*p = '\0';

	while (pCmdInfo)					// Search for a command that matches the name we have
	{
		if (stricmp(pStart, pCmdInfo->pszCmdName) == 0) 	// Found it, so restore the delimter and return the info pointer
		{
			*p = cSave;
			*ppStr = p;
			return pCmdInfo;
		}
		pCmdInfo = pCmdInfo->pNext;
	}
	*p = cSave; 						// Didn't find it, so restore delimiter and return
	return 0;							// not found
}