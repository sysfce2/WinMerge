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
 * @file  DiffFileInfo.h
 *
 * @brief Declaration file for DiffFileInfo
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DiffFileInfo.h,v 1.10 2005/06/17 19:21:17 kimmov Exp $

#ifndef _DIFF_FILE_INFO_H_INCLUDED
#define _DIFF_FILE_INFO_H_INCLUDED

#ifndef _FILE_INFO_H_INCLUDED
#include "FileInfo.h"
#endif

/**
 * @brief Class for fileflags and coding info.
 */
struct DiffFileFlags : public FileFlags
{
	DWORD coding; /**< Coding info for item */
	DiffFileFlags() : coding(0) { FileFlags(); }
	
	/// Convert flags and coding to string for UI.
	CString toString() const
		{
			CString sflags;
			if (attributes & FILE_ATTRIBUTE_READONLY)
				sflags += _T("R");
			if (attributes & FILE_ATTRIBUTE_HIDDEN)
				sflags += _T("H");
			if (attributes & FILE_ATTRIBUTE_SYSTEM)
				sflags += _T("S");
			if (attributes & FILE_ATTRIBUTE_ARCHIVE)
				sflags += _T("A");

			if ((coding & coding_mask) == UTF_8)
				sflags += _T("8");
			if ((coding & coding_mask) == UCS_2BE)
				sflags += _T("B");
			if ((coding & coding_mask) == UCS_2LE)
				sflags += _T("L");
			if ((coding & coding_mask) == UCS_4)
				sflags += _T("4");
			return sflags;
		}

	/**
	* @brief Encodings supported.
	*/
	enum
	{ 
		UTF_8 = 0x1000,
		UCS_4 = 0x2000,
		UCS_2BE = 0x3000,
		UCS_2LE = 0x4000,
		coding_mask = 0x7000,
	};
};

/**
 * @brief Information for file
 */
struct DiffFileInfo : public FileInfo
{
	bool bVersionChecked; /**< true if version string is up-to-date */
	DiffFileFlags flags; /**< file attributes */
	int codepage; /**< 8bit codepage, if applicable, 0 is unknown or N/A */
	int unicoding; /**< Unicode encoding (ucr::CODESET) */
	DiffFileInfo() { Clear(); }

	CString getEncodingString() const;
	// We could stash a pointer here to the parent DIFFITEM
	// but, I ran into trouble with, I think, the DIFFITEM copy constructor
	
	void Update(CString sFilePath);
	void Clear();
};

#endif // _DIFF_FILE_INFO_H_INCLUDED
