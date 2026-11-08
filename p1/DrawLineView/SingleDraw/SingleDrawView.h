// SingleDrawView.h : interface of the CSingleDrawView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEDRAWVIEW_H__AC47B564_5431_4C50_ADC2_BA5DE6F5F1E0__INCLUDED_)
#define AFX_SINGLEDRAWVIEW_H__AC47B564_5431_4C50_ADC2_BA5DE6F5F1E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSingleDrawView : public CView
{
protected: // create from serialization only
	CSingleDrawView();
	DECLARE_DYNCREATE(CSingleDrawView)

// Attributes
public:
	CSingleDrawDoc* GetDocument();

// Operations
public:

private:
	CPoint p1,p2;
	int mDragging;
	void saveLine(CPoint &p1,CPoint &p2);

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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SingleDrawView.cpp
inline CSingleDrawDoc* CSingleDrawView::GetDocument()
   { return (CSingleDrawDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEDRAWVIEW_H__AC47B564_5431_4C50_ADC2_BA5DE6F5F1E0__INCLUDED_)
