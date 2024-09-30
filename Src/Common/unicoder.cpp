/**
 *  @file   unicoder.cpp
 *  @author Perry Rapp, Creator, 2003-2004
 *  @date   Created: 2003-10
 *  @date   Edited:  2005-01-28 (Perry Rapp)
 *
 *  @brief  Implementation of utility unicode conversion routines
 */

/* The MIT License
Copyright (c) 2003 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "StdAfx.h"
#include "unicoder.h"
#include "codepage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is not in older platform sdk versions
#ifndef WC_NO_BEST_FIT_CHARS
#define WC_NO_BEST_FIT_CHARS        0x00000400
#endif

namespace ucr {

// current OS version
static bool f_osvi_fetched=false;
static OSVERSIONINFO f_osvi;

/**
 * @brief fetch current OS version into file level variable & set flag
 */
static void
fetch_verinfo()
{
	memset(&f_osvi, 0, sizeof(f_osvi));
	f_osvi.dwOSVersionInfoSize = sizeof(f_osvi);
	GetVersionEx(&f_osvi);
	f_osvi_fetched = true;
}

static LPCTSTR f_unicodesetNames[] = { _T("<NONE>"), _T("UCS-2LE"), _T("UCS-2BE"), _T("UTF-8") };
/**
 * @brief return string for enum value
 */
CString GetUnicodesetName(UNICODESET unicoding)
{
	if (unicoding>=0 && unicoding<sizeof(f_unicodesetNames)/sizeof(f_unicodesetNames[0]))
		return f_unicodesetNames[unicoding];
	else
		return _T("?");
}

/**
 * @brief Convert unicode codepoint to UTF-8 byte string
 *
 * utf8 must be a 7+ byte buffer
 * returns length of byte string written
 * Does not zero-terminate!
 */
int
Ucs4_to_Utf8(UINT unich, unsigned char * utf8)
{
#pragma warning(disable: 4244) // possible loss of data due to type conversion
	if (unich <= 0x7f)
	{
		utf8[0] = (unsigned char)unich;
		return 1;
	}
	else if (unich <= 0x7ff)
	{
		utf8[0] = 0xc0 + (unich >> 6);
		utf8[1] = 0x80 + (unich & 0x3f);
		return 2;
	}
	else if (unich <= 0xffff)
	{
		utf8[0] = 0xe0 + (unich >> 12);
		utf8[1] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[2] = 0x80 + (unich & 0x3f);
		return 3;
	}
	else if (unich <= 0x1fffff)
	{
		utf8[0] = 0xf0 + (unich >> 18);
		utf8[1] = 0x80 + ((unich >> 12) & 0x3f);
		utf8[2] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[3] = 0x80 + (unich & 0x3f);
		return 4;
	}
	else if (unich <= 0x3ffffff)
	{
		utf8[0] = 0xf8 + (unich >> 24);
		utf8[1] = 0x80 + ((unich >> 18) & 0x3f);
		utf8[2] = 0x80 + ((unich >> 12) & 0x3f);
		utf8[3] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[4] = 0x80 + (unich & 0x3f);
		return 5;
	}
	else if (unich <= 0x7fffffff)
	{
		utf8[0] = 0xfc + (unich >> 30);
		utf8[1] = 0x80 + ((unich >> 24) & 0x3f);
		utf8[2] = 0x80 + ((unich >> 18) & 0x3f);
		utf8[3] = 0x80 + ((unich >> 12) & 0x3f);
		utf8[4] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[5] = 0x80 + (unich & 0x3f);
		return 6;
	}
	else
	{
		// Invalid Unicode codepoint (high bit was set)
		// TODO: What do we do ?
		utf8[0] = '?';
		return 1;
	}
#pragma warning(default: 4244) // possible loss of data due to type conversion
}

/**
 * @brief return byte length of UTF-8 character from its initial character (-1 if invalid)
 */
int
Utf8len_fromLeadByte(unsigned char ch)
{
	if (ch < 0x80) return 1;
	if (ch < 0xC0) return -1;
	if (ch < 0xE0) return 2;
	if (ch < 0xF0) return 3;
	if (ch < 0xF8) return 4;
	if (ch < 0xFC) return 5;
	if (ch < 0xFE) return 6;
	return -1;
}

/**
 * @brief return #bytes required to represent Unicode codepoint as UTF-8 
 */
