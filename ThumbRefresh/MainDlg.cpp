// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	pThumbnailCache=NULL;
	//add 'About' item to sys menu
	ATLASSERT(ID_APP_ABOUT < 0xF000);
	CMenu pSysMenu=GetSystemMenu(FALSE);
	if (!pSysMenu.IsNull())
	{
		pSysMenu.AppendMenu(MF_SEPARATOR);
		pSysMenu.AppendMenu(MF_STRING, ID_APP_ABOUT, _T("About"));
	}

	m_hActionBtn=GetDlgItem(IDC_ACTION);
	::SetWindowText(m_hActionBtn, _T("Exit"));

	m_hFileLabel=GetDlgItem(IDC_FILELABEL);
	::ShowWindow(m_hFileLabel, SW_HIDE);
	::SetWindowText(m_hFileLabel, NULL);

	m_hProgress=GetDlgItem(IDC_PROGRESS1);
	::ShowWindow(m_hProgress, SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_PROGRESSLABEL), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_DROPLABEL), SW_SHOW);
	::ShowWindow(GetDlgItem(IDI_DROPICON), SW_SHOW);

	//IDC_DROPLABEL in bold font
	HFONT hf=(HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (hf)
	{
		LOGFONT lfont;
		if (0!=GetObject(hf, sizeof(LOGFONT), &lfont))
		{
			if (IsClearTypeEnabled()) lfont.lfQuality=CLEARTYPE_QUALITY;
			lfont.lfWeight=700;//bold
			if (m_fBold.CreateFontIndirect(&lfont))
				::SendMessage(GetDlgItem(IDC_DROPLABEL), WM_SETFONT, (WPARAM)m_fBold.m_hFont, MAKELPARAM(TRUE, 0));
		}
	}

	RegisterDropTarget();

return FALSE;//no focus rect on action btn
}

LRESULT CMainDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (wParam!=ID_APP_ABOUT) return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

	CAboutDlg _a;
	_a.DoModal();
return 0;
}

LRESULT CMainDlg::OnStaticCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if ((HWND)lParam==GetDlgItem(IDC_DROPAREA)) 
		return (LRESULT)::GetStockObject(WHITE_BRUSH);

	if ((HWND)lParam==GetDlgItem(IDI_DROPICON)) 
		return (LRESULT)::GetStockObject(WHITE_BRUSH);

	if ((HWND)lParam==GetDlgItem(IDC_DROPLABEL))
	{
		::SetBkMode((HDC)wParam,TRANSPARENT);
		::SetTextColor((HDC)wParam, RGB(40,40,40));
		return (LRESULT)::GetStockObject( WHITE_BRUSH);
	}

	if ((HWND)lParam==GetDlgItem(IDC_PROGRESSLABEL))
	{
		::SetBkMode((HDC)wParam,TRANSPARENT);
		return (LRESULT)::GetStockObject(WHITE_BRUSH);
	}
	if ((HWND)lParam==GetDlgItem(IDC_FILELABEL))
	{
		::SetBkMode((HDC)wParam,TRANSPARENT);
		return (LRESULT)::GetStockObject(WHITE_BRUSH);
	}

return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

BOOL CMainDlg::StartDropFile(UINT uNumFiles)
{
	ATLTRACE("CDropFileTarget::StartDropFile\n");
	//init
	m_bAbort=FALSE;
	m_uProcessed=0;

	if (S_OK!=::CoCreateInstance(CLSID_LocalThumbnailCache, NULL, CLSCTX_INPROC_SERVER,
										__uuidof(pThumbnailCache), (void**)&pThumbnailCache))
	{
		MessageBox(_T("Could not access thumbnail cache."), _T("Error"), MB_OK | MB_ICONWARNING);
	return FALSE;
	}

	//can't accept another drop while working
	RegisterDropTarget(FALSE);
	
	//show/hide  btn/progress
	::ShowWindow(m_hFileLabel, SW_SHOW);
	::ShowWindow(m_hProgress, SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_PROGRESSLABEL), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_DROPLABEL), SW_HIDE);
	::ShowWindow(GetDlgItem(IDI_DROPICON), SW_HIDE);
	//rename action btn
	::SetWindowText(m_hActionBtn, _T("Cancel"));
	//init progress
	::SendMessage(m_hProgress, PBM_SETRANGE32, 0, (LPARAM)uNumFiles);
	//emtpy file label
	::SetWindowText(m_hFileLabel, NULL);
return TRUE;
}

