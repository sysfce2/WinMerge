/**
 *  @file DiffItemList.h
 *
 *  @brief Declaration of DiffItemList
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DiffItemList.h,v 1.3 2005/08/11 20:13:17 kimmov Exp $

#ifndef _DIFF_ITEM_LIST_H_
#define _DIFF_ITEM_LIST_H_

#ifndef _DIFF_ITEM_H_
#include "DiffItem.h"
#endif

/**
 * @brief List of DiffItems
 */
class DiffItemList
{
public:
	// add & remove differences
	virtual void AddDiff(const DIFFITEM & di);
	virtual void RemoveDiff(POSITION diffpos);
	virtual void RemoveAll();

	// to iterate over all differences on list
	POSITION GetFirstDiffPosition() const;
	DIFFITEM GetNextDiffPosition(POSITION & diffpos) const;
	DIFFITEM GetDiffAt(POSITION diffpos) const;
	DIFFITEM & GetDiffRefAt(POSITION diffpos);

	int GetDiffCount() const;

	void SetDiffStatusCode(POSITION diffpos, UINT diffcode, UINT mask);
	void SetDiffCounts(POSITION diffpos, UINT diffs, UINT ignored);
	UINT GetCustomFlags1(POSITION diffpos) const;
	void SetCustomFlags1(POSITION diffpos, UINT flag);

protected:
	CList<DIFFITEM,DIFFITEM&> m_dirlist; /**< List of diffitems */
};

#endif // _DIFF_ITEM_LIST_H_