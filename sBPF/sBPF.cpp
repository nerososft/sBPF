#include <stdio.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include "include/vm/vm.h"
#include <elf.h>

void read_elf(Elf64_Ehdr header, const char* elfPath){
    FILE* file = fopen(elfPath, "rb");
    if(file){
       fread(&header,1,sizeof(header),file);
       if(memcmp(header.e_ident,ELFMAG,SELFMAG)!=0){
           perror("Not a vaild elf file");
           exit(0);
       }
    }   
    fclose(file);
}


unsigned long *readEBPFBinaryFile(const char *elfPath){
    Elf64_Ehdr header;
    read_elf(header, elfPath);
    printf("entry: %d\n",header.e_machine);
}

void bpf_attach_vm(const char* symbol,VM_t &vm){
    // todo : 1. get return from inkernel function
    // todo : 2. push them into register6 - register9
    // todo : 3. just run the vm
    vm.regs[6] = 1;
    vm.regs[7] = 2;
    vm.regs[8] = 3;
    vm.regs[9] = 4;

    vm_run(vm);
}

int main(int argc,char *argv[]){
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();

    // readEBPFBinaryFile("/workspaces/NMCS/sBPF/example/bpf_program.o");

    VM_t vm;
    vm_init(vm, 1);
    u64 program[] = {
        // 0xb701000021000000, 0x6b1af8ff00000000,
        // 0x1801000050462057, 0x000000006f726c64,
        // 0x7b1af0ff00000000, 0x1801000048656c6c,
        // 0x000000006f2c2042, 0x7b1ae8ff00000000,
        // 0xbfa1000000000000, 0x07010000e8ffffff,
        // 0xb702000012000000, 0x8500000006000000,
        // 0xb700000000000000, 0x9500000000000000,
        // 0x48656c6c6f2c2042, 0x504620576f726c64,
        // 0x210047504c000000, 0x0000000000000000,
        // 0x0000000000000000, 0x0000000000000000,
        // 0x3500000010000500, 0x0000000000000000,
        // 0x0000000000000000, 0x0700000010000300,
        // 0x0000000000000000, 0x0000000000000000,
        // 0x002e746578740062, 0x70665f70726f6700,
        // 0x7472616365706f69, 0x6e742f7379736361,
        // 0x6c6c732f7379735f, 0x656e7465725f6578,
        // 0x65637665005f6c69, 0x63656e7365002e73,
        // 0x7472746162002e73, 0x796d746162002e72,
        // 0x6f646174612e7374, 0x72312e3100000000,
        // 0x0000000000000000, 0x0000000000000000,
        // 0x0000000000000000, 0x0000000000000000,
        // 0x0000000000000000, 0x0000000000000000,
        // 0x0000000000000000, 0x0000000000000000,
        // 0x3e00000003000000, 0x0000000000000000,
        // 0x0000000000000000, 0x1001000000000000,
        // 0x5d00000000000000, 0x0000000000000000,
        // 0x0100000000000000, 0x0000000000000000,
        // 0x0100000001000000, 0x0600000000000000,
        // 0x0000000000000000, 0x4000000000000000,
        // 0x0000000000000000, 0x0000000000000000,
        // 0x0400000000000000, 0x0000000000000000,
        // 0x1000000001000000, 0x0600000000000000,
        // 0x0000000000000000, 0x4000000000000000,
        // 0x7000000000000000, 0x0000000000000000,
        // 0x0800000000000000, 0x0000000000000000,
        // 0x4e00000001000000, 0x3200000000000000,
        // 0x0000000000000000, 0xb000000000000000,
        // 0x1200000000000000, 0x0000000000000000,
        // 0x0100000000000000, 0x0100000000000000,
        // 0x3600000001000000, 0x0300000000000000,
        // 0x0000000000000000, 0xc200000000000000,
        // 0x0400000000000000, 0x0000000000000000,
        // 0x0100000000000000, 0x0000000000000000,
        // 0x4600000002000000, 0x0000000000000000,
        // 0x0000000000000000, 0xc800000000000000,
        // 0x4800000000000000, 0x0100000001000000,
        // 0x0800000000000000, 0x1800000000000000
        0x8500000000000001
    };
    vm_load_program(vm,program);
    vm_run(vm);

    return 1;
}