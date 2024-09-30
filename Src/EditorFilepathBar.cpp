/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// EditorFilepathBar.cpp : implementation of the CEditorFilepathBar class
//

#include "stdafx.h"
#include "Merge.h"
#include "EditorFilepathBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEditorFilePathBar construction destruction

CEditorFilePathBar::CEditorFilePathBar()
: m_pFont(NULL)
{
}

CEditorFilePathBar::~CEditorFilePathBar()
{
	delete m_pFont;
}


BOOL CEditorFilePathBar::Create(CWnd* pParentWnd)
{
	if (! CDialogBar::Create(pParentWnd, CEditorFilePathBar::IDD, 
													 CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, 
													 CEditorFilePathBar::IDD))
		return FALSE;

	// subclass the two custom edit boxes
	m_EditLeft.SubClassEdit(IDC_STATIC_TITLE_LEFT, this);
	m_EditRight.SubClassEdit(IDC_STATIC_TITLE_RIGHT, this);

	return TRUE;
};



/////////////////////////////////////////////////////////////////////////////

/**
 * @brief Set look of headerbars similar to other window.
 *
 * @param [in] pWnd Pointer to window we want to imitate
 * @return TRUE if parent must recompute layout
 */
BOOL CEditorFilePathBar::LookLikeThisWnd(const CWnd * pWnd)
{
	// Update font. Note that we must delete previous font
	// before creating a new one.
	CFont * pFont = pWnd->GetFont();
	if (pFont)
	{
		if (m_pFont != NULL)
			delete m_pFont;

		m_pFont = new CFont();

		if (m_pFont != NULL)
		{
			LOGFONT lfFont = {0};
			if (pFont->GetLogFont(&lfFont))
			{
				m_pFont->CreateFontIndirect(&lfFont);
				m_EditLeft.SetFont(m_pFont);
				m_EditRight.SetFont(m_pFont);
			}
		}
	}

	// Set same dimensions (than window we imitate)
	CRect rectNew;
	pWnd->GetWindowRect(rectNew);
	CRect rectCurrent;
	GetWindowRect(rectCurrent);
	if (rectNew != rectCurrent)
	{
		SetWindowPos(NULL,0,0,rectNew.Width(), rectNew.Height(),
			SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);     
		return TRUE;
	}
	return FALSE;
}

/** 
 * @brief resize both controls to an equal size
 */
void CEditorFilePathBar::Resize()
{
	if (m_hWnd == NULL)
		return;

	WINDOWPLACEMENT infoBar;
	GetWindowPlacement(&infoBar);

	WINDOWPLACEMENT info1;
	m_EditLeft.GetWindowPlacement(&info1);
	info1.rcNormalPosition.right = infoBar.rcNormalPosition.right /
		PaneCount - 2;
	m_EditLeft.SetWindowPlacement(&info1);
	m_EditLeft.RefreshDisplayText();

	WINDOWPLACEMENT info2;
	m_EditRight.GetWindowPlacement(&info2);
	info2.rcNormalPosition.left = infoBar.rcNormalPosition.right /
		PaneCount + 2;
	info2.rcNormalPosition.right = infoBar.rcNormalPosition.right;
	m_EditRight.SetWindowPlacement(&info2);
	m_EditRight.RefreshDisplayText();
}
/** 
 * @brief resize both controls to given sizes (the ones of the splitter views)
 */
void CEditorFilePathBar::Resize(int leftWidth, int rightWidth)
{
	if (m_hWnd == NULL)
		return;

	WINDOWPLACEMENT info1;

	// resize left filename
	m_EditLeft.GetWindowPlacement(&info1);
	info1.rcNormalPosition.right = info1.rcNormalPosition.left + leftWidth + 4;
	m_EditLeft.SetWindowPlacement(&info1);
	m_EditLeft.RefreshDisplayText();

	// resize right filename
	info1.rcNormalPosition.left = info1.rcNormalPosition.right + 3;
	info1.rcNormalPosition.right = info1.rcNormalPosition.left + rightWidth + 4;
	m_EditRight.SetWindowPlacement(&info1);
	m_EditRight.RefreshDisplayText();
}

/////////////////////////////////////////////////////////////////////////////
// CEditorFilePathBar message handlers

BEGIN_MESSAGE_MAP(CEditorFilePathBar, CDialogBar)
	ON_NOTIFY_EX (TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()


BOOL CEditorFilePathBar::OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult)
{
	if (m_hWnd == NULL)
		return FALSE;

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID =pTTTStruct->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the CEdit 
		nID = ::GetDlgCtrlID((HWND)nID);
		if(nID == IDC_STATIC_TITLE_LEFT || nID == IDC_STATIC_TITLE_RIGHT)
		{
			// compute max width : 97% of application width or 80% or full screen width
			CRect rect;
			GetWindowRect(rect);
			int maxWidth = (int)(rect.Width() * .97);
			CRect rectScreen; 
			SystemParametersInfo(SPI_GETWORKAREA, 0, rectScreen, 0);
			if (rectScreen.Width() * .8 > maxWidth)
				maxWidth = (int)(rectScreen.Width() * .8);

			// use the tooltip font
			HANDLE hFont = (HANDLE) ::SendMessage(pTTTStruct->hwndFrom, WM_GETFONT, 0, 0);
			CClientDC tempDC(this);
			HANDLE hOldFont = ::SelectObject(tempDC.GetSafeHdc(),hFont);

			// fill in the returned structure
			CFilepathEdit * pItem = (CFilepathEdit*) GetDlgItem(nID);
			pTTT->lpszText = (TCHAR*) pItem->GetUpdatedTipText(&tempDC, maxWidth);

			// set old font back
			if (hOldFont)
				::SelectObject(tempDC.GetSafeHdc(),hOldFont);

			// we must set TTM_SETMAXTIPWIDTH to use \n in tooltips
			// just to do the first time, but how to access the tooltip during init ?
			::SendMessage(pTTTStruct->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 5000);

			return(TRUE);
		}
	}

	return(FALSE);

}

/** 
 * @brief Set the path for one side
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] lpszString New text for pane.
 */
void CEditorFilePathBar::SetText(int pane, LPCTSTR lpszString)
{
	ASSERT (pane >= PANE_LEFT && pane < PaneCount);

	// Check for NULL since window may be closing..
	if (m_hWnd == NULL)
		return;

	if (pane == PANE_LEFT)
		m_EditLeft.SetWholeText(lpszString);
	else
		m_EditRight.SetWholeText(lpszString);
}

/** 
 * @brief Set the active status for one status (change the appearance)
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] bActive If TRUE activates pane, FALSE deactivates.
 */
void CEditorFilePathBar::SetActive(int pane, BOOL bActive)
{
	ASSERT (pane >= PANE_LEFT && pane < PaneCount);

	// Check for NULL since window may be closing..
	if (m_hWnd == NULL)
		return;

	if (pane == PANE_LEFT)
		m_EditLeft.SetActive(bActive);
	else
		m_EditRight.SetActive(bActive);
}
