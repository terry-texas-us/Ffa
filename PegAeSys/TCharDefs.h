// Here are collected definitions of output streams used in examples wide/narrow streams usage is controlled by _UNICODE symbol

#ifndef _tcout
#if defined(_UNICODE)
#define _tcout wcout
#else
#define _tcout cout
#endif
#endif

#ifndef _tostream
#if defined(_UNICODE)
#define _tostream wostream
#else
#define _tostream ostream
#endif
#endif

#ifndef TCHAR
#if defined(_UNICODE)
#define TCHAR wchar_t
#else
#define TCHAR char
#endif
#endif

#ifndef _T
#define _T(x) __T(x)
#if defined(_UNICODE)
#define __T(x) L ## x
#else
#define __T(x) x
#endif
#endif