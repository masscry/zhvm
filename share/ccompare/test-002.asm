
fib.o:     формат файла elf64-x86-64


Дизассемблирование раздела .text:

0000000000000000 <_Z3fibi>:
   0:	55                   	push   %rbp
   1:	53                   	push   %rbx
   2:	48 83 ec 08          	sub    $0x8,%rsp
   6:	85 ff                	test   %edi,%edi
   8:	74 3a                	je     44 <_Z3fibi+0x44>
   a:	83 ff 01             	cmp    $0x1,%edi
   d:	89 fb                	mov    %edi,%ebx
   f:	74 37                	je     48 <_Z3fibi+0x48>
  11:	31 ed                	xor    %ebp,%ebp
  13:	eb 08                	jmp    1d <_Z3fibi+0x1d>
  15:	0f 1f 00             	nopl   (%rax)
  18:	83 fb 01             	cmp    $0x1,%ebx
  1b:	74 1b                	je     38 <_Z3fibi+0x38>
  1d:	8d 7b ff             	lea    -0x1(%rbx),%edi
  20:	e8 00 00 00 00       	callq  25 <_Z3fibi+0x25>
  25:	01 c5                	add    %eax,%ebp
  27:	83 eb 02             	sub    $0x2,%ebx
  2a:	75 ec                	jne    18 <_Z3fibi+0x18>
  2c:	48 83 c4 08          	add    $0x8,%rsp
  30:	89 e8                	mov    %ebp,%eax
  32:	5b                   	pop    %rbx
  33:	5d                   	pop    %rbp
  34:	c3                   	retq   
  35:	0f 1f 00             	nopl   (%rax)
  38:	83 c5 01             	add    $0x1,%ebp
  3b:	48 83 c4 08          	add    $0x8,%rsp
  3f:	89 e8                	mov    %ebp,%eax
  41:	5b                   	pop    %rbx
  42:	5d                   	pop    %rbp
  43:	c3                   	retq   
  44:	31 ed                	xor    %ebp,%ebp
  46:	eb e4                	jmp    2c <_Z3fibi+0x2c>
  48:	bd 01 00 00 00       	mov    $0x1,%ebp
  4d:	eb dd                	jmp    2c <_Z3fibi+0x2c>
