#include <windows.h>
#include <winhttp.h>

#include <iostream>
#include <string>

//#pragma comment(lib, "winhttp.lib")

constexpr char CONFIG_FILE[] = ".\\config.ini";
constexpr wchar_t CURRENT_VERSION[] = L"1.0.0";

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

bool parseUpdateUrl(
    const std::string& url,
    std::wstring& host,
    INTERNET_PORT& port,
    std::wstring& path)
{
    if (url.rfind("http://", 0) != 0) {
        return false;
    }

    std::string rest = url.substr(7);

    size_t slash = rest.find('/');
    std::string hostPort =
        slash == std::string::npos ? rest : rest.substr(0, slash);

    std::string pathText =
        slash == std::string::npos ? "/" : rest.substr(slash);

    size_t colon = hostPort.find(':');

    std::string hostText =
        colon == std::string::npos
            ? hostPort
            : hostPort.substr(0, colon);

    port =
        colon == std::string::npos
            ? 80
            : static_cast<INTERNET_PORT>(
                std::stoi(hostPort.substr(colon + 1)));

    host.assign(hostText.begin(), hostText.end());
    path.assign(pathText.begin(), pathText.end());

    return true;
}

bool checkLatestVersion(
    const std::string& url,
    std::wstring& latestVersion)
{
    std::wstring host;
    std::wstring path;
    INTERNET_PORT port = 0;

    if (!parseUpdateUrl(url, host, port, path)) {
        return false;
    }

    HINTERNET session = WinHttpOpen(
        L"WindowsNativeAnalysisLab/1.0",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );

    if (!session) {
        return false;
    }

    HINTERNET connection = WinHttpConnect(
        session,
        host.c_str(),
        port,
        0
    );

    if (!connection) {
        WinHttpCloseHandle(session);
        return false;
    }

    HINTERNET request = WinHttpOpenRequest(
        connection,
        L"GET",
        path.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0
    );

    if (!request) {
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        return false;
    }

    bool ok =
        WinHttpSendRequest(
            request,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0
        ) &&
        WinHttpReceiveResponse(
            request,
            nullptr
        );

    if (!ok) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        return false;
    }

    char buffer[128]{};
    DWORD bytesRead = 0;

    ok = WinHttpReadData(
        request,
        buffer,
        sizeof(buffer) - 1,
        &bytesRead
    );

    if (ok) {
        buffer[bytesRead] = '\0';

        std::string text(buffer, bytesRead);

        while (!text.empty() &&
               (text.back() == '\r' || text.back() == '\n')) {
            text.pop_back();
        }

        latestVersion.assign(text.begin(), text.end());
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);

    return ok && !latestVersion.empty();
}

int main()
{
    std::cout << "NET-001-version-check\n\n";

    std::string enabled =
        getIniValue("update", "enabled", "false");

    std::string updateUrl =
        getIniValue(
            "update",
            "update_url",
            "http://127.0.0.1:8080/version"
        );

    std::cout
        << "update.enabled=" << enabled << "\n"
        << "update_url=" << updateUrl << "\n\n";

    if (enabled == "true") {
        std::wstring latestVersion;

        if (checkLatestVersion(updateUrl, latestVersion)) {
            std::wcout
                << L"Current version: " << CURRENT_VERSION << L"\n"
                << L"Latest version: " << latestVersion << L"\n";

            if (latestVersion == CURRENT_VERSION) {
                std::wcout << L"Already up to date.\n\n";
            } else {
                std::wcout << L"Update available.\n\n";
            }
        } else {
            std::cout << "Version check failed.\n\n";
        }
    } else {
        std::cout << "Update check disabled.\n\n";
    }

    std::cout << "Application is ready.\n";

    Sleep(2000);

    std::cout << "Application is shutting down.\n";

    return 0;
}
