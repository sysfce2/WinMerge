/**
 *  @file DirScan.cpp
 *
 *  @brief Implementation of DirScan (q.v.) and helper functions
 */ 
// ID line follows -- this is updated by SVN
// $Id: DirScan.cpp 5646 2008-07-20 16:22:24Z jtuc $

#include "stdafx.h"
#include "UnicodeString.h"
#include "Merge.h"
#include "LogFile.h"
#include "DirScan.h"
#include "CompareStats.h"
#include "DiffThread.h"
#include "FolderCmp.h"
#include "FileFilterHelper.h"
#include "codepage.h"
#include "IAbortable.h"
#include "FolderCmp.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Static functions (ie, functions only used locally)
void CompareDiffItem(DIFFITEM &di, CDiffContext * pCtxt);
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const FolderCmp * pCmpData);
static void AddToList(const String &sLeftDir, const String &sRightDir, const DirItem * lent, const DirItem * rent,
	UINT code, DiffFuncStruct *myStruct);
static void UpdateDiffItem(DIFFITEM & di, BOOL & bExists, CDiffContext *pCtxt);

/**
 * @brief Help minimize memory footprint by sharing CStringData if possible.
 * 
 * Use OPTIMIZE_SHARE_CSTRINGDATA to conditionally include code that is merely
 * intended to minimize memory footprint by having two CStrings share one
 * CStringData if possible. The rule is that semantics must be identical
 * regardless of whether OPTIMIZE_SHARE_CSTRINGDATA(X) expands to X or to
 * nothing. If you suspect some bug to be related to this kind of optimization,
 * then you can simply change OPTIMIZE_SHARE_CSTRINGDATA to expand to nothing,
 * recompile, and see if bug disappears.
 */
#define OPTIMIZE_SHARE_CSTRINGDATA(X) X

/**
 * @brief Collect file- and directory-names to list.
 * This function walks given folders and adds found subfolders and files into
 * lists. There are two modes, determined by the @p depth:
 * - in non-recursive mode we walk only given folders, and add files.
 *   contained. Subfolders are added as folder items, not walked into.
 * - in recursive mode we walk all subfolders and add the files they
 *   contain into list.
 *
 * Items are tested against file filters in this function.
 * 
 * @param [in] paths Root paths of compare
 * @param [in] leftsubdir Left side subdirectory under root path
 * @param [in] rightsubdir Right side subdirectory under root path
 * @param [in] casesensitive Is filename compare casesensitive?
 * @param [in] depth Levels of subdirectories to scan, -1 scans all
 * @param [in] pCtxt Compare context
 * @return 1 normally, -1 if compare was aborted
 */
