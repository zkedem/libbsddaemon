/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1990, 1993 The Regents of the University of California.
 * Copyright (c) 2017 Mariusz Zaborski <oshogbo@FreeBSD.org>
 * Copyright (c) 2024 Ziv Kedem <atxcorp101@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Changes from the original FreeBSD version from 2017:
 * - Calls to __libc_sigaction() have been replaced by calls to sigaction().
 * - Calls to _dup2(), _open(), and _close() have been replaced by calls to
 *   their non-underscored equivalents.
 * - namespace.h, un-namespace.h, and libc_private.h are not included.
 * - All prerequisite includes have been moved into bsddaemon.h.
 * - O_EXEC is defined in bsddaemon.h for non-FreeBSD systems.
 * The comments and coding style of the original daemon.c have been preserved.
 */
#include "bsddaemon.h"

int
daemonfd(int chdirfd, int nullfd)
{
	struct sigaction osa, sa;
	pid_t newgrp;
	int oerrno;
	int osa_ok;

	/* A SIGHUP may be thrown when the parent exits below. */
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	osa_ok = sigaction(SIGHUP, &sa, &osa);

	switch (fork()) {
	case -1:
		return (-1);
	case 0:
		break;
	default:
		/*
		 * A fine point:  _exit(0), not exit(0), to avoid triggering
		 * atexit(3) processing
		 */
		_exit(0);
	}

	newgrp = setsid();
	oerrno = errno;
	if (osa_ok != -1)
		sigaction(SIGHUP, &osa, NULL);

	if (newgrp == -1) {
		errno = oerrno;
		return (-1);
	}

	if (chdirfd != -1)
		(void)fchdir(chdirfd);

	if (nullfd != -1) {
		(void)dup2(nullfd, STDIN_FILENO);
		(void)dup2(nullfd, STDOUT_FILENO);
		(void)dup2(nullfd, STDERR_FILENO);
	}
	return (0);
}

int
daemon(int nochdir, int noclose)
{
	int chdirfd, nullfd, ret;

	if (!noclose)
		nullfd = open(_PATH_DEVNULL, O_RDWR, 0);
	else
		nullfd = -1;

	if (!nochdir)
		chdirfd = open("/", O_EXEC);
	else
		chdirfd = -1;

	ret = daemonfd(chdirfd, nullfd);

	if (chdirfd != -1)
		close(chdirfd);

	if (nullfd > 2)
		close(nullfd);

	return (ret);
}
