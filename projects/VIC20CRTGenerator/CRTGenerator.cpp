#include "CRTGenerator.hpp"

// ---
VIC20::CRTGenerator::CRTGenerator ()
	: _templates (
		{ { "none",	std::shared_ptr <VIC20::ParameterTemplate> (new VIC20::none_ParameterTemplate) },
		  { "i",	std::shared_ptr <VIC20::ParameterTemplate> (new VIC20::i_ParameterTemplate) },
		  { "o",	std::shared_ptr <VIC20::ParameterTemplate> (new VIC20::o_ParameterTemplate) },
		  { "n",	std::shared_ptr <VIC20::ParameterTemplate> (new VIC20::n_ParameterTemplate) },
		  { "h",	std::shared_ptr <VIC20::ParameterTemplate> (new VIC20::h_ParameterTemplate) } }),
	  _parameters (),
	  _error (false),
	  _errorNames ({ }),
	  _steps ({ })
{
	// Nothing else...
}

// ---
bool VIC20::CRTGenerator::run (int argc, char** argv)
{
	_error = false;
	_errorNames = { };
	_steps = { };
	_parameters = { };

	// Read the parameters...
	// ...and if error, finishes!
	readParametersFrom (argc, argv);
	if (_error)
		return (false);
	else
		_steps.emplace_back ("Parameters read");

	// If help is requested, nothing else is done!
	if (_parameters.exists ("h"))
	{ 
		printOutHelp ();

		return (true);
	}

	// Create the CRTData structure...
	// ...and if error, finishes!
	VIC20::CRTData data (std::move (createCRTStructure ()));
	if (_error)
		return (false);
	else
		_steps.emplace_back ("Internal CRT structure generated");

	// ...and finally saves the structure...
	// taking into account the parameters (potentially) and the output file name defined
	// It is again unlikely the file fails, but the possibility must be taken into account!
	data.saveTo (_parameters.parameter ("o").values ()[0], _parameters, _error);
	if (_error)
		_errorNames.emplace_back ("The result can not be saved");
	else
		_steps.emplace_back ("File sucessfully saved");

	return (!_error);
}

// ---
void VIC20::CRTGenerator::readParametersFrom (int argc, char** argv)
{
	// Everything starts with a non defined parameter...yet!
	VIC20::Parameter prm (paramTemplate ("none"));
	for (int i = 1 /** 0 is the name of the program. */; 
			i < argc; i++)
	{
		std::string avs = argv [i];

		// The definition of a new parameter is about to start...
		if (avs [0] == '/') 
		{
			std::string nP = avs.substr (1);
			
			// If there was nothing previouly defined...
			// ..the new parameter has to be the one!
			if (prm.paramTemplate () == paramTemplate ("none"))
			{
				// But it has to be allowed, otherwise an error is generated...
				if (_error |= !existsParamTemplate (nP))
					_errorNames.emplace_back ("The parameter: " + nP + " is not allowed");
				else
					prm.changeParamTemplate (paramTemplate (nP)); 
			}
			else
			{
				// The previous parameter has to be added to the list of them
				// but before doing so a consistency check is done
				// over the curent parameter...
				if (_error |= !prm.checkConsistency ())
					_errorNames.emplace_back ("The parameter: " + nP + " is not well defined");
				else
				{
					_parameters.add (prm);

					if (_error |= !existsParamTemplate (nP))
					{
						_errorNames.emplace_back ("The parameter: " + nP + " is not allowed");

						// Starts back from scratch!
						prm = VIC20::Parameter (paramTemplate ("none"));
					}
					else
						prm = VIC20::Parameter (paramTemplate (nP));
				}
			}
		}
		// It is not the definition of a new parameter
		// but a value that must be included in the one under construction...
		else
		{
			// ...but a new value with no parameter under construction?...mistake
			if (_error |= (prm.paramTemplate () == paramTemplate ("none")))
				_errorNames.emplace_back ("The value " + avs + " does match with any parameter");
			else
			{
				// if the value is possible has to alowwed by the parameter...
				if (_error |= !prm.addValue (avs))
					_errorNames.emplace_back ("The value " + avs + " is not allowed under the parameter " + prm.id ());
			}
		}
	}

	// The last paramater has to be added to the list if there were any issue!
	if (_error |= !prm.checkConsistency ())
		_errorNames.emplace_back ("The parameter: " + prm.id () + " is not well defined");
	else
		_parameters.add (prm);

	// Before moving forward the consistency of the body is checked...
	// Rules:a parameter i and a parameter o must be defined...
	if (_error |= (!existsParameter ("i") || !existsParameter ("o")))
		_errorNames.emplace_back ("There is no either parameter i or parameter o defined. Mandatory");
}

// ---
VIC20::CRTData VIC20::CRTGenerator::createCRTStructure () const
{
	// Read the input files and generate the blocks of the data...
	// Remember that, somehow, the files have been already validated,
	// so it is not likely the reading action fails...however the possibility is taken into account!
	VIC20::CRTData result;
	const MCHEmul::Strings& iF = _parameters.parameter ("i").values ();
	for (MCHEmul::Strings::const_iterator i = iF.begin ();
			i != iF.end () && !_error; i++)
	{
		result.addChipFrom (MCHEmul::DataMemoryBlock::loadBinaryFile ((*i), 
			_error, 2 /** length of the address in bytes. */, false /** little - endian. */));
		if (_error)
			_errorNames.emplace_back ("The file: " + (*i) + " cannot be loaded");
	}

	// Now, change the default definitions in the header
	// if something were defined at configuration level (parameters)...
	// A new name for the cartrige?
	if (_parameters.exists ("n"))
		result.header ().setName (_parameters.parameter ("n").values ()[0]);

	return (result);
}
