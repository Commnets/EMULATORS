/** \ingroup COMMS */
/*@{*/

/**
 *	@file
 *	File: AnsSysCommand.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 03/12/2022 \n
 *	Description:	Commands used to listen a communication!
 *					It is used in the consoles o similar sending commands to a remote emultation.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_ANSSYSCOMMAND__
#define __MCHEMUL_ANSSYSCOMMAND__

#include <CORE/incs.hpp>

namespace MCHEmul
{
	/** This command is used to be transmitted in the communication 
		between CommunciationSystems. */
	class CommsSystemAnswerCommand final : public Command
	{
		public:
		static const int _ID = 19;
		static const std::string _NAME;

		/** Remove all chars that can be confused in the communications. */
		static std::string replaceCharsForComms (const std::string& str);
		/** Restore the chars changed before being sent. */
		static std::string restablishCharsFromComms (const std::string& str);

		CommsSystemAnswerCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 1 /** The answer itself. */); }

		private:
		/** This method is really executed when the message is received. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
