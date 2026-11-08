// MouseLineView.h : interface of the CMouseLineView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSELINEVIEW_H__74D69686_C5F9_424B_B46B_96E7A489D683__INCLUDED_)
#define AFX_MOUSELINEVIEW_H__74D69686_C5F9_424B_B46B_96E7A489D683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMouseLineView : public CView
{
protected: // create from serialization only
	CMouseLineView();
	DECLARE_DYNCREATE(CMouseLineView)

// Attributes
public:
	CMouseLineDoc* GetDocument();
	CPoint p1,p2;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMouseLineView)
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
	virtual ~CMouseLineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMouseLineView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MouseLineView.cpp
inline CMouseLineDoc* CMouseLineView::GetDocument()
   { return (CMouseLineDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUSELINEVIEW_H__74D69686_C5F9_424B_B46B_96E7A489D683__INCLUDED_)
