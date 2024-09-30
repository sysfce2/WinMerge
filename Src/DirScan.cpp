/**
 *  @file DirScan.cpp
 *
 *  @brief Implementation of DirScan (q.v.) and helper functions
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id: DirScan.cpp 3944 2006-12-11 22:06:57Z kimmov $

#include "stdafx.h"
#include <shlwapi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "DirScan.h"
#include "CompareStats.h"
#include "common/unicoder.h"
#include "DiffContext.h"
#include "DiffWrapper.h"
#include "FileFilterHelper.h"
#include "logfile.h"
#include "paths.h"
#include "FileTransform.h"
#include "codepage.h"
#include "DiffItemList.h"
#include "PathContext.h"
#include "IAbortable.h"
#include "DiffFileData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Static types (ie, types only used locally)
/**
 * @brief directory or file info for one row in diff result
 * @note times are seconds since January 1, 1970.
 */
struct fentry
{
	CString name; /**< Item name */
	__int64 mtime; /**< Last modify time */
	__int64 ctime; /**< Creation modify time */
	__int64 size; /**< File size */
	int attrs; /**< Item attributes */
};
typedef CArray<fentry, fentry&> fentryArray;

// Static functions (ie, functions only used locally)
void CompareDiffItem(DIFFITEM di, CDiffContext * pCtxt);
static void LoadFiles(const CString & sDir, fentryArray * dirs, fentryArray * files);
void LoadAndSortFiles(const CString & sDir, fentryArray * dirs, fentryArray * files, bool casesensitive);
static void Sort(fentryArray * dirs, bool casesensitive);;
static int collstr(const CString & s1, const CString & s2, bool casesensitive);
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const DiffFileData * pDiffFileData);
static void AddToList(const CString & sLeftDir, const CString & sRightDir, const fentry * lent, const fentry * rent,
	int code, DiffItemList * pList, CDiffContext *pCtxt);
static void UpdateDiffItem(DIFFITEM & di, BOOL & bExists, CDiffContext *pCtxt);

static __int64 FiletimeToTimeT(FILETIME time);

/** @brief cmpmth is a typedef for a pointer to a method */
typedef int (CString::*cmpmth)(LPCTSTR sz) const;
/** @brief CALL_MEMBER_FN calls a method through a pointer to a method */
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

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
 * 
 * @param [in] paths Root paths of compare
 * @param [in] subdir Current subdirectory under root paths
 * @param [in,out] list List where found items are added
 * @param [in] casesensitive Is filename compare casesensitive?
 * @param [in] depth Levels of subdirectories to scan, -1 scans all
 * @param [in] pCtxt Compare context
 * @param [in] piAbortable Interface allowing compare to be aborted
 * @return 1 normally, -1 if compare was aborted
 */
