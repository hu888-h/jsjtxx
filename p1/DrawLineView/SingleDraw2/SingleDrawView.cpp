// SingleDrawView.cpp : implementation of the CSingleDrawView class
//

#include "stdafx.h"
#include "SingleDraw2.h"

#include "SingleDraw2Doc.h"
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
	ON_WM_RBUTTONDOWN()
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
	CSingleDraw2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	CLine *line=pDoc->getLine().getLinkHead();
	while(line)
	{
		p1=line->GetP1();
		p2=line->Getp2();
		pDC->MoveTo(p1);
		pDC->LineTo(p2);
		line=line->getNext();
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

CSingleDraw2Doc* CSingleDrawView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSingleDraw2Doc)));
	return (CSingleDraw2Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawView message handlers

void CSingleDrawView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(dlg.dlgStatus)
	{
		if(dlg.Button==1)
		{
			dlg.Button=0;
			mDragging=0;
			dlg.m_startx=point.x;
			dlg.m_starty=point.y;
			drawLine(R2_NOT,p1,point);
		}else if(dlg.Button==2)
		{
			dlg.Button=0;
			mDragging=0;
			dlg.m_endx=point.x;
			dlg.m_endy=point.y;
			drawLine(R2_NOT,p1,point);
		}
		if(dlg.DoModal()==IDOK)
		{
			p1.x=dlg.m_startx;
			p1.y=dlg.m_starty;
			p2.x=dlg.m_endx;
			p2.y=dlg.m_endy;
			drawLine(R2_COPYPEN,p1,p2);
			saveLine(p1,p2);
			dlg.m_startx=0;
			dlg.m_starty=0;
			dlg.m_endx=0;
			dlg.m_endy=0;
		}
	}else if(mDragging==0)
	{
		mDragging=1;
		p1=p2=point;
	}else if(mDragging==1)
	{
		mDragging=0;
		drawLine(p1,R2_NOT,p1,R2_COPYPEN,point);
		p2=point;
		saveLine(p1,p2);
	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CSingleDrawView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(dlg.dlgStatus)
	{
		if(dlg.Button==1&&mDragging==0)
		{
			mDragging=1;
			p1.x=dlg.m_endx;
			p1.y=dlg.m_endy;
			drawLine(R2_NOT,p1,point);
			p2=point;
		}else if(dlg.Button==1&&mDragging)
		{
			drawLine(p1,R2_NOT,p2,R2_NOT,point);
			p2=point;
		}else if(dlg.Button==2&&mDragging==0)
		{
			mDragging=1;
			p1.x=dlg.m_startx;
			p1.y=dlg.m_starty;
			drawLine(R2_NOT,p1,point);
			p2=point;
		}else if(dlg.Button==2&&mDragging)
		{
			drawLine(p1,R2_NOT,p2,R2_NOT,point);
			p2=point;
		}
	}else if(mDragging)
	{
		drawLine(R2_NOT,p1,p2);
		drawLine(R2_NOT,p1,point);
		p2=point;
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CSingleDrawView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(dlg.DoModal()==IDOK)
	{
		CPoint p1,p2;
		p1.x=dlg.m_startx;
		p1.y=dlg.m_starty;
		p2.x=dlg.m_endx;
		p2.y=dlg.m_endy;
		drawLine(R2_COPYPEN,p1,p2);
		saveLine(p1,p2);
		dlg.m_startx=0;
		dlg.m_starty=0;
		dlg.m_endx=0;
		dlg.m_endy=0;
	}
	
	CView::OnRButtonDown(nFlags, point);
}

void CSingleDrawView::saveLine(CPoint&p1,CPoint&p2)
{
	CSingleDraw2Doc *pDoc=GetDocument();
	CLine *pL=new CLine(p1,p2);
	pDoc->getLine().append(pL);
}

void CSingleDrawView::saveLine(int startX,int startY,int endX,int endY)
{
	CPoint p1,p2;
	p1.x=startX;
	p1.y=startY;
	p2.x=endX;
	p2.y=endY;
	saveLine(p1,p2);

}

void CSingleDrawView::drawLine(CPoint &startP,int nDrawMode1,CPoint &endP1,
		int nDrawMode2,CPoint &endP2)
{
	drawLine(nDrawMode1,startP,endP1);
	drawLine(nDrawMode2,startP,endP2);
}

void CSingleDrawView::drawLine(int nDrawMode,CPoint &startp,CPoint &endp)
{
	CClientDC clientDC(this);
	clientDC.SetROP2(nDrawMode);
	clientDC.MoveTo(startp);
	clientDC.LineTo(endp);
}