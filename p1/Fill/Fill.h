// Fill.h : main header file for the FILL application
//

#if !defined(AFX_FILL_H__771332B3_B599_48B9_8316_5F0216163DF1__INCLUDED_)
#define AFX_FILL_H__771332B3_B599_48B9_8316_5F0216163DF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFillApp:
// See Fill.cpp for the implementation of this class
//

class CFillApp : public CWinApp
{
public:
	CFillApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFillApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILL_H__771332B3_B599_48B9_8316_5F0216163DF1__INCLUDED_)