int DirScan_GetItems(const PathContext &paths, const CString & leftsubdir, const CString & rightsubdir, DiffItemList *pList,
		bool casesensitive, int depth, CDiffContext * pCtxt)
{
	static const TCHAR backslash[] = _T("\\");

	CString sLeftDir = paths.GetLeft();
	CString sRightDir = paths.GetRight();
	CString leftsubprefix;
	CString rightsubprefix;
	if (!leftsubdir.IsEmpty())
	{
		sLeftDir += backslash + leftsubdir;
		sRightDir += backslash + rightsubdir;
		leftsubprefix = leftsubdir + backslash;
		// minimize memory footprint by having left/rightsubprefix share CStringData if possible
		rightsubprefix = OPTIMIZE_SHARE_CSTRINGDATA
		(
			(LPCTSTR)leftsubdir == (LPCTSTR)rightsubdir ? leftsubprefix : 
		) rightsubdir + backslash;
	}

	fentryArray leftDirs, leftFiles, rightDirs, rightFiles;
	LoadAndSortFiles(sLeftDir, &leftDirs, &leftFiles, casesensitive);
	LoadAndSortFiles(sRightDir, &rightDirs, &rightFiles, casesensitive);

	// Allow user to abort scanning
	if (pCtxt->ShouldAbort())
		return -1;

	// Handle directories
	// i points to current directory in left list (leftDirs)
	// j points to current directory in right list (rightDirs)

	// If there is only one directory on each side, and no files
	// then pretend the directories have the same name
	bool bTreatDirAsEqual = 
		  (leftDirs.GetSize() == 1)
		&& (rightDirs.GetSize() == 1)
		&& (leftFiles.GetSize() == 0)
		&& (rightFiles.GetSize() == 0)
		;

	int i=0, j=0;
	while (1)
	{
		if (pCtxt->ShouldAbort())
			return -1;

		// Comparing directories leftDirs[i].name to rightDirs[j].name

		if (!bTreatDirAsEqual)
		{
			if (i<leftDirs.GetSize() && (j==rightDirs.GetSize() || collstr(leftDirs[i].name, rightDirs[j].name, casesensitive)<0))
			{
				int nDiffCode = DIFFCODE::LEFT | DIFFCODE::DIR;
				AddToList(leftsubdir, rightsubdir, &leftDirs[i], 0, nDiffCode, pList, pCtxt);
				// Advance left pointer over left-only entry, and then retest with new pointers
				++i;
				continue;
			}
			if (j<rightDirs.GetSize() && (i==leftDirs.GetSize() || collstr(leftDirs[i].name, rightDirs[j].name, casesensitive)>0))
			{
				int nDiffCode = DIFFCODE::RIGHT | DIFFCODE::DIR;
				AddToList(leftsubdir, rightsubdir, 0, &rightDirs[j], nDiffCode, pList, pCtxt);
				// Advance right pointer over right-only entry, and then retest with new pointers
				++j;
				continue;
			}
		}
		if (i<leftDirs.GetSize())
		{
			ASSERT(j<rightDirs.GetSize());
			if (!depth)
			{
				// Non-recursive compare
				// We are only interested about list of subdirectories to show - user can open them
				// TODO: scan one level deeper to see if directories are identical/different
				const int nDiffCode = DIFFCODE::BOTH | DIFFCODE::DIR;
				AddToList(leftsubdir, rightsubdir, &leftDirs[i], &rightDirs[j], nDiffCode, pList, pCtxt);
			}
			else
			{
				// Recursive compare
				CString leftnewsub = leftsubprefix + leftDirs[i].name;
				// minimize memory footprint by having left/rightnewsub share CStringData if possible
				CString rightnewsub = OPTIMIZE_SHARE_CSTRINGDATA
				(
					(LPCTSTR)leftsubprefix == (LPCTSTR)rightsubprefix
				&&	leftDirs[i].name == rightDirs[j].name ? leftnewsub :
				) rightsubprefix + rightDirs[j].name;
				// Test against filter so we don't include contents of filtered out directories
				// Also this is only place we can test for both-sides directories in recursive compare
				if (!pCtxt->m_piFilterGlobal->includeDir(leftnewsub, rightnewsub))
				{
					const int nDiffCode = DIFFCODE::BOTH | DIFFCODE::DIR | DIFFCODE::SKIPPED;
					AddToList(leftsubdir, rightsubdir, &leftDirs[i], &rightDirs[j], nDiffCode, pList, pCtxt);
				}
				else
				{
					// Scan recursively all subdirectories too, we are not adding folders
					if (DirScan_GetItems(paths, leftnewsub, rightnewsub, pList, casesensitive,
							depth - 1, pCtxt) == -1)
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
		
		if (i<leftFiles.GetSize() && (j==rightFiles.GetSize() ||
				collstr(leftFiles[i].name, rightFiles[j].name, casesensitive) < 0))
		{
			const int nDiffCode = DIFFCODE::LEFT | DIFFCODE::FILE;
			AddToList(leftsubdir, rightsubdir, &leftFiles[i], 0, nDiffCode, pList, pCtxt);
			// Advance left pointer over left-only entry, and then retest with new pointers
			++i;
			continue;
		}
		if (j<rightFiles.GetSize() && (i==leftFiles.GetSize() ||
				collstr(leftFiles[i].name, rightFiles[j].name, casesensitive) > 0))
		{
			const int nDiffCode = DIFFCODE::RIGHT | DIFFCODE::FILE;
			AddToList(leftsubdir, rightsubdir, 0, &rightFiles[j], nDiffCode, pList, pCtxt);
			// Advance right pointer over right-only entry, and then retest with new pointers
			++j;
			continue;
		}
		if (i<leftFiles.GetSize())
		{
			ASSERT(j<rightFiles.GetSize());
			const int nDiffCode = DIFFCODE::BOTH | DIFFCODE::FILE;
			AddToList(leftsubdir, rightsubdir, &leftFiles[i], &rightFiles[j], nDiffCode, pList, pCtxt);
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
 * @param piAbortable [in] Interface allowing to abort compare
 * @return 1 if compare finished, -1 if compare was aborted
 */
int DirScan_CompareItems(DiffItemList & list, CDiffContext * pCtxt)
{
	int res = 1;
	POSITION pos = list.GetFirstDiffPosition();
	
	while (pos != NULL)
	{
		if (pCtxt->ShouldAbort())
		{
			res = -1;
			break;
		}

		DIFFITEM di = list.GetNextDiffPosition(pos);
		CompareDiffItem(di, pCtxt);
	}
	return res;
}

/**
 * @brief Compare DiffItems in context marked for rescan.
 *
 * @param pCtxt [in,out] Compare context: contains list of items.
 * @param piAbortable [in] Interface allowing to abort compare
 * @return 1 if compare finished, -1 if compare was aborted
 */
int DirScan_CompareItems(CDiffContext * pCtxt)
{
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
		DIFFITEM di = pCtxt->GetNextDiffPosition(pos);
		if (di.isScanNeeded())
		{
			BOOL bItemsExist = TRUE;
			pCtxt->RemoveDiff(oldPos);
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
	di.left.Clear();
	di.right.Clear();
	BOOL bLeftExists = pCtxt->UpdateInfoFromDiskHalf(di, TRUE);
	BOOL bRightExists = pCtxt->UpdateInfoFromDiskHalf(di, FALSE);
	bExists = bLeftExists || bRightExists;
	if (bLeftExists)
	{
		if (bRightExists)
			di.setSideBoth();
		else
			di.setSideLeft();
	}
	else
	{
		if (bRightExists)
			di.setSideRight();
		else
			di.setSideNone();
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
void CompareDiffItem(DIFFITEM di, CDiffContext * pCtxt)
{
	// Clear rescan-request flag (not set by all codepaths)
	di.diffcode &= ~DIFFCODE::NEEDSCAN;
	// Is it a directory?
	if (di.isDirectory())
	{
		// 1. Test against filters
		if (pCtxt->m_piFilterGlobal->includeDir(di.sLeftFilename, di.sRightFilename))
			di.diffcode |= DIFFCODE::INCLUDED;
		else
			di.diffcode |= DIFFCODE::SKIPPED;
		// We don't actually 'compare' directories, just add non-ignored
		// directories to list.
		StoreDiffData(di, pCtxt, NULL);
	}
	else
	{
		// 1. Test against filters
		if (pCtxt->m_piFilterGlobal->includeFile(di.sLeftFilename, di.sRightFilename))
		{
			di.diffcode |= DIFFCODE::INCLUDED;
			// 2. Add unique files
			// We must compare unique files to itself to detect encoding
			if (di.isSideLeft() || di.isSideRight())
			{
				if (pCtxt->m_nCompMethod != CMP_DATE &&
					pCtxt->m_nCompMethod != CMP_DATE_SIZE &&
					pCtxt->m_nCompMethod != CMP_SIZE)
				{
					DiffFileData diffdata;
					int diffCode = diffdata.prepAndCompareTwoFiles(pCtxt, di);
					
					// Add possible binary flag for unique items
					if (diffCode & DIFFCODE::BIN)
						di.diffcode |= DIFFCODE::BIN;
					StoreDiffData(di, pCtxt, &diffdata);
				}
				else
				{
					StoreDiffData(di, pCtxt, NULL);
				}
			}
			// 3. Compare two files
			else if (pCtxt->m_nCompMethod == CMP_DATE ||
				pCtxt->m_nCompMethod == CMP_DATE_SIZE)
			{
				// Compare by modified date
				// Check that we have both filetimes
				if (di.left.mtime != 0 && di.right.mtime != 0)
				{
					__int64 nTimeDiff = di.left.mtime - di.right.mtime;
					// Remove sign
					nTimeDiff = (nTimeDiff > 0 ? nTimeDiff : -nTimeDiff);
					if (pCtxt->m_bIgnoreSmallTimeDiff)
					{
						// If option to ignore small timediffs (couple of seconds)
						// is set, decrease absolute difference by allowed diff
						nTimeDiff -= SmallTimeDiff;
					}
					if (nTimeDiff <= 0)
						di.diffcode |= DIFFCODE::TEXT | DIFFCODE::SAME;
					else
						di.diffcode |= DIFFCODE::TEXT | DIFFCODE::DIFF;
				}
				else
				{
					// Filetimes for item(s) could not be read. So we have to
					// set error status, unless we have DATE_SIZE -compare
					// when we have still hope for size compare..
					if (pCtxt->m_nCompMethod == CMP_DATE_SIZE)
						di.diffcode |= DIFFCODE::TEXT | DIFFCODE::SAME;
					else
						di.diffcode |= DIFFCODE::TEXT | DIFFCODE::CMPERR;
				}
				
				// This is actual CMP_DATE_SIZE method..
				// If file sizes differ mark them different
				if ( pCtxt->m_nCompMethod == CMP_DATE_SIZE && di.isResultSame())
				{
					if (di.left.size != di.right.size)
					{
						di.diffcode &= ~DIFFCODE::SAME;
						di.diffcode |= DIFFCODE::DIFF;
					}
				}

				// report result back to caller
				StoreDiffData(di, pCtxt, NULL);
			}
			else if (pCtxt->m_nCompMethod == CMP_SIZE)
			{
				// Compare by size
				if (di.left.size == di.right.size)
					di.diffcode |= DIFFCODE::SAME;
				else
					di.diffcode |= DIFFCODE::DIFF;

				// report result back to caller
				StoreDiffData(di, pCtxt, NULL);
			}
			else
			{
				// Really compare
				DiffFileData diffdata;
				di.diffcode |= diffdata.prepAndCompareTwoFiles(pCtxt, di);
				// report result back to caller
				StoreDiffData(di, pCtxt, &diffdata);
			}
		}
		else
		{
			di.diffcode |= DIFFCODE::SKIPPED;
			StoreDiffData(di, pCtxt, NULL);
		}
	}
}

/**
 * @brief Load arrays with all directories & files in specified dir
 */
void LoadAndSortFiles(const CString & sDir, fentryArray * dirs, fentryArray * files, bool casesensitive)
{
	LoadFiles(sDir, dirs, files);
	Sort(dirs, casesensitive);
	Sort(files, casesensitive);
}

/**
 * @brief Convert time in type FILETIME to type int (time_t compatible).
 * @param [in] time Time in FILETIME type.
 * @return Time in time_t compiliant integer.
 */
static __int64 FiletimeToTimeT(FILETIME time)
{
	const __int64 SecsTo100ns = 10000000;
	const __int64 SecsBetweenEpochs = 11644473600;
	__int64 converted_time;
	converted_time = ((__int64)time.dwHighDateTime << 32) + time.dwLowDateTime;
	converted_time -= (SecsBetweenEpochs * SecsTo100ns);
	converted_time /= SecsTo100ns;
	return converted_time;
}

/**
 * @brief Find files and subfolders from given folder.
 * This function saves all files and subfolders in given folder to arrays.
 * We use 64-bit version of stat() to get times since find doesn't return
 * valid times for very old files (around year 1970). Even stat() seems to
 * give negative time values but we can live with that. Those around 1970
 * times can happen when file is created so that it  doesn't get valid
 * creation or modificatio dates.
 * @param [in] sDir Base folder for files and subfolders.
 * @param [in, out] dirs Array where subfolders are stored.
 * @param [in, out] files Array where files are stored.
 */
void LoadFiles(const CString & sDir, fentryArray * dirs, fentryArray * files)
{
	CString sPattern = sDir;
	sPattern.TrimRight(_T("\\"));
	sPattern += _T("\\*.*");

	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(sPattern, &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			DWORD dwIsDirectory = ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if (dwIsDirectory && StrStr(_T(".."), ff.cFileName))
				continue;

			fentry ent;

			// Save filetimes as seconds since January 1, 1970
			// Note that times can be < 0 if they are around that 1970..
			// Anyway that is not sensible case for normal files so we can
			// just use zero for their time.
			ent.ctime = FiletimeToTimeT(ff.ftCreationTime);
			if (ent.ctime < 0)
				ent.ctime = 0;
			ent.mtime = FiletimeToTimeT(ff.ftLastWriteTime);
			if (ent.mtime < 0)
				ent.mtime = 0;

			if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				ent.size = -1;  // No size for directories
			else
			{
				ent.size = ((__int64)ff.nFileSizeHigh << 32) + ff.nFileSizeLow;
			}

			ent.name = ff.cFileName;
			ent.attrs = ff.dwFileAttributes;
			(dwIsDirectory ? dirs : files) -> Add(ent);
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}
}

/**
 * @brief case-sensitive collate function for qsorting an array
 */
static int __cdecl cmpstring(const void *elem1, const void *elem2)
{
	const fentry * s1 = static_cast<const fentry *>(elem1);
	const fentry * s2 = static_cast<const fentry *>(elem2);
	return s1->name.Collate(s2->name);
}

/**
 * @brief case-insensitive collate function for qsorting an array
 */
static int __cdecl cmpistring(const void *elem1, const void *elem2)
{
	const fentry * s1 = static_cast<const fentry *>(elem1);
	const fentry * s2 = static_cast<const fentry *>(elem2);
	return s1->name.CollateNoCase(s2->name);
}

/**
 * @brief sort specified array
 */
void Sort(fentryArray * dirs, bool casesensitive)
{
	fentry * data = dirs->GetData();
	if (!data) return;
	int (__cdecl *comparefnc)(const void *elem1, const void *elem2) = (casesensitive ? cmpstring : cmpistring);
	qsort(data, dirs->GetSize(), sizeof(dirs->GetAt(0)), comparefnc);
}

/**
 * @brief  Compare (NLS aware) two strings, either case-sensitive or case-insensitive as caller specifies
 */
static int collstr(const CString & s1, const CString & s2, bool casesensitive)
{
	if (casesensitive)
		return s1.Collate(s2);
	else
		return s1.CollateNoCase(s2);
}

/**
 * @brief Send one file or directory result back through the diff context
 */
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const DiffFileData * pDiffFileData)
{
	if (pDiffFileData)
	{
		// Set text statistics
		if (di.isSideLeftOrBoth())
			di.left.m_textStats = pDiffFileData->m_textStats0;
		if (di.isSideRightOrBoth())
			di.right.m_textStats = pDiffFileData->m_textStats1;

		di.nsdiffs = pDiffFileData->m_ndiffs;
		di.nidiffs = pDiffFileData->m_ntrivialdiffs;

		if (!di.isSideLeft())
		{
			di.right.encoding = pDiffFileData->m_FileLocation[1].encoding;
		}
		
		if (!di.isSideRight())
		{
			di.left.encoding = pDiffFileData->m_FileLocation[0].encoding;
		}
	}

	gLog.Write
	(
		CLogFile::LCOMPAREDATA, _T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d"),
		(LPCTSTR)di.sLeftFilename, (LPCTSTR)_T("di.left.spath"), (LPCTSTR)_T("di.right.spath"), di.diffcode
	);
	pCtxt->AddDiff(di);
}

/**
 * @brief Add one compare item to list.
 */
static void AddToList(const CString & sLeftDir, const CString & sRightDir, const fentry * lent, const fentry * rent,
	int code, DiffItemList * pList, CDiffContext *pCtxt)
{
	// We must store both paths - we cannot get paths later
	// and we need unique item paths for example when items
	// change to identical

	DIFFITEM di;

	di.sLeftSubdir = sLeftDir;
	di.sRightSubdir = sRightDir;

	if (lent)
	{
		di.sLeftFilename = lent->name;
		di.left.mtime = lent->mtime;
		di.left.ctime = lent->ctime;
		di.left.size = lent->size;
		di.left.flags.attributes = lent->attrs;
	}
	else
	{
		// Don't break CDirView::DoCopyRightToLeft()
		di.sLeftFilename = rent->name;
	}

	if (rent)
	{
		di.sRightFilename = OPTIMIZE_SHARE_CSTRINGDATA
		(
			di.sLeftFilename == rent->name ? di.sLeftFilename :
		) rent->name;
		di.right.mtime = rent->mtime;
		di.right.ctime = rent->ctime;
		di.right.size = rent->size;
		di.right.flags.attributes = rent->attrs;
	}
	else
	{
		// Don't break CDirView::DoCopyLeftToRight()
		di.sRightFilename = lent->name;
	}

	di.diffcode = code;

	gLog.Write
	(
		CLogFile::LCOMPAREDATA, _T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d"),
		(LPCTSTR)di.sLeftFilename, (LPCTSTR)_T("di.left.spath"), (LPCTSTR)_T("di.right.spath"), code
	);
	pCtxt->m_pCompareStats->IncreaseTotalItems();
	pList->AddDiff(di);
}

void // static
DirScan_InitializeDefaultCodepage()
{
	// Set thread default codepage
	// This is application-wide initialization
	// but neither MainFrame nor MergeApp included the needed headers
	DiffFileData::SetDefaultCodepage(getDefaultCodepage());
}

