// DiaLineWith2P.cpp : implementation file
//

#include "stdafx.h"
#include "SingleDraw2.h"
#include "DiaLineWith2P.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiaLineWith2P dialog


CDiaLineWith2P::CDiaLineWith2P(CWnd* pParent /*=NULL*/)
	: CDialog(CDiaLineWith2P::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiaLineWith2P)
	m_endx = 0;
	m_endy = 0;
	m_startx = 0;
	m_starty = 0;
	//}}AFX_DATA_INIT
	dlgStatus=0;
}


void CDiaLineWith2P::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiaLineWith2P)
	DDX_Text(pDX, IDC_END_X, m_endx);
	DDX_Text(pDX, IDC_END_Y, m_endy);
	DDX_Text(pDX, IDC_START_X, m_startx);
	DDX_Text(pDX, IDC_START_Y, m_starty);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiaLineWith2P, CDialog)
	//{{AFX_MSG_MAP(CDiaLineWith2P)
	ON_BN_CLICKED(IDC_STARTPICK_BUTTON, OnStartpickButton)
	ON_BN_CLICKED(IDC_ENDPICK_BUTTON, OnEndpickButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiaLineWith2P message handlers

void CDiaLineWith2P::OnStartpickButton() 
{
	// TODO: Add your control notification handler code here
	Button=1;
	UpdateData();
	EndDialog(0);
	
}

void CDiaLineWith2P::OnEndpickButton() 
{
	// TODO: Add your control notification handler code here
	Button=2;
	UpdateData();
	EndDialog(0);
	
}

BOOL CDiaLineWith2P::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	dlgStatus = 1;
	Button=0;	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDiaLineWith2P::OnOK() 
{
	// TODO: Add extra validation here
	dlgStatus = 0;
	Button = 0;
	UpdateData();
	
	CDialog::OnOK();
}

void CDiaLineWith2P::OnCancel() 
{
	// TODO: Add extra cleanup here
	dlgStatus = 0;
	Button = 0;
	m_startx = 0;
	m_starty = 0;
	m_endx = 0;
	m_endy = 0;
	
	CDialog::OnCancel();
}
