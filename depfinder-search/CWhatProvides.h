// Reads every installed package's file list once, builds an in-memory
// filename->packages index, then resolves all library queries in O(1) per
// lookup instead of the original O(packages) linear scan per library.
// See COPYING for license information.

#ifndef CWHATPROVIDES_H
#define CWHATPROVIDES_H

#include <string>
#include <unordered_map>
#include <vector>

class CWhatProvides
{
public:
    explicit CWhatProvides(const std::string& shellpid);
private:
    void build_index(const std::string& shellpid);
    void process_queries(const std::string& shellpid);

    // Maps absolute file path -> list of package names that contain it.
    std::unordered_map<std::string, std::vector<std::string>> _file_to_pkgs;
};

#endif
