/** 
 * @file PreferencesDlg.cpp
 *
 * @brief Implementation file for CPreferencesDlg
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */
// RCS ID line follows -- this is updated by CVS
// $Id: PreferencesDlg.cpp,v 1.9 2005/08/29 16:27:34 kimmov Exp $

#include "stdafx.h"
#include "resource.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "SyntaxColors.h"
#include "PreferencesDlg.h"
#include "MainFrm.h"

#include "winclasses.h"
#include "wclassdefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

const TCHAR PATHDELIM = '>';

CPreferencesDlg::CPreferencesDlg(COptionsMgr *regOptions, SyntaxColors *colors,
		UINT nMenuID, CWnd* pParent)   // standard constructor
	: CDialog(IDD_PREFERENCES, pParent), m_pOptionsMgr(regOptions), m_pageCompare(regOptions),
	m_pageColors(regOptions), m_pSyntaxColors(colors), m_pageSyntaxColors(colors)
{
	UNREFERENCED_PARAMETER(nMenuID);
}

CPreferencesDlg::~CPreferencesDlg()
{
}

void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDlg)
	DDX_Control(pDX, IDC_TREEOPT_PAGES, m_tcPages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TREEOPT_HELP, OnHelpButton)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEOPT_PAGES, OnSelchangedPages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg message handlers

BOOL CPreferencesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_tcPages.SetIndent(0);

	// Second parameter is 'path', page's parent page(s) and caption.
	// '>' is used as path separator.
	// For example "General" creates top-level "General" page
	// and "General>Colors" creates "Colors" sub-page for "General"
	AddPage(&m_pageGeneral, IDS_OPTIONSPG_GENERAL);
	AddPage(&m_pageCompare, IDS_OPTIONSPG_COMPARE);
	AddPage(&m_pageEditor, IDS_OPTIONSPG_EDITOR);
	AddPage(&m_pageColors, IDS_OPTIONSPG_COLORS);
	AddPage(&m_pageSyntaxColors, IDS_OPTIONSPG_SYNTAXCOLORS);
	AddPage(&m_pageArchive, IDS_OPTIONSPG_ARCHIVE);
	AddPage(&m_pageSystem, IDS_OPTIONSPG_SYSTEM);
	AddPage(&m_pageVss, IDS_OPTIONSPG_VERSIONCONTROL);
	AddPage(&m_pageCodepage, IDS_OPTIONSPG_CODEPAGE);

	ReadOptions();
	
	CRect rPPHost;
	GetDlgItem(IDC_TREEOPT_HOSTFRAME)->GetWindowRect(rPPHost);
	ScreenToClient(rPPHost);

	if (m_pphost.Create(rPPHost, this))
		SetActivePage(AfxGetApp()->GetProfileInt(_T("Settings"), _T("OptStartPage"), 0));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesDlg::OnOK()
{
	CDialog::OnOK();
	m_pphost.OnOK();

	SaveOptions();
}

void CPreferencesDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("OptStartPage"), m_pphost.GetActiveIndex());
}

void CPreferencesDlg::OnHelpButton() 
{
	CMainFrame *pMf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	pMf->ShowHelp();
}

void CPreferencesDlg::AddPage(CPropertyPage* pPage, UINT nResourceID)
{
	CString sPath;
	VERIFY(sPath.LoadString(nResourceID));
	AddPage(pPage, sPath);
}

