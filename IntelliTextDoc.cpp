/* This file is part of IntelliText application developed by Mihai MOGA.

IntelliText is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliText is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliText.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliTextDoc.cpp : implementation of the CIntelliTextDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IntelliText.h"
#endif

#include "IntelliTextDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::wstring utf8_to_wstring(const std::string& str)
{
	// convert UTF-8 string to wstring
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str);
}

std::string wstring_to_utf8(const std::wstring& str)
{
	// convert wstring to UTF-8 string
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

// CIntelliTextDoc

IMPLEMENT_DYNCREATE(CIntelliTextDoc, CDocument)

BEGIN_MESSAGE_MAP(CIntelliTextDoc, CDocument)
	ON_COMMAND(ID_FILE_SEND_MAIL, &CIntelliTextDoc::OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, &CIntelliTextDoc::OnUpdateFileSendMail)
END_MESSAGE_MAP()


// CIntelliTextDoc construction/destruction

CIntelliTextDoc::CIntelliTextDoc() noexcept
{
	VERIFY(m_fontTerminal.CreateFont(
		-MulDiv(10, GetDeviceCaps(::GetDC(NULL), LOGPIXELSY), 72), // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		ANTIALIASED_QUALITY,       // nQuality
		DEFAULT_PITCH | FF_MODERN, // nPitchAndFamily
		_T("Consolas")));
}

CIntelliTextDoc::~CIntelliTextDoc()
{
	VERIFY(m_fontTerminal.DeleteObject());
}

BOOL CIntelliTextDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// (SDI documents will reuse this document)
	reinterpret_cast<CEditView*>(m_viewList.GetHead())->GetEditCtrl().SetFont(&m_fontTerminal);

	return TRUE;
}

// CIntelliTextDoc serialization

void CIntelliTextDoc::Serialize(CArchive& ar)
{
	// CEditView contains an edit control which handles all serialization
	if (!m_viewList.IsEmpty())
	{
		// reinterpret_cast<CEditView*>(m_viewList.GetHead())->SerializeRaw(ar);
		CEditView* pEditView = reinterpret_cast<CEditView*>(m_viewList.GetHead());
		if (ar.IsLoading())
		{
			try
			{
				const ULONGLONG nFileLength = ar.GetFile()->GetLength();
				if (nFileLength > 0)
				{
					char* pFileBuffer = new char[(UINT)nFileLength + 1];
					if (NULL != pFileBuffer)
					{
						memset(pFileBuffer, 0, sizeof(pFileBuffer));
						const UINT nActualLength = ar.GetFile()->Read(pFileBuffer, (UINT)nFileLength);
						pFileBuffer[nActualLength] = 0;
						CString strConvertedText(utf8_to_wstring(pFileBuffer).c_str());
						pEditView->GetEditCtrl().SetWindowText(strConvertedText);
						delete pFileBuffer;
						pFileBuffer = NULL;
					}
				}
			}
			catch (CFileException* pEx)
			{
				// if an error occurs, just make a message box
				pEx->ReportError();
				pEx->Delete();
			}
		}
		else
		{
			try
			{
				CString strPlainText;
				pEditView->GetEditCtrl().GetWindowText(strPlainText);
				const UINT nFileLength =  2 * strPlainText.GetLength();
				CStringA strConvertedText(wstring_to_utf8(strPlainText.GetBuffer()).c_str());
				strPlainText.ReleaseBuffer();
				ar.GetFile()->Write(strConvertedText.GetBuffer(), nFileLength);
				strConvertedText.ReleaseBuffer();
			}
			catch (CFileException* pEx)
			{
				// if an error occurs, just make a message box
				pEx->ReportError();
				pEx->Delete();
			}
		}
		pEditView->GetEditCtrl().SetModify(FALSE);
		pEditView->GetEditCtrl().SetFont(&m_fontTerminal);
	}
#ifdef SHARED_HANDLERS

	if (m_viewList.IsEmpty() && ar.IsLoading())
	{
		CFile* pFile = ar.GetFile();
		pFile->Seek(0, FILE_BEGIN);
		ULONGLONG nFileSizeBytes = pFile->GetLength();
		ULONGLONG nFileSizeChars = nFileSizeBytes/sizeof(TCHAR);
		LPTSTR lpszText = (LPTSTR)malloc(((size_t)nFileSizeChars + 1) * sizeof(TCHAR));
		if (lpszText != nullptr)
		{
			ar.Read(lpszText, (UINT)nFileSizeBytes);
			lpszText[nFileSizeChars] = '\0';
			m_strThumbnailContent = lpszText;
			m_strSearchContent = lpszText;
		}
	}
#endif
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CIntelliTextDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(m_strThumbnailContent, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CIntelliTextDoc::InitializeSearchContent()
{
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// Use the entire text file content as the search content.
	SetSearchContent(m_strSearchContent);
}

void CIntelliTextDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CIntelliTextDoc diagnostics

#ifdef _DEBUG
void CIntelliTextDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIntelliTextDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CIntelliTextDoc commands
