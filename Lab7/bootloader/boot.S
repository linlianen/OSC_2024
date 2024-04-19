.section ".text.boot"

.global _start

_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f

1:
    wfe
    b 1b

2:

    ldr x1, =_bss_start
    ldr w2, =_bss_size
3:
    cbz w2, 4f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b

    // DTB load address is kept in x0 by CPU, save it in x28
    mov x28, x0

    //set top of stack to exec address
    ldr x1, =0x60000
    mov sp, x1
    bl self_relocate

// jump to reloacated main()
    mov x0, x28
4:
    bl (boot_main-0x80000+0x60000)
    b 1b

self_relocate:

    /*
    void self_relocate(){
        const u64 size = 0x10000 >> 3;
        u64 *load_addr = (u64*) 0x80000;
        u64 *exec_addr = (u64*) 0x60000;
        for(u64 i=0; i<size; i++)
            *exec_addr++ = *load_addr++;
        return;
    }
     */
    sub sp, sp, #32
    mov x0, (0x10000>>3) // size -> [0x10000 >> 3]
    str x0, [sp]
    mov x0, 0x80000
    str x0, [sp, 24]
    mov x0, 0x60000
    str x0, [sp, 16]
    str xzr, [sp, 8] //xzr=0
.L3:
    ldr x1, [sp, 8]         //
    mov x0, (0x10000>>3-1)  // i < size
    cmp x1, x0              //
    bgt .L5
    ldr x1, [sp, 24]
    add x0, x1, 8        
    str x0, [sp, 24]   // x1 = load_addr++
    ldr x0, [sp, 16]
    add x2, x0, 8
    str x2, [sp, 16]  // x2 = exec_addr++
    ldr x1, [x1]    
    str x1, [x0]    // x2 = x1
    ldr x0, [sp, 8]
    add x0, x0, 1       // i = i +1
    str x0, [sp, 8]
    b .L3
.L5:
    NOP
    add sp, sp, 32
    ret
