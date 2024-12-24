#include "Tests.hpp"
#include <nlohmann/json.hpp>
#include <assert.h>
#include <fstream>

Test::Test (const std::string& t, unsigned int mtpc)
	: _testsData (readTests (t, mtpc))
{
	// Nothing else to do...
}

void Test::runTest (F6500::C6500* cpu)
{
	assert (cpu != nullptr && 
			cpu -> memoryRef () != nullptr);

	// Start the tests...
	for (Test::TestsDataMap::const_iterator i = _testsData.begin (); 
		i != _testsData.end (); i++)
	{
		// Set the CPU situation...
		cpu -> accumulator ().set ({ (*i).second._statusIn._A });
		cpu -> xRegister ().set ({ (*i).second._statusIn._X });
		cpu -> yRegister ().set ({ (*i).second._statusIn._Y });

		// ...the status register...
		cpu -> statusRegister ().set ({ (*i).second._statusIn._ST });

		// ...the stack pointer...
		cpu -> memoryRef () -> stack () -> setPosition ((int) (*i).second._statusIn._SP);
		cpu -> memoryRef () -> stack () -> setNotUsed (false); // it is considered already used!

		// ...the content of the memory...
		for (const auto& j : (*i).second._statusIn._memoryData)
			cpu -> memoryRef () -> set (MCHEmul::Address (2, j._position), MCHEmul::UByte (j._value));

		// ...the program counter...
		cpu -> programCounter ().setAddress (MCHEmul::Address (2, (unsigned int) (*i).second._statusIn._PC));

		// Do the Test
		unsigned int nInst = 
			MCHEmul::UInt (
				cpu -> memoryRef () -> values (
					cpu -> programCounter ().asAddress (), cpu -> architecture ().instructionLength ()),
				cpu -> architecture ().bigEndian ()).asUnsignedInt ();
		MCHEmul::Instruction* inst = nullptr;
		if ((inst = cpu -> instruction (nInst)) == nullptr)
		{
			_errors.emplace_back ("Instruction not implemented:" + std::to_string (nInst));

			continue;
		}

		inst -> execute (cpu, 
			cpu -> memoryRef (), cpu -> memoryRef () -> stack (), &cpu -> programCounter ());

		// Verify the result...
		// First whether the internal regisrters have what it supposed they should have!
		Test::TestStatus tOut = (*i).second._statusOut;
		bool eI1 = 
			cpu -> accumulator ().values ()[0] == tOut._A &&
			cpu -> xRegister ().values ()[0] == tOut._X &&
			cpu -> yRegister ().values ()[0] == tOut._Y;
		bool eI2 =
			cpu -> statusRegister ().values ()[0] == tOut._ST;
		// Then the stack pointer...
		bool eI3 =
			cpu -> memoryRef () -> stack () -> position () ==  (int) tOut._SP;
		// The the content of the memory...
		bool eI4 = true;
		for (const auto& j : tOut._memoryData)
			eI4 &= (cpu -> memoryRef () -> value (MCHEmul::Address (2, (unsigned int) j._position)) == j._value);
		// ...and also the program counter...
		bool eI5 = 
			cpu -> programCounter ().internalRepresentation () == tOut._PC;

		// If after all testings, there is no error...
		if (!(eI1 && eI2 && eI3 && eI4 && eI5))
		{
			auto cpuRegs = [&]() -> std::string 
				{ 
					return ("A=" + std::to_string ((int) cpu -> accumulator ().values ()[0].value ()) + " " +
							"X=" + std::to_string ((int) cpu -> xRegister ().values ()[0].value ()) + " " +
							"Y=" + std::to_string ((int) cpu -> yRegister ().values ()[0].value ()));
				};

			auto testRegsOut = [](const Test::TestData& t) -> std::string
				{
					return ("A=" + std::to_string ((int) t._statusOut._A) + " " +
							"X=" + std::to_string ((int) t._statusOut._X) + " " +
							"Y=" + std::to_string ((int) t._statusOut._Y));
				};

			auto diffMemoryOut = [&](const Test::TestData& t) -> std::string
				{
					bool f = true;
					MCHEmul::UByte val;
					std::string result;
					for (const auto& j : t._statusOut._memoryData)
					{
						if ((val = cpu -> memoryRef () -> value (MCHEmul::Address (2, (unsigned int) j._position))) != j._value)
						{
							result += (!f ? "," : "") + 
								std::string ("Address=") + 
								std::to_string (j._position) + "(" + 
									std::to_string (val.value ()) + " vs " + std::to_string (j._value) + ")";
							f = false;
						}
					}

					return (result);
				};

			std::string sInst = inst -> asString ();
			std::cout << "Error:" << inst -> asString () << std::endl;
			if (!eI1) _errors.emplace_back 
				("Error in test " + ((*i).first) + ", file:" + (*i).second._file + 
					", line:" + std::to_string ((*i).second._line) + ". " + 
						inst -> asString () + " in Registers (" + cpuRegs () + " vs " + testRegsOut ((*i).second) + ")");
			if (!eI2) _errors.emplace_back 
				("Error in test " + ((*i).first) + ", file:" + (*i).second._file +
					", line:" + std::to_string ((*i).second._line) + ". " + 
						inst -> asString () + " in Status Register (" +
							std::to_string ((int) cpu -> statusRegister ().values ()[0].value ()) + " vs " + 
							std::to_string ((int) (tOut._ST)) + ")");
			if (!eI3) _errors.emplace_back 
				("Error in test " + ((*i).first) + ", file:" + (*i).second._file +
						", line:" + std::to_string ((*i).second._line) + ". " + 
						inst -> asString () + " in Program Stack Pointer (" +
							std::to_string (cpu -> memoryRef () -> stack () -> position ()) + " vs " + 
							std::to_string ((int) tOut._SP) + ")");
			if (!eI4) _errors.emplace_back 
				("Error in test " + ((*i).first) + ", file:" + (*i).second._file +
					", line:" + std::to_string ((*i).second._line) + ". " + 
						inst -> asString () + " in Memory (" + diffMemoryOut ((*i).second) + ")");
			if (!eI5) _errors.emplace_back 
				("Error in test " + ((*i).first) + ", file:" + (*i).second._file +
					", line:" + std::to_string ((*i).second._line) + ". " + 
						inst -> asString () + " in Program Counter (" + 
							std::to_string (cpu -> programCounter ().internalRepresentation ()) + " vs " +
							std::to_string (tOut._PC) + ")");
		}
	}

	delete (cpu);
}

