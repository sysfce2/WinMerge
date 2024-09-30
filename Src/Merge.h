/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
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
 * @file  Merge.h
 *
 * @brief main header file for the MERGE application
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: Merge.h,v 1.34.2.2 2006/02/15 20:20:48 kimmov Exp $

#if !defined(AFX_MERGE_H__BBCD4F88_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MERGE_H__BBCD4F88_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "MergeDoc.h"

#include "FileFilterHelper.h"

struct FileFilter;
class CAssureScriptsForThread;
class CMainFrame;
class CLanguageSelect;

/////////////////////////////////////////////////////////////////////////////
// CMergeApp:
// See Merge.cpp for the implementation of this class
//

enum { IDLE_TIMER = 9754 };

/** 
 * @brief WinMerge application class
 */
class CMergeApp : public CWinApp
{
public:
	BOOL m_bNeedIdleTimer;
	CMultiDocTemplate* m_pDiffTemplate;
	CMultiDocTemplate* m_pDirTemplate;
	CLanguageSelect * m_pLangDlg;
	FileFilterHelper m_globalFileFilter;

	WORD GetLangId() const;

	CMergeApp();
	~CMergeApp();

public:
	void SetNeedIdleTimer();
	void SetLastCompareResult(int nResult) { m_nLastCompareResult = nResult; }

// Implementation
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void InitializeFileFilters();
	void ParseArgs(int argc, TCHAR *argv[], CMainFrame* pMainFrame, CStringArray & files, UINT & nFiles, BOOL & recurse,
		DWORD & dwLeftFlags, DWORD & dwRightFlags);
	void ParseCCaseArgs(int argc, TCHAR *argv[], CMainFrame* pMainFrame, CStringArray & files, UINT & nFiles,
		DWORD & dwLeftFlags, DWORD & dwRightFlags);
	void ParseArgsAndDoOpen(int argc, TCHAR *argv[], CMainFrame* pMainFrame);
	BOOL LoadProjectFile(CStringArray & files, BOOL & recursive);



	//{{AFX_MSG(CMergeApp)
	afx_msg void OnAppAbout();
	afx_msg void OnViewLanguage();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CAssureScriptsForThread * m_mainThreadScripts;
	int m_nLastCompareResult;
	bool m_bNoninteractive;
};

extern CMergeApp theApp;

/////////////////////////////////////////////////////////////////////////////
CMergeDoc *GetDoc();
BOOL SelectFile(CString& path, LPCTSTR root_path = NULL, 
			 LPCTSTR title = _T("Open"), 
			 UINT filterid =0,
			 BOOL is_open =TRUE);

BOOL SelectFolder(CString& path, LPCTSTR root_path = NULL, 
			 LPCTSTR title = NULL, 
			 HWND hwndOwner = NULL);

void ConvertFilter(LPTSTR filterStr);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGE_H__BBCD4F88_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
