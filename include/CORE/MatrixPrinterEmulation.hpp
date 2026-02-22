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
				: _wChar (0), _hChar (0),
				  _charSet (),
				  _charsPerLine (0),
				  _charNewLine ('\n'),
				  _charTab ('\t'), _numSpacesTabChar (4)
							{ }

			Configuration (unsigned char wC, unsigned char hC,
				const CharSetDefinition& d, unsigned short cL, char cNL, char cT, unsigned short sT)
				: _wChar (wC), _hChar (hC),
				  _charSet (d),
				  _charsPerLine (cL),
				  _charNewLine (cNL),
				  _charTab (cT), _numSpacesTabChar (sT)
							{ }

			/** 
				The attributes added are: \n
				CHARWIDTH		= Attribute: The width in pixels per char (in normal size). \n
				CHARHEIGHT		= Attribute: The heigth in pixels per char (in normal size).  \n
				CHARSPERLINE	= Attribute: The number of chars (normal size) per line.
			  */
			InfoStructure getInfoStructure () const;

			/** The size in bits per char. 
				Usually 8x8 but in some systems (like Commodore's) can be 6x7. */
			unsigned char _wChar;
			unsigned char _hChar;
			/** The information about all chars. */
			CharSetDefinition _charSet;

			/** How many characters per line in the normal size. */
			unsigned short _charsPerLine;

			// Special chars to guide the printer...
			/** The char to change the line being printed out. */
			char _charNewLine;
			char _charTab;
			unsigned short _numSpacesTabChar;
		};

		MatrixPrinterEmulation (const Configuration& cfg, const Paper& p, 
			const std::string& pFN);

		virtual ~MatrixPrinterEmulation ()
							{ finalize (); }

		/** See the configuration... */
		const Configuration& configuration () const
							{ return (_configuration); }
		/** Manage the size of the paper. */
		const Paper& paper () const
							{ return (_paper); }
		void setPaper (const Paper& p)
							{ _paper = p; }

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
		/** Move the head x positions respect the current position. \n
			Can be moved forward and backward. Never less than the 0,0. \n
			The method returns the number of lines incremented or decremented or 0 if none. \n
			For more detail about the final position, use the method headPosition. */
		short moveHeadFromX (short px);
		/** Move the head y positions respect the current position. \n
			Can also be moved forward and backward, but never less than 0,0. \n
			In this case is is not needed to return anything. */
		void moveHeadFromY (short py);
		/** Move the head to a specific position. */
		void moveHeadTo (unsigned short px, unsigned short py)
							{ _posX = px; _posY = 0; }

		/** The way a char is printed out will depend on the specific char and the specific matriz printer emulation. \n
			First time printing anything something special could be done,
			and before and after printer anything again something spwecial could be done. 
			If when printing the max number of chars per line was reached, 
			the head of printer will be moved to another line. (easy, because everything is printed out char by char) */
		void printChar (unsigned char chr);
		/** Printing a string. */
		void printString (const std::string& str)
							{ for (const auto& i : str) printChar (i); }

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
		virtual void firstTimePrinting (unsigned char) { }
		virtual void beforePrintingChar (unsigned char) { }
		/** Returns true when the printer really printed out something. 
			It could happen that the char send represented a control character with no implications in the 
			position of the head of the printer. */
		virtual bool printCharImplementation (unsigned char) = 0;
		virtual void afterPrintingChar (unsigned char) { }

		protected:
		/** The configuration of the printer. 
			It is usefull to define things like the size of the characters, or the max size of the paper. */
		Configuration _configuration;
		/** The size of the paper being used.
			It can be changed while printing but the result is unpredictible. */
		Paper _paper;
		/** File's name where the printing is sent out. */
		std::string _printerFileName;

		// Implementation
		std::ofstream _printerFile;
		/** The number of page being printed out. */
		mutable unsigned short _page;
		/** Where the printer head is in every moment. */
		mutable unsigned short _posX, _posY; // It is changed any time printChar method is invoked...
		/** To indicate whether a first char has or not been printed out. */
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
							{ _configuration._charsPerLine = cPL; }

		protected:
		// Only alphanumeric characters are printed out...
		// ...and also the characters basic control characters: line feed = carry return
		virtual bool printCharImplementation (unsigned char chr) override;
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
							{ }

		protected:
		/** The main postscript routines are copied. */
		virtual void firstTimePrinting (unsigned char chr) override;
		virtual bool printCharImplementation (unsigned char chr) override;
	};
}

#endif
  
// End of the file
/*@}*/
