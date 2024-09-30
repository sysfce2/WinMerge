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
 * @file  MergeDoc.cpp
 *
 * @brief Implementation file for CMergeDoc
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: MergeDoc.cpp 4891 2008-01-09 23:22:08Z gerundt $

#include "stdafx.h"
#include <Shlwapi.h>		// PathCompactPathEx()

#include "Merge.h"
#include "direct.h"
#include "MainFrm.h"

#include "diff.h"
#include "diffcontext.h"	// FILE_SAME
#include "getopt.h"
#include "fnmatch.h"
#include "coretools.h"
#include "VssPrompt.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "cs2cs.h"
#include "childFrm.h"
#include "dirdoc.h"
#include "files.h"
#include "WaitStatusCursor.h"
#include "FileTransform.h"
#include "unicoder.h"
#include "UniFile.h"
#include "locality.h"
#include "OptionsDef.h"
#include "DiffFileInfo.h"
#include "SaveClosingDlg.h"
#include "DiffList.h"
#include "sbuffer.h"
#include "dllver.h"
#include "codepage.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "ProjectFile.h"
#include "MergeLineFlags.h"
#include "FileOrFolderSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLogFile gLog;

/** @brief Max len of path in caption. */
static const UINT CAPTION_PATH_MAX = 50;

/** @brief EOL types */
static LPCTSTR crlfs[] =
{
	_T ("\x0d\x0a"), //  DOS/Windows style
	_T ("\x0a"),     //  UNIX style
	_T ("\x0d")      //  Macintosh style
};

static CString GetLineByteTimeReport(UINT lines, __int64 bytes,
	const COleDateTime & start);

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc

IMPLEMENT_DYNCREATE(CMergeDoc, CDocument)

BEGIN_MESSAGE_MAP(CMergeDoc, CDocument)
	//{{AFX_MSG_MAP(CMergeDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_LEFT, OnFileSaveLeft)
	ON_COMMAND(ID_FILE_SAVE_RIGHT, OnFileSaveRight)
	ON_COMMAND(ID_FILE_SAVEAS_LEFT, OnFileSaveAsLeft)
	ON_COMMAND(ID_FILE_SAVEAS_RIGHT, OnFileSaveAsRight)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	ON_COMMAND(ID_FILE_ENCODING, OnFileEncoding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc construction/destruction


#pragma warning(disable:4355)

/**
 * @brief Constructor.
 */
CMergeDoc::CMergeDoc()
: m_pTempFiles(NULL)
, m_bEnableRescan(TRUE)
, m_nCurDiff(-1)
, m_pDirDoc(NULL)
{
	DIFFOPTIONS options = {0};

	// COleDateTime m_LastRescan
	curUndo = undoTgt.begin();
	m_pView[MERGE_VIEW_LEFT] = NULL;
	m_pView[MERGE_VIEW_RIGHT] = NULL;
	m_pDetailView[0] = NULL;
	m_pDetailView[1] = NULL;
	m_pInfoUnpacker = new PackingInfo;
	m_nBufferType[0] = BUFFER_NORMAL;
	m_nBufferType[1] = BUFFER_NORMAL;
	m_bMergingMode = GetOptionsMgr()->GetBool(OPT_MERGE_MODE);
	m_bEditAfterRescan[0] = FALSE;
	m_bEditAfterRescan[1] = FALSE;
	m_ptBuf[0] = new CDiffTextBuffer(this, 0);
	m_ptBuf[1] = new CDiffTextBuffer(this, 1);
	m_pSaveFileInfo[0] = new DiffFileInfo();
	m_pSaveFileInfo[1] = new DiffFileInfo();
	m_pRescanFileInfo[0] = new DiffFileInfo();
	m_pRescanFileInfo[1] = new DiffFileInfo();

	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);

	m_diffWrapper.SetOptions(&options);
	m_diffWrapper.SetPrediffer(NULL);
}

#pragma warning(default:4355)

/**
 * @brief Destructor.
 *
 * Informs associated dirdoc that mergedoc is closing.
 */
CMergeDoc::~CMergeDoc()
{	
	if (m_pDirDoc)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = NULL;
	}

	delete m_pInfoUnpacker;
	delete m_pTempFiles;
	delete m_pSaveFileInfo[0];
	delete m_pSaveFileInfo[1];
	delete m_pRescanFileInfo[0];
	delete m_pRescanFileInfo[1];
	delete m_ptBuf[0];
	delete m_ptBuf[1];
}

/**
 * @brief Deleted data associated with doc before closing.
 */
void CMergeDoc::DeleteContents ()
{
	CDocument::DeleteContents ();
	m_ptBuf[0]->FreeAll ();
	m_ptBuf[1]->FreeAll ();
	delete m_pTempFiles;
	m_pTempFiles = NULL;
}

void CMergeDoc::OnFileEvent (WPARAM /*wEvent*/, LPCTSTR /*pszPathName*/)
{
	/*if (!(theApp.m_dwFlags & EP_NOTIFY_CHANGES))
    return;
	MessageBeep (MB_ICONEXCLAMATION);
	CFrameWnd *pwndMain= (CFrameWnd*) theApp.GetMainWnd ();
	ASSERT (pwndMain);
	if (!pwndMain->IsWindowVisible ())
          ((CMainFrame*) pwndMain)->FlashUntilFocus ();
	if (wEvent & FE_MODIFIED)
  	{
  	  bool bReload = (theApp.m_dwFlags & EP_AUTO_RELOAD) != 0;
  	  if (!bReload)
  	    {
          CString sMsg;
          sMsg.Format (IDS_FILE_CHANGED, pszPathName);
  	      bReload = AfxMessageBox (sMsg, MB_YESNO|MB_ICONQUESTION) == IDYES;
  	    }
  	  if (bReload)
        {
	        POSITION pos = GetFirstViewPosition ();
          ASSERT (pos);
	        CEditPadView *pView;
          do
            {
	            pView = (CEditPadView*) GetNextView (pos);
              pView->PushCursor ();
            }
          while (pos);
          m_xTextBuffer.FreeAll ();
          m_xTextBuffer.LoadFromFile (pszPathName);
	        pos = GetFirstViewPosition ();
          ASSERT (pos);
          do
            {
	            pView = (CEditPadView*) GetNextView (pos);
              pView->PopCursor ();
              HWND hWnd = pView->GetSafeHwnd ();
              ::RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT|RDW_ERASE|RDW_ERASENOW|RDW_UPDATENOW|RDW_NOFRAME);
            }
          while (pos);
        }
    }
  else if (wEvent & FE_DELETED)
    {
      if (!(theApp.m_dwFlags & EP_AUTO_RELOAD))
        {
          CString sMsg;
          sMsg.Format (IDS_FILE_DELETED, pszPathName);
        	AfxMessageBox (sMsg, MB_OK|MB_ICONINFORMATION);
        }
    }*/
}

/**
 * @brief Called when new document is created.
 *
 * Initialises buffers.
 */
BOOL CMergeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString s;
	VERIFY(s.LoadString(IDS_FILE_COMPARISON_TITLE));
	SetTitle(s);
	
	m_ptBuf[0]->InitNew ();
	m_ptBuf[1]->InitNew ();
	return TRUE;
}

/**
 * @brief Determines currently active view.
 * @return one of MERGEVIEW_INDEX_TYPE values or -1 in error.
 */
int CMergeDoc::GetActiveMergeViewIndexType() const
{
	CMergeDoc * pThis = const_cast<CMergeDoc *>(this);
	// Get active view pointer
	CView * pActiveView = pThis->GetParentFrame()->GetActiveView();
	// Cast it to common base of all our views
	CCrystalTextView* curView = dynamic_cast<CCrystalTextView*> (pActiveView);
	// Now test it against all our views to see which it is
	if (curView == GetLeftView())
		return MERGEVIEW_LEFT;
	else if (curView == GetRightView())
		return MERGEVIEW_RIGHT;
	else if (curView == GetLeftDetailView())
		return MERGEVIEW_LEFT_DETAIL;
	else if (curView == GetRightDetailView())
		return MERGEVIEW_RIGHT_DETAIL;

	_RPTF0(_CRT_ERROR, "Invalid view pointer!");
	return -1;
}

/**
 * @brief Return active merge edit view (or left one if neither active)
 */
CMergeEditView * CMergeDoc::GetActiveMergeView()
{
	CView * pActiveView = GetParentFrame()->GetActiveView();
	CMergeEditView * pMergeEditView = dynamic_cast<CMergeEditView *>(pActiveView);
	if (!pMergeEditView)
		pMergeEditView = GetLeftView(); // default to left view (in case some location or detail view active)
	return pMergeEditView;
}

void CMergeDoc::SetUnpacker(PackingInfo * infoNewHandler)
{
	if (infoNewHandler)
	{
		*m_pInfoUnpacker = *infoNewHandler;
	}
}

