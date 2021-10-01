#pragma once

pid_t sys_wait(int *rstatus);
pid_t sys_waitpid(pid_t pid, int *rstatus, int opts);

