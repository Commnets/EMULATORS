/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: MatrixPrinterEmulation.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 08/02/2026 \n
 *	Description: To emulate an old matrix printer
 *				 The way the matrix prints can be in many different formats!
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_MATRIXPRINTEREMULATION__
#define __MCHEMUL_MATRIXPRINTEREMULATION__

#include <CORE/InfoClass.hpp>
#include <string>
#include <fstream>

namespace MCHEmul
{
	/** A class to emulate the way a dot matrix works. */
	class MatrixPrinterEmulation : public InfoClass
	{
		public:
		/** A sytructire to decribe the paper where the printing happens. */
		struct Paper
		{
			enum class Type
			{
				_WHITE		= 0,
				_BLUEBAND	= 1,
				_GREENBAND	= 2,
				_GRAYBAND	= 3
			};

			Paper ()
				: _type (Type::_BLUEBAND),
				  _width (9.5f), _height (11.0f),
				  _border (0.5f)
							{ }

			/** The border is always a part of the width. */
			Paper (Type t, float w, float h, float b)
				: _type (t),
				  _width (w), _height (h),
				  _border (b)
							{ assert ((w > (2 * b))); }

			/** 
				The attributes added are: \n
				PAPERWIDTH		= Attribute: The width in inches of the paper. \n
				PAPERHEIGHT		= Attribute: The heigth in inches if the paper.  \n
				PAPERBORDER		= Attribute: The width in inches of the border of the paper.
			  */
			InfoStructure getInfoStructure () const;
				  
			/** The type. */
			Type _type;
			/** The size of the paper was usually done in inches. */
			float _width, _height;
			/** The size of the boder, also in inches.
				That border is where the track marcks of the paper are usually. */
			float _border;

			/** Most common size of papers. */
			static const Paper _9HALFx11INCHES;
		};

		/** The configuration of the printer.
			Not all fields must be used in any type of printer. */
		struct Configuration
		{
			using CharDefinition	= std::vector <UByte>;
			using CharSetDefinition = std::map <unsigned int, CharDefinition>;

			Configuration ()
				: _description (""),
				  _wChar (8), _hChar (8),
				  _charSet (),
				  _charsPerLine (80),
				  _charsPerPage (80),
				  _charNewLine ((unsigned char) 0x0d), // This is the typical value for carry return!
				  _charSpace ((unsigned char) 0x20),
				  _charTab ((unsigned char) 0x09), _numSpacesTabChar (4) // ...and the typical value for TAB!
							{ }

			Configuration (unsigned char wC, unsigned char hC,
				const CharSetDefinition& d, unsigned short cL, unsigned short cP, 
				unsigned char cNL, unsigned char cSpc, char cT, unsigned short sT)
				: _description (""), // The description has specifically to be assigned later...
				  _wChar (wC), _hChar (hC),
				  _charSet (d),
				  _charsPerLine (cL),
				  _charsPerPage (cP),
				  _charNewLine (cNL),
				  _charSpace (cSpc),
				  _charTab (cT), _numSpacesTabChar (sT)
							{ }

			/** 
				The attributes added are: \n
				CHARWIDTH		= Attribute: The width in pixels per char (in normal size). \n
				CHARHEIGHT		= Attribute: The heigth in pixels per char (in normal size).  \n
				CHARSPERLINE	= Attribute: The number of chars (normal size) per line.
			  */
			InfoStructure getInfoStructure () const;

			/** A description for the configuration. */
			std::string _description;

			/** The size in bits per char. 
				Usually 8x8 but in some systems (like Commodore's) can be 6x7. */
			unsigned char _wChar;
			unsigned char _hChar;
			/** The information about all chars. */
			CharSetDefinition _charSet;

			/** How many characters per line in the normal size. */
			unsigned short _charsPerLine;
			/** How many characters per page in the normal size. */
			unsigned short _charsPerPage;

			// Special codes to guide the printer...
			unsigned char _charNewLine;
			unsigned char _charTab;
			unsigned char _charSpace;
			unsigned short _numSpacesTabChar;
		};

		MatrixPrinterEmulation (const Configuration& cfg, const Paper& p, 
			const std::string& pFN);

		virtual ~MatrixPrinterEmulation ();

