#pragma once

enum {
	/* Resources temporarily unavailable. */
	EAGAIN = 1,
	/* No child process. */
	ECHILD,
	/* File exists. */
	EEXIST,
	/* Bad address. */
	EFAULT,
	/* Invalid argument. */
	EINVAL,
	/* Not enough space/cannot allocate memory. */
	ENOMEM,
	/* Function not implemented. */
	ENOSYS,
	/* Operation not permitted. */
	EPERM,
};

