#include <windows.h>

#include <cstdio>
#include <iostream>
#include <string>

constexpr char REG_PATH[] =
    "Software\\TEST-WindowsNativeAnalysisLab";

constexpr char DEFAULT_INSTALL_ID[] =
    "WNL-REG-001";

std::string currentTimestamp()
{
    SYSTEMTIME st{};
    GetLocalTime(&st);

    char buffer[64]{};

    sprintf(
        buffer,
        "%04u-%02u-%02u %02u:%02u:%02u.%03u",
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond,
        st.wMilliseconds
    );

    return buffer;
}

bool readStringValue(
    HKEY key,
    const char* name,
    std::string& value)
{
    char buffer[256]{};
    DWORD type = 0;
    DWORD size = sizeof(buffer);

    LONG result = RegQueryValueExA(
        key,
        name,
        nullptr,
        &type,
        reinterpret_cast<BYTE*>(buffer),
        &size
    );

    if (result != ERROR_SUCCESS || type != REG_SZ) {
        return false;
    }

    value = buffer;
    return true;
}

bool readDwordValue(
    HKEY key,
    const char* name,
    DWORD& value)
{
    DWORD type = 0;
    DWORD size = sizeof(value);

    LONG result = RegQueryValueExA(
        key,
        name,
        nullptr,
        &type,
        reinterpret_cast<BYTE*>(&value),
        &size
    );

    return result == ERROR_SUCCESS &&
           type == REG_DWORD;
}

void writeStringValue(
    HKEY key,
    const char* name,
    const std::string& value)
{
    RegSetValueExA(
        key,
        name,
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(value.c_str()),
        static_cast<DWORD>(value.size() + 1)
    );
}

void writeDwordValue(
    HKEY key,
    const char* name,
    DWORD value)
{
    RegSetValueExA(
        key,
        name,
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&value),
        sizeof(value)
    );
}

int main()
{
    std::cout << "REG-001-registry-read-write\n\n";

    HKEY key = nullptr;

    LONG result = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        REG_PATH,
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WRITE,
        nullptr,
        &key,
        nullptr
    );

    if (result != ERROR_SUCCESS) {
        std::cerr
            << "Failed to open registry key. Error="
            << result
            << "\n";

        return 1;
    }

    std::string installId;

    if (!readStringValue(key, "install_id", installId)) {
        installId = DEFAULT_INSTALL_ID;
        writeStringValue(key, "install_id", installId);
    }

    DWORD launchCount = 0;
    readDwordValue(key, "launch_count", launchCount);

    launchCount++;
    writeDwordValue(key, "launch_count", launchCount);

    std::string startTime = currentTimestamp();
    writeStringValue(key, "last_start_time", startTime);

    std::cout
        << "install_id=" << installId << "\n"
        << "launch_count=" << launchCount << "\n"
        << "last_start_time=" << startTime << "\n\n"
        << "Application is ready.\n";

    Sleep(2000);

    std::string closeTime = currentTimestamp();
    writeStringValue(key, "last_close_time", closeTime);

    std::cout
        << "Application is shutting down.\n"
        << "last_close_time=" << closeTime << "\n";

    RegCloseKey(key);

    return 0;
}
