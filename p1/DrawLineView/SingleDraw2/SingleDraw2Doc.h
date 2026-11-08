// SingleDraw2Doc.h : interface of the CSingleDraw2Doc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEDRAW2DOC_H__F59783E1_5958_4EEB_B6C2_67D57659B96C__INCLUDED_)
#define AFX_SINGLEDRAW2DOC_H__F59783E1_5958_4EEB_B6C2_67D57659B96C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Line.h"


class CSingleDraw2Doc : public CDocument
{
protected: // create from serialization only
	CSingleDraw2Doc();
	DECLARE_DYNCREATE(CSingleDraw2Doc)

// Attributes
public:
	linkList<CLine> &getLine();

private:
	linkList<CLine> Line;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleDraw2Doc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSingleDraw2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSingleDraw2Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEDRAW2DOC_H__F59783E1_5958_4EEB_B6C2_67D57659B96C__INCLUDED_)
