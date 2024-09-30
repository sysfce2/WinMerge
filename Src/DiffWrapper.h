/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffWrapper.h
 *
 * @brief Declaration file for CDiffWrapper.
 *
 * @date  Created: 2003-08-22
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DiffWrapper.h 3590 2006-09-20 14:38:13Z kimmov $

#ifndef _DIFFWRAPPER_H
#define _DIFFWRAPPER_H

#include "FileLocation.h"

#ifndef FileTextStats_h_included
#include "FileTextStats.h"
#endif

class CDiffContext;
class PrediffingInfo;
struct DIFFRANGE;
class DiffList;
struct DiffFileData;
struct file_data;
class FilterCommentsManager;
struct FilterCommentsSet;

/** @enum COMPARE_TYPE
 * @brief Different foldercompare methods.
 * These values are the foldercompare methods WinMerge supports.
 */

/** @var CMP_CONTENT
 * @brief Normal by content compare.
 * This compare type is first, default and all-seeing compare type.
 * diffutils is used for producing compare results. So all limitations
 * of diffutils (like buffering) apply to this compare method. But this
 * is also currently only compare method that produces difference lists
 * we can use in file compare.
 */

/** @var CMP_QUICK_CONTENT
 * @brief Faster byte per byte -compare.
 * This type of compare was a response for needing faster compare results
 * in folder compare. It independent from diffutils, and fully customised
 * for WinMerge. It basically does byte-per-byte compare, still implementing
 * different whitespace ignore options.
 *
 * Optionally this compare type can be stopped when first difference is found.
 * Which gets compare as fast as possible. But misses sometimes binary files
 * if zero bytes aren't found before first difference. Also difference counts
 * are not useful with that option.
 */

/** @var CMP_DATE
 * @brief Compare by modified date.
 * This compare type was added after requests and realization that in some
 * situations difference in file's timestamps is enough to judge them
 * different. E.g. when modifying files in local machine, file timestamps
 * are certainly different after modifying them. This method doesn't even
 * open files for reading them. It only reads file's infos for timestamps
 * and compares them.
 *
 * This is no doubt fastest way to compare files.
 */

/** @var CMP_DATE_SIZE
 * @brief Compare by date and then by size.
 * This method is basically same than CMP_DATE, but it adds check for file
 * sizes if timestamps are identical. This is because there are situations
 * timestamps can't be trusted alone, especially with network shares. Adding
 * checking for file sizes adds some more reliability for results with
 * minimal increase in compare time.
 */

/** @var CMP_SIZE
 * @brief Compare by file size.
 * This compare method compares file sizes. This isn't quite accurate method,
 * other than it can detect files that certainly differ. But it can show lot of
 * different files as identical too. Advantage is in some use cases where different
 * size always means files are different. E.g. automatically created logs - when
 * more data is added size increases.
 */
enum COMPARE_TYPE
{
	CMP_CONTENT = 0,
	CMP_QUICK_CONTENT,
	CMP_DATE,
	CMP_DATE_SIZE,
	CMP_SIZE,
};

/**
 * @brief Patch styles
 *
 * Diffutils can output patch in these formats.
 * @note We really use only first three types (normal + context formats).
 */
enum DIFF_OUTPUT_TYPE
{
	/**< Default output style.  */
	DIFF_OUTPUT_NORMAL,
	/**< Output the differences with lines of context before and after (-c).  */
	DIFF_OUTPUT_CONTEXT,
	/**< Output the differences in a unified context diff format (-u). */
	DIFF_OUTPUT_UNIFIED,
	/**< Output the differences as commands suitable for `ed' (-e).  */
	DIFF_OUTPUT_ED,
	/**< Output the diff as a forward ed script (-f).  */
	DIFF_OUTPUT_FORWARD_ED,
	/**< Like -f, but output a count of changed lines in each "command" (-n). */
	DIFF_OUTPUT_RCS,
	/**< Output merged #ifdef'd file (-D).  */
	DIFF_OUTPUT_IFDEF,
	/**< Output sdiff style (-y).  */
	DIFF_OUTPUT_SDIFF
};

/**
 * @brief Diffutils options users of this class must use
 */
struct DIFFOPTIONS
{
	int nIgnoreWhitespace; /**< Ignore whitespace -option. */
	BOOL bIgnoreCase; /**< Ignore case -option. */
	BOOL bIgnoreBlankLines; /**< Ignore blank lines -option. */
	BOOL bIgnoreEol; /**< Ignore EOL differences -option. */
	BOOL bFilterCommentsLines; /**< Ignore Multiline comments differences -option. */
};

/**
 * @brief Additional options for creating patch files
 */
struct PATCHOPTIONS
{
	enum output_style outputStyle; /**< Patch file style. */
	int nContext; /**< Number of context lines. */
	BOOL bAddCommandline; /**< Add diff-style commandline to patch file. */
};

/**
 * @brief Diffutils returns this statusdata about files compared
 */
struct DIFFSTATUS
{
	BOOL bLeftMissingNL; /**< Left file is missing EOL before EOF */
	BOOL bRightMissingNL; /**< Right file is missing EOL before EOF */
	BOOL bBinaries; /**< Files are binaries */
	BOOL bIdentical; /**< diffutils said files are identical */
	BOOL bPatchFileFailed; /**< Creating patch file failed */

	DIFFSTATUS() { memset(this, 0, sizeof(*this)); } // start out with all flags clear
};

