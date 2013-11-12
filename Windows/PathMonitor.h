#pragma once

/////////////////////////////////////////////////////////////////////////////
// Path Monitor Class
//
// A small class that uses the FindFirstChangeNotification API
// to detect changes in a file path.
// A new thread is started to monitor changes. The thread will
// send a window message whenever the files in the path change.
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Corrections by David Burg (dburg@nero.com).
// Copyright (c) 2000/2001 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//


#if !defined(_ATL_MIN_CRT) & defined(_MT)
#include <process.h>
#endif

#ifndef _ASSERTE
#define _ASSERTE(x) (x)
#endif // _ASSERTE

#define PATH_CHANGED_MESSAGE_HANDLER(pm, func) \
    if(uMsg == pm.m_uPathChangedMessage && lParam == (LPARAM) &pm) \
    { \
        bHandled = TRUE; \
        lResult = func(bHandled); \
        if(bHandled) \
            return TRUE; \
    }

class CPathMonitor
{
private:
    TCHAR m_szPath[MAX_PATH];
    HWND m_hWnd;
    HANDLE m_hEvent;
    DWORD m_dwChangeFlag;
    DWORD m_dwDelay;
    BOOL m_bWatchSubtree;
    HANDLE m_hThread;

public:
    UINT m_uPathChangedMessage;

    CPathMonitor()
        : m_hThread( NULL ), m_hEvent( NULL ), m_hWnd( NULL ),
          m_dwChangeFlag( FILE_NOTIFY_CHANGE_LAST_WRITE ), m_bWatchSubtree( TRUE ),
          m_dwDelay( 0L ) {
        m_uPathChangedMessage = ::RegisterWindowMessage(
                                    TEXT( "PathMonitor_PathChangedMessage" ) );
        _ASSERTE( m_uPathChangedMessage != 0 );
    }

    ~CPathMonitor() {
        Uninstall();
    }

    BOOL Install( HWND hWnd, LPCTSTR pszPath ) {
        _ASSERTE( ::IsWindow( hWnd ) );
        _ASSERTE( !::IsBadStringPtr( pszPath, MAX_PATH ) );
        Uninstall();
        m_hWnd = hWnd;
        ::lstrcpy( m_szPath, pszPath );
        m_hEvent = ::CreateEvent( NULL, false, false, NULL );
        if( m_hEvent == NULL ) return FALSE;
        DWORD dwThreadID;
#if !defined(_ATL_MIN_CRT) & defined(_MT)
        m_hThread = ( HANDLE )_beginthreadex( NULL, 0,
                                              ( UINT( WINAPI * )( void * ) )MonitorProc, this, 0, ( UINT * )&dwThreadID );
#else
        m_hThread = ::CreateThread( NULL, 0, MonitorProc, this, 0, &dwThreadID );
#endif
        BOOL res = ( m_hThread != NULL );
        if( !res ) {
            ::CloseHandle( m_hEvent );
            m_hEvent = NULL;
            ::CloseHandle( m_hThread );
            m_hThread = NULL;
        }
        return res;
    }

    void Uninstall() {
        if( m_hThread == NULL ) return;
        ::SetEvent( m_hEvent ); // tell monitor that we transitioned to zero
        ::WaitForMultipleObjects( 1, &m_hThread, TRUE, INFINITE );
        if( m_hEvent != NULL ) {
            ::CloseHandle( m_hEvent );
            m_hEvent = NULL;
        }
        if( m_hThread != NULL ) {
            ::CloseHandle( m_hThread );
            m_hThread = NULL;
        }
    }

    void SetChangeFlag( DWORD dwFlag ) {
        _ASSERTE( m_hThread == NULL ); // not effective on active thread
        m_dwChangeFlag = dwFlag;
    }

    void SetSubtreeWatch( BOOL bState ) {
        _ASSERTE( m_hThread == NULL ); // not effective on active thread
        m_bWatchSubtree = bState;
    }

    void SetDelay( DWORD dwDelay ) {
        m_dwDelay = dwDelay;
    }

    static DWORD WINAPI MonitorProc( LPVOID pv ) {
        CPathMonitor * p = ( CPathMonitor * ) pv;
        _ASSERTE( p );
        bool bContinue = true;
        // Get a handle to a file change notification object.
        HANDLE hChange = ::FindFirstChangeNotification( p->m_szPath,
                         p->m_bWatchSubtree,
                         p->m_dwChangeFlag );
        // Return now if FindFirstChangeNotification() failed.
        if( hChange == INVALID_HANDLE_VALUE ) goto done;
        HANDLE aHandles[2];
        aHandles[0] = hChange;
        aHandles[1] = p->m_hEvent;
        // Sleep until a file change notification wakes this thread or
        // m_hEvent becomes set indicating it's time for the thread to end.
        while( bContinue ) {
            if( ::WaitForMultipleObjects( 2, aHandles, FALSE,
                                          INFINITE ) - WAIT_OBJECT_0 == 0 ) {
                // Wait if delay of messaged was asked.
                if( p->m_dwDelay > 0 ) ::Sleep( p->m_dwDelay );
                // Respond to a change notification.
                ::PostMessage( p->m_hWnd, p->m_uPathChangedMessage, 0, ( LPARAM ) p );
                ::FindNextChangeNotification( hChange );
            } else  {
                // Kill this thread (m_hEvent became signaled).
                bContinue = false;
            }
        }
        // Close the file change notification handle and return...
        ::FindCloseChangeNotification( hChange );
done:
#if !defined(_ATL_MIN_CRT) & defined(_MT)
        _endthreadex( 0 );
#endif
        return 0;
    }
};