		/** See the configuration... */
		const Configuration& configuration () const
							{ return (_configuration); }
		Configuration& configuration ()
							{ return (_configuration); } // To enable to change valued...
		/** Manage the size of the paper. */
		const Paper& paper () const
							{ return (_paper); }
		void setPaper (const Paper& p)
							{ _paper = p; }

		/** Close the current page. */
		void closeCurrentPage () 
							{ closePage (_page); setNewPage (++_page); moveHeadTo (0, 0); }

		/** To get a reference to the printer file. */
		const std::ofstream& printerFile () const
							{ return (_printerFile); }
		std::ofstream& printerFile ()
							{ return (_printerFile); }

		// Init and finish the printer!
		virtual bool initialize ();
		virtual bool finalize ();

		// Mamaging the head of the printer.
		/** Return the x position of the head of the printer. */
		unsigned short headXPosition () const
							{ return (_posX); }
		unsigned short headYPosition () const
							{ return (_posY); }
		/** Get the position of the head of the printer. */
		void headPosition (unsigned short &px, unsigned short& py)
							{ px = _posX, py = _posY; }
		unsigned short page () const
							{ return (_page); }
		/** Move the head x positions respect the current position. \n
			Can be moved forward and backward. Never less than the 0,0. \n
			The method returns whether there was a change in the line and a change in the page. \n
			For more detail about the final position, use the method headPosition. */
		std::tuple <bool /** line. */, bool /** page. */> moveHeadFromX (short px);
		/** Move the head y positions respect the current position. \n
			Can also be moved forward and backward, but never less than 0,0. \n
			Again, it is returned whether the line and the page have been moved. */
		std::tuple < bool /** line. */, bool /** page. */> moveHeadFromY (short py);
		/** Move the head to a specific position. */
		void moveHeadTo (unsigned short px, unsigned short py)
							{ _posX = px; _posY = py; }
		void setPage (unsigned short p)
							{ _page = p; }

		/** The emulation belogs always to a device. \n
			There can be different channels open for the same device, so 
			when a "listen" event happen at peripherical level the emulation needs to know to 
			which channel is that for!. It could be useful for some reasons... */
		unsigned char activeChannel (size_t t) const // BE carefull because no border limits are checked...
							{ return (_activeChannel [t]); }
		void setActiveChannel (size_t t, unsigned char chn)
							{ _activeChannel [t] = chn; }

		/** The way a char is printed out will depend on the specific char and the specific matriz printer emulation. \n
			First time printing anything something special could be done,
			and before and after printer anything again something spwecial could be done. 
			If when printing the max number of chars per line was reached, 
			the head of printer will be moved to another line. (easy, because everything is printed out char by char) */
		void printChar (unsigned char chr);
		/** Printing a string. */
		void printString (const std::string& str)
							{ for (const auto& i : str) printChar (i); }

		/** The emulation might be invoked 
			to activate/desactivate a specific channel function. \n
			By default it doesn't do anything. */
		virtual void activateFunction (unsigned char) { }
		virtual void desactivateFunction (unsigned char) { }
		/** Invoked just to desactivate all functions. very specific. */
		virtual void desactivateAllFunctions () { }

		/** Some times there cuould be something in the temporal mempory,
			so this instruction is to flush it! .*/
		void flush () { _printerFile.flush (); }

		/** 
			The attributes added are the ones of the parent class plus:
			Configuration		= InfoStructure : The configuration of the printer emulation.
			PRINTERFILE			= Attribute	: The name of the file where to print out the result. 
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		// All these methods are invoked from the method printChar..
		virtual void firstTimePrinting (unsigned char) { }
		virtual void beforePrintingChar (unsigned char) { }
		/** This method controls the implications of the caharacter being printed out. \n
			It can be overloaded but there is a default definition that consists on: \n
			1.- Check (isControlChar () method) whether the character is or not a control char, appart of newLine or tab.
			2.- If it were that, the method invoked will be manageControlChar (). \n
				Usually this method will activate or desactivate "flags" that will be used when printing a normal char. \n
				or will change the position of the head of the printer. \n
				So that's the reason to return how much to add to the position of the head of the printer or to new pages. \n
			3.- If it were a newLine char, the method invoked will be printLineNew (). \n
			4.- When the end of the page is reached, the method serNewPage () is invoked. \n
			5.- and, finally, in other case, the method invoked will be printOtherChar ();
			All of then can be overloaded for more flexibility. \n
			The tab is simulated, printing out "spaces".
			The method finally returns the number of positions to add to the head of the printer and the page. */
		virtual std::tuple <short, short, short> printCharImplementation (unsigned char chr);
		virtual void afterPrintingChar (unsigned char) { }

