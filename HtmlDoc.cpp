// HtmlDoc.cpp: implementation of the CHtmlDoc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HtmlDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHtmlDoc::CHtmlDoc():m_pHtmlDocApp(NULL),m_pHtmlDocCol(NULL),m_ptrHtmlFrames(NULL),
	m_ptrHtmlCollections(NULL),m_lFramesNumber(0)
{

}

CHtmlDoc::CHtmlDoc(LPDISPATCH pDispatch):m_pHtmlDocApp(NULL)
{
	ASSERT(pDispatch->QueryInterface(IID_IHTMLDocument2,(PVOID *)&m_pHtmlDocApp)==S_OK);
	if (!this->GetFrames())
		m_lFramesNumber=0;
}

CHtmlDoc::~CHtmlDoc()
{
	if (m_pHtmlDocApp!=NULL)
		m_pHtmlDocApp->Release();
	if (m_pHtmlDocCol!=NULL)
		m_pHtmlDocCol->Release();
	if (m_ptrHtmlFrames!=NULL)
		delete []m_ptrHtmlFrames;
	if (m_ptrHtmlCollections!=NULL)
		delete []m_ptrHtmlCollections;
}

BOOL CHtmlDoc::GetCookie(CString &CookieData)
{
	BSTR cookie;
	if (m_pHtmlDocApp->get_cookie(&cookie)==S_OK)
	{
		_bstr_t c_cookie(cookie);
		CookieData=(wchar_t *)c_cookie;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CHtmlDoc::SetCookie(const CString &CookieData)
{
	if (m_pHtmlDocApp->put_cookie(CookieData.AllocSysString())==S_OK)
		return TRUE;
	else
		return FALSE;
}

IHTMLDocument2 * CHtmlDoc::SetHtmlDocument(LPDISPATCH pDispatch)
{
	//IHTMLWindow2 *pWindow=NULL;
	ASSERT(pDispatch->QueryInterface(IID_IHTMLDocument2,(PVOID *)&m_pHtmlDocApp)==S_OK);
	//ASSERT(m_pHtmlDocApp->get_parentWindow(&pWindow)==S_OK);
	//pWindow->close();

	ASSERT(m_pHtmlDocApp->get_all(&m_pHtmlDocCol)==S_OK);
	if (!this->GetFrames())
		m_lFramesNumber=0;
	return m_pHtmlDocApp;
}

BOOL CHtmlDoc::SetInputTextValue(const CString &name, const CString &value)
{
	ASSERT(m_pHtmlDocApp!=NULL);
	LPDISPATCH pDispatch=this->FindElementByName(name,0);
	ASSERT(pDispatch!=NULL);
	IHTMLInputTextElement* pElement=NULL;
	if (pDispatch->QueryInterface(IID_IHTMLInputTextElement,(LPVOID*)&pElement)!=S_OK)
		return FALSE;
	if (pElement->put_value(value.AllocSysString())!=S_OK)
		return FALSE;
	return TRUE;
}

IHTMLElement * CHtmlDoc::FindElementByName(const CString &name, const long index)
{
	LPDISPATCH pDispatch=NULL;
	IHTMLElement *pElement=NULL;
	VARIANT itemName=COleVariant(name);
	VARIANT itemIndex;
	itemIndex.vt=VT_UINT;
	itemIndex.lVal=index;

	if (m_lFramesNumber==0)
	{
		ASSERT(m_pHtmlDocCol!=NULL);
		if (m_pHtmlDocCol->item(itemName,itemIndex,&pDispatch)!=S_OK)
			return NULL;
		if (pDispatch==NULL)
			return NULL;
		if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
			return NULL;
		if (pElement==NULL)
			return NULL;
		pDispatch->Release();
	}
	else
	{
		ASSERT(m_ptrHtmlCollections!=NULL);
		pElement=NULL;
		for (long i=0; i<m_lFramesNumber; i++)
		{
			pDispatch=NULL;
			pElement=NULL;
			if (m_ptrHtmlCollections[i]->item(itemName,itemIndex,&pDispatch)!=S_OK)
				return NULL;
			if (pDispatch==NULL)
				return NULL;
			if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
				return NULL;
			if (pElement==NULL)
				return NULL;
			pDispatch->Release();
			return pElement;
		}
	}
	return pElement;
}

BOOL CHtmlDoc::ClickInput(const CString &name, const long index)
{
	//ASSERT(m_pHtmlDocApp!=NULL);
	IHTMLElement *pElement=this->FindElementByName(name,index);
	if(pElement==NULL)
		return FALSE;
	if (pElement->click()!=S_OK)
		return FALSE;
	pElement->Release();
	return TRUE;
}

BOOL CHtmlDoc::ClickInput(const CString &value)
{
	IHTMLElement *pElement=this->FindElementByValue(value);
	if(pElement==NULL)
		return FALSE;
	if (pElement->click()!=S_OK)
		return FALSE;
	pElement->Release();
	return TRUE;
}

BOOL CHtmlDoc::ClickInput(IHTMLElement *input)
{
	ASSERT(input!=NULL);
	if (input->click()!=S_OK)
		return FALSE;
	return TRUE;
}

IHTMLElement * CHtmlDoc::FindElementByValue(const CString &value,CPoint *point)
{
	LPDISPATCH pDispatch=NULL;
	IHTMLElement *pElement=NULL;
	long itemLength;
	if (m_lFramesNumber==0)
	{
		ASSERT(m_pHtmlDocCol!=NULL);
		if (m_pHtmlDocCol->get_length(&itemLength)!=S_OK)
			return NULL;
		for (long i=0; i<itemLength; i++)
		{
			VARIANT itemName;
			itemName.vt=VT_UINT;
			itemName.lVal=i;
			VARIANT itemIndex={0};
			if (m_pHtmlDocCol->item(itemName,itemIndex,&pDispatch)!=S_OK)
				return NULL;
		
			if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
				return NULL;
			_bstr_t attrName(_T("VALUE"));
			VARIANT attrValue;
			if (pElement->getAttribute(attrName,FALSE,&attrValue)==S_OK)
				if (attrValue.vt==VT_BSTR)
				{
					_bstr_t bstrValue(attrValue.bstrVal);
					attrName=value.AllocSysString();
					if (bstrValue==attrName)
					{
						pDispatch->Release();
						if (point!=NULL)
						{
							point->x=-1;
							point->y=i;
						}
						return pElement;
					}
				}
			pDispatch->Release();
			pElement->Release();
		}
	}
	else
	{
		ASSERT(m_ptrHtmlCollections!=NULL);
		for (long i=0; i<m_lFramesNumber; i++)
		{
			if (m_ptrHtmlCollections[i]->get_length(&itemLength)!=S_OK)
				return NULL;
			for (long j=0; j<itemLength; j++)
			{
				VARIANT itemName;
				itemName.vt=VT_UINT;
				itemName.lVal=j;
				VARIANT itemIndex={0};
				if (m_ptrHtmlCollections[i]->item(itemName,itemIndex,&pDispatch)!=S_OK)
					return NULL;
		
				if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
					return NULL;
				_bstr_t attrName(_T("VALUE"));
				VARIANT attrValue;
				if (pElement->getAttribute(attrName,FALSE,&attrValue)==S_OK)
					if (attrValue.vt==VT_BSTR)
					{
						_bstr_t bstrValue(attrValue.bstrVal);
						attrName=value.AllocSysString();
						if (bstrValue==attrName)
						{
							if (point!=NULL)
							{
								point->x=i;
								point->y=j;
							}
							pDispatch->Release();
							return pElement;
						}
					}
				pDispatch->Release();
				pElement->Release();
			}
		}
	}
	return NULL;
}

CString CHtmlDoc::GetBodyInnerText()
{
	CString result(_T(""));
	ASSERT(m_pHtmlDocApp);
	IHTMLElement *pElement=NULL;
	if (m_pHtmlDocApp->get_body(&pElement)!=S_OK)
		return result;
	BSTR string;
	if (pElement->get_innerText(&string)!=S_OK)
		return result;
	_bstr_t bstr_string(string);
 	result=(wchar_t *)bstr_string;
 	pElement->Release();
 	return result;
}

IHTMLElement * CHtmlDoc::FindAnchorByText(const CString &text)
{
	IHTMLElementCollection *pCollection=NULL;
	LPDISPATCH pDispatch=NULL;
	IHTMLElement *pElement=NULL;
	VARIANT tagName=COleVariant(_T("A"));

	if (m_lFramesNumber==0)
	{
		ASSERT(m_pHtmlDocCol!=NULL);
		if (m_pHtmlDocCol->tags(tagName,&pDispatch)!=S_OK)
			return NULL;
		long itemsLenght;
		if (pDispatch->QueryInterface(IID_IHTMLElementCollection,(LPVOID*)&pCollection)!=S_OK)
			return NULL;
		if (pCollection->get_length(&itemsLenght)!=S_OK)
			return NULL;
		for (long i=0; i<itemsLenght; i++)
		{
			pDispatch=NULL;
			pElement=NULL;
			VARIANT itemName=COleVariant(i);
			VARIANT itemIndex={0};
			CString result=_T("");
			BSTR string;
			if (pCollection->item(itemName,itemIndex,&pDispatch)!=S_OK)
				return NULL;
			if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
				return NULL;
			if (pElement->get_innerText(&string)!=S_OK)
				return NULL;
			_bstr_t bstr_string(string);
			result=(wchar_t *)bstr_string;
			if (result==text)
			{
				pDispatch->Release();
				return pElement;
			}
			pElement->Release();
			pDispatch->Release();
		}
		pCollection->Release();
	}
	else
	{
		ASSERT(m_ptrHtmlCollections!=NULL);	
		for (long i=0; i<m_lFramesNumber; i++)
		{
			if (m_ptrHtmlCollections[i]->tags(tagName,&pDispatch)!=S_OK)
				return NULL;
			long itemsLenght;
			if (pDispatch->QueryInterface(IID_IHTMLElementCollection,(LPVOID*)&pCollection)!=S_OK)
				return NULL;
			if (pCollection->get_length(&itemsLenght)!=S_OK)
				return NULL;
			for (long j=0; j<itemsLenght; j++)
			{
				pDispatch=NULL;
				pElement=NULL;
				VARIANT itemName=COleVariant(j);
				VARIANT itemIndex={0};
				CString result=_T("");
				BSTR string;
				if (pCollection->item(itemName,itemIndex,&pDispatch)!=S_OK)
					return NULL;
				if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
					return NULL;
				if (pElement->get_innerText(&string)!=S_OK)
					return NULL;
				_bstr_t bstr_string(string);
				result=(wchar_t *)bstr_string;
				if (result==text)
				{
					pDispatch->Release();
					return pElement;
				}
				pElement->Release();
				pDispatch->Release();
			}
			pCollection->Release();
		}
	}
	return NULL;
}

BOOL CHtmlDoc::ClickAnchor(const CString &anchor)
{
	IHTMLElement *pElement=FindAnchorByText(anchor);
	ASSERT(pElement!=NULL);
	if (pElement->click()!=S_OK)
		return FALSE;
	return TRUE;
}

BOOL CHtmlDoc::ClickAnchor(IHTMLElement *anchor)
{
	ASSERT(anchor!=NULL);
	if (anchor->click()!=S_OK)
		return FALSE;
	return TRUE;
}

IHTMLElement * CHtmlDoc::FindElementByTag(const CString &tag, const long index)
{
	IHTMLElementCollection *pCollection=NULL;
	IHTMLElement *pElement=NULL;
	LPDISPATCH pDispatch=NULL;
	VARIANT tagName=COleVariant(tag);
	VARIANT itemName=COleVariant(index);
	VARIANT itemIndex={0};
	if (m_lFramesNumber==0)
	{
		ASSERT(m_pHtmlDocCol!=NULL);
		if (m_pHtmlDocCol->tags(tagName,&pDispatch)!=S_OK)
			return NULL;
		if (pDispatch==NULL)
			return NULL;
		if (pDispatch->QueryInterface(IID_IHTMLElementCollection,(LPVOID*)&pCollection)!=S_OK)
			return NULL;
		pDispatch->Release();
		pDispatch=NULL;
		if (pCollection->item(itemName,itemIndex,&pDispatch)!=S_OK)
			return NULL;
		if (pDispatch!=NULL)
			return NULL;
		if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
			return NULL;
		pCollection->Release();
		pDispatch->Release();
	}
	else
	{
		ASSERT(m_ptrHtmlCollections!=NULL);
		for (long i=0; i<m_lFramesNumber; i++)
		{
			if (m_ptrHtmlCollections[i]->tags(tagName,&pDispatch)!=S_OK)
				return NULL;
			if (pDispatch==NULL)
				return NULL;
			if (pDispatch->QueryInterface(IID_IHTMLElementCollection,(LPVOID*)&pCollection)!=S_OK)
				return NULL;
			pDispatch->Release();
			pDispatch=NULL;
			if (pCollection->item(itemName,itemIndex,&pDispatch)!=S_OK)
				return NULL;
			if (pDispatch!=NULL)
				return NULL;
			if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
				return NULL;
			pCollection->Release();
			pDispatch->Release();
		}
	}
	return pElement;
}

IHTMLImgElement * CHtmlDoc::FindImgByAlt(const CString &alt)
{
	IHTMLElementCollection *pCollection=NULL;
	IHTMLImgElement *pImg;
	if (m_lFramesNumber==0)
	{
		ASSERT(m_pHtmlDocApp!=NULL);
		if (m_pHtmlDocApp->get_images(&pCollection)!=S_OK)
			return NULL;
		if (pCollection==NULL)
			return NULL;
		long itemsLenght;
		if (pCollection->get_length(&itemsLenght)!=S_OK)
			return NULL;
		for (long i=0; i<itemsLenght; i++)
		{
			LPDISPATCH pDispatch=NULL;
			VARIANT itemName=COleVariant(i);
			VARIANT itemIndex={0};
			pImg=NULL;
			if (pCollection->item(itemName,itemIndex,&pDispatch)!=S_OK)
				return NULL;
			if (pDispatch->QueryInterface(IID_IHTMLImgElement,(LPVOID*)&pImg)!=S_OK)
				return NULL;
			if (pImg==NULL)
				return NULL;
			BSTR string;
			if (pImg->get_alt(&string)!=S_OK)
				return NULL;
			_bstr_t bstr_string(string);
			CString result=(wchar_t *)bstr_string;
			if (result.Find(alt)!=-1)
			{
				pCollection->Release();
				return pImg;
			}
			pImg->Release();
		}
		pCollection->Release();
	}
	else
	{
		ASSERT(m_ptrHtmlFrames!=NULL);
		for (long i=0; i<m_lFramesNumber; i++)
		{
			if (m_ptrHtmlFrames[i]->get_images(&pCollection)!=S_OK)
				return NULL;
			if (pCollection==NULL)
				return NULL;
			long itemsLenght;
			if (pCollection->get_length(&itemsLenght)!=S_OK)
				return NULL;
			for (long j=0; j<itemsLenght; j++)
			{
				LPDISPATCH pDispatch=NULL;
				VARIANT itemName=COleVariant(j);
				VARIANT itemIndex={0};
				pImg=NULL;
				if (pCollection->item(itemName,itemIndex,&pDispatch)!=S_OK)
					return NULL;
				if (pDispatch->QueryInterface(IID_IHTMLImgElement,(LPVOID*)&pImg)!=S_OK)
					return NULL;
				if (pImg==NULL)
					return NULL;
				BSTR string;
				if (pImg->get_alt(&string)!=S_OK)
					return NULL;
				_bstr_t bstr_string(string);
				CString result=(wchar_t *)bstr_string;
				if (result.Find(alt)!=-1)
				{
					pCollection->Release();
					return pImg;
				}
				pImg->Release();
			}
		}
	}
	return NULL;
}

BOOL CHtmlDoc::ClickImg(const CString &alt)
{
	IHTMLElement *pElement=NULL;
	IHTMLImgElement *pImg=this->FindImgByAlt(alt);
	this->GetImgSrc(pImg);
	if (pImg==NULL)
		return FALSE;
	if (pImg->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
		return FALSE;
	if (pElement->click()!=S_OK)
		return FALSE;
	return TRUE;
}

BOOL CHtmlDoc::ClickImg(IHTMLImgElement *img)
{
	ASSERT(img!=NULL);
	IHTMLElement *pElement=NULL;
	if (img==NULL)
		return FALSE;
	if (img->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
		return FALSE;
	if (pElement->click()!=S_OK)
		return FALSE;
	return TRUE;
}

CString CHtmlDoc::GetTitle()
{
	ASSERT(m_pHtmlDocApp!=NULL);
	CString title=_T("");
	BSTR bstr_title;
	if (m_pHtmlDocApp->get_title(&bstr_title)!=S_OK)
		return title;
	title=(wchar_t *)bstr_title;
	return title;
}

BOOL CHtmlDoc::GetFrames()
{
	ASSERT(m_pHtmlDocApp!=NULL);
	if (m_ptrHtmlFrames!=NULL)
		delete []m_ptrHtmlFrames;
	if (m_ptrHtmlCollections!=NULL)
		delete []m_ptrHtmlCollections;
	m_lFramesNumber=0;
	IHTMLFramesCollection2* pFramesCol=NULL;

	if (m_pHtmlDocApp->get_frames(&pFramesCol)!=S_OK)
		return FALSE;
	if (pFramesCol==NULL)
		return FALSE;
	pFramesCol->get_length(&m_lFramesNumber);
	if (m_lFramesNumber==0)
		return TRUE;
	m_ptrHtmlFrames=new IHTMLDocument2 *[m_lFramesNumber];
	ASSERT(m_ptrHtmlFrames!=NULL);
	m_ptrHtmlCollections=new IHTMLElementCollection *[m_lFramesNumber];
	ASSERT(m_ptrHtmlCollections!=NULL);
	for (long i=0; i<m_lFramesNumber; i++)
	{
		IHTMLWindow2* pWindow=NULL;
		VARIANT frameName=COleVariant(i);
		VARIANT frameOut;
		if (pFramesCol->item(&frameName, &frameOut)!=S_OK)
			return FALSE;
		if (frameOut.vt==VT_EMPTY)
			return FALSE;
		if (frameOut.pdispVal->QueryInterface(IID_IHTMLWindow2, (LPVOID *)&pWindow)!=S_OK)
			return FALSE;
		if (pWindow->get_document(&m_ptrHtmlFrames[i])!=S_OK)
			return FALSE;
		if (m_ptrHtmlFrames[i]->get_all(&m_ptrHtmlCollections[i])!=S_OK)
			return FALSE;
		long p;
		m_ptrHtmlCollections[i]->get_length(&p);
		if (p<100)
			pWindow->close();
		pWindow->Release();
	}
	pFramesCol->Release();
	return TRUE;
}

BOOL CHtmlDoc::SelectOptionByName(const CString &name, long index)
{
	IHTMLElement *pElement=FindElementByName(name,0);
	IHTMLSelectElement *pSelect=NULL;
	if (pElement==NULL)
		return FALSE;
	if (pElement->QueryInterface(IID_IHTMLSelectElement,(LPVOID *)&pSelect)!=S_OK)
		return FALSE;
	if (pSelect->put_selectedIndex(index)!=S_OK)
		return FALSE;
	pElement->Release();
	return TRUE;
}

IHTMLElement * CHtmlDoc::GetElementByIndex(const long frame_index,const long col_index)
{
	IHTMLElement *pElement=NULL;
	LPDISPATCH pDispatch=NULL;
	VARIANT itemName=COleVariant(col_index);
	VARIANT itemIndex={0};
	if (frame_index==-1)
	{
		ASSERT(m_pHtmlDocCol!=NULL);
		if (m_pHtmlDocCol->item(itemName,itemIndex,&pDispatch)!=S_OK)
			return NULL;
	}
	else
	{
		ASSERT(m_ptrHtmlCollections!=NULL);
		if (m_ptrHtmlCollections[frame_index]->item(itemName,itemIndex,&pDispatch)!=S_OK)
			return NULL;
	}
	if (pDispatch==NULL)
		return NULL;
	if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID *)&pElement)!=S_OK)
		return NULL;
	pDispatch->Release();
	return pElement;
}

CString CHtmlDoc::GetElementInnerText(IHTMLElement *element)
{
	ASSERT(element!=NULL);
	BSTR text;
	CString result=_T("");
	if (element->get_innerText(&text)!=S_OK)
		return result;
	_bstr_t bstrText(text);
	result=(wchar_t *)bstrText;
	return result;
}

BOOL CHtmlDoc::SaveToFile(const CString file_name)
{
	CStdioFile file;
	if (FAILED(file.Open(file_name,CFile::modeCreate | CFile::modeWrite)))
		return FALSE;
	long itemsLength;
	ASSERT(m_pHtmlDocCol!=NULL);
	IHTMLElement *pElement=NULL;
	LPDISPATCH pDispatch=NULL;
	if (FAILED(m_pHtmlDocCol->get_length(&itemsLength)))
		return FALSE;
	file.WriteString(_T("*************MAIN DOCUMENT**********\n"));
	for (long i=0; i<itemsLength; i++)
	{
		pElement=NULL;
		pDispatch=NULL;
		VARIANT itemName=COleVariant(i);
		VARIANT itemIndex={0};
		if (FAILED(m_pHtmlDocCol->item(itemName,itemIndex,&pDispatch)))
			return FALSE;
		if (pDispatch==NULL)
			return FALSE;
		if (FAILED(pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID *)&pElement)))
			return FALSE;
		BSTR text;
		if (FAILED(pElement->get_outerHTML(&text)))
			return FALSE;
		_bstr_t bstrText(text);
		CString Text=(wchar_t *)bstrText;
		file.WriteString(Text);
		if (pElement!=NULL)
			pElement->Release();
		if (pDispatch!=NULL)
			pDispatch->Release();
	}
	file.WriteString(_T("\n*************FRAMES**********\n"));
	for (i=0; i<m_lFramesNumber; i++)
	{
		file.WriteString(_T("\n*************FRAME**********\n"));
		ASSERT(m_ptrHtmlCollections[i]!=NULL);
		if (FAILED(m_ptrHtmlCollections[i]->get_length(&itemsLength)))
			return FALSE;
		for (long j=0; j<itemsLength; j++)
		{	
			pElement=NULL;
			pDispatch=NULL;
			VARIANT itemName=COleVariant(j);
			VARIANT itemIndex={0};
			if (FAILED(m_ptrHtmlCollections[i]->item(itemName,itemIndex,&pDispatch)))
				return FALSE;
			if (pDispatch==NULL)
				return FALSE;
			if (FAILED(pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID *)&pElement)))
				return FALSE;
			BSTR text;
			if (FAILED(pElement->get_outerHTML(&text)))
				return FALSE;
			_bstr_t bstrText(text);
			CString Text=(wchar_t *)bstrText;
			file.WriteString(Text);
			if (pElement!=NULL)
				pElement->Release();
			if (pDispatch!=NULL)
			pDispatch->Release();
		}
		file.WriteString(_T("\n"));
	}
	file.Close();
	return TRUE;
}