void CPreferencesDlg::AddPage(CPropertyPage* pPage, LPCTSTR szPath)
{
	CString sPath(szPath);

	if (m_pphost.AddPage(pPage))
	{
		HTREEITEM htiParent = TVI_ROOT; // default
		int nFind = sPath.Find(PATHDELIM);

		while (nFind != -1)
		{
			CString sParent = sPath.Left(nFind);
			sPath = sPath.Mid(nFind + 1);

			// see if parent already exists
			HTREEITEM htiParentParent = htiParent;
			htiParent = m_tcPages.GetChildItem(htiParentParent);

			while (htiParent)
			{
				if (sParent.CompareNoCase(m_tcPages.GetItemText(htiParent)) == 0)
					break;

				htiParent = m_tcPages.GetNextItem(htiParent, TVGN_NEXT);
			}

			if (!htiParent)
				htiParent = m_tcPages.InsertItem(sParent, htiParentParent);

			nFind = sPath.Find(PATHDELIM);
		}

		HTREEITEM hti = m_tcPages.InsertItem(sPath, htiParent); // whatever's left
		m_tcPages.EnsureVisible(hti);

		// map both ways
		m_tcPages.SetItemData(hti, (DWORD)pPage);
		m_mapPP2HTI[(void*)pPage] = (void*)hti;
	}
}

void CPreferencesDlg::OnSelchangedPages(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNREFERENCED_PARAMETER(pNMHDR);
	HTREEITEM htiSel = m_tcPages.GetSelectedItem();

	while (m_tcPages.ItemHasChildren(htiSel))
		htiSel = m_tcPages.GetChildItem(htiSel);

	CPropertyPage* pPage = (CPropertyPage*)m_tcPages.GetItemData(htiSel);
	ASSERT (pPage);

	if (pPage)
	{
		m_pphost.SetActivePage(pPage, FALSE);

		// update caption
		CString sCaption;
		AfxFormatString1(sCaption, IDS_OPTIONS_TITLE, GetItemPath(htiSel));
		SetWindowText(sCaption);
	}

	m_tcPages.SetFocus();
	
	*pResult = 0;
}

void CPreferencesDlg::SetActivePage(int nPage)
{
	m_pphost.SetActivePage(nPage, FALSE);

	// synchronize tree
	CPropertyPage* pPage = m_pphost.GetActivePage();
	HTREEITEM hti = NULL;

	if (m_mapPP2HTI.Lookup(pPage, (void*&)hti) && hti)
		m_tcPages.SelectItem(hti);
}

CString CPreferencesDlg::GetItemPath(HTREEITEM hti)
{
	CString sPath = m_tcPages.GetItemText(hti);

	while (hti = m_tcPages.GetParentItem(hti))
		sPath = m_tcPages.GetItemText(hti) + _T(" > ") + sPath;

	return sPath;
}

void CPreferencesDlg::SetDefaultEditor(LPCTSTR szDefaultEditor)
{
	m_sDefaultEditor = szDefaultEditor;
}