int DirScan_GetItems(const PathContext &paths, const String &leftsubdir,
		const String &rightsubdir, DiffFuncStruct *myStruct,
		bool casesensitive, int depth)
{
	static const TCHAR backslash[] = _T("\\");
	CDiffContext *pCtxt = myStruct->context;
	String sLeftDir(paths.GetLeft());
	String sRightDir(paths.GetRight());
	String leftsubprefix;
	String rightsubprefix;
	if (leftsubdir.length())
	{
		sLeftDir = paths_ConcatPath(sLeftDir, leftsubdir);
		sRightDir = paths_ConcatPath(sRightDir, rightsubdir);
		leftsubprefix = leftsubdir + backslash;
		// minimize memory footprint by having left/rightsubprefix share CStringData if possible
		rightsubprefix = OPTIMIZE_SHARE_CSTRINGDATA
		(
			leftsubdir.c_str() == rightsubdir.c_str() ? leftsubprefix : 
		) rightsubdir + backslash;
	}

	DirItemArray leftDirs, leftFiles, rightDirs, rightFiles;
	LoadAndSortFiles(sLeftDir.c_str(), &leftDirs, &leftFiles, casesensitive);
	LoadAndSortFiles(sRightDir.c_str(), &rightDirs, &rightFiles, casesensitive);

	// Allow user to abort scanning
	if (pCtxt->ShouldAbort())
		return -1;

	// Handle directories
	// i points to current directory in left list (leftDirs)
	// j points to current directory in right list (rightDirs)

	// If there is only one directory on each side, and no files
	// then pretend the directories have the same name
	bool bTreatDirAsEqual = 
		  (leftDirs.size() == 1)
		&& (rightDirs.size() == 1)
		&& (leftFiles.size() == 0)
		&& (rightFiles.size() == 0)
		;

	int i = 0, j = 0;
	while (1)
	{
		if (pCtxt->ShouldAbort())
			return -1;

		// Comparing directories leftDirs[i].name to rightDirs[j].name

		if (!bTreatDirAsEqual)
		{
			if (i<leftDirs.size() && (j == rightDirs.size() || collstr(leftDirs[i].filename, rightDirs[j].filename, casesensitive)<0))
			{
				const UINT nDiffCode = DIFFCODE::LEFT | DIFFCODE::DIR;
				AddToList(leftsubdir, rightsubdir, &leftDirs[i], 0, nDiffCode, myStruct);
				// Advance left pointer over left-only entry, and then retest with new pointers
				++i;
				continue;
			}
			if (j<rightDirs.size() && (i == leftDirs.size() || collstr(leftDirs[i].filename, rightDirs[j].filename, casesensitive)>0))
			{
				const UINT nDiffCode = DIFFCODE::RIGHT | DIFFCODE::DIR;
				AddToList(leftsubdir, rightsubdir, 0, &rightDirs[j], nDiffCode, myStruct);
				// Advance right pointer over right-only entry, and then retest with new pointers
				++j;
				continue;
			}
		}
		if (i<leftDirs.size())
		{
			ASSERT(j<rightDirs.size());
			if (!depth)
			{
				// Non-recursive compare
				// We are only interested about list of subdirectories to show - user can open them
				// TODO: scan one level deeper to see if directories are identical/different
				const UINT nDiffCode = DIFFCODE::BOTH | DIFFCODE::DIR;
				AddToList(leftsubdir, rightsubdir, &leftDirs[i], &rightDirs[j], nDiffCode, myStruct);
			}
			else
			{
				// Recursive compare
				String leftnewsub = leftsubprefix + leftDirs[i].filename;
				// minimize memory footprint by having left/rightnewsub share CStringData if possible
				String rightnewsub = OPTIMIZE_SHARE_CSTRINGDATA
				(
					leftsubprefix.c_str() == rightsubprefix.c_str()
				&&	leftDirs[i].filename == rightDirs[j].filename ? leftnewsub :
				) rightsubprefix + rightDirs[j].filename;
				// Test against filter so we don't include contents of filtered out directories
				// Also this is only place we can test for both-sides directories in recursive compare
				if (!pCtxt->m_piFilterGlobal->includeDir(leftnewsub.c_str(), rightnewsub.c_str()))
				{
					const UINT nDiffCode = DIFFCODE::BOTH | DIFFCODE::DIR | DIFFCODE::SKIPPED;
					AddToList(leftsubdir, rightsubdir, &leftDirs[i], &rightDirs[j], nDiffCode, myStruct);
				}
				else
				{
					// Scan recursively all subdirectories too, we are not adding folders
					if (DirScan_GetItems(paths, leftnewsub, rightnewsub, myStruct, casesensitive,
							depth - 1) == -1)
					{
						return -1;
					}
				}
			}
			++i;
			++j;
			continue;
		}
		break;
	}
	// Handle files
	// i points to current file in left list (leftFiles)
	// j points to current file in right list (rightFiles)
	i=0, j=0;
	while (1)
	{
		if (pCtxt->ShouldAbort())
			return -1;


		// Comparing file leftFiles[i].name to rightFiles[j].name
		
		if (i<leftFiles.size() && (j == rightFiles.size() ||
				collstr(leftFiles[i].filename, rightFiles[j].filename, casesensitive) < 0))
		{
			const UINT nDiffCode = DIFFCODE::LEFT | DIFFCODE::FILE;
			AddToList(leftsubdir, rightsubdir, &leftFiles[i], 0, nDiffCode, myStruct);
			// Advance left pointer over left-only entry, and then retest with new pointers
			++i;
			continue;
		}
		if (j<rightFiles.size() && (i == leftFiles.size() ||
				collstr(leftFiles[i].filename, rightFiles[j].filename, casesensitive) > 0))
		{
			const UINT nDiffCode = DIFFCODE::RIGHT | DIFFCODE::FILE;
			AddToList(leftsubdir, rightsubdir, 0, &rightFiles[j], nDiffCode, myStruct);
			// Advance right pointer over right-only entry, and then retest with new pointers
			++j;
			continue;
		}
		if (i<leftFiles.size())
		{
			ASSERT(j<rightFiles.size());
			const UINT nDiffCode = DIFFCODE::BOTH | DIFFCODE::FILE;
			AddToList(leftsubdir, rightsubdir, &leftFiles[i], &rightFiles[j], nDiffCode, myStruct);
			++i;
			++j;
			continue;
		}
		break;
	}
	return 1;
}

/**
 * @brief Compare DiffItems in list and add results to compare context.
 *
 * @param list [in] List of items to compare
 * @param pCtxt [in,out] Compare context: contains list where results are added.
 * @return 1 if compare finished, -1 if compare was aborted
 */
