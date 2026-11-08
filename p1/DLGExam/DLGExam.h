// DLGExam.h : main header file for the DLGEXAM application
//

#if !defined(AFX_DLGEXAM_H__91B94E54_12A5_47A2_ABE6_47797C49AFBD__INCLUDED_)
#define AFX_DLGEXAM_H__91B94E54_12A5_47A2_ABE6_47797C49AFBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDLGExamApp:
// See DLGExam.cpp for the implementation of this class
//

class CDLGExamApp : public CWinApp
{
public:
	CDLGExamApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLGExamApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDLGExamApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXAM_H__91B94E54_12A5_47A2_ABE6_47797C49AFBD__INCLUDED_)
