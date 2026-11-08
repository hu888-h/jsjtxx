// MouseLineDoc.h : interface of the CMouseLineDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSELINEDOC_H__60324F6E_0848_46EF_8CBE_CA1BA98FC1B4__INCLUDED_)
#define AFX_MOUSELINEDOC_H__60324F6E_0848_46EF_8CBE_CA1BA98FC1B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMouseLineDoc : public CDocument
{
protected: // create from serialization only
	CMouseLineDoc();
	DECLARE_DYNCREATE(CMouseLineDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMouseLineDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMouseLineDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMouseLineDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUSELINEDOC_H__60324F6E_0848_46EF_8CBE_CA1BA98FC1B4__INCLUDED_)