void CMergeDoc::SetPrediffer(PrediffingInfo * infoPrediffer)
{
	m_diffWrapper.SetPrediffer(infoPrediffer);
}
void CMergeDoc::GetPrediffer(PrediffingInfo * infoPrediffer)
{
	m_diffWrapper.GetPrediffer(infoPrediffer);
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc serialization

void CMergeDoc::Serialize(CArchive& ar)
{
	ASSERT(0); // we do not use CDocument serialization
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc commands

/**
 * @brief Save an editor text buffer to a file for prediffing (make UCS-2LE if appropriate)
 *
 * @note 
 * original file is Ansi : 
 *   buffer  -> save as Ansi -> Ansi plugins -> diffutils
 * original file is Unicode (UCS2-LE, UCS2-BE, UTF-8) :
 *   buffer  -> save as UCS2-LE -> Unicode plugins -> convert to UTF-8 -> diffutils
 * (the plugins are optional, not the conversion)
 * @todo Show SaveToFile() errors?
 */
static void SaveBuffForDiff(CMergeDoc::CDiffTextBuffer & buf, const CString & filepath)
{
	ASSERT(buf.m_nSourceEncoding == buf.m_nDefaultEncoding);  
	int orig_codepage = buf.getCodepage();
	int orig_unicoding = buf.getUnicoding();	

	// If file was in Unicode
	if (orig_unicoding!=ucr::NONE)
	{
	// we subvert the buffer's memory of the original file encoding
		buf.setUnicoding(ucr::UCS2LE);  // write as UCS-2LE (for preprocessing)
		buf.setCodepage(0); // should not matter
	}

	// and we don't repack the file
	PackingInfo * tempPacker = NULL;

	// write buffer out to temporary file
	CString sError;
	int retVal = buf.SaveToFile(filepath, TRUE, sError, tempPacker,
		CRLF_STYLE_AUTOMATIC, FALSE);

	// restore memory of encoding of original file
	buf.setUnicoding(orig_unicoding);
	buf.setCodepage(orig_codepage);
}

/**
 * @brief Save files to temp files & compare again.
 *
 * @param bBinary [in,out] [in] If TRUE, compare two binary files
 * [out] If TRUE binary file was detected.
 * @param bIdentical [out] If TRUE files were identical
 * @param bForced [in] If TRUE, suppressing is ignored and rescan
 * is done always
 * @return Tells if rescan was successfully, was suppressed, or
 * error happened
 * If this code is OK, Rescan has detached the views temporarily
 * (positions of cursors have been lost)
 * @note Rescan() ALWAYS compares temp files. Actual user files are not
 * touched by Rescan().
 * @sa CDiffWrapper::RunFileDiff()
 */
int CMergeDoc::Rescan(BOOL &bBinary, BOOL &bIdentical,
		BOOL bForced /* =FALSE */)
{
	DIFFOPTIONS diffOptions = {0};
	DiffFileInfo fileInfo;
	BOOL diffSuccess;
	int nResult = RESCAN_OK;
	BOOL bLeftFileChanged = FALSE;
	BOOL bRightFileChanged = FALSE;

	if (!bForced)
	{
		if (!m_bEnableRescan)
			return RESCAN_SUPPRESSED;
	}

	bLeftFileChanged = IsFileChangedOnDisk(m_filePaths.GetLeft(), fileInfo,
		FALSE, 0);
	bRightFileChanged = IsFileChangedOnDisk(m_filePaths.GetRight(), fileInfo,
		FALSE, 1);
	m_LastRescan = COleDateTime::GetCurrentTime();

	// get the desired files to temp locations so we can edit them dynamically
	if (m_pTempFiles == NULL)
	{
		m_pTempFiles = new TempFileContext;
		if (m_pTempFiles == NULL)
			return RESCAN_TEMP_ERR;

		if (!m_pTempFiles->CreateFiles(m_filePaths))
			return RESCAN_TEMP_ERR;

		m_ptBuf[0]->SetTempPath(m_pTempFiles->GetTempPath());
		m_ptBuf[1]->SetTempPath(m_pTempFiles->GetTempPath());
	}

	// output buffers to temp files (in UTF-8 if TCHAR=wchar_t or buffer was Unicode)
	if (bBinary == FALSE)
	{
		SaveBuffForDiff(*m_ptBuf[0], m_pTempFiles->GetLeft());
		SaveBuffForDiff(*m_ptBuf[1], m_pTempFiles->GetRight());
	}

	// Set up DiffWrapper
	m_diffWrapper.SetCreateDiffList(&m_diffList);
	m_diffWrapper.GetOptions(&diffOptions);
	
	// Clear diff list
	m_diffList.Clear();
	m_nCurDiff = -1;
	// Clear moved lines lists
	m_diffWrapper.ClearMovedLists();

	// Set paths for diffing and run diff
	m_diffWrapper.SetPaths(m_pTempFiles->GetLeft(), m_pTempFiles->GetRight(), TRUE);
	m_diffWrapper.SetCompareFiles(m_filePaths.GetLeft(), m_filePaths.GetRight());
	diffSuccess = m_diffWrapper.RunFileDiff();

	// Read diff-status
	DIFFSTATUS status;
	m_diffWrapper.GetDiffStatus(&status);
	if (bBinary) // believe caller if we were told these are binaries
		status.bBinaries = TRUE;

	// If comparing whitespaces and
	// other file has EOL before EOF and other not...
	if (status.bLeftMissingNL != status.bRightMissingNL &&
		!diffOptions.nIgnoreWhitespace && !diffOptions.bIgnoreBlankLines)
	{
		// ..lasf DIFFRANGE of file which has EOL must be
		// fixed to contain last line too
		m_diffWrapper.FixLastDiffRange(m_ptBuf[0]->GetLineCount(), m_ptBuf[1]->GetLineCount(),
				status.bRightMissingNL);
	}

	// set identical/diff result as recorded by diffutils
	bIdentical = status.bIdentical;

	// Determine errors and binary file compares
	if (!diffSuccess)
		nResult = RESCAN_FILE_ERR;
	else if (status.bBinaries)
	{
		bBinary = TRUE;
	}
	else
	{
		// Now update views and buffers for ghost lines

		// Prevent displaying views during this update 
		// BTW, this solves the problem of double asserts
		// (during the display of an assert message box, a second assert in one of the 
		//  display functions happens, and hides the first assert)
		m_pView[MERGE_VIEW_LEFT]->DetachFromBuffer();
		m_pView[MERGE_VIEW_RIGHT]->DetachFromBuffer();
		m_pDetailView[0]->DetachFromBuffer();
		m_pDetailView[1]->DetachFromBuffer();

		// Remove blank lines and clear winmerge flags
		// this operation does not change the modified flag
		m_ptBuf[0]->prepareForRescan();
		m_ptBuf[1]->prepareForRescan();

		// Analyse diff-list (updating real line-numbers)
		// this operation does not change the modified flag
		PrimeTextBuffers();

		// Apply flags to lines that moved, to differentiate from appeared/disappeared lines
		FlagMovedLines(m_diffWrapper.GetMoved0(), m_ptBuf[0]);
		FlagMovedLines(m_diffWrapper.GetMoved1(), m_ptBuf[1]);
		
		// After PrimeTextBuffers() we know amount of real diffs
		// (m_nDiffs) and trivial diffs (m_nTrivialDiffs)

		// Identical files are also updated
		if (!m_diffList.HasSignificantDiffs())
			bIdentical = TRUE;

		// just apply some options to the views
		m_pView[MERGE_VIEW_LEFT]->PrimeListWithFile();
		m_pView[MERGE_VIEW_RIGHT]->PrimeListWithFile();
		m_pDetailView[0]->PrimeListWithFile();
		m_pDetailView[1]->PrimeListWithFile();

		// Now buffers data are valid
		m_pView[MERGE_VIEW_LEFT]->ReAttachToBuffer();
		m_pView[MERGE_VIEW_RIGHT]->ReAttachToBuffer();
		m_pDetailView[0]->ReAttachToBuffer();
		m_pDetailView[1]->ReAttachToBuffer();

		m_bEditAfterRescan[0] = FALSE;
		m_bEditAfterRescan[1] = FALSE;
	}

	theApp.SetLastCompareResult(m_diffList.GetSignificantDiffs());

	m_pRescanFileInfo[0]->Update(m_filePaths.GetLeft());
	m_pRescanFileInfo[1]->Update(m_filePaths.GetRight());

	if (bLeftFileChanged)
	{
		CString msg;
		AfxFormatString1(msg, IDS_FILECHANGED_RESCAN, m_filePaths.GetLeft());
		AfxMessageBox(msg, MB_OK | MB_ICONWARNING);
	}
	else if (bRightFileChanged)
	{
		CString msg;
		AfxFormatString1(msg, IDS_FILECHANGED_RESCAN, m_filePaths.GetRight());
		AfxMessageBox(msg, MB_OK | MB_ICONWARNING);
	}
	return nResult;
}

/** @brief Adjust all different lines that were detected as actually matching moved lines */
void CMergeDoc::FlagMovedLines(const CMap<int, int, int, int> * movedLines, CDiffTextBuffer * pBuffer)
{
	int i;
	for (i=0; i<pBuffer->GetLineCount(); ++i)
	{
		int j=-1;
		if (movedLines->Lookup(i, j))
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = pBuffer->ComputeApparentLine(i);
			if (pBuffer->FlagIsSet(apparent, LF_DIFF))
			{
				pBuffer->SetLineFlag(apparent, LF_MOVED, TRUE, FALSE, FALSE);
			}
		}
	}
	// todo: Need to record actual moved information
}

/**
 * @brief Prints (error) message by rescan status.
 *
 * @param nRescanResult [in] Resultcocode from rescan().
 * @param bBinary [in] Were files binaries?.
 * @param bIdentical [in] Were files identical?.
 * @sa CMergeDoc::Rescan()
 */
void CMergeDoc::ShowRescanError(int nRescanResult,
	BOOL bBinary, BOOL bIdentical)
{
	// Rescan was suppressed, there is no sensible status
	if (nRescanResult == RESCAN_SUPPRESSED)
		return;

	CString s;

	if (nRescanResult == RESCAN_FILE_ERR)
	{
		VERIFY(s.LoadString(IDS_FILEERROR));
		LogErrorString(s);
		AfxMessageBox(s, MB_ICONSTOP);
		return;
	}

	if (nRescanResult == RESCAN_TEMP_ERR)
	{
		VERIFY(s.LoadString(IDS_TEMP_FILEERROR));
		LogErrorString(s);
		AfxMessageBox(s, MB_ICONSTOP);
		return;
	}

	// Binary files tried to load, this can happen when giving filenames
	// from commandline
	if (bBinary)
	{
		VERIFY(s.LoadString(IDS_FILEBINARY));
		AfxMessageBox(s, MB_ICONINFORMATION);
		return;
	}

	// Files are not binaries, but they are identical
	if (bIdentical)
	{
		if (!m_filePaths.GetLeft().IsEmpty() && !m_filePaths.GetRight().IsEmpty() && 
			m_filePaths.GetLeft() == m_filePaths.GetRight())
		{
			// compare file to itself, a custom message so user may hide the message in this case only
			VERIFY(s.LoadString(IDS_FILE_TO_ITSELF));
			AfxMessageBox(s, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN, IDS_FILE_TO_ITSELF);
		}
		else
		{
			UINT nFlags = MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN;

			// Show the "files are identical" error message even if the user
			// requested not to show it again. It is better than to close
			// the application without a warning.
			if (GetMainFrame()->m_bExitIfNoDiff)
			{
				nFlags &= ~MB_DONT_DISPLAY_AGAIN;
			}

			AfxMessageBox(IDS_FILESSAME, nFlags);

			// Exit application if files are identical.
			if (GetMainFrame()->m_bExitIfNoDiff)
			{
				GetMainFrame()->PostMessage(WM_COMMAND, ID_APP_EXIT);
			}
		}
	}
}

CMergeDoc::CDiffTextBuffer::CDiffTextBuffer(CMergeDoc * pDoc, int pane)
{
	m_pOwnerDoc = pDoc;
	m_nThisPane = pane;
	unpackerSubcode = 0;
	m_unicoding = 0;
	m_codepage = 0;
}

BOOL CMergeDoc::CDiffTextBuffer::GetLine(int nLineIndex, CString &strLine)
{ 
	int nLineLength = CCrystalTextBuffer::GetLineLength 
		( nLineIndex ); 
	
	if( nLineLength < 0 ) 
		return FALSE; 
	else if( nLineLength == 0 ) 
		strLine.Empty(); 
	else 
	{ 
		_tcsncpy ( strLine.GetBuffer( nLineLength + 1 ), 
			CCrystalTextBuffer::GetLineChars( nLineIndex ), 
			nLineLength ); 
		strLine.ReleaseBuffer( nLineLength ); 
	} 
	return TRUE; 
}

void CMergeDoc::CDiffTextBuffer::SetModified(BOOL bModified /*= TRUE*/)
{
	CCrystalTextBuffer::SetModified (bModified);
	m_pOwnerDoc->SetModifiedFlag (bModified);
}

BOOL CMergeDoc::CDiffTextBuffer::GetFullLine(int nLineIndex, CString &strLine)
{
	if (!GetFullLineLength(nLineIndex))
		return FALSE;
	strLine = GetLineChars(nLineIndex);
	return TRUE;
}

void CMergeDoc::CDiffTextBuffer::AddUndoRecord(BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos, LPCTSTR pszText, int nLinesToValidate, int nActionType /*= CE_ACTION_UNKNOWN*/, CDWordArray *paSavedRevisonNumbers)
{
	CGhostTextBuffer::AddUndoRecord(bInsert, ptStartPos, ptEndPos, pszText, nLinesToValidate, nActionType, paSavedRevisonNumbers);
	if (m_aUndoBuf[m_nUndoPosition - 1].m_dwFlags & UNDO_BEGINGROUP)
	{
		m_pOwnerDoc->undoTgt.erase(m_pOwnerDoc->curUndo, m_pOwnerDoc->undoTgt.end());
		m_pOwnerDoc->undoTgt.push_back(m_pOwnerDoc->m_pView[m_nThisPane]);
		m_pOwnerDoc->curUndo = m_pOwnerDoc->undoTgt.end();
	}
}

BOOL CMergeDoc::Undo()
{
	return FALSE;
}

/**
 * @brief An instance of RescanSuppress prevents rescan during its lifetime
 * (or until its Clear method is called, which ends its effect).
 */
class RescanSuppress
{
public:
	RescanSuppress(CMergeDoc & doc) : m_doc(doc)
	{
		m_bSuppress = TRUE;
		m_bPrev = doc.m_bEnableRescan;
		doc.m_bEnableRescan = FALSE;
	}
	void Clear() 
	{
		if (m_bSuppress)
		{
			m_bSuppress = FALSE;
			m_doc.m_bEnableRescan = m_bPrev;
		}
	}
	~RescanSuppress()
	{
		Clear();
	}
private:
	CMergeDoc & m_doc;
	BOOL m_bPrev;
	BOOL m_bSuppress;
};

/**
 * @brief Copy all diffs from one side to side.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 */
void CMergeDoc::CopyAllList(int srcPane, int dstPane)
{
	CopyMultipleList(srcPane, dstPane, 0, m_diffList.GetSize() - 1);
}

/**
 * @brief Copy range of diffs from one side to side.
 * This function copies given range of differences from side to another.
 * Ignored differences are skipped, and not copied.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 * @param [in] firstDiff First diff copied (0-based index)
 * @param [in] lastDiff Last diff copied (0-based index)
 */
void CMergeDoc::CopyMultipleList(int srcPane, int dstPane, int firstDiff, int lastDiff)
{
#ifdef _DEBUG
	if (firstDiff > lastDiff)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff > lastDiff)!");
	if (firstDiff < 0)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff < 0)!");
	if (lastDiff > m_diffList.GetSize() - 1)
		_RPTF0(_CRT_ERROR, "Invalid diff range (lastDiff < diffcount)!");
#endif

	lastDiff = min(m_diffList.GetSize() - 1, lastDiff);
	firstDiff = max(0, firstDiff);
	if (firstDiff > lastDiff)
		return;
	
	RescanSuppress suppressRescan(*this);

	// Note we don't care about m_nDiffs count to become zero,
	// because we don't rescan() so it does not change

	SetCurrentDiff(lastDiff);
	bool bGroupWithPrevious = false;
	if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious))
		return; // sync failure

	// copy from bottom up is more efficient
	for (int i = lastDiff - 1; i >= firstDiff; --i)
	{
		if (m_diffList.IsDiffSignificant(i))
		{
			SetCurrentDiff(i);
			// Group merge with previous (merge undo data to one action)
			bGroupWithPrevious = true;
			if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious))
				return; // sync failure
		}
	}

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
}

/**
 * @brief Sanity check difference.
 *
 * Checks that lines in difference are inside difference in both files.
 * If file is edited, lines added or removed diff lines get out of sync and
 * merging fails miserably.
 *
 * @param [in] dr Difference to check.
 * @return TRUE if difference lines match, FALSE otherwise.
 */
