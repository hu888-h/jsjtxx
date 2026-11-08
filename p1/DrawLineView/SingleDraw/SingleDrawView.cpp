// SingleDrawView.cpp : implementation of the CSingleDrawView class
//

#include "stdafx.h"
#include "SingleDraw.h"

#include "SingleDrawDoc.h"
#include "SingleDrawView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawView

IMPLEMENT_DYNCREATE(CSingleDrawView, CView)

BEGIN_MESSAGE_MAP(CSingleDrawView, CView)
	//{{AFX_MSG_MAP(CSingleDrawView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawView construction/destruction

CSingleDrawView::CSingleDrawView()
{
	// TODO: add construction code here
	mDragging = 0;

}

CSingleDrawView::~CSingleDrawView()
{
}

BOOL CSingleDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawView drawing

void CSingleDrawView::OnDraw(CDC* pDC)
{
	CSingleDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	CLine *line=pDoc->getLine().getLinkHead();
	while(line)
	{
		p1=line->GetP1();
		p2 = line->Getp2();
		pDC->MoveTo(p1);
		pDC->LineTo(p2);
		line = line->getNext();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawView printing

BOOL CSingleDrawView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSingleDrawView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSingleDrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawView diagnostics

#ifdef _DEBUG
void CSingleDrawView::AssertValid() const
{
	CView::AssertValid();
}

void CSingleDrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSingleDrawDoc* CSingleDrawView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSingleDrawDoc)));
	return (CSingleDrawDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawView message handlers

void CSingleDrawView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CClientDC clientDC(this);
	if(mDragging==0)
	{
		mDragging=1;
		p1=p2=point;
	}
	else
	{
		mDragging=0;
		clientDC.SetROP2(R2_NOT);
		clientDC.MoveTo(p1);
		clientDC.LineTo(p2);
		clientDC.SetROP2(R2_COPYPEN);
		clientDC.MoveTo(p1);
		clientDC.LineTo(point);
		p2=point;
		saveLine(p1,p2);
	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CSingleDrawView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CClientDC clientDC(this);
	if(mDragging)
	{
		clientDC.SetROP2(R2_NOT);
		clientDC.MoveTo(p1);
		clientDC.LineTo(p2);
		clientDC.MoveTo(p1);
		clientDC.LineTo(point);
		p2=point;
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CSingleDrawView::saveLine(CPoint &p1,CPoint &p2)
{
	CSingleDrawDoc *pDoc = GetDocument();
	CLine *pL = new CLine(p1,p2);
	pDoc->getLine().append(pL);
}
