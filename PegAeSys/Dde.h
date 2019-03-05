#pragma once

// String names for some standard DDE strings not defined in DDEML.H

namespace dde
{
// Some constants
	const WORD MAXFORMATS = 128;		 	// max no of CF formats we will list
	const WORD MAXOPS = 128;				// max no of opcodes we can execute
	const WORD MAXRESULTSIZE = 256;			// largest result string returned

	// Definition for a DDE Request processing function
	typedef HDDEDATA (REQUESTFN)(UINT wFmt, HSZ hszTopic, HSZ hszItem);
	typedef REQUESTFN *PREQUESTFN;
	
	// Definition for a DDE Poke processing function
	typedef bool (POKEFN)(UINT wFmt, HSZ hszTopic, HSZ hszItem, HDDEDATA hData);
	typedef POKEFN *PPOKEFN;

	// Definition for a DDE Execute processing function
	typedef bool (EXECFN)(UINT wFmt, HSZ hszTopic, HDDEDATA hData);
	typedef EXECFN *PEXECFN;

	// Structure used to hold a clipboard id and its text name
	typedef struct _CFTAGNAME 
	{
		WORD	wFmt;
		LPSTR	pszName;
	} CFTAGNAME, *PCFTAGNAME;
	CFTAGNAME CFNames[]; 

	// Definition for a DDE execute command procession function
	typedef bool (EXECCMDFN)(struct _TOPICINFO *pTopic, LPSTR pszResultString, UINT uiResultSize, UINT uiNargs, LPSTR *ppArgs);
	typedef EXECCMDFN *PEXECCMDFN;

	// Structure used to store information on a DDE item
	typedef struct _ITEMINFO
	{
		struct _ITEMINFO 		*pNext;			// pointer to the next item
		LPSTR					pszItemName;	// pointer to its string name
		HSZ 					hszItemName;	// DDE string handle for the name
		struct _TOPICINFO		*pTopic;		// pointer to the topic it belongs to
		LPWORD					pFormatList;	// ptr to null term list of CF format words.
		PREQUESTFN				pfnRequest; 	// pointer to the item specific request processor
		PPOKEFN					pfnPoke;		// pointer to the item specific poke processor
		HDDEDATA				hData;			// data for this item
	} ITEMINFO, *PITEMINFO;

	// Structure used to store information on a DDE execute command processor function
	typedef struct _EXECCMDFNINFO 
	{
		struct _EXECCMDFNINFO	*pNext;			// pointer to the next item
		struct _TOPICINFO		*pTopic;		// pointer to the topic it belongs to
		LPSTR					pszCmdName;		// The name of the command
		PEXECCMDFN				pFn;			// A pointer to the function
		UINT					uiMinArgs;		// min number of args
		UINT					uiMaxArgs;		// max number of args
	} EXECCMDFNINFO, *PEXECCMDFNINFO;

	// Structure used to store information about a DDE conversation
	typedef struct _CONVINFO 
	{
		struct _CONVINFO		*pNext; 			// pointer to the next one
		HCONV					hConv;				// handle to the conversation
		HSZ 					hszTopicName;		// HSZ for the topic of the conversation
		PITEMINFO				pResultItem;		// pointer to a temp result item
	} CONVERSATIONINFO,  *PCONVERSATIONINFO;

	// Structure used to store information on a DDE topic
	typedef struct _TOPICINFO 
	{
		struct _TOPICINFO	 	*pNext; 			// pointer to the next topic
		LPSTR					pszTopicName;		// pointer to its string name
		HSZ 					hszTopicName;		// DDE string handle for the name
		PITEMINFO				pItemList;			// pointer to its item list
		PEXECFN					pfnExec;			// pointer to its DDE Execute processor
		PREQUESTFN				pfnRequest; 		// pointer to the generic request processor
		PPOKEFN					pfnPoke;			// pointer to the generic poke processor
		PEXECCMDFNINFO			pCmdList;			// pointer to the execute command list
	} TOPICINFO,  *PTOPICINFO;

	// Define pointer type used in exec cmd op table

	typedef void *POP;
	typedef POP *PPOP;

