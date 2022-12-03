/** \ingroup CONSOLE */
/*@{*/

/**	
*	@file	
*	File: RemoteConsole.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Forniés (EMULATORS library) \n
*	Creation Date: 02/11/2022 \n
*	Description: A console that connects remotly against a emulation.
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_REMOTECONSOLE__
#define __MCHEMUL_REMOTECONSOLE__

#include <COMMS/incs.hpp>
#include <CONSOLE/Console.hpp>

namespace MCHEmul
{
	/** The remote console manages a nill emulation behind
		and just a simple command builder to interprete the answer commind from the real emulation. */
	class RemoteConsole : public Console
	{
		public:
		/**
		* The constructor reveives: \n
		* @param cS	The communication system used to listen the messages. \n
		* @param to	The address where the Emulation is running.
		* And also the parameters that the parent class needs. \n
		* The CommandBuilder needed must be the same that the one used by the CommunicationSystem at destination. */
		RemoteConsole (CommandBuilder* cB, ConsoleKeys* k /** will depend on the OS. */,
			CommunicationSystem* cS, const IPAddress& to, 
			const std::string& cF = "./console.def", std::ostream & oS = std::cout, size_t cK = 100)
			: Console (cB, k, cF, oS, cK),
			  _communicationSystem (cS), _to (to)
							{ assert (_communicationSystem != nullptr); }

		virtual void run () override;

		protected:
		virtual bool runPerCycle () override;

		/** Send the command to the remote emulator. */
		virtual void createAndExecuteCommand () override;

		/** To determine whether the command has to be sent across or executed here. */
		virtual bool localCommand (Command* cmd)
							{ return (dynamic_cast <HelpCommand*> (cmd) != nullptr || 
									  dynamic_cast <AuthorInfoCommand*> (cmd) != nullptr); }

		protected:
		CommunicationSystem* _communicationSystem;
		IPAddress _to;
	};
}

#endif

// End of the file
/*@}*/