BOOL CMergeDoc::SanityCheckDiff(DIFFRANGE dr)
{
	int cd_dbegin = dr.dbegin0;
	int cd_dend = dr.dend0;

	// Must ensure line number is in range before getting line flags
	if (cd_dend >= m_ptBuf[0]->GetLineCount())
		return FALSE;
	DWORD dwLeftFlags = m_ptBuf[0]->GetLineFlags(cd_dend);

	// Must ensure line number is in range before getting line flags
	if (cd_dend >= m_ptBuf[1]->GetLineCount())
		return FALSE;
	DWORD dwRightFlags = m_ptBuf[1]->GetLineFlags(cd_dend);

	// Optimization - check last line first so we don't need to
	// check whole diff for obvious cases
	if (!(dwLeftFlags & LF_WINMERGE_FLAGS) ||
		!(dwRightFlags & LF_WINMERGE_FLAGS))
	{
		return FALSE;
	}

	for (int line = cd_dbegin; line < cd_dend; line++)
	{
		dwLeftFlags = m_ptBuf[0]->GetLineFlags(cd_dend);
		dwRightFlags = m_ptBuf[1]->GetLineFlags(cd_dend);
		if (!(dwLeftFlags & LF_WINMERGE_FLAGS) ||
			!(dwRightFlags & LF_WINMERGE_FLAGS))
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * @brief Copy selected (=current) difference from from side to side.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 * @param [in] nDiff Diff to copy, if -1 function determines it.
 * @param [in] bGroupWithPrevious Adds diff to same undo group with
 * @return true if ok, false if sync failure & need to abort copy
 * previous action (allows one undo for copy all)
 */
bool CMergeDoc::ListCopy(int srcPane, int dstPane, int nDiff /* = -1*/,
		bool bGroupWithPrevious /*= false*/)
{
	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	// If diff-number not given, determine it from active view
	if (nDiff == -1)
	{
		nDiff = GetCurrentDiff();

		// No current diff, but maybe cursor is in diff?
		if (nDiff == -1 && (m_pView[srcPane]->IsCursorInDiff() ||
			m_pView[dstPane]->IsCursorInDiff()))
		{
			// Find out diff under cursor
			CPoint ptCursor;
			int nActiveViewIndexType = GetActiveMergeViewIndexType();
			if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
				ptCursor = m_pView[nActiveViewIndexType]->GetCursorPos();
			else if (nActiveViewIndexType == MERGEVIEW_LEFT_DETAIL ||
					nActiveViewIndexType == MERGEVIEW_RIGHT_DETAIL)
			{
				ptCursor = m_pView[nActiveViewIndexType - MERGEVIEW_LEFT_DETAIL]->GetCursorPos();
			}
			nDiff = m_diffList.LineToDiff(ptCursor.y);
		}
	}

	if (nDiff != -1)
	{
		DIFFRANGE cd;
		VERIFY(m_diffList.GetDiff(nDiff, cd));
		CDiffTextBuffer& sbuf = *m_ptBuf[srcPane];
		CDiffTextBuffer& dbuf = *m_ptBuf[dstPane];
		BOOL bSrcWasMod = sbuf.IsModified();
		int cd_dbegin = srcPane == 0 ? cd.dbegin0 : cd.dbegin1;
		int cd_dend = srcPane == 0 ? cd.dend0 : cd.dend1;
		int cd_blank = srcPane == 0 ? cd.blank0 : cd.blank1;
		BOOL bInSync = SanityCheckDiff(cd);

		if (bInSync == FALSE)
		{
			AfxMessageBox(IDS_VIEWS_OUTOFSYNC, MB_ICONSTOP);
			return false; // abort copying
		}

		// If we remove whole diff from current view, we must fix cursor
		// position first. Normally we would move to end of previous line,
		// but we want to move to begin of that line for usability.
		if ((cd.op == OP_LEFTONLY && dstPane == 0) ||
			(cd.op == OP_RIGHTONLY && dstPane == 1))
		{
			CCrystalTextView * pCurView = m_pView[dstPane];
			CPoint currentPos = pCurView->GetCursorPos();
			currentPos.x = 0;
			if (currentPos.y > 0)
				currentPos.y--;
			pCurView->SetCursorPos(currentPos);
		}

		// if the current diff contains missing lines, remove them from both sides
		int limit = cd_dend;

		// curView is the view which is changed, so the opposite of the source view
		CCrystalTextView* dstView = m_pView[dstPane];

		dbuf.BeginUndoGroup(bGroupWithPrevious);
		if (cd_blank>=0)
		{
			// text was missing, so delete rest of lines on both sides
			// delete only on destination side since rescan will clear the other side
			if(cd_blank==0)
			{
				dbuf.DeleteText(dstView, cd_blank, 0, cd_dend+1, 0, CE_ACTION_MERGE);
			}
			else
			{
				// If you are interesting in this code, please see at URL:
				// https://sourceforge.net/tracker/index.php?func=detail&aid=1555664&group_id=13216&atid=113216
				// and
				// https://sourceforge.net/tracker/index.php?func=detail&aid=1570173&group_id=13216&atid=113216
				dbuf.DeleteText(dstView, cd_blank-1, dbuf.GetLineLength(cd_blank-1), cd_dend, dbuf.GetLineLength(cd_dend), CE_ACTION_MERGE);
			}

			limit=cd_blank-1;
			dbuf.FlushUndoGroup(dstView);
			dbuf.BeginUndoGroup(TRUE);
		}

		CString strLine;

		// copy the selected text over
		for (int i=cd_dbegin; i <= limit; i++)
		{
			// text exists on left side, so just replace
			strLine = _T("");
			sbuf.GetFullLine(i, strLine);
			dbuf.ReplaceFullLine(dstView, i, strLine, CE_ACTION_MERGE);
			dbuf.FlushUndoGroup(dstView);
			dbuf.BeginUndoGroup(TRUE);
		}
		dbuf.FlushUndoGroup(dstView);

		// remove the diff
		SetCurrentDiff(-1);

		// reset the mod status of the source view because we do make some
		// changes, but none that concern the source text
		sbuf.SetModified(bSrcWasMod);
	}

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
	return true;
}

/**
 * @brief Save file with new filename.
 *
 * This function is called by CMergeDoc::DoSave() or CMergeDoc::DoSAveAs()
 * to save file with new filename. CMergeDoc::DoSave() calls if saving with
 * normal filename fails, to let user choose another filename/location.
 * Also, if file is unnamed file (e.g. scratchpad) then it must be saved
 * using this function.
 * @param [in, out] strPath 
 * - [in] : Initial path shown to user
 * - [out] : Path to new filename if saving succeeds
 * @param [in, out] nSaveResult 
 * - [in] : Statuscode telling why we ended up here. Maybe the result of
 * previous save.
 * - [out] : Statuscode of this saving try
 * @param [in, out] sError Error string from lower level saving code
 * @param [in] nBuffer Buffer we are saving
 * @return FALSE as long as the user is not satisfied. Calling function
 * should not continue until TRUE is returned.
 * @sa CMergeDoc::DoSave()
 * @sa CMergeDoc::DoSaveAs()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
BOOL CMergeDoc::TrySaveAs(CString &strPath, int &nSaveResult, CString & sError,
	int nBuffer, PackingInfo * pInfoTempUnpacker)
{
	CString s;
	CString strSavePath; // New path for next saving try
	CString title;
	BOOL result = TRUE;
	int answer = IDOK; // Set default we use for scratchpads
	int nActiveViewIndexType = GetActiveMergeViewIndexType();
	HWND parent = m_pView[nActiveViewIndexType]->GetSafeHwnd();

	// We shouldn't get here if saving is succeed before
	ASSERT(nSaveResult != SAVE_DONE);

	// Select message based on reason function called
	if (nSaveResult == SAVE_PACK_FAILED)
	{
		AfxFormatString2(s, IDS_FILEPACK_FAILED_LEFT + nBuffer,
			strPath, pInfoTempUnpacker->pluginName);
		// replace the unpacker with a "do nothing" unpacker
		pInfoTempUnpacker->Initialize(PLUGIN_MANUAL);
	}
	else
	{
		AfxFormatString2(s, IDS_FILESAVE_FAILED, strPath, sError);
	}

	// SAVE_NO_FILENAME is temporarily used for scratchpad.
	// So don't ask about saving in that case.
	if (nSaveResult != SAVE_NO_FILENAME)
		answer = AfxMessageBox(s, MB_OKCANCEL | MB_ICONWARNING);

	switch (answer)
	{
	case IDOK:
		if (nBuffer == 0)
			VERIFY(title.LoadString(IDS_SAVE_LEFT_AS));
		else
			VERIFY(title.LoadString(IDS_SAVE_RIGHT_AS));

		if (SelectFile(parent, s, strPath, title, NULL, FALSE))
		{
			CDiffTextBuffer *pBuffer = m_ptBuf[nBuffer];
			strSavePath = s;
			nSaveResult = pBuffer->SaveToFile(strSavePath, FALSE, sError,
				pInfoTempUnpacker);

			if (nSaveResult == SAVE_DONE)
			{
				// We are saving scratchpad (unnamed file)
				if (strPath.IsEmpty())
				{
					m_nBufferType[nBuffer] = BUFFER_UNNAMED_SAVED;
					m_strDesc[nBuffer].Empty();
				}
					
				strPath = strSavePath;
				UpdateHeaderPath(nBuffer);
			}
			else
				result = FALSE;
		}
		else
			nSaveResult = SAVE_CANCELLED;
		break;

	case IDCANCEL:
		nSaveResult = SAVE_CANCELLED;
		break;
	}
	return result;
}

/**
 * @brief Save file creating backups etc.
 *
 * Safe top-level file saving function. Checks validity of given path.
 * Creates backup file if wanted to. And if saving to given path fails,
 * allows user to select new location/name for file.
 * @param [in] szPath Path where to save including filename. Can be
 * empty/NULL if new file is created (scratchpad) without filename.
 * @param [out] bSaveSuccess Will contain information about save success with
 * the original name (to determine if file statuses should be changed)
 * @param [in] nBuffer Index (0-based) of buffer to save
 * @return Tells if caller can continue (no errors happened)
 * @note Return value does not tell if SAVING succeeded. Caller must
 * Check value of bSaveSuccess parameter after calling this function!
 * @note If CMainFrame::m_strSaveAsPath is non-empty, file is saved
 * to directory it points to. If m_strSaveAsPath contains filename,
 * that filename is used.
 * @sa CMergeDoc::TrySaveAs()
 * @sa CMainFrame::CheckSavePath()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
BOOL CMergeDoc::DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, int nBuffer)
{
	DiffFileInfo fileInfo;
	CString strSavePath(szPath);
	CString sError;
	BOOL bFileChanged = FALSE;
	BOOL bApplyToAll = FALSE;	
	int nRetVal = -1;

	bFileChanged = IsFileChangedOnDisk(szPath, fileInfo, TRUE, nBuffer);
	if (bFileChanged)
	{
		CString msg;
		AfxFormatString1(msg, IDS_FILECHANGED_ONDISK, szPath);
		if (AfxMessageBox(msg, MB_ICONWARNING | MB_YESNO) == IDNO)
		{
			bSaveSuccess = SAVE_CANCELLED;
			return TRUE;
		}		
	}

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = FALSE;
	
	// Check third arg possibly given from command-line
	if (!GetMainFrame()->m_strSaveAsPath.IsEmpty())
	{
		if (paths_DoesPathExist(GetMainFrame()->m_strSaveAsPath) == IS_EXISTING_DIR)
		{
			// third arg was a directory, so get append the filename
			CString sname;
			SplitFilename(szPath, 0, &sname, 0);
			strSavePath = GetMainFrame()->m_strSaveAsPath;
			if (GetMainFrame()->m_strSaveAsPath.Right(1) != _T('\\'))
				strSavePath += _T('\\');
			strSavePath += sname;
		}
		else
			strSavePath = GetMainFrame()->m_strSaveAsPath;	
	}

	nRetVal = GetMainFrame()->HandleReadonlySave(strSavePath, FALSE, bApplyToAll);
	if (nRetVal == IDCANCEL)
		return FALSE;

	if (!GetMainFrame()->CreateBackup(strSavePath))
		return FALSE;

	// FALSE as long as the user is not satisfied
	// TRUE if saving succeeds, even with another filename, or if the user cancels
	BOOL result = FALSE;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;
	CDiffTextBuffer *pBuffer = m_ptBuf[nBuffer];

	// Assume empty filename means Scratchpad (unnamed file)
	// Todo: This is not needed? - buffer type check should be enough
	if (strSavePath.IsEmpty())
		nSaveErrorCode = SAVE_NO_FILENAME;

	// Handle unnamed buffers
	if ((m_nBufferType[nBuffer] == BUFFER_UNNAMED) ||
		(m_nBufferType[nBuffer] == BUFFER_UNNAMED))
			nSaveErrorCode = SAVE_NO_FILENAME;

	if (nSaveErrorCode == SAVE_DONE)
		// We have a filename, just try to save
		nSaveErrorCode = pBuffer->SaveToFile(strSavePath, FALSE, sError, &infoTempUnpacker);

	if (nSaveErrorCode != SAVE_DONE)
	{
		// Saving failed, user may save to another location if wants to
		do
			result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, &infoTempUnpacker);
		while (!result);
	}

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		m_ptBuf[nBuffer]->SetModified(FALSE);
		m_pSaveFileInfo[nBuffer]->Update(strSavePath);
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths.GetPath(nBuffer));
		m_filePaths.SetPath(nBuffer, strSavePath);
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = TRUE;
		result = TRUE;
	}
	else if (nSaveErrorCode == SAVE_CANCELLED)
	{
		// User cancelled current operation, lets do what user wanted to do
		result = FALSE;
	}
	return result;
}

/**
 * @brief Save file with different filename.
 *
 * Safe top-level file saving function. Asks user to select filename
 * and path. Does not create backups.
 * @param [in] szPath Path where to save including filename. Can be
 * empty/NULL if new file is created (scratchpad) without filename.
 * @param [out] bSaveSuccess Will contain information about save success with
 * the original name (to determine if file statuses should be changed)
 * @param [in] nBuffer Index (0-based) of buffer to save
 * @return Tells if caller can continue (no errors happened)
 * @note Return value does not tell if SAVING succeeded. Caller must
 * Check value of bSaveSuccess parameter after calling this function!
 * @sa CMergeDoc::TrySaveAs()
 * @sa CMainFrame::CheckSavePath()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
BOOL CMergeDoc::DoSaveAs(LPCTSTR szPath, BOOL &bSaveSuccess, int nBuffer)
{
	CString strSavePath(szPath);
	CString sError;

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = FALSE;
	// FALSE as long as the user is not satisfied
	// TRUE if saving succeeds, even with another filename, or if the user cancels
	BOOL result = FALSE;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;

	// Use SAVE_NO_FILENAME to prevent asking about error
	nSaveErrorCode = SAVE_NO_FILENAME;

	// Loop until user succeeds saving or cancels
	do
		result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, &infoTempUnpacker);
	while (!result);

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		m_pSaveFileInfo[nBuffer]->Update(strSavePath);
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths.GetPath(nBuffer));
		m_filePaths.SetPath(nBuffer, strSavePath);
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = TRUE;
		result = TRUE;
	}
	return result;
}

/**
 * @brief Get left->right info for a moved line (apparent line number)
 */
int CMergeDoc::RightLineInMovedBlock(int apparentLeftLine)
{
	if (!(m_ptBuf[0]->GetLineFlags(apparentLeftLine) & LF_MOVED))
		return -1;

	int realLeftLine = m_ptBuf[0]->ComputeRealLine(apparentLeftLine);
	int realRightLine = m_diffWrapper.RightLineInMovedBlock(realLeftLine);
	if (realRightLine != -1)
		return m_ptBuf[1]->ComputeApparentLine(realRightLine);
	else
		return -1;
}

/**
 * @brief Get right->left info for a moved line (apparent line number)
 */
int CMergeDoc::LeftLineInMovedBlock(int apparentRightLine)
{
	if (!(m_ptBuf[1]->GetLineFlags(apparentRightLine) & LF_MOVED))
		return -1;

	int realRightLine = m_ptBuf[1]->ComputeRealLine(apparentRightLine);
	int realLeftLine = m_diffWrapper.LeftLineInMovedBlock(realRightLine);
	if (realLeftLine != -1)
		return m_ptBuf[0]->ComputeApparentLine(realLeftLine);
	else
		return -1;
}

/**
 * @brief Save modified documents.
 * This function asks if user wants to save modified documents. We also
 * allow user to cancel the closing.
 *
 * There is a special trick avoiding showing two save-dialogs, as MFC framework
 * sometimes calls this function twice. We use static counter for these calls
 * and if we already have saving in progress (counter == 1) we skip the new
 * saving dialog.
 *
 * @return TRUE if docs are closed, FALSE if closing is cancelled.
 */
BOOL CMergeDoc::SaveModified()
{
	static int counter;
	++counter;
	if (counter > 1)
		return FALSE;

	if (PromptAndSaveIfNeeded(TRUE))
	{
		counter = 0;
		return TRUE;
	}
	else
	{
		counter = 0;
		return FALSE;
	}
}

/**
 * @brief Sets the current difference.
 * @param [in] nDiff Difference to set as current difference.
 */
void CMergeDoc::SetCurrentDiff(int nDiff)
{
	if (nDiff >= 0 && nDiff <= m_diffList.LastSignificantDiff())
		m_nCurDiff = nDiff;
	else
		m_nCurDiff = -1;
}

/**
 * @brief Checks if a flag is set for line.
 * @param [in] line Index (0-based) for line.
 * @param [in] flag Flag to check.
 * @return TRUE if flag is set, FALSE otherwise.
 */
BOOL CMergeDoc::CDiffTextBuffer::FlagIsSet(UINT line, DWORD flag)
{
	return ((m_aLines[line].m_dwFlags & flag) == flag);
}


/**
Remove blank lines and clear winmerge flags
(2003-06-21, Perry: I don't understand why this is necessary, but if this isn't 
done, more and more gray lines appear in the file)
(2003-07-31, Laoran I don't understand either why it is necessary, but it works
fine, so let's go on with it)
*/
void CMergeDoc::CDiffTextBuffer::prepareForRescan()
{
	RemoveAllGhostLines();
	for(int ct=GetLineCount()-1; ct>=0; --ct)
	{
		SetLineFlag(ct, LF_DIFF, FALSE, FALSE, FALSE);
		SetLineFlag(ct, LF_TRIVIAL, FALSE, FALSE, FALSE);
		SetLineFlag(ct, LF_MOVED, FALSE, FALSE, FALSE);
	}
}


void CMergeDoc::CDiffTextBuffer::OnNotifyLineHasBeenEdited(int nLine)
{
	SetLineFlag(nLine, LF_DIFF, FALSE, FALSE, FALSE);
	SetLineFlag(nLine, LF_TRIVIAL, FALSE, FALSE, FALSE);
	SetLineFlag(nLine, LF_MOVED, FALSE, FALSE, FALSE);
	CGhostTextBuffer::OnNotifyLineHasBeenEdited(nLine);
}



/**
 * @brief Helper to determine the most used CRLF mode in the file
 * Normal call : determine the CRLF mode of the current line
 * Final call  : parameter lpLineBegin = NULL, return the style of the file
 *
 * @note  The lowest CRL_STYLE has the priority in case of equality
 */
int CMergeDoc::CDiffTextBuffer::NoteCRLFStyleFromBuffer(TCHAR *lpLineBegin, DWORD dwLineLen /* =0 */)
{
	static int count[3] = {0};
	int iStyle = -1;

	// give back the result when we ask for it
	if (lpLineBegin == NULL)
	{
		iStyle = 0;
		if (count[1] > count[iStyle])
			iStyle = 1;
		if (count[2] > count[iStyle])
			iStyle = 2;

		// reset the counter for the next file
		count[0] = count[1] = count[2] = 0;

		return iStyle;
	}

	if (dwLineLen >= 1)
	{
		if (lpLineBegin[dwLineLen-1] == _T('\r'))
			iStyle = CRLF_STYLE_MAC;
		if (lpLineBegin[dwLineLen-1] == _T('\n'))
			iStyle = CRLF_STYLE_UNIX;
	}
	if (dwLineLen >= 2)
	{
		if (lpLineBegin[dwLineLen-2] == _T('\r') && lpLineBegin[dwLineLen-1] == _T('\n'))
			iStyle = CRLF_STYLE_DOS;
	}
	ASSERT (iStyle != -1);
	count[iStyle] ++;
	return iStyle;
}

/// Reads one line from filebuffer and inserts to textbuffer
void CMergeDoc::CDiffTextBuffer::ReadLineFromBuffer(TCHAR *lpLineBegin, DWORD dwLineNum, DWORD dwLineLen /* =0 */)
{
	if (m_nSourceEncoding >= 0)
		iconvert (lpLineBegin, m_nSourceEncoding, 1, m_nSourceEncoding == 15);
	AppendLine(dwLineNum, lpLineBegin, dwLineLen);
}

/// Sets path for temporary files
void CMergeDoc::CDiffTextBuffer::SetTempPath(CString path)
{
	m_strTempPath = path;
}

bool CMergeDoc::CDiffTextBuffer::IsInitialized() const
{
	return !!m_bInit;
}

/**
 * @brief Examine statistics in textFileStats and return a crystaltextbuffer enum value for line style
 */
int GetTextFileStyle(const UniMemFile::txtstats & stats)
{
	if (stats.ncrlfs >= stats.nlfs)
	{
		if (stats.ncrlfs >= stats.ncrs)
		{
			return CRLF_STYLE_DOS;
		}
		else
		{
			return CRLF_STYLE_MAC;
		}
	}
	else
	{
		if (stats.nlfs >= stats.ncrs)
		{
			return CRLF_STYLE_UNIX;
		}
		else
		{
			return CRLF_STYLE_MAC;
		}
	}
}
/**
 * @brief Examine statistics in textFileStats and tell if the file has only one EOL type
 */
int IsTextFileStylePure(const UniMemFile::txtstats & stats)
{
	int nType = 0;
	nType += (stats.ncrlfs > 0);
	nType += (stats.ncrs > 0);
	nType += (stats.nlfs > 0);
	return (nType <= 1);
}


/**
 * @brief Return a string giving #lines and #bytes and how much time elapsed.
 * @param [in] lines Count of lines.
 * @param [in] bytes Count of bytes.
 * @param [in] start Time used.
 * @return Formatted string.
 */
static CString GetLineByteTimeReport(UINT lines, __int64 bytes,
	const COleDateTime & start)
{
	CString sLines = locality::NumToLocaleStr((int)lines);
	CString sBytes = locality::NumToLocaleStr(bytes);
	COleDateTimeSpan duration = COleDateTime::GetCurrentTime() - start;
	CString sMinutes = locality::NumToLocaleStr((int)duration.GetTotalMinutes());
	CString str;
	str.Format(_T("%s lines (%s byte) saved in %sm%02ds")
		, sLines, sBytes, sMinutes
		, duration.GetSeconds()
		);
	return str;
}

/**
 * @brief Load file from disk into buffer
 *
 * @param [in] pszFileNameInit File to load
 * @param [in] infoUnpacker Unpacker plugin
 * @param [in] sToFindUnpacker String for finding unpacker plugin
 * @param [out] readOnly Loading was lossy so file should be read-only
 * @param [in] nCrlfStyle EOL style used
 * @param [in] codepage Codepage used
 * @param [out] sError Error message returned
 * @return FRESULT_OK when loading succeed or (list in files.h):
 * - FRESULT_OK_IMPURE : load OK, but the EOL are of different types
 * - FRESULT_ERROR_UNPACK : plugin failed to unpack
 * - FRESULT_ERROR : loading failed, sError contains error message
 * - FRESULT_BINARY : file is binary file
 * @note If this method fails, it calls InitNew so the CDiffTextBuffer is in a valid state
 */
int CMergeDoc::CDiffTextBuffer::LoadFromFile(LPCTSTR pszFileNameInit,
		PackingInfo * infoUnpacker, CString sToFindUnpacker, BOOL & readOnly,
		int nCrlfStyle, int codepage, CString &sError)
{
	ASSERT(!m_bInit);
	ASSERT(m_aLines.GetSize() == 0);

	// Unpacking the file here, save the result in a temporary file
	CString sFileName = pszFileNameInit;
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(sFileName, sToFindUnpacker, infoUnpacker, &unpackerSubcode))
		{
			InitNew(); // leave crystal editor in valid, empty state
			return FileLoadResult::FRESULT_ERROR_UNPACK;
		}
	}
	else
	{
		if (!FileTransform_Unpacking(sFileName, infoUnpacker, &unpackerSubcode))
		{
			InitNew(); // leave crystal editor in valid, empty state
			return FileLoadResult::FRESULT_ERROR_UNPACK;
		}
	}
	// we use the same unpacker for both files, so it must be defined after first file
	ASSERT(infoUnpacker->bToBeScanned == FALSE);
	// we will load the transformed file
	LPCTSTR pszFileName = sFileName;

	CString sExt;
	DWORD nRetVal = FileLoadResult::FRESULT_OK;

	// Set encoding based on extension, if we know one
	SplitFilename(pszFileName, NULL, NULL, &sExt);
	CCrystalTextView::TextDefinition *def = 
			CCrystalTextView::GetTextType(sExt);
	if (def && def->encoding != -1)
		m_nSourceEncoding = def->encoding;
	
	UniMemFile ufile;
	UniFile * pufile = &ufile;

	// Now we only use the UniFile interface
	// which is something we could implement for HTTP and/or FTP files

	if (!pufile->OpenReadOnly(pszFileName))
	{
		nRetVal = FileLoadResult::FRESULT_ERROR;
		UniFile::UniError uniErr = pufile->GetLastUniError();
		if (uniErr.hasError())
		{
			if (uniErr.apiname.IsEmpty())
				sError = uniErr.desc;
			else
				sError = GetSysError(uniErr.syserrnum);
		}
		InitNew(); // leave crystal editor in valid, empty state
		goto LoadFromFileExit;
	}
	else
	{
		// Recognize Unicode files with BOM (byte order mark)
		// or else, use the codepage we were given to interpret the 8-bit characters
		if (!pufile->ReadBom())
			pufile->SetCodepage(codepage);
		UINT lineno = 0;
		CString eol, preveol;
		CString sline;
		bool done = false;
		UINT next_line_report = 100; // for trace messages
		UINT next_line_multiple = 5; // for trace messages
		COleDateTime start = COleDateTime::GetCurrentTime(); // for trace messages

		// Manually grow line array exponentially
		UINT arraysize = 500;
		m_aLines.SetSize(arraysize);
		
		// preveol must be initialized for empty files
		preveol = "\n";
		
		do {
			bool lossy=false;
			done = !pufile->ReadString(sline, eol, &lossy);


			const UniFile::txtstats & tstats = pufile->GetTxtStats();
			if (tstats.nzeros)
			{
				nRetVal = FileLoadResult::FRESULT_BINARY;
				ResetInit(); // leave crystal editor in valid, empty state
				goto LoadFromFileExit;
			}

			// if last line had no eol, we can quit
			if (done && preveol.IsEmpty())
				break;
			// but if last line had eol, we add an extra (empty) line to buffer

			// Manually grow line array exponentially
			if (lineno == arraysize)
			{
				arraysize *= 2;
				m_aLines.SetSize(arraysize);
			}

			sline += eol; // TODO: opportunity for optimization, as CString append is terrible
			if (lossy)
			{
				// TODO: Should record lossy status of line
			}
			AppendLine(lineno, sline, sline.GetLength());
			++lineno;
			preveol = eol;

#ifdef _DEBUG
			// send occasional line counts to trace
			// (at 100, 500, 1000, 5000, etc)
			if (lineno == next_line_report)
			{
				__int64 dwBytesRead = pufile->GetPosition();
				COleDateTimeSpan duration = COleDateTime::GetCurrentTime() - start;
				if (duration.GetTotalMinutes() > 0)
				{
					CString strace = GetLineByteTimeReport(lineno, dwBytesRead, start);
					TRACE(_T("%s\n"), (LPCTSTR)strace);
				}
				next_line_report = next_line_multiple * next_line_report;
				next_line_multiple = (next_line_multiple == 5) ? 2 : 5;
			}
#endif // _DEBUG
		} while (!done);

#ifdef _DEBUG
		// Send report of duration to trace (if it took a while)
		COleDateTime end = COleDateTime::GetCurrentTime();
		COleDateTimeSpan duration = end - start;
		if (duration.GetTotalMinutes() > 0)
		{
			__int64 dwBytesRead = pufile->GetPosition();
			CString strace = GetLineByteTimeReport(lineno, dwBytesRead, start);
			TRACE(_T("%s\n"), (LPCTSTR)strace);
		}
#endif // _DEBUG

		// fix array size (due to our manual exponential growth
		m_aLines.SetSize(lineno);
	
		
		//Try to determine current CRLF mode (most frequent)
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			nCrlfStyle = GetTextFileStyle(pufile->GetTxtStats());
		}
		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
		SetCRLFMode(nCrlfStyle);
		
		//  At least one empty line must present
		// (view does not work for empty buffers)
		ASSERT(m_aLines.GetSize() > 0);
		
		m_bInit = TRUE;
		m_bModified = FALSE;
		m_bUndoGroup = m_bUndoBeginGroup = FALSE;
		m_nUndoBufSize = 1024; // crystaltextbuffer.cpp - UNDO_BUF_SIZE;
		m_nSyncPosition = m_nUndoPosition = 0;
		ASSERT(m_aUndoBuf.GetSize() == 0);
		m_ptLastChange.x = m_ptLastChange.y = -1;
		
		FinishLoading();
		// flags don't need initialization because 0 is the default value

		// Set the return value : OK + info if the file is impure
		// A pure file is a file where EOL are consistent (all DOS, or all UNIX, or all MAC)
		// An impure file is a file with several EOL types
		// WinMerge may display impure files, but the default option is to unify the EOL
		// We return this info to the caller, so it may display a confirmation box
		if (IsTextFileStylePure(pufile->GetTxtStats()))
			nRetVal = FileLoadResult::FRESULT_OK;
		else
			nRetVal = FileLoadResult::FRESULT_OK_IMPURE;

		// stash original encoding away
		m_unicoding = pufile->GetUnicoding();
		m_codepage = pufile->GetCodepage();

		if (pufile->GetTxtStats().nlosses)
		{
			FileLoadResult::AddModifier(nRetVal, FileLoadResult::FRESULT_LOSSY);
			readOnly = TRUE;
		}
	}
	
