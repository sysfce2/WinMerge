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
 * @file  CompareStatisticsDlg.h
 *
 * @brief Declaration file for CompareStatisticsDlg dialog
 */
// RCS ID line follows -- this is updated by CVS
// $Id: CompareStatisticsDlg.h 4596 2007-10-07 09:44:06Z jtuc $

#ifndef _COMPARESTATISTICSDLG_H_
#define _COMPARESTATISTICSDLG_H_

class CompareStats;

class CompareStatisticsDlg : public CDialog
{
	DECLARE_DYNAMIC(CompareStatisticsDlg)

public:
	CompareStatisticsDlg(CWnd* pParent = NULL);   // standard constructor
	void SetCompareStats(const CompareStats * pStats);

// Dialog Data
	//{{AFX_DATA(CompareStatisticsDlg)
	enum { IDD = IDD_COMPARE_STATISTICS };
	//}}AFX_DATA

protected:
	// Generated message map functions
	//{{AFX_MSG(CompareStatisticsDlg)
	afx_msg BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation data
private:
	const CompareStats * m_pCompareStats;
};

#endif // _COMPARESTATISTICSDLG_H_
