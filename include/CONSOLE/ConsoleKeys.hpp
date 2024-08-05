/** \ingroup CONSOLE */
/*@{*/

/**	
 *	@file	
 *	File: ConsoleKeys.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 02/11/2022 \n
 *	Description: To manage the keys managed by the console.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CONSOLEKEYS__
#define __MCHEMUL_CONSOLEKEYS__

namespace MCHEmul
{
	/** To read the main keys managed by any console. */
	class ConsoleKeys
	{
		public:
		ConsoleKeys () = default;

		ConsoleKeys (const ConsoleKeys&) = delete;

		ConsoleKeys& operator = (const ConsoleKeys&) = delete;

		virtual ~ConsoleKeys ()
							{ }

		ConsoleKeys (ConsoleKeys&) = delete;

		ConsoleKeys& operator = (ConsoleKeys&&) = delete;

		/** Standard keys managed by any Consol.\n
			The console is always inserting characters. */
		static const char _RIGHTKEY = -1; // Cursor right
		static const char _LEFTKEY = -2; // Cursor left
		static const char _UPKEY = -3; // To recover previous instructions
		static const char _DOWNKEY = -4; // To recover next instructions
		static const char _ENTERKEY = -5; // Execute
		static const char _DELETEKEY = -6; // Del the char selected and maintains the position
		static const char _BACKKEY = -7; // Del the char before the selected position and move into that
		static const char _ENDKEY = -8; // Go to the end of the line
		static const char _BEGINKEY = -9; // Go to the beginning of the line
		static const char _ESCAPEKEY = -10; // Delete the content of the line

		/** The way a character is read from the console is different depending on the OS. \n
		The method returns true when a char is read and false in other case. \n
		The internal variable chr holds the char code read if any!. \n
		This method has to manage also the special characters. */
		virtual bool readKey (char& chr) const = 0;
	};

#ifdef _WIN32
	/** Version of class for W32 systems. */
	class Win32ConsoleKeys final : public ConsoleKeys
	{
		public:
		virtual bool readKey (char& chr) const override;
	};
#endif
}

#endif
  
// End of the file
/*@}*/
