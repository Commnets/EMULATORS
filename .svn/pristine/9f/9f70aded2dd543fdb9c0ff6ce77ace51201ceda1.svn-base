/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: Parser.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/06/2021 \n
 *	Description: To read a file and understanding it.
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_PARSER__
#define __ASSEMBLER_PARSER__

#include <CORE/Incs.hpp>
#include <ASSEMBLER/Grammar.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** The parser reads the file using specific pieces 
			aimed to understand specific parts of the code. \n
			The parser (or the command parsers) works for a specific CPU (and it set of instructions). \n
			The parser is really a builder (design pattern). */
		class Parser;
		class CommandParser
		{
			public:
			friend Parser;

			CommandParser ()
				: _cpu (nullptr), _parser (nullptr)
							{ }

			virtual ~CommandParser ()
							{ }

			const CPU* cpu () const
							{ return (_cpu); }
			const Parser* parser () const
							{ return (_parser); }

			/** Maybe one of them could need it. */
			virtual void initialize ()
							{ }

			/** Determine whether a line can or not be parsed by this Command Parser. */
			virtual bool canParse (const std::string& l) const = 0;
			/** Parse the line, obviously when it is able. \n
				Wheen parsing the line being parsed and the code structure are modified. */
			virtual void parse (std::string& l, unsigned int lC, Semantic* s) const = 0;

			/** Invoked from the Parser's constructor. */
			void setCPU (const CPU* c)
							{ assert (c != nullptr); _cpu = c; }
			void setParser (const Parser* p)
							{ assert (p!= nullptr); _parser = p; }

			protected:
			const CPU* _cpu;
			const Parser* _parser;
		};

		/** To facilitate the use of a list of commands. */
		using CommandParsers = std::vector <CommandParser*>;

		/** To parser a comment.
			Th symbol defining the beginning of a comment can be redefined. */
		class CommentCommandParser final : public CommandParser
		{
			public:
			CommentCommandParser (unsigned char s = ';')
				: CommandParser (),
				  _symbol (s)
							{ }

			unsigned char symbol () const
							{ return (_symbol); }

			virtual bool canParse (const std::string& l) const override
							{ return (l [0] == _symbol); }
			virtual void parse (std::string& l, unsigned int, Semantic*) const override
							{ l = ""; /** Nothing after the comment is important. */}

			private:
			const unsigned char _symbol = ';'; // Adjusted at construction level
		};

		/** To parser a file inclusion.
			The symbol defining that can be changed. */
		class IncludeCommandParser final : public CommandParser
		{
			public:
			IncludeCommandParser (unsigned char s = '#')
				: CommandParser (),
				  _symbol (s)
							{ }

			virtual bool canParse (const std::string& l) const override
							{ return (l [0] == _symbol); }
			virtual void parse (std::string& l, unsigned int lC, Semantic* c) const override;

			private:
			const unsigned char _symbol = '#'; // Adjusted at construction level
		};

		/** To parser a macro. 
			The symbol defining the separation between definition and value can be redefined. */
		class MacroCommandParser : public CommandParser
		{
			public:
			MacroCommandParser (unsigned char s = '=')
				: CommandParser (),
				  _symbol (s)
							{ }

			virtual bool canParse (const std::string& l) const override
							{ size_t eP = l.find (_symbol); 
							  return (eP != std::string::npos && validLabel (trim (l.substr (0, eP)))); }
			virtual void parse (std::string& l, unsigned int lC, Semantic* s) const override;

			protected:
			const unsigned char _symbol = '=';
		};

		/** To parser an address macro.
			The symbol defining the separation between definition and value can be redefined. 
			Qhen parser the address assigned is incremented. */
		class StartingPointCommandParser final : public CommandParser
		{
			public:
			StartingPointCommandParser (unsigned char s = '=')
				: CommandParser (),
				  _symbol (s),
				  _lastStartingPointId (0)
							{ }

			virtual void initialize () override
							{ _lastStartingPointId = 0; }

			virtual bool canParse (const std::string& l) const override
							{ size_t eP = l.find (_symbol); 
							  return (eP != std::string::npos && trim (l.substr (0, eP)) == "*"); }
			virtual void parse (std::string& l, unsigned int lC, Semantic* s) const override;

			private:
			const unsigned char _symbol = '=';

			// Implementation
			mutable unsigned int _lastStartingPointId;
		};

		/** To parser a label. */
		class LabelCommandParser final : public CommandParser
		{
			public:
			LabelCommandParser (unsigned char s = ':')
				: CommandParser (),
				  _symbol (s),
				  _lastLabelId (0)
							{ }

			virtual void initialize () override
							{ _lastLabelId = 0; }

			virtual bool canParse (const std::string& l) const override
							{ size_t eP = l.find (_symbol); 
							  return (eP != std::string::npos && validLabel (trim (l.substr (0, eP)))); }
			virtual void parse (std::string& l, unsigned int lC, Semantic* s) const override;

			private:
			const unsigned char _symbol = ':';

			// Implementation
			mutable unsigned int _lastLabelId;
		};

		/** To parser a set of bytes. */
		class BytesCommandParser final : public CommandParser
		{
			public:
			BytesCommandParser ()
				: CommandParser (),
				  _lastBytesId (0)
							{ }

			virtual void initialize () override
							{ _lastBytesId = 0; }

			virtual bool canParse (const std::string& l) const override
							{ size_t eP = l.find (' '); 
							  return (eP != std::string::npos && upper (trim (l.substr (0, eP))) == "BYTES"); }
			virtual void parse (std::string& l, unsigned int lC, Semantic* s) const override;

			private:
			// Implementation
			mutable unsigned int _lastBytesId;
		};

		/** To parser an instruction. */
		class InstructionCommandParser final : public CommandParser
		{
			public:
			InstructionCommandParser ()
				: CommandParser (),
				  _lastInstructionId (0)
							{ }

			virtual void initialize () override
							{ _lastInstructionId = 0; }

			virtual bool canParse (const std::string& l) const override;
			virtual void parse (std::string& l, unsigned int lC, Semantic* s) const override;

			private:
			// Implementation
			mutable unsigned char _lastInstructionId;
		};

		/** Now it is time to define the parser itself. \n
			To convert a file text (or text) into an internal structure. \n
			The parser is a line parser. That is, it is only able to parser a line. \n
			There can be added specific instruction parsers but always a comment command parser must exist. \n
			It is used to determine whether a line finishes or not. */
		class Parser final
		{
			public:
			Parser (const CPU* c, const CommandParsers& lP = // With the standard line parsers...
					{ new CommentCommandParser, new IncludeCommandParser, 
					  new MacroCommandParser, new StartingPointCommandParser, 
					  new LabelCommandParser, new InstructionCommandParser });

			Parser (Parser&) = delete;

			Parser& operator = (const Parser&) = delete;

			/** The parser doesn't own the instructions but the CPU, but the line parsers. */
			~Parser ()
							{ for (auto i : _commandParsers) delete (i); }

			const CPU* cpu () const
							{ return (_cpu); }
			const CommandParsers& commandParsers () const
							{ return (_commandParsers); }
			unsigned char commentSymbol () const;

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
			  *	; Means comment. After that, nothing will be taken into account. \n
			  * MACROS & LABELS are represented using characteres (upper and lower case) and numbers, but never starting with number. \n
			  * NUMBERS AND DIRECTIONS can be represented using decimal, octal (starting with 0) and hexadecimal (with $) numbers. \n
			  * * = xxxx will identify the address from which the code after will be inserted!
			  */
			Semantic* parse (const std::string& fN) const;

			Errors errors () const
							{ return (_errors); }

			/** To simplify checking whether there was or not an error parsing. */
			bool operator ! () const
							{ return (!_errors.empty ()); }

			private:
			/** To read a file and convert it into Lines to be treaten later. */
			std::vector <std::string> readLines (const std::string& fN) const;

			private:
			const CPU* _cpu;
			CommandParsers _commandParsers;

			// Implementation
			mutable Errors _errors;
			mutable unsigned char _commentSymbol;
		};
	}
}

#endif
  
// End of the file
/*@}*/