CString CHtmlDoc::GetImgSrc(IHTMLImgElement *pImg)
{
	ASSERT(pImg!=NULL);
	CString result(_T(""));
	BSTR src;
	if (FAILED(pImg->get_src(&src)))
		return result;
	_bstr_t bstrSrc(src);
	result=(wchar_t *)bstrSrc;
	return result;
}

IHTMLElement * CHtmlDoc::FindElementByAttrValue(const CString &attr, const CString &value,CPoint *point)
{
	LPDISPATCH pDispatch=NULL;
	IHTMLElement *pElement=NULL;

	long itemLength;
	if (m_lFramesNumber==0)
	{
		ASSERT(m_pHtmlDocCol!=NULL);
		if (m_pHtmlDocCol->get_length(&itemLength)!=S_OK)
			return NULL;
		for (long i=0; i<itemLength; i++)
		{
			VARIANT itemName;
			itemName.vt=VT_UINT;
			itemName.lVal=i;
			VARIANT itemIndex={0};
			if (m_pHtmlDocCol->item(itemName,itemIndex,&pDispatch)!=S_OK)
				return NULL;
		
			if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
				return NULL;
			_bstr_t attrName(attr);
			VARIANT attrValue;
			if (pElement->getAttribute(attrName,FALSE,&attrValue)==S_OK)
				if (attrValue.vt==VT_BSTR)
				{
					_bstr_t bstrValue(attrValue.bstrVal);
					_bstr_t bstrAttrValue=value.AllocSysString();
					if (bstrValue==bstrAttrValue)
					{
						pDispatch->Release();
						if (point!=NULL)
						{
							point->x=-1;
							point->y=i;
						}
						return pElement;
					}
				}
			pDispatch->Release();
			pElement->Release();
		}
	}
	else
	{
		ASSERT(m_ptrHtmlCollections!=NULL);
		for (long i=0; i<m_lFramesNumber; i++)
		{
			if (m_pHtmlDocCol->get_length(&itemLength)!=S_OK)
				return NULL;
			for (long j=0; j<itemLength; j++)
			{
				VARIANT itemName;
				itemName.vt=VT_UINT;
				itemName.lVal=j;
				VARIANT itemIndex={0};
				if (m_ptrHtmlCollections[i]->item(itemName,itemIndex,&pDispatch)!=S_OK)
					return NULL;
		
				if (pDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElement)!=S_OK)
					return NULL;
				_bstr_t attrName(attr);
				VARIANT attrValue;
				if (pElement->getAttribute(attrName,FALSE,&attrValue)==S_OK)
					if (attrValue.vt=VT_BSTR)
					{
						_bstr_t bstrValue(attrValue.bstrVal);
						_bstr_t bstrAttrValue=value.AllocSysString();
						if (bstrValue==bstrAttrValue)
						{
							if (point!=NULL)
							{
								point->x=i;
								point->y=j;
							}
							pDispatch->Release();
							return pElement;
						}
					}
				pDispatch->Release();
				pElement->Release();
			}
		}
	}
	return NULL;
}

