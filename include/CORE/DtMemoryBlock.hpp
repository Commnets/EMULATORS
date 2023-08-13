/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: DtMemoryBlock.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 01/11/2022 \n
 *	Description: Define the typical structure for a block memory.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_DTMEMORYBLOCK__
#define __MCHEMUL_DTMEMORYBLOCK__

#include <CORE/UBytes.hpp>
#include <CORE/Address.hpp>

namespace MCHEmul
{
	/** The structure is very useful to manage blocks of memory content. 
		The block of memory can represent many things, usually code or data associated. */
	class DataMemoryBlock;
	using DataMemoryBlocks = std::vector <DataMemoryBlock>;
	class DataMemoryBlock final
	{
		public:
		DataMemoryBlock () = default;

		DataMemoryBlock (const Address& a, const std::vector <UByte>& dt)
			: _name (""), _attributes (), _startAddress (a), _bytes (dt)
							{ }

		DataMemoryBlock (const Address& a, std::vector <UByte>&& dt) noexcept
			: _name (""), _attributes (), _startAddress (a), _bytes (std::move (dt))
							{ }

		// Get & Set methods
		/** The name. */
		const std::string& name () const
							{ return (_name); }
		void setName (const std::string& n)
							{ _name = n; }
		/** The attributes. */
		bool existAttribute (const std::string& aN) const
							{ return (_attributes.find (aN) != _attributes.end ()); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i != _attributes.end ()) ? (*i).second : _NOATTRIBUTE); }
		void setAttribute (const std::string& aN, const std::string& aV)
							{ _attributes [aN] = aV; }
		/** The address where the info should be start from. */
		const Address& startAddress () const
							{ return (_startAddress); }
		const std::vector <UByte>& bytes () const
							{ return (_bytes); }
		size_t size () const
							{ return (_bytes.size ()); }
		Address endAddress () const
							{ return (startAddress () + size ()); }

		/** Clear the bytes. */
		void clear ()
							{ _bytes.clear (); }

		/** To add bytes. */
		void addByte (const UByte& by)
							{ _bytes.emplace_back (by); }
		void addBytes (const std::vector <UByte>& bys)
							{ for (const auto& i : bys) _bytes.emplace_back (i); }
		void addBytes (const UBytes& by)
							{ addBytes (by.bytes ()); }

		/** Save the memory block.
			It receives only the size of the address at the header and whether that address
			has be kept into big or little endian. */
		bool save (const std::string& fN, size_t nB, bool bE = true) const;

		// Loading info...
		/** To load a binary file. \n
			The binary file has inside the address where to start to put the data into at the very first bytes loaded. \n
			If there were errors the variable "e" will be true. \n
			The method needs also the length of that address in the header and 
			whether it is store in big endian or little endian format. */
		static DataMemoryBlock loadBinaryFile (const std::string& fN, bool& e, size_t nB, bool bE = true);
		/** To load a file made up of block of data. \n
			This type of file is made up of several data memory blocks. \n
			If there were errors the variable "e" will be true.
			The method needs also the length of that address in the header and 
			whether it is store in big endian or little endian format. */
		static DataMemoryBlocks loadBlocksFile (const std::string& fN, bool& e, size_t nB, bool bE = true);

		// Saving blocks info...
		/** The structure of the file will repeat per block and will include after the address 
			4 bytes indicating the number of data bytes that follows. */
		static bool saveDataBlocks (const std::string& fN, const DataMemoryBlocks& blks, size_t nB, bool bE = true);

		private:
		std::string _name; // The name is very optional...
		Attributes _attributes; // The attributes are also optional...
		Address _startAddress;
		std::vector <MCHEmul::UByte> _bytes;

		// Implementation
		static const std::string _NOATTRIBUTE;
	};

	/** Just to represent complex info related with data. 
		It is not used too much except in the io functions. */
	struct ExtendedDataMemoryBlocks
	{
		std::string _name;
		Attributes _attributes;
		DataMemoryBlocks _data;
	};

	/** 
		A class to format like a text a datamemory block.
		according with a definition file. \n 
		The definition file must have the lines with following structrure:[COMMAND] TEXT. \n
		Where COMMAND can be: \n
		=> CT=NAME@INITIALVALUE@INCREMENT \n
		To define a integer counter. \n
		The counter has a name, and initial value and an increment. \n
		Anytime the counter is used it will be incremented in INCREMENT units. \n
		=> US=COUNTER NAME \n
		To use the COUNTER named NAME and increment its value in INCREMENT units (according with its definition. \n
		If a no defined COUNTER were tried to use, a error would be printed out. \n
		=> VR=VARIABLE NAME@OUTPUT FORMAT \n
		To print out the value of a variable of the data block and the data formate used. \n
		The data formats can be: 0 = _DECIMAL, 1 = _BINARY, 2 = _OCTAL, 3 = _HEXA. \n
		The variables now treated are: \n
		=> SIZE		The number of bytes of the block code. \n
		=> ADDRESS	The initial address of the block code. \n
		=> FADDRESS	The final address of the block code. \n
		=> DT=SEP@NUMBER OF ELEMENTS@OUTPUT FORMAT \n
		This command generates the list of bytes of the data block in sets of NUMBER OF ELEMENTS separated by SEP
		and using the output format OUTPUT FORMAT. \n
		This result of this commnad is affected direclty by all other formatters defined before in the same line. \n
		All previous variables used together could as an example 
		to create a line in BASIC code to load the data of a block: \n
		----
		Example:
		[CT=LN@10@10] \n
		[US=LN]REM TEST \n
		[US=LN]FORI:=0TO[VR=SIZE]STEP1:READA:POKE[VR=ADDRESS]+I,A:NEXTI \n
		[US=LN]DATA[DT=,@16]
		----
	*/
	class DataMemoryBlockTextFormatter final
	{
		public:
		/** Receives the Data Memory Block to format and the file with the instructions. \n
			A default name is provided. \n
			If the file didn't exit a default formatter structure would be provided. */
		DataMemoryBlockTextFormatter (const std::string& fN = ".\\blkFormatter.fmt");

		~DataMemoryBlockTextFormatter ()
						{ for (auto& i : _fmtElements) 
							delete (i); }

		std::string format (const DataMemoryBlock& dB) const;

		private:
		/** To format the data memory block a set of formatter are used */
		class FormatterElement
		{
			public:
			virtual std::string format (const DataMemoryBlock& dB) const = 0;

			protected:
			// Routine used in many classes inheriting this one.
			/** f can b 0 = _DECIMAL, 1 = _BINARY, 2 = _OCTAL, 3 = _HEXADECIMAL. 
				If it is not in that range, the value used is 3. */ 
			static std::string convertNumberToString (unsigned int n, unsigned int f);
		};

		/** To simplify the use of a list of formatters. */
		using FormatterElements = std::vector <FormatterElement*>;

		/** A simple formatter just to print out a text. 
			No calculus at all is done. */
		class TextFormatterElement final : public FormatterElement
		{
			public:
			/** The parameter received is just the text to print out. 
				It can also be made of special characters to include i.e. new lines or tabulators. */
			TextFormatterElement (const std::string& txt)
				: _text (txt)
						{ }

			virtual std::string format (const DataMemoryBlock& dB) const override
						{ return (_text); }

			private:
			const std::string _text;
		};

		/** To define a counter. */
		class CounterFormatterElement final : public FormatterElement
		{
			public:
			/** The parameters received are: 
				The name of the counter, 
				its initial value 
				and the incremental step that will be used when actualized. */
			CounterFormatterElement (const std::string& nC, unsigned int iV, unsigned int st)
				: _name (nC),
				  _initialValue (iV),
				  _actualValue (iV),
				  _step (st)
							{ _counters.insert (Elements::value_type (_name, this)); }

			~CounterFormatterElement ()
							{ _counters.erase (_name); }

			/** To manage the vaue of the counter. */
			void initialize ()
							{ _actualValue = _initialValue; }
			unsigned int value () const
							{ return (_actualValue); }
			void increment ()
							{ _actualValue += _step; }

			/** The definition of the counter doesn't generate any putput.
				It is just an internal command. */
			virtual std::string format (const DataMemoryBlock& dB) const override
							{ return (""); } // Does nothing when formatting...

			/** To locate a counter. 
				The result could be null when the counter doesn't exist. */
			static CounterFormatterElement* findCounter (const std::string& n)
							{ Elements::const_iterator i = _counters.find (n); 
							  return (i == _counters.end () ? nullptr : (*i).second); }

			private:
			const std::string _name;
			const unsigned int _initialValue;
			const unsigned int _step;

			// Implementation
			unsigned int _actualValue;
			using Elements = std::map <std::string, CounterFormatterElement*>;
			static Elements _counters;
		};

		/** To use the counter, 
			what means to get its value as a text and to increment its internal value.
			in the vaue defined by step. */
		class UseCounterFormatterElement final : public FormatterElement
		{
			public:
			/** The only parameter needed is the counter refered, that cannot be null. */
			UseCounterFormatterElement (CounterFormatterElement* ct)
				: _counter (ct)
							{ assert (_counter != nullptr); }

			virtual std::string format (const DataMemoryBlock& dB) const override
							{ std::string result = std::to_string (_counter -> value ());
							  _counter -> increment ();
							  return (result); } 

			private:
			CounterFormatterElement* _counter;
		};

		/** To get a variable from the data memory block and print out its value. */
		class VariableFormatterElement final : public FormatterElement
		{
			public:
			/** The parameters are the name of the variable and the output format,
				that you already know that can be 0 = _DECIMAL, 1 = _BINARY, _HEXADECIMAL = 2, _OCTAL = 3. */
			VariableFormatterElement (const std::string& vr, unsigned int oF)
				: _variable (vr),
				  _outputFormat (oF)
						{ assert (_outputFormat >= 0 && _outputFormat <= 3); }

			virtual std::string format (const DataMemoryBlock& dB) const override;

			private:
			const std::string _variable;
			const unsigned int _outputFormat;
		};

		/** The classes inheriting from this one, 
			generates a list of strings that (one by one) are affected for the list of formatters 
			also received as parameter at construction time, generating the final result. */
		class GroupFormatterElement : public FormatterElement
		{
			public:
			GroupFormatterElement (const FormatterElements& fe)
				: _formatters (fe)
							{ }

			~GroupFormatterElement ()
							{ for (const auto& i : _formatters)
								delete (i);}

			/** Invokes the method "generateLinesFrom", defined below. */
			virtual std::string format (const DataMemoryBlock& dB) const override final;

			protected:
			virtual Strings generateLinesFrom (const DataMemoryBlock& dB) const = 0;

			protected:
			FormatterElements _formatters;
		};
		
		/** To format the bytes of the memory block. 
			Probably it is the most important type of formatter. */
		class DataFormatterElement final : public GroupFormatterElement
		{
			public:
			
			/** 
			  *	The data needed are:
			  *	@param sep	The separator between the different elements.
			  *	@param ne	The number of elements per line,
			  *	@param fe	The list of the formatters to apply to each of the final lines created. 
			  */
			DataFormatterElement (const std::string& sep, unsigned int ne,
				unsigned int of, const FormatterElements& fe = FormatterElements ())
				: GroupFormatterElement (fe),
				  _separator (sep),
				  _numberElements (ne),
				  _outputFormat (of)
						{ assert (_outputFormat >= 0 && _outputFormat <= 3); }

			private:
			virtual Strings generateLinesFrom (const DataMemoryBlock& dB) const override;

			private:
			const std::string _separator;
			const size_t _numberElements;
			const unsigned int _outputFormat;
		};

		/** To create the right formatter element.
			The method reveives also the list of the formatter parameters already created in the line. 
			Notice that the parameter is passed as reference because it could be modified.
			When a group formatter is created, it will group the previous in the line. */
		FormatterElement* createFormatterElementFrom
			(const std::string& nC, const MCHEmul::Strings& vC, FormatterElements& fE);

		private:
		/** The list of all of them .*/
		FormatterElements _fmtElements;
	};
}

#endif
  
// End of the file
/*@}*/
