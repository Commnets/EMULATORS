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

namespace MCHEmul
{
	class Emulator 
	{
		public:
		/** The possible parameters of the Emulator. */
		static const unsigned char _PARAMHELP;
		static const unsigned char _PARAMBYTEFILE;
		static const unsigned char _PARAMBLOCKFILE;
		static const unsigned char _PARAMASMFILE;
		static const unsigned char _PARAMLOGLEVEL;
		static const unsigned char _PARAMADDRESS;
		static const unsigned char _PARAMADDRESSSTOP;
		static const unsigned char _PARAMSTOP;

		using MapOfActions = std::map <MCHEmul::Address, unsigned int>;

		/**
		  * Constructor:
		  * @param argv		: The parameters as a list of Strings. \n
		  * The basic parameters for any emulator are: \n
		  * /h				: Print out the help, telling you the rest of the parameters.
		  * /fFILENAME		: bytes file (with path) to be loaded into the memory. \n
		  * /cFILENAME		: ASM file (with path) to be parsed, compiled and loadd into the memory. \n
		  * /kFILENAME		: Set of blocks file (with path) to be loaded into the memory. \n
		  * /lLEVEL			: To print out logs. \n
		  * /aADDRESS		: The address where to start the execution of the emulator. \n
		  * /d[ADDRESS]		: Line off addresses separeted by comman where the emjulator has to stop. \n
		  * /s				: When the execution must start stopped. \n
		  * @param cs		: The communication system. It can be nullptr.
		  * The emulation is able to load/understood three types of file: \n
		  * 1.- The most classic one is an Assembler text file with instructions (@see assembler block for this). \n
		  * 2.- A "binary file" can be also loaded. The binary file is made up of two parts: \n
		  * The header, that ocuppies the first X bytes of the file.
		  * In these X bytes the address where to load the bytes after is contained. \n
		  * That address is kept in the format defined by the architecture of CPU behind (little or big endian),
		  * and ocuppies the maximum number of bytes addressable for that CPU. \n
		  * Then the code itself, that is the rest of of the binary file. \n
		  * 3.- A "set of blocks file" is also understood. \n
		  * The code file might not necessary ocuppy contiguous bytes of memory. \n
		  * Use always a binary format to keep info could be very expensive. \n
		  * A "set of blocks" format could be used instead. \n
		  * The format of the file is made up of "blocks" of bytes. \n
		  * Every block consists on a header with the initial address and the number of bytes of data of the block,
		  * and the block of data itself. \n
		  * The "binary file" and the "set of blocks file" can be generated from an "assembler" 
		  * using the compilers. @see Compilers for more details. \n
		  * @see class DatamemoryBlock for further details.
		  */
		Emulator (const CommandLineArguments& args, CommunicationSystem* cS = nullptr);

		virtual ~Emulator ();

		const CommandLineArguments& cmdlineArguments () const
							{ return (_cmdlineArguments); }

		/** To print out the attributes that this emulator undestand. */
		virtual void printOutParameters (std::ostream& o = std::cout) const;

		const Computer* computer () const
							{ return (_computer == nullptr) ? (_computer = createComputer ()) : _computer; }
		Computer* computer ()
							{ return ((Computer*) (((const Emulator*) this) -> computer ())); }

		const Assembler::Parser* parser () const
							{ return (_parser == nullptr) ? (_parser = createParser ()) : _parser; }
		Assembler::Parser* parser ()
							{ return ((Assembler::Parser*) (((const Emulator*) this) -> parser ())); }

		const Assembler::Compiler* compiler () const
							{ return (_compiler == nullptr) ? (_compiler = createCompiler ()) : _compiler; }
		Assembler::Compiler* compiler ()
							{ return ((Assembler::Compiler*) (((const Emulator*) this) -> compiler ())); }

		/** Print out help parameter. */
		bool helpNeeded () const
							{ return (_cmdlineArguments.existsArgument (_PARAMHELP)); }

		/** To know whether there is a byte file where data to be loaded. \n
			"" when there is no byte file data. */
		std::string byteFileName () const
							{ return (_cmdlineArguments.existsArgument (_PARAMBYTEFILE) 
								? _cmdlineArguments.argumentAsString (_PARAMBYTEFILE) : ""); }

		/** To know whether there is a block file where data to be loaded. \n
			"" when there is no block file data. */
		std::string blockFileName () const
							{ return (_cmdlineArguments.existsArgument (_PARAMBLOCKFILE) 
								? _cmdlineArguments.argumentAsString (_PARAMBLOCKFILE) : ""); }

