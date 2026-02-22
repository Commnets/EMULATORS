#include <COMMODORE/IOPBuilder.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>
#include <COMMODORE/1530Datasette.hpp>
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
		if (a.first != "")
		{
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
				mPE = 
					(a.second == "PSMPS801")
						? (MCHEmul::MatrixPrinterEmulation*) 
							new COMMODORE::MPS801PostscriptMatrixPrinterEmulation 
								(MCHEmul::MatrixPrinterEmulation::Paper (), pF)
						: ((a.second == "PS") 
							? (MCHEmul::MatrixPrinterEmulation*)
								new COMMODORE::MPS801BasicMatrixPrinterEmulation (pF)
							: (MCHEmul::MatrixPrinterEmulation*)
								new MCHEmul::BasicMatrixPrinterEmulation (80, pF));
			}
		}
	}

	return (std::make_tuple (pF, dN, mPE));
}
