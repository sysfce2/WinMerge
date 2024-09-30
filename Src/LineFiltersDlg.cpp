/**
 *  @file LineFiltersDlg.cpp
 *
 *  @brief Implementation of Line Filter dialog
 */ 
// ID line follows -- this is updated by SVN
// $Id: LineFiltersDlg.cpp 5746 2008-08-06 17:48:26Z kimmov $

#include "stdafx.h"
#include "merge.h"
#include "LineFiltersList.h"
#include "MainFrm.h"
#include "LineFiltersDlg.h"
#include "dllver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief Location for file compare specific help to open. */
static TCHAR FilterHelpLocation[] = _T("::/htmlhelp/Filters.html");

/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter property page

IMPLEMENT_DYNAMIC(LineFiltersDlg, CPropertyPage)

/**
 * @brief Constructor.
 */
LineFiltersDlg::LineFiltersDlg()
: CPropertyPage(LineFiltersDlg::IDD)
, m_pList(NULL)
, m_bEditing(FALSE)
, m_editedIndex(-1)
{
	//{{AFX_DATA_INIT(LineFiltersDlg)
	m_bIgnoreRegExp = FALSE;
	//}}AFX_DATA_INIT
	m_strCaption = theApp.LoadDialogCaption(m_lpszTemplateName).c_str();
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
	m_psp.hIcon = AfxGetApp()->LoadIcon(IDI_LINEFILTER);
	m_psp.dwFlags |= PSP_USEHICON;
}

void LineFiltersDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LineFiltersDlg)
	DDX_Check(pDX, IDC_IGNOREREGEXP, m_bIgnoreRegExp);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LFILTER_LIST, m_filtersList);
	DDX_Control(pDX, IDC_LFILTER_EDITBOX, m_editRegexp);
	DDX_Control(pDX, IDC_LFILTER_EDITSAVE, m_saveRegexp);
}


BEGIN_MESSAGE_MAP(LineFiltersDlg, CPropertyPage)
	//{{AFX_MSG_MAP(LineFiltersDlg)
	ON_BN_CLICKED(IDC_IGNOREREGEXP, OnIgnoreregexp)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LFILTER_ADDBTN, OnBnClickedLfilterAddBtn)
	ON_BN_CLICKED(IDC_LFILTER_EDITBTN, OnBnClickedLfilterEditbtn)
	ON_BN_CLICKED(IDC_LFILTER_REMOVEBTN, OnBnClickedLfilterRemovebtn)
	ON_BN_CLICKED(IDC_LFILTER_EDITSAVE, OnBnClickedLfilterEditsave)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LFILTER_LIST, OnLvnItemActivateLfilterList)
	ON_EN_KILLFOCUS(IDC_LFILTER_EDITBOX, OnEnKillfocusLfilterEditbox)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter message handlers

/**
 * @brief Initialize the dialog.
 */
BOOL LineFiltersDlg::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	InitList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Initialize the filter list in the dialog.
 * This function adds current line filters to the filter list.
 */
void LineFiltersDlg::InitList()
{
	// Show selection across entire row.
	DWORD newstyle = LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT |
		LVS_EX_ONECLICKACTIVATE;
	// Also enable infotips if they have new enough version for our
	// custom draw code
	// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
	if (GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(4,71))
		newstyle |= LVS_EX_INFOTIP;

	m_filtersList.ModifyStyle(0, LVS_EDITLABELS | LVS_SHOWSELALWAYS);

	m_filtersList.SetExtendedStyle(m_filtersList.GetExtendedStyle() | newstyle);

	String title = theApp.LoadString(IDS_FILTERLINE_REGEXP);
	m_filtersList.InsertColumn(1, title.c_str(), LVCFMT_LEFT, 500);

	int count = m_pList->GetCount();
	int ind = -1;
	for (int i = 0; i < count; i++)
	{
		const LineFilterItem &item = m_pList->GetAt(i);
		ind = AddRow(item.filterStr.c_str(), item.enabled);
	}
	if (count > 0)
	{
		m_filtersList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		BOOL bPartialOk = FALSE;
		m_filtersList.EnsureVisible(0, bPartialOk);
		CString text = m_filtersList.GetItemText(0, 0);
		m_editRegexp.SetWindowText(text);
	}
	UpdateData(FALSE);
}

/**
 * @brief User clicked filter enable/disable checkbox.
 */
void LineFiltersDlg::OnIgnoreregexp() 
{
	UpdateData();
}

/**
 * @brief Open help from mainframe when user presses F1.
 */
void LineFiltersDlg::OnHelp()
{
	GetMainFrame()->ShowHelp(FilterHelpLocation);
}

/**
 * @brief Add new row to the list control.
 * @param [in] Filter string to add.
 * @param [in] enabled Is filter enabled?
 * @return Index of added row.
 */
