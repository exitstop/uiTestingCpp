#include "myLib.h"

HWND hWndVictim = NULL;
std::wstring nameFindClass = L"WebViewHost";
std::wstring nameFindProg = L"Trend Micro Maximum Security";

Clicker::Clicker() {

}

Clicker::~Clicker() {

}

PROCESS_INFORMATION launchProc(std::wstring procName, DWORD timeOut) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;


	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		(LPWSTR)procName.c_str(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return pi;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, timeOut);

	return pi;
}

void findWindows(std::wstring nameFindProg_, std::wstring nameFindClass_)
{
	::hWndVictim = NULL;
	::nameFindClass = nameFindClass_;
	::nameFindProg = nameFindProg_;
	// Калбекэ для поиска нашего антивируса
	auto lambda = [](HWND hwnd, LPARAM lParam) {
		wchar_t className[200];
		if (GetClassNameW(hwnd, (LPWSTR)className, 200) == 0 || wcscmp(className, static_cast<const wchar_t*>(::nameFindClass.data())) != 0) {
			return TRUE;
		}

		wchar_t title[1000];
		if (GetWindowTextW(hwnd, (LPWSTR)title, 1000) == 0 || wcslen(title) == 0) {
			return TRUE;
		}

		std::wcout << "hwnd = " << hwnd << " title = " << std::wstring(title) << std::endl;

		//::hWndVictim = hwnd;
		return TRUE;
	};
	// Запускаем калбэк
	::EnumWindows(lambda, 0);
}


bool Clicker::run()
{
	PROCESS_INFORMATION piUiWinMgr = launchProc(L"C:\\Program Files\\Trend Micro\\Titanium\\UIFramework\\uiWinMgr.exe", 1000);
	// Эта программа как то влияет на определение кнопок в Антивирусе
	PROCESS_INFORMATION piAccEvent = launchProc(L"AccEvent.exe", 1000);
	
	::CoInitialize(NULL);

	::findWindows(L"", L"WebViewHost");
	findWindows(L"Trend Micro Maximum Security", L"WebViewHost");

	if (::hWndVictim == NULL) {
		return false;
	}

	CComPtr<IAccessible> pAccMain;
	::AccessibleObjectFromWindow(::hWndVictim, OBJID_CLIENT, IID_IAccessible, (void**)(&pAccMain));

	// Откроем окно About Your Software
	WalkTreeWithAccessibleChildren(pAccMain, L"lbl_titlebar_account", true);

	{
		// Ищем окно About Your Software
		::findWindows(L"", L"WebViewHost");
		findWindows(L"About Your Software", L"WebViewHost");

		if (::hWndVictim == NULL) {
			return false;
		}		

		CComPtr<IAccessible> pAccMain;
		::AccessibleObjectFromWindow(::hWndVictim, OBJID_CLIENT, IID_IAccessible, (void**)(&pAccMain));

		// Ждем пока база данных не обновится
		while (WalkTreeWithAccessibleChildren(pAccMain, L"Components are up-to-date.", true) != ReturnWalker::FOUND) {
			Sleep(2000);
		}
		// Закроем окно About Your Software
		WalkTreeWithAccessibleChildren(pAccMain, L"Close", true);
	}	
	
	::TerminateProcess(piAccEvent.hProcess, 0);
	::TerminateProcess(piUiWinMgr.hProcess, 0);

	::CloseHandle(piAccEvent.hProcess);
	::CloseHandle(piAccEvent.hThread);
	::CloseHandle(piUiWinMgr.hProcess);
	::CloseHandle(piUiWinMgr.hThread);

	return true;
}



void Clicker::findWindows(std::wstring nameFindProg_, std::wstring nameFindClass_)
{
	::hWndVictim = NULL;
	::nameFindClass = nameFindClass_;
	::nameFindProg = nameFindProg_;
	// Калбекэ для поиска нашего антивируса
	auto lambda = [](HWND hwnd, LPARAM lParam) {
		wchar_t className[200];
		if (GetClassNameW(hwnd, (LPWSTR)className, 200) == 0 || wcscmp(className, ::nameFindClass.data()) != 0) {
			return TRUE;
		}

		wchar_t title[1000];
		if (GetWindowTextW(hwnd, (LPWSTR)title, 1000) == 0 || wcscmp(title, ::nameFindProg.data()) != 0 || wcslen(title) == 0) {
			return TRUE;
		}

		std::wcout << "hwnd = " << hwnd << " title = " << std::wstring(title) << std::endl;

		::hWndVictim = hwnd;
		return FALSE;
	};
	// Запускаем калбэк
	::EnumWindows(lambda, 0);
}

std::wstring Clicker::getClass(CComPtr<IAccessible> pAcc)
{	
	wchar_t lpszRoleString[300];
	VARIANT varResult;
	HRESULT hr = pAcc->get_accRole(CComVariant((int)CHILDID_SELF), &varResult);

	DWORD roleId;

	if ((hr == S_OK) && (varResult.vt == VT_I4)) {
		roleId = varResult.lVal;
		UINT   roleLength;
		roleLength = GetRoleText(roleId, NULL, 0);

		if (lpszRoleString != NULL) {
			GetRoleText(roleId, (LPTSTR)lpszRoleString, roleLength + 1);
		} else {
			return L"";
		}
	} else {
		return L"";
	}

	return std::wstring(lpszRoleString);
}

