# Advanced Process Injector (PoC)

A stealthier Proof-of-Concept for Process Injection in C++, developed for Red Team operations and Malware Analysis research.

## Overview
This tool demonstrates advanced techniques used by APT (Advanced Persistent Threat) groups to execute arbitrary code within the address space of a legitimate running process (`notepad.exe`), evading modern EDR (Endpoint Detection and Response) solutions.

## Key Techniques Used
*   **ASLR Bypass:** Dynamically resolves the `MessageBoxA` address at runtime using `GetProcAddress` instead of hardcoding it.
*   **Stealthy Memory Allocation:** Allocates memory with `PAGE_READWRITE`, writes the payload, and then changes protection to `PAGE_EXECUTE_READ` using `VirtualProtectEx`. This helps to evade EDRs that monitor for `PAGE_EXECUTE_READWRITE` allocations.
*   **Process Injection:** Uses `OpenProcess`, `VirtualAllocEx`, `WriteProcessMemory`, and `CreateRemoteThread` to execute the payload inside the target process.

## Requirements
*   Windows OS (x64)
*   Compiler: MSVC or MinGW
*   Must be run as **Administrator**
*   Target process: `notepad.exe` (must be running)

## Disclaimer
This tool is for educational and security research purposes only. The author is not responsible for any illegal or malicious use. Test strictly in an isolated Virtual Machine (VM).
