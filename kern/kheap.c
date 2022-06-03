#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>
#define numberOfFrames (KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE
//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
int kheap_frame[numberOfFrames];
int lastAlocatedAddress = 0;
int32 kheap_neededPages[numberOfFrames]; // frames
uint32 loop = 0;
void* kmalloc(unsigned int size) {
	int flag = 0;
	int cnt = 0;
	int address = lastAlocatedAddress;
	uint32 startVA = (address * PAGE_SIZE) + KERNEL_HEAP_START;
	uint32 returnAddress = 0;
	int roundedSize = ROUNDUP(size, PAGE_SIZE);
	int numberOfPages = roundedSize / PAGE_SIZE;
	do {
		// check if the frame is empty
		if (kheap_frame[address] == 0) {
			cnt++;
			// check if the return address is not yet assigned
			if (returnAddress == 0) {
				returnAddress = (address * PAGE_SIZE) + KERNEL_HEAP_START;
			}
			if (cnt == numberOfPages) {
				flag = 1;
				break;
			}
			address++;
		} else {
			address++;
			cnt = 0;
			returnAddress = 0;
		}
		if ((address * PAGE_SIZE) + KERNEL_HEAP_START >= KERNEL_HEAP_MAX) {
			address = 0;
			cnt = 0;
			returnAddress = 0;
		}
	} while (flag != 1 && address != lastAlocatedAddress);
	if (flag == 1) {
		lastAlocatedAddress = address;
		uint32 tmp = returnAddress;
		if (returnAddress == 0) {
			return NULL;
		} else {
			for (int i = 0; i < numberOfPages; i++) {
				struct Frame_Info *frame = NULL;
				int returnFrame = allocate_frame(&frame);
				if (returnFrame == E_NO_MEM) {
					return NULL;
				} else {
					int mappedFrame = map_frame(ptr_page_directory, frame,
							(void*) tmp, PERM_PRESENT | PERM_WRITEABLE);
					if (mappedFrame == E_NO_MEM) {
						free_frame(frame);
						return NULL;
					}
					kheap_frame[(tmp - KERNEL_HEAP_START) / PAGE_SIZE] = 1;
					tmp += PAGE_SIZE;
				}
			}
		}

		//uint32 reference=KERNEL_HEAP_START;  // start address
		loop = tmp;
		int idx = (returnAddress - KERNEL_HEAP_START) / PAGE_SIZE;   // index 0
		kheap_neededPages[idx] = numberOfPages;
		return (void*) returnAddress;
		//return returnAddress;
	} else {
		return 0;
	}
}
void kfree(void* virtual_address) {
	//TODO: [PROJECT 2022 - [2] Kernel Heap] kfree()
	// removing pages by un-maping them
	int idx = ((uint32) virtual_address - KERNEL_HEAP_START) / PAGE_SIZE;
	int sizeOfVa = kheap_neededPages[idx];
	kheap_neededPages[idx] = 0;
	for (int i = 0; i < sizeOfVa; i++) {
		uint32 va = ((uint32) virtual_address + (i * PAGE_SIZE));
		unmap_frame(ptr_page_directory, (void*) va);
		kheap_frame[(va - KERNEL_HEAP_START) / PAGE_SIZE] = 0;
	}
}

unsigned int kheap_virtual_address(unsigned int physical_address) {
	//TODO: [PROJECT 2022 - [3] Kernel Heap] kheap_virtual_address()
	//panic("kheap_virtual_address() is not implemented yet...!!");
	int frameNumber = physical_address / PAGE_SIZE;
	for (uint32 i = KERNEL_HEAP_START; i < loop; i += PAGE_SIZE) {
		uint32* ptr_table = NULL;
		get_page_table(ptr_page_directory, (void*) i, &ptr_table);
		if (ptr_table != NULL) {
			int tableEntry = ptr_table[PTX(i)];
			int frame = tableEntry >> 12;
			if (frame == frameNumber) {
				if ((ptr_table[PTX(i)] & PERM_PRESENT) != 0) {
					return i;
				}
			}

		}

		//return the virtual address corresponding to given physical_address
		//refer to the project presentation and documentation for details

	}
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address) {
	//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	uint32* ptr_page_table = NULL;
	struct Frame_Info* virtualAddressFrame = get_frame_info(ptr_page_directory,
			(void*) virtual_address, &ptr_page_table);
	if (virtualAddressFrame == NULL) {
		return 0;
	}
	uint32 checkpresentBit = ptr_page_table[PTX(virtual_address)] & PERM_PRESENT;
	if (checkpresentBit == 0) {
		return 0;
	}

	uint32 ThePyhsicalAddress;
	ThePyhsicalAddress = to_physical_address(virtualAddressFrame);
	return ThePyhsicalAddress;

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	return 0;
}
