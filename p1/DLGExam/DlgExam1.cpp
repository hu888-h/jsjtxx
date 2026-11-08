// DlgExam1.cpp : implementation file
//

#include "stdafx.h"
#include "DLGExam.h"
#include "DlgExam1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExam dialog


CDlgExam::CDlgExam(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExam::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExam)
	m_exam = 0;
	//}}AFX_DATA_INIT
}


void CDlgExam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExam)
	DDX_Text(pDX, IDC_EDITEXAM, m_exam);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExam, CDialog)
	//{{AFX_MSG_MAP(CDlgExam)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExam message handlers
