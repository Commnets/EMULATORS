/** \ingroup C64 */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/10/2022 \n
 *	Description: Specific Commands for a C64 emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_COMMANDS__
#define __C64_COMMANDS__

#include <CORE/incs.hpp>

namespace C64
{
	/** To get the status of the CIA1 chip. */
	class CIA1StatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 200;
		static const std::string _NAME;

		CIA1StatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the CIA2 chip. */
	class CIA2StatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 201;
		static const std::string _NAME;

		CIA2StatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the PLA chip. */
	class PLAStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 202;
		static const std::string _NAME;

		PLAStatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get a status of the screen memory. */
	class ScreenMemoryDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 203;
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

	/** To get a status of the color memory. */
	class ColorMemoryDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 204;
		static const std::string _NAME;

		ColorMemoryDUMPCommand ()
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
		static const int _ID = 205;
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

	/** To get a status of the sprites memory. 
		The command is: \n
		SPRITESDUMP [n1 n2 n3...] 
		Empty or any set of numbers between 1 and 8. \n
		They can be repeated, but only one instance of the same number is added. */
	class SpritesMemoryDUMPCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 206;
		static const std::string _NAME;

		SpritesMemoryDUMPCommand ()
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
		Empty or any set of numbers from 1 to 8. \n
		They can be repeated, but only one instance of the same number is added. */
	class SpritesDrawCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 207;
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

	/** To get a "draw" of the characters in memory. 
		The command is: \n
		CHARSDRAW [n1 n2 n3...] 
		Empty or any set of numbers from 0 to 255. \n
		They can be repeated, but only one instance of the same number is added. */
	class CharactersDrawCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 208;
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
		static const int _ID = 209;
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
		static const int _ID = 210;
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

	/** Command to active or disactivate the Joysticks 
		"connecting" paddles (that could also act as potenciomaters in the POTX and POTY variables of the SID) instead. 
		The command is: \n
		PADDLE ON|OFF [PORTID1, PORTID2...] \n
		When there is no paddle id the ON|OFF will be applicable to all. 
		PortId is the game port number where the paddle should be plugged in (0 or 1). */
	class ManagePaddlesCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 211;
		static const std::string _NAME;

		ManagePaddlesCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () >= 1); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
