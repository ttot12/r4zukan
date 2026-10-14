@ R4 黑白图鉴菜单 · ARM9 最小启动 (原创)
@ 经典 devkitARM 布局：ARM9 入口 0x02000000，无安全区，老 R4 内核/DeSmuME 可加载
    .section .text
    .align 4
    .arm
    .global _start
_start:
    ldr  sp, =0x0237E000     @ ARM9 栈顶（在 EWRAM 高段、ARM7 区 0x02380000 之下）
    bl   main
1:  b    1b                   @ main 不应返回；若返回则死循环

    .end
