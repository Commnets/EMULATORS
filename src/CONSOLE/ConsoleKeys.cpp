#include <CONSOLE/ConsoleKeys.hpp>

#ifdef _WIN32
#include <conio.h>
// ---
bool MCHEmul::Win32ConsoleKeys::readKey (char& chr) const
{
	bool result = false;
	if (result = (_kbhit () != 0))
	{
		int k = _getch ();
		if (k == 0 || k == 0xe0)
			k += _getch ();

		switch (k)
		{
			case 299:
				chr = _LEFTKEY;
				break;

			case 301:
				chr = _RIGHTKEY;
				break;

			case 296:
				chr = _UPKEY;
				break;

			case 304:
				chr = _DOWNKEY;
				break;

			case 13:
				chr = _ENTERKEY;
				break;

			case 8:
				chr = _BACKKEY;
				break;

			case 307:
				chr = _DELETEKEY;
				break;

			case 295:
				chr = _BEGINKEY;
				break;

			case 303:
				chr = _ENDKEY;
				break;

			default:
				chr = static_cast <char> (k);
		}
	}

	return (result); 
}
#endif
