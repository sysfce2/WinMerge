/** 
 * @file OptionsMgr.cpp
 *
 * @brief Implementation of Options management classes
 *
 */
// ID line follows -- this is updated by SVN
// $Id: OptionsMgr.cpp 5182 2008-03-23 22:16:57Z sdottaka $


/* The MIT License
Copyright (c) 2004-2008 Kimmo Varis
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdafx.h"
#include <algorithm>
#include "UnicodeString.h"
#include "varprop.h"
#include "OptionsMgr.h"

static bool GetInt(LPCTSTR str, int & val);

/**
 * @brief Default constructor.
 */
COption::COption()
{
}

/**
 * @brief Copy constructor.
 * @param [in] option Object to copy.
 */
COption::COption(const COption& option)
{
	m_strName = option.m_strName;
	m_value = option.m_value;
	m_valueDef = option.m_valueDef;
}

/**
 * @brief Assignment operator override.
 * @param [in] option Object to copy.
 * @return Copy of given object.
 */
COption& COption::operator=(const COption& option)
{
	if (this != &option)
	{
		m_strName = option.m_strName;
		m_value = option.m_value;
		m_valueDef = option.m_valueDef;
	}
	return *this;
}

/**
 * @brief Set name, value and default value for option
 * @param [in] name Name of option with full path ("Settings/AutomaticRescan")
 * @param [in] defaultValue Default value for option. This value
 * is restored for otion when Reset() is run.
 * @sa COption::Reset()
 */
