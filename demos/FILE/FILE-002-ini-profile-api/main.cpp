#include <windows.h>

#include <iostream>
#include <string>

constexpr char CONFIG_FILE[] = ".\\config.ini";

std::string getIniValue(
    const char* section,
    const char* key,
    const char* defaultValue)
{
    char buffer[512]{};

    GetPrivateProfileStringA(
        section,
        key,
        defaultValue,
        buffer,
        sizeof(buffer),
        CONFIG_FILE
    );

    return buffer;
}

void writeIniValue(
    const char* section,
    const char* key,
    const std::string& value)
{
    WritePrivateProfileStringA(
        section,
        key,
        value.c_str(),
        CONFIG_FILE
    );
}

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

void createDefaultConfig()
{
    writeIniValue(
        "application",
        "app_id",
        "windows-native-analysis-lab"
    );

    writeIniValue(
        "application",
        "debug",
        "true"
    );

    writeIniValue(
        "update",
        "enabled",
        "true"
    );

    writeIniValue(
        "update",
        "update_url",
        "http://127.0.0.1:8080/version"
    );

    writeIniValue(
        "log",
        "log_dir",
        "logs"
    );
}

bool configExists()
{
    DWORD attributes = GetFileAttributesA(CONFIG_FILE);

    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

int main()
{
    std::cout << "FILE-002-ini-profile-api\n\n";

    if (!configExists()) {
        std::cout << "config.ini not found, creating default configuration.\n";

        createDefaultConfig();
    }

    std::string appId =
        getIniValue("application", "app_id", "unknown");

    std::string debug =
        getIniValue("application", "debug", "false");

    std::string updateEnabled =
        getIniValue("update", "enabled", "false");

    std::string updateUrl =
        getIniValue("update", "update_url", "");

    std::string logDir =
        getIniValue("log", "log_dir", "logs");

    std::cout
        << "app_id=" << appId << "\n"
        << "debug=" << debug << "\n"
        << "update.enabled=" << updateEnabled << "\n"
        << "update_url=" << updateUrl << "\n"
        << "log_dir=" << logDir << "\n\n";

    std::cout << "Application is ready.\n";

    Sleep(2000);

    std::string closeTime = currentTimestamp();

    writeIniValue(
        "runtime",
        "last_close_time",
        closeTime
    );

    std::cout
        << "Application is shutting down.\n"
        << "last_close_time=" << closeTime << "\n";

    return 0;
}
