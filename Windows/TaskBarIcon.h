#ifndef __TASKBARICON_H__
#define __TASKBARICON_H__

/////////////////////////////////////////////////////////////////////////////
// Task Bar Icon class
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#pragma once

#ifndef __cplusplus
   #error WTL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
   #error TaskBarIcon.h requires atlwin.h to be included first
#endif

#if (_WIN32_IE < 0x0400)
   #error TaskBarIcon.h requires _WIN32_IE >= 0x0400
#endif


#define TASKBAR_MESSAGE_HANDLER(ti, msg, func) \
   if(uMsg==ti.m_nid.uCallbackMessage && wParam==ti.m_nid.uID && lParam==msg) \
   { \
      bHandled = TRUE; \
      lResult = func(lParam, bHandled); \
      if(bHandled) \
         return TRUE; \
   }


class CTaskBarIcon
{
public:
   NOTIFYICONDATA m_nid; 
   HMENU m_hMenu;
   UINT m_iTaskbarRestartMsg;

   CTaskBarIcon() : m_hMenu(NULL)
   {
      ::ZeroMemory(&m_nid, sizeof(m_nid));
      m_nid.cbSize = sizeof(m_nid); 
      m_nid.uCallbackMessage = ::RegisterWindowMessage(TEXT("TaskbarNotifyMsg"));
      m_iTaskbarRestartMsg = ::RegisterWindowMessage(TEXT("TaskbarCreated"));
   }
   ~CTaskBarIcon()
   {
      Uninstall();
   }

   BEGIN_MSG_MAP(CTaskBarIcon)
      MESSAGE_HANDLER(m_iTaskbarRestartMsg, OnTaskbarRestart)
      TASKBAR_MESSAGE_HANDLER((*this), WM_RBUTTONDOWN, OnTaskbarContextMenu)
   END_MSG_MAP()

   BOOL Install(HWND hWnd, UINT iID, HICON hIcon, HMENU hMenu, LPTSTR lpszTip = NULL)
   {
      ATLASSERT(::IsWindow(hWnd));
      ATLASSERT(m_hMenu==NULL);
      ATLASSERT(m_nid.hIcon==NULL);
      m_nid.hWnd = hWnd;
      m_nid.uID = iID;
      m_nid.hIcon = hIcon; 
      ::lstrcpyn(m_nid.szTip, (lpszTip != NULL ? lpszTip : _T("")), sizeof(m_nid.szTip)/sizeof(TCHAR)); 
      m_hMenu = hMenu;
      return AddTaskBarIcon();
   }
   BOOL Install(HWND hWnd, UINT iID, UINT nRes)
   {
      ATLASSERT(::IsWindow(hWnd));
      ATLASSERT(m_hMenu==NULL);
      ATLASSERT(m_nid.hIcon==NULL);
      m_nid.hWnd = hWnd;
      m_nid.uID = iID;
      m_nid.hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
      m_nid.szTip[0] = '\0';
      ::LoadString(_Module.GetResourceInstance(), nRes, m_nid.szTip, sizeof(m_nid.szTip)/sizeof(TCHAR));
      m_hMenu = ::LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(nRes));
      return AddTaskBarIcon();
   }
   BOOL Uninstall()
   {
      BOOL res = TRUE;
      if( m_nid.hWnd != NULL ) res = DeleteTaskBarIcon();
      m_nid.hWnd = NULL;
      if( m_nid.hIcon != NULL ) ::DestroyIcon(m_nid.hIcon);
      m_nid.hIcon = NULL;
      if( m_hMenu != NULL ) ::DestroyMenu(m_hMenu);
      m_hMenu = NULL;
      return res;
   }
   BOOL IsInstalled() const
   {
      return m_nid.hWnd!=NULL;
   }
   void SetIcon(HICON hIcon) 
   { 
      ATLASSERT(m_nid.hIcon==NULL);
      m_nid.hIcon = hIcon; 
   }
   void SetMenu(HMENU hMenu) 
   { 
      ATLASSERT(m_hMenu==NULL);
      ATLASSERT(::IsMenu(hMenu));
      m_hMenu = hMenu; 
   }
   BOOL AddTaskBarIcon()
   {
      ATLASSERT(::IsWindow(m_nid.hWnd));
      m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
      BOOL res = ::Shell_NotifyIcon(NIM_ADD, &m_nid); 
      return res;
   }
   BOOL ChangeIcon(HICON hIcon)
   {
      // NOTE: The class takes ownership of the icon!
      ATLASSERT(::IsWindow(m_nid.hWnd));
      if( m_nid.hIcon != NULL ) ::DestroyIcon(m_nid.hIcon);
      m_nid.uFlags = NIF_ICON; 
      m_nid.hIcon = hIcon;
      BOOL res = ::Shell_NotifyIcon(NIM_MODIFY, &m_nid); 
      return res;
   }
   BOOL DeleteTaskBarIcon() 
   { 
       return ::Shell_NotifyIcon(NIM_DELETE, &m_nid); 
   } 
   BOOL ShowInfoTooltip(UINT nInfoTitleRes, UINT nInfoRes)
   {
      ATLASSERT(::IsWindow(m_nid.hWnd));
      NOTIFYICONDATA nid; 
      ::ZeroMemory(&nid, sizeof(nid));
      nid.cbSize = sizeof(nid); 
      nid.hWnd = m_nid.hWnd;
      nid.uID = m_nid.uID;
	  nid.szInfoTitle[0] = '\0';
      ::LoadString(_Module.GetResourceInstance(), nInfoTitleRes,
		  nid.szInfoTitle, sizeof(nid.szInfoTitle)/sizeof(TCHAR));
	  nid.szInfo[0] = '\0';
      ::LoadString(_Module.GetResourceInstance(), nInfoRes,
		  nid.szInfo, sizeof(nid.szInfo)/sizeof(TCHAR));
      nid.uFlags = NIF_INFO; 
	  nid.dwInfoFlags = NIIF_INFO;
      BOOL res = ::Shell_NotifyIcon(NIM_MODIFY, &nid); 
      return res;
   }

   // Message handlers

   LRESULT OnTaskbarRestart(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      AddTaskBarIcon();
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnTaskbarContextMenu(LPARAM /*uMsg*/, BOOL& bHandled)
   {
      if( !::IsMenu(m_hMenu) ) {
         bHandled = FALSE;
         return 0;
      }
      HMENU hSubMenu = ::GetSubMenu(m_hMenu, 0);   
      ATLASSERT(::IsMenu(hSubMenu));
      // Make first menu-item the default (bold font)
      //::SetMenuDefaultItem(hSubMenu, 0, TRUE); 
      // Display the menu at the current mouse location.
      POINT pt = { 0 };
      ::GetCursorPos(&pt);      
      ::SetForegroundWindow(m_nid.hWnd);       // Fixes Win95 bug; see Q135788
      ::TrackPopupMenu(hSubMenu, 0, pt.x, pt.y, 0, m_nid.hWnd, NULL);
      ::PostMessage(m_nid.hWnd, WM_NULL, 0,0); // Fixes another Win95 bug
      return 0;
   }
};

#endif // __TASKBARICON_H__
