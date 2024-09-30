/**
 *  @file DirScan.h
 *
 *  @brief Declaration of DirScan module (see DirScan function)
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id: DirScan.h,v 1.9 2005/08/25 22:48:26 elsapo Exp $

#ifndef DirScan_h_included
#define DirScan_h_included

class CDiffContext;
class DiffItemList;
class PathContext;
class IAbortable;

int DirScan_GetItems(const PathContext &paths, const CString & leftsubdir, const CString & rightsubdir, DiffItemList * pLst,
		bool casesensitive, int depth, CDiffContext * pCtxt);

int DirScan_CompareItems(DiffItemList & list, CDiffContext * pCtxt);
int DirScan_CompareItems(CDiffContext * pCtxt);

void DirScan_InitializeDefaultCodepage();

#endif // DirScan_h_included
