// dll_injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

BOOL InjectDll(DWORD pId, char* dllToInject)
{
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
    if (processHandle)
    {
        LPVOID loadLibAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        if (loadLibAddress == NULL) {
            CloseHandle(processHandle);
            return FALSE;
        }

        LPVOID baseAddress = VirtualAllocEx(processHandle, NULL, strlen(dllToInject), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (baseAddress == NULL) {
            CloseHandle(processHandle);
            return FALSE;
        }

        if (!WriteProcessMemory(processHandle, baseAddress, dllToInject, strlen(dllToInject), NULL)) {
            VirtualFreeEx(processHandle, baseAddress, strlen(dllToInject), MEM_RELEASE);
            CloseHandle(processHandle);
            return FALSE;
        }

        HANDLE remoteThreadHandle = CreateRemoteThread(processHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibAddress, baseAddress, 0, NULL);
        if (remoteThreadHandle == NULL) {
            VirtualFreeEx(processHandle, baseAddress, strlen(dllToInject), MEM_RELEASE);
            CloseHandle(processHandle);
            return FALSE;
        }

//        WaitForSingleObject(remoteThreadHandle, INFINITE);

        VirtualFreeEx(processHandle, baseAddress, strlen(dllToInject), MEM_RELEASE);
        CloseHandle(remoteThreadHandle);
        CloseHandle(processHandle);

        return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    DWORD pId = atoi(argv[1]);
    DWORD mode = atoi(argv[2]);   // 0 : 64bit  ,  1 : 32bit
        
    char x64dllpath[] = "E:\\workspace_VS\\api_hook_dll_x64_BBLT\\x64\\Debug\\api_hook_dll_x64.dll";
    char x86dllpath[] = "E:\\workspace_VS\\api_hook_dll_x86\\Debug\\api_hook_dll_x86.dll";

    InjectDll(pId, mode ? x64dllpath: x86dllpath);

    return 1;
}