		/** To know whether there is an ASM file where data to be loaded. 
			"" when there is no ASM file data. */
		std::string asmFileName () const
							{ return (_cmdlineArguments.existsArgument (_PARAMASMFILE) 
								? _cmdlineArguments.argumentAsString (_PARAMASMFILE) : ""); }

		/** To know the level of the log. 
			0 means no log. */
		unsigned int logLevel () const
							{ return (_cmdlineArguments.existsArgument (_PARAMLOGLEVEL) 
								? _cmdlineArguments.argumentAsInt (_PARAMLOGLEVEL) : 1); }

		/** To know the address where to start the execution from. \n
			0x00 when no address has been defined. */
		Address startingAddress () const
							{ return (_cmdlineArguments.existsArgument (_PARAMADDRESS) 
								? _cmdlineArguments.argumentAsAddress (_PARAMADDRESS) : Address ()); }

		/** To know the list of the addresses where the computer has to stop. 
			After any only the action 1 (continue) or 2 (next) is admitted. */
		Addresses stopAddresses () const;

		/** To know whether the computer hast to start stopped or not. */
		bool stoppedAtStarting () const
							{ return (_cmdlineArguments.existsArgument (_PARAMSTOP) 
								? _cmdlineArguments.argumentAsBool (_PARAMSTOP) : false); }

		/** To change the debug level. */
		unsigned int debugLevel () const
							{ return (_debugLevel); }
		void setDebugLevel (unsigned int dL)
							{ _debugLevel = dL; computer () -> setDebugLevel (dL); }

		bool connectPeripheral (int id, const Attributes& prms, MCHEmul::IODevice* d)
							{ return (computer () -> connect (peripherialBuilder () -> peripheral (id, prms), d)); }

		// Loading programs...
		/** To load a binary file. \n
			The binary file has the address where to start to put the data into at the very first bytes loaded. \n
			If there were errors the variable "e" will be true. */
		DataMemoryBlock loadBinaryFile (const std::string& fN, bool& e);
		/** To load a program into the memory of the computer,
			using the assembler and compiler declared for the emulator. \n
			Errors are loaded back into "e" variable and the byte code produced and returned. */
		Assembler::ByteCode loadProgram (const std::string& fN, Assembler::Errors& e);
		/** To load a file made up of block of data. \n
			This type of file is made up of several data memory blocks. \n
			If there were errors the variable "e" will be true. */
		DataMemoryBlocks loadBlocksFile (const std::string& fN, bool& e);

		/** To initialize the emulator. */
		virtual bool initialize ();

		/** True when finishes ok, false when no ok. */
		bool run ();

		/** Execute just one cycle. 
			The method can receive an action that can affect the execution of the compuer cycle. */
		bool runCycle (unsigned int a = 0);

		unsigned int error () const
							{ return (_error); }
		/** To simplify the way the errors are managed. */
		bool operator ! () const
							{ return (_error != _NOERROR); }

		protected:
		const IOPeripheralBuilder* peripherialBuilder () const
							{ return (_peripheralBuilder == nullptr) ? 
								(_peripheralBuilder = createPeripheralBuilder ()) : _peripheralBuilder; }
		IOPeripheralBuilder* peripherialBuilder ()
							{ return ((IOPeripheralBuilder*) (((const Emulator*) this) -> peripherialBuilder ())); }

		/** An exit method that can hold specific code needed per cycle and per type of emulator. 
			By default it does nothing. */
		virtual bool additionalRunCycle ()
							{ return (true); }

		// Implementation
		/** To create the right version of the parser. */
		virtual Assembler::Parser* createParser () const 
							{ Assembler::Parser* r = new Assembler::Parser (_computer -> cpu ()); 
							  r -> setPrintOutProcess (true); // To print out the parsing process...
							  return (r); }
		/** To create the right version of the compiler. */
		virtual Assembler::Compiler* createCompiler () const
							{ return (new Assembler::Compiler ((Assembler::Parser*) parser ())); }
		/** To create the right version of the computer, 
			attending the parameters received by the constructor. */
		virtual Computer* createComputer () const = 0;
		/** To create the right version of the Peripheral Builder. */
		virtual IOPeripheralBuilder* createPeripheralBuilder () const = 0;

		protected:
		/** Defined in the constructor. */
		CommandLineArguments _cmdlineArguments;
		CommunicationSystem* _communicationSystem;
		unsigned int _debugLevel;

		// Implementation
		mutable Assembler::Parser* _parser;
		mutable Assembler::Compiler* _compiler;
		mutable Computer* _computer;
		mutable IOPeripheralBuilder* _peripheralBuilder;
		mutable bool _running;
		mutable bool _jumpCycle;
		unsigned int _error;
	};
}

#endif
  
// End of the file
/*@}*/
#pragma once
