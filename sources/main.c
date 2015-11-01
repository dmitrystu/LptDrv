#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "main.h"
#include "driver.h"
#include "misc.h"
#include "defines.h"

//#define USE_LOG



    static void WriteLog(char* func, UINT p1, UINT p2, UINT r1) {
        FILE* fLog;
        fLog = fopen("log.txt","a");
        if (fLog) fprintf(fLog, "Called %s, Params %u, %u, Return %u\n", func, p1, p2, r1);
        fclose(fLog);
    }

const char* sDefault = "#WinRing0 driver behaviour"
                       "\n# 0 - shutdown driver on exit"
                       "\n# 1 - keep driver always running"
                       "\n# 2 - keep driver running until reboot"
                       "\nkeep_driver_active 2"
                       "\n\n#adds extra us delay to each port operation"
                       "\nextra_delay 0"
                       "\n\n#adds port address to config"
                       "\nport 0x378"
                       "\nport 0x278";


UINT PortList[0x10];
UINT PortCount = 0;
UINT PortCurrent = 0;
UINT ExtraDelay = 0;
UINT DrvBehavour = 2;
DWORD lasterror = ERROR_SUCCESS;
DWORD oldpriority = 0;

#define _OK TRUE
#define _FAIL FALSE




UINT _APICALL LptDrvCheckVersion (LPCTSTR lParam1) {
    lParam1 = GetNameWR0();
    return TRUE;
}

UINT _APICALL LptDrvClearError(void) {
    lasterror = ERROR_SUCCESS;
    return TRUE;
}

UINT _APICALL LptDrvClose(void) {
    PortCurrent = 0;
    if (0 != oldpriority) {
        SetPriorityClass(GetCurrentProcess(), oldpriority);
    }
    return TRUE;
}

UINT _APICALL LptDrvGetDLLVersion(void) {
    DWORD ver = DRV_MAJOR << 24 | DRV_MINOR << 16 | DRV_RELEASE << 8 | DRV_VER;
    return ver;
}


UINT _APICALL LptDrvGetError(void) {
    return lasterror;
}
UINT _APICALL LptDrvGetErrorString(UINT lParam1) {
    return FALSE;
}

UINT _APICALL LptDrvGetNumPorts(void) {
    return PortCount;
}

UINT _APICALL LptDrvGetOpenedPort(void) {
    return PortCurrent;
}

UINT _APICALL LptDrvGetStatus(void) {
    BYTE res = 0;
    if (PortCurrent) {
        lasterror = ReadWR0(PortList[PortCurrent-1] + 1, &res);
        if (ExtraDelay) WaitUs(ExtraDelay);
    }
    return res;
}

UINT _APICALL LptDrvGetVersion(void) {
    DWORD ver = DRV_MAJOR << 24 | DRV_MINOR << 16 | DRV_RELEASE << 8 | DRV_VER;
    return ver;
}

UINT _APICALL LptDrvGetVersionString(LPCTSTR lParam1, DWORD lParam2) {
    lParam1 = GetNameWR0();
    return TRUE;
}

UINT _APICALL LptDrvOpen(UINT LptNum, DWORD lParam2) {
    if (LptNum >= PortCount) return _FAIL;
    PortCurrent = LptNum;
    /* Try to set process priority */
    oldpriority = GetPriorityClass(GetCurrentProcess());
    if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
        oldpriority = 0;
    }
    return TRUE;
}

UINT _APICALL LptDrvReset(void) {
    if (PortCurrent) {
        LptDrvClose();
        lasterror = ERROR_SUCCESS;
    }
    return TRUE;
}

UINT _APICALL LptDrvSetControl (DWORD lParam1) {

    if (PortCurrent) {
        lasterror = WriteWR0(PortList[PortCurrent-1] + 2,(BYTE)lParam1);
        if (ExtraDelay) WaitUs(ExtraDelay);
    } else {
        lasterror = ERROR_INVALID_OPERATION;
    }
    return (lasterror == ERROR_SUCCESS);
}

UINT _APICALL LptDrvSetData (DWORD lParam1) {

    if (PortCurrent) {
        lasterror = WriteWR0(PortList[PortCurrent-1],(BYTE)lParam1);
        if (ExtraDelay) WaitUs(ExtraDelay);
    } else {
        lasterror = ERROR_INVALID_OPERATION;
    }
    return (lasterror == ERROR_SUCCESS);
}

