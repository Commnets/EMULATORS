/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: ConsoleCommand.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/08/2022 \n
 *	Description: Representing any command that can be introduced through the console.
 *	Versions: 1.0 Initial
 */

#ifndef __CONSOLE_COMMANDCONSOLE__
#define __CONSOLE_COMMANDCONSOLE__

#include <CORE/incs.hpp>

namespace MCHEmul
{
	class Command 
	{
		public:
		Command () = delete;

		Command (int id, const Attributes& prms = { })
			: _id (id), _parameters (prms)
							{ }

		Command (const Command&) = default;

		Command& operator = (const Command&) = default;

		virtual ~Command ()
							{ }

		int id () const
							{ return (_id); }
		const Attributes& parameters () const
							{ return (_parameters); }
		void setParameters (const Attributes& prms)
							{ _parameters = prms; }

		virtual bool canBeExecuted () const = 0;

		virtual bool execute (Computer* c, const Attributes& rst) = 0;

		protected:
		int _id;
		Attributes _parameters;
	};
}

#endif
  
// End of the file
/*@}*/
