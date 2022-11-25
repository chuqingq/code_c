.globl _start
	.code16

_start:
	mov     %cs, %ax  
	mov     %ax, %ds  
	mov     %ax, %es  
	call    DispStr                 # 调用显示字符串例程  
1:
	jmp     1b                      # 无限循环  
DispStr:  
	mov     $BootMessage, %ax
	mov     %ax, %bp                # ES:BP = 串地址  
	mov     $16, %cx                # CX = 串长度  
	mov     $0x1301, %ax            # AH = 13, AL = 01h  
	mov     $0x000c, %bx            # 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)  
	mov     $0x00, %dl  
	int     $0x10                   # 10h 号中断  
	ret  

BootMessage:
	.ascii "Hello, OS world!"  
	.org	510                      # 填充剩下的空间，使生成的二进制代码恰好为512字节
	.word	0xaa55                   # 引导扇区标志