void CPreferencesDlg::ReadOptions()
{
	m_pageGeneral.m_bBackup = m_pOptionsMgr->GetBool(OPT_CREATE_BACKUPS);
	m_pageGeneral.m_bScroll = m_pOptionsMgr->GetBool(OPT_SCROLL_TO_FIRST);
	m_pageGeneral.m_bDisableSplash = m_pOptionsMgr->GetBool(OPT_DISABLE_SPLASH);
	m_pageGeneral.m_bAutoCloseCmpPane = m_pOptionsMgr->GetBool(OPT_AUTOCLOSE_CMPPANE);
	m_pageGeneral.m_bSingleInstance = m_pOptionsMgr->GetBool(OPT_SINGLE_INSTANCE);
	m_pageGeneral.m_bVerifyPaths = m_pOptionsMgr->GetBool(OPT_VERIFY_OPEN_PATHS);
	m_pageGeneral.m_bCloseWindowWithEsc = m_pOptionsMgr->GetBool(OPT_CLOSE_WITH_ESC);
	m_pageGeneral.m_bMultipleFileCmp = m_pOptionsMgr->GetBool(OPT_MULTIDOC_MERGEDOCS);
	m_pageGeneral.m_bMultipleDirCmp = m_pOptionsMgr->GetBool(OPT_MULTIDOC_DIRDOCS);

	m_pageSystem.m_strEditorPath = m_pOptionsMgr->GetString(OPT_EXT_EDITOR_CMD);
	m_pageSystem.GetContextRegValues();
	m_pageSystem.m_bUseRecycleBin = m_pOptionsMgr->GetBool(OPT_USE_RECYCLE_BIN);
	m_pageSystem.m_bIgnoreSmallTimeDiff = m_pOptionsMgr->GetBool(OPT_IGNORE_SMALL_FILETIME);

	m_pageCompare.m_nIgnoreWhite = m_pOptionsMgr->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	m_pageCompare.m_bIgnoreBlankLines = m_pOptionsMgr->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	m_pageCompare.m_bIgnoreCase = m_pOptionsMgr->GetBool(OPT_CMP_IGNORE_CASE);
	m_pageCompare.m_bEolSensitive = m_pOptionsMgr->GetBool(OPT_CMP_EOL_SENSITIVE) ? false : true; // Reverse
	m_pageCompare.m_bMovedBlocks = m_pOptionsMgr->GetBool(OPT_CMP_MOVED_BLOCKS);
	m_pageCompare.m_compareMethod = m_pOptionsMgr->GetInt(OPT_CMP_METHOD);
	m_pageCompare.m_bStopAfterFirst = m_pOptionsMgr->GetBool(OPT_CMP_STOP_AFTER_FIRST);

	m_pageEditor.m_nTabSize = m_pOptionsMgr->GetInt(OPT_TAB_SIZE);
	m_pageEditor.m_nTabType = m_pOptionsMgr->GetInt(OPT_TAB_TYPE);
	m_pageEditor.m_bAutomaticRescan = m_pOptionsMgr->GetBool(OPT_AUTOMATIC_RESCAN);
	m_pageEditor.m_bHiliteSyntax = m_pOptionsMgr->GetBool(OPT_SYNTAX_HIGHLIGHT);
	m_pageEditor.m_bAllowMixedEol = m_pOptionsMgr->GetBool(OPT_ALLOW_MIXED_EOL);
	m_pageEditor.m_bApplySyntax = m_pOptionsMgr->GetBool(OPT_UNREC_APPLYSYNTAX);
	m_pageEditor.m_bViewLineDifferences = m_pOptionsMgr->GetBool(OPT_WORDDIFF_HIGHLIGHT);
	m_pageEditor.m_bBreakOnWords = m_pOptionsMgr->GetBool(OPT_BREAK_ON_WORDS);
	m_pageEditor.m_nBreakType = m_pOptionsMgr->GetInt(OPT_BREAK_TYPE);

	m_pageCodepage.m_nCodepageSystem = m_pOptionsMgr->GetInt(OPT_CP_DEFAULT_MODE);
	m_pageCodepage.m_nCustomCodepageValue = m_pOptionsMgr->GetInt(OPT_CP_DEFAULT_CUSTOM);
	m_pageCodepage.m_bDetectCodepage = m_pOptionsMgr->GetBool(OPT_CP_DETECT);

	m_pageVss.m_nVerSys = m_pOptionsMgr->GetInt(OPT_VCS_SYSTEM);
	m_pageVss.m_strPath = m_pOptionsMgr->GetString(OPT_VSS_PATH);

	int enable = m_pOptionsMgr->GetInt(OPT_ARCHIVE_ENABLE);
	m_pageArchive.m_bEnableSupport = enable > 0;
	m_pageArchive.m_nInstallType = enable > 1 ? enable - 1 : 0;
	m_pageArchive.m_bProbeType = m_pOptionsMgr->GetBool(OPT_ARCHIVE_PROBETYPE);
}

