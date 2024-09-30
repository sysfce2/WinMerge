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
/** 
 * @file  DirFrame.cpp
 *
 * @brief Implementation file for CDirFrame
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DirFrame.cpp,v 1.21 2005/06/27 16:33:13 elsapo Exp $

#include "stdafx.h"
#include "Merge.h"
#include "CompareStats.h"
#include "DirFrame.h"
#include "FilepathEdit.h"
#include "DirDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Statusbar pane indexes
 */
enum
{
	PANE_FILTER = 1,
	PANE_LEFT_RO,
	PANE_RIGHT_RO,
};

/**
 * @brief Width of filter name pane in statusbar
 */
const int FILTER_PANEL_WIDTH = 180;

/**
 * @brief Bottom statusbar panels and indicators
 */
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
};

/**
 * @brief RO status panel width
 */
static UINT RO_PANEL_WIDTH = 40;

/////////////////////////////////////////////////////////////////////////////
// CDirFrame

IMPLEMENT_DYNCREATE(CDirFrame, CMDIChildWnd)

CDirFrame::CDirFrame()
: m_picanclose(0)
, m_param(0)
{
}

CDirFrame::~CDirFrame()
{
}


BEGIN_MESSAGE_MAP(CDirFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CDirFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirFrame message handlers

/**
 * @brief Create statusbar
 */
int CDirFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_TOP);

	// Dir frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create header bar\n");
		return -1;      // fail to create
	}	

	// Dir frame has a floating bar displayed during comparison
	if (!CreateStateBar())
	{
		TRACE0("Failed to create floating dialog bar\n");
		return -1;      // fail to create
	}	

	// Directory frame has a status bar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}	
	
	CString sText;
	VERIFY(sText.LoadString(IDS_STATUSBAR_READONLY));
	m_wndStatusBar.SetPaneInfo(PANE_FILTER, ID_STATUS_FILTER, 0, FILTER_PANEL_WIDTH);
	m_wndStatusBar.SetPaneInfo(PANE_LEFT_RO, ID_STATUS_LEFTDIR_RO, 0, RO_PANEL_WIDTH);
	m_wndStatusBar.SetPaneInfo(PANE_RIGHT_RO, ID_STATUS_RIGHTDIR_RO, 0, RO_PANEL_WIDTH);
	m_wndStatusBar.SetPaneText(PANE_LEFT_RO, sText, TRUE); 
	m_wndStatusBar.SetPaneText(PANE_RIGHT_RO, sText, TRUE);
	return 0;
}

/**
 * @brief Create, or recreate, the floating state bar
 *
 * This is the bar with the file counts updated during a directory comparison
 */
bool CDirFrame::CreateStateBar()
{
	if (IsWindow(m_wndCompStateBar.m_hWnd))
	{
		m_wndCompStateBar.DestroyWindow();
	}
	// Dir frame has a floating bar displayed during comparison
	if (!m_wndCompStateBar.Create(this))
		return false;
	m_wndCompStateBar.EnableDocking(0);
	return true;
}

/**
 * @brief Set statusbar text
 */
void CDirFrame::SetStatus(LPCTSTR szStatus)
{
	m_wndStatusBar.SetPaneText(0, szStatus);
}

/**
 * @brief Set active filter name to statusbar
 * @param [in] szFilter Filtername to show
 */
void CDirFrame::SetFilterStatusDisplay(LPCTSTR szFilter)
{
	m_wndStatusBar.SetPaneText(PANE_FILTER, szFilter);
}

/**
 * @brief Get the interface to the header (path) bar
 */
IHeaderBar * CDirFrame::GetHeaderInterface() {
	return &m_wndFilePathBar;
}

/**
 * @brief Restore maximized state of directory compare window
 */
void CDirFrame::ActivateFrame(int nCmdShow) 
{
	// get the active child frame, and a flag whether it is maximized
	BOOL bMaximized;
	CMDIChildWnd * oldActiveFrame = GetMDIFrame()->MDIGetActive(&bMaximized);
	if (oldActiveFrame == NULL)
		// for the first frame, get the restored/maximized state from the registry
		bMaximized = theApp.GetProfileInt(_T("Settings"), _T("ActiveFrameMax"), FALSE);
	if (bMaximized)
		nCmdShow = SW_SHOWMAXIMIZED;
	else
		nCmdShow = SW_SHOWNORMAL;

	CMDIChildWnd::ActivateFrame(nCmdShow);
	m_bFrameIsActive = TRUE;

	// hide the floating bar
	ShowProcessingBar(FALSE);

	// and set its initial position (centered at 1/3 of screen)
	SetStateBarLoc();

	// prepare file path bar to look as a status bar
	if (m_wndFilePathBar.LookLikeThisWnd(&m_wndStatusBar) == TRUE)
		RecalcLayout();
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CDirFrame::UpdateResources()
{
	// Ensure show state is correct
	BOOL StateBarVisible = m_bStateBarIsActive;

	CreateStateBar();
	if (!StateBarVisible)
		m_wndCompStateBar.ShowWindow(SW_HIDE);
	SetStateBarLoc();
	
	if (StateBarVisible)
	{
		m_wndCompStateBar.UpdateElements();
		ShowProcessingBar(StateBarVisible);
	}
}

/**
 * @brief Set the location of the state control bar
 */
void CDirFrame::SetStateBarLoc()
{
	CRect rc;
	GetWindowRect(&rc);
	CPoint origin;
	origin.x = (rc.left+rc.right)/2;
	origin.y = (rc.top+rc.bottom)/3;
	CRect rcBar;
	m_wndCompStateBar.GetDefaultRect(&rcBar);
	origin -= rcBar.CenterPoint();
	// always call once FloatControlBar for a floating bar
	FloatControlBar(&m_wndCompStateBar, origin);
}

/**
 * @brief Store callback which we check to see if we're allowed to close
 * This keeps us decoupled from the doc
 */
void CDirFrame::SetClosableCallback(bool (*canclose)(void *), void * param)
{
	m_picanclose = canclose;
	m_param = param; 
}

void CDirFrame::OnClose() 
{
	if (m_picanclose && !(*m_picanclose)(m_param))
	{
		ShowWindow(SW_MINIMIZE);
		return;
	}
	
	CMDIChildWnd::OnClose();
}

/**
 * @brief Save maximized state before destroying window
 */
BOOL CDirFrame::DestroyWindow() 
{
	// If we are active, save the restored/maximized state
	// If we are not, do nothing and let the active frame do the job.
	if (this->GetParentFrame()->GetActiveFrame() == (CFrameWnd*)this)
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&wp);
		theApp.WriteProfileInt(_T("Settings"), _T("ActiveFrameMax"), (wp.showCmd == SW_MAXIMIZE));
	}
	return CMDIChildWnd::DestroyWindow();
}

void CDirFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	m_wndFilePathBar.Resize();
}

/// clear counters used to track diff progress
void CDirFrame::clearStatus()
{
	m_wndCompStateBar.Reset();
}

/** 
 * @brief Interface to show/hide the floating state bar
 */
void CDirFrame::ShowProcessingBar(BOOL bShow)
{
	if (bShow) 
	{
		ShowControlBar(&m_wndCompStateBar, TRUE, FALSE);
		// disable the list view as long as the state bar is shown
		GetActiveView()->EnableWindow(FALSE);
		m_wndCompStateBar.StartUpdating();
	}
	else if (!bShow)
	{
		m_wndCompStateBar.EndUpdating();
		ShowControlBar(&m_wndCompStateBar, FALSE, FALSE);
	}

	m_bStateBarIsActive = bShow;
}


/** 
 * @brief Enable the list view when the state bar becomes inactive
 */
void CDirFrame::NotifyHideStateBar()
{
	if (GetParentFrame()->GetActiveDocument() != NULL)
		return;
	if (!m_bFrameIsActive)
		// bar hidden because the frame get unactived
		return;
	
	m_bStateBarIsActive = FALSE;

	if (!GetActiveView()->IsWindowEnabled())
	{
		// enable the list view and set the focus
		GetActiveView()->EnableWindow();
		GetActiveView()->SetFocus();
	}
}

/**
 * @brief Override of the MFC functions (see windows help)
 * 
 * @note Hides the state bar when the document is inactive
 * (it is a topmost bar, and we don't want to display it above the new active document)
 */
void CDirFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{

	if (bActivate == TRUE)
		if (m_bStateBarIsActive)
			ShowControlBar(&m_wndCompStateBar, TRUE, FALSE);		

	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	m_bFrameIsActive = bActivate;
	
	if (bActivate == FALSE)
		if (m_bStateBarIsActive)
			ShowControlBar(&m_wndCompStateBar, FALSE, FALSE);
}

/** 
 * @brief Override of the MFC functions (see windows help)
 *
 * @note: this line 'pParentFrame->ShowWindow' is bad with MDI
 * It disturbs the coloring of the document caption if there are two docs
 * (light blue for inactive / blue for active)
 */
void CDirFrame::ShowControlBar( CControlBar* pBar, BOOL bShow, BOOL bDelay )
{
	ASSERT(pBar != NULL);
	CFrameWnd* pParentFrame = pBar->GetDockingFrame();
	ASSERT(pParentFrame->GetTopLevelParent() == GetTopLevelParent());
		// parent frame of bar must be related

	if (bDelay)
	{
		pBar->DelayShow(bShow);
		pParentFrame->DelayRecalcLayout();
	}
	else
	{
		pBar->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|
			(bShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
		// call DelayShow to clear any contradictory DelayShow
		pBar->DelayShow(bShow);
		if (bShow || !pBar->IsFloating())
			pParentFrame->RecalcLayout(FALSE);
	}

	// show or hide the floating frame as appropriate
	if (pBar->IsFloating())
	{
		int nVisCount = pBar->m_pDockBar != NULL ?
			pBar->m_pDockBar->GetDockedVisibleCount() : bShow ? 1 : 0;
		if (nVisCount == 1 && bShow)
		{
			pParentFrame->m_nShowDelay = -1;
			if (bDelay)
			{
				pParentFrame->m_nShowDelay = SW_SHOWNA;
				pParentFrame->RecalcLayout(FALSE);
			}
			else
				pParentFrame->ShowWindow(SW_SHOWNA);
		}
		else if (nVisCount == 0)
		{
			ASSERT(!bShow);
			pParentFrame->m_nShowDelay = -1;
			if (bDelay)
				pParentFrame->m_nShowDelay = SW_HIDE;
			else
				pParentFrame->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOACTIVATE  | SWP_NOMOVE | SWP_NOSIZE);
				// bug : this hides the window and give focus to the parent
				// (also parent may be disabled too)
				// pParentFrame->ShowWindow(SW_HIDE);
		}
		else if (!bDelay)
		{
			pParentFrame->RecalcLayout(FALSE);
		}
	}
}

void CDirFrame::SetCompareStats(CompareStats *pCompareStats)
{
	m_wndCompStateBar.SetCompareStat(pCompareStats);
}

void CDirFrame::UpdateStats()
{
	m_wndCompStateBar.UpdateElements();
}
