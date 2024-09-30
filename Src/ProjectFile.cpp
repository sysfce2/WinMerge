/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ProjectFile.cpp
 *
 * @brief Implementation file for ProjectFile class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id: ProjectFile.cpp 6216 2008-12-21 12:18:33Z sdottaka $

#include "stdafx.h"
#include <scew/scew.h>

#include "ProjectFile.h"
#include "Merge.h"

// ATL conversion macro hack for UTF-8 conversion
#define UTF82W(lpa) (\
	((_lpa = lpa) == NULL) ? NULL : (\
		_convert = (lstrlenA(_lpa)+1),\
		AtlA2WHelper((LPWSTR) alloca(_convert*2), _lpa, _convert, CP_UTF8)))

#define W2UTF8(lpw) (\
	((_lpw = lpw) == NULL) ? NULL : (\
		_convert = (lstrlenW(_lpw)+1)*6,\
		AtlW2AHelper((LPSTR) alloca(_convert), _lpw, _convert, CP_UTF8)))

#define UTF82A(lpu) W2A(UTF82W(lpu))
#define A2UTF8(lpa) W2UTF8(A2W(lpa))
#ifdef _UNICODE
#  define UTF82T(lpu) UTF82W(lpu)
#  define T2UTF8(lpw) W2UTF8(lpw)
#else
#  define UTF82T(lpu) UTF82A(lpu)
#  define T2UTF8(lpw) A2UTF8(lpw)
#endif

// Constants for xml element names
const char Root_element_name[] = "project";
const char Paths_element_name[] = "paths";
const char Left_element_name[] = "left";
const char Right_element_name[] = "right";
const char Filter_element_name[] = "filter";
const char Subfolders_element_name[] = "subfolders";
const char Left_ro_element_name[] = "left-readonly";
const char Right_ro_element_name[] = "right-readonly";

/** 
 * @brief Standard constructor.
 */
 ProjectFile::ProjectFile()
: m_bHasLeft(FALSE)
, m_bHasRight(FALSE)
, m_bHasFilter(FALSE)
, m_bHasSubfolders(FALSE)
, m_subfolders(-1)
, m_bLeftReadOnly(FALSE)
, m_bRightReadOnly(FALSE)
{
}

/** 
 * @brief Open given path-file and read data from it to member variables.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return TRUE if reading succeeded, FALSE if error happened.
 */
BOOL ProjectFile::Read(LPCTSTR path, String *sError)
{
	BOOL loaded = FALSE;
    scew_tree* tree = NULL;
    scew_parser* parser = NULL;

    parser = scew_parser_create();
    scew_parser_ignore_whitespaces(parser, 1);

	FILE * fp = _tfopen(path, _T("r"));
	if (fp)
	{
		if (scew_parser_load_file_fp(parser, fp))
		{
			tree = scew_parser_tree(parser);

			scew_element * root = GetRootElement(tree);
			if (root)
			{
				// Currently our content is paths, so expect
				// having paths in valid project file!
				if (GetPathsData(root))
					loaded = TRUE;
			};
		}
		scew_tree_free(tree);

		/* Frees the SCEW parser */
		scew_parser_free(parser);
		fclose(fp);
	}
	return loaded;
}

/** 
 * @brief Return project file XML's root element.
 * @param [in] tree XML tree we got from the parser.
 * @return Root element pointer.
 */
scew_element* ProjectFile::GetRootElement(scew_tree * tree)
{
	scew_element * root = NULL;

	if (tree != NULL)
	{
		root = scew_tree_root(tree);
	}

	if (root != NULL)
	{
		// Make sure we have correct root element
		if (strcmp(Root_element_name, scew_element_name(root)) != 0)
		{
			root = NULL;
		}
	}
	return root;
}

/** 
 * @brief Reads the paths data from the XML data.
 * This function reads the paths data inside given element in XML data.
 * @param [in] parent Parent element for the paths data.
 * @return TRUE if pathdata was found from the file.
 */
