#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>

/** To run a test with external definitions. 
	The definitions are made in files. */
class Test
{
	public:
	Test (const std::string& tIn, const std::string& tOut);

	const std::vector <std::string>& errors () const
							{ return (_errors); }
	const std::vector <std::string>& warnings () const
							{ return (_warnings); }
	const std::vector <std::string>& noimplemented () const
							{ return (_noimplemented); }

	size_t totalTests () const
							{ return (_testsIn.size ()); }

	/** To run the test.
		The CPU is destroyed when finishes. 
		A memory has to be attached to the CPU. */
	void runTest (FZ80::CZ80* cpu);

	bool operator ! () const
							{ return (!_errors.empty ()); }

	private:
	// Implementation
	using Tokens = std::vector <std::string>;
	using TestTokens = std::vector <Tokens>;
	
	struct TestStatus
	{
		struct Memory
		{
			Memory ()
				: _position (0), _data () 
								{ }
	
			unsigned short _position;
			std::vector <unsigned char> _data;
		};
	
		TestStatus ()
			: _AF (0), _BC (0), _DE (0), _HL (0), 
			  _AFP (0), _BCP (0), _DEP (0), _HLP (0), 
			  _IX (0), _IY (0), _SP (0), _PC (0), _MEMPTR (0),
			  _I (0), _R (0), _IM (0),
			  _IFF1 (false), _IFF2 (false), _halted (false),
			  _tStates (0), _memoryStart (0),
			  _data ()
								{ }
	
		unsigned short _AF, _BC, _DE, _HL, _AFP, _BCP, _DEP, _HLP, _IX, _IY, _SP, _PC, _MEMPTR;
		unsigned char _I, _R, _IM;
		bool _IFF1, _IFF2;
		bool _halted;
		unsigned short _tStates;
		unsigned int _memoryStart;
		std::vector <Memory> _data;
	};
	
	struct TestIn
	{
		TestIn ()
			: _name (""), _status ()
								{ }
	
		std::string _name;
		TestStatus _status;
	};
	
	using TestInMap = std::map <std::string, TestIn>;
	
	struct TestOut
	{
		std::string _name;
		
		struct Step
		{
			enum class Type
			{
				_MR, _MW, _MC, _PR, _PW, _PC
			};
	
			Step ()
				: _time (0),
				  _type (Type::_MC),
				  _position (0), _data (0)
								{ }
	
			unsigned char _time;
			Type _type;
			unsigned short _position;
			unsigned char _data;
		};
	
		using Steps = std::vector <Step>;
	
		Steps _steps;
		TestStatus _status;
	};
	
	using TestOutMap = std::map <std::string, TestOut>;

	unsigned short fromHexa (const std::string& str);
	Tokens getLineTokens (std::ifstream& f, bool& fF);
	TestStatus getTestStatus (const TestTokens& tT, size_t i);
	TestInMap readTestsIn (const std::string& n);
	TestOutMap readTestsOut (const std::string& n);

	private:
	TestInMap _testsIn;
	TestOutMap _testsOut;

	// Implementation
	std::vector <std::string> _errors;
	std::vector <std::string> _warnings;
	std::vector <std::string> _noimplemented;
};

