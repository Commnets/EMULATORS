/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SerialPrinterMPS802.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/03/2026 \n
 *	Description: To emulate the behaviour of a MPS802 serial dot matrix printer. \n
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SERIALPRINTERMPS802__
#define __COMMODORE_SERIALPRINTERMPS802__

#include <CORE/incs.hpp>
#include <COMMODORE/SerialIOPeripherals.hpp>
#include <array>

namespace COMMODORE
{
	/** First of all, MPS802 is able to format a string when a formatter is defined.
		This is the class to define a formatter. */
	class MPS802MatrixPrinterFormatter final
	{
		public:
		/** To format a string tehre are a couple of different formats to be applied. */
		class Format
		{
			public:
			Format ()
				: _error (false),
				  _errorText ("")
							{ }

			bool error () const
							{ return (_error); }
			const std::string& errorText () const
							{ return (_errorText); }
			void resetError ()
							{ _error = false; _errorText = ""; }

			virtual std::string format (const std::string& str) const = 0;

			protected:
			mutable bool _error;
			mutable std::string _errorText;
		};

		using Formats = std::vector <Format*>;

		/** Nothing to do. */
		class NoFormat final : public Format
		{
			public:
			NoFormat ()
				: Format ()
							{ }

			virtual std::string format (const std::string& str) const override
							{ return (""); }
		};

		/** The format is wrong, just from the beginning. */
		class WrongFormat final : public Format
		{
			public:
			WrongFormat ()
				: Format ()
							{ _error = true; _errorText = "*PE:F*"; }

			/** Nothing to do. */
			virtual std::string format (const std::string&) const override
							{ return (""); }
		};

		/** Very simple, just a set of spaces. 
			This format is applied always between fields of information. */
		class SpaceFormat final : public Format
		{
			public:
			SpaceFormat (unsigned char s)
				: Format (),
				  _spaces (s) 
							{ }

			/** The parameter is not used in this case. */
			virtual std::string format (const std::string&) const override
							{ return (MCHEmul::_SPACES.substr (0, _spaces)); }

			private:
			unsigned char _spaces;
		};

		/** Just to adjust a text to a fixed length. */
		class TextFormat final : public Format
		{
			public:
			TextFormat (unsigned char nL)
				: Format (),
				  _numberLetters (nL)
							{ }

			virtual std::string format (const std::string& str) const override
							{ return ((str.length () >= (size_t) _numberLetters) 
								? str.substr (0, _numberLetters) 
								: str + MCHEmul::_SPACES.substr (0, (_numberLetters - str.length ()))); }

			private:
			unsigned char _numberLetters;
		};

		/** The most complex one, 
			just to adjust how to print out a numeric value. */
		class NumericFormat final : public Format
		{
			public:
			NumericFormat (const std::string& str);

			virtual std::string format (const std::string& str) const override;

			private:
			std::string _structure;

			// Implementation
			/** To speed up the further analysis. */
			std::string _fBeforePoint;
			std::string _fAfterPoint;
		};

		static Formats createFormatsFrom (const std::string& f);

		MPS802MatrixPrinterFormatter (const std::string& f = "")
			: _formats (std::move (createFormatsFrom (f)))
							{ }

		MPS802MatrixPrinterFormatter (Formats&& fmts)
			: _formats (std::move (fmts))
							{ }

		~MPS802MatrixPrinterFormatter ()
							{ deleteFormats (); }

		/** To replace the formats. */
		void setFormats (Formats&& fmts)
							{ deleteFormats (); _formats = std::move (fmts); }
		void setFormats (const std::string& f)
							{ deleteFormats (); _formats = std::move (createFormatsFrom (f)); }

		// Related with the format
		/** Format the string received, 
			if the formatter is = "", none is done. */
		std::string format (const std::string& s) const;
		/** To know whether there is or not an error in the last formatting. */
		inline bool error () const;
		/** To get all errors. */
		inline std::string errorTexts () const;
		/** To reset the error. */
		void resetError ()
							{ for (const auto& i : _formats) i -> resetError (); }

		private:
		void deleteFormats ()
							{ for (const auto& i : _formats) delete (i); _formats = { }; }

		private:
		Formats _formats;
	};

	// ---
	inline bool MPS802MatrixPrinterFormatter::error () const
	{ 
		bool result = false; 
		for (const auto& i : _formats) 
			result |= i -> error (); 
		return (result);
	}

