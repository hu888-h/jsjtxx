// SingleDrawDoc.h : interface of the CSingleDrawDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEDRAWDOC_H__00EA572E_A33D_4C16_BBBB_ACCE2D17E0E2__INCLUDED_)
#define AFX_SINGLEDRAWDOC_H__00EA572E_A33D_4C16_BBBB_ACCE2D17E0E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Line.h"

class CSingleDrawDoc : public CDocument
{
protected: // create from serialization only
	CSingleDrawDoc();
	DECLARE_DYNCREATE(CSingleDrawDoc)

// Attributes
public:
	linkList<CLine> &getLine();

private:
	linkList<CLine> Line;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleDrawDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSingleDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSingleDrawDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEDRAWDOC_H__00EA572E_A33D_4C16_BBBB_ACCE2D17E0E2__INCLUDED_)
