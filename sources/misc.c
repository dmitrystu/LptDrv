#include <windows.h>
#include <tchar.h>

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef BOOL (WINAPI *LPFN_DISABLEWOW64REDIR)(PBOOL);
typedef BOOL (WINAPI *LPFN_RESTOREWOW64REDIR)(BOOL);

BOOL bWOW64state;


BOOL isWin64(void) {
#ifdef _WIN64
    return TRUE;
#else // _WIN64
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.
    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(_T("kernel32")),"IsWow64Process");
    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
      //     ASSERT( FALSE );
        }
    }
    return bIsWow64;
#endif
}
void DisableWOW64() {
#ifndef _WIN64
    LPFN_DISABLEWOW64REDIR fnWOW64Disable;
    fnWOW64Disable = (LPFN_DISABLEWOW64REDIR)GetProcAddress(GetModuleHandle(_T("kernel32")),"Wow64DisableWow64FsRedirection");
    if (NULL != fnWOW64Disable) {
        fnWOW64Disable(&bWOW64state);
    }
#endif // _WIN64
}

void RestoreWOW64() {
#ifndef _WIN64
    LPFN_RESTOREWOW64REDIR fnWOW64Restore;
    fnWOW64Restore = (LPFN_RESTOREWOW64REDIR)GetProcAddress(GetModuleHandle(_T("kernel32")),"Wow64RevertWow64FsRedirection");
    if (NULL != fnWOW64Restore) {
        fnWOW64Restore(bWOW64state);
    }
#endif // _WIN64
}


BOOL isFileExists(LPTSTR sFile) {
    BOOL res;
    DisableWOW64();
    res = ((DWORD)-1 != GetFileAttributes(sFile));
    RestoreWOW64();
    return res;
}


DWORD ResourceWrite(LPTSTR sFile, UINT id, HINSTANCE hInst) {
    HRSRC   hRsrs = FindResource(hInst, MAKEINTRESOURCE(id), RT_RCDATA);
    HGLOBAL hDrvr = LoadResource(hInst, hRsrs);
    LPVOID  hData = LockResource(hDrvr);
    DWORD   sz    = SizeofResource(hInst, hRsrs);
    DWORD   bw = 0;
    DisableWOW64();
    HANDLE  hFile = CreateFile(sFile,
                               GENERIC_READ | GENERIC_WRITE,
                               0, NULL, CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                               NULL);
    if (INVALID_HANDLE_VALUE == hFile) return GetLastError();
    if (!WriteFile(hFile, hData, sz, &bw, NULL)) {
        bw = GetLastError();
        if (ERROR_IO_PENDING == bw) bw = ERROR_SUCCESS;
    } else {
        bw = ERROR_SUCCESS;
    }
    FlushFileBuffers(hFile);
    CloseHandle(hFile);
    RestoreWOW64();
    return bw;
}

void WaitUs(ULONG us) {
    LARGE_INTEGER _tin, _tout;
    QueryPerformanceCounter(&_tin);
    QueryPerformanceFrequency(&_tout);
    _tout.QuadPart *= us;
    _tout.QuadPart /= 1000000;
    _tout.QuadPart += _tin.QuadPart;
    do {
        QueryPerformanceCounter(&_tin);
    } while (_tin.QuadPart <= _tout.QuadPart);
};
