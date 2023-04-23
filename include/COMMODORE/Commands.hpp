/** \ingroup COMMODORE */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: Specific Commands for a COMMODORE environment.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_COMMANDS__
#define __COMMODORE_COMMANDS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** To get the status of the VIC chip. */
	class VICStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 100;
		static const std::string _NAME;
	
		VICStatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the CIA chip. */
	class CIAStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 101;
		static const std::string _NAME;

		CIAStatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the SID chip. */
	class SIDStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 102;
		static const std::string _NAME;

		SIDStatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To change the sound wrapper used. */
	class SIDWrapperCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 103;
		static const std::string _NAME;

		SIDWrapperCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () != 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the Datasette. */
	class DatasetteStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 104;
		static const std::string _NAME;

		DatasetteStatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
