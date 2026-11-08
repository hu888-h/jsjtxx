// FillView.cpp : implementation of the CFillView class
//

#include "stdafx.h"
#include "Fill.h"

#include "FillDoc.h"
#include "FillView.h"

#include "Trigle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WINDOW_HEIGHT 768
#define WINDOW_WIDTH 1024
int window[WINDOW_WIDTH*WINDOW_HEIGHT];

typedef struct Edge{
	int yUpper;
	float xIntersect,dxPerScan;
	struct Edge *next;
}Edge;

void setpixel(int i,int scan)
{
	*(window+scan*WINDOW_WIDTH+i)=1;
}

//按x排序，将边插入边表
void insertEdge(Edge *list, Edge *edge)
{
	Edge *p,*q=list;
	p = q->next;
	while(p!=NULL){
		if(edge->xIntersect<p->xIntersect)
			p = NULL;
		else{
			q = p;
			p = p->next;
		}
	}
	edge->next = q->next;
	q->next = edge;
}

int yNext(int k,int cnt,dePt *pts)
{
	int j;
	if((k+1)>(cnt-1))
		j = 0;
	else
		j = k+1;
	while(pts[k].y == pts[j].y)
		if((j+1)>(cnt-1))
			j=0;
		else
			j++;
	return pts[j].y;
}

void makeEdgeRec(dePt lower,dePt upper,int yComp,Edge *edge,Edge *edges[])
{
	edge->dxPerScan = (float)(upper.x-lower.x)/(upper.y-lower.y);
	edge->xIntersect = (float)lower.x;
	if(upper.y<yComp)
		edge->yUpper = upper.y-1;
	else
		edge->yUpper = upper.y;
	insertEdge(edges[lower.y],edge);
}

void buildEdgeList(int cnt,dePt *pts,Edge *edges[])
{
	Edge *edge;
	dePt v1,v2;
	int i,yPrev=pts[cnt-2].y;
	v1.x = pts[cnt-1].x;
	v1.y = pts[cnt-1].y;
	for(i=0;i<cnt;i++){
		v2 = pts[i];
		if(v1.y!=v2.y){
			edge = (Edge*)malloc(sizeof(edge));
			if(v1.y<v2.y)
				makeEdgeRec(v1,v2,yNext(i,cnt,pts),edge,edges);
			else
				makeEdgeRec(v2,v1,yPrev,edge,edges);
		}
		yPrev = v1.y;
		v1 = v2;
	}
}

void buildActiveList(int scan,Edge *active,Edge *edges[])
{
	Edge *p,*q;
	p = edges[scan]->next;
	while(p){
		q = p->next;
		insertEdge(active,p);
		p = q;
	}
}

void fillScan(int scan,Edge *active)
{
	Edge *p1,*p2;
	int i;
	p1 = active->next;
	while(p1){
		p2 = p1->next;
		for(i=p1->xIntersect;i<p2->xIntersect;i++)
			setpixel((int)i,scan);
		p1 = p2->next;
	}
}

void deleteAfter(Edge *q)
{
	Edge *p = q->next;
	q->next = p->next;
	//free(p);
}

void updateActiveList(int scan,Edge *active)
{
	Edge *q=active,*p=active->next;
	while(p)
		if(scan>=p->yUpper){
			p=p->next;
			deleteAfter(q);
		}else{
			p->xIntersect=p->xIntersect+p->dxPerScan;
			q=p;
			p=p->next;
		}
}

void resortActiveList(Edge *active)
{
	Edge *q,*p=active->next;
	active->next = NULL;
	while(p){
		q=p->next;
		insertEdge(active,p);
		p=q;
	}
}

void scanFill(int cnt,dePt *pts)
{
	Edge *edges[WINDOW_HEIGHT],*active,*p,*q;
	int i,scan;
	for(i=0;i<WINDOW_HEIGHT;i++){
		edges[i]=(Edge*)malloc(sizeof(Edge));
		edges[i]->next=NULL;
	}
	buildEdgeList(cnt,pts,edges);
	active=(Edge*)malloc(sizeof(Edge));
	active->next=NULL;
	for(scan=0;scan<WINDOW_HEIGHT;scan++){
		buildActiveList(scan,active,edges);
		if(active->next){
			fillScan(scan,active);
			updateActiveList(scan,active);
			resortActiveList(active);
		}
	}
	free(active);
}

