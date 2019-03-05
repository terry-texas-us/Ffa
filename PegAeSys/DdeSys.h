#pragma once

#include "dde.h"

namespace dde
{
	HDDEDATA	SysReqItems(UINT wFmt, HSZ hszTopic, HSZ hszItem);
	HDDEDATA	SysReqFormats(UINT wFmt, HSZ hszTopic, HSZ hszItem);
	HDDEDATA	SysReqHelp(UINT wFmt, HSZ hszTopic, HSZ hszItem);
	HDDEDATA	SysReqProtocols(UINT wFmt, HSZ hszTopic, HSZ hszItem);
	HDDEDATA	SysReqTopics(UINT wFmt, HSZ hszTopic, HSZ hszItem);
}