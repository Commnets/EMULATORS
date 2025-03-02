#include <ZXSpectrum/Datasette.hpp>
#include <ZXSpectrum/FileIO.hpp>

// ---
ZXSPECTRUM::Datasette::Datasette (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZXSpectrum" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZXSpectrumDatasette");
}

// ---
ZXSPECTRUM::DatasetteP::DatasetteP (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZXSpectrum" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZXSpectrumDatasette");
}

// ---
ZXSPECTRUM::DatasetteInjection::DatasetteInjection (ZXSPECTRUM::Type t)
	: MCHEmul::DatasettePeripheral (_ID, 
		{ { "Name", "Datasette Injection ZXSpectrum" },
		  { "Manufacturer", "ICF to inject the code directly into the memory" } }),
	  _type (t),
	  _loadTrap { 
		0, 
		"LOAD", 
		// The LDA-Bytes routine starts at 0x0556. 
	    // At this positión is when returning if BREAK key was pressed...
	    // As the data is going to be loaded quick!, pushing BREAK makes no sense...
		MCHEmul::Address ({ 0x56, 0x05 }, false), 
		MCHEmul::Address ({ 0xe2, 0x05 }, false), // The classical return!
	    {  } },
	  _blockRead (0)
{
	setClassName ("ZXSpectrumDatasetteI");
}

// ---
bool ZXSPECTRUM::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	if ((cpu -> programCounter ().internalRepresentation () == _loadTrap._addressIn.value ()) &&
		!_data._data.empty ()) // there must be data inside...
	{
		_IFDEBUG debugSimulation (cpu);

		// If everything was ok, then it is time for the next block!
		if (simulateTrap (cpu))
			_blockRead++;
		else
			_IFDEBUG debugErrorTrap ();
			
		cpu -> programCounter ().setAddress (_loadTrap._addressOut);
	}

	return (true);
}

// ---
bool ZXSPECTRUM::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <ZXSPECTRUM::TAPFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = std::move (dt -> asMemoryBlocks ());

	// Starting from the beginning...
	_blockRead = 0;
	
	return (true); 
}