int
Utf8len_fromCodepoint(UINT ch)
{
	if (ch <= 0x7F) return 1;
	if (ch <= 0x7FF) return 2;
	if (ch <= 0xFFFF) return 3;
	if (ch <= 0x1FFFFF) return 4;
	if (ch <= 0x3FFFFFF) return 5;
	if (ch <= 0x7FFFFFFF) return 6;
	return -1;
}

/**
 * @brief How many bytes will it take to write string as UTF-8 ? 
 *
 * @param size size argument as filemapping are not 0 terminated
 *
 * @bug Fails for files larger than 2gigs
 */
UINT
Utf8len_of_string(LPCWSTR text, int size)
{
	UINT len=0;
	for (int i=0; i<size; ++i)
	{
		int chlen = Utf8len_fromCodepoint(text[i]);
		if (chlen < 1) chlen = 1;
		len += chlen;
	}
	return len;
}
/**
 * @brief How many chars in this UTF-8 string ? 
 *
 * @param size size argument as filemapping are not 0 terminated
 *
 * @bug Fails for files larger than 2gigs
 */
UINT
stringlen_of_utf8(LPCSTR text, int size)
{
	UINT len=0;
	for (int i=0; i<size; )
	{
		int chlen = Utf8len_fromLeadByte(text[i]);
		if (chlen < 1) chlen = 1;
		i += chlen;
		len ++;
	}
	return len;
}

/**
 * @brief Read UTF-8 character and return as Unicode
 */
UINT
GetUtf8Char(unsigned char * str)
{
	/* test short cases first, as probably much more common */
	if (!(*str & 0x80 && *str & 0x40)) {
		return str[0];
	}
	if (!(*str & 0x20)) {
		UINT ch = ((str[0] & 0x1F) << 6)
			+ (str[1] & 0x3F);
		return ch;
	}
	if (!(*str & 0x10)) {
		UINT ch = ((str[0] & 0x0f) << 12)
			+ ((str[1] & 0x3F) << 6)
			+ (str[2] & 0x3F);
		return ch;
	}
	if (!(*str & 0x08)) {
		UINT ch = ((str[0] & 0x0F) << 18)
			+ ((str[1] & 0x3F) << 12)
			+ ((str[2] & 0x3F) << 6)
			+ (str[3] & 0x3F);
		return ch;
	}
	if (!(*str & 0x04)) {
		UINT ch = ((str[0] & 0x0F) << 24)
			+ ((str[1] & 0x3F) << 18)
			+ ((str[2] & 0x3F) << 12)
			+ ((str[3] & 0x3F) << 6)
			+ (str[4] & 0x3F);
		return ch;
	} else {
		UINT ch = ((str[0] & 0x0F) << 30)
			+ ((str[1] & 0x3F) << 24)
			+ ((str[2] & 0x3F) << 18)
			+ ((str[3] & 0x3F) << 12)
			+ ((str[4] & 0x3F) << 6)
			+ (str[5] & 0x3F);
		return ch;
	}
}

/**
 * @brief Write unicode codepoint u out as UTF-8 to lpd, and advance lpd
 *
 * Returns number of bytes written (or -1 for error, in which case it writes '?')
 */
int to_utf8_advance(UINT u, unsigned char * &lpd)
{
#pragma warning(disable: 4244) // possible loss of data due to type conversion
	if (u < 0x80)
	{
		*lpd++ = u;
		return 1;
	}
	else if (u < 0x800)
	{
		*lpd++ = 0xC0 + (u >> 6);
		*lpd++ = 0x80 + (u & 0x3F);
		return 2;
	}
	else if (u < 0x10000)
	{
		*lpd++ = 0xE0 + (u >> 12);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 3;
	}
	else if (u < 0x200000)
	{
		*lpd++ = 0xF0 + (u >> 18);
		*lpd++ = 0x80 + ((u >> 12) & 0x3F);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 4;
	}
	else if (u < 0x4000000)
	{
		*lpd++ = 0xF8 + (u >> 24);
		*lpd++ = 0x80 + ((u >> 18) & 0x3F);
		*lpd++ = 0x80 + ((u >> 12) & 0x3F);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 5;
	}
	else if (u < 0x80000000)
	{
		*lpd++ = 0xF8 + (u >> 30);
		*lpd++ = 0x80 + ((u >> 24) & 0x3F);
		*lpd++ = 0x80 + ((u >> 18) & 0x3F);
		*lpd++ = 0x80 + ((u >> 12) & 0x3F);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 6;
	}
	else
	{
		*lpd++ = '?';
		return 1;
	}
#pragma warning(default: 4244) // possible loss of data due to type conversion
}

