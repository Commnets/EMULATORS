/** \ingroup COMMS */
/*@{*/

/**
 *	@file
 *	File: AnsSysCommand.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
		between CommunciationSystems. \n
		This command is linked with @see CommunicationSystem class. */
	class CommsSystemAnswerCommand final : public Command
	{
		public:
		static const int _ID = 100;
		static const std::string _NAME;
		static const std::string _PARFORMATTER;
		static const std::string _PARORIGINALCMMD;
		static const std::string _PARANSWER;

		/** Remove all chars that can be confused in the communications. */
		static std::string replaceCharsForComms (const std::string& str);
		/** Restore the chars changed before being sent. */
		static std::string restablishCharsFromComms (const std::string& str);

		CommsSystemAnswerCommand ()
			: Command (_ID, _NAME),
			_lastCommandAnswerReceived ("")
							{ }

		/** The command received 3 parameters. 
			1 of them (the second) is the command wich answer is executed. \n
			That data is kept into this attribute for further uses if needed. */
		const std::string lastCommandAnswerReceived () const
							{ return (_lastCommandAnswerReceived); }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 3 /** The type of formatter used and the answer itself. */); }

		private:
		/** This method is really executed when the message is received. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;

		private:
		std::string _lastCommandAnswerReceived;
	};
}

#endif

// End of the file
/*@}*/
