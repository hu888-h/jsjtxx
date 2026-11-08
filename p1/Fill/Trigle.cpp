// Trigle.cpp : implementation file
//

#include "stdafx.h"
#include "Fill.h"
#include "Trigle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrigle dialog


CTrigle::CTrigle(CWnd* pParent /*=NULL*/)
	: CDialog(CTrigle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrigle)
	m_x1 = 0;
	m_x2 = 0;
	m_x3 = 0;
	m_y1 = 0;
	m_y2 = 0;
	m_y3 = 0;
	//}}AFX_DATA_INIT
}


void CTrigle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrigle)
	DDX_Text(pDX, IDC_X1, m_x1);
	DDX_Text(pDX, IDC_X2, m_x2);
	DDX_Text(pDX, IDC_X3, m_x3);
	DDX_Text(pDX, IDC_Y1, m_y1);
	DDX_Text(pDX, IDC_Y2, m_y2);
	DDX_Text(pDX, IDC_Y3, m_y3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrigle, CDialog)
	//{{AFX_MSG_MAP(CTrigle)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrigle message handlers