int DirScan_CompareItems(DiffFuncStruct *myStruct)
{
	CDiffContext *pCtxt = myStruct->context;
	int res = 1;
	WaitForSingleObject(myStruct->hSemaphore, INFINITE);
	POSITION pos = pCtxt->GetFirstDiffPosition();
	while (pos)
	{
		if (pCtxt->ShouldAbort())
		{
			res = -1;
			break;
		}
		WaitForSingleObject(myStruct->hSemaphore, INFINITE);
		DIFFITEM &di = pCtxt->GetNextDiffRefPosition(pos);
		CompareDiffItem(di, pCtxt);
	}
	return res;
}

/**
 * @brief Compare DiffItems in context marked for rescan.
 *
 * @param pCtxt [in,out] Compare context: contains list of items.
 * @return 1 if compare finished, -1 if compare was aborted
 */
int DirScan_CompareRequestedItems(DiffFuncStruct *myStruct)
{
	CDiffContext *pCtxt = myStruct->context;
	int res = 1;
	POSITION pos = pCtxt->GetFirstDiffPosition();
	
	while (pos != NULL)
	{
		if (pCtxt->ShouldAbort())
		{
			res = -1;
			break;
		}

		POSITION oldPos = pos;
		DIFFITEM &di = pCtxt->GetNextDiffRefPosition(pos);
		if (di.diffcode.isScanNeeded())
		{
			BOOL bItemsExist = TRUE;
			UpdateDiffItem(di, bItemsExist, pCtxt);
			if (bItemsExist)
				CompareDiffItem(di, pCtxt);
		}
	}
	return res;
}

/**
 * @brief Update diffitem file/dir infos.
 *
 * Re-tests dirs/files if sides still exists, and updates infos for
 * existing sides. This assumes filenames, or paths are not changed.
 * Since in normal situations (I can think of) they cannot change
 * after first compare.
 *
 * @param [in,out] di DiffItem to update.
 * @param [out] bExists Set to
 *  - TRUE if one of items exists so diffitem is valid
 *  - FALSE if items were deleted, so diffitem is not valid
 * @param [in] pCtxt Compare context
 */
void UpdateDiffItem(DIFFITEM & di, BOOL & bExists, CDiffContext *pCtxt)
{
	// Clear side-info and file-infos
	di.left.ClearPartial();
	di.right.ClearPartial();
	BOOL bLeftExists = pCtxt->UpdateInfoFromDiskHalf(di, TRUE);
	BOOL bRightExists = pCtxt->UpdateInfoFromDiskHalf(di, FALSE);
	bExists = bLeftExists || bRightExists;
	if (bLeftExists)
	{
		if (bRightExists)
			di.diffcode.setSideBoth();
		else
			di.diffcode.setSideLeft();
	}
	else
	{
		if (bRightExists)
			di.diffcode.setSideRight();
		else
			di.diffcode.setSideNone();
	}
}

/**
 * @brief Compare two diffitems and add results to difflist in context.
 *
 * This function does the actual compare for previously gathered list of
 * items. Basically we:
 * - ignore items matching filefilters
 * - add non-ignored directories (no compare for directory items)
 * - add  unique files
 * - compare files
 *
 * @param [in] di DiffItem to compare
 * @param [in,out] pCtxt Compare context: contains difflist, encoding info etc.
 * @todo For date compare, maybe we should use creation date if modification
 * date is missing?
 */
void CompareDiffItem(DIFFITEM &di, CDiffContext * pCtxt)
{
	// Clear rescan-request flag (not set by all codepaths)
	di.diffcode.diffcode &= ~DIFFCODE::NEEDSCAN;
	// Is it a directory?
	if (di.diffcode.isDirectory())
	{
		// 1. Test against filters
		if (pCtxt->m_piFilterGlobal->includeDir(di.left.filename.c_str(), di.right.filename.c_str()))
			di.diffcode.diffcode |= DIFFCODE::INCLUDED;
		else
			di.diffcode.diffcode |= DIFFCODE::SKIPPED;
		// We don't actually 'compare' directories, just add non-ignored
		// directories to list.
		StoreDiffData(di, pCtxt, NULL);
	}
	else
	{
		// 1. Test against filters
		if (pCtxt->m_piFilterGlobal->includeFile(di.left.filename.c_str(), di.right.filename.c_str()))
		{
			di.diffcode.diffcode |= DIFFCODE::INCLUDED;
			// 2. Add unique files
			// We must compare unique files to itself to detect encoding
			if (di.diffcode.isSideLeftOnly() || di.diffcode.isSideRightOnly())
			{
				if (pCtxt->m_nCompMethod != CMP_DATE &&
					pCtxt->m_nCompMethod != CMP_DATE_SIZE &&
					pCtxt->m_nCompMethod != CMP_SIZE)
				{
					FolderCmp folderCmp;
					UINT diffCode = folderCmp.prepAndCompareTwoFiles(pCtxt, di);
					
					// Add possible binary flag for unique items
					if (diffCode & DIFFCODE::BIN)
						di.diffcode.diffcode |= DIFFCODE::BIN;
					StoreDiffData(di, pCtxt, &folderCmp);
				}
				else
				{
					StoreDiffData(di, pCtxt, NULL);
				}
			}
			// 3. Compare two files
			else
			{
				// Really compare
				FolderCmp folderCmp;
				di.diffcode.diffcode |= folderCmp.prepAndCompareTwoFiles(pCtxt, di);
				StoreDiffData(di, pCtxt, &folderCmp);
			}
		}
		else
		{
			di.diffcode.diffcode |= DIFFCODE::SKIPPED;
			StoreDiffData(di, pCtxt, NULL);
		}
	}
}

