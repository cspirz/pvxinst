// pvxinst.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "exdll.h"
#include <commctrl.h>
#include <process.h>
#include <string>

#define IDC_PROGRESSBAR    1004
#define IDC_PROGRESSBARNEW 1005
#define IDC_PROGRESSTEXT   1006
#define IDC_PROGRESSLIST   1016

HINSTANCE g_hInst;
HWND g_hWndParent;
UINT g_uMsgCreate;


CPathA m_path;
CAtlList<_bstr_t> m_ComponentList;

// HWND of our child window.
HWND g_hChildWnd;
// HWND of our own progress bar control.
HWND g_hProgressBarNew = FALSE;
// Stores the progress bar value to be added.
int g_iProgressBarAdd;
// Stores the total progress bar position.
int g_iProgressBarPos;
// Have we already added our own progress bar?
BOOL g_bProgressBarAdded;
// Set to true when we want to stop processing the current file extraction.
BOOL g_bFileProgressDone = TRUE;
// Handle to our thread that gradually increases the progress bar over a period of time.
HANDLE g_hGradualProgressThread;
// Number of seconds until next progress bar increase and the amount to increase by.
int g_iGradualProgressSeconds, g_iProgressBarIncrease;
// Enable/disable GradualProgress threads and window procedures.
BOOL g_bGradualProgressDone = TRUE,
// This is set to TRUE while a thread is running.
g_bGradualProgressThreadRunning,
// For GradualProgress should we check if a new item added = StopAt (below).
g_bGradualProgressStop;
// If this the new item = below, then stop increasing the progress bar.
char g_szGradualProgressStopAt[32];

// Stores pointer to NSIS's child window procedure.
WNDPROC WndProcOld;
// Stores pointer to NSIS's child dialog procedure.
DLGPROC DlgProcOld;

//#ifndef SetWindowLongPtr
//#define SetWindowLongPtr SetWindowLong
//#endif

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

#ifndef DWLP_DLGPROC
#define DWLP_DLGPROC DWL_DLGPROC
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC GWL_WNDPROC
#endif

int str2int(char *p);

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		g_hInst = (HINSTANCE)hModule;
	}
	case DLL_PROCESS_DETACH:
	{
	}
	}
	return TRUE;
}
extern "C" void __declspec(dllexport) CheckCom(HWND hwndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CComPtr<IScript> spPVX;
	EXDLL_INIT();

	HRESULT hr = spPVX.CoCreateInstance(__uuidof(Script));
	if (SUCCEEDED(hr)) {
		pushstring("1");
	}
	else {
		pushstring("0");
	}

}

extern "C" void __declspec(dllexport) CheckVersion(HWND hwndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CComPtr<IScript> spPVX;
	CComPtr<IPvxDispatch> spNewObj;

	EXDLL_INIT();
	CAtlString m_szPath(getuservariable(INST_INSTDIR));
	m_path.Append(m_szPath);
	m_path.AddBackslash();

	LPOLESTR szMember = L"sGetVersion";
	LPOLESTR szProp = L"SYSTEM";
	_bstr_t bsInitPath(m_path + "HOME");
	_variant_t varEmpty;
	_bstr_t bsInitString("");
	_bstr_t bsComponent("SYSTEM");
	_variant_t varResult;
	_bstr_t bsClass("INSTALLATION");
	CComVariant var;

	DISPID dispid_getmodulelevels = 0;
	DISPID dispid_newVersion = 0;

	DISPPARAMS dispparam = { NULL, NULL, 0, 0 };

	HRESULT hr = spPVX.CoCreateInstance(__uuidof(Script));
	if (SUCCEEDED(hr))
	{
		hr = spPVX->Init(bsInitPath, bsInitString);
		if (SUCCEEDED(hr))
		{
			ScriptState st;
			hr = spPVX->get_State(&st);
			if (SUCCEEDED(hr))
			{
				spNewObj = spPVX->NewObject(bsClass, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty);
				if (spNewObj)
					if (SUCCEEDED(hr))
						hr = spNewObj->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid_getmodulelevels);
				{

					hr = spNewObj->Invoke(dispid_getmodulelevels, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparam, &varResult, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						if (varResult.bstrVal)
						{
							CAtlString s(varResult.bstrVal);
							pushstring(s.Left(4));
						}
						else
						{
							pushstring("0");
						}

					}
				}
			}
		}
	}
}


