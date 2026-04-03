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
#include <SINCLAIR/ZXCodeToASCII.hpp>

namespace SINCLAIR
{
	/** The basic thermal emulation just print out the information to a file. \n
		A thermal printer can be simulated through out a matrix printer
		but using different "technique" to print out! */

	/** In this version the characteres are just inserted into a text plan plain.
		Only the characteres that has an equivalent in ASCII are printed out
		For the rest, just an space is shown. */
	class BasicThermalPrinterEmulation final : 
		public MCHEmul::BasicMatrixPrinterEmulation
	{
		public:
		/** The class is the owner of the ZXCodeToASCII converter. 
			So it must be deleted at destruction time. */
		BasicThermalPrinterEmulation (ZXCodeToASCII* c,
			const std::string& pFN = "MatrixPrinter.txt");

		~BasicThermalPrinterEmulation ()
							{ delete (_ZXCodeConversor); }

		private:
		/** None is a control char. 
			This type of printer is just the emulation of the screen. */

		virtual void setNewPage (unsigned short p) override // Just to point out the new page in the printer file...
							{ printerFile () << "----Page:" 
											 << MCHEmul::fixLenStr (std::to_string (p), 2, true, MCHEmul::_CEROS) 
											 << "----" << std::endl; }

		/** Everything is a normal char. 
			There is no control characters. */

		virtual unsigned short printNormalChar (unsigned char chr) override
							{ printerFile () << std::string (1, (char) _ZXCodeConversor -> convert (chr)); return (1); }

		private:
		ZXCodeToASCII* _ZXCodeConversor;
	};

	/** In this version a real simulation is done
		using postscript files. */
	class PostscriptThermalPrinterEmulation final : 
		public MCHEmul::PostscriptMatrixPrinterEmulation
	{
		public:
		PostscriptThermalPrinterEmulation 
			(const MCHEmul::MatrixPrinterEmulation::Configuration& cfg, 
			 const std::string& pFN = "MatrixPrinter.ps");

		private:
		/** The main postscript routines are copied in this function. */
		virtual void firstTimePrinting (unsigned char chr) override;

		virtual void closePage (unsigned short p) override;
		virtual void setNewPage (unsigned short p) override;

		virtual bool printNewLine () override
							{ return (true); }
		virtual unsigned short printNormalChar (unsigned char chr) override;
	};
}

#endif
  
// End of the file
/*@}*/
