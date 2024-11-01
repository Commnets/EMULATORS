/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: ZX81Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The root class to define any ZXSpectrum emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_ZXSPECTRUMEMULATOR__
#define __ZXSPECTRUM_ZXSPECTRUMEMULATOR__

#include <EMULATORS/Emulator.hpp>
#include <SINCLAIR/incs.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/FileIO.hpp>
#include <ZXSpectrum/IOPBuilder.hpp>

namespace ZXSPECTRUM
{
	/** The ZXSpectrum emulator admits some additional parameters. */
	class ZXSpectrumEmulator : public MCHEmul::Emulator
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
		  *	The basic parameters for any ZXSpectrum emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default. \n
		  *	/b[COLOR]		: To draw a black box around the writable part of the screen. The color is optional. \n
		  * /w[CONF]		: To define the configuration of the machine when starting: // TODO \n
		  * /m[MACHINE]		: To define the type of ROM loaded: // TODO.
		  * @param cS		: A reference to the communication system. It can be nullptr if no required.
		  */
		ZXSpectrumEmulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS = nullptr)
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
							{ return (new ZXSPECTRUM::SinclairZXSpectrum (
								(Memory::Configuration) configurationMode (),
								NTSCSystem () 
									? ZXSPECTRUM::SinclairZXSpectrum::VisualSystem::_NTSC 
									: ZXSPECTRUM::SinclairZXSpectrum::VisualSystem::_PAL,
								emulattedComputer (),
								computerLanguage ())); }
		virtual MCHEmul::IOPeripheralBuilder* createPeripheralBuilder () const override
							{ return (new IOPeripheralBuilder); }
		virtual MCHEmul::FileIO* createFileReader () const override
							{ return (new MCHEmul::FileIO 
								(MCHEmul::FileTypeIOList (
									{	// TODO
										new MCHEmul::RawFileTypeIO /** Row data. */ }))); }
	};

	// ---
	inline unsigned int ZXSpectrumEmulator::borderColor () const // Black by default!...
	{ 
		int bC = drawBorder () 
			? cmdlineArguments ().argumentAsInt (_PARAMBORDER) : 0;

		return (bC > 3 ? 3 : bC); 
	}

	// ---
	inline unsigned int ZXSpectrumEmulator::configurationMode () const
	{ 
		unsigned int result = 
			configuration () 
				? cmdlineArguments ().argumentAsInt (_PARAMCONFIGURATION) 
				: 0; /** Not expanded. */

		switch (emulattedComputer ())
		{
			case ZXSPECTRUM::Type::_STANDARD:
				if (result > 2) result = 2; // No more than 2 configurations are allowed: 16k/48k...
				break;

			default:
				{
					// It shouldn't be here...
					_LOG ("Model not emulated:" + 
						std::to_string ((int) emulattedComputer ()));
					assert (false); // For the debug mode...
				}

				break;
		}

		return (result);
	}

	// ---
	inline Type ZXSpectrumEmulator::emulattedComputer () const
	{ 
		ZXSPECTRUM::Type result = ZXSPECTRUM::Type::_STANDARD;
		if (cmdlineArguments ().existsArgument (_PARAMMACHINE))
		{
			std::string mT = 
				MCHEmul::upper (cmdlineArguments ().argumentAsString (_PARAMMACHINE));
			result = (mT == "ZXSTD") 
				? ZXSPECTRUM::Type::_STANDARD
				: ZXSPECTRUM::Type::_STANDARD /** The very basic one. */;
		}

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