void CPreferencesDlg::SaveOptions()
{
	CString sExtEditor;

	m_pOptionsMgr->SaveOption(OPT_CREATE_BACKUPS, m_pageGeneral.m_bBackup == TRUE);
	m_pOptionsMgr->SaveOption(OPT_SCROLL_TO_FIRST, m_pageGeneral.m_bScroll == TRUE);
	m_pOptionsMgr->SaveOption(OPT_DISABLE_SPLASH, m_pageGeneral.m_bDisableSplash == TRUE);
	m_pOptionsMgr->SaveOption(OPT_AUTOCLOSE_CMPPANE, m_pageGeneral.m_bAutoCloseCmpPane == TRUE);
	m_pOptionsMgr->SaveOption(OPT_SINGLE_INSTANCE, m_pageGeneral.m_bSingleInstance == TRUE);
	m_pOptionsMgr->SaveOption(OPT_VERIFY_OPEN_PATHS, m_pageGeneral.m_bVerifyPaths == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CLOSE_WITH_ESC, m_pageGeneral.m_bCloseWindowWithEsc == TRUE);
	m_pOptionsMgr->SaveOption(OPT_MULTIDOC_MERGEDOCS, m_pageGeneral.m_bMultipleFileCmp == TRUE);
	m_pOptionsMgr->SaveOption(OPT_MULTIDOC_DIRDOCS, m_pageGeneral.m_bMultipleDirCmp == TRUE);

	m_pOptionsMgr->SaveOption(OPT_USE_RECYCLE_BIN, m_pageSystem.m_bUseRecycleBin == TRUE);
	m_pOptionsMgr->SaveOption(OPT_IGNORE_SMALL_FILETIME, m_pageSystem.m_bIgnoreSmallTimeDiff == TRUE);


	m_pageSystem.SaveMergePath();
	sExtEditor = m_pageSystem.m_strEditorPath;
	sExtEditor.TrimLeft();
	sExtEditor.TrimRight();
	if (sExtEditor.IsEmpty())
		sExtEditor = m_sDefaultEditor;
	m_pOptionsMgr->SaveOption(OPT_EXT_EDITOR_CMD, sExtEditor);

	m_pOptionsMgr->SaveOption(OPT_CMP_IGNORE_WHITESPACE, m_pageCompare.m_nIgnoreWhite);
	m_pOptionsMgr->SaveOption(OPT_CMP_IGNORE_BLANKLINES, m_pageCompare.m_bIgnoreBlankLines == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_EOL_SENSITIVE, m_pageCompare.m_bEolSensitive == FALSE); // Reverse
	m_pOptionsMgr->SaveOption(OPT_CMP_IGNORE_CASE, m_pageCompare.m_bIgnoreCase == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_METHOD, m_pageCompare.m_compareMethod);
	m_pOptionsMgr->SaveOption(OPT_CMP_MOVED_BLOCKS, m_pageCompare.m_bMovedBlocks == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_STOP_AFTER_FIRST, m_pageCompare.m_bStopAfterFirst == TRUE);
	
	m_pOptionsMgr->SaveOption(OPT_TAB_SIZE, (int)m_pageEditor.m_nTabSize);
	m_pOptionsMgr->SaveOption(OPT_TAB_TYPE, (int)m_pageEditor.m_nTabType);
	m_pOptionsMgr->SaveOption(OPT_AUTOMATIC_RESCAN, m_pageEditor.m_bAutomaticRescan == TRUE);
	m_pOptionsMgr->SaveOption(OPT_ALLOW_MIXED_EOL, m_pageEditor.m_bAllowMixedEol == TRUE);
	m_pOptionsMgr->SaveOption(OPT_SYNTAX_HIGHLIGHT, m_pageEditor.m_bHiliteSyntax == TRUE);
	m_pOptionsMgr->SaveOption(OPT_UNREC_APPLYSYNTAX, m_pageEditor.m_bApplySyntax == TRUE);
	m_pOptionsMgr->SaveOption(OPT_WORDDIFF_HIGHLIGHT, !!m_pageEditor.m_bViewLineDifferences);
	m_pOptionsMgr->SaveOption(OPT_BREAK_ON_WORDS, !!m_pageEditor.m_bBreakOnWords);
	m_pOptionsMgr->SaveOption(OPT_BREAK_TYPE, m_pageEditor.m_nBreakType);

	m_pOptionsMgr->SaveOption(OPT_CLR_DIFF, m_pageColors.m_clrDiff);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_DIFF, m_pageColors.m_clrSelDiff);
	m_pOptionsMgr->SaveOption(OPT_CLR_DIFF_DELETED, m_pageColors.m_clrDiffDeleted);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_DIFF_DELETED, m_pageColors.m_clrSelDiffDeleted);
	m_pOptionsMgr->SaveOption(OPT_CLR_DIFF_TEXT, m_pageColors.m_clrDiffText);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_DIFF_TEXT, m_pageColors.m_clrSelDiffText);
	m_pOptionsMgr->SaveOption(OPT_CLR_TRIVIAL_DIFF, m_pageColors.m_clrTrivial);
	m_pOptionsMgr->SaveOption(OPT_CLR_TRIVIAL_DIFF_DELETED, m_pageColors.m_clrTrivialDeleted);
	m_pOptionsMgr->SaveOption(OPT_CLR_TRIVIAL_DIFF_TEXT, m_pageColors.m_clrTrivialText);
	m_pOptionsMgr->SaveOption(OPT_CLR_MOVEDBLOCK, m_pageColors.m_clrMoved);
	m_pOptionsMgr->SaveOption(OPT_CLR_MOVEDBLOCK_DELETED, m_pageColors.m_clrMovedDeleted);
	m_pOptionsMgr->SaveOption(OPT_CLR_MOVEDBLOCK_TEXT, m_pageColors.m_clrMovedText);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_MOVEDBLOCK, m_pageColors.m_clrSelMoved);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, m_pageColors.m_clrSelMovedDeleted);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, m_pageColors.m_clrSelMovedText);
	m_pOptionsMgr->SaveOption(OPT_CLR_WORDDIFF, m_pageColors.m_clrWordDiff);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_WORDDIFF, m_pageColors.m_clrSelWordDiff);
	m_pOptionsMgr->SaveOption(OPT_CLR_WORDDIFF_TEXT, m_pageColors.m_clrWordDiffText);
	m_pOptionsMgr->SaveOption(OPT_CLR_SELECTED_WORDDIFF_TEXT, m_pageColors.m_clrSelWordDiffText);

	m_pOptionsMgr->SaveOption(OPT_CP_DEFAULT_MODE, m_pageCodepage.m_nCodepageSystem);
	m_pOptionsMgr->SaveOption(OPT_CP_DEFAULT_CUSTOM, m_pageCodepage.m_nCustomCodepageValue);
	m_pOptionsMgr->SaveOption(OPT_CP_DETECT, m_pageCodepage.m_bDetectCodepage == TRUE);

	m_pOptionsMgr->SaveOption(OPT_VCS_SYSTEM, (int)m_pageVss.m_nVerSys);
	m_pOptionsMgr->SaveOption(OPT_VSS_PATH, m_pageVss.m_strPath);

	m_pSyntaxColors->Clone(m_pageSyntaxColors.m_pTempColors);
	m_pSyntaxColors->SaveToRegistry();

	if (m_pageArchive.m_bEnableSupport)
		m_pOptionsMgr->SaveOption(OPT_ARCHIVE_ENABLE, m_pageArchive.m_nInstallType + 1);
	else
		m_pOptionsMgr->SaveOption(OPT_ARCHIVE_ENABLE, (int)0);
	m_pOptionsMgr->SaveOption(OPT_ARCHIVE_PROBETYPE, m_pageArchive.m_bProbeType == TRUE);
}

void CPreferencesDlg::SetSyntaxColors(SyntaxColors *pColors)
{
	m_pSyntaxColors = pColors;
}
