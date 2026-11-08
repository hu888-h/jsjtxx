#if !defined(AFX_DLGEXAM1_H__5C5EF71D_7C20_4743_85B3_7AC19563710D__INCLUDED_)
#define AFX_DLGEXAM1_H__5C5EF71D_7C20_4743_85B3_7AC19563710D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExam1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExam dialog

class CDlgExam : public CDialog
{
// Construction
public:
	CDlgExam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExam)
	enum { IDD = IDD_DIALOGEXAM };
	int		m_exam;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExam)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXAM1_H__5C5EF71D_7C20_4743_85B3_7AC19563710D__INCLUDED_)