LoadFromFileExit:
	// close the file now to free the handle
	pufile->Close();

	// delete the file that unpacking may have created
	if (_tcscmp(pszFileNameInit, pszFileName) != 0)
		if (!::DeleteFile(pszFileName))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				pszFileName, GetSysError(GetLastError())));
		}

	return nRetVal;
}

/**
 * @brief Saves file from buffer to disk
 *
 * @param bTempFile : FALSE if we are saving user files and
 * TRUE if we are saving workin-temp-files for diff-engine
 *
 * @return SAVE_DONE or an error code (list in MergeDoc.h)
 */
int CMergeDoc::CDiffTextBuffer::SaveToFile (LPCTSTR pszFileName,
		BOOL bTempFile, CString & sError, PackingInfo * infoUnpacker /*= NULL*/,
		int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/,
		BOOL bClearModifiedFlag /*= TRUE*/ )
{
	ASSERT (nCrlfStyle == CRLF_STYLE_AUTOMATIC || nCrlfStyle == CRLF_STYLE_DOS ||
		nCrlfStyle == CRLF_STYLE_UNIX || nCrlfStyle == CRLF_STYLE_MAC);
	ASSERT (m_bInit);

	if (!pszFileName || _tcslen(pszFileName) == 0)
		return SAVE_FAILED;	// No filename, cannot save...

	if (nCrlfStyle == CRLF_STYLE_AUTOMATIC &&
		!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL))
	{
			// get the default nCrlfStyle of the CDiffTextBuffer
		nCrlfStyle = GetCRLFMode();
		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
	}

	BOOL bOpenSuccess = TRUE;
	BOOL bSaveSuccess = FALSE;

	UniStdioFile file;
	file.SetUnicoding(m_unicoding);
	file.SetCodepage(m_codepage);

	CString sIntermediateFilename; // used when !bTempFile

	if (bTempFile)
	{
		bOpenSuccess = !!file.OpenCreate(pszFileName);
	}
	else
	{
		LPTSTR intermedBuffer = sIntermediateFilename.GetBuffer(_MAX_PATH);
		if (!::GetTempFileName(m_strTempPath, _T("MRG"), 0, intermedBuffer))
			return SAVE_FAILED;  //Nothing to do if even tempfile name fails
		sIntermediateFilename.ReleaseBuffer();
		bOpenSuccess = !!file.OpenCreate(sIntermediateFilename);
	}

	if (!bOpenSuccess)
	{	
		UniFile::UniError uniErr = file.GetLastUniError();
		if (uniErr.hasError())
		{
			if (uniErr.apiname.IsEmpty())
				sError = uniErr.desc;
			else
				sError = GetSysError(uniErr.syserrnum);
			if (bTempFile)
				LogErrorString(Fmt(_T("Opening file %s failed: %s"),
					pszFileName, sError));
			else
				LogErrorString(Fmt(_T("Opening file %s failed: %s"),
					sIntermediateFilename, sError));
		}
		return SAVE_FAILED;
	}

	file.WriteBom();

	// line loop : get each real line and write it in the file
	CString sLine;
	CString sEol = GetStringEol(nCrlfStyle);
	int nLineCount = m_aLines.GetSize();
	for (int line=0; line<nLineCount; ++line)
	{
		if (GetLineFlags(line) & LF_GHOST)
			continue;

		// get the characters of the line (excluding EOL)
		if (GetLineLength(line) > 0)
			GetText(line, 0, line, GetLineLength(line), sLine, 0);
		else
			sLine = _T("");

		// last real line ?
		if (line == ApparentLastRealLine())
		{
			// last real line is never EOL terminated
			ASSERT (_tcslen(GetLineEol(line)) == 0);
			// write the line and exit loop
			file.WriteString(sLine);
			break;
		}

		// normal real line : append an EOL
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			// either the EOL of the line (when preserve original EOL chars is on)
			sLine += GetLineEol(line);
		}
		else
		{
			// or the default EOL for this file
			sLine += sEol;
		}

		// write this line to the file (codeset or unicode conversions are done there)
		file.WriteString(sLine);
	}
	file.Close();


	if (!bTempFile)
	{
		// If we are saving user files
		// we need an unpacker/packer, at least a "do nothing" one
		ASSERT(infoUnpacker != NULL);
		// repack the file here, overwrite the temporary file we did save in
		CString csTempFileName = sIntermediateFilename;
		infoUnpacker->subcode = unpackerSubcode;
		if (!FileTransform_Packing(csTempFileName, *infoUnpacker))
		{
			if (!::DeleteFile(sIntermediateFilename))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename, GetSysError(GetLastError())));
			}
			// returns now, don't overwrite the original file
			return SAVE_PACK_FAILED;
		}
		// the temp filename may have changed during packing
		if (csTempFileName != sIntermediateFilename)
		{
			if (!::DeleteFile(sIntermediateFilename))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename, GetSysError(GetLastError())));
			}
			sIntermediateFilename = csTempFileName;
		}

		// Write tempfile over original file
		if (::CopyFile(sIntermediateFilename, pszFileName, FALSE))
		{
			if (!::DeleteFile(sIntermediateFilename))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename, GetSysError(GetLastError())));
			}
			if (bClearModifiedFlag)
			{
				SetModified(FALSE);
				m_nSyncPosition = m_nUndoPosition;
			}
			bSaveSuccess = TRUE;

			// remember revision number on save
			m_dwRevisionNumberOnSave = m_dwCurrentRevisionNumber;

			// redraw line revision marks
			UpdateViews (NULL, NULL, UPDATE_FLAGSONLY);	
		}
		else
		{
			sError = GetSysError(GetLastError());
			LogErrorString(Fmt(_T("CopyFile(%s, %s) failed: %s"),
				sIntermediateFilename, pszFileName, sError));
		}
	}
	else
	{
		if (bClearModifiedFlag)
		{
			SetModified(FALSE);
			m_nSyncPosition = m_nUndoPosition;
		}
		bSaveSuccess = TRUE;
	}

	if (bSaveSuccess)
		return SAVE_DONE;
	else
		return SAVE_FAILED;
}

