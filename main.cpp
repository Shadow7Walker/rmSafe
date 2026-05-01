#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#else
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

bool moveToTrash(const fs::path& filePath) {
    if (!fs::exists(filePath)) return false;

#ifdef _WIN32
    // SHFileOperation requires double null-terminated string
    std::wstring pathStr = fs::absolute(filePath).lexically_normal().wstring();
    pathStr.push_back(L'\0');
    pathStr.push_back(L'\0');

    SHFILEOPSTRUCTW fileOp = {0};
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = pathStr.c_str();
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;

    int result = SHFileOperationW(&fileOp);
    return result == 0;
#else
    std::string pathStr = fs::absolute(filePath).string();
    
    // First try standard commands
    std::string cmd = "trash \"" + pathStr + "\" 2>/dev/null";
    if (system(cmd.c_str()) == 0) return true;

    cmd = "gio trash \"" + pathStr + "\" 2>/dev/null";
    if (system(cmd.c_str()) == 0) return true;

    cmd = "kioclient5 move \"" + pathStr + "\" trash:/ 2>/dev/null";
    if (system(cmd.c_str()) == 0) return true;

    // Manual XDG Trash fallback
    const char* homeDir = getenv("HOME");
    if (!homeDir) return false;

    fs::path trashDir = fs::path(homeDir) / ".local" / "share" / "Trash";
    fs::path filesDir = trashDir / "files";
    fs::path infoDir = trashDir / "info";

    std::error_code ec;
    fs::create_directories(filesDir, ec);
    fs::create_directories(infoDir, ec);

    std::string filename = filePath.filename().string();
    fs::path destFile = filesDir / filename;
    fs::path infoFile = infoDir / (filename + ".trashinfo");

    int counter = 1;
    while (fs::exists(destFile) || fs::exists(infoFile)) {
        std::string newName = filename + " " + std::to_string(counter++);
        destFile = filesDir / newName;
        infoFile = infoDir / (newName + ".trashinfo");
    }

    try {
        fs::rename(filePath, destFile);

        std::ofstream info(infoFile);
        if (info.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            struct tm* parts = std::localtime(&now_c);
            char dateStr[20];
            std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%dT%H:%M:%S", parts);

            info << "[Trash Info]\n";
            info << "Path=" << fs::absolute(filePath).string() << "\n";
            info << "DeletionDate=" << dateStr << "\n";
            info.close();
        }
        return true;
    } catch (...) {
        return false;
    }
#endif
}

int main(int argc, char* argv[]) {
    bool force = false;
    bool interactive = false;
    bool recursive = false;
    bool verbose = false;
    bool emptyDir = false;
    bool stopParsing = false;
    std::vector<std::string> files;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (!stopParsing && arg == "--") {
            stopParsing = true;
            continue;
        }
        if (!stopParsing && arg[0] == '-' && arg.length() > 1) {
            if (arg == "--force") force = true;
            else if (arg == "--interactive") interactive = true;
            else if (arg == "--recursive") recursive = true;
            else if (arg == "--dir") emptyDir = true;
            else if (arg == "--verbose") verbose = true;
            else if (arg == "--help") {
                std::cout << "Usage: rmSafe [OPTION]... [FILE]...\n"
                          << "Safe rm command that moves files to the recycle bin/trash instead of deleting permanently.\n\n"
                          << "Options:\n"
                          << "  -f, --force           ignore nonexistent files and arguments, never prompt\n"
                          << "  -i, --interactive     prompt before every removal\n"
                          << "  -r, -R, --recursive   remove directories and their contents recursively\n"
                          << "  -d, --dir             remove empty directories\n"
                          << "  -v, --verbose         explain what is being done\n"
                          << "      --help            display this help and exit\n";
                return 0;
            } else {
                for (size_t j = 1; j < arg.length(); ++j) {
                    char c = arg[j];
                    if (c == 'f') force = true;
                    else if (c == 'i') interactive = true;
                    else if (c == 'r' || c == 'R') recursive = true;
                    else if (c == 'd') emptyDir = true;
                    else if (c == 'v') verbose = true;
                    else {
                        std::cerr << "rmSafe: invalid option -- '" << c << "'\n";
                        return 1;
                    }
                }
            }
        } else {
            files.push_back(arg);
        }
    }

    if (files.empty()) {
        std::cerr << "rmSafe: missing operand\n";
        std::cerr << "Try 'rmSafe --help' for more information.\n";
        return 1;
    }

    int exitCode = 0;

    for (const auto& file : files) {
        fs::path p(file);
        
        std::error_code ec;
        auto st = fs::symlink_status(p, ec);
        if (ec || st.type() == fs::file_type::not_found) {
            if (!force) {
                std::cerr << "rmSafe: cannot remove '" << file << "': No such file or directory\n";
                exitCode = 1;
            }
            continue;
        }

        if (fs::is_directory(p)) {
            if (!recursive && !emptyDir) {
                std::cerr << "rmSafe: cannot remove '" << file << "': Is a directory\n";
                exitCode = 1;
                continue;
            }
            if (emptyDir && !recursive && !fs::is_empty(p)) {
                std::cerr << "rmSafe: cannot remove '" << file << "': Directory not empty\n";
                exitCode = 1;
                continue;
            }
        }

        if (interactive) {
            std::cout << "rmSafe: remove '" << file << "'? ";
            std::string answer;
            std::getline(std::cin, answer);
            if (answer.empty() || (answer[0] != 'y' && answer[0] != 'Y')) {
                continue;
            }
        }

        if (moveToTrash(p)) {
            if (verbose) {
                std::cout << "removed '" << file << "' (to trash)\n";
            }
        } else {
            if (!force) {
                std::cerr << "rmSafe: cannot remove '" << file << "': Failed to move to trash\n";
                exitCode = 1;
            }
        }
    }

    return exitCode;
}
