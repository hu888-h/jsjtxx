// DrawLineView.cpp : implementation of the CDrawLineView class
//

#include "stdafx.h"
#include "DrawLine.h"

#include "DrawLineDoc.h"
#include "DrawLineView.h"

#include "DlgPoints.h"
#include "CircleOrds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView

IMPLEMENT_DYNCREATE(CDrawLineView, CView)

BEGIN_MESSAGE_MAP(CDrawLineView, CView)
	//{{AFX_MSG_MAP(CDrawLineView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_DLGPOINTS, OnDlgpoints)
	ON_WM_SIZE()
	ON_COMMAND(IDM_CIRCLE, OnCircle)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView construction/destruction

CDrawLineView::CDrawLineView()
{
	// TODO: add construction code here
	m_centerx = m_centery = 0;
	m_sx = m_sy = 0;
	m_ex = m_ey = 0;
	m_cx = m_cy = 0;
	m_r = 0;
	m_flag = 1;
}

CDrawLineView::~CDrawLineView()
{
}

BOOL CDrawLineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView drawing

void CDrawLineView::OnDraw(CDC* pDC)
{
	CDrawLineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	wglMakeCurrent(pDC->m_hDC,hglrc);
	display();
	wglMakeCurrent(pDC->m_hDC,NULL);
	SwapBuffers(pDC->m_hDC);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView printing

BOOL CDrawLineView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDrawLineView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDrawLineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView diagnostics

#ifdef _DEBUG
void CDrawLineView::AssertValid() const
{
	CView::AssertValid();
}

void CDrawLineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDrawLineDoc* CDrawLineView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawLineDoc)));
	return (CDrawLineDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView message handlers

int CDrawLineView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	PIXELFORMATDESCRIPTOR pfd={sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,0,0,
		0,0,0,0,
		32,
		0,0,
		PFD_MAIN_PLANE,
		0,0,0,0};

	//获取客户区设备描述表
	CClientDC clientDC(this);
	int pixelFormat = ChoosePixelFormat(clientDC.m_hDC,&pfd);
	SetPixelFormat(clientDC.m_hDC,pixelFormat,&pfd);
	hglrc = wglCreateContext(clientDC.m_hDC);
	
	return 0;
}


void CDrawLineView::OnDestroy() 
{
	CView::OnDestroy();
	wglDeleteContext(hglrc);	
}

void CDrawLineView::init()
{
	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void CDrawLineView::display()
{
	init();

	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_LINES);
		glVertex2f(-1.0,0.0);
		glVertex2f(1.0,0.0);
		glVertex2f(0.0,-1.0);
		glVertex2f(0.0,1.0);
	glEnd();

	int *xx,*yy;
	int dx,dy;
	int n,m;
	float x,y;
	CStdioFile file;
	CString temstr,filename;

	if(m_flag==1){
		dx = abs(m_sx-m_ex);
		dy = abs(m_sy-m_ey);
		if(dx>=dy){
			n = dx+1;
		}else{
			n = dy+1;
		}
		xx = new int[n];
		yy = new int[n];
		getPoints(m_sx,m_sy,m_ex,m_ey,xx,yy);
	}else if(m_flag==2){
		n = (int)(0.8*m_r*8);
		xx = new int[n];
		yy = new int[n];
		getCirclePoints(m_cx,m_cy,m_r,xx,yy,&n);
	}

	m = 0;
	glColor3f(0.0f,0.0f,1.0f);
	glBegin(GL_POINTS);
	while(m<n){
		x = (float)*(xx+m)/m_centerx;
		y = (float)*(yy+m)/m_centery;
		glVertex2f(x,y);
		m++;
	}
	glEnd();

	if(m_flag==1){
		filename = "LinePoints.txt";
	}else if(m_flag==2){
		filename = "CirclePoints.txt";
	}
	if(file.Open(filename,CFile::modeCreate|CFile::modeWrite)){
		m = 0;
		while(m<n){
			temstr.Format("%d,\t%d\n",*(xx+m),*(yy+m));
			file.WriteString(temstr);
			m++;
		}
	}
	file.Close();

	delete[] xx;
	delete[] yy;

	glColor3f(0.7f,0.8f,0.0f);
	glFlush();
}

void CDrawLineView::OnDlgpoints() 
{
	CDlgPoints dlg;
	dlg.m_sx = m_sx;
	dlg.m_sy = m_sy;
	dlg.m_ex = m_ex;
	dlg.m_ey = m_ey;
	if(dlg.DoModal()==IDOK){
		m_sx = dlg.m_sx;
		m_sy = dlg.m_sy;
		m_ex = dlg.m_ex;
		m_ey = dlg.m_ey;
		m_flag = 1;
		Invalidate();
	}	
}

