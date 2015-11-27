#include <windows.h>
#include <tchar.h>
#include <winioctl.h>
#include <intrin.h>
#include "resources.h"
#include "misc.h"


#define SRVICE_PERMISS (SC_MANAGER_CREATE_SERVICE | SERVICE_START | SERVICE_STOP | DELETE | SERVICE_CHANGE_CONFIG)

/* WinRing0 */
#define IOCTL_READ_IO_PORT_BYTE             CTL_CODE(40000, 0x833, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_WRITE_IO_PORT_BYTE            CTL_CODE(40000, 0x836, METHOD_BUFFERED, FILE_WRITE_ACCESS)



const TCHAR* sDrvID    = _T("WinRing0_1_2_0");
const TCHAR* sDrvFile  = _T("WinRing0.sys");


TCHAR* sDrvName = NULL;
HANDLE hDriver = INVALID_HANDLE_VALUE;



static DWORD OpenWR0   (LPCTSTR DriverId);
static void  CloseWR0  (void);
static DWORD DrvInstall(LPCTSTR sDrvID, LPCTSTR sDrvPath, BOOL bAuto);
static DWORD DrvRemove (LPCTSTR sDrvID, BOOL bRemove);
static DWORD DrvAdjust(LPCTSTR sDrvId, BOOL bAuto);

DWORD  InstallWR0(HINSTANCE hinstDLL, UINT wBehavour) {
    CloseWR0();
    /* try to open driver */
    if (ERROR_SUCCESS != OpenWR0(sDrvID)) {
        /* try to install driver */
        TCHAR sPath[MAX_PATH];
        GetSystemDirectory(sPath, sizeof(sPath));
        _tcscat(sPath, _T("\\drivers\\"));
        _tcscat(sPath, sDrvFile);
        if (!isFileExists(sPath)) {
            /* copy file if not exists */
            DWORD dResultId = (isWin64()) ? IDR_WINRING64 : IDR_WINRING32;
            if (ERROR_SUCCESS != ResourceWrite(sPath, dResultId, hinstDLL)) return FALSE;  /* can't write file */
        }
        if (ERROR_SUCCESS != DrvInstall(sDrvID, sPath, (wBehavour == 1))) return FALSE;
        if (ERROR_SUCCESS != OpenWR0(sDrvID)) return FALSE;                                 /* can't open driver */
    }
    sDrvName = (isWin64()) ? _T("WinRing0x64") : _T("WinRing0x32");
    return TRUE;
}

DWORD RemoveWR0(HINSTANCE hinstDLL, UINT wBehavour) {
    DWORD dResult;
    CloseWR0();
    dResult = DrvAdjust(sDrvID, (wBehavour == 1));
    if (wBehavour == 0) {
        dResult = DrvRemove(sDrvID, 0);
    }
    return (ERROR_SUCCESS == dResult);
}

TCHAR* GetNameWR0(void) {
    return (sDrvName) ? sDrvName : _T("none");
}