/////////////////////////////////////////////////////////////////////////////
// CFillView

IMPLEMENT_DYNCREATE(CFillView, CView)

BEGIN_MESSAGE_MAP(CFillView, CView)
	//{{AFX_MSG_MAP(CFillView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_TRI, OnTri)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillView construction/destruction

CFillView::CFillView()
{
	// TODO: add construction code here
	m_x1=m_y1=0;
	m_x2=10;
	m_y2=20;
	m_x3=20;
	m_y3=10;
	m_flag=0;

}

CFillView::~CFillView()
{
}

BOOL CFillView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CFillView drawing

void CFillView::OnDraw(CDC* pDC)
{
	CFillDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	wglMakeCurrent(pDC->m_hDC,hglrc);
	display();
	wglMakeCurrent(pDC->m_hDC,NULL);
	SwapBuffers(pDC->m_hDC);
	
}

/////////////////////////////////////////////////////////////////////////////
// CFillView printing

BOOL CFillView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFillView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFillView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CFillView diagnostics

#ifdef _DEBUG
void CFillView::AssertValid() const
{
	CView::AssertValid();
}

void CFillView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFillDoc* CFillView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFillDoc)));
	return (CFillDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFillView message handlers

int CFillView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CFillView::OnDestroy() 
{
	CView::OnDestroy();
	wglDeleteContext(hglrc);
}

void CFillView::OnSize(UINT nType, int cx, int cy) 
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

void CFillView::init()
{
	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void CFillView::display()
{
	float x,y,x1,y1,x2,y2,x3,y3;
	int i,j;
	init();

	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_LINES);
		glVertex2f(-1.0,0.0);
		glVertex2f(1.0,0.0);
		glVertex2f(0.0,-1.0);
		glVertex2f(0.0,1.0);
	glEnd();

	x1 = (float)m_x1/m_centerx;
	y1 = (float)m_y1/m_centery;
	x2 = (float)m_x2/m_centerx;
	y2 = (float)m_y2/m_centery;
	x3 = (float)m_x3/m_centerx;
	y3 = (float)m_y3/m_centery;

	glColor3f(0.0f,1.0f,0.0f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(x1,y1);
		glVertex2f(x2,y2);
		glVertex2f(x3,y3);
	glEnd();

	if(m_flag==1){
		glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_POINTS);
		for(i=0;i<WINDOW_WIDTH;i++)
			for(j=0;j<WINDOW_HEIGHT;j++)
				if(*(window+j*WINDOW_WIDTH+i)==1){
					x = (float)i/m_centerx;
					y = (float)j/m_centery;
					glVertex2f(x,y);
				}
		glEnd();
		m_flag=0;
	}

	glColor3f(0.7f,0.8f,0.0f);
	glFlush();
	
}


void CFillView::OnTri() 
{
	CTrigle dlg;

	dlg.m_x1 = m_x1;
	dlg.m_y1 = m_y1;
	dlg.m_x2 = m_x2;
	dlg.m_y2 = m_y2;
	dlg.m_x3 = m_x3;
	dlg.m_y3 = m_y3;
	if(dlg.DoModal()==IDOK){
		m_x1=dlg.m_x1;
		m_y1=dlg.m_y1;
		m_x2=dlg.m_x2;
		m_y2=dlg.m_y2;
		m_x3=dlg.m_x3;
		m_y3=dlg.m_y3;
		memset(window,0,WINDOW_WIDTH*WINDOW_HEIGHT*sizeof(int));
		m_pts[0].x = m_x1;
		m_pts[0].y = m_y1;
		m_pts[1].x = m_x2;
		m_pts[1].y = m_y2;
		m_pts[2].x = m_x3;
		m_pts[2].y = m_y3;
		m_flag = 1;
		scanFill(3,m_pts);	
		Invalidate();
	}	
}
