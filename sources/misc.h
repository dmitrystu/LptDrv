#ifndef __MISC_H__
#define __MISC_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

BOOL isWin64(void);
BOOL isFileExists(LPTSTR sFile);
DWORD ResourceWrite(LPTSTR sFile, UINT id, HINSTANCE hInst);
void WaitUs(ULONG us);

#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // __MISC_H__
