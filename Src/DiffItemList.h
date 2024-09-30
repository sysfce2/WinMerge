/**
 *  @file DiffItemList.h
 *
 *  @brief Declaration of DiffItemList
 */
// ID line follows -- this is updated by SVN
// $Id: DiffItemList.h 5646 2008-07-20 16:22:24Z jtuc $

#ifndef _DIFF_ITEM_LIST_H_
#define _DIFF_ITEM_LIST_H_

#include "DiffItem.h"

/**
 * @brief List of DiffItems
 */
class DiffItemList
{
public:
	DiffItemList();
	~DiffItemList();
	// add & remove differences
	DIFFITEM &AddDiff();
	void RemoveDiff(POSITION diffpos);
	void RemoveAll();

	// to iterate over all differences on list
	POSITION GetFirstDiffPosition() const;
	const DIFFITEM & GetNextDiffPosition(POSITION & diffpos) const;
	DIFFITEM & GetNextDiffRefPosition(POSITION & diffpos);
	const DIFFITEM & GetDiffAt(POSITION diffpos) const;
	DIFFITEM & GetDiffRefAt(POSITION diffpos);

	int GetDiffCount() const;

	void SetDiffStatusCode(POSITION diffpos, UINT diffcode, UINT mask);
	void SetDiffCounts(POSITION diffpos, UINT diffs, UINT ignored);
	UINT GetCustomFlags1(POSITION diffpos) const;
	void SetCustomFlags1(POSITION diffpos, UINT flag);

protected:
	int m_count;
	ListEntry m_root; /**< Root of list of diffitems */
};

/**
 * @brief Get copy of Diff Item at given position of CDiffContext array
 * @param diffpos position of item to return
 */
inline const DIFFITEM & DiffItemList::GetDiffAt(POSITION diffpos) const
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}

/**
 * @brief Get Diff Item (by reference) at given position of CDiffContext array
 * @param diffpos position of item to return
 */
inline DIFFITEM & DiffItemList::GetDiffRefAt(POSITION diffpos)
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}

/**
 * @brief Get number of items in CDiffContext array
 */
inline int DiffItemList::GetDiffCount() const
{
	return m_count;
}

#endif // _DIFF_ITEM_LIST_H_
