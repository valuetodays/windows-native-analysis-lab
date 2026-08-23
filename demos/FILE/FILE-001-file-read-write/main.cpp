#include <windows.h>
#include <iostream>
#include <string>

constexpr char CONFIG_FILE[] = "config.ini";

std::string getValue(const std::string& text, const std::string& key)
{
    std::string target = key + "=";

    size_t begin = text.find(target);
    if (begin == std::string::npos) {
        return "";
    }

    begin += target.size();

    size_t end = text.find_first_of("\r\n", begin);
    return text.substr(begin, end - begin);
}

bool readConfig(std::string& content)
{
    HANDLE file = CreateFileA(
        CONFIG_FILE,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    char buffer[4096]{};
    DWORD bytesRead = 0;

    BOOL ok = ReadFile(
        file,
        buffer,
        sizeof(buffer) - 1,
        &bytesRead,
        nullptr);

    CloseHandle(file);

    if (!ok) {
        return false;
    }

    content.assign(buffer, bytesRead);
    return true;
}

std::string timestamp()
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
        st.wMilliseconds);

    return buffer;
}

std::string today()
{
    SYSTEMTIME st{};
    GetLocalTime(&st);

    char buffer[32]{};

    sprintf(
        buffer,
        "%04u-%02u-%02u",
        st.wYear,
        st.wMonth,
        st.wDay);

    return buffer;
}

void writeLog(
    const std::string& logFile,
    const char* category,
    const char* message)
{
    std::string line =
        timestamp() +
        " [INFO] [" +
        category +
        "] " +
        message +
        "\r\n";

    std::cout << line;

    HANDLE file = CreateFileA(
        logFile.c_str(),
        FILE_APPEND_DATA,
        FILE_SHARE_READ,
        nullptr,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD written = 0;

    WriteFile(
        file,
        line.data(),
        static_cast<DWORD>(line.size()),
        &written,
        nullptr);

    CloseHandle(file);
}

int main()
{
    std::string configText;

    if (!readConfig(configText)) {
        std::cerr << "Failed to read config.ini\n";
        return 1;
    }

    std::string appId = getValue(configText, "app_id");
    std::string debug = getValue(configText, "debug");
    std::string updateEnabled = getValue(configText, "enabled");
    std::string updateUrl = getValue(configText, "update_url");
    std::string logDir = getValue(configText, "log_dir");

    std::cout
        << "app_id=" << appId << "\n"
        << "debug=" << debug << "\n"
        << "update.enabled=" << updateEnabled << "\n"
        << "update_url=" << updateUrl << "\n"
        << "log_dir=" << logDir << "\n\n";

    CreateDirectoryA(logDir.c_str(), nullptr);

    std::string logFile =
        logDir + "\\" + today() + ".log";

    writeLog(logFile, "Startup", "Application starting");
    writeLog(logFile, "Config", "Configuration loaded");
    writeLog(logFile, "Startup", "Application is ready");

    Sleep(2000);

    writeLog(logFile, "Startup", "Application is shutting down");

    return 0;
}