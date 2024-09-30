/** 
 * @file  SyntaxColors.cpp
 *
 * @brief Implementation for SyntaxColors class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id: SyntaxColors.cpp 4167 2007-03-16 22:05:46Z kimmov $

#include "stdafx.h"
#include "SyntaxColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

static const TCHAR DefColorsPath[] =_T("DefaultSyntaxColors");
static const TCHAR Section[] = _T("Custom Colors");

/**
 * @brief Constructor, initialise with default colors.
 */
SyntaxColors::SyntaxColors()
: m_pOptionsMgr(NULL)
{
	m_colors.SetSize(COLORINDEX_COUNT);
	m_bolds.SetSize(COLORINDEX_COUNT);
	SetDefaults();
}

/**
 * @brief Copy constructor.
 */
SyntaxColors::SyntaxColors(SyntaxColors *pColors)
: m_pOptionsMgr(NULL)
{
	m_colors.SetSize(COLORINDEX_COUNT);
	m_bolds.SetSize(COLORINDEX_COUNT);
	m_colors.Copy(pColors->m_colors);
	m_bolds.Copy(pColors->m_bolds);
}

/**
 * @brief Copy colors from another instance.
 */
void SyntaxColors::Clone(SyntaxColors *pColors)
{
	m_colors.SetSize(COLORINDEX_COUNT);
	m_bolds.SetSize(COLORINDEX_COUNT);
	m_colors.Copy(pColors->m_colors);
	m_bolds.Copy(pColors->m_bolds);
}

bool SyntaxColors::IsThemeableColorIndex(int nColorIndex) const
{
	int temp=0;
	return GetSystemColorIndex(nColorIndex, &temp);
}

/**
 * @brief Get system color for this index (if this varies with theme)
 */
bool SyntaxColors::GetSystemColorIndex(int nColorIndex, int * pSysIndex) const
{
	switch (nColorIndex)
	{
		case COLORINDEX_WHITESPACE :
		case COLORINDEX_BKGND:
			*pSysIndex = COLOR_WINDOW;
			return true;
		case COLORINDEX_NORMALTEXT:
			*pSysIndex = COLOR_WINDOWTEXT;
			return true;
		case COLORINDEX_SELMARGIN:
			*pSysIndex = COLOR_SCROLLBAR;
			return true;
	}
	return false;
}

/**
 * @brief Set default color values.
 */
void SyntaxColors::SetDefaults()
{
	for (int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		COLORREF color;

		int nSysIndex = 0;
		if (GetSystemColorIndex(i, &nSysIndex))
		{
			// Colors that vary with Windows theme
			color = GetSysColor(nSysIndex);
		}
		else switch (i)
		{
			// Theme colors are handled above by GetSystemColorIndex
			//
			// COLORINDEX_WHITESPACE
			// COLORINDEX_BKGND:
			// COLORINDEX_NORMALTEXT:
			// COLORINDEX_SELMARGIN:

			// Hardcoded defaults
		case COLORINDEX_PREPROCESSOR:
			color = RGB (0, 128, 192);
			break;
		case COLORINDEX_COMMENT:
		//      return RGB(128, 128, 128);
			color = RGB (0, 128, 0);
			break;
		case COLORINDEX_NUMBER:
		//      return RGB(0x80, 0x00, 0x00);
			color = RGB (0xff, 0x00, 0x00);
			break;
		case COLORINDEX_OPERATOR:
		//      return RGB(0x00, 0x00, 0x00);
			color = RGB (96, 96, 96);
			break;
		case COLORINDEX_KEYWORD:
			color = RGB (0, 0, 255);
			break;
		case COLORINDEX_FUNCNAME:
			color = RGB (128, 0, 128);
			break;
		case COLORINDEX_USER1:
			color = RGB (0, 0, 128);
			break;
		case COLORINDEX_USER2:
			color = RGB (0, 128, 192);
			break;
		case COLORINDEX_SELBKGND:
			color = RGB (0, 0, 0);
			break;
		case COLORINDEX_SELTEXT:
			color = RGB (255, 255, 255);
			break;
		case COLORINDEX_HIGHLIGHTBKGND1:
			color = RGB (255, 160, 160);
			break;
		case COLORINDEX_HIGHLIGHTTEXT1:
			color = RGB (0, 0, 0);
			break;
		case COLORINDEX_HIGHLIGHTBKGND2:
			color = RGB (255, 255, 0);
			break;
		case COLORINDEX_HIGHLIGHTTEXT2:
			color = RGB (0, 0, 0);
			break;
		default:
		//  return RGB(255, 0, 0);
			color = RGB (128, 0, 0);
			break;
		}
		m_colors.SetAt(i, color);

		BOOL bBold = FALSE;
		if (i == COLORINDEX_KEYWORD)
			bBold = TRUE;
		m_bolds.SetAt(i, (int)bBold);
	}
}

