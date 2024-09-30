/** 
 * @file  PropCompare.h
 *
 * @brief Declaration of CPropCompare propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id: PropCompare.h 5040 2008-02-13 15:12:13Z kimmov $

#ifndef _PROPPAGE_COMPARE_H_
#define _PROPPAGE_COMPARE_H_

#include "IOptionsPanel.h"

class COptionsMgr;

/////////////////////////////////////////////////////////////////////////////
// CPropCompare dialog

/**
 * @brief Property page to set compare options for WinMerge.
 *
 * Whitespace compare:
 *  - Compare all whitespaces, recommended for merging!
 *  - Ignore changes in whitespaces (amount of spaces etc)
 *  - Ignore all whitespace characters
 *
 * Compare methods:
 *  - compare by contents
 *  - compare by modified date
 *  - compare by file size
 *  - compare by date and size
 *  - compare by quick contents
 *
 * @author Tim Musschoot, several modifications by others
 */
class CPropCompare : public CPropertyPage, public IOptionsPanel
{
// Construction
public:
	CPropCompare(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	//{{AFX_DATA(CPropCompare)
	enum { IDD = IDD_PROPPAGE_COMPARE };
	int     m_compareMethod;
	BOOL    m_bIgnoreEol;
	BOOL    m_bIgnoreCase;
	BOOL    m_bIgnoreBlankLines;
	int     m_nIgnoreWhite;
	BOOL    m_bMovedBlocks;
	BOOL    m_bMatchSimilarLines;
	BOOL    m_bStopAfterFirst;
	BOOL    m_bFilterCommentsLines;
	BOOL    m_bIgnoreSmallTimeDiff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropCompare)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropCompare)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnDefaults();
	afx_msg void OnCbnSelchangeComparemethodcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COptionsMgr * m_pOptionsMgr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _PROPPAGE_COMPARE_H_
