#ifndef MOD_HPP
#define MOD_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <optional>
#include <filesystem>

namespace mod {
    class ToriaFile {
    public:
        ToriaFile() = default;

        bool load(const std::filesystem::path& path) {
            m_path = path;
            if (!std::filesystem::exists(path)) return false;

            std::ifstream file(path);
            if (!file.is_open()) return false;

            m_data.clear();
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;

                size_t delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = line.substr(0, delimiterPos);
                    std::string value = line.substr(delimiterPos + 1);
                    m_data[key] = value;
                }
            }
            return true;
        }

        bool save(const std::filesystem::path& path = "") {
            std::filesystem::path targetPath = path.empty() ? m_path : path;
            if (targetPath.empty()) return false;

            std::ofstream file(targetPath);
            if (!file.is_open()) return false;

            for (const auto& [key, value] : m_data) {
                file << key << "=" << value << "\n";
            }
            return true;
        }

        std::optional<std::string> get(const std::string& key) const {
            if (auto it = m_data.find(key); it != m_data.end()) {
                return it->second;
            }
            return std::nullopt;
        }

        void set(const std::string& key, const std::string& value) {
            m_data[key] = value;
        }

    private:
        std::filesystem::path m_path;
        std::map<std::string, std::string> m_data;
    };
}

#endif /* MOD_HPP */
