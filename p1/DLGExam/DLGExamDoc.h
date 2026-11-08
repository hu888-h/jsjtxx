// DLGExamDoc.h : interface of the CDLGExamDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGEXAMDOC_H__9EFC9182_A9A8_4959_BE3B_C9D8B796AB5D__INCLUDED_)
#define AFX_DLGEXAMDOC_H__9EFC9182_A9A8_4959_BE3B_C9D8B796AB5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDLGExamDoc : public CDocument
{
protected: // create from serialization only
	CDLGExamDoc();
	DECLARE_DYNCREATE(CDLGExamDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLGExamDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDLGExamDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDLGExamDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXAMDOC_H__9EFC9182_A9A8_4959_BE3B_C9D8B796AB5D__INCLUDED_)