/// Replace text of line (no change to eol)
void CMergeDoc::CDiffTextBuffer::ReplaceLine(CCrystalTextView * pSource, int nLine, const CString &strText, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	if (GetLineLength(nLine)>0)
		DeleteText(pSource, nLine, 0, nLine, GetLineLength(nLine), nAction);
	int endl,endc;
	if (! strText.IsEmpty())
		InsertText(pSource, nLine, 0, strText, endl,endc, nAction);
}

/// Return pointer to the eol chars of this string, or pointer to empty string if none
LPCTSTR getEol(const CString &str)
{
	if (str.GetLength()>1 && str[str.GetLength()-2]=='\r' && str[str.GetLength()-1]=='\n')
		return (LPCTSTR)str + str.GetLength()-2;
	if (str.GetLength()>0 && (str[str.GetLength()-1]=='\r' || str[str.GetLength()-1]=='\n'))
		return (LPCTSTR)str + str.GetLength()-1;
	return _T("");
}

/// Replace line (removing any eol, and only including one if in strText)
void CMergeDoc::CDiffTextBuffer::ReplaceFullLine(CCrystalTextView * pSource, int nLine, const CString &strText, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	if (_tcscmp(GetLineEol(nLine), getEol(strText)) == 0)
	{
		// (optimization) eols are the same, so just replace text inside line
		// we must clean strText from its eol...
		int eolLength = _tcslen(getEol(strText));
		CString strTextWithoutEol = strText.Left(strText.GetLength() - eolLength);
		ReplaceLine(pSource, nLine, strTextWithoutEol, nAction);
		return;
	}

	// we may need a last line as the DeleteText end is (x=0,y=line+1)
	if (nLine+1 == GetLineCount())
		InsertGhostLine (pSource, GetLineCount());

	if (GetFullLineLength(nLine))
		DeleteText(pSource, nLine, 0, nLine+1, 0, nAction); 
	int endl,endc;
	if (! strText.IsEmpty())
		InsertText(pSource, nLine, 0, strText, endl,endc, nAction);
}

/**
 * @brief Take care of rescanning document.
 * 
 * Update view and restore cursor and scroll position after
 * rescanning document.
 * @param [in] bForced If TRUE rescan cannot be suppressed
 */
void CMergeDoc::FlushAndRescan(BOOL bForced /* =FALSE */)
{
	// Ignore suppressing when forced rescan
	if (!bForced)
		if (!m_bEnableRescan) return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_RESCANNING));

	int nActiveViewIndexType = GetActiveMergeViewIndexType();

	// store cursors and hide caret
	m_pView[MERGE_VIEW_LEFT]->PushCursors();
	m_pView[MERGE_VIEW_RIGHT]->PushCursors();
	m_pDetailView[0]->PushCursors();
	m_pDetailView[1]->PushCursors();
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		m_pView[nActiveViewIndexType]->HideCursor();

	BOOL bBinary = FALSE;
	BOOL bIdentical = FALSE;
	int nRescanResult = Rescan(bBinary, bIdentical, bForced);

	// restore cursors and caret
	m_pView[MERGE_VIEW_LEFT]->PopCursors();
	m_pView[MERGE_VIEW_RIGHT]->PopCursors();
	m_pDetailView[0]->PopCursors();
	m_pDetailView[1]->PopCursors();
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		m_pView[nActiveViewIndexType]->ShowCursor();

	// because of ghostlines, m_nTopLine may differ just after Rescan
	// scroll both views to the same top line
	CMergeEditView * fixedView = m_pView[MERGE_VIEW_LEFT];
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		// only one view needs to scroll so do not scroll the active view
		fixedView = m_pView[nActiveViewIndexType];
	fixedView->UpdateSiblingScrollPos(FALSE);

	// make sure we see the cursor from the curent view
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		m_pView[nActiveViewIndexType]->EnsureVisible(m_pView[nActiveViewIndexType]->GetCursorPos());

	// scroll both diff views to the same top line
	CMergeDiffDetailView * fixedDetailView = m_pDetailView[0];
	if (nActiveViewIndexType == MERGEVIEW_LEFT_DETAIL || nActiveViewIndexType == MERGEVIEW_RIGHT_DETAIL)
		// only one view needs to scroll so do not scroll the active view
		fixedDetailView = m_pDetailView[nActiveViewIndexType - MERGEVIEW_LEFT_DETAIL];
	fixedDetailView->UpdateSiblingScrollPos(FALSE);

	// Refresh display
	UpdateAllViews(NULL);

	// Show possible error after updating screen
	if (nRescanResult != RESCAN_SUPPRESSED)
		ShowRescanError(nRescanResult, bBinary, bIdentical);
}

/**
 * @brief Saves both files
 */
