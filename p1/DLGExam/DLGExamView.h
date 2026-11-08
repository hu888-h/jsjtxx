// DLGExamView.h : interface of the CDLGExamView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGEXAMVIEW_H__193C61E8_A524_459D_9CDF_6F1C9B5366B8__INCLUDED_)
#define AFX_DLGEXAMVIEW_H__193C61E8_A524_459D_9CDF_6F1C9B5366B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDLGExamView : public CView
{
protected: // create from serialization only
	CDLGExamView();
	DECLARE_DYNCREATE(CDLGExamView)

// Attributes
public:
	CDLGExamDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLGExamView)
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
	virtual ~CDLGExamView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDLGExamView)
	afx_msg void OnModel();
	afx_msg void OnNomodel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DLGExamView.cpp
inline CDLGExamDoc* CDLGExamView::GetDocument()
   { return (CDLGExamDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXAMVIEW_H__193C61E8_A524_459D_9CDF_6F1C9B5366B8__INCLUDED_)
