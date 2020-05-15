// PredigestDlg.h : header file
//

#if !defined(AFX_PREDIGESTDLG_H__CA4EDACD_3E53_4BE9_B89B_19A7EC51E9F1__INCLUDED_)
#define AFX_PREDIGESTDLG_H__CA4EDACD_3E53_4BE9_B89B_19A7EC51E9F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPredigestDlg dialog
#include "BtnST.h"

class CPredigestDlg : public CDialog
{
// Construction
public:
	CPredigestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPredigestDlg)
	enum { IDD = IDD_PREDIGEST_DIALOG };
	CButtonST	m_btnOK;
	CButtonST	m_btnParse;
	CString	m_strOriginalString;
	CString	m_strPredigestString;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPredigestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
//	CButtonST m_btnParse
	// Generated message map functions
	//{{AFX_MSG(CPredigestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnParse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREDIGESTDLG_H__CA4EDACD_3E53_4BE9_B89B_19A7EC51E9F1__INCLUDED_)
