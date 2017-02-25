// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "droptarget.h"
#include "dragdialog.h"
#include "tools.h"

#include <shobjidl.h>
#include <thumbcache.h>//IThumbnailCache

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler, public CDropFileTarget<CMainDlg>, public CDragDialog<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnStaticCtlColor)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)

		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)

		COMMAND_ID_HANDLER(IDC_ACTION, OnAction)
		COMMAND_ID_HANDLER(IDOK, OnAction)//OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnAction)//OnCancel)

		CHAIN_MSG_MAP(CDropFileTarget<CMainDlg>)
		CHAIN_MSG_MAP(CDragDialog<CMainDlg>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);

//private:
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnAction(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStaticCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	BOOL StartDropFile(UINT uNumFiles);
	BOOL ProcessDropFile(LPCTSTR szDropFile, UINT nFile);
	void FinishDropFile();
	void ProcessDlgMessages();
	HRESULT RefreshThumbnail(LPCTSTR szFile);

	HWND m_hActionBtn;
	HWND m_hProgress;
	HWND m_hFileLabel;
	IThumbnailCache* pThumbnailCache;
	CFont m_fBold;
	BOOL m_bAbort;
	UINT64 m_uProcessed;
	MSG m_msg;
};
