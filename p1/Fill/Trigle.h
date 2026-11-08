#if !defined(AFX_TRIGLE_H__6A58A520_F1D7_4AE7_A9F3_488E674CECDE__INCLUDED_)
#define AFX_TRIGLE_H__6A58A520_F1D7_4AE7_A9F3_488E674CECDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Trigle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrigle dialog

class CTrigle : public CDialog
{
// Construction
public:
	CTrigle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTrigle)
	enum { IDD = IDD_DIALOG1 };
	int		m_x1;
	int		m_x2;
	int		m_x3;
	int		m_y1;
	int		m_y2;
	int		m_y3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrigle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrigle)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGLE_H__6A58A520_F1D7_4AE7_A9F3_488E674CECDE__INCLUDED_)
