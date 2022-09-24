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

		/** True when finishes ok, false when no ok. */
		bool run ();

		protected:
		const MCHEmul::Computer* computer () const
							{ return (_computer == nullptr) ? (_computer = createComputer ()) : _computer; }
		MCHEmul::Computer* computer ()
							{ return (_computer == nullptr) ? (_computer = createComputer ()) : _computer; }

		const MCHEmul::CommunicationSystem* communicationSystem () const
							{ return (_communicationSystem == nullptr) 
								? (_communicationSystem = createCommunicationSystem ()) : _communicationSystem; }
		MCHEmul::CommunicationSystem* communicationSystem ()
							{ return (_communicationSystem == nullptr) 
								? (_communicationSystem = createCommunicationSystem ()) : _communicationSystem; }

		// Implementation
		/** To create the right version of the computer, 
			attending the parameters received by the constructor. */
		virtual MCHEmul::Computer* createComputer () const = 0;
		/** To create the communication system. */
		virtual MCHEmul::CommunicationSystem* createCommunicationSystem () const
							{ return (new MCHEmul::CommunicationSystem (new MCHEmul::StandardMessageBuilder ())); }

		protected:
		/** Defined in the constructor. */
		MCHEmul::Attributes _attributes;

		// Implementation
		mutable MCHEmul::Computer* _computer; // Can be create in a const method...
		mutable MCHEmul::CommunicationSystem* _communicationSystem; // Created when running!
	};
}

#endif
  
// End of the file
/*@}*/
#pragma once
