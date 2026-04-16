// See COPYING for license information.

// Defines for temporary file paths and the package database directory.
#ifndef TMPDIR
#define TMPDIR    "/tmp/depfinder."
#endif
#ifndef TMPFILE
#define TMPFILE(file, shellpid) \
    std::string(TMPDIR + std::string(shellpid) + std::string(file))
#endif
// Package database directory (Slackware-current uses /var/lib/pkgtools/packages/)
#ifndef PKGDIR
#define PKGDIR    "/var/lib/pkgtools/packages/"
#endif
