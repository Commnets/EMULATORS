#include "stdafx.h"

#include <iostream>

#include <EMULATORS/incs.hpp>

#ifndef _CONSOLE
std::vector <std::string> generateParamsFrom (int argc, char *argv [])
{
	std::vector <std::string> result;
	for (int i = 0; i < argc; i++)
		result.push_back (argv [i]);
	return (result);
}
#else
std::vector <std::string> generateParamsFrom (int argc, _TCHAR *argv [])
{
	std::vector <std::string> result;
	for (int i = 0; i < argc; i++)
		{ std::wstring wS = argv [i]; result.push_back (std::string (wS.begin (), wS.end ())); }
	return (result);
}
#endif

using namespace Emuls;

#ifndef _CONSOLE
#include <SDL.h>
#ifdef __cplusplus
#define C_LINKAGE "C"
#else
#define C_LINKAGE
#endif /* __cplusplus */
#if _MSC_VER >= 1900
extern C_LINKAGE FILE __iob_func[3] = { *stdin,*stdout,*stderr };
#endif
extern C_LINKAGE int main(int argc, char *argv[])
#else
int _tmain (int argc, _TCHAR *argv [])
#endif /* _CONSOLE */
{
	return (C64Emulator (generateParamsFrom (argc, argv)).run ());
}
