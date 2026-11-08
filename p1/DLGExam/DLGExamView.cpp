// DLGExamView.cpp : implementation of the CDLGExamView class
//

#include "stdafx.h"
#include "DLGExam.h"

#include "DLGExamDoc.h"
#include "DLGExamView.h"
#include "Input.h"
#include "DlgExam1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLGExamView

IMPLEMENT_DYNCREATE(CDLGExamView, CView)

BEGIN_MESSAGE_MAP(CDLGExamView, CView)
	//{{AFX_MSG_MAP(CDLGExamView)
	ON_COMMAND(IDM_MODEL, OnModel)
	ON_COMMAND(IDM_NOMODEL, OnNomodel)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLGExamView construction/destruction

CDLGExamView::CDLGExamView()
{
	// TODO: add construction code here

}

CDLGExamView::~CDLGExamView()
{
}

BOOL CDLGExamView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDLGExamView drawing

void CDLGExamView::OnDraw(CDC* pDC)
{
	CDLGExamDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDLGExamView printing

BOOL CDLGExamView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDLGExamView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDLGExamView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDLGExamView diagnostics

#ifdef _DEBUG
void CDLGExamView::AssertValid() const
{
	CView::AssertValid();
}

void CDLGExamView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDLGExamDoc* CDLGExamView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDLGExamDoc)));
	return (CDLGExamDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDLGExamView message handlers

void CDLGExamView::OnModel() 
{
	// TODO: Add your command handler code here
	int input;
	CString str;
	//CInput Dlg;
	CDlgExam Dlg;
	if(Dlg.DoModal()==IDOK)
	{
		input = Dlg.m_exam;//Dlg.m_ed;
		str.Format("%d",input);
		AfxMessageBox(str);
	}
}

void CDLGExamView::OnNomodel() 
{
	//CInput *Dlg;
	CDlgExam *Dlg;
	Dlg = new CDlgExam;//new CInput;
	Dlg->Create(IDD_DIALOGEXAM/*IDD_DIALOGINPUT*/,NULL);
	Dlg->ShowWindow(SW_SHOW);
}