/**
 * @brief convert character passed (Unicode codepoint) to a TCHAR (set lossy flag if imperfect conversion)
 */
CString maketchar(UINT unich, bool & lossy)
{
	static UINT codepage = CP_ACP;
	// NB: Windows always draws in CP_ACP, not CP_THREAD_ACP, so we must use CP_ACP as an internal codepage

	return maketchar(unich, lossy, codepage);
}

/**
 * @brief convert character passed (Unicode codepoint) to a TCHAR (set lossy flag if imperfect conversion)
 */
CString maketchar(UINT unich, bool & lossy, UINT codepage)
{
#ifdef _UNICODE
	if (unich < 0x10000)
	{
		CString s;
		s = (TCHAR)unich;
		return s;
	}
	lossy = TRUE;
	return '?';
#else
	if (unich < 0x80)
	{
		CString s = (TCHAR)unich;
		return s;
	}
	wchar_t wch = (wchar_t)unich;
	if (!lossy)
	{
		static bool vercheck=false;
		static bool has_no_best_fit=false;
		if (!vercheck)
		{
			if (!f_osvi_fetched) fetch_verinfo();
			// Need 2000 (5.x) or 98 (4.10)
			has_no_best_fit = f_osvi.dwMajorVersion>=5 || (f_osvi.dwMajorVersion==4 && f_osvi.dwMinorVersion>=10);
			vercheck = true;
		}
		// So far it isn't lossy, so try for lossless conversion
		TCHAR outch;
		BOOL defaulted=FALSE;
		DWORD flags = has_no_best_fit ? WC_NO_BEST_FIT_CHARS : 0;
		if (WideCharToMultiByte(codepage, flags, &wch, 1, &outch, 1, NULL, &defaulted)
			&& !defaulted)
		{
			CString s = outch;
			return s;
		}
		lossy = TRUE;
	}
	// already lossy, so make our best shot
	DWORD flags = WC_COMPOSITECHECK+WC_DISCARDNS+WC_SEPCHARS+WC_DEFAULTCHAR;
	TCHAR outbuff[16];
	int n = WideCharToMultiByte(codepage, flags, &wch, 1, outbuff, sizeof(outbuff)-1, NULL, NULL);
	if (n>0)
	{
		outbuff[n] =0;
		return outbuff;
	}
	return _T("?");
#endif
}

/**
 * @brief convert 8-bit character input to Unicode codepoint and return it
 */
UINT
byteToUnicode (unsigned char ch)
{
	static UINT codepage = CP_ACP;
	// NB: Windows always draws in CP_ACP, not CP_THREAD_ACP, so we must use CP_ACP as an internal codepage

	return byteToUnicode(ch, codepage);
}

/**
 * @brief convert 8-bit character input to Unicode codepoint and return it
 */
UINT
byteToUnicode (unsigned char ch, UINT codepage)
{

	if (ch < 0x80)
		return ch;

	DWORD flags = 0;
	wchar_t wbuff;
	int n = MultiByteToWideChar(codepage, flags, (LPCSTR)&ch, 1, &wbuff, 1);
	if (n>0)
		return wbuff;
	else
		return '?';
}

/**
 * @brief Return encoding used for TCHAR & CString
 */
void getInternalEncoding(UNICODESET * unicoding, int * codepage)
{
#ifdef _UNICODE
	*unicoding = UCS2LE;
	*codepage = 0;
#else
	// NB: Windows always draws in CP_ACP, not CP_THREAD_ACP, so we must use CP_ACP as an internal codepage
	*unicoding = NONE;
	*codepage = CP_ACP;
#endif
}

/**
 * @brief Write appropriate BOM (Unicode byte order marker)
 * returns #bytes written
 */
int
writeBom(LPVOID dest, UNICODESET unicoding)
{
	unsigned char * lpd = reinterpret_cast<unsigned char *>(dest);
	// write Unicode byte order marker (BOM)
	if (unicoding == UCS2LE)
	{
		*lpd++ = 0xFF;
		*lpd++ = 0xFE;
		return 2;
	}
	else if (unicoding == UCS2BE)
	{
		*lpd++ = 0xFE;
		*lpd++ = 0xFF;
		return 2;
	}
	else if (unicoding == UTF8)
	{
		*lpd++ = 0xEF;
		*lpd++ = 0xBB;
		*lpd++ = 0xBF;
		return 3;
	}
	return 0;
}