// ---
bool ZXSPECTRUM::DatasetteInjection::simulateTrap (MCHEmul::CPU* cpu)
{
	struct ADC1 : public FZ80::ADD_General
	{
		ADC1 ()
			: FZ80::ADD_General (0, 1, 1, { }, "ADC1") // This info is not important...
				{  }

		bool justDo (FZ80::CZ80* cpu)
			{ _lastExecutionData._cpu = cpu; return (executeWith (MCHEmul::UByte::_1, true)); }

		// This instruction is never used, but it has to be defined...
		bool executeImpl (bool &f) override
			{ return (false); }
	};

	struct CP1 : public FZ80::CP_General
	{
		CP1 ()
			: FZ80::CP_General (0, 1, 1, { }, "CP1") // This info is not important...
				{  }

		bool justDo (FZ80::CZ80* cpu)
			{ _lastExecutionData._cpu = cpu; return (executeWith (MCHEmul::UByte::_1)); }

		// This instruction is never used, but it has to be defined...
		bool executeImpl (bool &f) override
			{ return (false); }
	};

	struct INCB : public FZ80::INC_General
	{
		INCB ()
			: FZ80::INC_General (0, 1, 1, { }, "INCB") // This info is not important...
				{  }

		bool justDo (FZ80::CZ80* cpu)
			{ _lastExecutionData._cpu = cpu; return (executeWith (cpu -> bRegister ())); }

		// This instruction is never used, but it has to be defined...
		bool executeImpl (bool &f) override
			{ return (false); }
	};

	if (_data._data.empty ())
		return (true); // There is no data to load...

	// The main registers used here...
	FZ80::CZ80* c = static_cast <FZ80::CZ80*> (cpu);
	MCHEmul::Register& aR		= c -> aRegister ();
	MCHEmul::Register& bR		= c -> bRegister ();
	MCHEmul::Register& cR		= c -> cRegister ();
	MCHEmul::RefRegisters& deR	= c -> deRegister ();
	MCHEmul::RefRegisters& hlR	= c -> hlRegister ();
	MCHEmul::RefRegisters& ixR	= c -> ixRegister ();
	MCHEmul::RefRegisters& afpR	= c -> afpRegister ();
	MCHEmul::StatusRegister& st = c -> statusRegister ();
	unsigned char parity; // checksum...0 when everything is ok!
	unsigned short i; // Number of data read or the type of block if none!

	_IFDEBUG debugStatus ("Entering Routine Simulation", c);

	std::cout << MCHEmul::FormatterBuilder::instance () -> 
		formatter ("CPU") -> format (c -> getInfoStructure ()) << std::endl;
	std::cout << MCHEmul::FormatterBuilder::instance () -> 
		formatter ("Stack") -> format (c -> memoryRef () -> stack () -> getInfoStructure ()) << std::endl;

	auto commonReturn = [&]() -> void
		{
			cR.set ({ 0x01 });
			hlR [0] -> set ({ parity });

			c -> setValueInRegisters (deR, c -> valueFromRegisters (deR) - i);
			c -> setValueInRegisters (ixR, c -> valueFromRegisters (ixR) + i);
		};

	auto errorReturn = [&]() -> void
		{
			st.setBitStatus (FZ80::CZ80::_CARRYFLAG, false);

			commonReturn ();
		};

	auto _ADC1 = [&]() -> void
		{ ADC1 adc1; adc1.justDo (c); };

	auto _CP1 = [&]() -> void
		{ CP1 cp1; cp1.justDo (c); };

	auto _INCB = [&]() -> void
		{ INCB incb; incb.justDo (c); };

	const std::vector <MCHEmul::UByte>& dMB = _data._data [_blockRead].bytes ();
	unsigned short length = (unsigned short) dMB.size ();
	
	// No data...it shouldn't happen!
	if (length == 0) 
	{
		// Multiple times are allowed...
		_blockRead = 0;

		hlR [1] /** L. */ -> set ({ 0x01 });
		afpR [1] /** F' */ -> set ({ 0x01 });
		st.setBitStatus (FZ80::CZ80::_CARRYFLAG, false); 
		// Seems that when the carry = false, there is an error...

		_IFDEBUG debugStatus ("No data to read", c);

		return (true);
	}

	// Is is not possible to read more that the number of bytes
	// defined in the register DE...
	unsigned short read = length - 1;
	if (read > c -> valueFromRegisters (deR))
		read = c -> valueFromRegisters (deR);

	// Verify or load? It is defined in the bit 0 of the F' register...
	bool verify = afpR [1] -> values ()[0].bit (0);
	// In A' is the type of data expected: 0x00 header, 0xff block...
	// ...and it is going to be the first to start to calculate the checksum either!
	i = (unsigned short) afpR [0] -> values ()[0].value (); 
	aR.set ({ 0x00 });
	// The first data is the type of block read!
	parity = dMB [0].value ();
	hlR [1] -> set ({ parity });
	// From here onwards it is needed to read from the data pointer + 1
	// Because the value 0 is no longer taking into account...

	/** There is a bug in the rom when the DE == 0.
		That is simulated in this way. */
	if (c -> valueFromRegisters (deR) == 0)
	{
		i = 0;

		bR.set ({ 0xb0 });
		aR.set ({ parity });

		_CP1 ();

		commonReturn ();

		_IFDEBUG debugStatus ("ROM Bug simualtion", c);

		return (true);
	}

	afpR [0] -> set ({ 0x01 }); 
	afpR [1] -> set ({ 0x45 });

	// The expected type (from the rom) and the type read (from the data), must match!
	// ...otherwise there is an error, and the routine finishes...
	if (parity != (unsigned char) i)
	{
		errorReturn ();

		_IFDEBUG debugStatus ("Info in the file doesn't match request", c);

		return (true);
	}

	// Last byte read, by default unless there was an error (@see verify)
	hlR [1] -> set ({ dMB [read] });
	unsigned char dt;
	/** The block of information either to load or to verify 
		is what DEv marks (that can be shorted that the real block of info. */
	MCHEmul::Address ad = c -> addressFromRegisters (ixR);
	for (i = 0; i < read /** The number of bytes to load. */; i++)
	{
		parity ^= (dt = dMB [(size_t) 
			(i + 1 /** Because when i == 0, the byte to be used would be the type of block,
					   and it's been already considered.
					   And when i = (read - 1) = check */)].value ()); 
		// At the end of the loop parity == ckecksum...if everything goes ok!
		if (verify)
		{
			if (dt != c -> memoryRef () -> value (ad + i).value ())
			{
				hlR [1] -> set ({ dt });

				errorReturn ();

				_IFDEBUG debugStatus ("Verification process KO", c);

				return (true);
			}
		}
		else
			c -> memoryRef () -> put (ad + i, dt);
	}

	if ((c -> valueFromRegisters (deR) == i) && 
		((read + 1) < length))
	{
		// If everything is ok, 
		// parity at this point is the checksum...
		// and then, here parity would be 0...
		aR.set ({ parity ^= dMB [read + 1].value () });

		_CP1 ();

		bR.set ({ 0x0b });

		commonReturn ();
	}
	else
	{
		bR.set ({ 0xff });

		hlR [1] -> set ({ 0x01 });

		_INCB ();

		errorReturn ();
	}

	_IFDEBUG debugStatus ("Exiting Routine OK", c);

	std::cout << MCHEmul::FormatterBuilder::instance () -> 
		formatter ("CPU") -> format (c -> getInfoStructure ()) << std::endl;
	std::cout << MCHEmul::FormatterBuilder::instance () -> 
		formatter ("Stack") -> format (c -> memoryRef () -> stack () -> getInfoStructure ()) << std::endl;

	return (true);
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugStatus (const std::string& where, FZ80::CZ80* cpu)
{
	auto allRgsAsAttrs = [&](const MCHEmul::Registers& rgs) -> MCHEmul::Attributes
		{ MCHEmul::Attributes result;
		  for (const auto& i : rgs) 
			  result.insert (MCHEmul::Attributes::value_type (i.name (), i.asString ()));
		  return (result); };

	_deepDebugFile -> writeSimpleLine (where);
	_deepDebugFile -> writeCompleteLine ("ZXSPECTRUMDN", cpu -> clockCycles (), 
		"Registers:", allRgsAsAttrs (cpu -> internalRegisters ()));
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugSimulation (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine 
		("ZXSPECTRUMDN", 
		 cpu -> clockCycles (), 
		 "Reading block:",
			{ { "BLOCK:", std::to_string (_blockRead) },
			  { "NAME:", _data._data [_blockRead].name () },
			  { "TYPE:", _data._data [_blockRead].attribute ("TYPE") },
			  { "LENGTH:", _data._data [_blockRead].attribute ("DATALENGTH") } });
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugErrorTrap ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Error reading block:" + std::to_string (_blockRead));
}
