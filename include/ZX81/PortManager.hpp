/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: PortManager.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 26/03/2024 \n
 *	Description: To manager all ports in ZX81.
 *	Versions: 1.0 Initial
 *	Based on https://8bit-museum.de/heimcomputer-2/sinclair/sinclair-scans/scans-zx81-video-display-system/
 */

#ifndef __ZX81_PORTMANAGER__
#define __ZX81_PORTMANAGER__

#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>
#include <ZX81/Type.hpp>

namespace ZX81
{
	class ULARegisters;
	class ULA;

	// Generic Port to manage all...
	/**
	  * Implementation:
	  * PORT FE or any with A0 = 0: \n
	  * When reading: \n
	  *		bits 0 - 4	= keyboard row status. \n
	  *		bit 5		= not used. Always at true. \n
	  *		bit 6		= Display refresh rate. 0 = 60Hz, 1 = 50Hz. \n
	  *		bit 7		= Cassette input. 0 = normal. 1 = pulse. \n
	  *		If the NMI Generator is off: \n
	  *		Put the casette signal down. \n
	  *		Restart the LINCNTR. \n
	  * ANY PORT:
	  * When writting: \n
	  *		Whatever value, enables NMI generator. \n
	  *		NMIs (Non maskable interrupts) are used during SLOW mode vertical blanking
	  *		periods to count the number of drawn blank scanlines. \n
	  *		Put the casette signal up. \n
	  *		Restart the LINECNTR.
	  */
	class PortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 0;
		static const std::string _NAME;

		PortManager (Type t);

		// See that the constructor is private
		// because it can only be invoked from ULA

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, true)); }
		virtual MCHEmul::UByte peekValue (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, false)); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override;

		/** To link to the different elements. */
		void linkToULA (ULA* ula);

		virtual void initialize () override;

		private:
		/** ms = true when is is wanted to modify the internal status. */
		MCHEmul::UByte getValue (unsigned short ab, unsigned char id, bool ms = false) const;

		private:
		Type _type;
		ULA* _ULA;
		ULARegisters* _ULARegisters;
	};
}

#endif
  
// End of the file
/*@}*/