/**
 * @brief Extract character from pointer, handling UCS-2 codesets
 *  This does not handle MBCS or UTF-8 codepages correctly!
 *  Client should not use this except for Unicode or SBCS codepages.
 */
UINT
get_unicode_char(unsigned char * ptr, UNICODESET codeset, int codepage)
{
	UINT ch;
	switch (codeset)
	{
	case UCS2LE:
		ch = *((WORD *)ptr);
		break;
	case UCS2BE:
		ch = (ptr[0] << 8) + ptr[1];
		break;
	default:
		// TODO: How do we recognize valid codepage ?
		// if not, use byteToUnicode(*ptr)
		ch = byteToUnicode(*ptr, codepage);
	}
	return ch;
}

/**
 * @brief Convert series of bytes (8-bit chars) to TCHARs, using specified codepage
 *
 * TODO: This doesn't inform the caller whether translation was lossy
 *  In fact, this doesn't even know. Probably going to have to make
 *  two passes, the first with MB_ERR_INVALID_CHARS. Ugh. :(
 */
CString maketstring(LPCSTR lpd, UINT len, int codepage, bool * lossy)
{
	static int defcodepage = getDefaultCodepage();

	if (!len) return _T("");

	// NO ! codepage = 0 is the value of CP_ACP !
	// if (!codepage)
	// 	codepage = defcodepage;

#ifdef UNICODE
	// Convert input to Unicode, using specified codepage
	// TCHAR is wchar_t, so convert into CString (str)
	CString str;
	DWORD flags = 0;
	int wlen = len*2+6;
	LPWSTR wbuff = str.GetBuffer(wlen);
	int n = MultiByteToWideChar(codepage, flags, (LPCSTR)lpd, len, wbuff, wlen-1);
	if (n)
	{
		str.ReleaseBuffer(n);
	}
	else
	{
		str = _T("?");
	}
	return str;

#else
	if (EqualCodepages(codepage, getDefaultCodepage()))
	{
		// trivial case, they want the bytes in the file interpreted in our current codepage
		// Only caveat is that input (lpd) is not zero-terminated
		CString str;
		LPTSTR strbuff = str.GetBuffer(len+1);
		_tcsncpy(strbuff, lpd, len);
		strbuff[len] = 0; // Cannot call str.SetAt(...) until after ReleaseBuffer call
		str.ReleaseBuffer();
		return str;
	}

	CString str = CrossConvertToStringA(lpd, len, codepage, defcodepage, lossy);
	return str;
#endif
}

/**
 * @brief (ANSI build only) Convert from one 8 bit codepage to another
 */
#ifndef UNICODE
CString
CrossConvertToStringA(LPCSTR src, UINT srclen, int cpin, int cpout, bool * lossy)
{

	CString str;
	int wlen = srclen*2+6;
	int clen = wlen * 2 + 6;
	LPSTR cbuff = str.GetBuffer(clen);
	int nbytes = CrossConvert(src, srclen, cbuff, clen, cpin, cpout, lossy);
	str.ReleaseBuffer(nbytes);
	return str;
}
#endif

/**
 * @brief Convert from one 8-bit codepage to another
 *
 * destsize must be at least 2
 */
int
CrossConvert(LPCSTR src, UINT srclen, LPSTR dest, UINT destsize, int cpin, int cpout, bool * lossy)
{
	ASSERT(destsize > 1);

	// Convert input to Unicode, using specified codepage
	DWORD flags = 0;
	int wlen = srclen*2+6;
	wchar_t * wbuff = new wchar_t[wlen];
	int n = MultiByteToWideChar(cpin, flags, (LPCSTR)src, srclen, wbuff, wlen-1);
	if (!n)
	{
		int nsyserr = ::GetLastError();
		CString syserrstr = GetSysError(nsyserr);
		delete [] wbuff;
		dest[0] = '?';
		return 1;
	}
	wbuff[n] = 0; // zero-terminate string

	// Now convert to TCHAR (which means defcodepage)
	flags = WC_NO_BEST_FIT_CHARS; // TODO: Think about this
	wlen = n;
	int clen = wlen * 2 + 6;
	BOOL defaulted=FALSE;
	BOOL * pdefaulted = &defaulted;
	if (cpout == CP_UTF8)
	{
		flags = 0;
		pdefaulted = NULL;
	}
	n = WideCharToMultiByte(cpout, flags, wbuff, n, dest, destsize-1, NULL, pdefaulted);
	if (!n)
	{
		int nsyserr = ::GetLastError();
		CString syserrstr = GetSysError(nsyserr);
	}
	dest[n] = 0;
	delete [] wbuff;
	if (lossy)
		*lossy = !!defaulted;
	return n;
}

