#pragma once

class CFileInspector {

	CString m_strFilePath;
	CTime m_tmLastChanged;

	CTime GetLastChanged() {
		CFile f;
		if (f.Open(m_strFilePath)) {
			FILETIME ftCreate, ftAccess, ftModified;
			if (f.GetFileTime(&ftCreate, &ftAccess, &ftModified))
				return ftModified;
		}
		return CTime();
	}

public:

	CFileInspector() {}

	BOOL Initialize(CString const & strFilePath) {
		ATLASSERT(!strFilePath.IsEmpty());
		m_strFilePath = strFilePath;
		m_tmLastChanged = GetLastChanged();
		return m_tmLastChanged.GetTime() != 0;
	}

	BOOL HasChanged() {
		CTime tmLastChanged = GetLastChanged();
		if (tmLastChanged > m_tmLastChanged) {
			m_tmLastChanged = tmLastChanged;
			return TRUE;
		}
		return FALSE;
	}

};