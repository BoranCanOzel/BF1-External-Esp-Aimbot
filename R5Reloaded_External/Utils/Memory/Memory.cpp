#include "Memory.h"

bool Memory::Init()
{
    TargetHwnd = FindWindowW(NULL, L"Battlefield™ 1");

    if (!TargetHwnd)
    {
        printf("[-] Waiting BF1...\n");

        while (!TargetHwnd)
        {
            TargetHwnd = FindWindowW(NULL, L"Battlefield™ 1");

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }

    GetWindowThreadProcessId(TargetHwnd, &PID);
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (!pHandle)
    {
        printf("[-] Can't get pHandle!\n");

        return false;
    }

    return true;
}

HWND Memory::FindTarget()
{
    return FindWindowW(NULL, L"Battlefield™ 1");
}

Memory m;