/**
 * @brief Internally used diffutils options.
 */
struct DIFFSETTINGS
{
	enum output_style outputStyle; /**< Output style (for patch files) */
	int context; /**< Number of context lines (for patch files) */
	int alwaysText;
	int horizLines;
	int ignoreSpaceChange;
	int ignoreAllSpace;
	int ignoreBlankLines; /**< Ignore blank lines (both sides) */
	int filterCommentsLines;/**< Ignore Multiline comments differences.*/	
	int ignoreCase; /**< Ignore case differences? */
	int ignoreEOLDiff; /**< Ignore EOL style differences? */
	int ignoreSomeChanges;
	int lengthVaries;
	int heuristic;
	int recursive; /**< Recurse to subfolders? (not used) */
};

class FilterCommentsManager;

/**
 * @brief Wrapper class for diffengine (diffutils and ByteComparator).
 * Diffwappre class is used to run selected diffengine. For folder compare
 * there are several methods (COMPARE_TYPE), but for file compare diffutils
 * is used always. For file compare diffutils can output results to external
 * DiffList or to patch file. Output type must be selected with member
 * functions SetCreatePatchFile() and SetCreateDiffList().
 */
class CDiffWrapper
{
public:
	CDiffWrapper();
	~CDiffWrapper();
	void SetCreatePatchFile(const CString &filename);
	void SetCreateDiffList(DiffList *diffList);
	void SetDiffList(DiffList *diffList);
	void GetOptions(DIFFOPTIONS *options) const;
	void SetOptions(const DIFFOPTIONS *options);
	void SetTextForAutomaticPrediff(const CString &text);
	void SetPrediffer(PrediffingInfo * prediffer =NULL);
	void GetPrediffer(PrediffingInfo * prediffer);
	void GetPatchOptions(PATCHOPTIONS *options) const;
	void SetPatchOptions(const PATCHOPTIONS *options);
	void SetDetectMovedBlocks(BOOL bDetectMovedBlocks) { m_bDetectMovedBlocks = bDetectMovedBlocks; }
	BOOL GetDetectMovedBlocks() { return m_bDetectMovedBlocks; }
	BOOL GetAppendFiles() const;
	BOOL SetAppendFiles(BOOL bAppendFiles);
	void SetPaths(const CString &filepath1, const CString &filepath2, BOOL tempPaths);
	void SetAlternativePaths(const CString &altPath1, const CString &altPath2);
	BOOL RunFileDiff();
	void GetDiffStatus(DIFFSTATUS *status);
	void AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op);
	void FixLastDiffRange(int leftBufferLines, int rightBufferLines, BOOL left);
	void StartDirectoryDiff();
	void EndDirectoryDiff();
	const CMap<int, int, int, int> * GetMoved0() { return &m_moved0; }
	const CMap<int, int, int, int> * GetMoved1() { return &m_moved1; }
	int RightLineInMovedBlock(int leftLine);
	int LeftLineInMovedBlock(int rightLine);
	void ClearMovedLists();
	void SetCompareFiles(const CString &OriginalFile1, const CString &OriginalFile2);

protected:
	void InternalGetOptions(DIFFOPTIONS *options) const;
	void InternalSetOptions(const DIFFOPTIONS *options);
	void SwapToInternalSettings();
	void SwapToGlobalSettings();
	CString FormatSwitchString();
	BOOL Diff2Files(struct change ** diffs, DiffFileData *diffData,
		int * bin_status, int * bin_file);
	void LoadWinMergeDiffsFromDiffUtilsScript(struct change * script, const file_data * inf);
	void WritePatchFile(struct change * script, file_data * inf);

private:
	DIFFSETTINGS m_settings; /**< Compare settings for current compare */
	DIFFSETTINGS m_globalSettings; /**< Global compare settings */
	DIFFSTATUS m_status; /**< Status of last compare */
	CString m_s1File; /**< Full path to first diff'ed file. */
	CString m_s2File; /**< Full path to second diff'ed file. */
	CString m_s1AlternativePath; /**< First file's alternative path (may be relative). */
	CString m_s2AlternativePath; /**< Second file's alternative path (may be relative). */
	CString m_sOriginalFile1; /**< First file's original (NON-TEMP) path. */
	CString m_sOriginalFile2; /**< Second file's original (NON-TEMP) path. */
	CString m_sPatchFile; /**< Full path to created patch file. */
	BOOL m_bPathsAreTemp; /**< Are compared paths temporary? */
	/// prediffer info are stored only for MergeDoc
	PrediffingInfo * m_infoPrediffer;
	/// prediffer info are stored only for MergeDoc
	CString m_sToFindPrediffer;
	BOOL m_bUseDiffList; /**< Are results returned in difflist? */
	BOOL m_bDetectMovedBlocks; /**< Are moved blocks detected? */
	BOOL m_bCreatePatchFile; /**< Do we create a patch file? */
	BOOL m_bAddCmdLine; /**< Do we add commandline to patch file? */
	BOOL m_bAppendFiles; /**< Do we append to existing patch file? */
	int m_nDiffs; /**< Difference count */
	DiffList *m_pDiffList; /**< Pointer to external DiffList */
	CMap<int, int, int, int> m_moved0; /**< Moved lines map for first side */
	CMap<int, int, int, int> m_moved1; /**< Moved lines map for second side */
	FilterCommentsManager * m_FilterCommentsManager; /**< Comments filtering manager */
};


#endif // _DIFFWRAPPER_H
