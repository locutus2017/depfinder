// See COPYING for license information.

#include <fstream>
#include <algorithm>
#include <iostream>
#include <climits>
#include <cstdlib>
#include "defs.h"
#include "CWhatProvides.h"

CWhatProvides::CWhatProvides(const std::string& shellpid)
{
    build_index(shellpid);
    process_queries(shellpid);
}

// Phase 1: Walk every installed package file once and build the index.
// Time: O(total files across all installed packages) — done once.
void CWhatProvides::build_index(const std::string& shellpid)
{
    std::ifstream pkglist(TMPFILE("/pkglist.log", shellpid));
    if (!pkglist.is_open()) {
        std::cerr << "depfinder-search: cannot open pkglist.log\n";
        return;
    }

    std::string pkgname;
    while (std::getline(pkglist, pkgname)) {
        if (pkgname.empty()) continue;

        std::ifstream pkgfile(std::string(PKGDIR) + pkgname);
        if (!pkgfile.is_open()) continue;

        std::string line;
        bool in_file_list = false;
        while (std::getline(pkgfile, line)) {
            if (!in_file_list) {
                // Detect the "FILE LIST:" section header (case-insensitive)
                std::string lower = line;
                std::transform(lower.begin(), lower.end(), lower.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                if (lower.find("file list:") == 0)
                    in_file_list = true;
            } else {
                if (line.empty()) continue;
                // Strip a trailing .new suffix (config file variants)
                if (line.size() >= 4 &&
                    line.compare(line.size() - 4, 4, ".new") == 0)
                    line.erase(line.size() - 4);
                // Store with a leading / to match absolute paths from ldconfig
                _file_to_pkgs["/" + line].push_back(pkgname);
            }
        }
    }
}

// Phase 2: Read every library path from LIBS, look it up in the index,
// and write the owning package(s) to DEPS.
// Time: O(1) per library after the index is built.
void CWhatProvides::process_queries(const std::string& shellpid)
{
    std::ifstream libs(TMPFILE("/LIBS", shellpid));
    std::ofstream deps(TMPFILE("/DEPS", shellpid));
    if (!libs.is_open()) {
        std::cerr << "depfinder-search: cannot open LIBS\n";
        return;
    }
    if (!deps.is_open()) {
        std::cerr << "depfinder-search: cannot open DEPS for writing\n";
        return;
    }

    std::string libpath;
    while (std::getline(libs, libpath)) {
        if (libpath.empty()) continue;

        // Normalize to an absolute path for the index lookup
        std::string key = (libpath[0] == '/') ? libpath : "/" + libpath;

        auto it = _file_to_pkgs.find(key);

        // If not found by direct path, try resolving symlinks via realpath(3)
        if (it == _file_to_pkgs.end() || it->second.empty()) {
            char resolved[PATH_MAX];
            if (realpath(key.c_str(), resolved) != nullptr &&
                key != std::string(resolved)) {
                it = _file_to_pkgs.find(resolved);
            }
        }

        if (it == _file_to_pkgs.end() || it->second.empty()) continue;

        const std::vector<std::string>& pkgs = it->second;
        // Multiple packages owning the same file are joined with '|' (OR)
        for (std::size_t i = 0; i < pkgs.size(); ++i) {
            if (i > 0) deps << "|";
            deps << pkgs[i];
        }
        deps << "\n";
    }
}
