#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"

int main()
{
	//Get procId of target process
	DWORD procId = GetProcId(L"ac_client.exe");

	//Getmodulebaseaddress
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

	//Get handle to process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	
	//resolve base address of pointer chain
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10F4F4;


	std::cout << "DynamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;

	//resolve our ammo pointer chain
	std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 };
	std::vector<unsigned int> healthOffset = { 0xF8 };
	uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);
	uintptr_t healthAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, healthOffset);

	std::cout << "AmmoAddr = " << "0x" << std::hex << ammoAddr << std::endl;
	//read ammo value
	int ammoValue = 0;
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	
	std::cout << "Current Ammo = " << std::dec << ammoValue << std::endl;

	//write to it
	int newAmmo = 666;
	int newHp = 666;
	while (true)
	{
		WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);
		WriteProcessMemory(hProcess, (BYTE*)healthAddr, &newHp, sizeof(newHp), nullptr);
	}
	
	
	//read out again
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	
	std::cout << "New Ammo = " << std::dec << ammoValue << std::endl;

	char a = getchar();

	return 0;
}