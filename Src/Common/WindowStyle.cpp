/** 
 * @file  WindowStyle.cpp
 *
 * @brief Utilities (trivial) for manipulating window style
 */
// RCS ID line follows -- this is updated by CVS
// $Id: WindowStyle.cpp 2177 2005-03-07 13:47:22Z elsapo $

#include "StdAfx.h"
#include "WindowStyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * @brief Add requested style(s) to windows current style
 */
void
WindowStyle_Add(CWnd * pwnd, DWORD newStyle)
{
	DWORD removeStyle = 0;
	pwnd->ModifyStyle(removeStyle, newStyle, 0);
}
