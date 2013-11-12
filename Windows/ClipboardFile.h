#pragma once

LPCWSTR ClipboardFileTextPrefix = L"UNICODETEXT\r\n";

class CClipboardFile
{

    BOOL WriteFile() {
        CFile file;
        if( file.Create( m_strFilePath ) ) {
            ATLVERIFY( file.Write( "\xFF\xFE", 2 * sizeof CHAR ) );
            ATLVERIFY( file.Write( ClipboardFileTextPrefix,
                                   ( DWORD ) wcslen( ClipboardFileTextPrefix ) * sizeof WCHAR ) );
            if( m_wstrContent.IsEmpty() )
                ATLVERIFY( file.SetEOF() );
            else
                ATLVERIFY( file.Write( ( LPCWSTR ) m_wstrContent,
                                       m_wstrContent.GetLength() * sizeof WCHAR ) );
            ATLVERIFY( file.SetEOF() );
            return TRUE;
        }
        return FALSE;
    }

    BOOL ReadFile() {
        CFile file;
        if( file.Open( m_strFilePath ) ) {
            CStringW wstrContent;
            CHAR szBuf[32768];
            DWORD dwRead;
            while( file.Read( szBuf, sizeof szBuf, &dwRead ) ) {
                if( dwRead == 0 )
                    break;
                LPCWSTR pszBuf = ( LPCWSTR ) szBuf;
                DWORD dwAppend = dwRead / sizeof WCHAR;
                if( wstrContent.IsEmpty() && dwRead >= 2 &&
                        ( UCHAR ) szBuf[0] == 0xFF && ( UCHAR ) szBuf[1] == 0xFE ) {
                    ++pszBuf;
                    --dwAppend;
                }
                wstrContent.Append( pszBuf, dwAppend );
            }
            if( wstrContent.Find( ClipboardFileTextPrefix ) == 0 )
                wstrContent.Delete( 0, ( int ) wcslen( ClipboardFileTextPrefix ) );
            if( m_wstrContent != wstrContent ) {
                m_wstrContent = wstrContent;
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL LoadClipboard() {
        if( !m_wstrContent.IsEmpty() ) {
            HGLOBAL hglb = GlobalAlloc( GMEM_MOVEABLE,
                                        ( m_wstrContent.GetLength() + 1 ) * sizeof WCHAR );
            if( hglb != NULL ) {
                LPWSTR lpwstr = ( LPWSTR ) GlobalLock( hglb );
                if( lpwstr != NULL ) {
                    CStringW::CopyChars( lpwstr, m_wstrContent.GetLength(),
                                         m_wstrContent, m_wstrContent.GetLength() );
                    lpwstr[m_wstrContent.GetLength()] = ( WCHAR ) 0;
                    if( GlobalUnlock( hglb ) == 0 ) {
                        SetClipboardData( CF_UNICODETEXT, hglb );
                        return TRUE;
                    }
                }
            }
        }
        return FALSE;
    }

    BOOL SaveClipboard( HGLOBAL hglb ) {
        BOOL bResult = FALSE;
        LPWSTR lpwstr = ( LPWSTR ) GlobalLock( hglb );
        if( lpwstr != NULL ) {
            if( m_wstrContent != lpwstr ) {
                m_wstrContent = lpwstr;
                bResult = WriteFile();
            }
            GlobalUnlock( hglb );
        }
        return bResult;
    }

    BOOL m_bUpdating;

public:

    CStringW m_wstrContent;
    CString m_strFilePath;
    CFileInspector m_fi;

    CClipboardFile() : m_bUpdating( FALSE ) {}

    BOOL Initialize( CString const & strFilePath ) {
        ATLASSERT( sizeof CHAR == 1 );
        ATLASSERT( sizeof WCHAR == 2 * sizeof CHAR );
        ATLASSERT( !strFilePath.IsEmpty() );
        m_strFilePath = strFilePath;
        return m_fi.Initialize( m_strFilePath );
    }

    BOOL UpdateFile( HWND hWnd ) {
        ATLASSERT( ::IsWindow( hWnd ) );
        BOOL bResult = FALSE;
        if( !m_bUpdating ) {
            m_bUpdating = TRUE;
            UINT auPriorityList[] = { CF_UNICODETEXT };
            int uFormat = GetPriorityClipboardFormat( auPriorityList,
                          sizeof auPriorityList / sizeof auPriorityList[0] );
            if( uFormat > 0 ) {
                if( OpenClipboard( hWnd ) ) {
                    HGLOBAL hglb = GetClipboardData( uFormat );
                    if( hglb != NULL ) {
                        switch( uFormat ) {
                            case CF_UNICODETEXT:
                                bResult = SaveClipboard( hglb );
                        }
                    }
                    CloseClipboard();
                }
            } else if( uFormat == 0 ) {
                m_wstrContent.Empty();
                bResult = WriteFile();
            }
            m_bUpdating = FALSE;
        }
        return bResult;
    }

    BOOL UpdateClipboard( HWND hWnd ) {
        ATLASSERT( ::IsWindow( hWnd ) );
        BOOL bResult = FALSE;
        if( !m_bUpdating ) {
            m_bUpdating = TRUE;
            if( ReadFile() ) {
                if( OpenClipboard( hWnd ) ) {
                    EmptyClipboard();
                    bResult = LoadClipboard();
                    CloseClipboard();
                }
            }
            m_bUpdating = FALSE;
        }
        return bResult;
    }

};