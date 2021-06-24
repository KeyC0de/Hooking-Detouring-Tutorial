#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//////////////////////////////////////////////////////////////////////
//#include <winnt.h>
#include <winternl.h>
PPEB pPeb;

void getProcessEnvironmentBlock()
{
	// Thread Environment Block (TEB)
	PTEB pTeb;
#if defined(_M_X64) // x64
	pTeb = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
#else // x86
	pTeb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
#endif

	// Process Environment Block (PEB)
	pPeb = pTeb->ProcessEnvironmentBlock;
}
//////////////////////////////////////////////////////////////////////

int sum( int x,
	int y )
{
	return x + y;
}


int main()
{
	getProcessEnvironmentBlock();
	std::cout << "address of sum @" << &sum << '\n';
	std::cout << "exe base address @" << GetModuleHandle( nullptr ) << '\n';
	// alternative way to get the process's handle in case it is hooked (useful in game hacking etc):
	const auto peb = reinterpret_cast<PPEB>( __readfsdword(0x30) );
	const auto pBase = peb->Reserved3[1];
	std::cout << "alternative exe base address @"
		<< pBase
		<< '\n';
	// What is the "FS" & "GS" registers?
	// They are used to access data beyond the default data segment DS, just like ES.

	while( true )
	{
		std::cout << "Program.exe test call: sum( 5, 5 ) = "
			<< sum( 5, 5 )
			<< '\n';
		Sleep( 3000 );
	}

	return 0;
}