extern "C" void __declspec(dllexport) CheckLMAVVersion(HWND hwndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CComPtr<IScript> spPVX;
	CComPtr<IPvxDispatch> spNewObj;

	EXDLL_INIT();
	CAtlString m_szPath(getuservariable(INST_INSTDIR));
	m_path.Append(m_szPath);
	m_path.AddBackslash();

	LPOLESTR szMember = L"nGetLevel";
	LPOLESTR szProp = L"P/X";
	_bstr_t bsInitPath(m_path + "HOME");
	_variant_t varEmpty;
	_bstr_t bsInitString("");
	_bstr_t bsComponent("P/X");
	_variant_t varResult;
	_bstr_t bsClass("INSTALLATION");
	CComVariant var;

	DISPID dispid_getmodulelevels = 0;
	DISPID dispid_newVersion = 0;

	DISPPARAMS dispparam = { NULL, NULL, 0, 0 };

	HRESULT hr = spPVX.CoCreateInstance(__uuidof(Script));
	if (SUCCEEDED(hr))
	{
		hr = spPVX->Init(bsInitPath, bsInitString);
		if (SUCCEEDED(hr))
		{
			ScriptState st;
			hr = spPVX->get_State(&st);
			if (SUCCEEDED(hr))
			{
				spNewObj = spPVX->NewObject(bsClass, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty);
				if (spNewObj)
					if (SUCCEEDED(hr))
						hr = spNewObj->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid_getmodulelevels);
				{
					// DISPPARAMS HERE
					dispparam.rgvarg = new VARIANTARG[7];
					dispparam.rgvarg[0].vt = VT_I4;
					dispparam.rgvarg[1].vt = VT_I4;
					dispparam.rgvarg[2].vt = VT_I4;
					dispparam.rgvarg[3].vt = VT_I4;
					dispparam.rgvarg[4].vt = VT_I4;
					dispparam.rgvarg[5].vt = VT_I4;
					dispparam.rgvarg[6].vt = VT_BSTR;
					dispparam.rgvarg[6].bstrVal = bsComponent;
					dispparam.cArgs = 7;
					dispparam.cNamedArgs = 0;
					dispparam.rgdispidNamedArgs = NULL;


					hr = spNewObj->Invoke(dispid_getmodulelevels, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparam, &varResult, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						if (varResult.iVal > 0)
						{
							pushstring("1");
						}
						else
						{
							pushstring("0");
						}

					}
				}
			}
		}
	}
}

