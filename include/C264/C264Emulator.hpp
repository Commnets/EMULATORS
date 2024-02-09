/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C264Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/02/2023 \n
 *	Description: The root class to define any C264 series emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C264EMULATOR__
#define __C264_C264EMULATOR__

#include <COMMODORE/incs.hpp>
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
		static const unsigned char _PARAMMACHINE;
		static const unsigned char _PARAMCONFIGURATION;

		/**
		  * Constructor:
		  * @param argv		: The parameters in the form of Strings.\n
		  *	The basic parameters for any C264 emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default. \n
		  *	/b[COLOR]		: To draw a black box around the writable part of the screen. The color is optional. \n
		  * /m[MACHINE]		: Type of machine to be emulated: C16, C116 or CP4 \n
		  * /w[CONF]		: To define the configuration of the machine when starting, the meaning will depend on the machine created. \n
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
		unsigned int borderColor () const // Black by default!. From 0 to 4 max. Above 4 = 4
							{ return (drawBorder () ? cmdlineArguments ().argumentAsInt (_PARAMBORDER) : 0); }

		/** To know which is the type of computer to be emulated: 0 = C16, 1 = C116, 2 = CPlus/4. */
		inline unsigned int emulattedComputer () const;
	
		/** To know the parameter related with the type of computer configuration. */
		bool configuration () const
							{ return (cmdlineArguments ().existsArgument (_PARAMCONFIGURATION)); }
		inline unsigned int configurationMode () const; // The meaning will depend on the type of computer emulated...

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
									  new COMMODORE::RawFileTypeIO /** Row data. */ }))); }
	};

	// ---
	inline unsigned int C264Emulator::emulattedComputer () const
	{ 
		unsigned int result = 0;
		
		if (cmdlineArguments ().existsArgument (_PARAMMACHINE))
		{
			std::string mT = 
				MCHEmul::upper (cmdlineArguments ().argumentAsString (_PARAMMACHINE));
			result = (mT == "C16") 
				? 0 
				: ((mT == "C116") 
					? 1 
					: ((mT == "CP4") 
						? 2 : 0 /** by default. */));
		}

		return (result);
	}

	// ---
	inline unsigned int C264::C264Emulator::configurationMode () const
	{ 
		unsigned int result = 
			configuration () 
				? cmdlineArguments ().argumentAsInt (_PARAMCONFIGURATION) 
				: 0;
		return ((result > 4) ? 4 : result); 
	}
}

#endif
  
// End of the file
/*@}*/
