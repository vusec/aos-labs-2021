/* Unmaps the .text segment. This should page fault. */
#include <lib.h>

int main(int argc, char **argv)
{
	struct vma_info info;
	void *addr = NULL;

	while (addr < (void *)USER_LIM) {
		if (mquery(&info, addr) < 0) {
			break;
		}

		addr = info.vm_end;

		if (info.vm_type == VMA_FREE) {
			continue;
		}
		
		if (strcmp(info.vm_name, ".text") != 0) {
			continue;
		}
	
		munmap(info.vm_base, (size_t)info.vm_end - (size_t)info.vm_base);
	}

	return 0;
}