void CDrawLineView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	CRect rect;
	int w = cx;
	int h = cy;

	CClientDC clientDC(this);
	wglMakeCurrent(clientDC.m_hDC,hglrc);
	glViewport(0,0,w,h);
	wglMakeCurrent(NULL,NULL);

	GetClientRect(&rect);
	m_centerx = rect.Width()/2;
	m_centery = rect.Height()/2;	
}

void CDrawLineView::getPoints(int sx,int sy,int ex,int ey,int *xx,int *yy)
{
	int dx,dy,p,twody,twodx,twodydx,twodxdy;
	int x,y,xend,yend;
	int i=0;

	if(sx==ex){
		while(sy<=ey){
			*(xx+i) = sx;
			*(yy+i) = sy;
			sy++;
			i++;
		}
	}else if(sy==ey){
		while(sx<=ex){
			*(xx+i) = sx;
			*(yy+i) = sy;
			sx++;
			i++;
		}
	}else{
		dx = abs(sx-ex);
		dy = abs(sy-ey);
		if(dx>=dy){
			p = 2*dy-dx;
			twody = 2*dy;
			twodydx = 2*(dy-dx);
			if(sx>ex){
				x = ex;
				y = ey;
				xend = sx;
			}else{
				x = sx;
				y = sy;
				xend = ex;
			}
			*(xx+i) = x;
			*(yy+i) = y;
			i++;
			while(x<xend){
				x++;
				if(p<0)
					p+=twody;
				else{
					y++;
					p+=twodydx;
				}
				*(xx+i) = x;
				*(yy+i) = y;
				i++;
			}
		}else{
			p = 2*dx-dy;
			twodx = 2*dx;
			twodxdy = 2*(dx-dy);
			if(sy>ey){
				x = ex;
				y = ey;
				yend = sy;
			}else{
				x = sx;
				y = sy;
				yend = ey;
			}
			*(xx+i) = x;
			*(yy+i) = y;
			i++;
			while(y<yend){
				y++;
				if(p<0)
					p+=twodx;
				else{
					x++;
					p+=twodxdy;
				}
				*(xx+i) = x;
				*(yy+i) = y;
				i++;
			}
		}
	}
}

void CDrawLineView::OnCircle() 
{
 	CCircleOrds dlg;
 
 	dlg.m_cx = m_cx;
 	dlg.m_cy = m_cy;
 	dlg.m_r = m_r;
 	if(dlg.DoModal()==IDOK){
 		m_cx = dlg.m_cx;
 		m_cy = dlg.m_cy;
 		m_r = dlg.m_r;
 		m_flag = 2;
 		Invalidate();
 	}	
}

void CDrawLineView::getCirclePoints(int cx,int cy,int r,int *xx,int *yy,int *nn)
{
	int x=0,i=0;
	int y=r;
	int d=1-r;

	*(xx+i) = cx+x;
	*(yy+i) = cy+y;
	i++;
	*(xx+i) = cx-x;
	*(yy+i) = cy+y;
	i++;
	*(xx+i) = cx+x;
	*(yy+i) = cy-y;
	i++;
	*(xx+i) = cx-x;
	*(yy+i) = cy-y;
	i++;
	*(xx+i) = cx+y;
	*(yy+i) = cy+x;
	i++;
	*(xx+i) = cx-y;
	*(yy+i) = cy+x;
	i++;
	*(xx+i) = cx+y;
	*(yy+i) = cy-x;
	i++;
	*(xx+i) = cx-y;
	*(yy+i) = cy-x;
	i++;

	while(x<y){
		x++;
		if(d<0)
			d+=2*x+1;
		else{
			y--;
			d+=2*(x-y)+1;
		}
		*(xx+i) = cx+x;
		*(yy+i) = cy+y;
		i++;
		*(xx+i) = cx-x;
		*(yy+i) = cy+y;
		i++;
		*(xx+i) = cx+x;
		*(yy+i) = cy-y;
		i++;
		*(xx+i) = cx-x;
		*(yy+i) = cy-y;
		i++;
		*(xx+i) = cx+y;
		*(yy+i) = cy+x;
		i++;
		*(xx+i) = cx-y;
		*(yy+i) = cy+x;
		i++;
		*(xx+i) = cx+y;
		*(yy+i) = cy-x;
		i++;
		*(xx+i) = cx-y;
		*(yy+i) = cy-x;
		i++;
	}
	*nn = i;
}
