/** \ingroup C264 */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: Specific commands for a C264 series emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_COMMANDS__
#define __C264_COMMANDS__

#include <CORE/incs.hpp>

namespace C264
{
	/** To get a status of the screen memory. */
	class ScreenMemoryDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 200;
		static const std::string _NAME;

		ScreenMemoryDUMPCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get a status of the attribute memory. */
	class AttributeMemoryDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 201;
		static const std::string _NAME;

		AttributeMemoryDUMPCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get a status of the bitmap memory. */
	class BitmapMemoryDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 202;
		static const std::string _NAME;

		BitmapMemoryDUMPCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get a "draw" of the characters in memory. 
		The command is: \n
		CHARSDRAW [n1 n2 n3...] 
		Empty or any set of numbers from 0 to 255. \n
		They can be repeated, but only one instance of the same number is added. */
	class CharactersDrawCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 203;
		static const std::string _NAME;

		CharactersDrawCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (true); } // With any parameter...

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To set border and grid. \n
		The comamand is: \n
		GRIDON COLOR \n
		This method overload the one defined at CORE level because it also activates to draw
		the line where the raster interruptions are generated in the screen. */
	class GridOnCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 204;
		static const std::string _NAME;

		GridOnCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		/** The parameter is the clor of the grid. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 1 || _parameters.size () == 2); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To unset border and grid. \n
		The comamand is: \n
		GRIDOFF \n
		This one overload the one defined at CORE level because it also desactivate 
		to draw the position where the raster interrupts are generated. */
	class GridOffCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 205;
		static const std::string _NAME;

		GridOffCommand ()
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