void CMergeDoc::OnFileSave() 
{
	// We will need to know if either of the originals actually changed
	// so we know whether to update the diff status
	BOOL bLChangedOriginal = FALSE;
	BOOL bRChangedOriginal = FALSE;

	if (m_ptBuf[0]->IsModified() && !m_ptBuf[0]->GetReadOnly())
	{
		// (why we don't use return value of DoSave)
		// DoSave will return TRUE if it wrote to something successfully
		// but we have to know if it overwrote the original file
		BOOL bSaveOriginal = FALSE;
		DoSave(m_filePaths.GetLeft(), bSaveOriginal, 0);
		if (bSaveOriginal)
			bLChangedOriginal = TRUE;
	}

	if (m_ptBuf[1]->IsModified() && !m_ptBuf[1]->GetReadOnly())
	{
		// See comments above for left case
		BOOL bSaveOriginal = FALSE;
		DoSave(m_filePaths.GetRight(), bSaveOriginal, 1);
		if (bSaveOriginal)
			bRChangedOriginal = TRUE;
	}

	// If either of the actual source files being compared was changed
	// we need to update status in the dir view
	if (bLChangedOriginal || bRChangedOriginal)
	{
		// If DirDoc contains diffs
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(FALSE);

			BOOL bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves left-side file
 */
void CMergeDoc::OnFileSaveLeft()
{
	BOOL bLSaveSuccess = FALSE;
	BOOL bLModified = FALSE;

	if (m_ptBuf[0]->IsModified() && !m_ptBuf[0]->GetReadOnly())
	{
		bLModified = TRUE;
		DoSave(m_filePaths.GetLeft(), bLSaveSuccess, 0);
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if (bLModified && bLSaveSuccess)
	{
		// If DirDoc contains compare results
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(FALSE);

			BOOL bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves right-side file
 */
void CMergeDoc::OnFileSaveRight()
{
	BOOL bRSaveSuccess = FALSE;
	BOOL bRModified = FALSE;

	if (m_ptBuf[1]->IsModified() && !m_ptBuf[1]->GetReadOnly())
	{
		bRModified = TRUE;
		DoSave(m_filePaths.GetRight(), bRSaveSuccess, 1);
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if (bRModified && bRSaveSuccess)
	{
		// If DirDoc contains compare results
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(FALSE);

			BOOL bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves left-side file with name asked
 */
void CMergeDoc::OnFileSaveAsLeft()
{
	BOOL bSaveResult = FALSE;
	DoSaveAs(m_filePaths.GetLeft(), bSaveResult, 0);
}

/**
 * @brief Saves right-side file with name asked
 */
void CMergeDoc::OnFileSaveAsRight()
{
	BOOL bSaveResult = FALSE;
	DoSaveAs(m_filePaths.GetRight(), bSaveResult, 1);
}

/**
 * @brief Update diff-number pane text
 */
void CMergeDoc::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	CString sIdx,sCnt,s;
	const int nDiffs = m_diffList.GetSignificantDiffs();
	
	// Files are identical - show text "Identical"
	if (nDiffs <= 0)
		VERIFY(s.LoadString(IDS_IDENTICAL));
	
	// There are differences, but no selected diff
	// - show amount of diffs
	else if (GetCurrentDiff() < 0)
	{
		if (nDiffs == 1)
			VERIFY(s.LoadString(IDS_1_DIFF_FOUND));
		else
		{
			sCnt.Format(_T("%ld"), nDiffs);
			AfxFormatString1(s, IDS_NO_DIFF_SEL_FMT, sCnt);
		}
	}
	
	// There are differences and diff selected
	// - show diff number and amount of diffs
	else
	{
		sIdx.Format(_T("%ld"), GetCurrentDiff()+1);
		sCnt.Format(_T("%ld"), nDiffs);
		AfxFormatString2(s, IDS_DIFF_NUMBER_STATUS_FMT, sIdx, sCnt); 
	}
	pCmdUI->SetText(s);
}

bool CMergeDoc::CDiffTextBuffer::curUndoGroup()
{
	return (m_aUndoBuf.GetSize()!=0 && m_aUndoBuf[0].m_dwFlags&UNDO_BEGINGROUP);
}


/**
 * @brief Build the diff array and prepare buffers accordingly (insert ghost lines, set WinMerge flags)
 *
 * @note Buffers may have different length after PrimeTextBuffers. Indeed, no
 * synchronization is needed after the last line. So no ghost line will be created
 * to face an ignored difference in the last line (typically : 'ignore blank lines' 
 * + empty last line on one side).
 * If you fell that different length buffers are really strange, CHANGE FIRST
 * the last diff to take into account the empty last line.
 */
void CMergeDoc::PrimeTextBuffers()
{
	SetCurrentDiff(-1);
	m_nTrivialDiffs = 0;
	int nDiff;
	int nDiffCount = m_diffList.GetSize();

	// walk the diff list and calculate numbers of extra lines to add
	int LeftExtras = 0;   // extra lines added to left view
	int RightExtras = 0;   // extra lines added to right view
	m_diffList.GetExtraLinesCounts(LeftExtras, RightExtras);

	// resize m_aLines once for each view
	UINT lcount0 = m_ptBuf[0]->GetLineCount();
	UINT lcount1 = m_ptBuf[1]->GetLineCount();
	UINT lcount0new = lcount0 + LeftExtras;
	UINT lcount1new = lcount1 + RightExtras;
// this ASSERT may be false because of empty last line (see function's note)
//	ASSERT(lcount0new == lcount1new);
	m_ptBuf[0]->m_aLines.SetSize(lcount0new);
	m_ptBuf[1]->m_aLines.SetSize(lcount1new);

	// walk the diff list backward, move existing lines to proper place,
	// add ghost lines, and set flags
	for (nDiff = nDiffCount - 1; nDiff >= 0; nDiff --)
	{
		DIFFRANGE curDiff;
		VERIFY(m_diffList.GetDiff(nDiff, curDiff));

		// move matched lines after curDiff
		int nline0 = lcount0 - curDiff.end0 - 1; // #lines on left after current diff
		int nline1 = lcount1 - curDiff.end1 - 1; // #lines on right after current diff
		// Matched lines should really match...
		// But matched lines after last diff may differ because of empty last line (see function's note)
		if (nDiff < nDiffCount - 1)
			ASSERT(nline0 == nline1);
		// Move all lines after current diff down as far as needed
		// for any ghost lines we're about to insert
		m_ptBuf[0]->MoveLine(curDiff.end0+1, lcount0-1, lcount0new-nline0);
		m_ptBuf[1]->MoveLine(curDiff.end1+1, lcount1-1, lcount1new-nline1);
		lcount0new -= nline0;
		lcount1new -= nline1;
		lcount0 -= nline0;
		lcount1 -= nline1;

		// move unmatched lines and add ghost lines
		nline0 = curDiff.end0 - curDiff.begin0 + 1; // #lines in diff on left
		nline1 = curDiff.end1 - curDiff.begin1 + 1; // #lines in diff on right
		int nextra = nline0-nline1;
		int nextraAbs = (nextra >= 0) ? nextra : -nextra;

		if (nextra > 0) 
		{
			// more lines on the left
			m_ptBuf[0]->MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline0);
			m_ptBuf[1]->MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline0);
			m_ptBuf[1]->SetEmptyLine(lcount1new - nextraAbs, nextraAbs);
			for (int i = 1; i <= nextraAbs; i++)
				m_ptBuf[1]->SetLineFlag(lcount1new-i, LF_GHOST, TRUE, FALSE, FALSE);
			lcount0new -= nline0;
			lcount1new -= nline0;
		}
		else if (nextra < 0) 
		{
			// more lines on the right
			m_ptBuf[0]->MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline1);
			m_ptBuf[1]->MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline1);
			m_ptBuf[0]->SetEmptyLine(lcount0new - nextraAbs, nextraAbs);
			for (int i = 1; i <= nextraAbs; i++)
				m_ptBuf[0]->SetLineFlag(lcount0new-i, LF_GHOST, TRUE, FALSE, FALSE);
			lcount0new -= nline1;
			lcount1new -= nline1;
		}
		else 
		{
			// same number of lines
			m_ptBuf[0]->MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline0);
			m_ptBuf[1]->MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline1);
			lcount0new -= nline0;
			lcount1new -= nline1;
		}
		lcount0 -= nline0;
		lcount1 -= nline1;

		// set dbegin, dend, blank, and line flags
		curDiff.dbegin0 = lcount0new;
		curDiff.dbegin1 = lcount1new;

		switch (curDiff.op)
		{
		case OP_LEFTONLY:
			// set curdiff
			{
				// left side
				curDiff.dend0 = lcount0new+nline0-1;
				curDiff.blank0 = -1;
				// right side
				curDiff.dend1 = lcount1new+nline0-1;
				curDiff.blank1 = curDiff.dbegin1;
			}
			// flag lines
			{
				for (UINT i = curDiff.dbegin0 ; i <= curDiff.dend0; i++)
					m_ptBuf[0]->SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				// blanks are already inserted (and flagged) to compensate for diff on other side
			}
			break;
		case OP_RIGHTONLY:
			// set curdiff
			{
				// left side
				curDiff.dend0 = lcount0new+nline1-1;
				curDiff.blank0 = curDiff.dbegin0;
				// right side
				curDiff.dend1 = lcount1new+nline1-1;
				curDiff.blank1 = -1;
			}
			// flag lines
			{
				for (UINT i = curDiff.dbegin1 ; i <= curDiff.dend1 ; i++)
					m_ptBuf[1]->SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				// blanks are already inserted (and flagged) to compensate for diff on other side
			}
			break;
		case OP_TRIVIAL:
			++m_nTrivialDiffs;
			// fall through and handle as diff
		case OP_DIFF:
			// set curdiff
			{
				int nline = max(nline0, nline1);
				// left side
				curDiff.dend0 = lcount0new+nline-1;
				curDiff.blank0 = -1;
				// right side
				curDiff.dend1 = lcount1new+nline-1;
				curDiff.blank1 = -1;
				if (nline0 > nline1)
					// more lines on left, ghost lines on right side
					curDiff.blank1 = curDiff.dend1+1 - nextraAbs;
				else if (nline0 < nline1)
					// more lines on right, ghost lines on left side
					curDiff.blank0 = curDiff.dend0+1 - nextraAbs;
			}
			// flag lines
			{
				// left side
				UINT i=0;
				for (i = curDiff.dbegin0; i <= curDiff.dend0 ; i++)
				{
					if (curDiff.blank0 == -1 || (int)i < curDiff.blank0)
					{
						// set diff or trivial flag
						DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
						m_ptBuf[0]->SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
					}
					else
					{
						// ghost lines are already inserted (and flagged)
						// ghost lines opposite to trivial lines are ghost and trivial
						if (curDiff.op == OP_TRIVIAL)
							m_ptBuf[0]->SetLineFlag(i, LF_TRIVIAL, TRUE, FALSE, FALSE);
					}
				}
				// right side
				for (i = curDiff.dbegin1; i <= curDiff.dend1 ; i++)
				{
					if (curDiff.blank1 == -1 || (int)i < curDiff.blank1)
					{
						// set diff or trivial flag
						DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
						m_ptBuf[1]->SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
					}
					else
					{
						// ghost lines are already inserted (and flagged)
						// ghost lines opposite to trivial lines are ghost and trivial
						if (curDiff.op == OP_TRIVIAL)
							m_ptBuf[1]->SetLineFlag(i, LF_TRIVIAL, TRUE, FALSE, FALSE);
					}
				}
			}
			break;
		}           // switch (curDiff.op)
		VERIFY(m_diffList.SetDiff(nDiff, curDiff));
	}             // for (nDiff = nDiffCount; nDiff-- > 0; )

	m_diffList.ConstructSignificantChain();

	// Used to strip trivial diffs out of the diff chain
	// if m_nTrivialDiffs
	// via copying them all to a new chain, then copying only non-trivials back
	// but now we keep all diffs, including trivial diffs


	m_ptBuf[0]->FinishLoading();
	m_ptBuf[1]->FinishLoading();
}

/**
 * @brief Checks if file has changed since last update (save or rescan).
 * @param [in] szPath File to check
 * @param [in] dfi Previous fileinfo of file
 * @param [in] bSave If TRUE Compare to last save-info, else to rescan-info
 * @param [in] nBuffer Index (0-based) of buffer
 * @return TRUE if file is changed.
 */
BOOL CMergeDoc::IsFileChangedOnDisk(LPCTSTR szPath, DiffFileInfo &dfi,
	BOOL bSave, int nBuffer)
{
	DiffFileInfo *fileInfo = NULL;
	BOOL bFileChanged = FALSE;
	BOOL bIgnoreSmallDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	int tolerance = 0;
	if (bIgnoreSmallDiff)
		tolerance = SmallTimeDiff; // From MainFrm.h

	if (bSave)
		fileInfo = m_pSaveFileInfo[nBuffer];
	else
		fileInfo = m_pRescanFileInfo[nBuffer];

	dfi.Update(szPath);

	int timeDiff = dfi.mtime - fileInfo->mtime;
	timeDiff = abs(timeDiff);
	if (timeDiff > tolerance ||
		dfi.size != fileInfo->size)
	{
		bFileChanged = TRUE;
	}
	return bFileChanged;
}

/**
 * @brief Asks and then saves modified files.
 *
 * This function saves modified files. Dialog is shown for user to select
 * modified file(s) one wants to save or discard changed. Cancelling of
 * save operation is allowed unless denied by parameter. After successfully
 * save operation file statuses are updated to directory compare.
 * @param [in] bAllowCancel If FALSE "Cancel" button is disabled.
 * @return TRUE if user selected "OK" so next operation can be
 * executed. If FALSE user choosed "Cancel".
 * @note If filename is empty, we assume scratchpads are saved,
 * so instead of filename, description is shown.
 * @todo If we have filename and description for file, what should
 * we do after saving to different filename? Empty description?
 */
BOOL CMergeDoc::PromptAndSaveIfNeeded(BOOL bAllowCancel)
{
	const BOOL bLModified = m_ptBuf[0]->IsModified();
	const BOOL bRModified = m_ptBuf[1]->IsModified();
	BOOL result = TRUE;
	BOOL bLSaveSuccess = FALSE;
	BOOL bRSaveSuccess = FALSE;

	if (!bLModified && !bRModified) //Both files unmodified
		return TRUE;

	SaveClosingDlg dlg;
	dlg.DoAskFor(bLModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = TRUE;
	if (!m_filePaths.GetLeft().IsEmpty())
		dlg.m_sLeftFile = m_filePaths.GetLeft();
	else
		dlg.m_sLeftFile = m_strDesc[0];
	if (!m_filePaths.GetRight().IsEmpty())
		dlg.m_sRightFile = m_filePaths.GetRight();
	else
		dlg.m_sRightFile = m_strDesc[1];

	if (dlg.DoModal() == IDOK)
	{
		if (bLModified && dlg.m_leftSave == SAVECLOSING_SAVE)
		{
			if (!DoSave(m_filePaths.GetLeft(), bLSaveSuccess, 0))
				result = FALSE;
		}

		if (bRModified && dlg.m_rightSave == SAVECLOSING_SAVE)
		{
			if (!DoSave(m_filePaths.GetRight(), bRSaveSuccess, 1))
				result = FALSE;
		}
	}
	else
		result = FALSE;

	// If file were modified and saving was successfull,
	// update status on dir view
	if ((bLModified && bLSaveSuccess) ||
		 (bRModified && bRSaveSuccess))
	{
		// If directory compare has results
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(FALSE);

			BOOL bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
	return result;
}

/** Rescan only if we did not Rescan during the last timeOutInSecond seconds*/
void CMergeDoc::RescanIfNeeded(float timeOutInSecond)
{
	// if we did not rescan during the request timeOut, Rescan
	// else we did Rescan after the request, so do nothing
	COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - m_LastRescan;
	if (elapsed.GetTotalSeconds() >= timeOutInSecond)
		// (laoran 08-01-2003) maybe should be FlushAndRescan(TRUE) ??
		FlushAndRescan();
}

/**
 * @brief We have two child views (left & right), so we keep pointers directly
 * at them (the MFC view list doesn't have them both)
 */
void CMergeDoc::SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight)
{
	ASSERT(pLeft && !m_pView[MERGE_VIEW_LEFT]);
	m_pView[MERGE_VIEW_LEFT] = pLeft;
	ASSERT(pRight && !m_pView[MERGE_VIEW_RIGHT]);
	m_pView[MERGE_VIEW_RIGHT] = pRight;
}

/**
 * @brief Someone is giving us pointers to our detail views
 */
void CMergeDoc::SetMergeDetailViews(CMergeDiffDetailView * pLeft, CMergeDiffDetailView * pRight)
{
	ASSERT(pLeft && !m_pDetailView[0]);
	m_pDetailView[0] = pLeft;
	ASSERT(pRight && !m_pDetailView[1]);
	m_pDetailView[1] = pRight;
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CMergeDoc::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc && !m_pDirDoc);
	m_pDirDoc = pDirDoc;
}

/**
 * @brief Return pointer to parent frame
 */
CChildFrame * CMergeDoc::GetParentFrame() 
{
	return dynamic_cast<CChildFrame *>(m_pView[MERGE_VIEW_LEFT]->GetParentFrame()); 
}

/**
 * @brief DirDoc is closing
 */
void CMergeDoc::DirDocClosing(CDirDoc * pDirDoc)
{
	ASSERT(m_pDirDoc == pDirDoc);
	m_pDirDoc = 0;
	// TODO (Perry 2003-03-30): perhaps merge doc should close now ?
}

/**
 * @brief DirDoc commanding us to close
 */
BOOL CMergeDoc::CloseNow()
{
	// Allow user to cancel closing
	if (!PromptAndSaveIfNeeded(TRUE))
		return FALSE;

	GetParentFrame()->CloseNow();
	return TRUE;
}

/**
 * @brief Loads file to buffer and shows load-errors
 * @param [in] sFileName File to open
 * @param [in] nBuffer Index (0-based) of buffer to load
 * @param [out] readOnly whether file is read-only
 * @param [in] codepage relevant 8-bit codepage if any (0 if none or unknown)
 * @return Tells if files were loaded successfully
 * @sa CMergeDoc::OpenDocs()
 **/
int CMergeDoc::LoadFile(CString sFileName, int nBuffer, BOOL & readOnly, int codepage)
{
	CDiffTextBuffer *pBuf;
	CString sError;
	DWORD retVal = FileLoadResult::FRESULT_ERROR;

	pBuf = m_ptBuf[nBuffer];
	nBuffer == 0 ? m_filePaths.SetLeft(sFileName) : m_filePaths.SetRight(sFileName);

	int nCrlfStyle = CRLF_STYLE_AUTOMATIC;
	CString sOpenError;
	retVal = pBuf->LoadFromFile(sFileName, m_pInfoUnpacker,
		m_strBothFilenames, readOnly, nCrlfStyle, codepage, sOpenError);

	// if CMergeDoc::CDiffTextBuffer::LoadFromFile failed,
	// it left the pBuf in a valid (but empty) state via a call to InitNew

	if (FileLoadResult::IsOkImpure(retVal))
	{
		// File loaded, and multiple EOL types in this file
		FileLoadResult::SetMainOk(retVal);
		// By default, WinMerge unifies EOL to the most used type (when diffing or saving)
		// As some info are lost, we request a confirmation from the user
		if (!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL))
		{
			CString s;
			AfxFormatString1(s, IDS_SUGGEST_PRESERVEEOL, sFileName); 
			if (AfxMessageBox(s, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN,
				IDS_SUGGEST_PRESERVEEOL) == IDYES)
			{
				// the user wants to keep the original chars
				GetMainFrame()->SetEOLMixed(TRUE);
				GetOptionsMgr()->SaveOption(OPT_ALLOW_MIXED_EOL, true);
			}
		}
	}

	if (FileLoadResult::IsError(retVal))
	{
		// Error from Unifile/system
		if (!sOpenError.IsEmpty())
			AfxFormatString2(sError, IDS_ERROR_FILEOPEN, sFileName, sOpenError);
		else
			AfxFormatString1(sError, IDS_ERROR_FILE_NOT_FOUND, sFileName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
	}
	else if (FileLoadResult::IsErrorUnpack(retVal))
	{
		AfxFormatString1(sError, IDS_ERROR_FILE_NOT_UNPACKED, sFileName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
	}
	return retVal;
}

/**
 * @brief Is specified codepage number valid for use in WinMerge Merge Editor?
 */
bool CMergeDoc::IsValidCodepageForMergeEditor(unsigned cp) const
{
	if (!cp) // 0 is our signal value for invalid
		return false;
	// Codepage must be actually installed on system
	// for us to be able to use it
	// We accept whatever codepages that codepage module says are installed
	return isCodepageInstalled(cp);
}

/**
 * @brief Loads files and does initial rescan.
 * @param filelocLeft [in] File to open to left side (path & encoding info)
 * @param fileLocRight [in] File to open to right side (path & encoding info)
 * @param bROLeft [in] Is left file read-only
 * @param bRORight [in] Is right file read-only
 * @return Success/Failure/Binary (failure) per typedef enum OpenDocsResult_TYPE
 * @todo Options are still read from CMainFrame, this will change
 * @sa CMainFrame::ShowMergeDoc()
 */
OPENRESULTS_TYPE CMergeDoc::OpenDocs(FileLocation filelocLeft, FileLocation filelocRight,
		BOOL bROLeft, BOOL bRORight)
{
	BOOL bBinary = FALSE;
	BOOL bIdentical = FALSE;
	int nRescanResult = RESCAN_OK;

	// Filter out invalid codepages, or editor will display all blank
	if (filelocLeft.encoding.m_unicoding == ucr::NONE
		&& !IsValidCodepageForMergeEditor(filelocLeft.encoding.m_codepage))
	{
		int cp = getDefaultCodepage();
		if (!IsValidCodepageForMergeEditor(cp))
			cp = CP_ACP;
		filelocLeft.encoding.SetCodepage(cp);
	}
	if (filelocRight.encoding.m_unicoding == ucr::NONE
		&& !IsValidCodepageForMergeEditor(filelocRight.encoding.m_codepage))
	{
		int cp = getDefaultCodepage();
		if (!IsValidCodepageForMergeEditor(cp))
			cp = CP_ACP;
		filelocRight.encoding.SetCodepage(cp);
	}

	// clear undo stack
	undoTgt.clear();
	curUndo = undoTgt.begin();

	// Prevent displaying views during LoadFile
	// Note : attach buffer again only if both loads succeed
	m_pView[MERGE_VIEW_LEFT]->DetachFromBuffer();
	m_pView[MERGE_VIEW_RIGHT]->DetachFromBuffer();
	m_pDetailView[0]->DetachFromBuffer();
	m_pDetailView[1]->DetachFromBuffer();

	// free the buffers
	m_ptBuf[0]->FreeAll();
	m_ptBuf[1]->FreeAll();

	CString sLeftFile = filelocLeft.filepath;
	CString sRightFile = filelocRight.filepath;

	// build the text being filtered, "|" separates files as it is forbidden in filenames
	m_strBothFilenames = sLeftFile + _T("|") + sRightFile;

	// Load left side file
	DWORD nLeftSuccess = FileLoadResult::FRESULT_ERROR;
	if (!sLeftFile.IsEmpty())
	{
		if (GetMainFrame()->m_strLeftDesc.IsEmpty())
			m_nBufferType[0] = BUFFER_NORMAL;
		else
		{
			m_nBufferType[0] = BUFFER_NORMAL_NAMED;
			m_strDesc[0] = GetMainFrame()->m_strLeftDesc;
			GetMainFrame()->m_strLeftDesc.Empty();
		}

		m_pSaveFileInfo[0]->Update(sLeftFile);
		m_pRescanFileInfo[0]->Update(sLeftFile);

		// Load left side file
		nLeftSuccess = LoadFile(sLeftFile, 0, bROLeft, filelocLeft.encoding.m_codepage);
	}
	else
	{
		m_nBufferType[0] = BUFFER_UNNAMED;

		m_ptBuf[0]->InitNew();
		m_strDesc[0] = GetMainFrame()->m_strLeftDesc;
		nLeftSuccess = FileLoadResult::FRESULT_OK;
	}
	
	// Load right side only if left side was successfully loaded
	DWORD nRightSuccess = FileLoadResult::FRESULT_ERROR;
	if (!sRightFile.IsEmpty())
	{
		if (GetMainFrame()->m_strRightDesc.IsEmpty())
			m_nBufferType[1] = BUFFER_NORMAL;
		else
		{
			m_nBufferType[1] = BUFFER_NORMAL_NAMED;
			m_strDesc[1] = GetMainFrame()->m_strRightDesc;
			GetMainFrame()->m_strRightDesc.Empty();
		}

		m_pSaveFileInfo[1]->Update(sRightFile);
		m_pRescanFileInfo[1]->Update(sRightFile);
		if (FileLoadResult::IsOk(nLeftSuccess) || FileLoadResult::IsBinary(nLeftSuccess))
		{
			nRightSuccess = LoadFile(sRightFile, 1, bRORight, filelocRight.encoding.m_codepage);
		}
	}
	else
	{
		m_nBufferType[1] = BUFFER_UNNAMED;

		m_ptBuf[1]->InitNew();
		m_strDesc[1] = GetMainFrame()->m_strRightDesc;
		nRightSuccess = FileLoadResult::FRESULT_OK;
	}

	// scratchpad : we don't call LoadFile, so
	// we need to initialize the unpacker as a "do nothing" one
	if (m_nBufferType[0] == BUFFER_UNNAMED && m_nBufferType[1] == BUFFER_UNNAMED)
		m_pInfoUnpacker->Initialize(PLUGIN_MANUAL);

	// Bail out if either side failed
	if (!FileLoadResult::IsOk(nLeftSuccess) || !FileLoadResult::IsOk(nRightSuccess))
	{
		OPENRESULTS_TYPE retVal = OPENRESULTS_FAILED_MISC;
		if (FileLoadResult::IsBinary(nLeftSuccess) || FileLoadResult::IsBinary(nRightSuccess))
		{
			CompareBinaries(sLeftFile, sRightFile, nLeftSuccess, nRightSuccess);
			retVal = OPENRESULTS_FAILED_BINARY;
		}
		CChildFrame *pFrame = GetParentFrame();
		if (pFrame)
		{
			// Use verify macro to trap possible error in debug.
			VERIFY(pFrame->DestroyWindow());
		}
		return retVal;
	}

	// Warn user if file load was lossy (bad encoding)
	if (FileLoadResult::IsLossy(nLeftSuccess) || FileLoadResult::IsLossy(nRightSuccess))
	{
		// TODO: It would be nice to report how many lines were lossy
		// we did calculate those numbers when we loaded the files, in the text stats

		int idres=0;
		if (FileLoadResult::IsLossy(nLeftSuccess) && FileLoadResult::IsLossy(nRightSuccess))
		{
			idres = IDS_LOSSY_TRANSCODING_BOTH;
		}
		else if (FileLoadResult::IsLossy(nLeftSuccess))
		{
			idres = IDS_LOSSY_TRANSCODING_LEFT;
		}
		else
		{
			idres = IDS_LOSSY_TRANSCODING_RIGHT;
		}
		AfxMessageBox(idres, MB_ICONSTOP);
	}

	// Now buffers data are valid
	m_pView[MERGE_VIEW_LEFT]->AttachToBuffer();
	m_pView[MERGE_VIEW_RIGHT]->AttachToBuffer();
	m_pDetailView[0]->AttachToBuffer();
	m_pDetailView[1]->AttachToBuffer();

	// Currently there is only one set of syntax colors, which all documents & views share
	m_pView[MERGE_VIEW_LEFT]->SetColorContext(GetMainSyntaxColors());
	m_pView[MERGE_VIEW_RIGHT]->SetColorContext(GetMainSyntaxColors());
	m_pDetailView[0]->SetColorContext(GetMainSyntaxColors());
	m_pDetailView[1]->SetColorContext(GetMainSyntaxColors());

	// Set read-only statuses
	m_ptBuf[0]->SetReadOnly(bROLeft);
	m_ptBuf[1]->SetReadOnly(bRORight);

	// Check the EOL sensitivity option (do it before Rescan)
	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	if (m_ptBuf[0]->GetCRLFMode() != m_ptBuf[1]->GetCRLFMode() &&
		!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) && !diffOptions.bIgnoreEol)
	{
		// Options and files not are not compatible :
		// Sensitive to EOL on, allow mixing EOL off, and files have a different EOL style.
		// All lines will differ, that is not very interesting and probably not wanted.
		// Propose to turn off the option 'sensitive to EOL'
		CString s = LoadResString(IDS_SUGGEST_IGNOREEOL);
		if (AfxMessageBox(s, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN, IDS_SUGGEST_IGNOREEOL) == IDYES)
		{
			diffOptions.bIgnoreEol = TRUE;
			m_diffWrapper.SetOptions(&diffOptions);
		}
	}

	// Define the prediffer
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	m_pDirDoc->FetchPluginInfos(m_strBothFilenames, &infoUnpacker, &infoPrediffer);
	m_diffWrapper.SetPrediffer(infoPrediffer);
	m_diffWrapper.SetTextForAutomaticPrediff(m_strBothFilenames);
	
	nRescanResult = Rescan(bBinary, bIdentical);

	// Open filed if rescan succeed and files are not binaries
	if (nRescanResult == RESCAN_OK && bBinary == FALSE)
	{
		// prepare the four views
		CMergeEditView * pLeft = GetLeftView();
		CMergeEditView * pRight = GetRightView();
		CMergeDiffDetailView * pLeftDetail = GetLeftDetailView();
		CMergeDiffDetailView * pRightDetail = GetRightDetailView();
		
		// set the document types
		// Warning : it is the first thing to do (must be done before UpdateView,
		// or any function that calls UpdateView, like SelectDiff)
		// Note: If option enabled, and another side type is not recognized,
		// we use recognized type for unrecognized side too.
		CString sextL = GetFileExt(sLeftFile, m_strDesc[0]);
		BOOL bLeftTyped = pLeft->SetTextType(sextL);
		pLeftDetail->SetTextType(sextL);
		CString sextR = GetFileExt(sRightFile, m_strDesc[1]);
		BOOL bRightTyped = pRight->SetTextType(sextR);
		pRightDetail->SetTextType(sextR);

		// If other side didn't have recognized texttype, apply recognized
		// type to unrecognized one. (comparing file.cpp and file.bak applies
		// cpp file type to .bak file.
		if (bLeftTyped != bRightTyped)
		{
			CCrystalTextView::TextDefinition *enuType;

			if (bLeftTyped)
			{
				enuType = pLeft->GetTextType(sextL);
				pRight->SetTextType(enuType);
				pRightDetail->SetTextType(enuType);
			}
			else
			{
				enuType = pRight->GetTextType(sextR);
				pLeft->SetTextType(enuType);
				pLeftDetail->SetTextType(enuType);
			}
		}

		// set the frame window header
		UpdateHeaderPath(0);
		UpdateHeaderPath(1);

		pLeft->DocumentsLoaded();
		pRight->DocumentsLoaded();
		pLeftDetail->DocumentsLoaded();
		pRightDetail->DocumentsLoaded();

		// Inform user that files are identical
		// Don't show message if new buffers created
		if ((TRUE == bIdentical) &&
			((m_nBufferType[0] == BUFFER_NORMAL) ||
			 (m_nBufferType[0] == BUFFER_NORMAL_NAMED) ||
			 (m_nBufferType[1] == BUFFER_NORMAL) ||
			 (m_nBufferType[1] == BUFFER_NORMAL_NAMED)))
		{
			ShowRescanError(nRescanResult, bBinary, bIdentical);
		}

		// scroll to first diff
		if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST) &&
			m_diffList.HasSignificantDiffs())
		{
			int nDiff = m_diffList.FirstSignificantDiff();
			pLeft->SelectDiff(nDiff, TRUE, FALSE);
		}

		// Exit if files are identical should only work for the first
		// comparison and must be disabled afterward.
		GetMainFrame()->m_bExitIfNoDiff = FALSE;
	}
	else
	{
		// CMergeDoc::Rescan fails if files do not exist on both sides 
		// or the really arcane case that the temp files couldn't be created, 
		// which is too obscure to bother reporting if you can't write to 
		// your temp directory, doing nothing is graceful enough for that).
		ShowRescanError(nRescanResult, bBinary, bIdentical);
		GetParentFrame()->DestroyWindow();
		return (bBinary ? OPENRESULTS_FAILED_BINARY: OPENRESULTS_FAILED_MISC);
	}

	// Force repaint of location pane to update it in case we had some warning
	// dialog visible and it got painted before files were loaded
	if (m_pView[MERGE_VIEW_LEFT])
		m_pView[MERGE_VIEW_LEFT]->RepaintLocationPane();

	return OPENRESULTS_SUCCESS;
}

