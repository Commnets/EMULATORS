/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SerialPrinterMPS801.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/03/2026 \n
 *	Description: To emulate the behaviour of a MPS801 serial dot matrix printer. \n
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SERIALPRINTERMPS801__
#define __COMMODORE_SERIALPRINTERMPS801__

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
		MPS801BasicMatrixPrinterEmulation (const std::string& pFN = "MPS801MatrixPrinter.txt");

		/** When the secondary address is used,
			and it is 7, the businessMode is activated or desactivated. */
		virtual void activateFunction (unsigned char f) override
							{ if (f == 0 || f == 7) _businessMode = (f == 7); }
		virtual void desactivateFunction (unsigned char f) override
							{ if (f == 0 || f == 7) _businessMode = !(f == 7); }
		virtual void desactivateAllFunctions () override
							{ _businessMode = false; }

		private:
		/** The only control character managed is to double the size of the letters, 
			repeating the same char, the line feed, and the type of letter (uuper case or lower case) writtend down). */
		virtual bool isControlChar (unsigned char chr) override;
		virtual std::tuple <short, short, short> manageControlChar (unsigned char chr) override;
		virtual void setNewPage (unsigned short p) override // Just to point out the new page in the printer file...
							{ printerFile () << "----Page:" 
											 << MCHEmul::fixLenStr (std::to_string (p), 2, true, MCHEmul::_CEROS) 
											 << "----" << std::endl; }
		/** Only the list of letters and numbers both in business mnode and in the graphical mode. */
		virtual bool isNormalChar (unsigned char chr) override;

		virtual unsigned short printNormalChar (unsigned char chr) override;

		private:
		bool _businessMode;
		bool _double;
	};

	/** MPS801 emulated generating a postscript file. 
		The special characters are managed in the full emulation, including the graphical things. */
	class MPS801PostscriptMatrixPrinterEmulation final : 
		public MCHEmul::PostscriptMatrixPrinterEmulation
	{
		public:
		// The default configuration of the printer...
		// Usually is not needed more...
		static const MCHEmul::MatrixPrinterEmulation::Configuration _CONFIGURATION;

		MPS801PostscriptMatrixPrinterEmulation (
				const MCHEmul::MatrixPrinterEmulation::Paper& p,
				const std::string& pFN = "MPS801MatrixPrinter.ps");

		/** When the secondary address is used,
			and it is 7, the businessMode is activated or desactivated. */
		virtual void activateFunction (unsigned char f) override
							{ if (f == 0 || f == 7) _businessMode = (f == 7); }
		virtual void desactivateFunction (unsigned char f) override
							{ if (f == 0 || f == 7) _businessMode = !(f == 7); }
		virtual void desactivateAllFunctions () override
							{ _businessMode = false; }

		private:
		/** The main postscript routines are copied. */
		virtual void firstTimePrinting (unsigned char chr) override;

		virtual bool isControlChar (unsigned char chr) override;
		virtual std::tuple <short, short, short> manageControlChar (unsigned char chr) override;
		virtual void closePage (unsigned short p) override;
		virtual void setNewPage (unsigned short p) override;
		virtual bool isNormalChar (unsigned char chr) override;

		virtual bool printNewLine () override;
		virtual unsigned short printNormalChar (unsigned char chr) override;

		private:
		// Implementation
		/** True when the double width per char is activated. */
		bool _double;
		/** True when the business mode printing is set. */
		bool _businessMode;
		/** The graphics mode is or not set. */
		bool _graphicMode;
		/** Setting the tab. */
		bool _settingTab;
		unsigned char _charsSettingtabPending;
		unsigned char _nextTabSettingValue [2];
		/** Setting the specific dot address. 
			The previous one are also used. */
		bool _setSpecificDotAddress;
		/** When the reverse function is selected. */
		bool _reverse;

		/** The position inside the character being printed out... */
		unsigned short _posXInside;
	};
}

#endif
  
// End of the file
/*@}*/
