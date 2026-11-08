// SingleDraw2Doc.cpp : implementation of the CSingleDraw2Doc class
//

#include "stdafx.h"
#include "SingleDraw2.h"

#include "SingleDraw2Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSingleDraw2Doc

IMPLEMENT_DYNCREATE(CSingleDraw2Doc, CDocument)

BEGIN_MESSAGE_MAP(CSingleDraw2Doc, CDocument)
	//{{AFX_MSG_MAP(CSingleDraw2Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSingleDraw2Doc construction/destruction

CSingleDraw2Doc::CSingleDraw2Doc()
{
	// TODO: add one-time construction code here

}

CSingleDraw2Doc::~CSingleDraw2Doc()
{
}

BOOL CSingleDraw2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSingleDraw2Doc serialization

void CSingleDraw2Doc::Serialize(CArchive& ar)
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
// CSingleDraw2Doc diagnostics

#ifdef _DEBUG
void CSingleDraw2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSingleDraw2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSingleDraw2Doc commands

linkList<CLine> &CSingleDraw2Doc::getLine()
{
	return Line;
}