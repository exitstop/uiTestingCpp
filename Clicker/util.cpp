#include "util.h"

PROCESS_INFORMATION launchProc(std::wstring procName, DWORD timeOut) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;


	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		(LPTSTR)procName.c_str(),        // Command line
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

