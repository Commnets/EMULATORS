/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: PortManager.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: To manager all ports in a ZXSpectrum.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_PORTMANAGER__
#define __ZXSPECTRUM_PORTMANAGER__

#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>

namespace ZXSPECTRUM
{
	class ULARegisters;
	class ULA;

	/** Generic Port to manage all. \n
		It manages mainly the port 254 (and similars) that takes care of the keyboard presses and
		also the port 31 that takes care of the simulation of the KEMPSTON joystick. */
	class PortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 0;
		static const std::string _NAME;

		PortManager ();

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, true)); }
		virtual MCHEmul::UByte peekValue (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, false)); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override;
		virtual unsigned int additionalClockCyclesForIO
			(unsigned short ab, unsigned int cC) const override;

		/** To link to the different elements. */
		void linkToULA (ULA* ula);

		virtual void initialize () override;

		private:
		/** The T-state stored by the input instruction currently being executed. */
		unsigned int IOAccessClockCycle () const;
		/** ms = true when is is wanted to modify the internal status. */
		MCHEmul::UByte getValue (unsigned short ab, unsigned char id, bool ms = false) const;

		private:
		ULA* _ULA;
		ULARegisters* _ULARegisters;

		// Implementation...
		/** The log information must be written only once to avoid delays in the code. 
			bot accesing when reading and when writting. */
		mutable std::vector <unsigned char> _portWriteNotLinked;
		mutable std::vector <unsigned char> _portReadNotLinked;
	};
}

#endif
  
// End of the file
/*@}*/
