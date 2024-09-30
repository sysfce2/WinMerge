//////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffViewBar.h
 *
 * @brief Declaration of CDiffViewBar class
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DiffViewBar.h,v 1.2.4.1 2006/01/07 12:03:21 kimmov Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef DIFFVIEWBAR_H
#define DIFFVIEWBAR_H


#include "sizecbar.h"
#include "scbarg.h"
#include "SplitterWndEx.h"

// You must #define this for viewbar to compile properly
#define TViewBarBase CSizingControlBarG
//#define TViewBarBase CSizingControlBar

class CDiffViewBar : public TViewBarBase
{
	DECLARE_DYNAMIC(CDiffViewBar);
public:
	CDiffViewBar();
	virtual ~CDiffViewBar();
	virtual BOOL Create(
		CWnd* pParentWnd,			// mandatory
		LPCTSTR lpszWindowName = NULL,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_PANE_FIRST);

	void setSplitter (CSplitterWndEx * pwndDetailSplitter)
		{	m_pwndDetailSplitter = pwndDetailSplitter; }

	int  GetPanelHeight();
	void UpdateBarHeight(int DiffPanelHeight);

protected:
	CSplitterWndEx * m_pwndDetailSplitter;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiffViewBar)
public:
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CDiffViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // DIFFVIEWBAR_H
