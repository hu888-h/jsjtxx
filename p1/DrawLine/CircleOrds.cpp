// CircleOrds.cpp : implementation file
//

#include "stdafx.h"
#include "DrawLine.h"
#include "CircleOrds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCircleOrds dialog


CCircleOrds::CCircleOrds(CWnd* pParent /*=NULL*/)
	: CDialog(CCircleOrds::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCircleOrds)
	m_cx = 0;
	m_cy = 0;
	m_r = 0;
	//}}AFX_DATA_INIT
}


void CCircleOrds::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCircleOrds)
	DDX_Text(pDX, IDC_CX, m_cx);
	DDX_Text(pDX, IDC_CY, m_cy);
	DDX_Text(pDX, IDC_R, m_r);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCircleOrds, CDialog)
	//{{AFX_MSG_MAP(CCircleOrds)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCircleOrds message handlers
