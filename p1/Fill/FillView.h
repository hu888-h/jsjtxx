// FillView.h : interface of the CFillView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILLVIEW_H__C4B06FE7_143E_4319_86ED_331C06D8B441__INCLUDED_)
#define AFX_FILLVIEW_H__C4B06FE7_143E_4319_86ED_331C06D8B441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

typedef struct dePt{
	int x,y;
}dePt;

class CFillView : public CView
{
protected: // create from serialization only
	CFillView();
	DECLARE_DYNCREATE(CFillView)

// Attributes
public:
	CFillDoc* GetDocument();

// Operations
public:
	int m_centerx,m_centery;
	int m_x1,m_y1;
	int m_x2,m_y2;
	int m_x3,m_y3;
	dePt m_pts[3];
	int m_flag;

private:
	HGLRC hglrc;
	void init();
	void display();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillView)
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
	virtual ~CFillView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFillView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTri();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FillView.cpp
inline CFillDoc* CFillView::GetDocument()
   { return (CFillDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILLVIEW_H__C4B06FE7_143E_4319_86ED_331C06D8B441__INCLUDED_)
