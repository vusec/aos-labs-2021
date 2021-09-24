#include <lib.h>

int main(int argc, char **argv)
{
	struct vma_info info;
	const char *s = "Hello, world!";
	void *addr = NULL;

	while (addr < (void *)USER_LIM) {
		if (mquery(&info, addr) < 0) {
			break;
		}

		addr = info.vm_end;

		if (info.vm_type == VMA_FREE) {
			continue;
		}
		
		if (strcmp(info.vm_name, ".data") != 0) {
			continue;
		}

		switch (info.vm_mapped) {
		case VM_UNMAPPED: printf("unmapped\n"); break;
		case VM_4K_PAGE: printf("4K page\n"); break;
		case VM_2M_PAGE: printf("2M page\n"); break;
		}
	}

	print_vmas();

	return 0;
}