	// Structure used to store information on a DDE server which has only one service
	typedef struct _SERVERINFO 
	{
		LPSTR				lpszServiceName;	// pointer to the service string name
		HSZ 				hszServiceName; 	// DDE string handle for the name
		PTOPICINFO			pTopicList; 		// pointer to the topic list
		DWORD				dwInstance;			// DDE Instance value
		PFNCALLBACK 		pfnStdCallback; 	// pointer to standard DDE callback fn
		PFNCALLBACK 		pfnCustomCallback;	// pointer to custom DDE callback fn
		PCONVERSATIONINFO	pConvList;			// pointer to the active conversation list
	} SERVERINFO,  *PSERVERINFO;

	extern SERVERINFO ServerInfo;
	extern WORD SysFormatList[];
	extern WORD MyFormats[];
	extern DWORD dwInstance; 				// DDE Instance value
	
	void				AddCommands(LPSTR);
	void				AddFormatsToList(LPWORD pMain, int iMax, LPWORD pList);
	void				AddSystemTopic(LPSTR, LPWORD);

	bool				ConversationAdd(HCONV hConv, HSZ hszTopic);
	PCONVERSATIONINFO	ConversationFind(HSZ hszTopic);
	bool				ConversationRemove(HCONV hConv, HSZ hszTopic);

	bool				DoCallback(UINT wType, UINT wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, HDDEDATA *phReturnData);
	HDDEDATA			DoWildConnect(HSZ hszTopic);

	PEXECCMDFNINFO		ExecCmdAdd(LPSTR pszTopic, LPSTR pszCmdName, PEXECCMDFN pExecCmdFn, UINT uiMinArgs, UINT uiMaxArgs);
	PEXECCMDFNINFO		ExecCmdFind(PTOPICINFO pTopic, LPSTR lpszCmd);
	bool 				ExecCmdRemove(LPSTR pszTopic, LPSTR pszCmdName);

	PITEMINFO	 		ItemAdd(LPSTR lpszTopic, LPSTR lpszItem, LPWORD pFormatList, PREQUESTFN lpReqFn, PPOKEFN lpPokeFn);
	PITEMINFO			ItemFind(PTOPICINFO pTopic, HSZ hszItem);
	PITEMINFO 			ItemFind(PTOPICINFO pTopic, LPSTR lpszItem);
	bool 				ItemRemove(LPSTR lpszTopic, LPSTR lpszItem);

	PTOPICINFO			TopicAdd(LPSTR lpszTopic, PEXECFN pfnExec, PREQUESTFN pfnRequest, PPOKEFN pfnPoke);
	PTOPICINFO			TopicFind(HSZ hszName);
	PTOPICINFO			TopicFind(LPSTR lpszName);
	bool 				TopicRemove(LPSTR lpszTopic);

	LPSTR				GetCFNameFromId(WORD wFmt, LPSTR lpBuf, int iSize);
	HDDEDATA 			MakeCFText(UINT, LPSTR, HSZ);
	HDDEDATA			MakeDataFromFormatList(LPWORD pFmt, WORD wFmt, HSZ hszItem);
	bool				ParseCmd(LPSTR *ppszCmdLine, PTOPICINFO pTopic, LPSTR pszError, UINT uiErrorSize, PPOP pOpTable, UINT uiNops, LPSTR pArgBuf);
	void 				PostAdvise(PITEMINFO pItemInfo);
	bool				ProcessExecRequest(PTOPICINFO pTopic, HDDEDATA hData);
	PEXECCMDFNINFO		ScanForCommand(PEXECCMDFNINFO pCmdInfo, LPSTR *ppStr);
	void				Setup(HINSTANCE);
	HDDEDATA	WINAPI	StdCallback(UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD);
	HDDEDATA			SysReqResultInfo(UINT wFmt, HSZ hszTopic, HSZ hszItem);
	bool				SysResultExecCmd(PTOPICINFO pTopic, LPSTR pszResult, UINT uiResultSize, UINT uiNargs, LPSTR *ppArgs);
	HDDEDATA			TopicReqFormats(UINT wFmt, HSZ hszTopic, HSZ hszItem);
	void				Uninitialize();
}