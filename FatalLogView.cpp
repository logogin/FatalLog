// FatalLogView.cpp : implementation of the CFatalLogView class
//

#include "stdafx.h"
#include "FatalLog.h"

#include "FatalLogDoc.h"
#include "FatalLogView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define DEBUG_ON_LINE

/////////////////////////////////////////////////////////////////////////////
// CFatalLogView

IMPLEMENT_DYNCREATE(CFatalLogView, CHtmlView)

BEGIN_MESSAGE_MAP(CFatalLogView, CHtmlView)
	//{{AFX_MSG_MAP(CFatalLogView)
	ON_BN_CLICKED(IDC_CHECK_REFRESH, OnCheckRefresh)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_EN_KILLFOCUS(IDC_EDIT_TIME, OnKillfocusEditTime)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFatalLogView construction/destruction

CFatalLogView::CFatalLogView()
{
	// TODO: add construction code here

}

CFatalLogView::~CFatalLogView()
{
}

BOOL CFatalLogView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CHtmlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CFatalLogView drawing

void CFatalLogView::OnDraw(CDC* pDC)
{
	CFatalLogDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CFatalLogView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	m_bFirstRead=TRUE;
	m_bNewMoves=FALSE;
	m_sLogAddress=_T("");

	CFatalLogDoc* pDoc = GetDocument();
	CMainFrame *pMainFrame = (CMainFrame *)GetParentFrame();
	CDialogBar *pDlgBar=(CDialogBar *)pMainFrame->GetDlgItem(IDD_DIALOGBAR);
	CSpinButtonCtrl *pSpin=(CSpinButtonCtrl *)pDlgBar->GetDlgItem(IDC_SPIN_TIME);
	pDlgBar->SetDlgItemText(IDC_EDIT_TIME,_T("10"));
	pSpin->SetRange(10,600);
	m_bAutoRefresh=FALSE;
	pDlgBar->CheckDlgButton(IDC_CHECK_REFRESH,TRUE);
	OnCheckRefresh();

	// TODO: This code navigates to a popular spot on the web.
	//  change the code to go where you'd like.
	//Navigate2(_T("d:/work/Temp/FatalLog/Pages/logs[1].html"),NULL,NULL);
	LoadFromResource(IDR_HTMLSTAT);
}

/////////////////////////////////////////////////////////////////////////////
// CFatalLogView diagnostics

