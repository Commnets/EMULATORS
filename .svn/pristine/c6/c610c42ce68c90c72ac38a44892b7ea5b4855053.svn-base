/** \ingroup EMULS */
/*@{*/

/**	
 *	@file	
 *	File: Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/04/2021 \n
 *	Description: The root class all emulators inherits from.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_EMULATOR__
#define __MCHEMUL_EMULATOR__

#include <CORE/incs.hpp>
#include <ASSEMBLER/incs.hpp>
#include <COMMS/System.hpp>
#include <COMMS/StdMessages.hpp>

namespace Emuls
{
	class Emulator 
	{
		public:
		/** The possible parameters of the Emulator. */
		static const unsigned char _PARAMBYTEFILE;
		static const std::string _BYTEFILE;
		static const unsigned char _PARAMASMFILE;
		static const std::string _ASMFILE;
		static const unsigned char _PARAMLOGLEVEL;
		static const std::string _LOGLEVEL;
		static const unsigned char _PARAMADDRESS;
		static const std::string _ADDRESS;

		/**
		  * Constructor:
		  * @param argc		: Número de parámetros.
		  * @param argv		: The parameters.
		  * The basic parameters for any emulator are: \n
		  * /fFILENAME		: bytes file (with path) to be loaded into the memory.
		  * /cFILENAME		: ASM file (with path) to be parsed, compiled and loadd into the memory.
		  * /lLEVEL			: To print out logs.
		  * /aADDRESS		: The address where to start the execution of the emulator.
		  */
		Emulator (const std::vector <std::string>& argv);

		virtual ~Emulator ();

		const MCHEmul::Attributes& attributes () const
							{ return (_attributes); }

		/** Not possible to change when running. */
		void setCommunicationSystem (MCHEmul::CommunicationSystem* cS);

		/** To know whether there is a byte file where data to be loaded. \n
			"" when there is no byte file data. */
		std::string byteFileName () const
							{ MCHEmul::Attributes::const_iterator i; 
								return (((i = _attributes.find (_BYTEFILE)) != _attributes.end ()) ? (*i).second : ""); }

		/** To know whether there is an ASM file where data to be loaded. 
			"" when there is no ASM file data. */
		std::string asmFileName () const
							{ MCHEmul::Attributes::const_iterator i; 
								return (((i = _attributes.find (_ASMFILE)) != _attributes.end ()) ? (*i).second : ""); }

		/** To know the level of the log. 
			0 means no log. */
		unsigned int logLevel () const
							{ MCHEmul::Attributes::const_iterator i; 
								return ((unsigned int) std::stoi (((i = _attributes.find (_LOGLEVEL)) != _attributes.end ()) 
									? (*i).second : "")); }

		/** To know the address where to start the execution from. \n
			0x00 when no address has been defined. */
		MCHEmul::Address startingAddress () const
							{ MCHEmul::Attributes::const_iterator i; 
								return (MCHEmul::Address::fromStr (((i = _attributes.find (_ADDRESS)) != _attributes.end ()) 
									? (*i).second : "")); }

		unsigned int debugLevel () const
							{ return (_debugLevel); }
		void setDebugLevel (unsigned int dL)
							{ _debugLevel = dL; computer () -> setDebugLevel (dL); }

		/** To initialize the emulator. */
		bool initialize ();

		/** True when finishes ok, false when no ok. */
		bool run ();

		/** Execute just onee cycle. */
		bool runCycle ();

		protected:
		const MCHEmul::Computer* computer () const
							{ return (_computer == nullptr) ? (_computer = createComputer ()) : _computer; }
		MCHEmul::Computer* computer ()
							{ return (_computer == nullptr) ? (_computer = createComputer ()) : _computer; }

		// Implementation
		/** To create the right version of the computer, 
			attending the parameters received by the constructor. */
		virtual MCHEmul::Computer* createComputer () const = 0;

		protected:
		/** Defined in the constructor. */
		MCHEmul::Attributes _attributes;
		MCHEmul::CommunicationSystem* _communicationSystem;
		unsigned int _debugLevel;

		// Implementation
		mutable MCHEmul::Computer* _computer;
		mutable bool _running;
	};
}

#endif
  
// End of the file
/*@}*/
#pragma once
