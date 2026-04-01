#include <COMMODORE/IOPBuilder.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>
#include <COMMODORE/1530Datasette.hpp>
#include <COMMODORE/SerialPrinterMPS801.hpp>
#include <COMMODORE/SerialPrinterMPS802.hpp>
#include <C64/Cartridge.hpp>

// ---
MCHEmul::IOPeripheral* COMMODORE::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == COMMODORE::NoUserIOPeripheral::_ID)
		result = new COMMODORE::NoUserIOPeripheral;
	else if (id == MCHEmul::NoDatasettePeripheral::_ID)
		result = new MCHEmul::NoDatasettePeripheral;
	else if (id == COMMODORE::NoExpansionPeripheral::_ID)
		result = new COMMODORE::NoExpansionPeripheral;
	else if (id == COMMODORE::Datasette1530::_ID)
		/** https://www.c64-wiki.com/wiki/Datassette_Encoding
			CN2 datasette keeps three types on short pulses (in NTSC):
			long pulses = 1488Hz (672us length),
			medium pulses = 1953Hz (512us length),
			short pulses = 2840Hz (352us length = 176 + 176).
			Beging this one the shortest one too:
			0,000176 s/device cycle == (* 1022727 CPU cycles/s) 180 CPU cycles / device cyles.
			Making the numbers with PAL definition, the constant would be the same!.
			Simular calculus can be done for every commodore computer type. */
		result = new COMMODORE::Datasette1530
			(180, new COMMODORE::Datasette1530::TAPFileFormatImplementation (180));

	// Take care, it could be null...
	return (result);
}

// ---
std::tuple <std::string, unsigned char, MCHEmul::MatrixPrinterEmulation*> 
	COMMODORE::IOPeripheralBuilder::getDataPrinterFrom
		(const MCHEmul::Attributes& prms,
		 const std::tuple <std::string, unsigned char, MCHEmul::MatrixPrinterEmulation*>& eD) const
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

	std::string pF;
	unsigned char dN;
	MCHEmul::MatrixPrinterEmulation* mPE;
	std::tie (pF, dN, mPE) = eD;

	for (auto const& i : prms)
	{
		auto a = getParameter (i.second);
		if (a.second.empty ()) continue;

		if (a.first == "D")
		{
			dN = (unsigned char) std::atoi (a.second.c_str ());
		}
		else
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
			// The printer to emulate is the MPS801...
			if (pz [0] == "MPS801")
			{
				if (pz.size () == 1)
				{
					mPE = new COMMODORE::MPS801BasicMatrixPrinterEmulation (pF);
				}
				else
				if (pz.size () == 2)
				{
					if (pz [1] == "PS")
					{
						mPE = new COMMODORE::MPS801PostscriptMatrixPrinterEmulation 
							(getPaperDataPrinterEmulationFrom (a.second), pF);
					}
					else
					{
						_LOG ("Type of emulation mechanism: " + pz [1] + 
							" not supported for MPS801 printer, using basic emulation.");

						mPE = new COMMODORE::MPS801BasicMatrixPrinterEmulation (pF);
					}
				}
				else
				{
					_LOG ("Too many details for MPS801 printer, using basic emulation.");

					mPE = new COMMODORE::MPS801BasicMatrixPrinterEmulation (pF);
				}
			}
			else
			// The printer to emulate is the MPS802...
			if (pz [0] == "MPS802")
			{
				if (pz.size () == 1)
				{
					mPE = new COMMODORE::MPS802BasicMatrixPrinterEmulation (pF);
				}
				else
				if (pz.size () == 2)
				{
					if (pz [1] == "PS")
					{
						mPE = new COMMODORE::MPS802PostscriptMatrixPrinterEmulation 
							(getPaperDataPrinterEmulationFrom (a.second), pF);
					}
					else
					{
						_LOG ("Type of emulation mechanism: " + pz [1] + 
							" not supported for MPS802 printer, using basic emulation.");

						mPE = new COMMODORE::MPS802BasicMatrixPrinterEmulation (pF);
					}
				}
				else
				{
					_LOG ("Too many details for MPS802 printer, using basic emulation.");

					mPE = new COMMODORE::MPS802BasicMatrixPrinterEmulation (pF);
				}
			}
			// Creates the basic printer...
			else
			{
				_LOG ("Type of printer emulation: " + pz [0] + 
					" not supported, using basic emulation.");

				mPE = new MCHEmul::BasicMatrixPrinterEmulation (80, pF);
			}
		}
	}

	return (std::make_tuple (pF, dN, mPE));
}

// ---
MCHEmul::MatrixPrinterEmulation::Paper
	COMMODORE::IOPeripheralBuilder::getPaperDataPrinterEmulationFrom (const std::string& pDt) const
{
	MCHEmul::MatrixPrinterEmulation::Paper defPaper 
		(MCHEmul::MatrixPrinterEmulation::Paper::Type::_BLUEBAND, 9.5f, 11.0f, 0.5f); 

	size_t iP = pDt.find ('(');
	size_t fP = pDt.find (')');
	if (iP == std::string::npos || fP == std::string::npos || iP >= fP)
		return (defPaper);

	std::string data = pDt.substr (iP + 1, fP - iP - 1);
	MCHEmul::Strings strs = MCHEmul::getElementsFrom (data, ',');
	if (strs.size () != 4)
		return (defPaper);

	std::string pT = MCHEmul::upper (strs [0]);
	defPaper._type = 
		(pT == "BLUEBAND") 
			? MCHEmul::MatrixPrinterEmulation::Paper::Type::_BLUEBAND
			: ((pT == "GREENBAND") 
				? MCHEmul::MatrixPrinterEmulation::Paper::Type::_GREENBAND
				: ((pT == "GRAYBAND") 
					? MCHEmul::MatrixPrinterEmulation::Paper::Type::_GRAYBAND
					: MCHEmul::MatrixPrinterEmulation::Paper::Type::_WHITE));
	defPaper._width		= std::stof (strs [1]);
	defPaper._height	= std::stof (strs [2]);
	defPaper._border	= std::stof (strs [3]);

	return (defPaper);
}
