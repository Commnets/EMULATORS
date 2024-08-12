/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: ZX81Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description: The root class to define any ZX81 emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_ZX81EMULATOR__
#define __ZX81_ZX81EMULATOR__

#include <EMULATORS/Emulator.hpp>
#include <SINCLAIR/incs.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/FileIO.hpp>
#include <ZX81/IOPBuilder.hpp>

namespace ZX81
{
	/** The ZX81 emulator admits some additional parameters. */
	class ZX81Emulator : public MCHEmul::Emulator
	{
		public:
		/** The possible additional parameters of the ZX81 Emulator. */
		static const unsigned char _PARAMNTSC;
		static const unsigned char _PARAMBORDER;
		static const unsigned char _PARAMCONFIGURATION;
		static const unsigned char _PARAMMACHINE;

		/**
		  * Constructor:
		  * @param argv		: The parameters in the form of Strings.\n
		  *	The basic parameters for any ZX81 emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default. \n
		  *	/b[COLOR]		: To draw a black box around the writable part of the screen. The color is optional. \n
		  * /w[CONF]		: To define the configuration of the machine when starting: 0 (unexpanded), 1 (3k), 2 (8k) ,3 (16k) ,4 or more (24k). \n
		  * /m[MACHINE]		: To define the type of ROM loaded: 0 (ZX80), 1 (ZX81_1 old), 2 (ZX81_2 rare), 3 (ZX81_3 newest).
		  * @param cS		: A reference to the communication system. It can be nullptr if no required.
		  */
		ZX81Emulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS = nullptr)
			: MCHEmul::Emulator (args, cS)
							{ }

		virtual void printOutParameters (std::ostream& o = std::cout) const override;

		/** To know whether the visualizacion system is or not NTSC. */
		bool NTSCSystem () const
							{ return (cmdlineArguments ().existsArgument (_PARAMNTSC)); }

		/** To know whether the border has or not to be drawn. */
		bool drawBorder () const
							{ return (cmdlineArguments ().existsArgument (_PARAMBORDER)); }
		inline unsigned int borderColor () const; // Black by default!. From 0 to 4 max. Above 4 = 4
	
		/** To know the parameter related with the type of computer configuration. */
		bool configuration () const
							{ return (cmdlineArguments ().existsArgument (_PARAMCONFIGURATION)); }
		inline unsigned int configurationMode () const; // Not expanded by default!

		/** To know which is the type of computer to be emulated: 
			0 = ZX81(1), 1 = ZX81(2), 2 = ZX81(3). */
		inline Type emulattedComputer () const;

		/** To add the peripherals linked to the computer, according to the parameters. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Computer* createComputer () const override
							{ return (new ZX81::SinclairZX81 (
								(Memory::Configuration) configurationMode (),
								NTSCSystem () 
									? ZX81::SinclairZX81::VisualSystem::_NTSC : ZX81::SinclairZX81::VisualSystem::_PAL,
								emulattedComputer ())); }
		virtual MCHEmul::IOPeripheralBuilder* createPeripheralBuilder () const override
							{ return (new IOPeripheralBuilder); }
		virtual MCHEmul::FileIO* createFileReader () const override
							{ return (new MCHEmul::FileIO 
								(MCHEmul::FileTypeIOList (
									{	new ZX81::OAndPFileTypeIO, /** O, 80, P, 81 and P81 extensions. */
										new MCHEmul::RawFileTypeIO /** Row data. */ }))); }
	};

	// ---
	inline unsigned int ZX81Emulator::borderColor () const // Black by default!. From 0 to 3 max. Above 3 = 3
	{ 
		int bC = drawBorder () 
			? cmdlineArguments ().argumentAsInt (_PARAMBORDER) : 0;

		return (bC > 3 ? 3 : bC); 
	}

	// ---
	inline unsigned int ZX81Emulator::configurationMode () const
	{ 
		unsigned int result = 
			configuration () 
				? cmdlineArguments ().argumentAsInt (_PARAMCONFIGURATION) 
				: 0; /** Not expanded. */

		// No more than 2 configurations are allowed...
		return ((result > 2) ? 2 : result); 
	}

	// ---
	inline Type ZX81Emulator::emulattedComputer () const
	{ 
		ZX81::Type result = ZX81::Type::_ZX80;
		if (cmdlineArguments ().existsArgument (_PARAMMACHINE))
		{
			std::string mT = 
				MCHEmul::upper (cmdlineArguments ().argumentAsString (_PARAMMACHINE));
			result = (mT == "ZX80") 
				? ZX81::Type::_ZX80
				: ((mT == "ZX811") 
					? ZX81::Type::_ZX811
					: ((mT == "ZX812") 
						? ZX81::Type::_ZX812 
						: ((mT == "ZX813")
							? ZX81::Type::_ZX813 
							: ZX81::Type::_ZX80 /** by default, the very basic one. */)));
		}

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
