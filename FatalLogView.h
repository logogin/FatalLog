// FatalLogView.h : interface of the CFatalLogView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FATALLOGVIEW_H__99DC2D6C_3B1A_43E0_BCD9_9B464ABB5C98__INCLUDED_)
#define AFX_FATALLOGVIEW_H__99DC2D6C_3B1A_43E0_BCD9_9B464ABB5C98__INCLUDED_

#include "HtmlDoc.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _Move
{
	_Move(const UINT uiHit,const UINT uiType,const CString &sOpp,const CString &sTime)
		:uiMoveHit(uiHit),uiMoveType(uiType),sMoveTime(sTime),sOppName(sOpp)
	{
	
	}
	UINT uiMoveHit;
	UINT uiMoveType;
	CString sMoveTime;
	CString sOppName;
} Move;

typedef struct _TeamMember
{
	_TeamMember(const CString &Name,const CString &ID):sMemberName(Name),sMemberID(ID)
	{

	}

	CString sMemberName;
	CString sMemberID;
	std::list<Move> lstMoveSequence;
} TeamMember;

class CFatalLogView : public CHtmlView
{
protected: // create from serialization only
	CFatalLogView();
	DECLARE_DYNCREATE(CFatalLogView)

// Attributes
public:
	CFatalLogDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFatalLogView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_sLogAddress;
	CString m_sGlobalPos;
	BOOL m_bNewMoves;
	UINT m_uiRefreshTimer;
	BOOL m_bFirstRead;
	void ProcessLog(void);
	BOOL m_bAutoRefresh;
	void CreateSequenceTable(void);
	CString GenerateString(std::list<Move> lstSequence);
	void UpdateSequence(const UINT uiTeam,
		const CString &sMemberName,
		const UINT uiBodyPart,
		const UINT uiMoveType,
		const CString &sOppName,
		const CString &sMoveTime,
		const UINT uiDamage);
	void GetMoveType(CString &sData,UINT *lpBodyPart,UINT *lpMoveType,UINT *lpDamage);
	UINT GetMoveMembers(const UINT uiPos,UINT *lpMembers,CString *lpstMembers);
	void GetSequence(void);
	UINT m_uiGlobalPos;
	UINT GetPages(const BOOL bUpdateGlobalPos=FALSE);
	CString m_sFileData;
	void GetTeamMembers(const BOOL bUpdateGlobalPos=FALSE);
	CString m_sStartDate;
	CString GetStartDate(const BOOL bUpdateGlobalPos=FALSE);
	virtual ~CFatalLogView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	CHtmlDoc m_CHtmlDoc;
	std::list<TeamMember> m_lstTeamMembers[2];
	//{{AFX_MSG(CFatalLogView)
	afx_msg void OnCheckRefresh();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonRefresh();
	afx_msg void OnKillfocusEditTime();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FatalLogView.cpp
inline CFatalLogDoc* CFatalLogView::GetDocument()
   { return (CFatalLogDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FATALLOGVIEW_H__99DC2D6C_3B1A_43E0_BCD9_9B464ABB5C98__INCLUDED_)
