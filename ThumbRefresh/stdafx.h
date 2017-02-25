// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
//#define WINVER		0x0500
#define _WIN32_WINNT	_WIN32_WINNT_VISTA
#define _WIN32_IE		_WIN32_IE_IE70
//#define _RICHEDIT_VER	0x0200

// This project was generated for VC++ 2005 Express and ATL 3.0 from Platform SDK.
// Comment out this line to build the project with different versions of VC++ and ATL.
#define _WTL_SUPPORT_SDK_ATL3

// Support for VS2005 Express & SDK ATL
#ifdef _WTL_SUPPORT_SDK_ATL3
#if (_ATL_VER<0x0800)
    #define _CRT_SECURE_NO_DEPRECATE
    #pragma conform(forScope, off)
#endif
#pragma comment(linker, "/NODEFAULTLIB:atlthunk.lib")
#endif // _WTL_SUPPORT_SDK_ATL3

#include <atlbase.h>

// Support for VS2005 Express & SDK ATL
#ifdef _WTL_SUPPORT_SDK_ATL3
#if (_ATL_VER>=0x0800)
    #ifndef __ATLSTDTHUNK_H__
    #include <atlstdthunk.h> // _stdcallthunk definition
    #endif
#endif
  namespace ATL
  {
	inline void * __stdcall __AllocStdCallThunk()
	{
		return ::VirtualAlloc(0, sizeof(_stdcallthunk), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	}

	inline void __stdcall __FreeStdCallThunk(void *p)
	{
		if (p) ::VirtualFree(p, 0, MEM_RELEASE);
	}
  };
#endif // _WTL_SUPPORT_SDK_ATL3

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlmisc.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
