// MouseLine.h : main header file for the MOUSELINE application
//

#if !defined(AFX_MOUSELINE_H__4F7A9798_DDCD_4229_9420_815D0262E494__INCLUDED_)
#define AFX_MOUSELINE_H__4F7A9798_DDCD_4229_9420_815D0262E494__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMouseLineApp:
// See MouseLine.cpp for the implementation of this class
//

class CMouseLineApp : public CWinApp
{
public:
	CMouseLineApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMouseLineApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMouseLineApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUSELINE_H__4F7A9798_DDCD_4229_9420_815D0262E494__INCLUDED_)