int COption::Init(LPCTSTR name, varprop::VariantValue defaultValue)
{
	int retVal = OPT_OK;

	m_strName = name;
	if (m_strName.empty())
		return OPT_ERR;

	// Dont' check type here since we are initing it!
	varprop::VT_TYPE inType = defaultValue.GetType();

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_value.SetBool(defaultValue.GetBool());
		m_valueDef.SetBool(defaultValue.GetBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(defaultValue.GetInt());
		m_valueDef.SetInt(defaultValue.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(defaultValue.GetFloat());
		m_valueDef.SetFloat(defaultValue.GetFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(defaultValue.GetString());
		m_valueDef.SetString(defaultValue.GetString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(defaultValue.GetTime());
		m_valueDef.SetTime(defaultValue.GetTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Return option value.
 * @return Value as Variant type.
 */
varprop::VariantValue COption::Get() const
{
	varprop::VariantValue retval = m_value;
	return retval;
}

/**
 * @brief Return option default value.
 * @return Default value as varian type.
 */
varprop::VariantValue COption::GetDefault() const
{
	varprop::VariantValue retval = m_valueDef;
	return retval;
}

/**
 * @brief Convert string to integer.
 * @param [in] str String to convert.
 * @param [out] val Converted integer.
 * @return true if conversion succeeded, false otherwise.
 */
static bool GetInt(LPCTSTR str, int & val)
{
	if (str == NULL)
		return false;
	int len = _tcslen(str);
	if (len == 0)
		return false;

	for (int i = 0; i < len; ++i)
	{
		if (!_istascii(str[i]) || !_istdigit(str[i]))
			return false;
	}
	val = _ttoi(str);
	return true;
}

/**
 * @brief Convert integer value to desired type.
 * @param [in, out] value Value to convert.
 * @param [in] nType Type to convert to.
 * @return true if conversion succeeded, false otherwise.
 * @note Only supports converting to boolean at the moment.
 * @todo Add other conversions (string?).
  */
bool COption::ConvertInteger(varprop::VariantValue & value, varprop::VT_TYPE nType)
{
	int ivalue = value.GetInt();

	switch(nType)
	{
	case varprop::VT_BOOL:
		// Convert integer to boolean
		{
			if (ivalue > 0)
			{
				value.SetBool(true);
				return true;
			}
			else
			{
				value.SetBool(false);
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief Convert string value to desired type.
 * @param [in, out] value Value to convert.
 * @param [in] nType Type to convert to.
 * @return true if conversion succeeded, false otherwise.
 * @note Only supports converting to integer and boolean at the moment.
 * @todo Add other conversions (float?).
  */
bool COption::ConvertString(varprop::VariantValue & value, varprop::VT_TYPE nType)
{
	String svalue = value.GetString();
	switch(nType)
	{
	case varprop::VT_INT:
		// Convert string to integer
		{
			int val=0;
			if (!::GetInt(svalue.c_str(), val))
				return false;
			value.SetInt(val);
			return true;
		}
	case varprop::VT_BOOL:
		// Convert string to boolean
		{
			std::transform(svalue.begin(), svalue.end(), svalue.begin(), ::tolower);
			if (svalue == _T("1") || !svalue.compare(_T("yes"))
				|| !svalue.compare(_T("true")))
			{
				value.SetBool(true);
				return true;
			}
			if (svalue == _T("0") || !svalue.compare(_T("no"))
				|| !svalue.compare(_T("false")))
			{
				value.SetBool(false);
				return true;
			}
			return false;
		}
	}
	return false;
}

/**
 * @brief Convert value to desired type.
 * @param [in, out] value Value to convert.
 * @param [in] nType Type to convert to.
 * @return true if conversion succeeded, false otherwise.
 * @note Currently converts only strings and integers.
 * @todo Add other conversions.
 */
bool COption::ConvertType(varprop::VariantValue & value, varprop::VT_TYPE nType)
{
	if (value.GetType() == varprop::VT_STRING)
		return ConvertString(value, nType);
	if (value.GetType() == varprop::VT_INT)
		return ConvertInteger(value, nType);
	return false;
}

/**
 * @brief Set option value.
 * 
 * Set new value for option. If automatic conversion is not allowed,
 * type of value must match to type set when option was initialised.
 * @param [in] value New value to set.
 * @param [in] allowConversion Is automatic type conversion allowed?
 * @sa COption::Init()
 */
int COption::Set(varprop::VariantValue value, bool allowConversion)
{
	int retVal = OPT_OK;

	// Check that type matches
	varprop::VT_TYPE inType = value.GetType();
	if (value.GetType() != m_value.GetType())
	{
		if (allowConversion)
		{
			if (ConvertType(value, m_value.GetType()))
				return Set(value);
		}
		_RPTF1(_CRT_ERROR, "Wrong type for option: %s", m_strName);
		return OPT_WRONG_TYPE;
	}

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_value.SetBool(value.GetBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(value.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(value.GetFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(value.GetString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(value.GetTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Change default value for option.
 *
 * Set new value for option default value.  If automatic conversion is not
 * allowed, type of value must match to type set when option was initialised.
 * @param [in] defaultValue New default value.
 * @sa COption::Init()
 */
int COption::SetDefault(varprop::VariantValue defaultValue)
{
	int retVal = OPT_OK;

	// Check that type matches
	varprop::VT_TYPE inType = defaultValue.GetType();
	if (inType != m_valueDef.GetType())
	{
		_RPTF1(_CRT_ERROR, "Wrong type for option: %s!", m_strName);
		return OPT_WRONG_TYPE;
	}

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_valueDef.SetBool(defaultValue.GetBool());
		break;
	case varprop::VT_INT:
		m_valueDef.SetInt(defaultValue.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_valueDef.SetFloat(defaultValue.GetFloat());
		break;
	case varprop::VT_STRING:
		m_valueDef.SetString(defaultValue.GetString());
		break;
	case varprop::VT_TIME:
		m_valueDef.SetTime(defaultValue.GetTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Reset option's value to default value.
 */
void COption::Reset()
{
	switch (m_value.GetType())
	{
	case varprop::VT_BOOL:
		m_value.SetBool(m_valueDef.GetBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(m_valueDef.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(m_valueDef.GetFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(m_valueDef.GetString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(m_valueDef.GetTime());
		break;
	}
}

/**
 * @brief Add new option to list.
 * @param [in] name Option's name.
 * @param [in] defaultValue Option's initial and default value.
 */
int COptionsMgr::AddOption(LPCTSTR name, varprop::VariantValue defaultValue)
{
	int retVal = OPT_OK;
	COption tmpOption;

#ifdef _DEBUG
	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
		_RPTF1(_CRT_WARN, "Re-adding option: %s !", name);
#endif

	retVal = tmpOption.Init(name, defaultValue);
	if (retVal == OPT_OK)
		m_optionsMap[name] = tmpOption;
	else
		_RPTF1(_CRT_ERROR, "Could not add option: %s!", name);

	return retVal;
}

/**
 * @brief Get option value from list by name.
 * @param [in] name Name of the option to get.
 * @return Option's value as variant type.
 */
varprop::VariantValue COptionsMgr::Get(LPCTSTR name) const
{
	varprop::VariantValue value;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		value = found->second.Get();
	}
	else
	{
		_RPTF1(_CRT_ERROR, "Could not find option: %s!", name);
	}
	return value;
}

/**
 * @brief Return string option value.
 * @param [in] name Option's name.
 */
String COptionsMgr::GetString(LPCTSTR name) const
{
	varprop::VariantValue val = Get(name);
	return val.GetString();
}

/**
 * @brief Return integer option value.
 * @param [in] name Option's name.
 */
int COptionsMgr::GetInt(LPCTSTR name) const
{
	varprop::VariantValue val = Get(name);
	return val.GetInt();
}

/**
 * @brief Return boolean option value
 * @param [in] name Option's name.
 */
bool COptionsMgr::GetBool(LPCTSTR name) const
{
	varprop::VariantValue val = Get(name);
	return val.GetBool();
}

/**
 * @brief Set new value for option.
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::Set(LPCTSTR name, varprop::VariantValue value)
{
	int retVal = OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		// Allow automatic conversion so we don't bother callsites about this!
		COption tmpOption = found->second;
		retVal = tmpOption.Set(value, true);
		if (retVal == OPT_OK)
			m_optionsMap[name] = tmpOption;
	}
	else
	{
		_RPTF1(_CRT_ERROR, "Could not set option: %s", name);
		retVal = OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Type-convert and forward to SaveOption(String, int)
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::SaveOption(LPCTSTR name, UINT value)
{
	int xvalue = value;
	return SaveOption(name, xvalue);
}

/**
 * @brief Type-convert and forward to SaveOption(String, int)
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::SaveOption(LPCTSTR name, COLORREF value)
{
	int xvalue = value;
	return SaveOption(name, xvalue);
}

/**
 * @brief Remove option from options list.
 * @param [in] name Name of the option to remove.
 */
int COptionsMgr::RemoveOption(LPCTSTR name)
{
	int retVal = OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		BOOL succeeded = m_optionsMap.erase(name);
		if (!succeeded)
			retVal = OPT_NOTFOUND;
	}
	else
		retVal = OPT_NOTFOUND;

	return retVal;
}

/**
 * @brief Reset option value to default.
 * @param [in] name Name of the option to reset.
 */
int COptionsMgr::Reset(LPCTSTR name)
{
	int retVal = OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		COption tmpOption = found->second;
		tmpOption.Reset();
		m_optionsMap[name] = tmpOption;
	}
	else
	{
		retVal = OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Return default string value.
 * @param [in] name Option's name.
 * @param [out] value Option's default value.
 */
int COptionsMgr::GetDefault(LPCTSTR name, String & value) const
{
	int retVal = OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		varprop::VariantValue val = found->second.GetDefault();
		if (val.IsString())
			value = val.GetString();
		else
			retVal = OPT_WRONG_TYPE;
	}
	else
	{
		retVal = OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Return default number value
 * @param [in] name Option's name.
 * @param [out] value Option's default value.
 */
int COptionsMgr::GetDefault(LPCTSTR name, DWORD & value) const
{
	int retVal = OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		varprop::VariantValue val = found->second.GetDefault();
		if (val.IsInt())
			value = val.GetInt();
		else
			retVal = OPT_WRONG_TYPE;
	}
	else
	{
		retVal = OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Return default boolean value
 * @param [in] name Option's name.
 * @param [out] value Option's default value.
 */
int COptionsMgr::GetDefault(LPCTSTR name, bool & value) const
{
	int retVal = OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		varprop::VariantValue val = found->second.GetDefault();
		if (val.IsBool())
			value = val.GetBool();
		else
			retVal = OPT_WRONG_TYPE;
	}
	else
	{
		retVal = OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Export options to file.
 *
 * This function enumerates through our options storage and saves
 * every option name and value to file.
 *
 * @param [in] filename Filename where optios are written.
 * @return
 * - OPT_OK when succeeds
 * - OPT_ERR when writing to the file fails
 */
int COptionsMgr::ExportOptions(LPCTSTR filename)
{
	int retVal = OPT_OK;
	OptionsMap::iterator optIter = m_optionsMap.begin();
	while (optIter != m_optionsMap.end() && retVal == OPT_OK)
	{
		const String name(optIter->first);
		String strVal;
		varprop::VariantValue value = optIter->second.Get();
		if (value.GetType() == varprop::VT_BOOL)
		{
			if (value.GetBool())
				strVal = _T("1");
			else
				strVal = _T("0");
		}
		else if (value.GetType() == varprop::VT_INT)
		{
			TCHAR num[10] = {0};
			_itot(value.GetInt(), num, 10);
			strVal = num;
		}
		else if (value.GetType() == varprop::VT_STRING)
		{
			strVal = value.GetString();
		}

		BOOL bRet = WritePrivateProfileString(_T("WinMerge"), name.c_str(),
				strVal.c_str(), filename);
		if (!bRet)
			retVal = OPT_ERR;
		optIter++;
	}
	return retVal;
}

/**
 * @brief Import options from file.
 *
 * This function reads options values and names from given file and
 * updates values to our options storage. If valuename does not exist
 * already in options storage its is not created.
 *
 * @param [in] filename Filename where optios are written.
 * @return 
 * - OPT_OK when succeeds
 * - OPT_NOTFOUND if file wasn't found or didn't contain values
 */
int COptionsMgr::ImportOptions(LPCTSTR filename)
{
	int retVal = OPT_OK;
	const int BufSize = 10240; // This should be enough for a long time..
	TCHAR buf[BufSize] = {0};

	// Query keys - returns NULL separated strings
	DWORD len = GetPrivateProfileString(_T("WinMerge"), NULL, _T(""),buf, BufSize, filename);
	if (len == 0)
		return OPT_NOTFOUND;

	TCHAR *pKey = buf;
	while (*pKey != NULL)
	{
		varprop::VariantValue value = Get(pKey);
		if (value.GetType() == varprop::VT_BOOL)
		{
			BOOL boolVal = GetPrivateProfileInt(_T("WinMerge"), pKey, 0, filename);
			value.SetBool(boolVal == 1);
		}
		else if (value.GetType() == varprop::VT_INT)
		{
			int intVal = GetPrivateProfileInt(_T("WinMerge"), pKey, 0, filename);
			value.SetInt(intVal);
		}
		else if (value.GetType() == varprop::VT_STRING)
		{
			TCHAR strVal[MAX_PATH] = {0};
			GetPrivateProfileString(_T("WinMerge"), pKey, _T(""), strVal, MAX_PATH, filename);
			value.SetString(strVal);
		}
		Set(pKey, value);

		pKey += _tcslen(pKey);

		// Check: pointer is not past string end, and next char is not null
		// double NULL char ends the keynames string
		if ((pKey < buf + len) && (*(pKey + 1) != NULL))
			pKey++;
	}
	return retVal;
}

/**
 * @brief Split option name to path (in registry) and
 * valuename (in registry).
 *
 * Option names are given as "full path", e.g. "Settings/AutomaticRescan".
 * This function splits that to path "Settings/" and valuename
 * "AutomaticRescan".
 * @param [in] strName Option name
 * @param [out] srPath Path (key) in registry
 * @param [out] strValue Value in registry
 */
void CRegOptionsMgr::SplitName(String strName, String &strPath,
	String &strValue)
{
	int pos = strName.rfind('/');
	if (pos > 0)
	{
		int len = strName.length();
		strValue = strName.substr(pos + 1, len - pos - 1); //Right(len - pos - 1);
		strPath = strName.substr(0, pos);  //Left(pos);
	}
	else
	{
		strValue = strName;
		strPath.erase();
	}
}

/**
 * @brief Load value from registry.
 *
 * Loads one value from registry from key previously opened. Type
 * is read from value parameter and error is returned if value
 * cannot be found or if it is different type than value parameter.
 * @param [in] hKey Handle to open registry key
 * @param [in] strName Name of value to read (incl. path!).
 * @param [in, out] value
 * [in] Values type must match to value type in registry
 * [out] Read value is returned
 * @note This function must handle ANSI and UNICODE data!
 * @todo Handles only string and integer types
 */
int CRegOptionsMgr::LoadValueFromReg(HKEY hKey, LPCTSTR strName,
	varprop::VariantValue &value)
{
	String strPath;
	String strValueName;
	LONG retValReg = 0;
	LPBYTE pData = NULL;
	DWORD type = 0;
	TCHAR * valueBuf = NULL;
	DWORD size = 0;
	int valType = value.GetType();
	int retVal = OPT_OK;

	SplitName(strName, strPath, strValueName);

	// Get type and size of value in registry
	retValReg = RegQueryValueEx(hKey, strValueName.c_str(), 0, &type,
		NULL, &size);
	
	if (retValReg == ERROR_SUCCESS)
	{
		pData = new BYTE[size + sizeof(TCHAR)];
		if (pData == NULL)
			return OPT_ERR;
		ZeroMemory(pData, size + sizeof(TCHAR));

		// Get data
		retValReg = RegQueryValueEx(hKey, strValueName.c_str(),
			0, &type, pData, &size);
	}
	
	if (retValReg == ERROR_SUCCESS)
	{
		if (type == REG_SZ && valType == varprop::VT_STRING )
		{
			value.SetString((LPCTSTR)pData);
			retVal = Set(strName, value);
		}
		else if (type == REG_DWORD)
		{
			if (valType == varprop::VT_INT)
			{
				DWORD dwordValue;
				CopyMemory(&dwordValue, pData, sizeof(DWORD));
				value.SetInt(dwordValue);
				retVal = Set(strName, value);
			}
			else if (valType == varprop::VT_BOOL)
			{
				DWORD dwordValue;
				CopyMemory(&dwordValue, pData, sizeof(DWORD));
				value.SetBool(dwordValue > 0 ? true : false);
				retVal = Set(strName, value);
			}
			else
				retVal = OPT_WRONG_TYPE;
		}
		else
			retVal = OPT_WRONG_TYPE;
	}
	else
		retVal = OPT_ERR;

	delete [] pData;

	return retVal;
}

/**
 * @brief Save value to registry.
 *
 * Saves one value to registry to key previously opened. Type of
 * value is determined from given value parameter.
 * @param [in] hKey Handle to open registry key
 * @param [in] strValueName Name of value to write
 * @param [in] value value to write to registry value
 * @todo Handles only string and integer types
 */
int CRegOptionsMgr::SaveValueToReg(HKEY hKey, LPCTSTR strValueName,
	varprop::VariantValue value)
{
	LONG retValReg = 0;
	int valType = value.GetType();
	int retVal = OPT_OK;

	if (valType == varprop::VT_STRING)
	{
		String strVal = value.GetString();
		if (strVal.length() > 0)
		{
			retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_SZ,
					(LPBYTE)strVal.c_str(), (strVal.length() + 1) * sizeof(TCHAR));
		}
		else
		{
			TCHAR str[1] = {0};
			retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_SZ,
					(LPBYTE)&str, 1 * sizeof(TCHAR));
		}
	}
	else if (valType == varprop::VT_INT)
	{
		DWORD dwordVal = value.GetInt();
		retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_DWORD,
				(LPBYTE)&dwordVal, sizeof(DWORD));
	}
	else if (valType == varprop::VT_BOOL)
	{
		DWORD dwordVal = value.GetBool() ? 1 : 0;
		retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_DWORD,
				(LPBYTE)&dwordVal, sizeof(DWORD));
	}
	else
	{
		retVal = OPT_UNKNOWN_TYPE;
	}
		
	if (retValReg != ERROR_SUCCESS)
	{
		retVal = OPT_ERR;
	}
	return retVal;
}

/**
 * @brief Init and add new option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(LPCTSTR name, varprop::VariantValue defaultValue)
{
	// Check type & bail if null
	int valType = defaultValue.GetType();
	if (valType == varprop::VT_NULL)
		return OPT_ERR;

	// If we're not loading & saving options, bail
	if (!m_serializing)
		return AddOption(name, defaultValue);

	// Figure out registry path, for saving value
	String strPath;
	String strValueName;
	SplitName(name, strPath, strValueName);
	String strRegPath(m_registryRoot);
	strRegPath += strPath;

	// Open key. Create new key if it does not exist.
	HKEY hKey = NULL;
	DWORD action = 0;
	LONG retValReg = RegCreateKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
		NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
		&hKey, &action);

	if (retValReg != ERROR_SUCCESS)
		return OPT_ERR;

	// Check previous value
	// This just checks if the value exists, LoadValueFromReg() below actually
	// loads the value.
	DWORD type = 0;
	BYTE dataBuf[MAX_PATH] = {0};
	DWORD size = MAX_PATH;
	retValReg = RegQueryValueEx(hKey, strValueName.c_str(),
		0, &type, dataBuf, &size);

	// Actually save value into our in-memory options table
	int retVal = AddOption(name, defaultValue);
	
	// Update registry if successfully saved to in-memory table
	if (retVal == OPT_OK)
	{
		// Value didn't exist. Save default value to registry
		if (retValReg == ERROR_FILE_NOT_FOUND)
		{
			retVal = SaveValueToReg(hKey, strValueName.c_str(), defaultValue);
		}
		// Value already exists so read it.
		else if (retValReg == ERROR_SUCCESS || ERROR_MORE_DATA)
		{
			retVal = LoadValueFromReg(hKey, name, defaultValue);
			if (retVal == OPT_OK)
				retVal = Set(name, defaultValue);
		}
	}
	RegCloseKey(hKey);
	return retVal;
}

/**
 * @brief Init and add new string option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(LPCTSTR name, LPCTSTR defaultValue)
{
	varprop::VariantValue defValue;
	int retVal = OPT_OK;
	
	defValue.SetString(defaultValue);
	retVal = InitOption(name, defValue);
	return retVal;
}

/**
 * @brief Init and add new int option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(LPCTSTR name, int defaultValue, bool serializable)
{
	varprop::VariantValue defValue;
	int retVal = OPT_OK;
	
	defValue.SetInt(defaultValue);
	if (serializable)
		retVal = InitOption(name, defValue);
	else
		AddOption(name, defValue);
	return retVal;
}

/**
 * @brief Init and add new boolean option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(LPCTSTR name, bool defaultValue)
{
	varprop::VariantValue defValue;
	int retVal = OPT_OK;
	
	defValue.SetBool(defaultValue);
	retVal = InitOption(name, defValue);
	return retVal;
}

/**
 * @brief Load option from registry.
 * @note Currently handles only integer and string options!
 */
int CRegOptionsMgr::LoadOption(LPCTSTR name)
{
	varprop::VariantValue value;
	String strPath;
	String strValueName;
	String strRegPath(m_registryRoot);
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int valType = varprop::VT_NULL;
	int retVal = OPT_OK;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	value = Get(name);
	valType = value.GetType();
	if (valType == varprop::VT_NULL)
		retVal = OPT_NOTFOUND;
	
	if (retVal == OPT_OK)
	{
		retValReg = RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
			NULL, KEY_READ, &hKey);

		if (retValReg == ERROR_SUCCESS)
		{
			retVal = LoadValueFromReg(hKey, name, value);
			RegCloseKey(hKey);
		}
		else
			retVal = OPT_ERR;
	}
	return retVal;
}

/**
 * @brief Save option to registry
 * @note Currently handles only integer and string options!
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name)
{
	if (!m_serializing) return OPT_OK;

	varprop::VariantValue value;
	String strPath;
	String strValueName;
	String strRegPath(m_registryRoot);
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int valType = varprop::VT_NULL;
	int retVal = OPT_OK;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	value = Get(name);
	valType = value.GetType();
	if (valType == varprop::VT_NULL)
		retVal = OPT_NOTFOUND;
	
	if (retVal == OPT_OK)
	{
		retValReg = RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
			NULL, KEY_WRITE, &hKey);

		if (retValReg == ERROR_SUCCESS)
		{
			retVal = SaveValueToReg(hKey, strValueName.c_str(), value);
			RegCloseKey(hKey);
		}
		else
			retVal = OPT_ERR;
	}
	return retVal;
}

/**
 * @brief Set new value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, varprop::VariantValue value)
{
	int retVal = OPT_OK;
	retVal = Set(name, value);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new string value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, LPCTSTR value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetString(value);
	retVal = Set(name, val);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new integer value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, int value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetInt(value);
	retVal = Set(name, val);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new boolean value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, bool value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetBool(value);
	retVal = Set(name, val);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

int CRegOptionsMgr::RemoveOption(LPCTSTR name)
{
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int retVal = OPT_OK;
	String strRegPath(m_registryRoot);
	String strPath;
	String strValueName;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	retValReg = RegOpenKey(HKEY_CURRENT_USER, strRegPath.c_str(), &hKey);
	if (retValReg == ERROR_SUCCESS)
	{
		retValReg = RegDeleteValue(hKey, strValueName.c_str());
		if (retValReg != ERROR_SUCCESS)
		{
			retVal = OPT_ERR;
		}
		RegCloseKey(hKey);
	}
	else
		retVal = OPT_ERR;

	return retVal;

}

/**
 * @brief Set registry root path for options.
 *
 * Sets path used as root path when loading/saving options. Paths
 * given to other functions are relative to this path.
 */
int CRegOptionsMgr::SetRegRootKey(LPCTSTR key)
{
	String keyname(key);
	HKEY hKey = NULL;
	LONG retValReg = 0;
	DWORD action = 0;
	int retVal = OPT_OK;
	int ind = 0;

	ind = keyname.find(_T("Software"));
	if (ind != 0)
		keyname.insert(0, _T("Software\\"));
	
	m_registryRoot = keyname;

	retValReg =  RegCreateKeyEx(HKEY_CURRENT_USER, m_registryRoot.c_str(), NULL, _T(""),
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &action);

	if (retValReg == ERROR_SUCCESS)
	{
		if (action == REG_CREATED_NEW_KEY)
		{
			// TODO: At least log message..?
		}
		RegCloseKey(hKey);
	}
	else
	{
		retVal = OPT_ERR;
	}

	return retVal;
}
