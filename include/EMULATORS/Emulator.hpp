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
		static const unsigned char _PARAMLANGUAGE;
		static const unsigned char _PARAMSOUND;
		static const unsigned char _PARAMPERIPHERALS;

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
		  * /d[ADDRESS]		: Line of addresses separeted by comma where the emulator has to stop. \n
		  * /i[LANGID]		: The language of the emulation. \n
		  *					  It has to be interpreted by the Computer (@see Computer class). \n
		  *					  In some situations the ROM and KERNEL, e.g. can be different depending on the language. \n
		  * /p[PER1:dt,..]	: Connect the peripherals identified in the list separated by comma. \n
		  * /r[ON|OFF]		: To activate or desactivate the sound when starting. By default it is activated.
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

		Emulator (const Emulator&) = delete;

		Emulator& operator = (const Emulator&)= delete;

		virtual ~Emulator ();

		Emulator (Emulator&&) = delete;

		Emulator& operator = (Emulator&&)= delete;

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
								? _cmdlineArguments.argumentAsInt (_PARAMLOGLEVEL) : _DEBUGERRORS); }

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

		/** To know the language used by the computer emulated behind. \n
			The value by default will be always _DEFAULTLANGUAGE refering to english. */
		std::string computerLanguage () const
							{ return (_cmdlineArguments.existsArgument (_PARAMLANGUAGE) 
								? _cmdlineArguments.argumentAsString (_PARAMLANGUAGE) : _DEFAULTLANGUAGE); }

		/** To activate or desactivate the sound. */
		bool soundAtStarting () const
							{ return (_cmdlineArguments.existsArgument (_PARAMSOUND)
									? _cmdlineArguments.argumentAsBool (_PARAMSOUND) : true); }

		/** To know the list of the peripherals connected. */
		std::vector <int> peripheralsConnected () const;

		/** To change the debug level. */
		unsigned int debugLevel () const
							{ return (_debugLevel); }
		void setDebugLevel (unsigned int dL)
							{ _debugLevel = dL; _computer -> setDebugLevel (dL); }

		// The builders...
		/** To create peripherals. */
		const IOPeripheralBuilder* peripherialBuilder () const
							{ return (_peripheralBuilder == nullptr) ? 
								(_peripheralBuilder = createPeripheralBuilder ()) : _peripheralBuilder; }
		IOPeripheralBuilder* peripherialBuilder ()
							{ return ((IOPeripheralBuilder*) (((const Emulator*) this) -> peripherialBuilder ())); }
		/** To create file readers. */
		const FileIO* fileIO () const
							{ return (_fileReader == nullptr) ? 
								(_fileReader = createFileReader ()) : _fileReader; }
		FileIO* fileIO ()
							{ return ((FileIO*) (((const Emulator*) this) -> fileIO ())); }

		// Managing peripherals...
		/** Build up and connect a peripheral. */
		bool connectPeripheral (int id, const Attributes& prms, MCHEmul::IODevice* d);
		/** This is the simpliest version. It will try to detect automatically 
			the device where to connect the peripheral which constrction parameters are received. */
		bool connectPeripheral (int id, const Attributes& prms = { });
		/** There is also a method to connect a list of them. \n
			It there have been attributes they would have to be in the same order than ids. */
		bool connectPeripherals (std::vector <int> ids, const std::vector <Attributes>& prms = { });
		/** Connect the data to a peripheral, building up the data. 
			Returns a reference to the data loaded and nullptr if error or nothing. */
		FileData* connectDataToPeripheral (const std::string& fN, int id);
		/** To retrieve data from a peripheral into a file. 
			Returns true when the data was sucessfully saved and false in other circunstance. */
		bool saveDataFromPeripheral (const std::string& fN, int id);
		/** To disconnect the peripherals. */
		bool disconnectPeripheral (int id)
							{ return (_computer -> disconnectPeripheral (id)); }
		/** Same but all. */
		bool disconnectAllPeripherals ()
							{ return (_computer -> disconnectAllPeripherals ()); }

		// Loading programs directly into the memory...
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
		/** An exit method that can hold specific code needed per cycle and per type of emulator. 
			By default just actualize the global time (@see global.hpp). \n
			Returns true when evrythinh ok, and false in other case. */
		virtual bool additionalRunCycle ()
							{ actualizeGlobalTime (); return (true); }

		// Implementation
		/** To create the right version of the parser. */
		virtual Assembler::Parser* createParser () const;
		/** To create the right version of the compiler. */
		virtual Assembler::Compiler* createCompiler () const
							{ return (new Assembler::Compiler ((Assembler::Parser*) parser ())); }
		/** To create the right version of the computer, 
			attending the parameters received by the constructor. */
		virtual Computer* createComputer () const = 0;
		/** To create the right version of the Peripheral Builder. */
		virtual IOPeripheralBuilder* createPeripheralBuilder () const = 0;
		/** To create the right version of the File Reader. */
		virtual FileIO* createFileReader () const = 0;

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
		mutable FileIO* _fileReader;
		mutable bool _running;
		unsigned int _error;
	};
}

#endif
  
// End of the file
/*@}*/
#pragma once
