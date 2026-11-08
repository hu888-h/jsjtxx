#if !defined(AFX_DLGPOINTS_H__974264C9_D172_4438_B587_2B8CB71C15B4__INCLUDED_)
#define AFX_DLGPOINTS_H__974264C9_D172_4438_B587_2B8CB71C15B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPoints.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPoints dialog

class CDlgPoints : public CDialog
{
// Construction
public:
	CDlgPoints(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPoints)
	enum { IDD = IDD_DLGCORDS };
	int		m_ex;
	int		m_ey;
	int		m_sx;
	int		m_sy;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPoints)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPoints)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPOINTS_H__974264C9_D172_4438_B587_2B8CB71C15B4__INCLUDED_)
