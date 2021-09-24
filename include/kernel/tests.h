#pragma once

void lab1_check_mem(struct boot_info *boot_info);

void lab2_check_pml4(void);
void lab2_check_paging(void);
void lab2_check_buddy(struct boot_info *boot_info);

void lab3_check_kmem();
void lab3_check_vas(struct page_table *pml4);

