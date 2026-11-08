// SingleDrawDoc.cpp : implementation of the CSingleDrawDoc class
//

#include "stdafx.h"
#include "SingleDraw.h"

#include "SingleDrawDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawDoc

IMPLEMENT_DYNCREATE(CSingleDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CSingleDrawDoc, CDocument)
	//{{AFX_MSG_MAP(CSingleDrawDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawDoc construction/destruction

CSingleDrawDoc::CSingleDrawDoc()
{
	// TODO: add one-time construction code here

}

CSingleDrawDoc::~CSingleDrawDoc()
{
}

BOOL CSingleDrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSingleDrawDoc serialization

void CSingleDrawDoc::Serialize(CArchive& ar)
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
// CSingleDrawDoc diagnostics

#ifdef _DEBUG
void CSingleDrawDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSingleDrawDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSingleDrawDoc commands

linkList<CLine> &CSingleDrawDoc::getLine()
{
	return Line;
}