buffer::buffer(unsigned int needed)
{
	used = 0;
	size = needed;
	ptr = (unsigned char *)calloc(size, 1);
}
buffer::~buffer()
{
	free(ptr);
}
void buffer::resize(unsigned int needed)
{
	if (size < needed)
	{
		size = needed;
		ptr = (unsigned char *)realloc(ptr, size);
	}
}

/**
 * @brief Convert from one text encoding to another; return false if any lossing conversions
 */
bool convert(UNICODESET unicoding1, int codepage1, const unsigned char * src, int srcbytes, UNICODESET unicoding2, int codepage2, buffer * dest)
{
	if (unicoding1 == unicoding2 && (unicoding1 || EqualCodepages(codepage1, codepage2)))
	{
		// simple byte copy
		dest->resize(srcbytes);
		CopyMemory(dest->ptr, src, srcbytes);
		dest->used = srcbytes;
		return true;
	}
	if ((unicoding1 == UCS2LE && unicoding2 == UCS2BE)
		|| (unicoding1 == UCS2BE && unicoding2 == UCS2LE))
	{
		// simple byte swap
		dest->resize(srcbytes);
		for (int i=0; i<srcbytes; i += 2)
		{
			// Byte-swap into destination
			dest->ptr[i] = src[i+1];
			dest->ptr[i+1] = src[i];
		}
		dest->used = srcbytes;
		return true;
	}
	if (unicoding1 != UCS2LE && unicoding2 != UCS2LE)
	{
		// Break problem into two simpler pieces by converting through UCS-2LE
		buffer intermed(dest->size);
		bool step1 = convert(unicoding1, codepage1, src, srcbytes, UCS2LE, 0, &intermed);
		bool step2 = convert(UCS2LE, 0, intermed.ptr, intermed.used, unicoding2, codepage2, dest);
		return step1 && step2;
	}
	if (unicoding1 == UCS2LE)
	{
		// From UCS-2LE to 8-bit (or UTF-8)

		// WideCharToMultiByte: lpDefaultChar & lpUsedDefaultChar must be NULL when using UTF-8

		int destcp = (unicoding2 == UTF8 ? CP_UTF8 : codepage2);
		DWORD flags = 0;
		int bytes = WideCharToMultiByte(destcp, flags, (LPCWSTR)src, srcbytes/2, 0, 0, NULL, NULL);
		dest->resize(bytes);
		int losses = 0;
		bytes = WideCharToMultiByte(destcp, flags, (LPCWSTR)src, srcbytes/2, (char *)dest->ptr, dest->size, NULL, NULL);
		dest->used = bytes;
		return losses==0;
	}
	else
	{
		// From 8-bit (or UTF-8) to UCS-2LE
		int srccp = (unicoding1 == UTF8 ? CP_UTF8 : codepage1);
		DWORD flags = 0;
		int wchars = MultiByteToWideChar(srccp, flags, (LPCSTR)src, srcbytes, 0, 0);
		dest->resize(wchars*2);
		wchars = MultiByteToWideChar(srccp, flags, (LPCSTR)src, srcbytes, (LPWSTR)dest->ptr, dest->size/2);
		dest->used = wchars * 2;
		return true;
	}
}

} // namespace ucr

/**
 * @brief Change any special codepage constants into real codepage numbers
 */
static int NormalizeCodepage(int cp)
{
	if (cp == CP_THREAD_ACP) // should only happen on Win2000+
	{
		TCHAR buff[32];
		if (GetLocaleInfo(GetThreadLocale(), LOCALE_IDEFAULTANSICODEPAGE, buff, sizeof(buff)/sizeof(buff[0])))
			cp = _ttol(buff);
		else
			// a valid codepage is better than no codepage
			cp = GetACP();
	}
	if (cp == CP_ACP) cp = GetACP();
	if (cp == CP_OEMCP) cp = GetOEMCP();
	return cp;
}

/**
 * @brief Compare two codepages for equality
 */
bool EqualCodepages(int cp1, int cp2)
{
	return (cp1 == cp2)
		|| (NormalizeCodepage(cp1) == NormalizeCodepage(cp2));
}

