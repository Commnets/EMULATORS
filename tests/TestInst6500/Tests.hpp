#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <CORE/incs.hpp>
#include <F6500/incs.hpp>

/** To run a test with external definitions. 
	The definitions are made in files. */
class Test
{
	public:
	Test (const std::string& t, 
		  unsigned int mtpc = std::numeric_limits <unsigned int>::max ());

	const std::vector <std::string>& errors () const
							{ return (_errors); }
	const std::vector <std::string>& warnings () const
							{ return (_warnings); }
	const std::vector <std::string>& noimplemented () const
							{ return (_noimplemented); }

	size_t totalTests () const
							{ return (_testsData.size ()); }

	/** To run the test.
		The CPU is destroyed when finishes. 
		A memory has to be attached to the CPU. */
	void runTest (F6500::C6500* cpu);

	bool operator ! () const
							{ return (!_errors.empty ()); }

	private:
	// Implementation
	using Tokens = std::vector <std::string>;
	using TestTokens = std::vector <Tokens>;
	
	struct MemoryPosition
	{
		unsigned int _position;
		unsigned char _value;
	};

	struct TestStatus
	{
		unsigned short _PC;
		unsigned char _A, _X, _Y;
		unsigned char _ST;
		unsigned char _SP;
		std::vector <MemoryPosition> _memoryData;
	};
	
	struct Cycle
	{
		enum class Type
		{
			_MR, _MW
		};
	
		Type _type;
		MemoryPosition _memoryData;
	};
	
	using Cycles = std::vector <Cycle>;
	
	struct TestData
	{
		std::string _file;
		unsigned int _line;
		std::string _name;
		TestStatus _statusIn, _statusOut;
		Cycles _cycles;
	};
	
	using TestsDataMap = std::map <std::string, TestData>;

	unsigned short fromHexa (const std::string& str);

	private:
	TestsDataMap readTests (const std::string& t, unsigned int mtpc) const;
	TestsDataMap readTest (const std::string& t, unsigned int mtpc, unsigned int ict) const;

	private:
	TestsDataMap _testsData;
	unsigned int _maxTestPerCode;

	// Implementation
	mutable std::vector <std::string> _errors;
	mutable std::vector <std::string> _warnings;
	mutable std::vector <std::string> _noimplemented;
};