	// ---
	inline std::string MPS802MatrixPrinterFormatter::errorTexts () const
	{
		std::string result = "";
		for (const auto& i : _formats) 
			result += i -> errorText () + "\n";
		return (result);
	}

	// The MPS802 PRTINTER was more complex and the characteres had more resolution...
	/** MPS802 emulated in a very simple way. \n
		Just printing out chars and when double is set up (the only configuration set), 
		two letters equal are printed out. */
	class MPS802BasicMatrixPrinterEmulation final : 
		public MCHEmul::BasicMatrixPrinterEmulation
	{
		public:
		MPS802BasicMatrixPrinterEmulation (const std::string& pFN = "MPS802MatrixPrinter.txt");

		/** Many of the different possibilities of this printer 
			ara managed throught the secondaryt address when open, that is = functions. */
		virtual void activateFunction (unsigned char f) override;
		virtual void desactivateFunction (unsigned char f) override
							{ _activeFunction = Function::_NONE; _businessMode = false; }
		virtual void desactivateAllFunctions () override
							{ _activeFunction = Function::_NONE; _businessMode = false; }

		private:
		/** Nothing is "by default" a control char.
			All all them will be taken as normal chars, but they might or not have effect depending on 
			the type of function active. \n
			So the default implementation of isControlChar (that returns always false)
			and manageControlChar (that doesn't do anything and return 0,0,0) are enough. */

		virtual void setNewPage (unsigned short p) override // Just to point out the new page in the printer file...
							{ printerFile () << "----Page:" 
											 << MCHEmul::fixLenStr (std::to_string (p), 2, true, MCHEmul::_CEROS) 
											 << "----" << std::endl; }

		/** Everything will be taken as a "normal char", 
			but when printNormalChar is executed, the effect might change depending on the function active. \n
			So the default implementation of isNormalChar (that returns always true) is enough. */

		virtual bool printNewLine () override;
		virtual unsigned short printNormalChar (unsigned char chr) override;

		private:
		// Implementation
		/** The different functions that can be activated in the printer (in this implementatuion) 
			using a secondary address. */
		enum class Function : unsigned char
		{
			_NONE = 0x00,							// The defaul mode. It uses the grphic mode characters...
			_USEFORMATTER = 0x01,					// When used, the format, if defined, is used... 
			_DEFININGFORMATTER = 0x02,				// To define a formatter...
			_ENABLEPRINTERFORMATMESSAGES = 0x04,	// When it is used and there is any error in printing with a format, 
													// the error message printed out
			_BUSINESSMODE = 0x07,					// The characters are different (lower case as default)...
			_SUPPRESDIAGNOSTICMESSAGES = 0x09,		// When it is used, the diagnostic messages are not printed out in the printer file...
			_RESETPRINTER = 0x0a					// The configuration is set jup to the default one...
		};

		/** The function active, by default it is nothing. */
		Function _activeFunction;

		/** Is the business mode active? */
		bool _businessMode;
		/** The last formatter defined. \n
			Empty when none. */
		MPS802MatrixPrinterFormatter _lastFormatter;
		/** How many times a letter has to be repeated. Usually just 1
			unless the enlarge mode is up. */
		unsigned char _timesRepeated;
		/** Printing or not messages when error */
		bool _printingErrorMessages;

		/** When the definition of a format is in progress,
			this variable is used to accumulate the definition of the format until a new line is printed out. */
		std::string _lineUnderConstruction;
	};