extern "C" void __declspec(dllexport) InstallPrepare(HWND hwndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CComPtr<IScript> spPVX;
	CComPtr<IPvxDispatch> spNewObj;

	EXDLL_INIT();
	CAtlString m_szPath(getuservariable(INST_INSTDIR));
	m_path.Append(m_szPath);
	m_path.AddBackslash();

	LPOLESTR szMember = L"nPrepare";
	_bstr_t bsInitPath(m_path + "HOME");
	_variant_t varEmpty;
	_bstr_t bsInitString("");
	_bstr_t bsComponent;
	_variant_t varResult;
	_bstr_t bsClass("INSTALLATION");

	DISPID dispid_prepare = 0;

	DISPPARAMS dispparam = { NULL, NULL, 0, 0 };

	HRESULT hr = spPVX.CoCreateInstance(__uuidof(Script));
	if (SUCCEEDED(hr))
	{
		hr = spPVX->Init(bsInitPath, bsInitString);
		if (SUCCEEDED(hr))
		{
			ScriptState st;
			hr = spPVX->get_State(&st);
			if (SUCCEEDED(hr))
			{
				spNewObj = spPVX->NewObject(bsClass, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty);
				if (spNewObj)
					if (SUCCEEDED(hr))
						hr = spNewObj->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid_prepare);
				{
					hr = spNewObj->Invoke(dispid_prepare, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparam, &varResult, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						pushstring("1");
						return;
					}
				}

			}
		}
	}
	pushstring("0");

}
extern "C" void __declspec(dllexport) LoadCom(HWND hwndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CComPtr<IScript> spPVX;
	CComPtr<IPvxDispatch> spNewObj;

	EXDLL_INIT();

	CAtlString m_szPath(getuservariable(INST_INSTDIR));
	m_path.Append(m_szPath);
	m_path.AddBackslash();

	_bstr_t bsInitPath(m_path + "HOME");
	_variant_t varEmpty;
	_bstr_t bsInitString("");
	_bstr_t bsComponent;
	_variant_t varResult;
	LPOLESTR szMember = L"nInstall";
	_bstr_t bsClass("INSTALLATION");
	BOOL bSuccess = false;
	HRESULT hr = spPVX.CoCreateInstance(__uuidof(Script));
	if (SUCCEEDED(hr))
	{
		hr = spPVX->Init(bsInitPath, bsInitString);
		if (SUCCEEDED(hr))
		{
			ScriptState st;
			hr = spPVX->get_State(&st);
			if (SUCCEEDED(hr))
			{
				spNewObj = spPVX->NewObject(bsClass, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty);
				if (spNewObj)
					pushstring("1");
				else
					pushstring("0");

			}
		}
	}

}
extern "C" void __declspec(dllexport) InstallModules(HWND hwndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CComPtr<IScript> spPVX;
	CComPtr<IPvxDispatch> spNewObj;

	EXDLL_INIT();

	CAtlString m_szPath(getuservariable(INST_INSTDIR));
	m_path.Append(m_szPath);
	m_path.AddBackslash();

	_bstr_t bsInitPath(m_path + "HOME");
	_variant_t varEmpty;
	_bstr_t bsInitString("");
	_bstr_t bsComponent;
	_variant_t varResult;
	LPOLESTR szMember = L"nInstall";
	_bstr_t bsClass("INSTALLATION");

	DISPPARAMS dispparam = { NULL, NULL, 0, 0 };
	DISPID dispid_install = 0;

	m_path += "BUILD\\INSTALL";
	if (!m_path.FileExists())
	{
		CAtlString sMsg("The INSTALL File was not found in " + m_szPath + "\\BUILD");
		pushstring(sMsg);
		pushstring("0");
		return;
	}
	CAtlFile file;
	HRESULT hr = file.Create(m_path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (FAILED(hr))
	{
		CAtlString sMsg("Error on File Open: " + m_path);
		pushstring(sMsg);
		pushstring("0");
		return;
	}
	// Build the List from INSTALL
	char buff[MAX_PATH] = { '\0' };
	DWORD dwBytesRead = 0;
	hr = file.Read(LPVOID(buff), sizeof(buff), dwBytesRead);
	while (dwBytesRead)
	{
		// Should have read the whole INSTALL line into the buff
		CAtlString strTo(buff);
		CAtlString resToken;
		int curPos = 0;
		resToken = strTo.Tokenize("'\n'", curPos);
		while (resToken != "")
		{
			if (resToken.GetLength() > 3)
				if (resToken.Mid(3, 1) = '\n')
					resToken.Truncate(3);
			if (resToken.Mid(1, 1) != "/" && resToken.Mid(0, 3) != "SYSTEM")
			{
				resToken.Truncate(2);
				resToken.Insert(1, "/");
			}
			else
				resToken.Truncate(3);
			_bstr_t module(resToken);
			m_ComponentList.AddHead(module);
			resToken = strTo.Tokenize("'\n'", curPos);
		}
		file.Read((LPVOID)buff, sizeof(buff), dwBytesRead);
	}

	file.Close();

	POSITION p = m_ComponentList.GetHeadPosition();
	if (p) // Ensures there was something in INSTALL
	{
		HRESULT hr = spPVX.CoCreateInstance(__uuidof(Script));
		if (SUCCEEDED(hr))
		{
			hr = spPVX->Init(bsInitPath, bsInitString);
			if (SUCCEEDED(hr))
			{
				ScriptState st;
				hr = spPVX->get_State(&st);
				if (SUCCEEDED(hr))
				{
					spNewObj = spPVX->NewObject(bsClass, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty);
					if (spNewObj)
					{

						bsComponent = m_ComponentList.GetAt(p);
						HRESULT hr = spNewObj->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid_install);
						if (SUCCEEDED(hr))
						{
							dispparam.rgvarg = new VARIANTARG[1];
							dispparam.rgvarg[0].vt = VT_BSTR;
							dispparam.rgvarg[0].bstrVal = bsComponent;
							dispparam.cArgs = 1;
							dispparam.cNamedArgs = 0;
							dispparam.rgdispidNamedArgs = NULL;
							hr = spNewObj->Invoke(dispid_install, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparam, &varResult, NULL, NULL);

						}
						pushstring("1");
						return;
					}
				}
			}
		}
	}
	pushstring("0");
}
extern "C" void __declspec(dllexport) InstallFinalize(HWND hwndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CComPtr<IScript> spPVX;
	CComPtr<IPvxDispatch> spNewObj;

	EXDLL_INIT();
	CAtlString m_szPath(getuservariable(INST_INSTDIR));
	m_path.Append(m_szPath);
	m_path.AddBackslash();

	LPOLESTR szMember = L"nFinalize";
	_bstr_t bsInitPath(m_path + "HOME");
	_variant_t varEmpty;
	_bstr_t bsInitString("");
	_bstr_t bsComponent;
	_variant_t varResult;
	_bstr_t bsClass("INSTALLATION");

	DISPID dispid_finalize = 0;

	DISPPARAMS dispparam = { NULL, NULL, 0, 0 };

	HRESULT hr = spPVX.CoCreateInstance(__uuidof(Script));
	if (SUCCEEDED(hr))
	{
		hr = spPVX->Init(bsInitPath, bsInitString);
		if (SUCCEEDED(hr))
		{
			ScriptState st;
			hr = spPVX->get_State(&st);
			if (SUCCEEDED(hr))
			{
				spNewObj = spPVX->NewObject(bsClass, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty, varEmpty);
				if (spNewObj)
					if (SUCCEEDED(hr))
						hr = spNewObj->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid_finalize);
				{
					hr = spNewObj->Invoke(dispid_finalize, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparam, &varResult, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						pushstring("1");
						return;
					}
				}

			}
		}
	}
	pushstring("0");

}
// Increases progress bar position var.
// Prevents the value being > 100.
void IncreaseProgressBarPos(int iBy)
{
	// Can't go over 100.
	if (g_iProgressBarPos + iBy > 100)
		g_iProgressBarPos = 100;
	// Can't go under 0.
	else if (g_iProgressBarPos + iBy < 0)
		g_iProgressBarPos = 0;
	else
		g_iProgressBarPos += iBy;
}
// Handles window messages and notifications sent from the child window.
static LRESULT CALLBACK WndProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// g_uMsgCreate is the message that we sent ourselves!
	if (uMsg == g_uMsgCreate)
	{
		if (!g_bProgressBarAdded)
		{
			HWND hProgressBar = GetDlgItem(hWndDlg, IDC_PROGRESSBAR);
			RECT rProgressBarPos;

			if (!(g_hProgressBarNew = GetDlgItem(hWndDlg, IDC_PROGRESSBARNEW)))
			{
				// Get the position and dimensions of the existing progress bar.
				GetWindowRect(hProgressBar, &rProgressBarPos);
				MapWindowPoints(HWND_DESKTOP, hWndDlg, (LPPOINT)&rProgressBarPos, 2);
				// Create the new progress bar in the same place.
				g_hProgressBarNew = CreateWindow
				(
					"msctls_progress32",
					"",
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					rProgressBarPos.left,
					rProgressBarPos.top,
					rProgressBarPos.right - rProgressBarPos.left,
					rProgressBarPos.bottom - rProgressBarPos.top,
					hWndDlg,
					(HMENU)IDC_PROGRESSBARNEW,
					g_hInst,
					NULL
				);

				// Set the style of the new progress bar to that of the old one.
				SetWindowLongPtr(g_hProgressBarNew, GWL_STYLE, GetWindowLongPtr(hProgressBar, GWL_STYLE));
			}

			// Hide the old progress bar and show the new one.
			ShowWindow(hProgressBar, SW_HIDE);
			ShowWindow(g_hProgressBarNew, SW_SHOWNA);

			// We've added our progress bar now!
			g_bProgressBarAdded = TRUE;

		}
		return FALSE;
	}
	else
		// Call the default window procedure if we haven't used our own.
		return CallWindowProc(WndProcOld, hWndDlg, uMsg, wParam, lParam);
}

