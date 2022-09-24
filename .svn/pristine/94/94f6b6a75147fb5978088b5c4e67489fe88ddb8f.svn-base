/** \ingroup language */
/*@{*/

/**	
 *	@file	
 *	File: Parser.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: To read a file and convert it into set of data.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_PARSER__
#define __MCHEMUL_PARSER__

#include <core/CPU.hpp>
#include <language/Instruction.hpp>
#include <core/Address.hpp>

namespace MCHEmul
{
	class Mempory;

	/** To read a file or a string and convrrting into data loaded in the memoty. */
	class Parser
	{
		public:
		/** The possible errors whn parsing. */
		enum class ParserError
		{
			_NOERROR,
			_CODENOTFOUND,
			_BADNUMBERPARAMETERS,
			_BADTYPEPARAMETERS,
			_BADDEFINEDLINE
		};

		using Lines = std::vector <std::string>;
		using Labels = std::map <std::string, Address>;
		using Macros = std::map <std::string, std::vector <MCHEmul::UByte>>;

		/** An structure to keep the info about a simple line of code. */
		struct CodeLine
		{
			public:
			CodeLine ()
				: _address (), _code ()
							{ }

			CodeLine (const CodeLine&) = default;

			CodeLine& operator = (const CodeLine&) = default;

			friend std::ostream& operator << (std::ostream& o, const CodeLine& cL);

			Address _address;
			std::vector <UByte> _code;
		};

		/** The code is made up of many code lines. */
		using Code = std::vector <CodeLine>;

		Parser () = delete;

		/**
		  * Type of code that is is accepted:
		  * The example has been written using the machine languaje of the Commodore 64
		  *	; Simple test that everything works \n
		  *	; By Ignacio Cea \n
		  * \n
		  *	; MACROS \n
		  *	FOREGROUND = $D020 \n
		  *	BACKGROUND = $D021 \n
		  * \n
		  *	* = $C000 \n
		  * \n
		  *	; Now the code \n
		  * \n
		  *	START:			LDA #$00 ;Load accumulator \n
		  *					STA BACKGROUND \n
		  *					STA FOREGROUND \n
		  *					BNE START \n
		  * \n
		  *	; Very simple
		  * Rules:
		  *	; Means comment. After that, nothing is taken into account. \n
		  * MACROS & LABELS are represented using characteres (upper and lower cas) and numbers, but never starting with number \n
		  * NUMBERS AND DIRECTIONS can be represented using decimal, octal (starting with 0) and hexadecimal (with $) numbers. \n
		  * * = xxxx will identify the starting address!
		  */
		Parser (CPU* c)
			: _instructions (c -> instructions ()),
			  _architecture (c -> architecture ()),
			  _parserError (ParserError::_NOERROR), _lastParserLine (0)
							{ }

		Parser (Parser&) = delete;

		Parser& operator = (const Parser&) = delete;

		/** The parser doesn't own the instructions but the CPU. */
		virtual ~Parser ()
							{ }

		/** To parser the file. The code is returned. */
		Code parse (const std::string& fN) const;
		/** To load a code into the memory (if possible). \n
			Returns true when ok and false in other case. */
		bool loadInMemory (const std::string& fN, Memory* m);
		/** To know whether an error has or not happened after parsing. */
		ParserError parserError () const
							{ return (_parserError); }
		unsigned int lastParserLine () const
							{ return (_lastParserLine); }

		/** To simplify checking whether there was or not an error parsing. */
		bool operator ! () const
							{ return (_parserError == ParserError::_NOERROR); }

		protected:
		/** To read a file and convert it into Lines to be treaten later. */
		Lines readLines (const std::string& fN) const;
		/** To convert a line into the part of the code. */
		Code parseLine (std::string& l, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;

		// Implementation
		Code parseCommentInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseIncludeInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseMacroInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseLabelInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseOpcodeInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;

		protected:
		Instructions _instructions;
		CPUArchitecture _architecture;

		// Implementation
		mutable ParserError _parserError;
		mutable unsigned int _lastParserLine;
	};
}

#endif
  
// End of the file
/*@}*/