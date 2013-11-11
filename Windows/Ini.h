// Ini.h: interface for the CIni class.
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2000.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__2478E9E2_E904_11D1_93C1_241C08C10000__INCLUDED_)
#define AFX_INI_H__2478E9E2_E904_11D1_93C1_241C08C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
// INI file class
//
// Author:
// Bjarke Viksøe
// Description:
// Implements helper functions to access
// an .INI configuration file using
// conventional CString operations
//

// Ini-file wrapper class
class CIni {
public:
	CIni();
	CIni( LPCTSTR IniFilename );
	~CIni();

	// Methods
public:
	// Sets the current Ini-file to use.
	BOOL SetIniFilename(LPCTSTR IniFilename);
	//
	// Reads an integer from the ini-file.
	UINT GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault=0);
	// Reads a boolean value from the ini-file.
	BOOL GetBoolean(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bDefault=FALSE);
	// Reads a string from the ini-file.
	CString GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault=NULL);
	// Reads a string from the ini-file, expanding ebnvironment variables enclosed in %.
	CString GetExpandedString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault=NULL);
	//
	// Writes an integer to the ini-file.
	BOOL WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	// Writes a boolean value to the ini-file.
	BOOL WriteBoolean(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bValue);
	// Writes a string to the ini-file.
	BOOL WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	//
	// Removes an item from the current ini-file.
	BOOL DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszEntry);
	// Removes a complete section from the ini-file.
	BOOL DeleteSection(LPCTSTR lpszSection);

	// Variables
protected:
	CString m_IniFilename; // The current ini-file used.
};

CIni::CIni() {
	m_IniFilename.Empty();
}

CIni::CIni(LPCTSTR IniFilename) {
	SetIniFilename( IniFilename );
}

CIni::~CIni() {
	// Flush .ini file
	// (This should perhaps not be here. We risk to slow
	//  down the system and this would be done at a more appropriate
	//  time by the OS scheduler anyway)
	::WritePrivateProfileString( NULL, NULL, NULL, m_IniFilename );
}


//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

#define MAX_INI_BUFFER 300   // Defines the maximum number of chars we can
// read from the ini file 

BOOL CIni::SetIniFilename(LPCTSTR IniFilename) {
	ATLASSERT(AtlIsValidString(IniFilename));
	m_IniFilename = IniFilename;
	if( m_IniFilename.IsEmpty() ) return FALSE;
	return TRUE;
};


UINT CIni::GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault) {
	ATLASSERT(AtlIsValidString(lpszSection));
	ATLASSERT(AtlIsValidString(lpszEntry));
	if( m_IniFilename.IsEmpty() ) return 0; // error
	CString sDefault;
	sDefault.Format( _T("%d"), nDefault );
	CString s = GetString( lpszSection, lpszEntry, sDefault );
	return _ttol( s );
};

CString CIni::GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) {
	ATLASSERT(AtlIsValidString(lpszSection));
	ATLASSERT(AtlIsValidString(lpszEntry));
	if( m_IniFilename.IsEmpty() ) return CString();
	CString s;
	long ret = ::GetPrivateProfileString( lpszSection, lpszEntry, lpszDefault, s.GetBuffer( MAX_INI_BUFFER ), MAX_INI_BUFFER, m_IniFilename );
	s.ReleaseBuffer();
	if( ret==0 ) return CString(lpszDefault);
	return s;
};

CString CIni::GetExpandedString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) {
	CString raw(GetString(lpszSection, lpszEntry, lpszDefault));
	CString expanded;
	::ExpandEnvironmentStrings(raw, expanded.GetBuffer( MAX_INI_BUFFER + 2 ), MAX_INI_BUFFER);
	expanded.ReleaseBuffer();
	return expanded;
};

BOOL CIni::GetBoolean(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bDefault) {
	CString s = GetString(lpszSection,lpszEntry);
	if( s.IsEmpty() ) return bDefault;
	TCHAR c = _totupper( s[0] );
	switch( c ) {
	case _T('T'): // TRUE
	case _T('Y'): // YES
	case _T('1'): // 1 (binary)
	case _T('O'): // OK
		return TRUE;
	default:
		return FALSE;
	};
};


BOOL CIni::WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue) {
	ATLASSERT(AtlIsValidString(lpszSection));
	ATLASSERT(AtlIsValidString(lpszEntry));
	CString s;
	s.Format( _T("%d"), nValue );
	return WriteString( lpszSection, lpszEntry, s );
};

BOOL CIni::WriteBoolean(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bValue) {
	CString s;
	bValue ? s=_T("Y") : s=_T("N");
	return WriteString( lpszSection, lpszEntry, s );
};

BOOL CIni::WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) {
	ATLASSERT(AtlIsValidString(lpszSection));
	ATLASSERT(AtlIsValidString(lpszEntry));
	if( m_IniFilename.IsEmpty() ) return FALSE;
	return ::WritePrivateProfileString( lpszSection, lpszEntry, lpszValue, m_IniFilename );
};

BOOL CIni::DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszEntry) {
	ATLASSERT(AtlIsValidString(lpszSection));
	ATLASSERT(AtlIsValidString(lpszEntry));
	if( m_IniFilename.IsEmpty() ) return FALSE;
	return ::WritePrivateProfileString( lpszSection, lpszEntry, NULL, m_IniFilename );
};

BOOL CIni::DeleteSection(LPCTSTR lpszSection) {
	ATLASSERT(AtlIsValidString(lpszSection));
	if( m_IniFilename.IsEmpty() ) return FALSE;
	return ::WritePrivateProfileString( lpszSection, NULL, NULL, m_IniFilename );
};

#endif // !defined(AFX_INI_H__2478E9E2_E904_11D1_93C1_241C08C10000__INCLUDED_)