// Handles window notifications sent from the child dialog for GradualProgress.
static LRESULT CALLBACK GradualProgressDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static NMHDR* pnmh;
	static HWND hDetails = GetDlgItem(g_hChildWnd, IDC_PROGRESSTEXT);
	static char szDetails[1024];
	if (uMsg == WM_NOTIFY)
	{
		pnmh = (NMHDR *)lParam;
		if (pnmh->idFrom == IDC_PROGRESSLIST && !g_bGradualProgressDone)
		{
			if (pnmh->code == LVN_INSERTITEM)
			{
				// We should check if the newly added item matches that of the item that we are after.
				if (g_bGradualProgressStop)
				{
					// Get the text of the newly added item.
					GetWindowText(hDetails, szDetails, sizeof(szDetails));
					// If it equals the item that we are after, then we must stop increasing the progress bar.
					if (lstrcmpi(szDetails, g_szGradualProgressStopAt) == 0)
						g_bGradualProgressDone = TRUE;
				}
				// We've finished.
				else
					g_bGradualProgressDone = TRUE;
			}
		}
	}
	// Call the default window procedure if we haven't used our own.
	return CallWindowProc((WNDPROC)DlgProcOld, hWndDlg, uMsg, wParam, lParam);
}

// Increases our progress bar by x per y seconds up to z!
BOOL WINAPI GradualProgressThread(LPVOID Param)
{
	int iProgressBarNewPos = g_iProgressBarPos - g_iProgressBarAdd,
		iGradualProgressIncrease = g_iProgressBarIncrease,
		iGradualProgressSeconds = g_iGradualProgressSeconds,
		iProgressBarEndPos = g_iProgressBarPos;

	g_bGradualProgressThreadRunning = TRUE;

	do
	{
		// Pause execution for set amount of time.
		Sleep(iGradualProgressSeconds * 1000);
		// Get the new progress bar value.
		iProgressBarNewPos += iGradualProgressIncrease;
		// Set the new progress bar value.
		SendMessage(g_hProgressBarNew, PBM_SETPOS, (LPARAM)(iProgressBarNewPos), 0);
	} while (iProgressBarNewPos < iProgressBarEndPos && !g_bGradualProgressDone);

	// Set the final progress bar position.
	SendMessage(g_hProgressBarNew, PBM_SETPOS, (LPARAM)iProgressBarEndPos, 0);

	g_bGradualProgressDone = TRUE;
	g_bGradualProgressThreadRunning = FALSE;

	return TRUE;
}

