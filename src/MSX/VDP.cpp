#include <MSX/VDP.hpp>

// ---
MSX::VDP::VDP (MCHEmul::GraphicalChip* gC)
	: _chip (gC)
{ 
	assert (_chip != nullptr);
}

// ---
MSX::VDPOverTexasInstruments99XXFamily::VDPOverTexasInstruments99XXFamily (TEXASINSTRUMENTS::TMS99XX* tI)
	: MSX::VDP ((MCHEmul::GraphicalChip*) tI),
	  _texasChip (tI)
{  
	assert (_texasChip != nullptr);
}
