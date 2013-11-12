// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainFrame :
    public CWindowImpl<CMainFrame, CWindow, CFrameWinTraits>,
    public CUpdateUI<CMainFrame>,
    public CMessageFilter,
    public CIdleHandler
{

    CSettingsFile m_sf;
    CClipboardFile m_cf;
    CPathMonitor m_pmSettingsFile;
    CPathMonitor m_pmClipboardFile;
    CClipboardMonitor m_cm;
    CTaskBarIcon m_ti;

    BOOL Initialize() {
        m_cf.Initialize( m_sf.m_s.m_strClipboardFile );
        CPath pathClipboard( m_cf.m_strFilePath );
        ATLVERIFY( pathClipboard.RemoveFileSpec() );
        m_pmClipboardFile.Install( m_hWnd, pathClipboard );
        return TRUE;
    }

    BOOL Uninitialize() {
        m_pmClipboardFile.Uninstall();
        return TRUE;
    }

public:

    CMainFrame() {}

    virtual BOOL PreTranslateMessage( MSG * /*pMsg*/ ) {
        return FALSE;
    }

    virtual BOOL OnIdle() {
        return FALSE;
    }

    BEGIN_UPDATE_UI_MAP( CMainFrame )
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP( CMainFrame )
    MESSAGE_HANDLER( WM_CREATE, OnCreate )
    MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
    COMMAND_ID_HANDLER( ID_SYNCLIP_CONFIGURE, OnSynClipConfigure )
    COMMAND_ID_HANDLER( ID_SYNCLIP_ENABLE, OnSynClipEnable )
    COMMAND_ID_HANDLER( ID_SYNCLIP_ENABLE, OnSynClipNotify )
    COMMAND_ID_HANDLER( ID_APP_ABOUT, OnAppAbout )
    COMMAND_ID_HANDLER( ID_APP_EXIT, OnAppExit )
    MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
    MESSAGE_HANDLER( WM_INITMENUPOPUP, OnTaskbarContextMenuInit )
    TASKBAR_MESSAGE_HANDLER( m_ti, WM_LBUTTONDOWN, OnTaskIconClick )
    CLIPBOARD_CHANGED_MESSAGE_HANDLER( m_cm, OnCliboardChanged )
    PATH_CHANGED_MESSAGE_HANDLER( m_pmClipboardFile, OnClipboardPathChanged )
    PATH_CHANGED_MESSAGE_HANDLER( m_pmSettingsFile, OnSettingsPathChanged )
    CHAIN_MSG_MAP_MEMBER( m_ti )
    CHAIN_MSG_MAP_MEMBER( m_cm )
    CHAIN_MSG_MAP( CUpdateUI<CMainFrame> )
    END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//  LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//  LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//  LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCreate( UINT /*uMsg*/,
                      WPARAM /*wParam*/,
                      LPARAM /*lParam*/,
                      BOOL & /*bHandled*/ ) {
        m_sf.Initialize();
        CPath pathSettings( m_sf.m_strFilePath );
        ATLVERIFY( pathSettings.RemoveFileSpec() );
        m_pmSettingsFile.Install( m_hWnd, pathSettings );
        Initialize();
        m_cm.Install( m_hWnd );
        m_ti.Install( m_hWnd, 1, IDR_TASKBAR );
        CMessageLoop * pLoop = _Module.GetMessageLoop();
        ATLASSERT( pLoop != NULL );
        pLoop->AddMessageFilter( this );
        pLoop->AddIdleHandler( this );
        return 0;
    }

    LRESULT OnDestroy( UINT /*uMsg*/,
                       WPARAM /*wParam*/,
                       LPARAM /*lParam*/,
                       BOOL & bHandled ) {
        m_pmSettingsFile.Uninstall();
        m_cm.Uninstall();
        m_ti.Uninstall();
        CMessageLoop * pLoop = _Module.GetMessageLoop();
        ATLASSERT( pLoop != NULL );
        pLoop->RemoveMessageFilter( this );
        pLoop->RemoveIdleHandler( this );
        ::PostQuitMessage( 1 );
        bHandled = FALSE;
        return 1;
    }

    LRESULT OnTaskbarContextMenuInit( UINT /*uMsg*/,
                                      WPARAM wParam,
                                      LPARAM /*lParam*/,
                                      BOOL & /*bHandled*/ ) {
        HMENU hSubMenu = ( HMENU ) wParam;
        ATLASSERT( ::IsMenu( hSubMenu ) );
        CheckMenuItem( hSubMenu, ID_SYNCLIP_ENABLE, MF_BYCOMMAND |
                       ( m_sf.m_s.m_bSychronizingEnabled ? MF_CHECKED : MF_UNCHECKED ) );
        CheckMenuItem( hSubMenu, ID_SYNCLIP_NOTIFY, MF_BYCOMMAND |
                       ( m_sf.m_s.m_bNotifyOnClipboardChange ? MF_CHECKED : MF_UNCHECKED ) );
        return 0;
    }

    LRESULT OnTaskIconClick( LPARAM /*uMsg*/,
                             BOOL & bHandled ) {
        OnSynClipEnable( 0, 0, 0, bHandled );
        return 0;
    }

    LRESULT OnSynClipConfigure( WORD /*wNotifyCode*/,
                                WORD /*wID*/,
                                HWND /*hWndCtl*/,
                                BOOL & /*bHandled*/ ) {
        m_sf.EditFile( m_hWnd );
        return 0;
    }

    LRESULT OnSynClipEnable( WORD /*wNotifyCode*/,
                             WORD /*wID*/,
                             HWND /*hWndCtl*/,
                             BOOL & /*bHandled*/ ) {
        m_sf.m_s.m_bSychronizingEnabled = !m_sf.m_s.m_bSychronizingEnabled;
        return 0;
    }

    LRESULT OnSynClipNotify( WORD /*wNotifyCode*/,
                             WORD /*wID*/,
                             HWND /*hWndCtl*/,
                             BOOL & /*bHandled*/ ) {
        m_sf.m_s.m_bNotifyOnClipboardChange = !m_sf.m_s.m_bNotifyOnClipboardChange;
        return 0;
    }

    LRESULT OnAppAbout( WORD /*wNotifyCode*/,
                        WORD /*wID*/,
                        HWND /*hWndCtl*/,
                        BOOL & /*bHandled*/ ) {
        CAboutDlg dlg;
        dlg.DoModal();
        return 0;
    }

    LRESULT OnAppExit( WORD /*wNotifyCode*/,
                       WORD /*wID*/,
                       HWND /*hWndCtl*/,
                       BOOL & /*bHandled*/ ) {
        PostMessage( WM_CLOSE );
        return 0;
    }

    LRESULT OnCliboardChanged( BOOL & /*bHandled*/ ) {
        if( m_sf.m_s.m_bSychronizingEnabled )
            m_cf.UpdateFile( m_hWnd );
        return 0;
    }

    LRESULT OnClipboardPathChanged( BOOL & /*bHandled*/ ) {
        if( m_sf.m_s.m_bSychronizingEnabled && m_cf.m_fi.HasChanged() )
            if( m_cf.UpdateClipboard( m_hWnd ) && m_sf.m_s.m_bNotifyOnClipboardChange )
                m_ti.ShowInfoTooltip( IDS_NOTIFY_TITLE, IDS_NOTIFY_MESSAGE );
        return 0;
    }

    LRESULT OnSettingsPathChanged( BOOL & /*bHandled*/ ) {
        if( m_sf.m_fi.HasChanged() && m_sf.UpdateSettings() ) {
            Uninitialize();
            Initialize();
        }
        return 0;
    }

};
