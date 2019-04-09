#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <windows.h>
#include <oleacc.h>
#include <atlbase.h>


PROCESS_INFORMATION launchProc(std::wstring procName, DWORD timeOut);