BOOL CHtmlDoc::SetElementInnerHTMLText(IHTMLElement *pElement, const CString &sHtmlText)
{
	ASSERT(pElement!=NULL);
	if (pElement->put_innerHTML(sHtmlText.AllocSysString())!=S_OK)
		return FALSE;
	else
		return TRUE;
}

IHTMLTableRow * CHtmlDoc::InsertTableRow(IHTMLElement *pElement,const LONG lIndex,const LONG lBGColor)
{
	ASSERT(pElement!=NULL);
	IHTMLTable *pTable=NULL;
	LPDISPATCH pDispatch=NULL;
	IHTMLTableRow *pRow=NULL;
	if (pElement->QueryInterface(IID_IHTMLTable,(LPVOID *)&pTable)!=S_OK)
		return NULL;
	if (pTable->insertRow(lIndex,&pDispatch)!=S_OK)
		return NULL;
	if (pDispatch->QueryInterface(IID_IHTMLTableRow,(LPVOID *)&pRow))
		return NULL;
	if (lBGColor!=-1)
	{
		VARIANT vColor=COleVariant(lBGColor);
		if (pRow->put_bgColor(vColor)!=S_OK)
			return NULL;
	}
	return pRow;
}

BOOL CHtmlDoc::SetElementClassName(IHTMLElement *pElement,const CString &sClassName)
{
	ASSERT(pElement!=NULL);
	_bstr_t bstrClassName(sClassName);
	if (pElement->put_className(bstrClassName)!=S_OK)
		return FALSE;
	return TRUE;
}

IHTMLTableCell * CHtmlDoc::InsertTableCell(IHTMLTableRow *pRow,
										   const LONG lIndex,
										   const LONG lColSpan,
										   const LONG lWidth,
										   const BOOL bNoWrap)
{
	ASSERT(pRow!=NULL);
	LPDISPATCH pDispatch=NULL;
	IHTMLTableCell *pCell=NULL;
	if (pRow->insertCell(lIndex,&pDispatch)!=S_OK)
		return NULL;
	if (pDispatch->QueryInterface(IID_IHTMLTableCell,(LPVOID *)&pCell)!=S_OK)
		return NULL;
	if (lColSpan!=-1)
		if (pCell->put_colSpan(lColSpan)!=S_OK)
			return NULL;
	if (lWidth!=-1)
	{
		VARIANT vWidth=COleVariant(lWidth);
		if (pCell->put_width(vWidth)!=S_OK)
			return NULL;
	}
	if (bNoWrap)
	{
		VARIANT_BOOL vbNoWrap=VARIANT_TRUE;
		if (pCell->put_noWrap(vbNoWrap)!=S_OK)
			return NULL;
	}
	return pCell;
}