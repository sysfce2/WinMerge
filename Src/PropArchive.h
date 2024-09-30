/** 
 * @file  PropArchive.h
 *
 * @brief Declaration file for CPropArchive propertyheet
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: PropArchive.h 3126 2006-03-04 02:36:46Z elsapo $

#ifndef _PROP_ARCHIVE_H_
#define _PROP_ARCHIVE_H_

#include "StatLink.h"

#ifndef _IOPTIONSPANEL_H_
#include "IOptionsPanel.h"
#endif

class COptionsMgr;

/** 
 * @brief Class for Archive Support options page.
 */
class CPropArchive : public CPropertyPage
{
public:
	CPropArchive(COptionsMgr *optionsMgr);
	virtual ~CPropArchive();

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	enum { IDD = IDD_PROP_ARCHIVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void UpdateControls();

	afx_msg void OnEnableClicked();

	DECLARE_MESSAGE_MAP()
public:
	CStaticLink m_wwwLink;
	BOOL m_bEnableSupport;
	int m_nInstallType;
	BOOL m_bProbeType;

private:
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};
#endif // _PROP_ARCHIVE_H_
