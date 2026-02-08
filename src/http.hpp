#ifndef HTTP_HPP
#define HTTP_HPP

#include <Windows.h>
#include <winhttp.h>
#include <string>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

namespace Http {
    
    inline std::string HttpGet(const std::string& url) {
        std::string result;
        
        // Parse URL
        std::wstring wUrl(url.begin(), url.end());
        
        URL_COMPONENTS urlComp = { 0 };
        urlComp.dwStructSize = sizeof(urlComp);
        
        wchar_t hostName[256] = { 0 };
        wchar_t urlPath[2048] = { 0 };
        
        urlComp.lpszHostName = hostName;
        urlComp.dwHostNameLength = sizeof(hostName) / sizeof(wchar_t);
        urlComp.lpszUrlPath = urlPath;
        urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(wchar_t);
        
        if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.length(), 0, &urlComp)) {
            return "[HTTP Error] Failed to parse URL";
        }
        
        HINTERNET hSession = WinHttpOpen(
            L"PolyExecutor/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0
        );
        
        if (!hSession) {
            return "[HTTP Error] Failed to open session";
        }
        
        HINTERNET hConnect = WinHttpConnect(
            hSession,
            hostName,
            urlComp.nPort,
            0
        );
        
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to connect";
        }
        
        DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
        
        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            urlPath,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            flags
        );
        
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to open request";
        }
        
        // Send request
        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to send request";
        }
        
        // Receive response
        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to receive response";
        }
        
        // Read data
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;
            
            char* buffer = new char[dwSize + 1];
            ZeroMemory(buffer, dwSize + 1);
            
            if (WinHttpReadData(hRequest, buffer, dwSize, &dwDownloaded)) {
                result.append(buffer, dwDownloaded);
            }
            
            delete[] buffer;
        } while (dwSize > 0);
        
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        
        return result;
    }
    
    inline std::string HttpPost(const std::string& url, const std::string& data, const std::string& contentType = "application/json") {
        std::string result;
        
        std::wstring wUrl(url.begin(), url.end());
        
        URL_COMPONENTS urlComp = { 0 };
        urlComp.dwStructSize = sizeof(urlComp);
        
        wchar_t hostName[256] = { 0 };
        wchar_t urlPath[2048] = { 0 };
        
        urlComp.lpszHostName = hostName;
        urlComp.dwHostNameLength = sizeof(hostName) / sizeof(wchar_t);
        urlComp.lpszUrlPath = urlPath;
        urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(wchar_t);
        
        if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.length(), 0, &urlComp)) {
            return "[HTTP Error] Failed to parse URL";
        }
        
        HINTERNET hSession = WinHttpOpen(
            L"PolyExecutor/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0
        );
        
        if (!hSession) return "[HTTP Error] Failed to open session";
        
        HINTERNET hConnect = WinHttpConnect(hSession, hostName, urlComp.nPort, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to connect";
        }
        
        DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
        
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to open request";
        }
        
        std::wstring wContentType = L"Content-Type: " + std::wstring(contentType.begin(), contentType.end());
        
        if (!WinHttpSendRequest(hRequest, wContentType.c_str(), -1L, (LPVOID)data.c_str(), (DWORD)data.length(), (DWORD)data.length(), 0)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to send request";
        }
        
        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "[HTTP Error] Failed to receive response";
        }
        
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;
            
            char* buffer = new char[dwSize + 1];
            ZeroMemory(buffer, dwSize + 1);
            
            if (WinHttpReadData(hRequest, buffer, dwSize, &dwDownloaded)) {
                result.append(buffer, dwDownloaded);
            }
            
            delete[] buffer;
        } while (dwSize > 0);
        
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        
        return result;
    }
}

#endif // HTTP_HPP