BOOL ProjectFile::GetPathsData(scew_element * parent)
{
	USES_CONVERSION;
	BOOL bFoundPaths = FALSE;
	scew_element *paths = NULL;

	if (parent != NULL)
	{
		paths = scew_element_by_name(parent, Paths_element_name);
	}

	if (paths != NULL)
	{
		bFoundPaths = TRUE;
		scew_element *left = NULL;
		scew_element *right = NULL;
		scew_element *filter = NULL;
		scew_element *subfolders = NULL;
		scew_element *left_ro = NULL;
		scew_element *right_ro = NULL;

		left = scew_element_by_name(paths, Left_element_name);
		right = scew_element_by_name(paths, Right_element_name);
		filter = scew_element_by_name(paths, Filter_element_name);
		subfolders = scew_element_by_name(paths, Subfolders_element_name);
		left_ro = scew_element_by_name(paths, Left_ro_element_name);
		right_ro = scew_element_by_name(paths, Right_ro_element_name);

		if (left)
		{
			LPCSTR path = NULL;
			path = scew_element_contents(left);
			m_leftFile = UTF82T(path);
			m_bHasLeft = TRUE;
		}
		if (right)
		{
			LPCSTR path = NULL;
			path = scew_element_contents(right);
			m_rightFile = UTF82T(path);
			m_bHasRight = TRUE;
		}
		if (filter)
		{
			LPCSTR filtername = NULL;
			filtername = scew_element_contents(filter);
			m_filter = UTF82T(filtername);
			m_bHasFilter = TRUE;
		}
		if (subfolders)
		{
			LPCSTR folders = NULL;
			folders = scew_element_contents(subfolders);
			m_subfolders = atoi(folders);
			m_bHasSubfolders = TRUE;
		}
		if (left_ro)
		{
			LPCSTR readonly = NULL;
			readonly = scew_element_contents(left_ro);
			m_bLeftReadOnly = (atoi(readonly) != 0);
		}
		if (right_ro)
		{
			LPCSTR readonly = NULL;
			readonly = scew_element_contents(right_ro);
			m_bRightReadOnly = (atoi(readonly) != 0);
		}
	}
	return bFoundPaths;
}

/** 
 * @brief Save data from member variables to path-file.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return TRUE if saving succeeded, FALSE if error happened.
 */
BOOL ProjectFile::Save(LPCTSTR path, String *sError)
{
	BOOL success = TRUE;
	scew_tree* tree = NULL;
	scew_element* root = NULL;
	scew_element* paths = NULL;

	tree = scew_tree_create();
	root = scew_tree_add_root(tree, Root_element_name);
	if (root != NULL)
	{
		paths = AddPathsElement(root);
	}
	else
		success = FALSE;

	if (paths != NULL)
	{
		AddPathsContent(paths);
	}
	else
		success = FALSE;
	
	scew_tree_set_xml_encoding(tree, "UTF-8");

	// Set the XML file standalone
	scew_tree_set_xml_standalone(tree, 1);

	FILE * fp = _tfopen(path, _T("w"));
	if (fp)
	{
		if (!scew_writer_tree_fp(tree, fp))
		{
			success = FALSE;
			*sError = theApp.LoadString(IDS_FILEWRITE_ERROR);
		}
		fclose(fp);
	}
	else
	{
		success = FALSE;
	}
	
	/* Frees the SCEW tree */
	scew_tree_free(tree);

	if (success == FALSE)
	{
		*sError = theApp.LoadString(IDS_FILEWRITE_ERROR);
	}
	return success;
}

/**
 * @brief Add paths element into XML tree.
 * @param [in] parent Parent element for the paths element.
 * @return pointer to added paths element.
 */
scew_element* ProjectFile::AddPathsElement(scew_element * parent)
{
	scew_element* element = NULL;
	element = scew_element_add(parent, Paths_element_name);
	return element;
}

/**
 * @brief Covert characters that are unsafe for use in XML
 * @param [in] str The string to be converted
 * @return The converted string
 */
static String EscapeXML(const String &str)
{
	String escapedStr = str;
	string_replace(escapedStr, _T("&"), _T("&amp;"));
	string_replace(escapedStr, _T("<"), _T("&lt;"));
	string_replace(escapedStr, _T(">"), _T("&gt;"));
	return escapedStr;
}

/**
 * @brief Add paths data to the XML tree.
 * This function adds our paths data to the XML tree.
 * @param [in] parent Parent element for paths data.
 * @return TRUE if we succeeded, FALSE otherwise.
 */
BOOL ProjectFile::AddPathsContent(scew_element * parent)
{
	USES_CONVERSION;
	scew_element* element = NULL;

	if (!m_leftFile.IsEmpty())
	{
		element = scew_element_add(parent, Left_element_name);
		String path = m_leftFile;
		scew_element_set_contents(element, T2UTF8(EscapeXML(path).c_str()));
	}

	if (!m_rightFile.IsEmpty())
	{
		element = scew_element_add(parent, Right_element_name);
		String path = m_rightFile;
		scew_element_set_contents(element, T2UTF8(EscapeXML(path).c_str()));
	}

	if (!m_filter.IsEmpty())
	{
		element = scew_element_add(parent, Filter_element_name);
		String filter = m_filter;
		scew_element_set_contents(element, T2UTF8(EscapeXML(filter).c_str()));
	}

	element = scew_element_add(parent, Subfolders_element_name);
	if (m_subfolders != 0)
		scew_element_set_contents(element, "1");
	else
		scew_element_set_contents(element, "0");

	element = scew_element_add(parent, Left_ro_element_name);
	if (m_bLeftReadOnly)
		scew_element_set_contents(element, "1");
	else
		scew_element_set_contents(element, "0");

	element = scew_element_add(parent, Right_ro_element_name);
	if (m_bRightReadOnly)
		scew_element_set_contents(element, "1");
	else
		scew_element_set_contents(element, "0");

	return TRUE;
}

