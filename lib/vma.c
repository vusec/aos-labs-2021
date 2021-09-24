#include <lib.h>

void print_vmas(void)
{
	struct vma_info info;
	void *addr = NULL;

	while (addr < (void *)USER_LIM) {
		if (mquery(&info, addr) < 0) {
			break;
		}

		addr = info.vm_end;
		printf("0x%016llx - 0x%016llx ", info.vm_base, info.vm_end);

		switch (info.vm_type) {
		case VMA_FREE:
			printf("[---] free\n");
			break;
		case VMA_ANONYMOUS:
		case VMA_EXECUTABLE:
			printf("[%c%c%c] %s\n",
				(info.vm_prot & PROT_READ) ? 'r' : '-',
				(info.vm_prot & PROT_WRITE) ? 'w' : '-',
				(info.vm_prot & PROT_EXEC) ? 'x' : '-',
				info.vm_name);
			break;
		}
	}
}

