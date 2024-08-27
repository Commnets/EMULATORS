/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SIDRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: SID Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SIDREGISTERS__
#define __COMMODORE_SIDREGISTERS__

#include <CORE/incs.hpp>
#include <COMMODORE/SID/SIDLibWrapper.hpp>

namespace COMMODORE
{
	class SID;

	class SIDRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend SID;

		static const int _SIDREGS_SUBSET = 1050;

		/** 2 groups of potenciometers (with 2 axis each, so 4 potentiometers in total) 
			are admitted. @see SID for further details. */
		SIDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, 
			const MCHEmul::Address& a, size_t s);

		~SIDRegisters ()
							{ delete [] _potenciometerValues; 
							  _potenciometerValues = nullptr; }

		virtual size_t numberRegisters () const override
							{ return (0x20); }

		/** To manage the potenciometer group active. 
			MCHEmul::_SO meand none. */
		void setPotenciometerGroupActive (size_t n)
							{ _potenciometerGroupActive = n; }
		size_t potenciometerGroupActive () const
							{ return (_potenciometerGroupActive); }
		/** To set the "position" of the a potenciometer (a) within a group (np). \n
			No boundaries checks are done to speed up code in production, 
			so take care when using this method. */
		void setPotenciometerValue (size_t np, size_t a, unsigned char v)
							{ _potenciometerValues [np][a] = v; }
		unsigned char potenciometerValue (size_t np, size_t a) const
							{ return (_potenciometerValues [np][a]); }

		virtual void initialize () override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		/** To initialize the internal values. */
		void initializeInternalValues ();

		/** This method is to establish the wrapper used. */
		void setSIDLibWrapper (SIDLibWrapper* w)
							{ _sidWrapper = w; }

		private:
		/** MCHEmul::_S0 means none. */
		size_t _potenciometerGroupActive;
		/** The position of the joysticks, and their different axis are simulated as potenciometers. \n
			The memory structure is initialized at construction and initialized in initializeInternalvalues (). 
			As the number of joysticks is not know and depends on a parameter 
			there must be quicker to use pointers instead e.g. vectors. */
		unsigned char** _potenciometerValues;
		/** A reference to SID Lib Wrapper. */
		SIDLibWrapper* _sidWrapper;

		// Implementation
		/** Just to avoid unnecessary copies later. */
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