/**
 * @brief Compare binary files and print results to user.
 *
 * @param sLeftFile [in] Full path to left file
 * @param sRightFile [in] Full path to right file
 * @param nLeftSuccess [in] Returnvalue from file load for leftside
 * @param nRightSuccess [in] Returnvalue from file load for rightside
 * @sa CMergeDoc::OpenDocs()
 * @sa CMergeDoc::Rescan()
 */
void CMergeDoc::CompareBinaries(CString sLeftFile, CString sRightFile, int nLeftSuccess, int nRightSuccess)
{
	int nRescanResult = RESCAN_OK;
	BOOL bBinary = FALSE;
	BOOL bIdentical = FALSE;

	// Compare binary files
	if (FileLoadResult::IsBinary(nLeftSuccess) && FileLoadResult::IsBinary(nRightSuccess))
	{
		bBinary = TRUE; // Compare binary files
		nRescanResult = Rescan(bBinary, bIdentical);
	}

	if (nRescanResult == RESCAN_OK)
	{
		// Format message shown to user: both files are binaries
		if (FileLoadResult::IsBinary(nLeftSuccess) && FileLoadResult::IsBinary(nRightSuccess))
		{
			CString msg;
			CString msg2;
			if (bIdentical)
				AfxFormatString2(msg, IDS_BINFILES_IDENTICAL, sLeftFile, sRightFile);
			else
				AfxFormatString2(msg, IDS_BINFILES_DIFFERENT, sLeftFile, sRightFile);
			msg += _T("\n\n");
			VERIFY(msg2.LoadString(IDS_FILEBINARY));
			msg += msg2;
			AfxMessageBox(msg, MB_ICONINFORMATION);
		}
		else if (FileLoadResult::IsBinary(nLeftSuccess) || FileLoadResult::IsBinary(nRightSuccess))
		{
			// Other file binary, other text
			CString msg;
			CString msg2;
			if (FileLoadResult::IsBinary(nLeftSuccess))
				AfxFormatString1(msg, IDS_OTHER_IS_BINARY, sLeftFile);
			else
				AfxFormatString1(msg, IDS_OTHER_IS_BINARY, sRightFile);

			AfxMessageBox(msg, MB_ICONSTOP);
		}
	}
	else
		ShowRescanError(nRescanResult, bBinary, bIdentical);
}

