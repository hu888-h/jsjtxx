// SingleDraw2.h : main header file for the SINGLEDRAW2 application
//

#if !defined(AFX_SINGLEDRAW2_H__7772FAE9_C452_4472_BB76_12A0E6C2BA1E__INCLUDED_)
#define AFX_SINGLEDRAW2_H__7772FAE9_C452_4472_BB76_12A0E6C2BA1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSingleDraw2App:
// See SingleDraw2.cpp for the implementation of this class
//

class CSingleDraw2App : public CWinApp
{
public:
	CSingleDraw2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleDraw2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSingleDraw2App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEDRAW2_H__7772FAE9_C452_4472_BB76_12A0E6C2BA1E__INCLUDED_)
