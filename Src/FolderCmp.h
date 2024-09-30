/** 
 * @file  FolderCmp.h
 *
 * @brief Declaration file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id: FolderCmp.h 4619 2007-10-14 08:50:20Z jtuc $

#ifndef _FOLDERCMP_H_
#define _FOLDERCMP_H_

#include "DiffFileData.h"
#include "DiffUtils.h"
#include "ByteCompare.h"

//using namespace CompareEngines;

class CDiffContext;
class IAbortable;
class PackingInfo;
class PrediffingInfo;
//class CompareEngines::DiffUtils;

/**
 * @brief Holds plugin-related paths and information.
 */
struct PluginsContext
{
	String origFileName1;
	String origFileName2;
	String filepathUnpacked1;
	String filepathUnpacked2;
	String filepathTransformed1;
	String filepathTransformed2;
	PackingInfo * infoUnpacker;
	PrediffingInfo * infoPrediffer;
};

/**
 * @brief Class implementing file compare for folder compare.
 * This class implements (called from DirScan.cpp) compare of two files
 * during folder compare. The class implements both diffutils compare and
 * quick compare.
 */
class FolderCmp
{
public:
	FolderCmp();
	~FolderCmp();
	bool RunPlugins(CDiffContext * pCtxt, PluginsContext * plugCtxt, CString &errStr);
	void CleanupAfterPlugins(PluginsContext *plugCtxt);
	int prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di);
	int byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable);

	int m_ndiffs;
	int m_ntrivialdiffs;

	DiffFileData m_diffFileData;
	CDiffContext * m_pCtx;

private:
	CompareEngines::DiffUtils *m_pDiffUtilsEngine;
	CompareEngines::ByteCompare *m_pByteCompare;
};


#endif // _FOLDERCMP_H_
