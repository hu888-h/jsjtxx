// DlgPoints.cpp : implementation file
//

#include "stdafx.h"
#include "DrawLine.h"
#include "DlgPoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPoints dialog


CDlgPoints::CDlgPoints(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPoints::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPoints)
	m_ex = 0;
	m_ey = 0;
	m_sx = 0;
	m_sy = 0;
	//}}AFX_DATA_INIT
}


void CDlgPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPoints)
	DDX_Text(pDX, IDC_EDTEX, m_ex);
	DDX_Text(pDX, IDC_EDTEY, m_ey);
	DDX_Text(pDX, IDC_EDTSX, m_sx);
	DDX_Text(pDX, IDC_EDTSY, m_sy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPoints, CDialog)
	//{{AFX_MSG_MAP(CDlgPoints)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPoints message handlers
