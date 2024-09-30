/**
 *  @file DiffItem.cpp
 *
 *  @brief Implementation of DIFFITEM
 */ 
// ID line follows -- this is updated by SVN
// $Id: DiffItem.cpp 5019 2008-02-10 11:50:33Z jtuc $

#include "stdafx.h"
#include "DiffItem.h"
#include "paths.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/**
 * @brief Copy constructor.
 * @param [in] di Object to copy.
 */
DIFFITEM::DIFFITEM(const DIFFITEM& di)
{
	diffcode = di.diffcode;
	left = di.left;
	right = di.right;
	nsdiffs = di.nsdiffs;
	nidiffs = di.nidiffs;
	errorDesc = di.errorDesc;
	customFlags1 = di.customFlags1;
	empty = di.empty;
}

/**
 * @brief Assignment operator override.
 * @param [in] di Object to copy.
 * @return Copy of given object.
 */
DIFFITEM& DIFFITEM::operator=(const DIFFITEM& di)
{
	if (this != &di)
	{
		diffcode = di.diffcode;
		left = di.left;
		right = di.right;
		nsdiffs = di.nsdiffs;
		nidiffs = di.nidiffs;
		errorDesc = di.errorDesc;
		customFlags1 = di.customFlags1;
		empty = di.empty;
	}
	return *this;
}

/** @brief Make and return a diffitem with no data */
DIFFITEM DIFFITEM::MakeEmptyDiffItem() // static
{
	DIFFITEM di;
	di.empty = true;
	return di;
}

/** @brief Return path to left file, including all but file name */
String DIFFITEM::getLeftFilepath(const String &sLeftRoot) const
{
	String sPath;
	if (!diffcode.isSideRightOnly())
	{
		sPath = paths_ConcatPath(sLeftRoot, left.path);
	}
	return sPath;
}

/** @brief Return path to right file, including all but file name */
String DIFFITEM::getRightFilepath(const String &sRightRoot) const
{
	String sPath;
	if (!diffcode.isSideLeftOnly())
	{
		sPath = paths_ConcatPath(sRightRoot, right.path);
	}
	return sPath;
}
