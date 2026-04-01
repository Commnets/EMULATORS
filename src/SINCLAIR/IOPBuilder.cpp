#include <SINCLAIR/IOPBuilder.hpp>
#include <SINCLAIR/Printer.hpp>

// ---
MCHEmul::IOPeripheral* SINCLAIR::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	// TODO

	// Take care, it could be null...
	return (result);
}

// ---
std::tuple <
	std::string, 
	MCHEmul::MatrixPrinterEmulation*> 
	SINCLAIR::IOPeripheralBuilder::getDataPrinterFrom
		(const MCHEmul::Attributes& prms,
		 const std::tuple <
			SINCLAIR::ZXCodeToASCII*,
			MCHEmul::MatrixPrinterEmulation::Configuration,
			std::string, 
			MCHEmul::MatrixPrinterEmulation*>& eD) const
{
	// To get the parameter....
	auto getParameter = [](const std::string& attr) -> std::pair <std::string, std::string>
		{
			MCHEmul::Strings strs = MCHEmul::getElementsFrom (attr,':');
			return (strs.size () == 2 
				? std::make_pair
					(std::move (MCHEmul::upper (strs [0])), std::move (MCHEmul::upper (strs [1])))
				: std::make_pair ("", ""));
		};

	SINCLAIR::ZXCodeToASCII* cvt = nullptr;
	MCHEmul::MatrixPrinterEmulation::Configuration cfg;
	std::string pF;
	MCHEmul::MatrixPrinterEmulation* mPE;
	// Received as parameter...
	std::tie (cvt, cfg, pF, mPE) = eD;

	for (auto const& i : prms)
	{
		auto a = getParameter (i.second);
		if (a.second.empty ()) continue;

		if (a.first == "F")
		{
			pF = a.second;
		}
		else
		if (a.first == "P")
		{
			delete mPE;

			// There are at least one element in the list of details
			// to specify the name of the printer...
			MCHEmul::Strings pz = MCHEmul::getElementsFrom (a.second, '-');
			// The printer to emulate is the THERMAL...
			if (pz [0] == "THERMAL")
			{
				if (pz.size () == 1)
				{
					mPE = new SINCLAIR::BasicThermalPrinterEmulation (cvt, pF);
				}
				else
				if (pz.size () == 2)
				{
					if (pz [1] == "PS")
					{
						delete cvt; // Not used...

						mPE = new SINCLAIR::PostscriptThermalPrinterEmulation (cfg, pF);
					}
					else
					{
						_LOG ("Type of emulation mechanism: " + pz [1] + 
							" not supported for THERMAL printer, using basic emulation.");

						mPE = new SINCLAIR::BasicThermalPrinterEmulation (cvt, pF);
					}
				}
				else
				{
					_LOG ("Too many details for MPS801 printer, using basic emulation.");

					mPE = new SINCLAIR::BasicThermalPrinterEmulation (cvt, pF);
				}
			}
			// Creates the basic printer...
			else
			{
				_LOG ("Type of printer emulation: " + pz [0] + 
					" not supported, using basic emulation.");

				delete cvt; // Not used...

				mPE = new MCHEmul::BasicMatrixPrinterEmulation (80, pF);
			}
		}
	}

	return (std::make_tuple (pF, mPE));
}