UINT _APICALL LptDrvSetDelay (DWORD lParam1) {
    return FALSE;
}
UINT _APICALL LptDrvSetTimeout (DWORD lParam1) {
    return FALSE;
}
UINT _APICALL LptI2cCheckAdapter(void) {
    return FALSE;
}
UINT _APICALL LptI2cEnableAckCheck (DWORD lParam1) {
    return FALSE;
}
UINT _APICALL LptI2cOpen (DWORD lParam1, DWORD lParam2) {
    return FALSE;
}
UINT _APICALL LptI2cRead (DWORD lParam1, DWORD lParam2, DWORD lParam3, DWORD lParam4, DWORD lParam5) {
    return FALSE;
}
UINT _APICALL LptI2cReadByte (DWORD lParam1, DWORD lParam2) {
    return FALSE;
}
UINT _APICALL LptI2cReadSubByte (DWORD lParam1, DWORD lParam2, DWORD lParam3) {
    return FALSE;
}
UINT _APICALL LptI2cReadSubWord (DWORD lParam1, DWORD lParam2, DWORD lParam3) {
    return FALSE;
}
UINT _APICALL LptI2cReadWord (DWORD lParam1, DWORD lParam2) {
    return FALSE;
}
UINT _APICALL LptI2cWrite (DWORD lParam1, DWORD lParam2, DWORD lParam3, DWORD lParam4, DWORD lParam5) {
    return FALSE;
}
UINT _APICALL LptI2cWriteByte (DWORD lParam1, DWORD lParam2) {
    return FALSE;
}
UINT _APICALL LptI2cWriteSubByte (DWORD lParam1, DWORD lParam2, DWORD lParam3) {
    return FALSE;
}
UINT _APICALL LptI2cWriteSubWord (DWORD lParam1, DWORD lParam2, DWORD lParam3) {
    return FALSE;
}
UINT _APICALL LptI2cWriteWord (DWORD lParam1, DWORD lParam2) {
    return FALSE;
}
UINT _APICALL LptSpiSetInvOut (DWORD lParam1) {
    return FALSE;
}
UINT _APICALL LptSpiWrite (DWORD lParam1, DWORD lParam2, DWORD lParam3) {
    return FALSE;
}
UINT _APICALL LptSpiWriteWord (DWORD lParam1) {
    return FALSE;
}


BOOL GetConfig (HMODULE hInst) {
    TCHAR  tPath[MAX_PATH];
    UINT  dTemp;
    FILE*  fConfig;
    dTemp = GetModuleFileName(hInst, tPath, MAX_PATH);
    if (!dTemp || dTemp < 5 || dTemp >= MAX_PATH) return FALSE;
    _tcscpy(&tPath[dTemp-3],_T("ini"));
    fConfig = _tfopen(tPath,_T("r"));
    if (fConfig) {
        while (fgets((char*)tPath, MAX_PATH, fConfig)) {
            if (*(char*)tPath == '#' || *(char*)tPath == ';') continue; /* Ignore comments */
            if (sscanf((char*)tPath,"extra_delay %d", &dTemp)) ExtraDelay = dTemp;
            if (sscanf((char*)tPath,"port %x", &dTemp)) {
                PortList[PortCount] = dTemp;
                PortCount++;
                if (PortCount > 0x10) break;
            }
            if (sscanf((char*)tPath,"keep_driver_active %d", &dTemp)) DrvBehavour = dTemp;
        }
    } else {
        fConfig = _tfopen(tPath, _T("w"));
        if (!fConfig) return FALSE;
        fprintf(fConfig, sDefault);
        PortCount = 2;
        PortList[0] = 0x378;
        PortList[1] = 0x278;
    }
    fclose(fConfig);
    return (PortCount > 0);
}






BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fwdReason, LPVOID lpvReserved) {
    BOOL res = TRUE;
    if (fwdReason == DLL_PROCESS_ATTACH) {
        PortCurrent = 0;
        res  = GetConfig(hinstDLL);
        res &= InstallWR0(hinstDLL, DrvBehavour);
    } else if (fwdReason == DLL_PROCESS_DETACH) {
        res = RemoveWR0(hinstDLL, DrvBehavour);
    }
    return res;
}

