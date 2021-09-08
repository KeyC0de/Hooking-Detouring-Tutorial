#include <iostream>
#include "../winner.h"


#pragma region optionalExtraneous1
[[noreturn]]
void directJump( DWORD ip )
{
#if defined _WIN32 || defined _MSC_VER
	__asm {
		jmp ip
	};
#elif defined __linux__
	void *ptr = (void *)ip;
	goto *ptr;
#endif
}

extern "C" [[noreturn]] void hookSumAlternative();
#pragma endregion
void hook( bool bRestoreState );

// this is our hookSum function
[[noreturn]]
void hookSum() 
{
	while ( true )
	{
		std::cout << "Instead of adding we print this!\n";
		if ( GetAsyncKeyState( VK_F10 ) & 1 )
		{
			std::cout << "Return Program.exe to previous state\n";
			hook( true );
		}
		Sleep( 3000 );
	}
}

DWORD returnAddress = 0x00A51316;	// jump back to Program@main

DWORD targetAddress = 0x00A5109b;
// Allow executable access to this dll to the memory section where the call to sum(int,int) occurs in Program.exe.
// You find this address through a reversing program say CheatEngine (or the VS debugger)
// Then you plug the address here in the variable `targetAddress`.
BYTE previousContents[5];
DWORD oldProtection;


void hook( bool bRestoreState )
{

#pragma region optionalExtraneous2
	// this is optional - I just did it to restore the memory contents
	// of Program.exe on demand
	if ( bRestoreState )
	{
		*(volatile BYTE*)( targetAddress ) = previousContents[0];
		*(volatile BYTE*)( targetAddress + 1 ) = previousContents[1];
		*(volatile BYTE*)( targetAddress + 2 ) = previousContents[2];
		*(volatile BYTE*)( targetAddress + 3 ) = previousContents[3];
		*(volatile BYTE*)( targetAddress + 4 ) = previousContents[4];

		directJump( returnAddress );	// return to Program.exe - just for fun
	}
#pragma endregion

	DWORD address = targetAddress;
	
	VirtualProtect( (void*)address,
		5,
		PAGE_EXECUTE_READWRITE,
		&oldProtection );

#pragma region optionalExtraneous3
	// safekeep previous memory contents
	previousContents[0] = *(volatile BYTE*)( address );
	previousContents[1] = *(volatile BYTE*)( address + 1 );
	previousContents[2] = *(volatile BYTE*)( address + 2 );
	previousContents[3] = *(volatile BYTE*)( address + 3 );
	previousContents[4] = *(volatile BYTE*)( address + 4 );
#pragma endregion

	*(volatile BYTE*)(address) = 0xE9;	// write the JMP opcode
	*(volatile DWORD*)(address + 1) = (DWORD)&hookSum - ( address + 5 );	// write the address to jump to 
	// jmp @hookSum

	// restore page to its former status
	VirtualProtect( (void*)targetAddress,
		5,
		oldProtection,
		nullptr );

	return;
}


BOOL APIENTRY DllMain( HANDLE hModule,
	DWORD args,
	LPVOID lpReserved )
{
	switch ( args )
	{
	case DLL_THREAD_ATTACH:
	case DLL_PROCESS_ATTACH:
		hook( false );
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		// to load/unload dlls it has to be done from the main program or through a 3rd
		// program - it can't be done from within the dll itself!
		break;
	}
	return TRUE;
}


#pragma region optionalExtraneous4
// an alternative hookSum
static char title[] = "KeyC0de Hook";
static char body[] = "My Body";

extern "C"
[[noreturn]]
__declspec(naked)
void hookSumAlternative()
{
	// during optimisation, VS moved a pointer to MessageBoxA into esi
	__asm {
		pushad
		push 0
		push title
		push body
		push 0
		call esi
		popad
		mov eax, 010B1406h
		call eax
	}
}
#pragma endregion