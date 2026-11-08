// MouseLineView.cpp : implementation of the CMouseLineView class
//

#include "stdafx.h"
#include "MouseLine.h"

#include "MouseLineDoc.h"
#include "MouseLineView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMouseLineView

IMPLEMENT_DYNCREATE(CMouseLineView, CView)

BEGIN_MESSAGE_MAP(CMouseLineView, CView)
	//{{AFX_MSG_MAP(CMouseLineView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMouseLineView construction/destruction

CMouseLineView::CMouseLineView()
{
	// TODO: add construction code here

}

CMouseLineView::~CMouseLineView()
{
}

BOOL CMouseLineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMouseLineView drawing

void CMouseLineView::OnDraw(CDC* pDC)
{
	CMouseLineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMouseLineView printing

BOOL CMouseLineView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMouseLineView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMouseLineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMouseLineView diagnostics

#ifdef _DEBUG
void CMouseLineView::AssertValid() const
{
	CView::AssertValid();
}

void CMouseLineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMouseLineDoc* CMouseLineView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMouseLineDoc)));
	return (CMouseLineDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMouseLineView message handlers

void CMouseLineView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CClientDC clientDC(this);
	p1 = point;
	
	CView::OnLButtonDown(nFlags, point);
}

void CMouseLineView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CClientDC clientDC(this);
	clientDC.MoveTo(p1);
	clientDC.LineTo(point);
	
	CView::OnLButtonUp(nFlags, point);
}