#ifdef _DEBUG
void CFatalLogView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CFatalLogView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CFatalLogDoc* CFatalLogView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFatalLogDoc)));
	return (CFatalLogDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFatalLogView message handlers

void CFatalLogView::OnDocumentComplete(LPCTSTR lpszURL) 
{
	// TODO: Add your specialized code here and/or call the base class
		
	LPDISPATCH pDispatch=NULL;

	pDispatch=GetHtmlDocument();
	if (pDispatch!=NULL

#ifdef DEBUG_ON_LINE

		&&m_sLogAddress!=_T("")

#endif DEBUG_ON_LINE

		&&m_bNewMoves)
	{
		m_CHtmlDoc.SetHtmlDocument(pDispatch);
		CreateSequenceTable();
		if (m_bAutoRefresh)
		{	
			CFatalLogDoc* pDoc = GetDocument();
			CMainFrame *pMainFrame = (CMainFrame *)GetParentFrame();
			CDialogBar *pDlgBar=(CDialogBar *)pMainFrame->GetDlgItem(IDD_DIALOGBAR);
		
			m_uiRefreshTimer=SetTimer(1,(pDlgBar->GetDlgItemInt(IDC_EDIT_TIME))*1000,NULL);
		}
	}

	CHtmlView::OnDocumentComplete(lpszURL);
}

CString CFatalLogView::GetStartDate(const BOOL bUpdateGlobalPos)
{
	CString sResult=_T("");
	UINT uiPos1;
	UINT uiPos2;

	uiPos1=m_sFileData.Find(_T("<font class=date>"),m_uiGlobalPos);
	uiPos2=m_sFileData.Find(_T(">"),uiPos1);
	uiPos1=m_sFileData.Find(_T(" "),uiPos2);
	uiPos2=m_sFileData.Find(_T("<"),uiPos1);
	sResult=m_sFileData.Mid(uiPos1+1,uiPos2-(uiPos1+1));
	if (bUpdateGlobalPos)
		m_uiGlobalPos=uiPos2;
	return sResult;
}

void CFatalLogView::GetTeamMembers(const BOOL bUpdateGlobalPos)
{
	UINT uiPos1;
	UINT uiPos2;
	CString sMember;
	CString sMemberID;

	UINT uiTerminator=m_sFileData.Find(_T("<font class=B2>"),m_uiGlobalPos);

	uiPos2=m_sFileData.Find(_T("<font class=B1>"),m_uiGlobalPos);
	uiPos1=m_sFileData.Find(_T("<B>"),uiPos2);
	/*First Team*/
	while (uiPos1<uiTerminator&&uiPos1!=-1)
	{
		/*Memeber Name*/
		uiPos2=m_sFileData.Find(_T(">"),uiPos1+1);
		uiPos1=m_sFileData.Find(_T("</B>"),uiPos2);
		sMember=m_sFileData.Mid(uiPos2+1,uiPos1-(uiPos2+1));
		/*Member ID*/
		uiPos2=m_sFileData.Find(_T("?"),uiPos1);
		uiPos1=m_sFileData.Find(_T(" target=_blank"),uiPos2);
		sMemberID=m_sFileData.Mid(uiPos2+1,uiPos1-(uiPos2+1));
		TeamMember Temp(sMember,sMemberID);
		m_lstTeamMembers[0].push_back(Temp);
		uiPos1=m_sFileData.Find(_T("<B>"),uiPos2);
	}

	uiPos2=m_sFileData.Find(_T("<font class=B2>"),m_uiGlobalPos);
	uiTerminator=m_sFileData.Find(_T("<bR><font class=date>"),uiPos2);
	uiPos1=m_sFileData.Find(_T("<B>"),uiPos2);
	/*Second Team*/
	while (uiPos1<uiTerminator&&uiPos1!=-1)
	{
		/*Member Name*/
		uiPos2=m_sFileData.Find(_T(">"),uiPos1+1);
		uiPos1=m_sFileData.Find(_T("</B>"),uiPos2);
		sMember=m_sFileData.Mid(uiPos2+1,uiPos1-(uiPos2+1));
		/*Member ID*/
		uiPos2=m_sFileData.Find(_T("?"),uiPos1);
		uiPos1=m_sFileData.Find(_T(" target=_blank"),uiPos2);
		sMemberID=m_sFileData.Mid(uiPos2+1,uiPos1-(uiPos2+1));
		TeamMember Temp(sMember,sMemberID);
		m_lstTeamMembers[1].push_back(Temp);
		uiPos1=m_sFileData.Find(_T("<B>"),uiPos2);
	}
	if (bUpdateGlobalPos)
		m_uiGlobalPos=uiTerminator;
}

UINT CFatalLogView::GetPages(const BOOL bUpdateGlobalPos)
{
	const TCHAR _PAGES[]="Страницы: ";
	UINT uiPos1;
	UINT uiPos2;
	UINT uiResult=1;
	CString sResult;

	if ((uiPos1=m_sFileData.Find(_PAGES))==-1)
		return uiResult;

	uiPos2=m_sFileData.Find(_T("<font class=number>"),uiPos1);
	uiPos1=m_sFileData.Find(_T(">"),uiPos2);
	uiPos2=m_sFileData.Find(_T("<"),uiPos1);
	sResult=m_sFileData.Mid(uiPos1+1,uiPos2-(uiPos1+1));
	uiResult=_ttoi(sResult);
	if (bUpdateGlobalPos)
		m_uiGlobalPos=uiPos2;
	return uiResult;
}

void CFatalLogView::GetSequence()
{
	

	UINT uiPos1;
	UINT uiPos2;
	UINT uiNextMove;
	UINT uiBodyPart;
	UINT uiMoveType;
	UINT uiDamage;
	UINT lpMembers[2]={0,0};
	CString lpstMembers[2];
	CString sCurTime;
	CString sText;

	/*Move Time*/
	uiPos1=m_sFileData.Find(_T("<font class=date>"),m_uiGlobalPos);
	while (uiPos1!=-1)
	{
		m_bNewMoves=TRUE;
		uiPos2=m_sFileData.Find(_T(">"),uiPos1);
		uiPos1=m_sFileData.Find(_T("</font>"),uiPos2);
		sCurTime=m_sFileData.Mid(uiPos2+1,uiPos1-(uiPos2+1));

		/*Save Global Point*/
		uiNextMove=m_sFileData.Find(_T("<bR>"),uiPos2);
		m_sGlobalPos=m_sFileData.Mid(uiPos2+1,uiNextMove-(uiPos2+1));

		/*Members*/
		uiPos1=GetMoveMembers(uiPos1,lpMembers,lpstMembers);

		/*Move Data*/
		uiPos2=m_sFileData.Find(_T(">"),uiPos1);
		
		if (uiPos1==-1||uiPos1>uiNextMove)
		{
			AfxMessageBox(_T("Unexpected tokens!!!"),MB_OK);
			break;
		}

		sText=m_sFileData.Mid(uiPos2+1,uiNextMove-(uiPos2+1));

		/*Move Recognition*/
		GetMoveType(sText,&uiBodyPart,&uiMoveType,&uiDamage);

		/*Update Sequence*/
		if (!uiMoveType||uiMoveType==6)
			UpdateSequence(lpMembers[1],lpstMembers[1],uiBodyPart,uiMoveType,
				lpstMembers[0],sCurTime,uiDamage);
		else
			UpdateSequence(lpMembers[0],lpstMembers[0],uiBodyPart,uiMoveType,
				lpstMembers[1],sCurTime,uiDamage);
		
		uiPos1=m_sFileData.Find(_T("<font class=date>"),uiNextMove);
	}
	m_uiGlobalPos=0;
}

UINT CFatalLogView::GetMoveMembers(const UINT uiPos,UINT *lpMembers, CString *lpstMembers)
{
	UINT uiPos1=uiPos;
	UINT uiPos2;
	for (UINT i=0; i<2; i++)
	{
		uiPos2=m_sFileData.Find(_T("<font class=B"),uiPos1);
		uiPos1=m_sFileData.Find(_T("B"),uiPos2);
		uiPos2=m_sFileData.Find(_T(">"),uiPos1);

		lpMembers[i]=_ttoi(m_sFileData.Mid(uiPos1+1,uiPos2-(uiPos1+1)))-1;

		uiPos1=m_sFileData.Find(_T("</font>"),uiPos2);

		lpstMembers[i]=m_sFileData.Mid(uiPos2+1,uiPos1-(uiPos2+1));
	}
	return uiPos1;
}

void CFatalLogView::GetMoveType(CString &sData, UINT *lpBodyPart, UINT *lpMoveType, UINT *lpDamage)
{
	const UINT _HEAD_SIZE=15;
	const UINT _TORSO_SIZE=7;
	const UINT _GROIN_SIZE=7;
	const UINT _LEGS_SIZE=8;

	const _BLOCK_SIZE=5;
	const _DODGE_SIZE=5;
	const _HIT_SIZE=6;

	const TCHAR *_HEAD[_HEAD_SIZE]={" глаз"," лоб"," нос"," затылок"," щекам"," кадык"," голову",
		" ноздрю"," репу"," горло"," челюсть"," шею"," подбородок"," темечко"," ухо"};
	const TCHAR *_TORSO[_TORSO_SIZE]={" грудь"," корпус"," сплетение"," плечо"," ключицу",
		" сердце"," ребрам"};
	const TCHAR *_GROIN[_GROIN_SIZE]={" пупок"," пах"," печень"," почкам"," <вырезано цензурой>",
		" живот"," куда обычно не бьют"};
	const TCHAR *_LEGS[_LEGS_SIZE]={" колено"," ноге"," голень"," лодыжку"," ногам"," чашечке",
		" пятку"," бедрам"};

	const TCHAR *_BLOCK[_BLOCK_SIZE]={"увел","остановил","блок","отбил","парировал"};
	const TCHAR *_DODGE[_DODGE_SIZE]={"ушел","ушла","увернул","отпрыгнул","уклонил"};
	const TCHAR *_HIT[_HIT_SIZE]={"саданул","влепил","провел","вломил","нанес","приложил"};

	BOOL bCrit=FALSE;
	BOOL bHit=FALSE;
	BOOL bBlock=FALSE;
	BOOL bDodge=FALSE;
	BOOL bFound=FALSE;

	UINT uiPos1;
	UINT uiPos2;
	UINT uiType;

	*lpDamage=0;

	/*Hit*/
	for (UINT i=0; i<_HIT_SIZE&&!bHit; i++)
		if (sData.Find(_HIT[i])!=-1)
		{
			bHit=TRUE;
			uiType=0;
		}

	/*Block*/
	for (i=0; i<_BLOCK_SIZE&&!bBlock; i++)
		if (sData.Find(_BLOCK[i])!=-1)
		{
			bBlock=TRUE;
			uiType=2;
		}
	/*Dodge*/
	for (i=0; i<_DODGE_SIZE&&!bDodge; i++)
		if (sData.Find(_DODGE[i])!=-1)
		{
			bDodge=TRUE;
			uiType=4;
		}

	/*Ctritical Hit*/
	if (sData.Find(_T("Color=red"))!=-1)
	{
		bCrit=TRUE;
		uiType=6;
	}

	/*Regular Hit*/
	if (!bCrit&&sData.Find(_T("Color=#006699"))!=-1)
		bHit=TRUE;

	/*Damage*/
	if (bCrit||bHit)
	{
		uiPos1=sData.Find(_T("<b>"));
		uiPos2=sData.Find(_T("-"),uiPos1);
		uiPos1=sData.Find(_T("<"),uiPos2);
		*lpDamage=_ttoi(sData.Mid(uiPos2+1,uiPos1-(uiPos2+1)));
	}

	/*Body Part Recognition*/

	/*Head*/
	bFound=FALSE;
	for (i=0; i<_HEAD_SIZE&&!bFound; i++)
		if (sData.Find(_HEAD[i])!=-1)
		{
			bFound=TRUE;
			*lpBodyPart=0;
		}

	/*Torso*/
	for (i=0; i<_TORSO_SIZE&&!bFound; i++)
		if (sData.Find(_TORSO[i])!=-1)
		{
			bFound=TRUE;
			*lpBodyPart=1;
		}

	/*Groin*/
	for (i=0; i<_GROIN_SIZE&&!bFound; i++)
		if (sData.Find(_GROIN[i])!=-1)
		{
			bFound=TRUE;
			*lpBodyPart=2;
		}

	/*Legs*/
	for (i=0; i<_LEGS_SIZE&&!bFound; i++)
		if (sData.Find(_LEGS[i])!=-1)
		{
			bFound=TRUE;
			*lpBodyPart=3;
		}
	*lpMoveType=uiType;
}

void CFatalLogView::UpdateSequence(const UINT uiTeam,
								   const CString &sMemberName,
								   const UINT uiBodyPart,
								   const UINT uiMoveType,
								   const CString &sOppName,
								   const CString &sMoveTime,
								   const UINT uiDamage)
{
	BOOL bFound=FALSE;
	std::list<TeamMember>::iterator iter;
	for (
		iter=m_lstTeamMembers[uiTeam].begin();
		iter!=m_lstTeamMembers[uiTeam].end()&&!bFound;
		iter++)

		if (iter->sMemberName==sMemberName)
		{
			bFound=TRUE;
			Move Temp(uiBodyPart,uiMoveType,sOppName,sMoveTime);
			iter->lstMoveSequence.push_back(Temp);
		}
	if (!bFound)
	{
		TeamMember TeamTemp(sMemberName,_T(""));
		Move MoveTemp(uiBodyPart,uiMoveType,sOppName,sMoveTime);
		TeamTemp.lstMoveSequence.push_back(MoveTemp);
		m_lstTeamMembers[uiTeam].push_back(TeamTemp);
	}
}

CString CFatalLogView::GenerateString(std::list<Move> lstSequence)
{
	const TCHAR *_PARTS[4]={
		_T("to head"),
		_T("to torso"),
		_T("to groin"),
		_T("to legs")};

	CString sImg;
	CString sResult=_T("");
	std::list<Move>::iterator iter;
	for (iter=lstSequence.begin(); iter!=lstSequence.end(); iter++)
	{
		CString sAlt(_PARTS[iter->uiMoveHit]);
		sAlt+=_T(" of ")+iter->sOppName+_T(" at ")+iter->sMoveTime;
		sImg.Format(_T("<img border=0 src=\"DR_GIF/#%d\" alt=\"%s\"> "),
			178+iter->uiMoveHit+2*iter->uiMoveType,sAlt);
		sResult+=sImg;
	}
	return sResult;
}

void CFatalLogView::CreateSequenceTable()
{
	IHTMLElement *pTableElement=m_CHtmlDoc.FindElementByName(_T("sequenceTable"),0);
	IHTMLElement *pElement=NULL;
	IHTMLTableRow *pRow=m_CHtmlDoc.InsertTableRow(pTableElement);
	IHTMLTableCell *pCell=m_CHtmlDoc.InsertTableCell(pRow,-1,2,-1,TRUE);
	pCell->QueryInterface(IID_IHTMLElement,(LPVOID *)&pElement);
	m_CHtmlDoc.SetElementClassName(pElement,_T("thead"));
	m_CHtmlDoc.SetElementInnerHTMLText(pElement,_T("<b>Hits Sequence</b>"));

	pRow->Release();
	pRow=NULL;
	pCell->Release();
	pCell=NULL;
	pElement->Release();
	pElement=NULL;

	for (UINT i=0; i<2; i++)
	{
		std::list<TeamMember>::iterator iter;
		
		for (iter=m_lstTeamMembers[i].begin(); iter!=m_lstTeamMembers[i].end(); iter++)
		{
			pRow=m_CHtmlDoc.InsertTableRow(pTableElement,-1,0xEDEDEF);
			pCell=m_CHtmlDoc.InsertTableCell(pRow,-1,-1,-1,TRUE);
			pCell->QueryInterface(IID_IHTMLElement,(LPVOID *)&pElement);
			CString sTeam;
			sTeam.Format(_T("b%d"),i+1);
			m_CHtmlDoc.SetElementClassName(pElement,sTeam);
			m_CHtmlDoc.SetElementInnerHTMLText(pElement,iter->sMemberName);
			pElement->Release();
			pElement=NULL;
			pCell->Release();
			pCell=NULL;
			pCell=m_CHtmlDoc.InsertTableCell(pRow);
			pCell->QueryInterface(IID_IHTMLElement,(LPVOID *)&pElement);
			m_CHtmlDoc.SetElementClassName(pElement,_T("text"));
			m_CHtmlDoc.SetElementInnerHTMLText(pElement,GenerateString(iter->lstMoveSequence));
			pElement->Release();
			pElement=NULL;
			pRow->Release();
			pRow=NULL;
			pCell->Release();
			pCell=NULL;
		}
	}
}

void CFatalLogView::OnCheckRefresh() 
{
	// TODO: Add your control notification handler code here
	CFatalLogDoc* pDoc = GetDocument();
	CMainFrame *pMainFrame = (CMainFrame *)GetParentFrame();
	CDialogBar *pDlgBar=(CDialogBar *)pMainFrame->GetDlgItem(IDD_DIALOGBAR);
	CWnd *pWnd=pDlgBar->GetDlgItem(IDC_STATIC_EVERY);

	m_bAutoRefresh=!m_bAutoRefresh;

	pWnd->EnableWindow(m_bAutoRefresh);
	pWnd=pDlgBar->GetDlgItem(IDC_EDIT_TIME);
	pWnd->EnableWindow(m_bAutoRefresh);
	pWnd=pDlgBar->GetDlgItem(IDC_SPIN_TIME);
	pWnd->EnableWindow(m_bAutoRefresh);
	pWnd=pDlgBar->GetDlgItem(IDC_STATIC_SEC);
	pWnd->EnableWindow(m_bAutoRefresh);
}

void CFatalLogView::OnButtonStart() 
{
	// TODO: Add your control notification handler code here
	CString sNewAddress;
	KillTimer(m_uiRefreshTimer);
	CFatalLogDoc* pDoc = GetDocument();
	CMainFrame *pMainFrame = (CMainFrame *)GetParentFrame();
	CDialogBar *pDlgBar=(CDialogBar *)pMainFrame->GetDlgItem(IDD_DIALOGBAR);
	pDlgBar->GetDlgItemText(IDC_EDIT_ADDRESS,sNewAddress);
	if (sNewAddress!=m_sLogAddress)
	{
		m_sLogAddress=sNewAddress;
		for (UINT i=0; i<2; i++)
			m_lstTeamMembers[i].clear();
		m_bFirstRead=TRUE;
	}

#ifdef DEBUG_ON_LINE

	SUCCEEDED(URLDownloadToFile(0,m_sLogAddress,_T("Logs/log.html"),0,0));

#endif DEBUG_ON_LINE

	ProcessLog();
}

void CFatalLogView::OnButtonRefresh() 
{
	// TODO: Add your control notification handler code here
	KillTimer(m_uiRefreshTimer);

#ifdef DEBUG_ON_LINE

	SUCCEEDED(URLDownloadToFile(0,m_sLogAddress,_T("Logs/log.html"),0,0));

#endif DEBUG_ON_LINE

	ProcessLog();
}

void CFatalLogView::OnKillfocusEditTime() 
{
	// TODO: Add your control notification handler code here
	CFatalLogDoc* pDoc = GetDocument();
	CMainFrame *pMainFrame = (CMainFrame *)GetParentFrame();
	CDialogBar *pDlgBar=(CDialogBar *)pMainFrame->GetDlgItem(IDD_DIALOGBAR);
	if (pDlgBar->GetDlgItemInt(IDC_EDIT_TIME)<10)
		pDlgBar->SetDlgItemInt(IDC_EDIT_TIME,10);
}

void CFatalLogView::ProcessLog()
{
	CFile LogFile;

	LogFile.Open(_T("Logs/log.html"),CFile::modeRead | CFile::shareDenyNone);
	LogFile.ReadHuge(m_sFileData.GetBuffer(LogFile.GetLength()),LogFile.GetLength());
	m_sFileData.ReleaseBuffer(LogFile.GetLength());
	LogFile.Close();

	UINT uiMaxPages=GetPages(TRUE);
	CString sFileName;
	CString sPage;

	m_bNewMoves=FALSE;

	sFileName.Format(_T("Logs/log%d.html"),uiMaxPages);
	CFile::Rename(_T("Logs/log.html"),sFileName);
	if (m_bFirstRead)
	{
		m_bFirstRead=FALSE;
		for (UINT i=0; i<uiMaxPages-1; i++)
		{
			sPage.Format(_T("&p=%d"),i+1);
			sFileName.Format(_T("Logs/log%d.html"),i+1);

#ifdef DEBUG_ON_LINE

			SUCCEEDED(URLDownloadToFile(0,m_sLogAddress+sPage,sFileName,0,0));

#endif DEBUG_ON_LINE

		}

		LogFile.Open(_T("Logs/log1.html"),CFile::modeRead | CFile::shareDenyNone);
		LogFile.ReadHuge(m_sFileData.GetBuffer(LogFile.GetLength()),LogFile.GetLength());
		m_sFileData.ReleaseBuffer(LogFile.GetLength());
		LogFile.Close();

		m_sStartDate=GetStartDate(TRUE);
		GetTeamMembers(TRUE);
		GetSequence();
		for (i=2; i<=uiMaxPages; i++)
		{
			sFileName.Format(_T("Logs/log%d.html"),i);
			LogFile.Open(sFileName,CFile::modeRead | CFile::shareDenyNone);
			LogFile.ReadHuge(m_sFileData.GetBuffer(LogFile.GetLength()),LogFile.GetLength());
			m_sFileData.ReleaseBuffer(LogFile.GetLength());
			LogFile.Close();
			GetSequence();
		}
	}
	else
	{
		UINT uiPage=uiMaxPages;
		while ((m_uiGlobalPos=m_sFileData.Find(m_sGlobalPos))==-1&&uiPage>1)
		{
			sPage.Format(_T("&p=%d"),uiPage-1);
			sFileName.Format(_T("Logs/log%d.html"),uiPage-1);

#ifdef DEBUG_ON_LINE

			SUCCEEDED(URLDownloadToFile(0,(m_sLogAddress+sPage),sFileName,0,0));

#endif DEBUG_ON_LINE

			LogFile.Open(sFileName,CFile::modeRead | CFile::shareDenyNone);
			LogFile.ReadHuge(m_sFileData.GetBuffer(LogFile.GetLength()),LogFile.GetLength());
			m_sFileData.ReleaseBuffer(LogFile.GetLength());
			LogFile.Close();
			uiPage--;
		}
		if (m_uiGlobalPos==-1)
		{
			AfxMessageBox(_T("Unexpected tokens!!!"),MB_OK);
			return;
		}

		GetSequence();
		for (UINT i=uiPage+1; i<=uiMaxPages; i++)
		{
			sFileName.Format(_T("Logs/log%d.html"),i);
			LogFile.Open(sFileName,CFile::modeRead | CFile::shareDenyNone);
			LogFile.ReadHuge(m_sFileData.GetBuffer(LogFile.GetLength()),LogFile.GetLength());
			m_sFileData.ReleaseBuffer(LogFile.GetLength());
			LogFile.Close();

			GetSequence();
		}
	}
	if (m_bNewMoves)
		LoadFromResource(IDR_HTMLSTAT);
}

void CFatalLogView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent==m_uiRefreshTimer)
		OnButtonRefresh();
	CHtmlView::OnTimer(nIDEvent);
}
