#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <windows.h>
#include <oleacc.h>
#include <atlbase.h>

enum class ReturnWalker {
	FOUND,
	NO_FOUND,
	NONE,
	OK
};

class Clicker {
private:	
	Clicker();
	Clicker(const Clicker&);
	Clicker& operator =(const Clicker&) {};
	std::wstring getName(CComPtr<IAccessible> pAcc);
	std::wstring getClass(CComPtr<IAccessible> pAcc);
	void checkItem();
	void findWindows(std::wstring nameFindProg_, std::wstring nameFindClass_);
public:	
	~Clicker();
	static Clicker& getInstance()
	{
		static Clicker ret;
		return ret;
	}

	bool run();

	ReturnWalker WalkTreeWithAccessibleChildren(CComPtr<IAccessible> pAcc, std::wstring nameButton, bool flagRecursion);
};