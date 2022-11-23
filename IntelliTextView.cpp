/* This file is part of IntelliText application developed by Mihai MOGA.

IntelliText is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliText is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliText. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliTextView.cpp : implementation of the CIntelliTextView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IntelliText.h"
#endif

#include "IntelliTextDoc.h"
#include "IntelliTextView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIntelliTextView

IMPLEMENT_DYNCREATE(CIntelliTextView, CEditView)

BEGIN_MESSAGE_MAP(CIntelliTextView, CEditView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CIntelliTextView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CIntelliTextView construction/destruction

CIntelliTextView::CIntelliTextView() noexcept
{
	// TODO: add construction code here

}

CIntelliTextView::~CIntelliTextView()
{
}

BOOL CIntelliTextView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

// CIntelliTextView printing

void CIntelliTextView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CIntelliTextView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default CEditView preparation
	return CEditView::OnPreparePrinting(pInfo);
}

void CIntelliTextView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView begin printing
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void CIntelliTextView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView end printing
	CEditView::OnEndPrinting(pDC, pInfo);
}

void CIntelliTextView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIntelliTextView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CIntelliTextView diagnostics

#ifdef _DEBUG
void CIntelliTextView::AssertValid() const
{
	CEditView::AssertValid();
}

void CIntelliTextView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CIntelliTextDoc* CIntelliTextView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIntelliTextDoc)));
	return (CIntelliTextDoc*)m_pDocument;
}
#endif //_DEBUG

// CIntelliTextView message handlers
