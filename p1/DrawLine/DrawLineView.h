// DrawLineView.h : interface of the CDrawLineView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWLINEVIEW_H__31F03161_B79D_4F07_9F3E_04603A7443D6__INCLUDED_)
#define AFX_DRAWLINEVIEW_H__31F03161_B79D_4F07_9F3E_04603A7443D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

class CDrawLineView : public CView
{
protected: // create from serialization only
	CDrawLineView();
	DECLARE_DYNCREATE(CDrawLineView)

// Attributes
public:
	CDrawLineDoc* GetDocument();

// Operations
public:
	int m_centerx,m_centery;
	int m_sx,m_sy,m_ex,m_ey;
	int m_flag;
	int m_cx,m_cy,m_r;

private:
	HGLRC hglrc;
	void init();
	void display();

	void getPoints(int sx,int sy,int ex,int ey,int *x,int *y);
	void getCirclePoints(int cx,int cy,int r,int *x,int *y,int *n);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawLineView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawLineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawLineView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDlgpoints();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCircle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DrawLineView.cpp
inline CDrawLineDoc* CDrawLineView::GetDocument()
   { return (CDrawLineDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWLINEVIEW_H__31F03161_B79D_4F07_9F3E_04603A7443D6__INCLUDED_)
