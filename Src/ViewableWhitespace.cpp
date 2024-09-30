#include "StdAfx.h" 
#include "ViewableWhitespace.h"

static bool f_initialized=false;

#ifdef UNICODE
// For UNICODE build, there is just one set which is always used
// tab, space, cr, lf, eol
struct ViewableWhitespaceChars
// Do not use L literals, as they involve runtime mbcs expansion, apparently
 f_specialChars = {
	0
	, L" " // U+BB: RIGHT POINTING DOUBLE ANGLE QUOTATION MARK
	, L" " // U+B7: MIDDLE DOT
	, L" " // U+A7: SECTION SIGN
	, L" " // U+B6: PILCROW SIGN
	, L" " // U+A4: CURRENCY SIGN
};
static void initialize()
{
	f_specialChars.c_tab[0] = 0xBB;
	f_specialChars.c_space[0] = 0xB7;
	f_specialChars.c_cr[0] = 0xA7;
	f_specialChars.c_lf[0] = 0xB6;
	f_specialChars.c_eol[0] = 0xA4;

	f_initialized = true;
}

#else
// For ANSI build, there are various sets for different codepages
static CMap<int, int, int, int> f_offset; // map codepage to offset

// tab, space, cr, lf, eol
static struct ViewableWhitespaceChars
 f_specialChars[] = {
	 { 0, ">", ".", "$", "!", "&" } // default all-ASCII for unhandled codepages
	, { 850, "\xAF", "\xFA", "\xF5", "\xF4", "\xCF" } // CP-850 MSDOS Multilingual

	// CP-932 Windows version of Japanese ShiftJIS
	// \x81\x98 = u+A7
	// \x81\xF7 = u+B6
	, { 932, ">", ".", "\x81\x98", "\x81\xF7", "&" } // CP-932 Japanese

	// CP-949 Korean
	// \xA1\xA4 = u+B7
	// \xA1\xD7 = u+A7
	// \xA2\xD2 = u+B6
	// \xA2\xB4 = u+A4
	, { 949, ">", "\xA1\xA4", "\xA1\xD7", "\xA2\xD2", "\xA2\xB4" } // CP-949 Korean

	// CP-950 Windows version of Big5 Chinese Traditional
	// \xA1\x6D = u+300A
	// \xA1\x50 = u+B7
	// \xA1\xB1 = u+A7
	// \xA2\x54 = ?
	// \xA1\x3B = u+33A1
	, { 951, "\xA1\x6D", "\xA1\x50", "\xA1\xB1", "\xA2\x54", "\xA1\x3B" } // CP-950 Chinese Traditional

	, { 1250, "\xBB", "\xB7", "\xA7", "\xB6", "\xA4" } // CP-1250 Central Europe
	, { 1251, "\xBB", "\xB7", "\xA7", "\xB6", "\xA4" } // CP-1251 Windows Cyrillic
	, { 1252, "\xBB", "\xB7", "\xA7", "\xB6", "\xA4" } // CP-1252 Western Europe
	, { 1253, "\xBB", "\xB7", "\xA7", "\xB6", "\xA4" } // CP-1253 Windows Greek
	, { 1254, "\xBB", "\xB7", "\xA7", "\xB6", "\xA4" } // CP-1254 Windows Turkish
	, { 1255, "\xBB", "\xB7", "\xA7", "\xB6", "\xA5" } // CP-1255 Windows Hebrew
	, { 1256, "\xBB", "\xB7", "\xA7", "\xB6", "\xA4" } // CP-1256 Windows Arabic
	, { 1257, "\xBB", "\xB7", "\xA7", "\xB6", "\xA4" } // CP-1257 Windows Baltic Rim
};

static void initialize()
{
	for (int i=0; i<sizeof(f_specialChars)/sizeof(f_specialChars[0]); ++i)
	{
		int codepage = f_specialChars[i].c_codepage;
		f_offset[codepage] = i;
	}

	f_initialized = true;
}

#endif


const ViewableWhitespaceChars * GetViewableWhitespaceChars(int codepage)
{
	if (!f_initialized)
		initialize();
#ifdef UNICODE
	return &f_specialChars;
#else
	// Use the [0] version by default, if lookup fails to find a better one
	int offset = 0;
	if (f_offset.Lookup(codepage, offset))
	{
		int d=9;
	}
	return &f_specialChars[offset];
#endif
}
