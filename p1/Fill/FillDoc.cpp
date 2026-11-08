// FillDoc.cpp : implementation of the CFillDoc class
//

#include "stdafx.h"
#include "Fill.h"

#include "FillDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillDoc

IMPLEMENT_DYNCREATE(CFillDoc, CDocument)

BEGIN_MESSAGE_MAP(CFillDoc, CDocument)
	//{{AFX_MSG_MAP(CFillDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillDoc construction/destruction

CFillDoc::CFillDoc()
{
	// TODO: add one-time construction code here

}

CFillDoc::~CFillDoc()
{
}

BOOL CFillDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFillDoc serialization

void CFillDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFillDoc diagnostics

#ifdef _DEBUG
void CFillDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFillDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFillDoc commands