	/** MPS802 emulated generating a postscript file. 
		The special characters are managed in the full emulation, including the graphical things. */
	class MPS802PostscriptMatrixPrinterEmulation final : 
		public MCHEmul::PostscriptMatrixPrinterEmulation
	{
		public:
		// The default configuration of the printer...
		// Usually is not needed more...
		static const MCHEmul::MatrixPrinterEmulation::Configuration _CONFIGURATION;

		MPS802PostscriptMatrixPrinterEmulation (
				const MCHEmul::MatrixPrinterEmulation::Paper& p,
				const std::string& pFN = "MPS802MatrixPrinter.ps");

		/** The secondary address determines the function for the active channel. */
		virtual void activateFunction (unsigned char f) override;
		virtual void desactivateFunction (unsigned char f) override
							{ _activeFunction = Function::_NONE; _businessMode = false; }
		virtual void desactivateAllFunctions () override
							{ _activeFunction = Function::_NONE; _businessMode = false; }

		private:
		/** The main postscript routines are copied. */
		virtual void firstTimePrinting (unsigned char chr) override;

		/** Nothing is "by default" a control char.
			All all them will be taken as normal chars, but they might or not have effect depending on 
			the type of function active. \n
			So the default implementation of isControlChar (that returns always false)
			and manageControlChar (that doesn't do anything and return 0,0,0) are enough. */

		virtual void closePage (unsigned short p) override;
		virtual void setNewPage (unsigned short p) override;

		/** Everything will be taken as a "normal char", 
			but when printNormalChar is executed, the effect might change depending on the function active. \n
			So the default implementation of isNormalChar (that returns always true) is enough. */

		virtual bool printNewLine () override;
		virtual unsigned short printNormalChar (unsigned char chr) override;

		// Implementation
		/** Just to simulate the carryReturn. */
		void carryReturn ()
							{ _posXInside = 0; moveHeadTo (0, headYPosition ()); }
		/** Just to simulate a lineFeed. 
			When the limit of the page is reached out (taken into account whether paging is on or off,
			a new page might be set up (setNewPage). */
		void lineFeed ();
		/** Just to simulate carryReturn and lineFeed both together. \n
			Usually when a new line happens. */
		void carryReturnAndLineFeed ()
							{ carryReturn (); lineFeed (); }
		/** Just to advance one X internal position. \n
			Anytime a byte is printed out the head of the printer has to me moved one position right. \n
			The limit of the printer can be reached out, so a carryReturn and a lineFeed might happend. */
		void advance1HeadPosition ();
		/** Just to print out a byte. \n
			Any time a byte is printed out then the head of the printer has to advance a position (internal) in X. */
		void printBytePostscript (const MCHEmul::UByte& b);
		/** Used to print out a set of bytes, one after the other,
			considering of how to move the head of the printer. */
		void printBytesPostscript (const std::vector <MCHEmul::UByte>& bs)
							{ for (size_t i = 0; i < bs.size (); printBytePostscript (bs [i++])); }
		void printChrPostScript (unsigned char chr);
		/** Used to print out a set of characters. \n 
			This is used when formatting is active. */
		void printTextPostScript (const std::string& txt)
							{ for (const auto& i : txt) printChrPostScript (i); }
		/** Print a line of text. */
		void printLineOfTextPostScript (const std::string& txt)
							{ printTextPostScript (txt); carryReturnAndLineFeed (); }

		private:
		// Implementation
		/** The different functions that can be activated in the printer (in this implementatuion) 
			using a secondary address. */
		enum class Function : unsigned char
		{
			_NONE = 0x00,							// The defaul mode. It uses the graphic mode characters...
			_USEFORMATTER = 0x01,					// When used, the format, if defined, is used... 
			_DEFININGFORMATTER = 0x02,				// To define a formatter...
			_SETNUMBERLINESPERPAGE = 0x03,			// To set the number of lines used per page, 
													// including 3 lines at the top and 3 more at the bottom
			_ENABLEPRINTERFORMATMESSAGES = 0x04,	// When it is used and there is any error in printing with a format, 
													// the error message printed out
			_DEFINEPROGRAMABLECHARACTER = 0x05,		// To define a programmable character...
			_SETTINGSPACEBETWEENLINES = 0x06,		// The space between lines can be changed...
			_BUSINESSMODE = 0x07,					// The characters are different (lower case as default)...
			_SUPPRESDIAGNOSTICMESSAGES = 0x09,		// When it is used, the diagnostic messages are not printed out in the printer file...
			_RESETPRINTER = 0x0a					// The configuration is set jup to the default one...
		};

		/** The function active, by default it is nothing. */
		Function _activeFunction;

		/** Is the business mode active? */
		bool _businessMode;
		/** The last formatter defined. \n
			Empty when none. */
		MPS802MatrixPrinterFormatter _lastFormatter;
		/** The double mode, when active, can be multiplied by several times. */
		unsigned char _timesRepeated;
		/** Printing or not messages when error */
		bool _printingErrorMessages;
		/** Reverse on/off. */
		bool _reverse;
		/** Paging on/off. */
		bool _paging;
		/** To define a programable char. */
		std::array <MCHEmul::UByte, 8> _programableGraphic;
		/** When the graphic has been defined. */
		bool _programableGraphicDefined;

		/** When the definition of a format is in progress,
			this variable is used to accumulate the definition of the format until a new line is printed out. */
		std::string _lineUnderConstruction;

		/** The position inside character being printed out... */
		unsigned short _posXInside;
	};
}

#endif
  
// End of the file
/*@}*/
