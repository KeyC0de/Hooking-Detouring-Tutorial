#include <iostream>
#include "../winner.h"


#if defined _WIN64 || defined __x86_64__ || defined __ppc64__
using TWORD = UINT64;
#elif defined _WIN32
using TWORD = DWORD;
#endif

#pragma region optionalExtraneous1
[[noreturn]]
void directJump( TWORD ip )
{
	__asm {
		jmp ip
	};
}
#pragma endregion

void hookedSum();

//TWORD returnAddress = 0x001B1010;	// jump back to Program@main
// Allow executable access to this dll to the memory section where the call to sum(int,int) occurs in Program.exe.
// You find this address through a debugger say CheatEngine (or the VS debugger)
// Then you assign the address here in the variable `targetAddress`.
// [note that it should be a Relative Virtual Address (RVA)]
// TWORD targetAddress = 0x001B10E5;

void hook( TWORD targetAddress = 0,
	TWORD returnAddress = 0 )
{
#pragma region optionalExtraneous2
	static BYTE previousContents[5];
	TWORD oldProtection;
	// this is optional - I just did it to restore the memory contents
	// of Program.exe on demand
	if ( targetAddress == 0 && returnAddress != 0 )
	{
		*(volatile BYTE*)( targetAddress ) = previousContents[0];
		*(volatile BYTE*)( targetAddress + 1 ) = previousContents[1];
		*(volatile BYTE*)( targetAddress + 2 ) = previousContents[2];
		*(volatile BYTE*)( targetAddress + 3 ) = previousContents[3];
		*(volatile BYTE*)( targetAddress + 4 ) = previousContents[4];

		directJump( returnAddress );	// return to Program.exe - just for fun
	}
#pragma endregion

	VirtualProtect( (void*)targetAddress,
		5,
		PAGE_EXECUTE_READWRITE,
		&oldProtection );

#pragma region optionalExtraneous3
	// safekeep previous memory contents
	previousContents[0] = *(volatile BYTE*)( targetAddress );
	previousContents[1] = *(volatile BYTE*)( targetAddress + 1 );
	previousContents[2] = *(volatile BYTE*)( targetAddress + 2 );
	previousContents[3] = *(volatile BYTE*)( targetAddress + 3 );
	previousContents[4] = *(volatile BYTE*)( targetAddress + 4 );
#pragma endregion

	*(volatile BYTE*)(targetAddress) = 0xE9;	// write the JMP opcode
	*(volatile TWORD*)(targetAddress + 1) = (TWORD)&hookedSum - ( targetAddress + 5 );	// write the RVA to jump to
	// jmp @hookedSum

	// restore page to its former status
	VirtualProtect( (void*)targetAddress,
		5,
		oldProtection,
		nullptr );
}

[[noreturn]]
void hookedSum()
{
	while ( true )
	{
		std::cout << "Instead of adding we print this!\n";
		if ( GetAsyncKeyState( VK_F10 ) & 1 )
		{
			std::cout << "Return Program.exe to previous state\n";
			hook( 0x001B10E5,
				0x001B1010 );
		}
		Sleep( 3000 );
	}
}


int WINAPI DllMain( HINSTANCE hDll,
	DWORD ulReasonForcall,
	LPVOID pReserved )
{
	switch ( ulReasonForcall )
	{
	case DLL_PROCESS_ATTACH:
	{
		hook( 0x001B10E5 );
		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
	{
		// dll cleanup must be done from code that loaded the dll,
		//	as if by FreeLibraryAndExitThread( hDll, 0u );
		// If the dll itself called LoadLibrary then it can unload itself like so:
		// GetModuleHandleExW( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		// 	(LPCTSTR)DllMain,
		// 	&hDll );
		// FreeLibraryAndExitThread( hDll,
		// 	0u );
	}
	default:
		// Do any cleanup..
		break;
	}
	return TRUE;
}


#pragma region optionalExtraneous4
// an alternative hookedSum function
#if defined _WIN64 || defined __x86_64__ || defined __ppc64__
//
#elif defined _WIN32
static char title[] = "KeyC0de Hook";
static char body[] = "My Body";

extern "C"
[[noreturn]]
__declspec(naked)
void hookedSumAlternative()
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
		mov eax, 001B1406h
		call eax
	}
}
#endif
#pragma endregion