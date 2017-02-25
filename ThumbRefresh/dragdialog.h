#ifndef _DRAGDIALOG_00963332_3E7F_485E_B158_7D74EAB60EDE_
#define _DRAGDIALOG_00963332_3E7F_485E_B158_7D74EAB60EDE_

//dialog drag support
template <class T> class CDragDialog
{
public:
		BEGIN_MSG_MAP(CDragDialog<T>)
			MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		END_MSG_MAP()

protected:
	virtual LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT=static_cast<T*>(this);
		m_lHitTest = ::DefWindowProcW(pT->m_hWnd, uMsg, wParam, lParam);
		if (m_lHitTest == HTCLIENT) return HTCAPTION;
	return m_lHitTest;
	}

private:
	LRESULT m_lHitTest;
};


#endif//_DRAGDIALOG_00963332_3E7F_485E_B158_7D74EAB60EDE_
