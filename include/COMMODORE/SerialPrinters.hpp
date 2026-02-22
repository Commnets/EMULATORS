/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SerialPrinter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/01/2026 \n
 *	Description: To emulate the behaviour of a serial printer. \n
 *				 The way the serial printers work in the COMMODORE world are all matrix dot printers. \n
 *				 The most standard ones are declared.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SERIALPRINTERPERIPHERALS__
#define __COMMODORE_SERIALPRINTERPERIPHERALS__

#include <CORE/incs.hpp>
#include <COMMODORE/SerialIOPeripherals.hpp>

namespace COMMODORE
{
	/** MPS801 emulated in a very simple way. \n
		Just printing out chars and when double is set up (the only configuration set), 
		two letters equal are printed out. */
	class MPS801BasicMatrixPrinterEmulation final : 
		public MCHEmul::BasicMatrixPrinterEmulation
	{
		public:
		MPS801BasicMatrixPrinterEmulation (const std::string& pFN = "MPS801MatrixPrinter.txt")
			: MCHEmul::BasicMatrixPrinterEmulation (80 /** Always. */, pFN),
			  _double (false) // There i no more configuration needed...
							{ }

		private:
		/** The main thing to do is the concversion between PETSCII y ASCII. */
		virtual bool printCharImplementation (unsigned char chr) override;

		private:
		bool _double;
	};

	/** MPS801 emulated generating a postscript file. 
		The special characters are managed in the full emulation, including the graphical things. */
	class MPS801PostscriptMatrixPrinterEmulation final : 
		public MCHEmul::PostscriptMatrixPrinterEmulation
	{
		public:
		MPS801PostscriptMatrixPrinterEmulation (const MCHEmul::MatrixPrinterEmulation::Paper& p,
				const std::string& pFN = "MPS801MatrixPrinter.ps")
			: MCHEmul::PostscriptMatrixPrinterEmulation (_CONFIGURATION, p, pFN),
			  _double (false),
			  _graphical (false),
			  _settingTab (false),
			  _reverse (false),
			  _cInside (0),
			  _posXInside (0), _posYInside (0)
							{ }

		private:
		/** The main postscript routines are copied. */
		virtual void firstTimePrinting (unsigned char chr) override;
		/** The main thing to do is the concversion between PETSCII y ASCII. */
		virtual bool printCharImplementation (unsigned char chr) override;

		private:
		static const MCHEmul::MatrixPrinterEmulation::Configuration _CONFIGURATION;

		// Implementation
		/** True when the double width per char is activated. */
		bool _double;
		/** True when the graphical printing is set. */
		bool _graphical;
		/** Setting the tab. */
		bool _settingTab;
		/** When the reverse function is selected. */
		bool _reverse;
		/** The number of column with a character. */
		unsigned char _cInside;

		// The position inside the page...
		unsigned short _posXInside, _posYInside;
	};

	/** To emulate a serial printer.
		The name of the file created to keep the info is received as parameter, but there is a default value. \n
		The object is owner of the way the printer is emulated (see the destructor). */
	class SerialPrinterPeripheralSimulation : public SerialIOPeripheralSimulation
	{
		public:
		SerialPrinterPeripheralSimulation 
			(int id, unsigned char dN, 
			 const Definition& dt,
			 const MCHEmul::Attributes & attrs,
			 MCHEmul::MatrixPrinterEmulation* mPE = 
				new MPS801BasicMatrixPrinterEmulation ("Printer.txt"));

		virtual ~SerialPrinterPeripheralSimulation () override;

		const MCHEmul::MatrixPrinterEmulation* emulation () const
							{ return (_emulation); }
		MCHEmul::MatrixPrinterEmulation* emulation ()
							{ return (_emulation); }

		/** Open & close the output file. */
		virtual bool initialize () override
							{ return (_emulation -> initialize ()); }
		virtual bool finalize () override
							{ return (_emulation -> finalize ()); }

		/** To execute a command.
			The commands are:
			1: FLUSH, to send the content still kept in the buffer to the printer file. */
		virtual bool executeCommand (int cId, const MCHEmul::Strings& prms) override;
		/** To get the commands to manage the device, if any. */
		virtual MCHEmul::Strings commandDescriptions () const
							{ return { "FLUSH:Send buffer to printer" }; }

		/** The info returned is the one from the parent class, plus:
			MatrixPrinterEmulation	= InfoStructure : Information about the emulation. */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual unsigned char listen (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		virtual unsigned char unlisten (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		virtual unsigned char talk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		virtual unsigned char untalk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		virtual unsigned char openChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn) override;
		virtual unsigned char closeChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn) override;
		virtual unsigned char sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		virtual unsigned char receiveByte (MCHEmul::CPU* cpu, MCHEmul::UByte& b) override;
		virtual unsigned char getReady (MCHEmul::CPU* cpu) override;

		protected:
		/** The emulation used. */
		MCHEmul::MatrixPrinterEmulation* _emulation;
	};
}

#endif
  
// End of the file
/*@}*/
