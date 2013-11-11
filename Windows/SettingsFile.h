#pragma once

class CSettingsFile {

	static CString GetUserHomeDir()
	{
		CHandle token;
		ATLVERIFY(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token.m_h));
		ATLASSERT(token);
		TCHAR szHomeDirBuf[MAX_PATH] = { 0 };
		DWORD BufSize = MAX_PATH;
		ATLVERIFY(GetUserProfileDirectory(token, szHomeDirBuf, &BufSize));
		return CString(szHomeDirBuf);
	}

	BOOL WriteFile() {
		CFile file;
		if (file.Create(m_strFilePath)) {
			LPCSTR pszContent = CSettings::GetDefault();
			ATLVERIFY(file.Write(pszContent, (DWORD) strlen(pszContent)));
			ATLVERIFY(file.SetEOF());
			return TRUE;
		}
		return FALSE;
	}

	BOOL m_bUpdating;

public:

	CSettings m_s;

	CString m_strFilePath;
	CFileInspector m_fi;

	CString m_strClipboardFile;
	BOOL m_bSychronizingEnabled;

	CSettingsFile() : m_bUpdating(FALSE) {}

	BOOL Initialize() {
		CPath path(GetUserHomeDir());
		ATLVERIFY(path.Append(TEXT("SynClip.ini")));
		m_strFilePath = path.m_strPath;
		if (!m_fi.Initialize(m_strFilePath)) {
			m_bUpdating = TRUE;
			WriteFile();
			m_fi.Initialize(m_strFilePath);
			m_bUpdating = FALSE;
		}
		return m_s.Initialize(m_strFilePath);
	}

	BOOL EditFile(HWND hWnd) {
		ATLASSERT(::IsWindow(hWnd));
		int nResult = (int) ShellExecute(hWnd, TEXT("edit"), m_strFilePath, NULL, NULL, SW_SHOWNORMAL);
		return nResult > 32;
	}

	BOOL UpdateSettings() {
		BOOL bResult = FALSE;
		if (!m_bUpdating) {
			m_bUpdating = TRUE;
			CSettings s;
			s.Initialize(m_strFilePath);
			if (m_s != s) {
				m_s = s;
				bResult = TRUE;
			}
			m_bUpdating = FALSE;
		}
		return bResult;
	}

};