/**
 * @brief Set color value
 * @param [in] Index index of color to set (COLORINDEX).
 * @param [in] color New color value.
 */
void SyntaxColors::SetColor(UINT index, COLORREF color)
{
	m_colors.SetAt(index, color);
}

/**
 * @brief Set bold value.
 * @param [in] Index index of color to set (COLORINDEX).
 * @param [in] bold If TRUE bold is enabled.
 */
void SyntaxColors::SetBold(UINT index, BOOL bold)
{
	m_bolds.SetAt(index, bold);
}

/**
 * @brief Initialize color table.
 * @param [in] pOptionsMgr pointer to OptionsMgr used as storage.
 */
void SyntaxColors::Initialize(COptionsMgr *pOptionsMgr)
{
	ASSERT(pOptionsMgr);
	CString valuename(DefColorsPath);

	m_pOptionsMgr = pOptionsMgr;

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	m_pOptionsMgr->InitOption(valuename, count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		// Since we want to initialize with default colors (already
		// set to array, we must first call OptionsMrg->InitOption()
		// with default value. And since InitOption() reads stored value
		// from storage we must set that value to array we use.
		int color = 0;
		COLORREF ref;
		valuename.Format(_T("%s/Color%02u"), DefColorsPath, i);
		color = m_colors.GetAt(i);

		// Special handling for themable colors
		// These are text colors which by default follow the current system colors
		// unless the user has overridden this behavior to specify them explicitly
		bool serializable = true;
		if (IsThemeableColorIndex(i))
		{
			if (m_pOptionsMgr->GetBool(OPT_CLR_DEFAULT_TEXT_COLORING))
				serializable = false;
		}
		m_pOptionsMgr->InitOption(valuename, color, serializable);
		color = m_pOptionsMgr->GetInt(valuename);
		ref = color;
		m_colors.SetAt(i, ref);
	
		int nBold = 0;
		BOOL bBold = FALSE;
		valuename.Format(_T("%s/Bold%02u"), DefColorsPath, i);
		bBold = m_bolds.GetAt(i);
		m_pOptionsMgr->InitOption(valuename, (int) bBold);
		nBold = m_pOptionsMgr->GetInt(valuename);
		bBold = nBold ? TRUE : FALSE;
		m_bolds.SetAt(i, bBold);
	}

}

/**
 * @brief Save color values to storage
 */
void SyntaxColors::SaveToRegistry()
{
	ASSERT(m_pOptionsMgr);
	CString valuename(DefColorsPath);

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	m_pOptionsMgr->SetInt(valuename, count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		valuename.Format(_T("%s/Color%02u"), DefColorsPath, i);
		int color = m_colors.GetAt(i);
		m_pOptionsMgr->SetInt(valuename, color);
		valuename.Format(_T("%s/Bold%02u"), DefColorsPath, i);
		BOOL bold = m_bolds.GetAt(i);
		m_pOptionsMgr->SetInt(valuename, bold);
	}
}

void SyntaxColors_Load(COLORREF * colors, int count)
{
	for (int i = 0; i < count; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		colors[i] = ::AfxGetApp()->GetProfileInt(Section,
			sEntry, RGB(255, 255, 255));
	}
}

void SyntaxColors_Save(COLORREF * colors, int count)
{
	for (int i = 0; i < count; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		if (colors[i] == RGB(255, 255, 255))
			::AfxGetApp()->WriteProfileString(Section, sEntry, NULL);
		else 
			::AfxGetApp()->WriteProfileInt(Section, sEntry, colors[i]);
	}
}

