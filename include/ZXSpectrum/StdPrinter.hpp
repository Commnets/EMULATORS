/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: Printer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/03/2026 \n
 *	Description: To emulate the behaviour of the Thermal Printer connected 
 *				 to a ZXSpectrum Computer (through the Edge Connector). \n
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_PRINTER__
#define __ZXSPECTRUM_PRINTER__

#include <SINCLAIR/incs.hpp>
#include <ZXSpectrum/EdgeConnectorPeripherals.hpp>
#include <array>

namespace ZXSPECTRUM
{
	/** The Thermal Printer simulation. */
	class ThermalPrinterSimulation final : public EdgeConnectorPeripheral
	{
		public:
		/** The configuration of the printer.
			Usually it is needed once. */
		static const MCHEmul::MatrixPrinterEmulation::Configuration _CONFIGURATION;

		static const int _ID = 102;

		/** The name of the traps. */
		static const int _PRINT_A = 0;
		static const int _COPY_BUFF = 1;

		ThermalPrinterSimulation (MCHEmul::MatrixPrinterEmulation* mPE = 
			new MCHEmul::BasicMatrixPrinterEmulation (32, "Printer.txt"));

		/** Open & close the output file. */
		virtual bool initialize () override;
		virtual bool finalize () override
							{ return (_emulation -> finalize ()); }

		/** To execute a command.
			The commands are:
			1: FLUSH, to send the content still kept in the buffer to the printer file. */
		virtual bool executeCommand (int cId, const MCHEmul::Strings& prms) override;
		/** To get the commands to manage the device, if any. */
		virtual MCHEmul::Strings commandDescriptions () const
							{ return (MCHEmul::Strings (
								{ "1:FLUSH",
								  "2:NEW PAGE" })); }

		/** The traps in the ZXSpectrum are different than in a ZX81 e.g. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** This method is invoked from simulate method to determine whether the 
			CPU is just on a point where a trap maust be executed. ºn
			Returns true when the trap is really executed. */
		bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu, MCHEmul::Address& rA);
		/** The traps to be executed. */
		bool executePrintATrap (MCHEmul::CPU* cpu, unsigned char& st, MCHEmul::Address& rA);
		bool executeCopyBuffTrap (MCHEmul::CPU* cpu, unsigned char& st, MCHEmul::Address& rA);

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugStatus (const std::string& where, MCHEmul::CPU* cpu);
		// -----

		private:
		/** Relative position (respect IY register) of the address 
			with the position of the next element in the printing buffer. */
		static const size_t _PR_CC_POS;
		/** Relative position (respect IY register) of the address 
			with the FLAG2, where it is pointed whether the buffer is or not empty. */
		static const size_t _PR_FLAG2_POS;
		/** Address of the memory buffer. */
		static const MCHEmul::Address _PRBUFF;
		/** Length of the memory buffer (= 0x100). */
		static const size_t _BLEN;
		/** The position of the CURCHL. 
			Thi position point somwhow what is the logical channel active
			so it is usefull to determine if the printer is the channel being used actually. */
		static const MCHEmul::Address _CURCHL;
		/** The position PFLAG keeps whether the inverse is active or not for the current printing. 
			This is important to print out. */
		static const MCHEmul::Address _PFLAG;

		/** A reference to the emulation. */
		MCHEmul::MatrixPrinterEmulation* _emulation;
		/** The trap where the info to the printer is caught. */
		const MCHEmul::Traps _traps;

		// Implementation
		/** The temporal buffer where the chars and the way the will be printed out are stored. 
			The first byte in every position keeps the status of the char to print out,
			whilst the second one keeps the char to realy print out. */
		union CharBufferElement
		{
			struct
			{
				unsigned char _status;
				unsigned char _char;
			};

			unsigned short _fullValue;
		};

		std::array <CharBufferElement, 32> _charsBuffer;
		/** Where the pointer is now. */
		unsigned char _charsBufferPointer;
	};
}

#endif
  
// End of the file
/*@}*/
