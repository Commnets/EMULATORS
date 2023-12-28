/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: C64Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/04/2021 \n
 *	Description: The root class to define any C64 emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_C64EMULATOR__
#define __C64_C64EMULATOR__

#include <COMMODORE/incs.hpp>
#include <C64/C64.hpp>
#include <C64/IOPBuilder.hpp>

namespace C64
{
	/** The c64 emulator admits some additional parameters. */
	class C64Emulator : public MCHEmul::Emulator
	{
		public:
		/** The possible additional parameters of the C64 Emulator. */
		static const unsigned char _PARAMNTSC;
		static const unsigned char _PARAMSID;
		static const unsigned char _PARAMBORDER;

		/**
		  * Constructor:
		  * @param argv		: The parameters in the form of Strings.\n
		  *	The basic parameters for any C64 emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default. \n
		  * /w[OWN|SID]		: To indicate the libary to be used in the simulation of the sound. SID = RESID, OWN = Mine. \n
		  *	/b[COLOR]		: To draw a black box around the writable part of the screen. The color is optional. \n
		  * @param cS		: A reference to the communication system. It can be nullptr if no required.
		  */
		C64Emulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS = nullptr)
			: MCHEmul::Emulator (args, cS)
							{ }

		virtual void printOutParameters (std::ostream& o = std::cout) const override;

		/** To know whether the visualizacion system is or not NTSC. */
		bool NTSCSystem () const
							{ return (cmdlineArguments ().existsArgument (_PARAMNTSC)); }

		/** To know which is the library used for the SID simultion. */
		std::string SIDEmulationLib () const // My own (ICF) by default!
							{ return (cmdlineArguments ().existsArgument (_PARAMSID) 
								? cmdlineArguments ().argumentAsString (_PARAMSID) : "OWN" /** My own. */); }

		/** To know whether the border has or not to be drawn. */
		bool drawBorder () const
							{ return (cmdlineArguments ().existsArgument (_PARAMBORDER)); }
		unsigned int borderColor () const // Black by default!
							{ return (drawBorder () ? cmdlineArguments ().argumentAsInt (_PARAMBORDER) : 0); }

		/** To add the peripherals linked to the computer, according to the parameters. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Computer* createComputer () const override
							{ return (new C64::Commodore64 (
								NTSCSystem () 
									? C64::Commodore64::VisualSystem::_NTSC : C64::Commodore64::VisualSystem::_PAL,
								SIDEmulationLib (),
								computerLanguage ())); }
		virtual MCHEmul::IOPeripheralBuilder* createPeripheralBuilder () const override
							{ return (new IOPeripheralBuilder); }
		virtual MCHEmul::FileIO* createFileReader () const override
							{ return (new MCHEmul::FileIO 
								(MCHEmul::FileTypeIOList (
									{ new COMMODORE::CRTFileTypeIO /** Cartridges. */,
									  new COMMODORE::T64FileTypeIO /** Tapes. */,
									  new COMMODORE::TAPFileTypeIO /** Tapes. */,
									  new COMMODORE::RawFileTypeIO /** Row data. */ }))); }
	};
}

#endif
  
// End of the file
/*@}*/
