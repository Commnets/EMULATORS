/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C264Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2023 \n
 *	Description: The root class to define any C264 series emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C264EMULATOR__
#define __C264_C264EMULATOR__

#include <EMULATORS/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <C264/Type.hpp>
#include <C264/C264.hpp>
#include <C264/IOPBuilder.hpp>

namespace C264
{
	/** The C264 emulator admits some additional parameters,
		one oif the most important is the type of machine to be emulated, 
		because it can be a C16, a CPlus/4 or a C116 machine, which are different
		is several aspects like the number of ports the RAM at the beginning or the ROM loaded. */
	class C264Emulator : public MCHEmul::Emulator
	{
		public:
		/** The possible additional parameters of the C264 Emulator. */
		static const unsigned char _PARAMNTSC;
		static const unsigned char _PARAMBORDER;
		static const unsigned char _PARAMCONFIGURATION;
		static const unsigned char _PARAMMACHINE;

		/**
		  * Constructor:
		  * @param argv		: The parameters in the form of Strings.\n
		  *	The basic parameters for any C264 emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default. \n
		  *	/b[COLOR]		: To draw a black box around the writable part of the screen. The color is optional. \n
		  * /m[MACHINE]		: Type of machine to be emulated: C16, C116 or CP4 \n
		  * @param cS		: A reference to the communication system. It can be nullptr if no required.
		  */
		C264Emulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS = nullptr)
			: MCHEmul::Emulator (args, cS)
							{ }

		virtual void printOutParameters (std::ostream& o = std::cout) const override;

		/** To know whether the visualizacion system is or not NTSC. */
		bool NTSCSystem () const
							{ return (cmdlineArguments ().existsArgument (_PARAMNTSC)); }

		/** To know whether the border has or not to be drawn. */
		bool drawBorder () const
							{ return (cmdlineArguments ().existsArgument (_PARAMBORDER)); }
		unsigned int borderColor () const // Black by default!...
							{ return (drawBorder () ? cmdlineArguments ().argumentAsInt (_PARAMBORDER) : 0); }

		/** To know the parameter related with the type of computer configuration. */
		bool configuration () const
							{ return (cmdlineArguments ().existsArgument (_PARAMCONFIGURATION)); }
		inline unsigned int configurationMode () const; // Not expanded by default!

		/** To know which is the type of computer to be emulated: 
			0 = C16, 1 = C116, 2 = CPlus/4. */
		inline Type emulattedComputer () const;

		/** To add the peripherals linked to the computer, according to the parameters. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Computer* createComputer () const override;
		virtual MCHEmul::IOPeripheralBuilder* createPeripheralBuilder () const override
							{ return (new IOPeripheralBuilder); }
		virtual MCHEmul::FileIO* createFileReader () const override
							{ return (new MCHEmul::FileIO 
								(MCHEmul::FileTypeIOList (
									{ new COMMODORE::CRTFileTypeIO /** Cartridges. */,
									  new COMMODORE::TAPFileTypeIO /** Tapes. */,
									  new COMMODORE::PRGFileTypeIO /** Very simple PRG files. */,
									  new COMMODORE::KeystrokeTypeIO /** Keystroke data. */,
									  new MCHEmul::RawFileTypeIO /** Row data. */ }))); }
	};

	// ---
	inline unsigned int C264Emulator::configurationMode () const
	{ 
		unsigned int result = 0; // The basic by default...

		// The configuration mode allowed will depend on the type of computer emulatted!
		if (configuration ())
		{
			unsigned int cfg = cmdlineArguments ().argumentAsInt (_PARAMCONFIGURATION);
			if (emulattedComputer () == Type::_C16 || emulattedComputer () == Type::_C116)
				// C16/116 only 0 or 1 allowed...
				result = (cfg > 1 ? 0 : cfg); // Either 16k or 32k...
			else // Type CPlus/4
				result = 2; // Always 64k....
		}

		return (result);
	}

	// ---
	inline Type C264Emulator::emulattedComputer () const
	{ 
		Type result = Type::_C16;
		
		if (cmdlineArguments ().existsArgument (_PARAMMACHINE))
		{
			std::string mT = 
				MCHEmul::upper (cmdlineArguments ().argumentAsString (_PARAMMACHINE));
			result = (mT == "C16") 
				? Type::_C16
				: ((mT == "C116") 
					? Type::_C116 // Notice that it is the same than C16...
					: ((mT == "CP4") 
						? Type::_CPLUS4 : Type::_C16 /** The one by default. */));
		}

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
