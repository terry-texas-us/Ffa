#pragma once

#include "dde.h"

// prototypes for handling procedures

namespace dde
{
	bool ExecFileGet(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecGotoPoint(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecLine(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecNote(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecPen(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecSend(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecSetPoint(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecDA(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecDL(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecScale(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecFill(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecNoteHT(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecTracingBlank(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecTracingMap(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
//	bool ExecTracingOpen(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
	bool ExecTracingView(PTOPICINFO, LPSTR, UINT, UINT, LPSTR *);
}