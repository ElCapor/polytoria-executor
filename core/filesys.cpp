#include <core/filesys.h>
#include <nfd.h>
#include <fstream>
#include <ostream>
#include <iostream>
#include <sstream>

std::optional<std::string> filesys::SaveDialog(std::vector<FileSelectFilters> filters)
{
    std::string savePath;
    NFD_Init();

    std::vector<nfdu8filteritem_t> nfdFilters;
    for (const auto &filter : filters)
    {
        nfdFilters.push_back({filter.description.c_str(), filter.extensions.c_str()});
    }

    nfdu8char_t *outPath;
    nfdsavedialogu8args_t args = {0};
    args.filterList = nfdFilters.data();
    args.filterCount = static_cast<nfdfiltersize_t>(nfdFilters.size());
    nfdresult_t result = NFD_SaveDialogU8_With(&outPath, &args);

    if (result == NFD_OKAY)
    {
        savePath = outPath;
        NFD_FreePathU8(outPath);
        NFD_Quit();
        return savePath;
    }
    else if (result == NFD_CANCEL)
    {
        NFD_Quit();
        return std::nullopt;
    }
    else
    {
        printf("Error: %s\n", NFD_GetError());
        NFD_Quit();
        return std::nullopt;
    }
}

std::optional<std::string> filesys::OpenDialog(std::vector<FileSelectFilters> filters)
{
    std::string openPath;
    NFD_Init();

    std::vector<nfdu8filteritem_t> nfdFilters;
    for (const auto &filter : filters)
    {
        nfdFilters.push_back({filter.description.c_str(), filter.extensions.c_str()});
    }

    nfdu8char_t *outPath;
    nfdopendialogu8args_t args = {0};
    args.filterList = nfdFilters.data();
    args.filterCount = static_cast<nfdfiltersize_t>(nfdFilters.size());
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);

    if (result == NFD_OKAY)
    {
        openPath = outPath;
        NFD_FreePathU8(outPath);
        NFD_Quit();
        return openPath;
    }
    else if (result == NFD_CANCEL)
    {
        NFD_Quit();
        return std::nullopt;
    }
    else
    {
        printf("Error: %s\n", NFD_GetError());
        NFD_Quit();
        return std::nullopt;
    }
}

std::optional<std::string> filesys::ReadFileAsString(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void filesys::WriteStringToFile(const std::string &filePath, const std::string &content)
{
    std::ofstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return;
    }

    file << content;
    if (file.fail())
    {
        std::cerr << "Failed to write to file: " << filePath << std::endl;
    }
}

void filesys::OpenUrlInBrowser(const std::string &url)
{
    std::string command = "start " + url;
    system(command.c_str());
}