unsigned short Test::fromHexa (const std::string& str)
{
	unsigned short result = 0;
	std::istringstream ss (str);
	ss >> std::hex >> result;
	return (result);
}

// ---
Test::TestsDataMap Test::readTests (const std::string& t, unsigned int mtpc) const
{
	Test::TestsDataMap result;

	MCHEmul::Strings fileNames;
	std::ifstream file (t);
	if (!file)
	{
		return (result);
	}

	std::string line;
	while (std::getline (file, line)) 
		fileNames.emplace_back (line);

	file.close ();

	unsigned int ct = 0;
	for (const auto& i : fileNames)
	{
		std::cout << "Reading: " + i + " tests" << std::endl;

		Test::TestsDataMap pResult = std::move (readTest (i, mtpc, ct));
		result.insert (pResult.begin (), pResult.end ());
		ct += mtpc;
	}
		
	return (result);
}

// ---
Test::TestsDataMap Test::readTest (const std::string& t, unsigned int mtpc, unsigned int ict) const
{
	Test::TestsDataMap result;

	std::ifstream file = std::ifstream (t);
	if (!file)
	{
		_errors.emplace_back ("The file can not be opened:" + t);

		return (result);
	}

	nlohmann::json jsonDataArray;
	try
	{
		file >> jsonDataArray;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		_errors.emplace_back (e.what ());

		return (result);
	}

	unsigned int ct = 0;
	for (const auto& j : jsonDataArray)
	{
		TestData testData = { };
		testData._file = t;
		testData._line = ct + 1;

		try
		{
			testData._name = j.at ("name");

			std::cout << "Creating: " + testData._name + " test" << std::endl;

			auto in = j.at ("initial");
			testData._statusIn._A  = (unsigned char)  in.at ("a");
			testData._statusIn._X  = (unsigned char)  in.at ("x");
			testData._statusIn._Y  = (unsigned char)  in.at ("y");
			testData._statusIn._PC = (unsigned short) in.at ("pc");
			testData._statusIn._SP = (unsigned char)  in.at ("s");
			testData._statusIn._ST = (unsigned char)  in.at ("p");
			for (const auto& k : in.at ("ram"))
				testData._statusIn._memoryData.emplace_back
					(Test::MemoryPosition { (unsigned int) k [0], (unsigned char) k [1] });

			auto out = j.at ("final");
			testData._statusOut._A  = (unsigned char)  out.at ("a");
			testData._statusOut._X  = (unsigned char)  out.at ("x");
			testData._statusOut._Y  = (unsigned char)  out.at ("y");
			testData._statusOut._PC = (unsigned short) out.at ("pc");
			testData._statusOut._SP = (unsigned char)  out.at ("s");
			testData._statusOut._ST = (unsigned char)  out.at ("p");
			for (const auto& k : out.at ("ram"))
				testData._statusOut._memoryData.emplace_back 
					(Test::MemoryPosition { (unsigned int) k [0], (unsigned char) k [1] });

			auto cycles = j.at ("cycles");
			for (const auto& k : cycles)
				testData._cycles.emplace_back (Test::Cycle
					{ (k [2] == "read") ? Test::Cycle::Type::_MR : Test::Cycle::Type::_MW,
						{ (unsigned int) k [0], (unsigned char) k [1] } });

			if (result.find (testData._name) == result.end ())
				result.insert (Test::TestsDataMap::value_type 
					(MCHEmul::fixLenStr (std::to_string (ict + ct), 4, true, MCHEmul::_CEROS), testData));
			else
				_errors.emplace_back ("Test repeated: " + testData._name);
		}
		catch (const nlohmann::json::parse_error& e)
		{
			_errors.emplace_back (e.what ());
		}

		if (++ct == mtpc)
			break;
	}

	return (result);
}
