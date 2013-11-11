#pragma once

class CSettings {

	BOOL Load() {
		CIni ini(m_strFilePath);
		m_strClipboardFile = ini.GetExpandedString(TEXT("Default"),
			TEXT("ClipboardFile"), TEXT("%USERPROFILE%\\Dropbox\\Clipboard.txt"));
		m_bSychronizingEnabled = ini.GetBoolean(TEXT("Default"), TEXT("SychronizingEnabled"), TRUE);
		return TRUE;
	}

	CString m_strFilePath;

public:

	CString m_strClipboardFile;
	BOOL m_bSychronizingEnabled;

	CSettings() {}

	BOOL Initialize(CString const & strFilePath) {
		ATLASSERT(!strFilePath.IsEmpty());
		m_strFilePath = strFilePath;
		return Load();
	}

	BOOL Save() {
		CIni ini(m_strFilePath);
		ini.WriteString(TEXT("Default"), TEXT("ClipboardFile"), m_strClipboardFile);
		ini.WriteBoolean(TEXT("Default"), TEXT("SychronizingEnabled"), m_bSychronizingEnabled);
		return TRUE;
	}

	BOOL operator ==(CSettings const & s) {
		return m_strClipboardFile == s.m_strClipboardFile &&
			m_bSychronizingEnabled == s.m_bSychronizingEnabled;
	}

	BOOL operator !=(CSettings const & s) {
		return !operator ==(s);
	}

	CSettings & operator =(CSettings const & s) {
		m_strClipboardFile = s.m_strClipboardFile;
		m_bSychronizingEnabled = s.m_bSychronizingEnabled;
		return *this;
	}

	static LPCSTR GetDefault() {
		return
			"[Default]\r\n";
			"; Absolute path to the file with the clipboard content.\r\n"
			"; Environment variables encapsulated by % will be resolved.\r\n"
			"ClipboardFile=%USERPROFILE%\\Dropbox\\Clipboard.txt\r\n";
	}

};