int LineFiltersDlg::AddRow(LPCTSTR filter /*= NULL*/, BOOL enabled /*=FALSE*/)
{
	int items = m_filtersList.GetItemCount();
	int ind = m_filtersList.InsertItem(items + 1, filter);
	m_filtersList.SetCheck(ind, enabled);
	return ind;
}

/**
 * @brief Edit currently selected filter.
 */
void LineFiltersDlg::EditSelectedFilter()
{
	int sel =- 1;
	m_filtersList.SetFocus();
	sel = m_filtersList.GetNextItem(sel, LVNI_SELECTED);
	if (sel > -1)
	{
		CString text = m_filtersList.GetItemText(sel, 0);
		m_editRegexp.SetWindowText(text);
		m_editRegexp.SetReadOnly(FALSE);
		m_saveRegexp.EnableWindow(TRUE);
		m_editRegexp.SetFocus();
		m_editRegexp.SetSel(0, -1);
		m_bEditing = TRUE;
		m_editedIndex = sel;
	}
}

/**
 * @brief Called when Add-button is clicked.
 */
void LineFiltersDlg::OnBnClickedLfilterAddBtn()
{
	int ind = AddRow(_T(""));
	if (ind >= -1)
	{
		m_filtersList.SetItemState(ind, LVIS_SELECTED, LVIS_SELECTED);
		BOOL bPartialOk = FALSE;
		m_filtersList.EnsureVisible(ind, bPartialOk);
		EditSelectedFilter();
	}
}

/**
 * @brief Called when Edit button is clicked.
 */
void LineFiltersDlg::OnBnClickedLfilterEditbtn()
{
	EditSelectedFilter();
}

/**
 * @brief Save filters to list when exiting the dialog.
 */
void LineFiltersDlg::OnOK()
{
	m_pList->Empty();

	for (int i = 0; i < m_filtersList.GetItemCount(); i++)
	{
		CString text = m_filtersList.GetItemText(i, 0);
		BOOL enabled = m_filtersList.GetCheck(i);

		m_pList->AddFilter(text, enabled);
	}

	CPropertyPage::OnClose();
}

/**
 * @brief Sets external filter list.
 * @param [in] list External filter list.
 */
void LineFiltersDlg::SetList(LineFiltersList * list)
{
	m_pList = list;
}

/**
 * @brief Called when Remove button is clicked.
 */
void LineFiltersDlg::OnBnClickedLfilterRemovebtn()
{
	int sel =- 1;
	sel = m_filtersList.GetNextItem(sel, LVNI_SELECTED);
	if (sel != -1)
	{
		m_filtersList.DeleteItem(sel);
	}

	int newSel = min(m_filtersList.GetItemCount() - 1, sel);
	if (newSel >= -1)
	{
		m_filtersList.SetItemState(newSel, LVIS_SELECTED, LVIS_SELECTED);
		BOOL bPartialOk = FALSE;
		m_filtersList.EnsureVisible(newSel, bPartialOk);
	}
}

/**
 * @brief Called when Save button is clicked.
 */
void LineFiltersDlg::OnBnClickedLfilterEditsave()
{
	SaveItem();
}

/**
 * @brief Cancel editing of filter when ESC is pressed.
 */
BOOL LineFiltersDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_bEditing)
	{
		// Handle Esc key press
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		{
			m_saveRegexp.EnableWindow(FALSE);
			m_editRegexp.SetReadOnly(TRUE);
			m_bEditing = FALSE;
			m_filtersList.SetFocus();
			return TRUE;
		}
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}

/**
 * @brief Called when selected item in list changes.
 */
void LineFiltersDlg::OnLvnItemActivateLfilterList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int item = pNMIA->iItem;
	if (m_bEditing)
	{
		m_saveRegexp.EnableWindow(FALSE);
		m_editRegexp.SetReadOnly(TRUE);
		m_bEditing = FALSE;
	}
	CString text = m_filtersList.GetItemText(item, 0);
	m_editRegexp.SetWindowText(text);
	*pResult = 0;
}

/**
 * @brief Save the current filter in edit box.
 */
void LineFiltersDlg::SaveItem()
{
	if (m_bEditing)
	{
		CString text;
		m_editRegexp.GetWindowText(text);
		m_filtersList.SetItemText(m_editedIndex, 0, text);
		m_bEditing = FALSE;
		m_editRegexp.SetReadOnly(TRUE);
		m_saveRegexp.EnableWindow(FALSE);
		m_editedIndex = -1;
	}
}

/**
 * @brief Called when filter edit box loses its focus.
 */
void LineFiltersDlg::OnEnKillfocusLfilterEditbox()
{
	SaveItem();
}
