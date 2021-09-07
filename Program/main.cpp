#include <iostream>
#include "../winner.h"


//////////////////////////////////////////////////////////////////////
//#include <winnt.h>
#include <winternl.h>
PPEB pPeb;

void fetchPeb()
{
	// Thread Environment Block (TEB)
	PTEB pTeb;
#if defined _WIN64 || defined _M_X64
	pTeb = reinterpret_cast<PTEB>( __readgsqword( reinterpret_cast<DWORD_PTR>( &static_cast<NT_TIB*>( nullptr )->Self ) ) );
#else
	pTeb = reinterpret_cast<PTEB>( __readfsdword( reinterpret_cast<DWORD_PTR>( &static_cast<NT_TIB*>( nullptr )->Self ) ) );
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
	fetchPeb();
	std::cout << "address of main @"
		<< &main
		<< '\n';
	std::cout << "address of sum @"
		<< &sum
		<< '\n';
	std::cout << "exe base address @"
		<< GetModuleHandle( nullptr )
		<< '\n';
	// alternative way to get the process's handle in case it is hooked
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
