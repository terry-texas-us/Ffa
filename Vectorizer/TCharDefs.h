// Here are collected definitions of output streams used in examples
// wide/narrow streams usage is controlled by _UNICODE symbol
// MinGW currently does not support wide streams


#if defined(__BORLANDC__) && (__BORLANDC__ < 0x580) && defined(_UNICODE)
#define STORED_UNICODE
#undef _UNICODE
#undef TCHAR
#undef _T
#undef __T
#endif

#ifndef _tcout
#if defined(_UNICODE) && !defined(__MINGW32__)
#define _tcout wcout
#else
#define _tcout cout
#endif
#endif

#ifndef _tostream
#if defined(_UNICODE) && !defined(__MINGW32__)
#define _tostream wostream
#else
#define _tostream ostream
#endif
#endif

#ifndef TCHAR
#if defined(_UNICODE) && !defined(__MINGW32__)
#define TCHAR wchar_t
#else
#define TCHAR char
#endif
#endif

#ifndef _T
#define _T(x) __T(x)
#if defined(_UNICODE) && !defined(__MINGW32__)
#define __T(x) L ## x
#else
#define __T(x) x
#endif
#endif

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x580) && defined(STORED_UNICODE)
#define _UNICODE 
#undef STORED_UNICODE
#endif
