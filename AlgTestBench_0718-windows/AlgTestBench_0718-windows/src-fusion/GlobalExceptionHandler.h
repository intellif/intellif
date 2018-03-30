// GlobalExceptionHandler.h: interface for the CGlobalExceptionHandler class.
// Version : 1.1
// Modified by ma.zhihong
//		Add module list to XML file.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALEXCEPTIONHANDLER_H__D196F576_E4A7_46EF_9DEF_FE575518C909__INCLUDED_)
#define AFX_GLOBALEXCEPTIONHANDLER_H__D196F576_E4A7_46EF_9DEF_FE575518C909__INCLUDED_

#ifdef WIN32

#include <windows.h>
#include <atlstr.h>

// Import MSXML interfaces

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGlobalExceptionHandler  
{
public:
	CGlobalExceptionHandler();
	virtual ~CGlobalExceptionHandler();
public:
private:
	// Variables used by the class
    static LPTOP_LEVEL_EXCEPTION_FILTER m_lpPreviousFilter;
private:
	static LONG Dump( CString strFilePrefix, PEXCEPTION_POINTERS pExceptionInfo );
	// entry point where control comes on an unhandled exception
    static LONG WINAPI CustomUnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo );
};

#endif// WIN32

#endif // !defined(AFX_GLOBALEXCEPTIONHANDLER_H__D196F576_E4A7_46EF_9DEF_FE575518C909__INCLUDED_)
