// FatalLogDoc.h : interface of the CFatalLogDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FATALLOGDOC_H__5951D4C1_9454_4D3D_BDC4_6326DCDFC310__INCLUDED_)
#define AFX_FATALLOGDOC_H__5951D4C1_9454_4D3D_BDC4_6326DCDFC310__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFatalLogDoc : public CDocument
{
protected: // create from serialization only
	CFatalLogDoc();
	DECLARE_DYNCREATE(CFatalLogDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFatalLogDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFatalLogDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFatalLogDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FATALLOGDOC_H__5951D4C1_9454_4D3D_BDC4_6326DCDFC310__INCLUDED_)
