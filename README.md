# libbsddaemon
A portable version of FreeBSD's daemonization functions

## Why?
Creating a daemon, or background process, on POSIX-compliant operating systems is rather tedious. Traditionally, it involves some variation of the following steps[^1]\:

1. Closing all open file descriptors except for standard input, output, and error.
2. Resetting all signal handlers.
3. Resetting the signal mask.
4. Sanitizing environment variables.
5. Creating a background process with `fork()`.
6. Creating a new session with `setsid()`.
7. Calling `fork()` a second time.
8. Exiting the original background process, so that init/PID 1 becomes our daemon's parent.
9. Connecting /dev/null to standard input, output, and error.
10. Resetting the umask to 0.
11. Navigating to the root directory.
12. Saving the daemon's PID to a file to prevent multiple invocations.
13. Dropping privileges (optional).
14. Informing the original process that daemon initialization has completed.
15. Exiting the original process.

The actual steps needed to daemonize a program are dependent on the particular init system in use by the OS (this is foretold by step 8). The above steps are typical of a daemon written for an OS with a System V-style init. While systemd and other init systems will recognize System V-compatible daemons, none of the above steps are required, and some may even cause the init system to malfunction[^1]. Moreover, POSIX does not specify a standard init system, and since daemons depend on the particularities of init, this makes it challenging to create a truly cross-POSIX daemon.

FreeBSD provides two functions in its C standard library, `daemon()` and `daemonfd()`, that can provide much of the heavy lifting needed to initialize a daemon. They perform some of the above listed steps, which works well with FreeBSD's rc.d init system[^2]. The `daemon()` function is replicated in the GNU C library (glibc) for compatibility with BSD, and is enabled when the macros `_BSD_SOURCE` or `_XOPEN_SOURCE` are defined at compilation[^3]. Despite their usefulness, these functions are not part of POSIX, likely for reasons mentioned earlier.

While I am aware of other implementations of what I am trying to achieve, they all lack at least one of these attributes:

+ Simplicity. [libdaemon](https://github.com/thingnario/libdaemon-0.14) (not to be confused with THIS repo!) exports a function for every step of the daemonization routine as described above, rather than one or two functions that do everything in one go.
+ Generalization. The daemon skeleton in [daemon-skeleton-linux-c](https://github.com/pasce/daemon-skeleton-linux-c) assumes that the program should immediately exit if it fails to daemonize, and that writing to a log should be part of the daemonization routine, even though not all daemons produce log files, and logging can be handled at a later stage.
+ Familiarity. `daemon()` and `daemonfd()` are well-established in FreeBSD, and to an extent in GNU/Linux as well. It is better to leverage an existing API that is well-known and proven rather than reinventing the wheel.

## Goals
The overarching goal of libbsddaemon is to create a library providing daemonization routines that is portable across all POSIX-compliant OSes and easy to use and integrate. Bearing this in mind, I hope to keep the functionality of libbsddaemon as close as possible to its original FreeBSD counterpart as both implementations evolve over time.

## Credits
libbsddaemon is based on [daemon.c](https://cgit.freebsd.org/src/tree/lib/libc/gen/daemon.c) from FreeBSD's libc.

[^1]: [daemon(7) - Linux manual page](https://man7.org/linux/man-pages/man7/daemon.7.html)
[^2]: [paulgorman.org/technical](https://paulgorman.org/technical/freebsd-init.txt.html)
[^3]: [c - warning: implicit declaration of function 'daemon' - Stack Overflow](https://stackoverflow.com/questions/24161945/warning-implicit-declaration-of-function-daemon)