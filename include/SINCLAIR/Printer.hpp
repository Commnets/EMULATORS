/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: Printer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/03/2026 \n
 *	Description: To emulate the behaviour of the Thermal Printers.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_PRINTER__
#define __SINCLAIR_PRINTER__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
	/** The basic thermal emulation just print out the information to a file. \n
		A thermal printer is really a matrix printer but using different technique to print out! */
	class BasicThermalPrinterEmulation final : 
		public MCHEmul::BasicMatrixPrinterEmulation
	{
		public:
		BasicThermalPrinterEmulation (const std::string& pFN = "MatrixPrinter.txt")
			: MCHEmul::BasicMatrixPrinterEmulation (32 /** As wide as the screen. */, pFN)
							{ }

		private:
		/** None is a control char. 
			This type of printer is just the emulation of the screen. */

		virtual void setNewPage (unsigned short p) override // Just to point out the new page in the printer file...
							{ printerFile () << "----Page:" 
											 << MCHEmul::fixLenStr (std::to_string (p), 2, true, MCHEmul::_CEROS) 
											 << "----" << std::endl; }

		/** Everything is a normal char. 
			There is no control characters. */

		virtual unsigned short printNormalChar (unsigned char chr) override;
	};

	/** Very basic print simulation,
		but using the postscript to simulate the thermal printer effect. */
	class PostscriptThermalPrinterEmulation final : 
		public MCHEmul::PostscriptMatrixPrinterEmulation
	{
		public:
		// The default configuration of the printer...
		// Usually is not needed more...
		static const MCHEmul::MatrixPrinterEmulation::Configuration _CONFIGURATION;

		PostscriptThermalPrinterEmulation (const std::string& pFN = "MatrixPrinter.ps");

		private:
		/** The main postscript routines are copied in this function. */
		virtual void firstTimePrinting (unsigned char chr) override;

		virtual void closePage (unsigned short p) override;
		virtual void setNewPage (unsigned short p) override;

		virtual bool printNewLine () override;
		virtual unsigned short printNormalChar (unsigned char chr) override;

		private:
		// Implementation
		/** The position inside the character being printed out... */
		unsigned short _posXInside;
	};
}

#endif
  
// End of the file
/*@}*/