/** 
 * @brief Returns if left path is defined in project file.
 * @return TRUE if project file has left path.
 */
BOOL ProjectFile::HasLeft() const
{
	return m_bHasLeft;
}

/** 
 * @brief Returns if right path is defined in project file.
 * @return TRUE if project file has right path.
 */
BOOL ProjectFile::HasRight() const
{
	return m_bHasRight;
}

/** 
 * @brief Returns if filter is defined in project file.
 * @return TRUE if project file has filter.
 */
BOOL ProjectFile::HasFilter() const
{
	return m_bHasFilter;
}

/** 
 * @brief Returns if subfolder is defined in projectfile.
 * @return TRUE if project file has subfolder definition.
 */
BOOL ProjectFile::HasSubfolders() const
{
	return m_bHasSubfolders;
}

/** 
 * @brief Returns left path.
 * @param [out] pReadOnly TRUE if readonly was specified for path.
 */
CString ProjectFile::GetLeft(BOOL * pReadOnly /*=NULL*/) const
{
	if (pReadOnly)
		*pReadOnly = m_bLeftReadOnly;
	return m_leftFile;
}

/** 
 * @brief Returns if left path is specified read-only.
 */
BOOL ProjectFile::GetLeftReadOnly() const
{
	return m_bLeftReadOnly;
}

/** 
 * @brief Set left path, returns old left path.
 * @param [in] sLeft Left path.
 * @param [in] bReadOnly Will path be recorded read-only?
 */
CString ProjectFile::SetLeft(const CString& sLeft, const BOOL * pReadOnly /*=NULL*/)
{
	CString sLeftOld = GetLeft();
	m_leftFile = sLeft;
	if (pReadOnly)
		m_bLeftReadOnly = *pReadOnly;

	return sLeftOld;
}

/** 
 * @brief Returns right path.
 * @param [out] pReadOnly TRUE if readonly was specified for path.
 */
CString ProjectFile::GetRight(BOOL * pReadOnly /*=NULL*/) const
{
	if (pReadOnly)
		*pReadOnly = m_bRightReadOnly;
	return m_rightFile;
}

/** 
 * @brief Returns if right path is specified read-only.
 */
BOOL ProjectFile::GetRightReadOnly() const
{
	return m_bRightReadOnly;
}

/** 
 * @brief Set right path, returns old right path.
 * @param [in] sRight Right path.
 * @param [in] bReadOnly Will path be recorded read-only?
 */
CString ProjectFile::SetRight(const CString& sRight, const BOOL * pReadOnly /*=NULL*/)
{
	CString sRightOld = GetRight();
	m_rightFile = sRight;
	if (pReadOnly)
		m_bRightReadOnly = *pReadOnly;

	return sRightOld;
}

/** 
 * @brief Returns filter.
 */
CString ProjectFile::GetFilter() const
{
	return m_filter;
}

/** 
 * @brief Set filter, returns old filter.
 */
CString ProjectFile::SetFilter(const CString& sFilter)
{
	CString sFilterOld = GetFilter();
	m_filter = sFilter;

	return sFilterOld;
}

/** 
 * @brief Returns subfolder included -setting.
 */
int ProjectFile::GetSubfolders() const
{
	return m_subfolders;
}

/** 
 * @brief set subfolder, returns old subfolder value.
 */
int ProjectFile::SetSubfolders(const int iSubfolder)
{
	int iSubfoldersOld = GetSubfolders(); 
	m_subfolders = iSubfolder ? 1 : 0;

	return iSubfoldersOld;
}

/** 
 * @brief Returns left and right paths and recursive from project file
 * 
 * @param [out] sLeft Left path
 * @param [out] sRight Right path
 * @param [out] bSubFolders If TRUE subfolders included (recursive compare)
 */
void ProjectFile::GetPaths(CString & sLeft, CString & sRight,
	BOOL & bSubfolders) const
{
	if (HasLeft())
		sLeft = GetLeft();
	if (HasRight())
		sRight = GetRight();
	if (HasSubfolders())
		bSubfolders = (GetSubfolders() == 1);
}
