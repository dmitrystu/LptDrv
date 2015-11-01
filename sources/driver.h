#ifndef __DRIVER_H__
#define __DRIVER_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus


DWORD  InstallWR0(HINSTANCE hinstDLL, UINT wBehavour);
DWORD  RemoveWR0 (HINSTANCE hinstDLL, UINT wBehavour);
DWORD  ReadWR0 (WORD wPort, BYTE* bValue);
DWORD  WriteWR0(WORD wPort, BYTE  bValue);
TCHAR* GetNameWR0(void);


#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // __DRIVER_H__
