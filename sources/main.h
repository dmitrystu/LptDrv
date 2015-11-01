#ifndef __LPTDRV_H__
#define __LPTDRV_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#define _APIEXP __declspec(dllexport)
#define _APICALL __stdcall

UINT _APICALL _APIEXP LptDrvCheckVersion (LPCTSTR lParam1);
UINT _APICALL _APIEXP LptDrvClearError(void);
UINT _APICALL _APIEXP LptDrvClose(void);
UINT _APICALL _APIEXP LptDrvGetDLLVersion(void);
UINT _APICALL _APIEXP LptDrvGetError(void);
UINT _APICALL _APIEXP LptDrvGetErrorString(UINT lParam1);
UINT _APICALL _APIEXP LptDrvGetNumPorts(void);
UINT _APICALL _APIEXP LptDrvGetOpenedPort(void);
UINT _APICALL _APIEXP LptDrvGetStatus(void);
UINT _APICALL _APIEXP LptDrvGetVersion(void);
UINT _APICALL _APIEXP LptDrvGetVersionString(LPCTSTR lParam1, DWORD lParam2);
UINT _APICALL _APIEXP LptDrvOpen(UINT LptNum, DWORD lParam2);
UINT _APICALL _APIEXP LptDrvReset(void);
UINT _APICALL _APIEXP LptDrvSetControl (DWORD lParam1);
UINT _APICALL _APIEXP LptDrvSetData (DWORD lParam1);
UINT _APICALL _APIEXP LptDrvSetDelay (DWORD lParam1);
UINT _APICALL _APIEXP LptDrvSetTimeout (DWORD lParam1);
UINT _APICALL _APIEXP LptI2cCheckAdapter(void);
UINT _APICALL _APIEXP LptI2cEnableAckCheck (DWORD lParam1);
UINT _APICALL _APIEXP LptI2cOpen (DWORD lParam1, DWORD lParam2);
UINT _APICALL _APIEXP LptI2cRead (DWORD lParam1, DWORD lParam2, DWORD lParam3, DWORD lParam4, DWORD lParam5);
UINT _APICALL _APIEXP LptI2cReadByte (DWORD lParam1, DWORD lParam2);
UINT _APICALL _APIEXP LptI2cReadSubByte (DWORD lParam1, DWORD lParam2, DWORD lParam3);
UINT _APICALL _APIEXP LptI2cReadSubWord (DWORD lParam1, DWORD lParam2, DWORD lParam3);
UINT _APICALL _APIEXP LptI2cReadWord (DWORD lParam1, DWORD lParam2);
UINT _APICALL _APIEXP LptI2cWrite (DWORD lParam1, DWORD lParam2, DWORD lParam3, DWORD lParam4, DWORD lParam5);
UINT _APICALL _APIEXP LptI2cWriteByte (DWORD lParam1, DWORD lParam2);
UINT _APICALL _APIEXP LptI2cWriteSubByte (DWORD lParam1, DWORD lParam2, DWORD lParam3);
UINT _APICALL _APIEXP LptI2cWriteSubWord (DWORD lParam1, DWORD lParam2, DWORD lParam3);
UINT _APICALL _APIEXP LptI2cWriteWord (DWORD lParam1, DWORD lParam2);
UINT _APICALL _APIEXP LptSpiSetInvOut (DWORD lParam1);
UINT _APICALL _APIEXP LptSpiWrite (DWORD lParam1, DWORD lParam2, DWORD lParam3);
UINT _APICALL _APIEXP LptSpiWriteWord (DWORD lParam1);

#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // __LPTDRV_H__