// Adds our own progress bar to the InstFiles page, replacing the original.
// The original will be hidden (not destroyed).
void AddProgressBar()
{
	// Reset our progress bar position value.
	g_iProgressBarPos = 0;

	// Get the child window handle.
	if (!g_hChildWnd)
		g_hChildWnd = FindWindowEx(g_hWndParent, NULL, "#32770", NULL);

	// Assign our child window procedure.
	g_uMsgCreate = RegisterWindowMessage("nsis pvxinst create");
	WndProcOld = (WNDPROC)SetWindowLongPtr(g_hChildWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
	// This causes the WndProc callback to be called so that we can do stuff right away!
	SendMessage(g_hChildWnd, g_uMsgCreate, TRUE, (LPARAM)g_hChildWnd);

}

extern "C" void __declspec(dllexport) GradualProgress(HWND hWndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	g_hWndParent = hWndParent;

	EXDLL_INIT();
	{
		char szParam[32];
		LPDWORD ThreadID = 0;

		// Attempt to get a parameter.
		if (popstring(szParam) == 0)
		{
			// Get the amount of time to increment the progress bar by.
			g_iGradualProgressSeconds = str2int(szParam);

			// Attempt to get a second parameter.
			if (popstring(szParam) == 0)
			{
				// Get the amount to increment the progress bar by each time.
				g_iProgressBarIncrease = str2int(szParam);

				// Attempt to get a third parameter.
				if (popstring(szParam) == 0)
				{
					// Get the amount to increment the progress bar by.
					g_iProgressBarAdd = str2int(szParam);

					// Attempt to get a fourth parameter.
					if (popstring(szParam) == 0)
					{
						lstrcpy(g_szGradualProgressStopAt, szParam);
						g_bGradualProgressStop = TRUE;
					}
					else
						g_bGradualProgressStop = FALSE;

					// If the plug in has not already been called,
					// add our new progress bar control.
					if (!g_bProgressBarAdded)
						AddProgressBar();

					// Assign our child dialog procedure.
					if (!DlgProcOld)
						DlgProcOld = (DLGPROC)SetWindowLongPtr(g_hChildWnd, DWLP_DLGPROC, (LONG_PTR)GradualProgressDlgProc);
					else
						SetWindowLongPtr(g_hChildWnd, DWLP_DLGPROC, (LONG_PTR)GradualProgressDlgProc);

					// If any previous threads are running, close them.
					if (g_hGradualProgressThread)
						CloseHandle(g_hGradualProgressThread);

					// If this was TRUE, our thread code would not be executed!
					g_bGradualProgressDone = TRUE;
					while (g_bGradualProgressThreadRunning)
					{
						Sleep(50);
					}
					g_bGradualProgressDone = FALSE;

					IncreaseProgressBarPos(g_iProgressBarAdd);

					// Start our thread to increase the progress bar.
					g_hGradualProgressThread = CreateThread(
						(LPSECURITY_ATTRIBUTES)NULL,
						0,
						(LPTHREAD_START_ROUTINE)GradualProgressThread,
						(LPVOID)NULL,
						0,
						(LPDWORD)ThreadID
					);

				}
			}
		}

	}
}
extern "C"
void __declspec(dllexport) Unload(HWND hWndParent, int string_size,
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	g_hWndParent = hWndParent;

	EXDLL_INIT();
	{
		// We don't need to handle any more window notifications.
		// Give control back to the installer.
		SetWindowLongPtr(g_hChildWnd, GWLP_WNDPROC, (LONG_PTR)WndProcOld);
		SendMessage(g_hChildWnd, g_uMsgCreate, FALSE, (LPARAM)g_hChildWnd);
		if (DlgProcOld)
			SetWindowLongPtr(g_hChildWnd, DWLP_DLGPROC, (LONG_PTR)DlgProcOld);

		// If any previous threads are running, close them.
		if (g_hGradualProgressThread)
		{
			g_bGradualProgressDone = TRUE;
			CloseHandle(g_hGradualProgressThread);
		}
	}
}

// Function: Converts char to int
int str2int(char *p)
{
	int n = 0, f = 0;

	for (;; p++) {
		switch (*p) {
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			p++;
		}
		break;
	}
	while (*p >= '0' && *p <= '9')
		n = n * 10 + *p++ - '0';
	return(f ? -n : n);
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

