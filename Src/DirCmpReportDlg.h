/** 
 * @file  DirCmpReportDlg.h
 *
 * @brief Declaration file for DirCmpReport Dialog.
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DirCmpReportDlg.h,v 1.1 2005/07/25 11:48:20 kimmov Exp $

#ifndef _DIRCMPREPORTDLG_H_
#define _DIRCMPREPORTDLG_H_

#include "SuperComboBox.h"

// DirCmpReportDlg dialog

class DirCmpReportDlg : public CDialog
{
	DECLARE_DYNAMIC(DirCmpReportDlg)

public:
	DirCmpReportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~DirCmpReportDlg();

// Dialog Data
	enum { IDD = IDD_DIRCMP_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnClickReportBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CSuperComboBox m_ctlReportFile; /**< Report filename control */
	CString m_sReportFile; /**< Report filename string */
	CComboBox m_ctlStyle; /**< Report type control */
	int m_nReportType; /**< Report type integer */
};

#endif // _DIRCMPREPORTDLG_H_
