#include <ptoria/http.h>
#include <curl/curl.h>
#include <spdlog/spdlog.h>
#include <ptoria/scriptservice.h>

// Helper for writing curl response
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string PerformHttpGet(const std::string& url)
{
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Ignore SSL verification for simplicity if needed, or keep it strict
        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 
        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
            readBuffer = ""; // Return empty on failure or handle error differently
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

DynValue* httpget(void* _, ScriptExecutionContext* ctx, CallbackArguments* args)
{
    int count = args->Count();
    if (count < 1)
    {
        return DynValue::FromString("Not enough arguments passed to httpget");
    }

    DynValue* urlVal = args->RawGet(0, false);
    if (urlVal == nullptr || urlVal->Type() != DynValue::DataType::String)
    {
        return DynValue::FromString("Invalid argument, expected string URL");
    }
    
    // We need to cast the DynValue string to a UnityString, then to std::string
    // But wait, DynValue::DataType::String in MoonSharp might map to System.String which is a UnityString*
    // Let's check how other functions do it.
    // loadstring: UnityString *code = (UnityString *)source->Cast(stringClass->GetType());
    
    UnityClass* stringClass = Unity::GetClass<"String", "mscorlib.dll", "System">();
    UnityString* urlUnityStr = (UnityString*)urlVal->Cast(stringClass->GetType());
    
    if (urlUnityStr == nullptr)
    {
         return DynValue::FromString("Failed to cast URL string");
    }
    
    std::string urlStr = urlUnityStr->ToString();
    
    std::string response = PerformHttpGet(urlStr);
    
    return DynValue::FromString(response);
}
