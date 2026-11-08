// DrawLine.h : main header file for the DRAWLINE application
//

#if !defined(AFX_DRAWLINE_H__45240222_70E7_48FA_83AA_D7B95886AF19__INCLUDED_)
#define AFX_DRAWLINE_H__45240222_70E7_48FA_83AA_D7B95886AF19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDrawLineApp:
// See DrawLine.cpp for the implementation of this class
//

class CDrawLineApp : public CWinApp
{
public:
	CDrawLineApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawLineApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDrawLineApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWLINE_H__45240222_70E7_48FA_83AA_D7B95886AF19__INCLUDED_)
