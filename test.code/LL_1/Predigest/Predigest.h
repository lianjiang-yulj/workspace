// Predigest.h : main header file for the PREDIGEST application
//

#if !defined(AFX_PREDIGEST_H__BE9D32F2_8D2C_4575_B4A5_4B1CFBD03F0D__INCLUDED_)
#define AFX_PREDIGEST_H__BE9D32F2_8D2C_4575_B4A5_4B1CFBD03F0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPredigestApp:
// See Predigest.cpp for the implementation of this class
//

class CPredigestApp : public CWinApp
{
public:
	CPredigestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPredigestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPredigestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREDIGEST_H__BE9D32F2_8D2C_4575_B4A5_4B1CFBD03F0D__INCLUDED_)
