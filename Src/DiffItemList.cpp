/**
 *  @file DiffItemList.cpp
 *
 *  @brief Implementation of DiffItemList
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id: DiffItemList.cpp 5646 2008-07-20 16:22:24Z jtuc $

#include "stdafx.h"
#include "DiffItemList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/**
 * @brief Constructor
 */
DiffItemList::DiffItemList()
: m_count(0)
{
}

/**
 * @brief Destructor
 */
DiffItemList::~DiffItemList()
{
	RemoveAll();
}

/**
 * @brief Add new diffitem to CDiffContext array
 */
DIFFITEM &DiffItemList::AddDiff()
{
	DIFFITEM *p = new DIFFITEM;
	m_root.Append(p);
	++m_count;
	return *p;
}

/**
 * @brief Remove diffitem from CDiffContext array
 * @param diffpos position of item to remove
 */
void DiffItemList::RemoveDiff(POSITION diffpos)
{
	DIFFITEM *p = (DIFFITEM *)diffpos;
	p->RemoveSelf();
	--m_count;
	delete p;
}

/**
 * @brief Empty CDiffContext array
 */
void DiffItemList::RemoveAll()
{
	while (m_root.IsSibling(m_root.Flink))
		RemoveDiff((POSITION)m_root.Flink);
}

/**
 * @brief Get position of first item in CDiffContext array
 */
POSITION DiffItemList::GetFirstDiffPosition() const
{
	return (POSITION)m_root.IsSibling(m_root.Flink);
}

/**
 * @brief Get position of next item in CDiffContext array
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item in current position
 */
const DIFFITEM &DiffItemList::GetNextDiffPosition(POSITION & diffpos) const
{
	DIFFITEM *p = (DIFFITEM *)diffpos;
	diffpos = (POSITION)m_root.IsSibling(p->Flink);
	return *p;
}

/**
 * @brief Get position of next item in CDiffContext array
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item (by reference) in current position
 */
DIFFITEM &DiffItemList::GetNextDiffRefPosition(POSITION & diffpos)
{
	DIFFITEM *p = (DIFFITEM *)diffpos;
	diffpos = (POSITION)m_root.IsSibling(p->Flink);
	return *p;
}

/**
 * @brief Alter some bit flags of the diffcode.
 *
 * Examples:
 *  SetDiffStatusCode(pos, DIFFCODE::SAME, DIFFCODE::COMPAREFLAGS)
 *   changes the comparison result to be the same.
 * 
 *  SetDiffStatusCode(pos, DIFFCODE::BOTH, DIFFCODE::SIDEFLAG)
 *   changes the side status to be both (sides).
 *
 * SetDiffStatusCode(pos, DIFFCODE::SAME+DIFFCODE::BOTH, DIFFCODE::COMPAREFLAGS+DIFFCODE::SIDEFLAG);
 *  changes the comparison result to be the same and the side status to be both
 */
void DiffItemList::SetDiffStatusCode(POSITION diffpos, UINT diffcode, UINT mask)
{
	ASSERT(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	ASSERT(! ((~mask) & diffcode) ); // make sure they only set flags in their mask
	di.diffcode.diffcode &= (~mask); // remove current data
	di.diffcode.diffcode |= diffcode; // add new data
}

/**
 * @brief Update difference counts.
 */
void DiffItemList::SetDiffCounts(POSITION diffpos, UINT diffs, UINT ignored)
{
	ASSERT(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.nidiffs = ignored; // see StoreDiffResult() in DirScan.cpp
	di.nsdiffs = diffs;
}

/**
 * @brief Returns item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @return Custom flags from item.
 */
UINT DiffItemList::GetCustomFlags1(POSITION diffpos) const
{
	ASSERT(diffpos);
	const DIFFITEM & di = GetDiffAt(diffpos);
	return di.customFlags1;
}

/**
 * @brief Sets item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @param [in] flag Value of flag to set.
 */
void DiffItemList::SetCustomFlags1(POSITION diffpos, UINT flag)
{
	ASSERT(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.customFlags1 = flag;
}
