// FatalLogDoc.cpp : implementation of the CFatalLogDoc class
//

#include "stdafx.h"
#include "FatalLog.h"

#include "FatalLogDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFatalLogDoc

IMPLEMENT_DYNCREATE(CFatalLogDoc, CDocument)

BEGIN_MESSAGE_MAP(CFatalLogDoc, CDocument)
	//{{AFX_MSG_MAP(CFatalLogDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFatalLogDoc construction/destruction

CFatalLogDoc::CFatalLogDoc()
{
	// TODO: add one-time construction code here

}

CFatalLogDoc::~CFatalLogDoc()
{
}

BOOL CFatalLogDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFatalLogDoc serialization

void CFatalLogDoc::Serialize(CArchive& ar)
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
// CFatalLogDoc diagnostics

#ifdef _DEBUG
void CFatalLogDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFatalLogDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFatalLogDoc commands
