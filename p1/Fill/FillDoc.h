// FillDoc.h : interface of the CFillDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILLDOC_H__961A1B71_6FA2_4CFA_8AAD_8866DF4066DE__INCLUDED_)
#define AFX_FILLDOC_H__961A1B71_6FA2_4CFA_8AAD_8866DF4066DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFillDoc : public CDocument
{
protected: // create from serialization only
	CFillDoc();
	DECLARE_DYNCREATE(CFillDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFillDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFillDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILLDOC_H__961A1B71_6FA2_4CFA_8AAD_8866DF4066DE__INCLUDED_)
