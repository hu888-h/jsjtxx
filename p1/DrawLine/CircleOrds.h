#if !defined(AFX_CIRCLEORDS_H__61C3F50B_B85E_4DE0_B68E_4D3763951DD2__INCLUDED_)
#define AFX_CIRCLEORDS_H__61C3F50B_B85E_4DE0_B68E_4D3763951DD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CircleOrds.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCircleOrds dialog

class CCircleOrds : public CDialog
{
// Construction
public:
	CCircleOrds(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCircleOrds)
	enum { IDD = IDD_DLGCIRCLE };
	int		m_cx;
	int		m_cy;
	int		m_r;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCircleOrds)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCircleOrds)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CIRCLEORDS_H__61C3F50B_B85E_4DE0_B68E_4D3763951DD2__INCLUDED_)
