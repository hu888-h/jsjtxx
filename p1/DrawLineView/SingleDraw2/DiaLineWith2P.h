#if !defined(AFX_DIALINEWITH2P_H__E5A92FDF_E9E8_4CDD_A9E3_B47BFEA1F996__INCLUDED_)
#define AFX_DIALINEWITH2P_H__E5A92FDF_E9E8_4CDD_A9E3_B47BFEA1F996__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiaLineWith2P.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiaLineWith2P dialog

class CDiaLineWith2P : public CDialog
{
// Construction
public:
	int dlgStatus,Button;
	CDiaLineWith2P(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiaLineWith2P)
	enum { IDD = IDD_DIALOG1 };
	int		m_endx;
	int		m_endy;
	int		m_startx;
	int		m_starty;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiaLineWith2P)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDiaLineWith2P)
	afx_msg void OnStartpickButton();
	afx_msg void OnEndpickButton();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALINEWITH2P_H__E5A92FDF_E9E8_4CDD_A9E3_B47BFEA1F996__INCLUDED_)
