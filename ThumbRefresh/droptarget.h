#ifndef _DROPTARGET_22D5EC3E_31DE_4620_AA1E_3948683120E3_
#define _DROPTARGET_22D5EC3E_31DE_4620_AA1E_3948683120E3_

#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

template <class T> class CDropFileTarget // implements dropfile target window
{
public:
	BEGIN_MSG_MAP(CDropFileTarget<T>)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles) 
	END_MSG_MAP()

	void RegisterDropTarget(BOOL bAccept = TRUE)
	{
		T* pT=static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		::DragAcceptFiles(pT->m_hWnd, bAccept);
	}

protected:
	virtual BOOL StartDropFile(UINT uNumFiles){ATLTRACE("CDropFileTarget::StartDropFile\n");return TRUE;}
	virtual BOOL ProcessDropFile(LPCTSTR szDropFile, UINT nFile){ATLTRACE(_T("CDropFileTarget::ProcessDropFile: %s"),szDropFile);return TRUE;}
	virtual void FinishDropFile(){ATLTRACE("CDropFileTarget::FinishDropFile\n");}

	virtual LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UINT i;
		UINT uMaxLen=0;
		UINT uNumFiles = ::DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
		if (!StartDropFile(uNumFiles)) return 0;

		for (i=0; i<uNumFiles; i++)
			uMaxLen=max(uMaxLen, ::DragQueryFile((HDROP)wParam, i, NULL, 0));
		uMaxLen++;//DragQueryFile doesn't include terminating null

		TCHAR* szFilename=(TCHAR*)::CoTaskMemAlloc(sizeof(TCHAR)*uMaxLen);

		if (szFilename!=NULL)
			for (i=0; i<uNumFiles; i++)
			{
				::DragQueryFile((HDROP)wParam, i, szFilename, uMaxLen);
				if (!ProcessDropFile(szFilename, i)) break;
			}

		::CoTaskMemFree(szFilename);
		::DragFinish((HDROP)wParam);
		FinishDropFile();
	return 0;
	}
};


#endif//_DROPTARGET_22D5EC3E_31DE_4620_AA1E_3948683120E3_
