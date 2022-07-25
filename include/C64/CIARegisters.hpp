/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 CIA Registers Emultation
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIAREGISTERS__
#define __C64_CIAREGISTERS__

#include <CORE/incs.hpp>

namespace C64
{
	/** In the CIA1 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class CIA1Registers final : public MCHEmul::PhisicalStorageSubset
	{
		public:
		CIA1Registers (int id, MCHEmul::PhisicalStorage* pS)
			: MCHEmul::PhisicalStorageSubset (id, pS, 0xdc00, MCHEmul::Address ({ 0x00, 0xdc }, false), 0x0100),
			  _lastValue (MCHEmul::PhisicalStorage::_DEFAULTVALUE)
			  // At this point all internal variables will have random values...
							{ }

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		private:
		// Implementation
		mutable MCHEmul::UByte _lastValue;
	};

	/** In the CIA2 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class CIA2Registers final : public MCHEmul::PhisicalStorageSubset
	{
		public:
		CIA2Registers (int id, MCHEmul::PhisicalStorage* pS)
			: MCHEmul::PhisicalStorageSubset (id, pS, 0xdc00, MCHEmul::Address ({ 0x00, 0xdc }, false), 0x0100),
			  _lastValue (MCHEmul::PhisicalStorage::_DEFAULTVALUE)
			  // At this point all internal variables will have random values...
							{ initializeInternalValues (); }

		unsigned short VICIIBank () const
							{ return (_VICBank); }

		virtual void initialize () override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		void initializeInternalValues ();

		private:
		unsigned short _VICBank;

		// Implementation
		mutable MCHEmul::UByte _lastValue;
	};
}

#endif
  
// End of the file
/*@}*/
