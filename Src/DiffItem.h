/**
 *  @file DiffItem.h
 *
 *  @brief Declaration of DIFFITEM
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DiffItem.h,v 1.11 2005/09/06 23:28:06 elsapo Exp $

#ifndef _DIFF_ITEM_H_
#define _DIFF_ITEM_H_

#ifndef _DIFF_FILE_INFO_H_INCLUDED
#include "DiffFileInfo.h"
#endif

/**
 * @brief values for DIFFITEM.code
 */
struct DIFFCODE
{
	enum
	{
		// We use extra bits so that no valid values are 0
		// and each set of flags is in a different hex digit
		// to make debugging easier
		// These can always be packed down in the future
		TEXTFLAGS=0x3, TEXT=0x1, BIN=0x2,
		DIRFLAGS=0x30, FILE=0x10, DIR=0x20,
		SIDEFLAGS=0x300, LEFT=0x100, RIGHT=0x200, BOTH=0x300,
		COMPAREFLAGS=0x7000, NOCMP=0x0000, SAME=0x1000, DIFF=0x2000, CMPERR=0x3000, CMPABORT=0x4000,
		FILTERFLAGS=0x30000, INCLUDED=0x10000, SKIPPED=0x20000,
		SCANFLAGS=0x100000, NEEDSCAN=0x100000,
	};

	int diffcode;

	DIFFCODE(int diffcode = 0) : diffcode(diffcode) { }

private:
	static bool Check(int code, int mask, int result) { return ((code & mask) == result); }
	static bool CheckCompare(int code, int result) { return Check(code, DIFFCODE::COMPAREFLAGS, result); }
	static bool CheckFilter(int code, int result) { return Check(code, DIFFCODE::FILTERFLAGS, result); }
	static bool CheckSide(int code, int result) { return Check(code, DIFFCODE::SIDEFLAGS, result); }

	void Set(int mask, int result) { diffcode &= (~mask); diffcode |= result; }
	void SetSide(int result) { Set(DIFFCODE::SIDEFLAGS, result); }
public:

	// file/directory
	bool isDirectory() const { return Check(diffcode, DIFFCODE::DIRFLAGS, DIFFCODE::DIR); }
	// left/right
	bool isSideLeft() const { return CheckSide(diffcode, DIFFCODE::LEFT); }
	void setSideLeft() { SetSide(DIFFCODE::LEFT); }
	bool isSideRight() const { return CheckSide(diffcode, DIFFCODE::RIGHT); }
	void setSideRight() { SetSide(DIFFCODE::RIGHT); }
	bool isSideBoth() const { return CheckSide(diffcode, DIFFCODE::BOTH); }
	void setSideBoth() { SetSide(DIFFCODE::BOTH); }
	void setSideNone() { SetSide(0); }
	// compare result
	bool isResultSame() const { return CheckCompare(diffcode, DIFFCODE::SAME); }
	bool isResultDiff() const { return (!isResultSame() && !isResultFiltered() && !isResultError() &&
			!isSideLeft() && !isSideRight()); }
	static bool isResultError(int code) { return CheckCompare(code, DIFFCODE::CMPERR); }
	bool isResultError() const { return isResultError(diffcode); }
	static bool isResultAbort(int code) { return CheckCompare(code, DIFFCODE::CMPABORT); }
	bool isResultAbort() const { return isResultAbort(diffcode); }
	// filter status
	bool isResultFiltered() const { return CheckFilter(diffcode, DIFFCODE::SKIPPED); }
	// type
	bool isBin() const { return Check(diffcode, DIFFCODE::TEXTFLAGS, DIFFCODE::BIN); }
	void setBin() { Set(DIFFCODE::TEXTFLAGS, DIFFCODE::BIN); }
	// rescan
	bool isScanNeeded() const { return ((diffcode & DIFFCODE::SCANFLAGS) == DIFFCODE::NEEDSCAN); }

};

/**
 * @brief information about one diff (including files on both sides)
 *
 * Bitmask can be seen as a 4 dimensional space; that is, there are four
 * different attributes, and each entry picks one of each attribute
 * independently.
 *
 * One dimension is how the compare went: same or different or
 * skipped or error.
 *
 * One dimension is file mode: text or binary (text is only if
 * both sides were text)
 *
 * One dimension is existence: both sides, left only, or right only
 *
 * One dimension is type: directory, or file
 *
 * @note times in fileinfo's are seconds since January 1, 1970.
 * See Dirscan.cpp/fentry and Dirscan.cpp/LoadFiles()
 */
struct DIFFITEM : DIFFCODE
{
	DiffFileInfo left; /**< Fileinfo for left file */
	DiffFileInfo right; /**< Fileinfo for right file */
	CString sLeftFilename; /**< Left filename (without path!) */
	CString sRightFilename; /**< Right filename (without path!) */
	CString sLeftSubdir; /**< Left subdirectory from root of comparison */
	CString sRightSubdir; /**< Right subdirectory from root of comparison */
	int	nsdiffs; /**< Amount of non-ignored differences */
	int ndiffs; /**< Total amount of differences */
	CString errorDesc; /**< technical note about error */
	UINT customFlags1; /**< Custom flags set 1 */

	DIFFITEM() : ndiffs(-1), nsdiffs(-1), customFlags1(0) { }

	CString getLeftFilepath(const CString &sLeftRoot) const;
	CString getRightFilepath(const CString &sRightRoot) const;
};

#endif // _DIFF_ITEM_H_