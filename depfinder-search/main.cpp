// depfinder-search: resolves shared library paths to owning Slackware packages.
// Adapted from zpm by Nigel Bosch <sparkynigel@gmail.com>.
// See COPYING for license information.
//
// Usage: depfinder-search <pid>
//   Reads  /tmp/depfinder.<pid>/LIBS      (one library path per line)
//   Writes /tmp/depfinder.<pid>/DEPS      (one "pkg1|pkg2" entry per line)

#include <iostream>
#include "CWhatProvides.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: depfinder-search <pid>\n";
        return 1;
    }

    std::string shellpid = argv[1];
    CWhatProvides wp(shellpid);
    return 0;
}