		// All methods invoked from printCharImplementation () default (algorithm)
		virtual bool isControlChar (unsigned char) { return (false); } // By default, there is no control chars...
		virtual std::tuple <short, short, short> manageControlChar (unsigned char chr) 
							{ return (std::make_tuple (0, 0, 0)); } // Nothing to add by default...
		virtual void closePage (unsigned short) { } // Again, byt detault it doesn't do anything...
		virtual void setNewPage (unsigned short) { } // Nothing by default...
		virtual bool isNormalChar (unsigned char) { return (true); }

		// The methods to print out the chars, invoked from printCharImplementation () default (algorithm)
		/** Just print out the new line instructions (if needed), 
			but sometimes print out something (depending on the funtion active) 
			has no effect in the position of the head of the printer, so the method returns really whether the head has to be moved forward or not. */
		virtual bool printNewLine () { _printerFile << std::endl; return (true); } // By default uses the c++ standard...
		/** Somethimes when printing a normal char the head of the printer can move forward several positions actually.
			So the method returns the number of positions that the head of the printer has to move forward. \n
			If the limit of the line were overpassed while printing out, the excess might be lost. */
		virtual unsigned short printNormalChar (unsigned char chr)
							{ _printerFile << std::string (1, (char) chr); return (1);  } // Again, the default output...

		protected:
		/** The configuration of the printer. 
			It is usefull to define things like the size of the characters, or the max size of the paper. */
		Configuration _configuration;
		/** The size of the paper being used.
			It can be changed while printing but the result is unpredictible. */
		Paper _paper;
		/** File's name where the printing is sent out. */
		std::string _printerFileName;
		/** The channel active. */
		unsigned char _activeChannel [2]; // One for IN (0) and other for OUT (1)

		// Implementation
		/** The file where to print out the data.
			It is open at construction time and closed when destroy method is invoked. */
		std::ofstream _printerFile;
		/** The number of page being printed out. */
		mutable unsigned short _page; // It might be changed anytime prinChar method is invoked...
		/** Where the printer head is in the page in every moment. */
		mutable unsigned short _posX, _posY; // It might be changed any time printChar method is invoked...
		/** To indicate whether a first char has or not been printed out yet. */
		mutable bool _firstChar;
	};

	/** To simulate a very basic printer,
		Just the very basic ASCII alphanum chars are printed out. \n
		Special characters are not taking into account. */
	class BasicMatrixPrinterEmulation : public MatrixPrinterEmulation
	{
		public:
		BasicMatrixPrinterEmulation (unsigned char cPL, // The paper here is not important...
				const std::string& pFN = "MatrixPrinter.txt")
			: MatrixPrinterEmulation (Configuration (), Paper (), pFN)
							{ _configuration._charsPerLine = cPL; 
							  _configuration._description = "Basic Matrix Emulation"; }
	};

	/** To simulate a basic printing based on postscript
		that can be converted into a PDF file later. \n
		What special characters could do is not taking into account, 
		just only the ASCII alphanum are printed out */
	class PostscriptMatrixPrinterEmulation : public MatrixPrinterEmulation
	{
		public:
		PostscriptMatrixPrinterEmulation (const Configuration& cfg, const Paper& p,
				const std::string& pFN = "MatrixPrinter.ps") // The extension is to determine that it is postscript file...
			: MatrixPrinterEmulation (cfg, p, pFN)
							{ _configuration._description = "Basic Postscript Matrix Emulation"; }

		protected:
		/** The main postscript routines are copied.
			The fine defining everything is: PSBasic.ps. \n
			It has to exist in the directory where the application is running. */
		virtual void firstTimePrinting (unsigned char chr) override;

		/** Again,using postscript instructions. */
		virtual bool printNewLine () override
							{ printerFile () << "newLine" << std::endl; return (true); } // The postcript instruction in defined in 
		virtual unsigned short printNormalChar (unsigned char chr) override
							{ printerFile () << std::to_string ((int) chr) + " printCharAtHead" << std::endl; return (1); }
	};
}

#endif
  
// End of the file
/*@}*/
