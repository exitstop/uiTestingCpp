#include "myLib.h"
#include "util.h"

HWND hWndVictim = NULL;
std::wstring nameFindClass = L"WebViewHost";
std::wstring nameFindProg = L"Trend Micro Maximum Security";

Clicker::Clicker() {

}

Clicker::~Clicker() {

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
	// Как работать с IAccessible
	// https://habr.com/ru/company/infopulse/blog/253729/

	// Запускаем антивирус
	PROCESS_INFORMATION piUiWinMgr = launchProc(L"C:\\Program Files\\Trend Micro\\Titanium\\UIFramework\\uiWinMgr.exe", 1000);	
	Sleep(3000);
	
	::CoInitialize(NULL);
	
	::findWindows(L"", L"WebViewHost");
	findWindows(L"Trend Micro Maximum Security", L"WebViewHost");

	if (::hWndVictim == NULL) {
		return false;
	}

	// Без этого нельзя считать данные из окна
	CComPtr<IAccessible> pAccMain0;
	HRESULT hr = ::AccessibleObjectFromWindow(::hWndVictim, 1, IID_IAccessible, (void**)(&pAccMain0)); // 1 - захардкоженный идентификатор ловушки

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

		// Без этого нельзя считать данные из окна
		CComPtr<IAccessible> pAccMain0;
		HRESULT hr = ::AccessibleObjectFromWindow(::hWndVictim, 1, IID_IAccessible, (void**)(&pAccMain0)); // 1 - захардкоженный идентификатор ловушки

		CComPtr<IAccessible> pAccMain;
		::AccessibleObjectFromWindow(::hWndVictim, OBJID_CLIENT, IID_IAccessible, (void**)(&pAccMain));

		// Ждем пока база данных не обновится
		while (WalkTreeWithAccessibleChildren(pAccMain, L"Components are up-to-date.", true) != ReturnWalker::FOUND) {
			Sleep(2000);
		}
		// Закроем окно About Your Software
		WalkTreeWithAccessibleChildren(pAccMain, L"Close", true);
	}


	// Закроем антивирус
	::TerminateProcess(piUiWinMgr.hProcess, 0);
	::CloseHandle(piUiWinMgr.hProcess);
	::CloseHandle(piUiWinMgr.hThread);	

	

	// Сканируем файл на вирусы
	PROCESS_INFORMATION piPowerShell = launchProc(L"powershell -File C:\\Users\\user\\source\\repos\\Clicker\\Clicker\\contextMenu.ps1", 1000);
	::TerminateProcess(piPowerShell.hProcess, 0);
	::CloseHandle(piPowerShell.hProcess);
	::CloseHandle(piPowerShell.hThread);

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
	HRESULT ret{ 0 };
	try {
		ret = pAcc->get_accName(CComVariant((int)CHILDID_SELF), &bstrName);
	}catch (...) {
		return L"";
	}
	if (!pAcc || ret || (bstrName && !bstrName.m_str)) {
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
			// open menu
			VARIANT varId;
			varId.vt = VT_I4;
			varId.lVal = CHILDID_SELF;
			pAccChild->accDoDefaultAction(varId);
		}

		if ( wcscmp(nameButton.c_str(), L"lbl_titlebar_account") == 0) {
			// openAbout
			if (flagRecursion && name.find(nameButton) != -1) {
				ReturnWalker ret = openAbout(pAccChild, nameButton, flagRecursion);
				if (ret  != ReturnWalker::NO_FOUND) {
					return ret;
				}
			}
		}
		else if (wcscmp(nameButton.c_str(), L"Close") == 0) {
			// closeAbout
			if (flagRecursion && name.find(nameButton) != -1) {
				closeAbout(pAccChild, nameButton, flagRecursion);
			}
		}
		else if (wcscmp(nameButton.c_str(), L"Components are up-to-date.") == 0) {
			// waitAbout
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


ReturnWalker Clicker::openAbout(CComQIPtr<IAccessible>& pAccChild, std::wstring nameButton, bool flagRecursion)
{
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
	long returnCount = 0;
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
		ReturnWalker::OK;
	}
	else {
		ReturnWalker::NO_FOUND;
	}

}

ReturnWalker Clicker::closeAbout(CComQIPtr<IAccessible>& pAccChild, std::wstring nameButton, bool flagRecursion)
{
	VARIANT varId;
	varId.vt = VT_I4;
	varId.lVal = CHILDID_SELF;
	pAccChild->accDoDefaultAction(varId);
	return ReturnWalker::OK;
}