std::wstring Clicker::getName(CComPtr<IAccessible> pAcc)
{
	CComBSTR bstrName;

	if (!pAcc
		|| FAILED(pAcc->get_accName(CComVariant((int)CHILDID_SELF), &bstrName))
		|| !bstrName.m_str) {
		return L"";
	}

	return bstrName.m_str;
}



ReturnWalker Clicker::WalkTreeWithAccessibleChildren(CComPtr<IAccessible> pAcc, std::wstring nameButton , bool flagRecursion)
{
	long childCount = 0;
	long returnCount = 0;

	HRESULT hr = pAcc->get_accChildCount(&childCount);
	std::cerr << "childCount = " << childCount << std::endl;
	if (childCount == 0) {
		return ReturnWalker::OK;
	}

	std::vector<CComVariant> pArray(childCount);
	hr = ::AccessibleChildren(pAcc, 0L, childCount, pArray.data(), &returnCount);
	if (FAILED(hr)) {
		return ReturnWalker::NONE;
	}

	for (int x = 0; x < returnCount; x++)
	{
		CComVariant vtChild = pArray[x];
		if (vtChild.vt != VT_DISPATCH) {
			continue;
		}

		CComQIPtr<IAccessible> pAccChild(static_cast<IAccessible*>(vtChild.pdispVal));
		if (!pAccChild) {
			continue;
		}

		std::wstring name = getName(pAccChild).data();
		std::wstring className = getClass(pAccChild).data();
		std::wcout << " class = " << className << " name = " << name << std::endl;

		if (name.find(L"About the Software") != -1) {
			VARIANT varId;
			varId.vt = VT_I4;
			varId.lVal = CHILDID_SELF;
			pAccChild->accDoDefaultAction(varId);
		}

		if ( wcscmp(nameButton.c_str(), L"lbl_titlebar_account") == 0) {

			if (flagRecursion && name.find(nameButton) != -1) {
				VARIANT varId;
				varId.vt = VT_I4;
				varId.lVal = CHILDID_SELF;
				pAccChild->accDoDefaultAction(varId);

				IDispatch* ppdispParent{ 0 };
				pAccChild->get_accParent(&ppdispParent);

				long childCountLocal = 0;

				CComQIPtr<IAccessible> pAccChildLocal(static_cast<IAccessible*>(ppdispParent));


				HRESULT hr = pAccChildLocal->get_accChildCount(&childCountLocal);

				std::cout << "childCountLocal = " << childCountLocal << std::endl;

				if (childCountLocal == 0) {
					return ReturnWalker::OK;
				}

				std::vector<CComVariant> pArrayLocal(childCountLocal);
				hr = ::AccessibleChildren(pAccChildLocal, 0L, childCountLocal, pArrayLocal.data(), &returnCount);
				if (FAILED(hr)) {
					return ReturnWalker::NONE;
				}

				for (int x = 0; x < childCountLocal; x++)
				{
					CComVariant vtChild = pArrayLocal[x];
					if (vtChild.vt != VT_DISPATCH) {
						continue;
					}

					CComQIPtr<IAccessible> pAccChildLocal(static_cast<IAccessible*>(vtChild.pdispVal));
					if (!pAccChildLocal) {
						continue;
					}

					std::wstring name = getName(pAccChildLocal).data();
					std::wcout << "-------------name = " << name << std::endl;
					flagRecursion = false;
					ReturnWalker ret = WalkTreeWithAccessibleChildren(pAccChildLocal, nameButton, flagRecursion);
					if (ret != ReturnWalker::OK) {
						return ReturnWalker::OK;
					}
				}

				std::cout << "------------------Click();" << std::endl;

				if (!flagRecursion) {
					return ReturnWalker::OK;
				}
			}
		}
		else if (wcscmp(nameButton.c_str(), L"Close") == 0) {
			if (flagRecursion && name.find(nameButton) != -1) {
				VARIANT varId;
				varId.vt = VT_I4;
				varId.lVal = CHILDID_SELF;
				pAccChild->accDoDefaultAction(varId);
			}
		}
		else if (wcscmp(nameButton.c_str(), L"Components are up-to-date.") == 0) {
			if (flagRecursion && name.find(L"Checking for program updates...") != -1) {
				return ReturnWalker::NO_FOUND;
			}
			else if(flagRecursion && name.find(nameButton) != -1) {
				return ReturnWalker::FOUND;
			}
		}

		/*
		if (name.find(L"Адресная строка и строка поиска") != -1)
		{
			CComBSTR bstrValue;
			if (SUCCEEDED(pAccChild->get_accValue(CComVariant((int)CHILDID_SELF), &bstrValue)) && bstrValue.m_str) {
				std::wcout << std::wstring(bstrValue.m_str).c_str();
			}

			return S_FALSE;
		}
		*/

		// Рекурсивно ищем дальше
		ReturnWalker ret = WalkTreeWithAccessibleChildren(pAccChild, nameButton, flagRecursion);
		if (ret != ReturnWalker::OK) {
			return ret;
		}
	}
	return ReturnWalker::OK;
}


