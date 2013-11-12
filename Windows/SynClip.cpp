// SynClip.cpp : main source file for SynClip.exe
//

#include "stdafx.h"

#include <userenv.h>
#pragma comment(lib, "userenv")

#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>
#include <atlsync.h>
#include <atlstr.h>
#include <atlutil.h>
#include <atltime.h>
#include <atlpath.h>

#include "resource.h"

#include "atlwfile.h"
#include "Ini.h"
#include "TaskBarIcon.h"
#include "ClipboardMonitor.h"
#include "PathMonitor.h"
#include "FileInspector.h"
#include "Settings.h"
#include "SettingsFile.h"
#include "ClipboardFile.h"
#include "AboutDlg.h"
#include "MainFrm.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL,
		int /*nCmdShow*/ = SW_SHOWDEFAULT) {
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if (wndMain.Create(NULL) == NULL) {
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance,
					 HINSTANCE /*hPrevInstance*/,
					 LPTSTR lpstrCmdLine,
					 int nCmdShow) {
	HRESULT hRes = OleInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when
	// Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	OleUninitialize();

	return nRet;
}