/**
 * @brief Send one file or directory result back through the diff context.
 * @param [in] di Data to store.
 * @param [in] pCtxt Compare context.
 * @param [in] pCmpData Folder compare data.
 */
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const FolderCmp * pCmpData)
{
	if (pCmpData)
	{
		// Set text statistics
		if (di.diffcode.isSideLeftOrBoth())
			di.left.m_textStats = pCmpData->m_diffFileData.m_textStats0;
		if (di.diffcode.isSideRightOrBoth())
			di.right.m_textStats = pCmpData->m_diffFileData.m_textStats1;

		di.nsdiffs = pCmpData->m_ndiffs;
		di.nidiffs = pCmpData->m_ntrivialdiffs;

		if (!di.diffcode.isSideLeftOnly())
		{
			di.right.encoding = pCmpData->m_diffFileData.m_FileLocation[1].encoding;
		}
		
		if (!di.diffcode.isSideRightOnly())
		{
			di.left.encoding = pCmpData->m_diffFileData.m_FileLocation[0].encoding;
		}
	}

	GetLog()->Write
	(
		CLogFile::LCOMPAREDATA, _T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d"),
		di.left.filename.c_str(), di.left.path.c_str(), di.right.path.c_str(), di.diffcode
	);
	pCtxt->m_pCompareStats->AddItem(di.diffcode.diffcode);
	//pCtxt->AddDiff(di);
}

/**
 * @brief Add one compare item to list.
 * @param [in] sLeftDir Left subdirectory.
 * @param [in] sRightDir Right subdirectory.
 * @param [in] lent Left item data to add.
 * @param [in] rent Right item data to add.
 * @param [in] pCtxt Compare context.
 */
static void AddToList(const String &sLeftDir, const String &sRightDir,
	const DirItem * lent, const DirItem * rent,
	UINT code, DiffFuncStruct *myStruct)
{
	// We must store both paths - we cannot get paths later
	// and we need unique item paths for example when items
	// change to identical
	DIFFITEM &di = myStruct->context->AddDiff();

	di.left.path = sLeftDir;
	di.right.path = sRightDir;

	if (lent)
	{
		di.left.filename = lent->filename;
		di.left.mtime = lent->mtime;
		di.left.ctime = lent->ctime;
		di.left.size = lent->size;
		di.left.flags.attributes = lent->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyRightToLeft()
		di.left.filename = rent->filename;
	}

	if (rent)
	{
		di.right.filename = OPTIMIZE_SHARE_CSTRINGDATA
		(
			di.left.filename.c_str() == rent->filename.c_str() ? di.left.filename :
		) rent->filename;
		di.right.mtime = rent->mtime;
		di.right.ctime = rent->ctime;
		di.right.size = rent->size;
		di.right.flags.attributes = rent->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyLeftToRight()
		di.right.filename = lent->filename;
	}

	di.diffcode = code;

	GetLog()->Write
	(
		CLogFile::LCOMPAREDATA, _T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d"),
		di.left.filename.c_str(), di.left.path.c_str(), di.right.path.c_str(), code
	);
	myStruct->context->m_pCompareStats->IncreaseTotalItems();
	ReleaseSemaphore(myStruct->hSemaphore, 1, 0);
}

void // static
DirScan_InitializeDefaultCodepage()
{
	// Set thread default codepage
	// This is application-wide initialization
	// but neither MainFrame nor MergeApp included the needed headers
	DiffFileData::SetDefaultCodepage(getDefaultCodepage());
}

