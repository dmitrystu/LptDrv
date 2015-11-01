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



const TCHAR* sDrvID   = _T("WinRing0_1_2_0");
const TCHAR* sDrvFile = _T("WinRing0.sys");

typedef struct {
    ULONG PortNumber;
    UCHAR CharData;
} WRITE_IO_PORT_INPUT;


HANDLE hDriver = INVALID_HANDLE_VALUE;
TCHAR* sDrvName = _T("none");


static DWORD OpenWR0   (LPCTSTR DriverId);
static void  CloseWR0  (void);
static DWORD DrvInstall(LPCTSTR sDrvID, LPCTSTR sDrvPath, BOOL bAuto);
static DWORD DrvRemove (LPCTSTR sDrvID, BOOL bRemove);


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
    DWORD dResult = ERROR_SUCCESS;
    CloseWR0();
    if (wBehavour == 0) {
        dResult = DrvRemove(sDrvID, 0);
    }
    return (dResult == ERROR_SUCCESS);
}

TCHAR* GetNameWR0(void) {
    return sDrvName;
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
    hDriver = CreateFile(sDriver, GENERIC_READ | GENERIC_WRITE, 0, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    Sleep(0);
    if (hDriver == INVALID_HANDLE_VALUE)
        return GetLastError();
    return ERROR_SUCCESS;
}

DWORD WriteWR0(WORD wPort, BYTE bValue) {
    // Write a Byte to an I/O Port via Driver
    DWORD u32_RetLen;
    WRITE_IO_PORT_INPUT k_In;
    k_In.CharData   = bValue;
    k_In.PortNumber = wPort;
//	WaitUs(2);
    if (!DeviceIoControl(hDriver, IOCTL_WRITE_IO_PORT_BYTE, &k_In, sizeof(k_In),
            NULL, 0, &u32_RetLen, NULL))
        return GetLastError();
    return ERROR_SUCCESS;
}

DWORD ReadWR0(WORD wPort, BYTE* bValue) {
    DWORD dRetLen;
    WORD  wValue;
    if (!DeviceIoControl(hDriver, IOCTL_READ_IO_PORT_BYTE, &wPort, sizeof(wPort),
            &wValue, sizeof(wValue), &dRetLen, NULL))
        return GetLastError();

    *bValue = (BYTE)wValue;
    return ERROR_SUCCESS;
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
                    ChangeServiceConfig(hService,
                            SERVICE_NO_CHANGE,
                            (bAuto) ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
                            SERVICE_NO_CHANGE,
                            NULL, NULL, NULL, NULL, NULL, NULL, NULL);
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
    return dResult;
}
