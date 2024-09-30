//////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeDiffDetailView.h
 *
 * @brief Declaration of CMergeDiffDetailView class
 */
// RCS ID line follows -- this is updated by CVS
// $Id: MergeDiffDetailView.h,v 1.9 2005/08/07 20:27:57 elsapo Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef __MERGEDIFFDETAILVIEW_H__
#define __MERGEDIFFDETAILVIEW_H__


/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView view
#ifndef __EDTLIB_H
#include "edtlib.h"
#endif



/**
 * @brief Class for Diff Pane View
 *
 * @note This class must not be used in a vertical scrollable splitter
 * as we want to scroll only in the current diff, but the vertical
 * scrollbar would be for the whole buffer.
 * There are three virtual functions : ScrollToSubLine/EnsureVisible/SetSelection
 * to be sure that the top line and the cursor/selection pos remains in the
 * current diff.
 */
class CMergeDiffDetailView : public CCrystalTextView
{
protected:
	CMergeDiffDetailView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMergeDiffDetailView)

// Attributes
public:
	BOOL m_bIsLeft;
protected:
	/// first line of diff (first displayable line)
	int m_lineBegin;
	/// last line of diff (last displayable line)
	int m_lineEnd; 
	/// number of displayed lines
	int m_diffLength;
	/// height (in lines) of the view
	int m_displayLength;

	/// memorize first line of diff
	int m_lineBeginPushed;
	/// memorize cursor position
	CPoint m_ptCursorPosPushed;
	/// memorize top line positions
	int m_nTopLinePushed;

// Operations
private:
	int GetDiffLineLength();

public:
	virtual CCrystalTextBuffer *LocateTextBuffer ();
	void DoScroll(UINT code, UINT pos, BOOL bDoScroll);
	CMergeDoc* GetDocument();
	void UpdateResources();
	BOOL IsModified() { return FALSE; }
	BOOL PrimeListWithFile();
	int ComputeInitialHeight(); 
	void SetDisplayHeight(int h); 
	virtual void UpdateSiblingScrollPos (BOOL bHorz);
	virtual void RecalcHorzScrollBar (BOOL bPositionOnly = FALSE );

	virtual void EnsureVisible (CPoint pt);
	virtual void SetSelection (const CPoint & ptStart, const CPoint & ptEnd);

	void SelectArea(const CPoint & ptStart, const CPoint & ptEnd) { SetSelection(ptStart, ptEnd); } // make public
	virtual void OnDisplayDiff(int nDiff=0);

	/* Push cursors before detaching buffer
	 *
	 * @note : laoran 2003/10/03 : don't bother with real lines. 
	 * I tried and it does not work fine
	 */
	void PushCursors();
	/*
	 * @brief Pop cursors after attaching buffer
	 *
	 * @note : also scroll to the old top line
	 */
	void PopCursors();

protected:
	BOOL EnsureInDiff(CPoint & pt);
  virtual void ScrollToSubLine (int nNewTopLine, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDiffDetailView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMergeDiffDetailView();
	virtual int GetAdditionalTextBlocks (int nLineIndex, TEXTBLOCK *pBuf);
	virtual COLORREF GetColor(int nColorIndex);
	virtual void GetLineColors (int nLineIndex, COLORREF & crBkgnd,
                              COLORREF & crText, BOOL & bDrawWhitespace);
	virtual void GetLineColors2 (int nLineIndex, DWORD ignoreFlags, COLORREF & crBkgnd,
                              COLORREF & crText, BOOL & bDrawWhitespace);
	virtual void OnUpdateSibling (CCrystalTextView * pUpdateSource, BOOL bHorz);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDiffDetailView)
	afx_msg void OnRefresh();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowlineworddiff();
	afx_msg void OnShowlinechardiff();
	afx_msg void OnUpdateShowlineworddiff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowlinechardiff(CCmdUI* pCmdUI);
	afx_msg void OnChangePane();
	afx_msg void OnUpdateChangePane(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiffView.cpp
inline CMergeDoc* CMergeDiffDetailView::GetDocument()
   { return (CMergeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif //__MERGEDIFFDETAILVIEW_H__
