// SingleDrawView.h : interface of the CSingleDrawView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEDRAWVIEW_H__946EB1AD_5EFB_4FBF_A471_5DD2B9FC7CFB__INCLUDED_)
#define AFX_SINGLEDRAWVIEW_H__946EB1AD_5EFB_4FBF_A471_5DD2B9FC7CFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include"Line.h"
#include"DiaLineWith2P.h"


class CSingleDrawView : public CView
{
protected: // create from serialization only
	CSingleDrawView();
	DECLARE_DYNCREATE(CSingleDrawView)

// Attributes
public:
	CSingleDraw2Doc* GetDocument();

private:
	CPoint p1,p2;
	int mDragging;
	void saveLine(CPoint &p1,CPoint &p2);
	void saveLine(int startx,int starty,int endx,int endy);
	CDiaLineWith2P dlg;
	void drawLine(CPoint &startP,int nDrawMode1,CPoint &endP1,
		int nDrawMode2,CPoint &endP2);
	void drawLine(int nDrawMode,CPoint &startp,CPoint &endp);


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleDrawView)
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
	virtual ~CSingleDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSingleDrawView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SingleDrawView.cpp
inline CSingleDraw2Doc* CSingleDrawView::GetDocument()
   { return (CSingleDraw2Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEDRAWVIEW_H__946EB1AD_5EFB_4FBF_A471_5DD2B9FC7CFB__INCLUDED_)
