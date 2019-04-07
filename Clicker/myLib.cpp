#include "myLib.h"

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
	::CoInitialize(NULL);

	findWindows(L"Trend Micro Maximum Security", L"WebViewHost");

	if (::hWndVictim == NULL) {
		return false;
	}

	CComPtr<IAccessible> pAccMain;
	::AccessibleObjectFromWindow(::hWndVictim, OBJID_CLIENT, IID_IAccessible, (void**)(&pAccMain));

	// Ищем нужные кнопки и их детей
	WalkTreeWithAccessibleChildren(pAccMain);

	{
		// Ищем окно About Your Software
		::findWindows(L"", L"WebViewHost");
		findWindows(L"About Your Software", L"WebViewHost");

		if (::hWndVictim == NULL) {
			return false;
		}

		CComPtr<IAccessible> pAccMain;
		::AccessibleObjectFromWindow(::hWndVictim, OBJID_CLIENT, IID_IAccessible, (void**)(&pAccMain));

		// Ищем нужные кнопки и их детей
		WalkTreeWithAccessibleChildren(pAccMain);
	}

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



HRESULT Clicker::WalkTreeWithAccessibleChildren(CComPtr<IAccessible> pAcc, bool flagRecursion)
{
	long childCount = 0;
	long returnCount = 0;

	HRESULT hr = pAcc->get_accChildCount(&childCount);
	std::cerr << "childCount = " << childCount << std::endl;
	if (childCount == 0) {
		return S_OK;
	}

	std::vector<CComVariant> pArray(childCount);
	hr = ::AccessibleChildren(pAcc, 0L, childCount, pArray.data(), &returnCount);
	if (FAILED(hr)) {
		return hr;
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

		if (flagRecursion && name.find(L"lbl_titlebar_account") != -1) {
			VARIANT varId;
			varId.vt = VT_I4;
			varId.lVal = CHILDID_SELF;
			pAccChild->accDoDefaultAction(varId);

			IDispatch* ppdispParent{ 0 };
			pAccChild->get_accParent(&ppdispParent);

			long childCountLocal = 0;
			//IAccessible* pAccChildLocal = (IAccessible*)ppdispParent;
			CComQIPtr<IAccessible> pAccChildLocal(static_cast<IAccessible*>(ppdispParent));


			HRESULT hr = pAccChildLocal->get_accChildCount(&childCountLocal);

			std::cout << "childCountLocal = " << childCountLocal << std::endl;

			if (childCountLocal == 0) {
				return S_OK;
			}

			std::vector<CComVariant> pArrayLocal(childCountLocal);
			hr = ::AccessibleChildren(pAccChildLocal, 0L, childCountLocal, pArrayLocal.data(), &returnCount);
			if (FAILED(hr)) {
				return hr;
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
				if (WalkTreeWithAccessibleChildren(pAccChildLocal, flagRecursion) == S_FALSE) {
					//break;
					return S_OK;
				}
			}

			std::cout << "------------------Click();" << std::endl;

			if (!flagRecursion) {
				return S_OK;
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
		if (WalkTreeWithAccessibleChildren(pAccChild) == S_FALSE) {
			return S_FALSE;
		}
	}
	return S_OK;
}