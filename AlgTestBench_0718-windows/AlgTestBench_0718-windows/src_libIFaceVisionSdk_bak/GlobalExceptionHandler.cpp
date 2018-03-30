// GlobalExceptionHandler.cpp: implementation of the CGlobalExceptionHandler class.
// Thanks to : Michael Carruth
//////////////////////////////////////////////////////////////////////

#include "GlobalExceptionHandler.h"
#ifdef WIN32
#include <TLHELP32.H>
#include <ATLComTime.h>
#include <crtdbg.h>

#ifndef __out_bcount_opt
#define __out_bcount_opt( x )
#endif// __out_bcount_opt

#ifndef __in_bcount_opt
#define __in_bcount_opt( x )
#endif// __in_bcount_opt

#ifndef __deref_out_opt
#define __deref_out_opt
#endif// __deref_out_opt


#include <atlbase.h>
#include "dbghelp.h"
#pragma comment( lib, "dbghelp" )
#pragma comment( lib, "version" )

#ifndef ASSERT
#define ASSERT _ASSERT
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//
// COM helper macros
//
#define CheckHResult(x) do {HRESULT hr = x; if (FAILED(hr)) goto CleanUp;} while( 0 )
#define SafeRelease(p) do {if (p) {(p)->Release(); p = NULL;}} while ( 0 )


LPTOP_LEVEL_EXCEPTION_FILTER CGlobalExceptionHandler::m_lpPreviousFilter = NULL;

/*
 * The global instance changes the global exception handler
 *	automatically when the program starts.
 */
static CGlobalExceptionHandler local_instance;// Global instance.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalExceptionHandler::CGlobalExceptionHandler()
{
	SetErrorMode( SEM_NOGPFAULTERRORBOX );
	
    // Install the unhandled exception filter function
    m_lpPreviousFilter = SetUnhandledExceptionFilter( CGlobalExceptionHandler::CustomUnhandledExceptionFilter );
}


CGlobalExceptionHandler::~CGlobalExceptionHandler()
{
	SetUnhandledExceptionFilter( m_lpPreviousFilter );
}

LONG CGlobalExceptionHandler::CustomUnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo )
{
	LONG nReturn = EXCEPTION_CONTINUE_SEARCH;
	
	TCHAR strAppPathname[ MAX_PATH ];
	GetModuleFileName( NULL, strAppPathname, MAX_PATH );
	
	CString strFilePrefix = strAppPathname;
	COleDateTime tmNow = COleDateTime::GetCurrentTime();
	strFilePrefix += _T( '-' );
	strFilePrefix += tmNow.Format( _T("%Y_%m_%d_%H_%M_%S") );
	
	// Create .dmp file
	nReturn = Dump( strFilePrefix, pExceptionInfo );
	
	return nReturn;
}

LONG CGlobalExceptionHandler::Dump(CString strFilePrefix, PEXCEPTION_POINTERS pExceptionInfo)
{
	LONG nReturn = EXCEPTION_CONTINUE_SEARCH;
	
	CString strDumpFilePathname = strFilePrefix;
	strDumpFilePathname+= ".dmp";
	HANDLE hDumpFile = ::CreateFile( strDumpFilePathname, 
		GENERIC_WRITE, 
		FILE_SHARE_WRITE, 
		NULL, 
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, 
		NULL );
	
	if ( hDumpFile != INVALID_HANDLE_VALUE )
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;
		
		OutputDebugString( _T("Program crashes, creates dmp file") );
		
		// write the dump
		BOOL bOK = MiniDumpWriteDump( GetCurrentProcess(), 
			GetCurrentProcessId(), 
			hDumpFile, 
			MiniDumpNormal, 
			&ExInfo, 
			NULL, 
			NULL );
		
		::CloseHandle( hDumpFile );
		
		nReturn = EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		OutputDebugString( _T("Program crashes but failed to create dmp file") );
	}
	
	return nReturn;
}


#endif// WIN32
