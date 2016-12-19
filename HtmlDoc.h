// HtmlDoc.h: interface for the CHtmlDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLDOC_H__A6C6F65B_23E0_4150_92A1_B819997903B7__INCLUDED_)
#define AFX_HTMLDOC_H__A6C6F65B_23E0_4150_92A1_B819997903B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHtmlDoc  
{
public:
	IHTMLTableCell * InsertTableCell(IHTMLTableRow *pRow,const LONG lIndex=-1,const LONG lColSpan=-1,const LONG lWidth=-1,const BOOL bNoWrap=FALSE);
	BOOL SetElementClassName(IHTMLElement *pElement,const CString &sClassName);
	IHTMLTableRow * InsertTableRow(IHTMLElement *pElement,const LONG lIndex=-1,const LONG lBGColor=-1);
	BOOL SetElementInnerHTMLText(IHTMLElement *pElement,const CString &sHtmlText);
	CString GetImgSrc(IHTMLImgElement *img);
	BOOL SaveToFile(const CString file_name);
	CString GetElementInnerText(IHTMLElement *element);
	IHTMLElement * GetElementByIndex(const long frame_index,const long col_index);
	BOOL SelectOptionByName(const CString &name,long index);
	BOOL ClickInput(IHTMLElement *input);
	BOOL ClickImg(IHTMLImgElement *img);
	BOOL ClickAnchor(IHTMLElement *anchor);
	CString GetTitle(void);
	BOOL ClickImg(const CString &alt);
	IHTMLImgElement * FindImgByAlt(const CString &alt);
	IHTMLElement * FindElementByTag(const CString &tag,const long index);
	BOOL ClickAnchor(const CString &anchor);
	IHTMLElement * FindAnchorByText(const CString &text);
	CString GetBodyInnerText(void);
	BOOL ClickInput(const CString &value);
	IHTMLElement * FindElementByValue(const CString &value,CPoint *point=NULL);
	IHTMLElement * FindElementByAttrValue(const CString &attr,const CString &value,CPoint *point=NULL);
	BOOL ClickInput(const CString &name,const long index);
	IHTMLElement * FindElementByName(const CString &name,const long index);
	BOOL SetInputTextValue(const CString &name,const CString &value);
	IHTMLDocument2 * SetHtmlDocument(LPDISPATCH pDispatch);
	BOOL SetCookie(const CString &CookieData);
	BOOL GetCookie(CString &CookieData);
	CHtmlDoc();
	CHtmlDoc(LPDISPATCH pDispatch);
	virtual ~CHtmlDoc();

protected:
	BOOL GetFrames(void);
	long m_lFramesNumber;
	IHTMLElementCollection ** m_ptrHtmlCollections;
	IHTMLDocument2 ** m_ptrHtmlFrames;
	IHTMLElementCollection * m_pHtmlDocCol;
	IHTMLDocument2 * m_pHtmlDocApp;
};

#endif // !defined(AFX_HTMLDOC_H__A6C6F65B_23E0_4150_92A1_B819997903B7__INCLUDED_)
