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
 * @file  PatchTool.h
 *
 * @brief Declaration file for PatchTool class
 */
// RCS ID line follows -- this is updated by CVS
// $Id: PatchTool.h 3197 2006-04-05 15:01:32Z kimmov $

#ifndef _PATCHTOOL_H_
#define _PATCHTOOL_H_

#include "PatchDlg.h"

/** 
 * @brief Provides patch creation functionality.
 */
class CPatchTool
{
public:
	void AddFiles(const CString &file1, const CString &file2);
	void AddFiles(const CString &file1, const CString &altPath1,
		const CString &file2, const CString &altPath2);
	int CreatePatch();
	CString GetPatchFile() const;
	BOOL GetOpenToEditor() const;

protected:
	BOOL ShowDialog();

private:
	CList<PATCHFILES, PATCHFILES&> m_fileList; /**< List of files to patch. */
	CDiffWrapper m_diffWrapper; /**< DiffWrapper instance we use to create patch. */
	CPatchDlg m_dlgPatch; /**< Dialog for selecting files and options. */
	CString m_sPatchFile; /**< Patch file path and filename. */
	BOOL m_bOpenToEditor; /**< Is patch file opened to external editor? */
};

#endif	// _PATCHTOOL_H_