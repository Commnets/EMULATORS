/** \ingroup MSX */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2024 \n
 *	Description: Specific Commands for a MSX emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_COMMANDS__
#define __MSX_COMMANDS__

#include <CORE/incs.hpp>
#include <SINCLAIR/incs.hpp>

namespace MSX
{
	/** To get the status of the VDP chip. */
	class VDPStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 100;
		static const std::string _NAME;
	
		VDPStatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get a status of the pattern name table. */
	class PatternNameTableDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 101;
		static const std::string _NAME;

		PatternNameTableDUMPCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get a status of the Pattern Generation Table. */
	class PatternGenerationTableDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 102;
		static const std::string _NAME;

		PatternGenerationTableDUMPCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the Color Name Table. */
	class ColorNameTableDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 103;
		static const std::string _NAME;

		ColorNameTableDUMPCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get a "draw" of the sprites in memory. 
		The command is: \n
		SPRITESDRAW [n1 n2 n3...] 
		Empty or any set of numbers from 1 to 32. \n
		They can be repeated, but only one instance of the same number is added. */
	class SpritesDrawCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 104;
		static const std::string _NAME;

		SpritesDrawCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (true); } // With any parameter...

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

}

#endif

// End of the file
/*@}*/
