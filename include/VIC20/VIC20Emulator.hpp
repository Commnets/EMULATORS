/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIC20Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: The root class to define any VIC20 emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIC20EMULATOR__
#define __VIC20_VIC20EMULATOR__

#include <COMMODORE/incs.hpp>
#include <VIC20/VIC20.hpp>
#include <VIC20/IOPBuilder.hpp>

namespace VIC20
{
	/** The VIC20 emulator admits some additional parameters. */
	class VIC20Emulator : public MCHEmul::Emulator
	{
		public:
		/** The possible additional parameters of the VIC20 Emulator. */
		static const unsigned char _PARAMNTSC;
		static const unsigned char _PARAMBORDER;
		static const unsigned char _PARAMCONFIGURATION;

		/**
		  * Constructor:
		  * @param argv		: The parameters in the form of Strings.\n
		  *	The basic parameters for any VIC20 emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default. \n
		  *	/b[COLOR]		: To draw a black box around the writable part of the screen. The color is optional. \n
		  * /w[CONF]		: To define the configuration of the machine when starting. 0 (unexpanded), 1 (3k), 2 (8k) ,3 (16k) ,4 or more (24k). \n
		  * @param cS		: A reference to the communication system. It can be nullptr if no required.
		  */
		VIC20Emulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS = nullptr)
			: MCHEmul::Emulator (args, cS)
							{ }

		virtual void printOutParameters (std::ostream& o = std::cout) const override;

		/** To know whether the visualizacion system is or not NTSC. */
		bool NTSCSystem () const
							{ return (cmdlineArguments ().existsArgument (_PARAMNTSC)); }

		/** To know whether the border has or not to be drawn. */
		bool drawBorder () const
							{ return (cmdlineArguments ().existsArgument (_PARAMBORDER)); }
		unsigned int borderColor () const // Black by default!. From 0 to 4 max. Above 4 = 4
							{ return (drawBorder () ? cmdlineArguments ().argumentAsInt (_PARAMBORDER) : 0); }
	
		/** To know the parameter related with the type of computer configuration. */
		bool configuration () const
							{ return (cmdlineArguments ().existsArgument (_PARAMCONFIGURATION)); }
		inline unsigned int configurationMode () const; // Not expanded by default!

		/** To add the peripherals linked to the computer, according to the parameters. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Computer* createComputer () const override
							{ return (new VIC20::CommodoreVIC20 (
								(Memory::Configuration) configurationMode (),
								NTSCSystem () 
									? VIC20::CommodoreVIC20::VisualSystem::_NTSC : VIC20::CommodoreVIC20::VisualSystem::_PAL,
								computerLanguage ())); }
		virtual MCHEmul::IOPeripheralBuilder* createPeripheralBuilder () const override
							{ return (new IOPeripheralBuilder); }
		virtual MCHEmul::FileIO* createFileReader () const override
							{ return (new MCHEmul::FileIO 
								(MCHEmul::FileTypeIOList (
									{ new COMMODORE::CRTFileTypeIO /** Cartridges. */,
									  new COMMODORE::TAPFileTypeIO /** Tapes. */,
									  new COMMODORE::RawFileTypeIO /** Row data. */ }))); }
	};

	// ---
	inline unsigned int VIC20::VIC20Emulator::configurationMode () const
	{ 
		unsigned int result = 
			configuration () 
				? cmdlineArguments ().argumentAsInt (_PARAMCONFIGURATION) 
				: 0; /** Not expanded. */
		return ((result > 4) ? 4 : result); 
	}
}

#endif
  
// End of the file
/*@}*/
