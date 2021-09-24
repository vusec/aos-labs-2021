/* Attempt to merge from the middle. */
#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	struct vma_info info;
	char *addr = (void *)0x1000000;

	mmap(addr, HPAGE_SIZE, PROT_READ | PROT_WRITE,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	memset(addr, 0, HPAGE_SIZE);
	mquery(&info, addr);

	switch (info.vm_mapped) {
	case VM_UNMAPPED: printf("unmapped\n"); break;
	case VM_4K_PAGE: printf("4K page\n"); break;
	case VM_2M_PAGE: printf("2M page\n"); break;
	}

	print_vmas();

	return 0;
}
