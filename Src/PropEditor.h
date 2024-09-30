/** 
 * @file  PropEditor.h
 *
 * @brief Declaration file for CPropEditor propertyheet
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: PropEditor.h,v 1.3 2005/05/03 15:12:45 elsapo Exp $

#if !defined(AFX_PROPEDITOR_H__1F2D57BB_6C09_488B_834D_575A94B2BDB8__INCLUDED_)
#define AFX_PROPEDITOR_H__1F2D57BB_6C09_488B_834D_575A94B2BDB8__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CPropEditor dialog

/**
 * @brief Property page for editor options.
 */
class CPropEditor : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropEditor)

// Construction
public:
	CPropEditor();

// Dialog Data
	//{{AFX_DATA(CPropEditor)
	enum { IDD = IDD_PROPPAGE_EDITOR };
	BOOL    m_bHiliteSyntax;
	int	    m_nTabType;
	UINT    m_nTabSize;
	BOOL    m_bAutomaticRescan;
	BOOL    m_bAllowMixedEol;
	BOOL    m_bApplySyntax;
	BOOL    m_bViewLineDifferences;
	BOOL    m_bBreakOnWords;
	int     m_nBreakType;
	//}}AFX_DATA

private:
// Implementation methods
	void LoadBreakTypeStrings();
	void UpdateDataToWindow() { UpdateData(FALSE); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropEditor)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnSyntaxHighlight();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPEDITOR_H__1F2D57BB_6C09_488B_834D_575A94B2BDB8__INCLUDED_)
