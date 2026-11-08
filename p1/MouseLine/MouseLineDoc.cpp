// MouseLineDoc.cpp : implementation of the CMouseLineDoc class
//

#include "stdafx.h"
#include "MouseLine.h"

#include "MouseLineDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMouseLineDoc

IMPLEMENT_DYNCREATE(CMouseLineDoc, CDocument)

BEGIN_MESSAGE_MAP(CMouseLineDoc, CDocument)
	//{{AFX_MSG_MAP(CMouseLineDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMouseLineDoc construction/destruction

CMouseLineDoc::CMouseLineDoc()
{
	// TODO: add one-time construction code here

}

CMouseLineDoc::~CMouseLineDoc()
{
}

BOOL CMouseLineDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMouseLineDoc serialization

void CMouseLineDoc::Serialize(CArchive& ar)
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
// CMouseLineDoc diagnostics

#ifdef _DEBUG
void CMouseLineDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMouseLineDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMouseLineDoc commands