BOOL CMainDlg::ProcessDropFile(LPCTSTR szDropFile, UINT nFile)
{
	ATLTRACE(_T("ProcessDropFile %d: %s\n"), nFile, szDropFile);

	if (!pThumbnailCache) return FALSE;//pThumbnailCache==NULL if not working
	if (m_bAbort) return FALSE;

	//skip dirs (for now)
	if (PathIsDirectory(szDropFile)) return TRUE;

	::SetWindowText(m_hFileLabel, PathFindFileName(szDropFile));
	RefreshThumbnail(szDropFile);
	m_uProcessed++;

	//refresh target
	if (StrCmpI(_T(".lnk"), PathFindExtension(szDropFile))==0)
	{
		TCHAR lnkpath[2048+1];//better than MAX_PATH
		if SUCCEEDED(ResolveShellShortcut(szDropFile, lnkpath, 2048))
		{
			ATLTRACE(_T("resolved LNK: %s\n"),lnkpath);
			if (PathFileExists(lnkpath) && !PathIsDirectory(lnkpath))
			{
				::SetWindowText(m_hFileLabel, PathFindFileName(lnkpath));
				RefreshThumbnail(lnkpath);
				m_uProcessed++;
			}
		}
	}
	//set progress
	::SendMessage(m_hProgress, PBM_SETPOS, (WPARAM)nFile, 0);

return TRUE;
}

HRESULT CMainDlg::RefreshThumbnail(LPCTSTR szFile)
{
	ProcessDlgMessages();

	CComPtr<IShellItem> pShellItem;
	HRESULT hr=SHCreateItemFromParsingName(szFile, NULL, IID_IShellItem, (void**)&pShellItem);//_WIN32_IE >= _WIN32_IE_IE70
	if (hr!=S_OK) return E_FAIL;

	// bigger thumbs are scaled down for display
	// 256 downto medium, 96 under medium, 32 below medium size
	// GetThumbnail() fails if filetype has no thumbnail handler

//?threading


//?koji step je najbrži?(use timing) vidi sa jpg (windows' handler bi trebao biti referentno brz)

	hr=pThumbnailCache->GetThumbnail(pShellItem, 256, WTS_FORCEEXTRACTION, NULL, NULL, NULL);
//if SUCCEEDED(hr) m_uProcessed++;//>>>>>>> i za svaki thumbsize

	ProcessDlgMessages();//each step may be slow

	hr=pThumbnailCache->GetThumbnail(pShellItem,  96, WTS_FORCEEXTRACTION, NULL, NULL, NULL);

	ProcessDlgMessages();

	hr=pThumbnailCache->GetThumbnail(pShellItem,  32, WTS_FORCEEXTRACTION, NULL, NULL, NULL);


	ProcessDlgMessages();
return hr;
}

void CMainDlg::FinishDropFile()
{
	ATLTRACE("CDropFileTarget::FinishDropFile\n");
	pThumbnailCache->Release();
	pThumbnailCache=NULL;

	::ShowWindow(m_hFileLabel, SW_HIDE);
	::ShowWindow(m_hProgress, SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_PROGRESSLABEL), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_DROPLABEL), SW_SHOW);
	::ShowWindow(GetDlgItem(IDI_DROPICON), SW_SHOW);
	::SetWindowText(m_hActionBtn, _T("Exit"));
	::SendMessage(m_hProgress, PBM_SETPOS, 0, NULL);
	::SetWindowText(m_hFileLabel, NULL);


	//(hard) refresh all thumbnail sizes
	if (m_uProcessed)
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSHNOWAIT | SHCNF_NOTIFYRECURSIVE, NULL, NULL);

	//ready for another drop
	RegisterDropTarget();
}

void CMainDlg::ProcessDlgMessages()
{
	while (PeekMessage(&m_msg, m_hWnd, NULL,NULL,PM_REMOVE) && !m_bAbort)
	{
		if(!::IsDialogMessage(m_hWnd, &m_msg))
		{
			TranslateMessage(&m_msg);
			::DispatchMessage(&m_msg);
		}
	}
}

LRESULT CMainDlg::OnAction(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("CMainDlg::OnAction\n");

	if (pThumbnailCache) {m_bAbort=TRUE; return 0;}//use pThumbnailCache ptr as bool var/true if working

	//else, close dialog if idle
	CloseDialog(wID);
return 0;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);
	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

//LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	// TODO: Add validation code 
//	CloseDialog(wID);
//	return 0;
//}
//
//LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	CloseDialog(wID);
//	return 0;
//}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}
