#pragma once

#define CLIPBOARD_CHANGED_MESSAGE_HANDLER(cm, func) \
    if(uMsg == cm.m_uClipboardChangedMessage && lParam == (LPARAM) &cm) { \
        bHandled = TRUE; \
        lResult = func(bHandled); \
        if(bHandled) \
            return TRUE; \
    }

class CClipboardMonitor
{

    HWND m_hwndNextClipboardViewer;

public:

    HWND m_hwndClipboardViewer;
    UINT m_uClipboardChangedMessage;

    CClipboardMonitor() {
        m_uClipboardChangedMessage = ::RegisterWindowMessage(
                                         TEXT( "ClipboardMonitor_ClipboardChangedMessage" ) );
        ATLASSERT( m_uClipboardChangedMessage != 0 );
    }

    ~CClipboardMonitor() {
        Uninstall();
    }

    BEGIN_MSG_MAP( CClipboardMonitor )
    MESSAGE_HANDLER( WM_CHANGECBCHAIN, OnChangeCliboardChain )
    MESSAGE_HANDLER( WM_DRAWCLIPBOARD, OnDrawCliboard )
    END_MSG_MAP()

    BOOL Install( HWND hWnd ) {
        ATLASSERT( ::IsWindow( hWnd ) );
        m_hwndClipboardViewer = hWnd;
        m_hwndNextClipboardViewer = SetClipboardViewer( m_hwndClipboardViewer );
        return m_hwndNextClipboardViewer != NULL;
    }

    BOOL Uninstall() {
        if( m_hwndNextClipboardViewer != NULL ) {
            ChangeClipboardChain( m_hwndClipboardViewer, m_hwndNextClipboardViewer );
            m_hwndNextClipboardViewer = NULL;
        }
        return TRUE;
    }

    LPVOID GetData() {
        //IDataObject * pDataObj = 0;
        //if (OleGetClipboard(&pDataObj) == S_OK)
        //  return pDataObj;
        //    dataObject->Release();
        //return 0;
    }

    LRESULT OnChangeCliboardChain( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   BOOL & /*bHandled*/ ) {
        if( ( HWND ) wParam == m_hwndNextClipboardViewer )
            m_hwndNextClipboardViewer = ( HWND ) lParam;
        else if( m_hwndNextClipboardViewer != NULL )
            SendMessage( m_hwndNextClipboardViewer, uMsg, wParam, lParam );
        return 0;
    }

    LRESULT OnDrawCliboard( UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam,
                            BOOL & /*bHandled*/ ) {
        SendMessage( m_hwndClipboardViewer, m_uClipboardChangedMessage, 0,
                     ( LPARAM ) this );
        if( m_hwndNextClipboardViewer != NULL )
            SendMessage( m_hwndNextClipboardViewer, uMsg, wParam, lParam );
        return 0;
    }

};