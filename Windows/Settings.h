#pragma once

class CSettings {

	BOOL Load() {
		CIni ini(m_strFilePath);
		m_strClipboardFile = ini.GetExpandedString(TEXT("Default"),
			TEXT("ClipboardFile"), TEXT("%USERPROFILE%\\Dropbox\\Clipboard.txt"));
		m_bSychronizingEnabled = ini.GetBoolean(TEXT("Default"), TEXT("SychronizingEnabled"), TRUE);
		m_bNotifyOnClipboardChange = ini.GetBoolean(TEXT("Default"), TEXT("NotifyOnClipboardChange"), TRUE);
		return TRUE;
	}

	CString m_strFilePath;

public:

	CString m_strClipboardFile;
	BOOL m_bSychronizingEnabled;
	BOOL m_bNotifyOnClipboardChange;

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
		ini.WriteBoolean(TEXT("Default"), TEXT("NotifyOnClipboardChange"), m_bNotifyOnClipboardChange);
		return TRUE;
	}

	BOOL operator ==(CSettings const & s) {
		return m_strClipboardFile == s.m_strClipboardFile &&
			m_bSychronizingEnabled == s.m_bSychronizingEnabled &&
			m_bNotifyOnClipboardChange == s.m_bNotifyOnClipboardChange;
	}

	BOOL operator !=(CSettings const & s) {
		return !operator ==(s);
	}

	CSettings & operator =(CSettings const & s) {
		m_strClipboardFile = s.m_strClipboardFile;
		m_bSychronizingEnabled = s.m_bSychronizingEnabled;
		m_bNotifyOnClipboardChange = s.m_bNotifyOnClipboardChange;
		return *this;
	}

	static LPCSTR GetDefault() {
		return
			"[Default]\r\n"
			"; Absolute path to the file with the clipboard content.\r\n"
			"; Environment variables encapsulated by % will be resolved.\r\n"
			"ClipboardFile=%USERPROFILE%\\Dropbox\\Clipboard.txt\r\n"
			"; Enables monitoring the clipboard changes to update the\r\n"
			"; file and the file changes to update the clipboard.\r\n"
			"SychronizingEnabled=yes\r\n"
			"; Shows a taskbar tooltip if clipboard changes.\r\n"
			"NotifyOnClipboardChange=yes\r\n";
	}

};