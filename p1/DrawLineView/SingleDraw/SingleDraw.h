// SingleDraw.h : main header file for the SINGLEDRAW application
//

#if !defined(AFX_SINGLEDRAW_H__5D66E4BD_8C39_41D1_9BFB_F9EEBA9739B5__INCLUDED_)
#define AFX_SINGLEDRAW_H__5D66E4BD_8C39_41D1_9BFB_F9EEBA9739B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawApp:
// See SingleDraw.cpp for the implementation of this class
//

class CSingleDrawApp : public CWinApp
{
public:
	CSingleDrawApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleDrawApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSingleDrawApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEDRAW_H__5D66E4BD_8C39_41D1_9BFB_F9EEBA9739B5__INCLUDED_)
