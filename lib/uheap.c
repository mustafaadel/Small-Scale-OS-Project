#include <inc/lib.h>

#define numberOfFrames (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE
int uheap_frame[numberOfFrames];
int lastAlocatedAddress = 0;
int32 uheap_neededPages[numberOfFrames]; // frames
// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
void* malloc(uint32 size) {
	//TODO: [PROJECT 2022 - [9] User Heap malloc()] [User Side]
	// Write your code here, remove the panic and write your code
	///panic("malloc() is not implemented yet...!!");

	// Steps:
	//    1) Implement NEXT FIT strategy to search the heap for suitable space
	//        to the required allocation size (space should be on 4 KB BOUNDARY)
	int roundedSize = ROUNDUP(size, PAGE_SIZE);
	int numberOfPages = roundedSize / PAGE_SIZE;
	int flag = 0;
	int cnt = 0;
	int address = lastAlocatedAddress;
	uint32 startVA = (address * PAGE_SIZE) + USER_HEAP_START;
	uint32 returnAddress = 0;
	do {
		// check if the frame is empty
		if (uheap_frame[address] == 0) {
			cnt++;
			// check if the return address is not yet assigned
			if (returnAddress == 0) {
				returnAddress = (address * PAGE_SIZE) + USER_HEAP_START;
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
		if ((address * PAGE_SIZE) + USER_HEAP_START >= USER_HEAP_MAX) {
			address = 0;
			cnt = 0;
			returnAddress = 0;
		}
	} while (flag != 1 && address != lastAlocatedAddress);
	if (flag == 1) {
		lastAlocatedAddress = address;
		uint32 ret = returnAddress;
		uint32 tmp = returnAddress;

//    //    2) if no suitable space found, return NULL
		if (ret == 0) {
			return NULL;
		}
		//     Else,
//    else {
//        //    3) Call sys_allocateMem to invoke the Kernel for allocation
		else {
			sys_allocateMem(ret, (uint32) size);
			for (int i = 0; i < numberOfPages; i++) {
				uheap_frame[(tmp - USER_HEAP_START) / PAGE_SIZE] = 1;
				tmp += PAGE_SIZE;
			}
//        //     4) Return pointer containing the virtual address of allocated space,
			int idx = (ret - USER_HEAP_START) / PAGE_SIZE;   // index 0
			uheap_neededPages[idx] = numberOfPages;
			//   cprintf("allocated address is: %x\n",(void *)ret);
		}
		return (void *) ret;
	} else
		return 0;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable) {
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName) {
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address) {
	//cprintf("in free:\n");
	//TODO: [PROJECT 2022 - [11] User Heap free()] [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");

	int idx = ((uint32) virtual_address - USER_HEAP_START) / PAGE_SIZE;
	int sizeOfVa = uheap_neededPages[idx];
	uint32 size = sizeOfVa * PAGE_SIZE;
	sys_freeMem((uint32) virtual_address, size);
	//cprintf("freed address is: %x\n",(void *)virtual_address);
	uheap_neededPages[idx] = 0;
	//sizeOfVa = ROUNDUP(sizeOfVa, PAGE_SIZE);
	for (int i = 0; i < sizeOfVa; i++) {
		uint32 va = ((uint32) virtual_address + (i * PAGE_SIZE));
		uheap_frame[(va - USER_HEAP_START) / PAGE_SIZE] = 0;
	}
}

//you shold get the size of the given allocation using its address
//you need to call sys_freeMem()
//refer to the project presentation and documentation for details

void sfree(void* virtual_address) {
	panic("sfree() is not requried ..!!");
}

//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size) {
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
