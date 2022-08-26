/* This file is part of IntelliText application developed by Mihai MOGA.

IntelliText is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliText is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliText.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliTextView.h : interface of the CIntelliTextView class
//

#pragma once

class CIntelliTextView : public CEditView
{
protected: // create from serialization only
	CIntelliTextView() noexcept;
	DECLARE_DYNCREATE(CIntelliTextView)

// Attributes
public:
	CIntelliTextDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CIntelliTextView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in IntelliTextView.cpp
inline CIntelliTextDoc* CIntelliTextView::GetDocument() const
   { return reinterpret_cast<CIntelliTextDoc*>(m_pDocument); }
#endif
