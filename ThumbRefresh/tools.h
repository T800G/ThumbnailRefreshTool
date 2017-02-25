#ifndef _TOOLS_BBFA8B6D_CAF9_4FE3_95C3_7F3B43F140F0_
#define _TOOLS_BBFA8B6D_CAF9_4FE3_95C3_7F3B43F140F0_

inline BOOL IsClearTypeEnabled()
{
	BOOL check=0;
	if (!SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &check,0)) return FALSE;
	if (!check) return FALSE;
	UINT check2=0;
	if (!SystemParametersInfo(SPI_GETFONTSMOOTHINGTYPE, 0, &check2,0)) return FALSE;
return (check2==FE_FONTSMOOTHINGCLEARTYPE);
}




// safe string functions
#include <strsafe.h>
#pragma comment(lib,"strsafe.lib")
// IPersistFile is using LPCOLESTR,so make sure that the string is Unicode
#ifndef _UNICODE
	#error IPersistFile requires UNICODE
#endif
#include <shlguid.h>
#include <shobjidl.h>
inline HRESULT ResolveShellShortcut(/*in*/ LPCTSTR lpszShortcutPath, /*out*/ LPTSTR lpszFilePath, /*in*/size_t cchFilePath)
{
	CComPtr<IShellLink> ipShellLink;
	// Get a pointer to the IShellLink interface
	HRESULT hRes=CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&ipShellLink);
	if (hRes!=S_OK) return hRes;

	// Get a pointer to the IPersistFile interface on the shortcut
	CComQIPtr<IPersistFile> ipPersistFile(ipShellLink);

	// Open the shortcut file and initialize it from its contents
	hRes=ipPersistFile->Load(lpszShortcutPath, STGM_READ);
	if (hRes!=S_OK) return hRes;

	// Get the path to the shortcut target
	// Returns S_OK if the operation is successful and a valid path is retrieved. 
	// If the operation is successful, but no path is retrieved, 
	// it returns S_FALSE and pszFile will be empty.	//(!((hRes==S_OK) || (hRes==S_FALSE)))
	// SLGP_UNCPRIORITY - Retrieves the Universal Naming Convention (UNC) path name of the file.>>>>>>>>za UNC

	hRes = ipShellLink->GetPath(lpszFilePath, cchFilePath, NULL, SLGP_RAWPATH | SLGP_UNCPRIORITY); 

return hRes;
}

#endif//_TOOLS_BBFA8B6D_CAF9_4FE3_95C3_7F3B43F140F0_
