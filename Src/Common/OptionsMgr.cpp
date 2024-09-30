/** 
 * @file OptionsMgr.cpp
 *
 * @brief Implementation of Options management classes
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: OptionsMgr.cpp 3464 2006-08-10 20:47:31Z kimmov $


/* The MIT License
Copyright (c) 2004 Kimmo Varis
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
#include "varprop.h"
#include "OptionsMgr.h"

/**
 * @brief Set name, value and default value for option
 * @param [in] optName Name of option ("Settings/AutomaticRescan")
 * @param [in] defaultValue Default value for option. This value
 * is restored for otion when Reset() is run.
 * @sa COption::Reset()
 */
int COption::Init(CString optName, varprop::VariantValue defaultValue)
{
	int retVal = OPT_OK;

	// Dont' check type here since we are initing it!
	m_strName = optName;
	varprop::VT_TYPE inType = defaultValue.getType();

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_value.SetBool(defaultValue.getBool());
		m_valueDef.SetBool(defaultValue.getBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(defaultValue.getInt());
		m_valueDef.SetInt(defaultValue.getInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(defaultValue.getFloat());
		m_valueDef.SetFloat(defaultValue.getFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(defaultValue.getString());
		m_valueDef.SetString(defaultValue.getString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(defaultValue.getTime());
		m_valueDef.SetTime(defaultValue.getTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;		
}

/**
 * @brief Return option value
 */
varprop::VariantValue COption::Get() const
{
	return m_value;
}

/**
 * @brief Return option default value
 */
varprop::VariantValue COption::GetDefault() const
{
	return m_valueDef;
}

/**
 * @brief Convert string to integer, or return false if not an integer number
 */
static bool
getInt(const CString & str, int & val)
{
	if (str.IsEmpty()) return false;
	for (int i=0; i<str.GetLength(); ++i)
	{
		TCHAR ch = str[i];
		if (!_istascii(ch) || !_istdigit(ch)) return false;
	}
	val = _ttoi(str);
	return true;
}

/**
 * @brief Convert value to desired type (or return false)
 */
bool COption::CoerceType(varprop::VariantValue & value, varprop::VT_TYPE nType)
{
	if (value.getType() == varprop::VT_STRING)
	{
		CString svalue = value.getString();
		switch(nType)
		{
		case varprop::VT_INT:
			// Convert string to integer
			{
				int val=0;
				if (!getInt(svalue, val)) return false;
				value.SetInt(val);
				return true;
			}
		case varprop::VT_BOOL:
			// Convert string to boolean
			{
				if (svalue == _T("1") || !svalue.CompareNoCase(_T("Yes"))
					|| !svalue.CompareNoCase(_T("True")))
				{
					value.SetBool(true);
					return true;
				}
				if (svalue == _T("0") || !svalue.CompareNoCase(_T("No"))
					|| !svalue.CompareNoCase(_T("False")))
				{
					value.SetBool(false);
					return true;
				}
				return false;
			}
		}
	}
	return false;
}

/**
 * @brief Set option value.
 * 
 * Set new value for option. Type of value must match to type
 * set when option was initialised.
 * @sa COption::Init()
 */
int COption::Set(varprop::VariantValue value, coercion_type coercion)
{
	int retVal = OPT_OK;

	// Check that type matches
	varprop::VT_TYPE inType = value.getType();
	if (value.getType() != m_value.getType())
	{
		if (coercion == coerce)
		{
			if (CoerceType(value, m_value.getType()))
				return Set(value, nocoerce);
		}
		return OPT_WRONG_TYPE;
	}

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_value.SetBool(value.getBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(value.getInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(value.getFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(value.getString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(value.getTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Change default value for option.
 *
 * Set new value for option default value. Type of given value
 * must match to type given when option was initialised.
 * @sa COption::Init()
 */
int COption::SetDefault(varprop::VariantValue defaultValue)
{
	int retVal = OPT_OK;

	// Check that type matches
	varprop::VT_TYPE inType = defaultValue.getType();
	if (inType != m_valueDef.getType())
		return OPT_WRONG_TYPE;

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_valueDef.SetBool(defaultValue.getBool());
		break;
	case varprop::VT_INT:
		m_valueDef.SetInt(defaultValue.getInt());
		break;
	case varprop::VT_FLOAT:
		m_valueDef.SetFloat(defaultValue.getFloat());
		break;
	case varprop::VT_STRING:
		m_valueDef.SetString(defaultValue.getString());
		break;
	case varprop::VT_TIME:
		m_valueDef.SetTime(defaultValue.getTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Reset options value to default value
 */
void COption::Reset()
{
	switch (m_value.getType())
	{
	case varprop::VT_BOOL:
		m_value.SetBool(m_valueDef.getBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(m_valueDef.getInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(m_valueDef.getFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(m_valueDef.getString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(m_valueDef.getTime());
		break;
	}
}

/**
 * @brief Add new option to list.
 */
int COptionsMgr::AddOption(CString name, varprop::VariantValue defaultValue)
{
	int retVal = OPT_OK;
	COption tmpOption;
	
	retVal = tmpOption.Init(name, defaultValue);
	if (retVal == OPT_OK)
		m_optionsMap.SetAt(name, tmpOption);

	return retVal;
}

/**
 * @brief Get option value from list by name
 */
varprop::VariantValue COptionsMgr::Get(CString name) const
{
	COption tmpOption;
	varprop::VariantValue value;
	BOOL optionFound = FALSE;

	optionFound = m_optionsMap.Lookup(name, tmpOption);
	if (optionFound)
	{
		value = tmpOption.Get();
	}
	return value;
}

/**
 * @brief Return string option value
 */
CString COptionsMgr::GetString(CString name) const
{
	varprop::VariantValue val;
	val = Get(name);
	return val.getString();
}

/**
 * @brief Return integer option value
 */
int COptionsMgr::GetInt(const CString & name) const
{
	varprop::VariantValue val;
	val = Get(name);
	return val.getInt();
}

/**
 * @brief Return boolean option value
 */
bool COptionsMgr::GetBool(CString name) const
{
	varprop::VariantValue val;
	val = Get(name);
	return val.getBool();
}

/**
 * @brief Set new value for option
 */
int COptionsMgr::Set(CString name, varprop::VariantValue value, COption::coercion_type coercion)
{
	COption tmpOption;
	BOOL optionFound = FALSE;
	int retVal = OPT_OK;

	optionFound = m_optionsMap.Lookup(name, tmpOption);
	if (optionFound == TRUE)
	{
		retVal = tmpOption.Set(value, coercion);
		if (retVal == OPT_OK)
			m_optionsMap.SetAt(name, tmpOption);
	}
	else
	{
		retVal = OPT_NOTFOUND;
	}
	return retVal;
}

/*
 * @brief Type-convert and forward to SaveOption(CString, int)
 */
int COptionsMgr::SaveOption(CString name, UINT value)
{
	int xvalue = value;
	return SaveOption(name, xvalue);
}

/*
 * @brief Type-convert and forward to SaveOption(CString, int)
 */
int COptionsMgr::SaveOption(CString name, COLORREF value)
{
	int xvalue = value;
	return SaveOption(name, xvalue);
}

/**
 * @brief Reset option value to default
 */
int COptionsMgr::Reset(CString name)
{
	COption tmpOption;
	BOOL optionFound = FALSE;
	int retVal = OPT_OK;

	optionFound = m_optionsMap.Lookup(name, tmpOption);
	if (optionFound == TRUE)
	{
		tmpOption.Reset();
		m_optionsMap.SetAt(name, tmpOption);
	}
	else
	{
		retVal = OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Return default string value
 */
int COptionsMgr::GetDefault(CString name, CString & value) const
{
	COption tmpOption;
	BOOL optionFound = FALSE;
	int retVal = OPT_OK;

	optionFound = m_optionsMap.Lookup(name, tmpOption);
	if (optionFound == TRUE)
	{
		varprop::VariantValue val = tmpOption.GetDefault();
		if (val.isString())
			value = val.getString();
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
 */
int COptionsMgr::GetDefault(CString name, DWORD & value) const
{
	COption tmpOption;
	BOOL optionFound = FALSE;
	int retVal = OPT_OK;

	optionFound = m_optionsMap.Lookup(name, tmpOption);
	if (optionFound == TRUE)
	{
		varprop::VariantValue val = tmpOption.GetDefault();
		if (val.isInt())
			value = val.getInt();
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
 */
int COptionsMgr::GetDefault(CString name, bool & value) const
{
	COption tmpOption;
	BOOL optionFound = FALSE;
	int retVal = OPT_OK;

	optionFound = m_optionsMap.Lookup(name, tmpOption);
	if (optionFound == TRUE)
	{
		varprop::VariantValue val = tmpOption.GetDefault();
		if (val.isBool())
			value = val.getBool();
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
int COptionsMgr::ExportOptions(CString filename)
{
	int retVal = OPT_OK;
	POSITION pos = m_optionsMap.GetStartPosition();
	while (pos && retVal == OPT_OK)
	{
		COption option;
		CString name;
		CString strVal;
		m_optionsMap.GetNextAssoc(pos, name, option);
		varprop::VariantValue value = option.Get();
		if (value.getType() == varprop::VT_BOOL)
		{
			if (value.getBool())
				strVal = _T("1");
			else
				strVal = _T("0");
		}
		else if (value.getType() == varprop::VT_INT)
		{
			strVal.Format(_T("%d"), value.getInt());
		}
		else if (value.getType() == varprop::VT_STRING)
		{
			strVal = value.getString();
		}

		BOOL bRet = WritePrivateProfileString(_T("WinMerge"), name, strVal, filename);
		if (!bRet)
			retVal = OPT_ERR;
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
int COptionsMgr::ImportOptions(CString filename)
{
	int retVal = OPT_OK;
	CString name;
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
		if (value.getType() == varprop::VT_BOOL)
		{
			BOOL boolVal = GetPrivateProfileInt(_T("WinMerge"), pKey, 0, filename);
			value.SetBool(boolVal == 1);
		}
		else if (value.getType() == varprop::VT_INT)
		{
			int intVal = GetPrivateProfileInt(_T("WinMerge"), pKey, 0, filename);
			value.SetInt(intVal);
		}
		else if (value.getType() == varprop::VT_STRING)
		{
			TCHAR strVal[MAX_PATH] = {0};
			GetPrivateProfileString(_T("WinMerge"), pKey, _T(""), strVal, MAX_PATH, filename);
			CString csVal = strVal;
			value.SetString(csVal);
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
void CRegOptionsMgr::SplitName(CString strName, CString &strPath,
	CString &strValue)
{
	int pos = strName.ReverseFind('/');
	if (pos > 0)
	{
		int len = strName.GetLength();
		strValue = strName.Right(len - pos - 1);
		strPath = strName.Left(pos);
	}
	else
	{
		strValue = strName;
		strPath.Empty();
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
int CRegOptionsMgr::LoadValueFromReg(HKEY hKey, CString strName,
	varprop::VariantValue &value)
{
	CString strPath;
	CString strValueName;
	LONG retValReg = 0;
	LPBYTE pData = NULL;
	DWORD type = 0;
	TCHAR * valueBuf = NULL;
	DWORD size = 0;
	int valType = value.getType();
	int retVal = OPT_OK;

	SplitName(strName, strPath, strValueName);

	// Get type and size of value in registry
	retValReg = RegQueryValueEx(hKey, (LPCTSTR)strValueName, 0, &type,
		NULL, &size);
	
	if (retValReg == ERROR_SUCCESS)
	{
		pData = new BYTE[size];
		if (pData == NULL)
			return OPT_ERR;

		// Get data
		retValReg = RegQueryValueEx(hKey, (LPCTSTR)strValueName,
			0, &type, pData, &size);
	}
	
	if (retValReg == ERROR_SUCCESS)
	{
		if (type == REG_SZ && valType == varprop::VT_STRING )
		{
			CString strValue;
			valueBuf = strValue.GetBuffer(size);
			CopyMemory(valueBuf, pData, size); // Copy NULL also
			strValue.ReleaseBuffer();
			value.SetString(strValue);
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
int CRegOptionsMgr::SaveValueToReg(HKEY hKey, CString strValueName,
	varprop::VariantValue value)
{
	LONG retValReg = 0;
	int valType = value.getType();
	int retVal = OPT_OK;

	if (valType == varprop::VT_STRING)
	{
		CString strVal = value.getString();
		retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_SZ,
				(LPBYTE)(LPCTSTR)value.getString(),
				(strVal.GetLength() + 1) * sizeof(TCHAR));
	}
	else if (valType == varprop::VT_INT)
	{
		DWORD dwordVal = value.getInt();
		retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_DWORD,
				(LPBYTE)&dwordVal, sizeof(DWORD));
	}
	else if (valType == varprop::VT_BOOL)
	{
		DWORD dwordVal = value.getBool() ? 1 : 0;
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
int CRegOptionsMgr::InitOption(CString name, varprop::VariantValue defaultValue)
{
	// Check type & bail if null
	int valType = defaultValue.getType();
	if (valType == varprop::VT_NULL)
		return OPT_ERR;

	// If we're not loading & saving options, bail
	if (!m_serializing)
		return AddOption(name, defaultValue);

	// Figure out registry path, for saving value
	CString strPath;
	CString strValueName;
	SplitName(name, strPath, strValueName);
	CString strRegPath = m_registryRoot + strPath;

	// Open key. Create new key if it does not exist.
	HKEY hKey = NULL;
	DWORD action = 0;
	LONG retValReg = RegCreateKeyEx(HKEY_CURRENT_USER, strRegPath, NULL, _T(""),
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &action);

	if (retValReg != ERROR_SUCCESS)
		return OPT_ERR;

	// Check previous value
	DWORD type = 0;
	BYTE dataBuf[MAX_PATH] = {0};
	DWORD size = MAX_PATH;
	retValReg = RegQueryValueEx(hKey, (LPCTSTR)strValueName,
		0, &type, dataBuf, &size);

	// Actually save value into our in-memory options table
	int retVal = AddOption(name, defaultValue);
	
	// Update registry if successfully saved to in-memory table
	if (retVal == OPT_OK)
	{
		// Value didn't exist. Save default value to registry
		if (retValReg == ERROR_FILE_NOT_FOUND)
		{
			retVal = SaveValueToReg(hKey, strValueName,	defaultValue);
		}
		// Value already exists so read it.
		else if (retValReg == ERROR_SUCCESS)
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
int CRegOptionsMgr::InitOption(CString name, LPCTSTR defaultValue)
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
int CRegOptionsMgr::InitOption(CString name, int defaultValue, bool serializable)
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
int CRegOptionsMgr::InitOption(CString name, bool defaultValue)
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
int CRegOptionsMgr::LoadOption(CString name)
{
	varprop::VariantValue value;
	CString strPath;
	CString strValueName;
	CString strRegPath = m_registryRoot;
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int valType = varprop::VT_NULL;
	int retVal = OPT_OK;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	value = Get(name);
	valType = value.getType();
	if (valType == varprop::VT_NULL)
		retVal = OPT_NOTFOUND;
	
	if (retVal == OPT_OK)
	{
		retValReg = RegOpenKeyEx(HKEY_CURRENT_USER, (LPCTSTR)strRegPath,
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
int CRegOptionsMgr::SaveOption(CString name)
{
	if (!m_serializing) return OPT_OK;

	varprop::VariantValue value;
	CString strPath;
	CString strValueName;
	CString strRegPath = m_registryRoot;
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int valType = varprop::VT_NULL;
	int retVal = OPT_OK;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	value = Get(name);
	valType = value.getType();
	if (valType == varprop::VT_NULL)
		retVal = OPT_NOTFOUND;
	
	if (retVal == OPT_OK)
	{
		retValReg = RegOpenKeyEx(HKEY_CURRENT_USER, (LPCTSTR)strRegPath,
			NULL, KEY_WRITE, &hKey);

		if (retValReg == ERROR_SUCCESS)
		{
			retVal = SaveValueToReg(hKey, strValueName, value);
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
int CRegOptionsMgr::SaveOption(CString name, varprop::VariantValue value)
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
int CRegOptionsMgr::SaveOption(CString name, CString value)
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
 * @brief Set new string value for option, with coercion, and save option to registry
 */
int CRegOptionsMgr::CoerceAndSaveOption(CString name, CString value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetString(value);
	retVal = Set(name, val, COption::coerce);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new integer value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(CString name, int value)
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
int CRegOptionsMgr::SaveOption(CString name, bool value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetBool(value);
	retVal = Set(name, val);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set registry root path for options.
 *
 * Sets path used as root path when loading/saving options. Paths
 * given to other functions are relative to this path.
 */
int CRegOptionsMgr::SetRegRootKey(CString key)
{
	HKEY hKey = NULL;
	LONG retValReg = 0;
	DWORD action = 0;
	int retVal = OPT_OK;
	int ind = 0;

	ind = key.Find(_T("Software"), 0);
	if (ind != 0)
		key.Insert(0, _T("Software\\"));
	
	m_registryRoot = key;

	retValReg =  RegCreateKeyEx(HKEY_CURRENT_USER, key, NULL, _T(""),
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