/**
 * @brief Refresh cached options.
 *
 * For compare speed, we have to cache some frequently needed options,
 * instead of getting option value every time from OptionsMgr. This
 * function must be called every time options are changed to OptionsMgr.
 */
void CMergeDoc::RefreshOptions()
{
	DIFFOPTIONS options = {0};
	
	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);

	m_diffWrapper.SetOptions(&options);

	// Refresh view options
	m_pView[MERGE_VIEW_LEFT]->RefreshOptions();
	m_pView[MERGE_VIEW_RIGHT]->RefreshOptions();
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CMergeDoc::UpdateHeaderPath(int pane)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	CString sText;
	BOOL bChanges = FALSE;

	if (m_nBufferType[pane] == BUFFER_UNNAMED ||
		m_nBufferType[pane] == BUFFER_NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths.GetPath(pane);
		if (m_pDirDoc)
		{
			if (pane == 0)
				m_pDirDoc->ApplyLeftDisplayRoot(sText);
			else
				m_pDirDoc->ApplyRightDisplayRoot(sText);
		}
	}
	bChanges = m_ptBuf[pane]->IsModified();

	if (bChanges)
		sText.Insert(0, _T("* "));

	pf->GetHeaderInterface()->SetText(pane, sText);

	SetTitle(NULL);
}

/**
 * @brief Paint differently the headerbar of the active view
 */
void CMergeDoc::UpdateHeaderActivity(int pane, BOOL bActivate)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	pf->GetHeaderInterface()->SetActive(pane, bActivate);
}

/**
 * @brief Return if doc is in Merging/Editing mode
 */
BOOL CMergeDoc::GetMergingMode() const
{
	return m_bMergingMode;
}

/**
 * @brief Set doc to Merging/Editing mode
 */
void CMergeDoc::SetMergingMode(BOOL bMergingMode)
{
	m_bMergingMode = bMergingMode;
	GetOptionsMgr()->SaveOption(OPT_MERGE_MODE, m_bMergingMode == TRUE);
}

/**
 * @brief Set detect/not detect Moved Blocks
 */
void CMergeDoc::SetDetectMovedBlocks(BOOL bDetectMovedBlocks)
{
	if (bDetectMovedBlocks == m_diffWrapper.GetDetectMovedBlocks())
		return;

	GetOptionsMgr()->SaveOption(OPT_CMP_MOVED_BLOCKS, bDetectMovedBlocks == TRUE);
	m_diffWrapper.SetDetectMovedBlocks(bDetectMovedBlocks);
	FlushAndRescan();
}

void CMergeDoc::SetEditedAfterRescan(int nBuffer)
{
	m_bEditAfterRescan[nBuffer] = TRUE;
}

/**
 * @brief Update document filenames to title
 */
void CMergeDoc::SetTitle(LPCTSTR lpszTitle)
{
	const TCHAR pszSeparator[] = _T(" - ");
	CString sTitle;

	if (lpszTitle)
		sTitle = lpszTitle;
	else
	{
		if (!m_strDesc[0].IsEmpty())
			sTitle += m_strDesc[0];
		else
		{
			CString file;
			CString ext;
			SplitFilename(m_filePaths.GetLeft(), NULL, &file, &ext);
			sTitle += file;
			if (!ext.IsEmpty())
			{
				sTitle += _T(".");
				sTitle += ext;
			}
		}

		sTitle += pszSeparator;

		if (!m_strDesc[1].IsEmpty())
			sTitle += m_strDesc[1];
		else
		{
			CString file;
			CString ext;
			SplitFilename(m_filePaths.GetRight(), NULL, &file, &ext);
			sTitle += file;
			if (!ext.IsEmpty())
			{
				sTitle += _T(".");
				sTitle += ext;
			}
		}
	}
	CDocument::SetTitle(sTitle);
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeDoc::UpdateResources()
{
	CString str;

	VERIFY(str.LoadString(IDS_EMPTY_LEFT_FILE));
	m_strDesc[0] = str;
	VERIFY(str.LoadString(IDS_EMPTY_RIGHT_FILE));
	m_strDesc[1] = str;
	UpdateHeaderPath(0);
	UpdateHeaderPath(1);

	GetParentFrame()->UpdateResources();
	GetLeftView()->UpdateResources();
	GetRightView()->UpdateResources();
}

// Lookup named property and return as int
BOOL CMergeDoc::GetOptionInt(LPCTSTR name) const
{
	// Currently options are held by the main frame, in a subobject called m_options
	return GetOptionsMgr()->GetInt(name);
}

// Lookup named property and return as BOOL
BOOL CMergeDoc::GetOptionBool(LPCTSTR name) const
{
	// Currently options are held by the main frame, in a subobject called m_options
	return GetOptionsMgr()->GetBool(name);
}

// Return current word breaking break type setting (whitespace only or include punctuation)
bool CMergeDoc::GetBreakType() const
{
	return !!GetOptionInt(OPT_BREAK_TYPE);
}

// Return true to do line diff colors at the byte level (false to do them at word level)
bool CMergeDoc::GetByteColoringOption() const
{
	// color at byte level if 'break_on_words' option not set
	return !GetOptionBool(OPT_BREAK_ON_WORDS);
}

/// Swap files and update views
void CMergeDoc::SwapFiles()
{
	// Swap buffers and so on
	swap<CDiffTextBuffer *>(&m_ptBuf[0], &m_ptBuf[1]);
	swap<DiffFileInfo *>(&m_pSaveFileInfo[0], &m_pSaveFileInfo[1]);
	swap<DiffFileInfo *>(&m_pRescanFileInfo[0], &m_pRescanFileInfo[1]);
	swap<BUFFERTYPE>(&m_nBufferType[0], &m_nBufferType[1]);
	swap<BOOL>(&m_bEditAfterRescan[0], &m_bEditAfterRescan[1]);
	swap<CString>(&m_strDesc[0], &m_strDesc[1]);

	CString tmpString = m_filePaths.GetLeft();
	m_filePaths.SetLeft(m_filePaths.GetRight());
	m_filePaths.SetRight(tmpString);
	
	m_diffList.Swap();

	// Reattach text buffers
	m_pView[MERGE_VIEW_LEFT]->ReAttachToBuffer(m_ptBuf[0]);
	m_pView[MERGE_VIEW_RIGHT]->ReAttachToBuffer(m_ptBuf[1]);
	m_pDetailView[0]->ReAttachToBuffer(m_ptBuf[0]);
	m_pDetailView[1]->ReAttachToBuffer(m_ptBuf[1]);

	// Update views
	UpdateHeaderPath(0);
	UpdateHeaderPath(1);

	UpdateAllViews(NULL);
}

/**
 * @brief Display encodings to user
 */
void CMergeDoc::OnFileEncoding()
{
	DoFileEncodingDialog();
}

// Return file extension either from file name or file description (if WinMerge is used as an
// external Rational ClearCase tool.
CString CMergeDoc::GetFileExt(const CString& sFileName, const CString& sDescription)
{
	CString sExt;
	SplitFilename(sFileName, NULL, NULL, &sExt);

	if (TRUE == GetMainFrame()->m_bClearCaseTool)
	{
		// If no extension found in real file name.
		if (TRUE == sExt.IsEmpty())
		{
			SplitViewName(sFileName, NULL, NULL, &sExt);
		}
		// If no extension found in repository file name.
		if (TRUE == sExt.IsEmpty())
		{
			SplitViewName(sDescription, NULL, NULL, &sExt);
		}
	}
	return sExt;
}