static void CloseWR0(void) {
    if (hDriver != INVALID_HANDLE_VALUE) CloseHandle(hDriver);
    hDriver = INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------
// returns API error or 0 on success
static DWORD OpenWR0 (LPCTSTR DriverId) {
    TCHAR sDriver[MAX_PATH];
    _tcscpy(sDriver, _T("\\\\.\\"));
    _tcscat(sDriver, DriverId);
    hDriver = CreateFile(sDriver,
                         GENERIC_READ | GENERIC_WRITE, 0, NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,
                         NULL);
    Sleep(0);
    if (hDriver == INVALID_HANDLE_VALUE)
        return GetLastError();
    return ERROR_SUCCESS;
}

DWORD WriteWR0(WORD wPort, BYTE bValue) {
    // Write a Byte to an I/O Port via Driver
    DWORD dBytes;
    BYTE  bCount = 10;
    struct  {
        ULONG   uPort;
        UCHAR   bData;
    } sData;
    sData.uPort = wPort;
    sData.bData = bValue;
    do {
        if (DeviceIoControl(hDriver, IOCTL_WRITE_IO_PORT_BYTE, &sData, sizeof(sData), NULL, 0, &dBytes, NULL)) {
            return ERROR_SUCCESS;
        }
        Sleep(0);
    } while (bCount--);
    return GetLastError();
}

DWORD ReadWR0(WORD wPort, BYTE* bValue) {
    DWORD dBytes;
    WORD  wValue;
    BYTE  bCount = 10;
    do {
        if (DeviceIoControl(hDriver, IOCTL_READ_IO_PORT_BYTE, &wPort, sizeof(wPort), &wValue, sizeof(wValue), &dBytes, NULL)) {
            *bValue = (BYTE)wValue;
            return ERROR_SUCCESS;
        }
        Sleep(0);
    } while (bCount--);
    return GetLastError();
}

static DWORD DrvAdjust(LPCTSTR sDrvId, BOOL bAuto) {
    DWORD dResult = ERROR_SUCCESS;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM) {
        SC_HANDLE hService = OpenService(hSCM, sDrvID, SRVICE_PERMISS);
        if (hService) {
            ChangeServiceConfig(hService,
                    SERVICE_NO_CHANGE,
                    (bAuto) ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
                    SERVICE_NO_CHANGE,
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            CloseServiceHandle(hService);
        } else {
            dResult = GetLastError();
        }
        CloseServiceHandle(hSCM);
    } else {
        dResult = GetLastError();
    }
    Sleep(0);
    return dResult;
}



static DWORD DrvInstall(LPCTSTR sDrvID, LPCTSTR sDrvPath, BOOL bAuto) {
    DWORD dResult = ERROR_SUCCESS;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM) {
        SC_HANDLE hService = CreateService(hSCM, sDrvID, sDrvID,
                SRVICE_PERMISS, SERVICE_KERNEL_DRIVER,
                (bAuto) ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
                SERVICE_ERROR_NORMAL,
                sDrvPath, NULL, NULL, NULL, NULL, NULL);
        if (!hService) {
            dResult = GetLastError();
            if (ERROR_SERVICE_EXISTS == dResult) {
                dResult = ERROR_SUCCESS;
                hService = OpenService(hSCM, sDrvID, SRVICE_PERMISS);
                if (!hService) {
                    dResult = GetLastError();
                } else {
                    if (!StartService(hService, 0, NULL)) {
                        dResult = GetLastError();
                        if (ERROR_SERVICE_ALREADY_RUNNING == dResult) dResult = ERROR_SUCCESS;
                    }
                }
            }
        }
        if (hService) CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);
    } else {
        dResult = GetLastError();
    }
    Sleep(0);
    return dResult;
}

static DWORD DrvRemove(LPCTSTR sDrvID, BOOL bRemove) {
    DWORD dResult = ERROR_SUCCESS;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM) {
        SC_HANDLE hService = OpenService(hSCM, sDrvID, SRVICE_PERMISS);
        if (hService) {
            SERVICE_STATUS sstatus;
            if (!ControlService(hService, SERVICE_CONTROL_STOP, &sstatus)) {
                dResult = GetLastError();
                if (dResult == ERROR_SERVICE_NOT_ACTIVE || dResult == ERROR_SERVICE_MARKED_FOR_DELETE)
                    dResult = ERROR_SUCCESS;
            } else {
                while (sstatus.dwCurrentState != SERVICE_STOPPED) {
                    Sleep(100);
                    if (!QueryServiceStatus(hService, &sstatus)) {
                        dResult = GetLastError();
                        break;
                    }
                }
            }
            CloseServiceHandle(hService);
        } else {
            dResult = GetLastError();
        }
    } else {
        dResult = GetLastError();
    }
    Sleep(0);
    return dResult;
}
