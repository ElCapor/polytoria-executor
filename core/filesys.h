#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <optional>
#include <string>
#include <vector>

namespace filesys {
    struct FileSelectFilters {
            std::string description;
            std::string extensions; // e.g. "txt,log"
        };

        std::optional<std::string> SaveDialog(std::vector<FileSelectFilters> filters);
        std::optional<std::string> OpenDialog(std::vector<FileSelectFilters> filters);
        
        std::optional<std::string> ReadFileAsString(const std::string &filePath);
        void WriteStringToFile(const std::string &filePath, const std::string &content);

        void OpenUrlInBrowser(const std::string &url);
}

#endif /* FILESYSTEM_H */
