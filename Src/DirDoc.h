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
 * @file  DirDoc.h
 *
 * @brief Declaration file for CDirDoc
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DirDoc.h,v 1.55 2005/08/31 18:02:16 jtuc Exp $

#if !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_
#pragma once

#include "diffThread.h"
#include "DiffWrapper.h"

#ifndef PluginManager_h_included
#include "PluginManager.h"
#endif

#ifndef REGEXP_H
#include "RegExp.h"
#endif

class CDirView;
class CMergeDoc;
typedef CTypedPtrList<CPtrList, CMergeDoc *> MergeDocPtrList;
class DirDocFilterGlobal;
class DirDocFilterByExtension;
class CustomStatusCursor;
class CTempPathContext;
/////////////////////////////////////////////////////////////////////////////
// CDirDoc document

/**
 * @brief Documentclass for directory compare
 */
class CDirDoc : public CDocument
{
protected:
	CDirDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirDoc)

// Attributes
public:
	CTempPathContext *m_pTempPathContext;
// Operations
public:
	BOOL CloseMergeDocs();
	CDirView * GetMainView();
	CMergeDoc * GetMergeDocForDiff(BOOL * pNew);
	BOOL ReusingDirDoc();
	bool CanFrameClose();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void SetTitle(LPCTSTR lpszTitle);
	protected:
	virtual BOOL OnNewDocument();
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitCompare(const PathContext & paths, BOOL bRecursive, CTempPathContext *);
	void Rescan();
	BOOL GetRecursive() { return m_bRecursive; }
	BOOL GetReadOnly(BOOL bLeft) const;
	void SetReadOnly(BOOL bLeft, BOOL bReadOnly);
	BOOL HasDirView() { return m_pDirView != NULL; }
	void RefreshOptions();
	void CompareReady();
	void UpdateChangedItem(PathContext & paths,
		UINT nDiffs, UINT nTrivialDiffs, BOOL bIdentical);
	POSITION FindItemFromPaths(LPCTSTR pathLeft, LPCTSTR pathRight);
	void SetDiffSide(UINT diffcode, int idx);
	void SetDiffCompare(UINT diffcode, int idx);
	void UpdateResources();
	void InitStatusStrings();
	void ReloadItemStatus(UINT nIdx, BOOL bLeft, BOOL bRight);
	void Redisplay();
	virtual ~CDirDoc();
	void SetDirView( CDirView *newView ); // TODO Perry
	void AddMergeDoc(CMergeDoc * pMergeDoc);
	void MergeDocClosing(CMergeDoc * pMergeDoc);
	CDiffThread m_diffThread;
	void SetDiffStatus(UINT diffcode, UINT mask, int idx);
	void SetDiffCounts(UINT diffs, UINT ignored, int idx);
	void UpdateHeaderPath(BOOL bLeft);
	void AbortCurrentScan();
	bool IsCurrentScanAbortable() const;
	void SetDescriptions(const CString &strLeftDesc, const CString &strRightDesc);
	void ApplyLeftDisplayRoot(CString &);
	void ApplyRightDisplayRoot(CString &);

	void SetPluginPrediffSetting(const CString & filteredFilenames, int newsetting);
	void FetchPluginInfos(const CString& filteredFilenames, 
	                      PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);
	BOOL IsShowable(const DIFFITEM & di);

	BOOL HasDiffs() const { return m_pCtxt != NULL; }
	const CDiffContext & GetDiffContext() const { return *m_pCtxt; }
	DIFFITEM GetDiffByKey(POSITION key) const { return m_pCtxt->GetDiffAt(key); }
	DIFFITEM & GetDiffRefByKey(POSITION key) { return m_pCtxt->GetDiffRefAt(key); }
	CString GetLeftBasePath() const { return m_pCtxt->GetNormalizedLeft(); }
	CString GetRightBasePath() const { return m_pCtxt->GetNormalizedRight(); }
	void RemoveDiffByKey(POSITION key) { m_pCtxt->RemoveDiff(key); }
	void SetMarkedRescan() {m_bMarkedRescan = TRUE; }
	struct AllowUpwardDirectory
	{
		enum ReturnCode
		{
			Never,
			No,
			ParentIsRegularPath,
			ParentIsTempPath
		};
	};
	AllowUpwardDirectory::ReturnCode AllowUpwardDirectory(CString &leftParent, CString &rightParent);
	void SetItemViewFlag(POSITION key, UINT flag, UINT mask);
	void SetItemViewFlag(UINT flag, UINT mask);

protected:
	CDiffWrapper m_diffWrapper;

	// Generated message map functions
	//{{AFX_MSG(CDirDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation data
private:
	CDiffContext *m_pCtxt; /**< Pointer to diff-data */
	CDirView *m_pDirView;
	CompareStats *m_pCompareStats;
	MergeDocPtrList m_MergeDocs;
	BOOL m_bROLeft; /**< Is left side read-only */
	BOOL m_bRORight; /**< Is right side read-only */
	BOOL m_bRecursive; /**< Is current compare recursive? */
	CustomStatusCursor * m_statusCursor;
	CString m_strLeftDesc; /**< Left side desription text */
	CString m_strRightDesc; /**< Left side desription text */
	PluginManager m_pluginman;
	BOOL m_bReuseCloses; /**< Are we closing because of reuse? */
	BOOL m_bMarkedRescan; /**< If TRUE next rescan scans only marked items */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
