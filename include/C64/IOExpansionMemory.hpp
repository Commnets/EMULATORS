/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: IOExpansionMemory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 19/03/2023 \n
 *	Description: C64 IO Expansion Memory
 *	Versions: 1.0 Initial
 */

#ifndef __C64_IOEXPANSIONMEMORY__
#define __C64_IOEXPANSIONMEMORY__

#include <COMMODORE/incs.hpp>

namespace C64
{
	/** This zone of the memory is the one destinated to be used by the expansion port. */
	class IOExpansionMemory : public MCHEmul::PhysicalStorageSubset
	{
		public:
		struct EventData : public MCHEmul::Event::Data
		{
			EventData ()
				: _position (0), _value (MCHEmul::UByte::_0)
							{ }

			EventData (size_t p, const MCHEmul::UByte& v)
				: _position (p), _value (v)
							{ }

			size_t _position;
			MCHEmul::UByte _value;
		};

		// Different events.
		/** When this part of the memory is accesed usually the cartridge should do something. */
		static const unsigned int _SET		= 310;
		static const unsigned int _READ		= 311;

		IOExpansionMemory (int id, MCHEmul::PhysicalStorage* ps, size_t ss, const MCHEmul::Address& a, size_t s)
			: MCHEmul::PhysicalStorageSubset (id, ps, ss, a, s)
							{ }

		private:
		/** Just to send a notification when set happens. */
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		/** Just to send a notification when read happens. */
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
	};

	/** And it has to versions. 
		The one from 0xde00 to 0xdeff. */
	class IOExpansionMemoryI final : public IOExpansionMemory
	{
		public:
		static const int _IO1_SUBSET = 1010;

		IOExpansionMemoryI (MCHEmul::PhysicalStorage* ps, size_t ss, const MCHEmul::Address& a, size_t s)
			: IOExpansionMemory (_IO1_SUBSET, ps, ss, a, s)
							{ }
	};

	/** And the one from 0xdf00 to 0xdfff. */
	class IOExpansionMemoryII final : public IOExpansionMemory
	{
		public:
		static const int _IO2_SUBSET = 1011;

		IOExpansionMemoryII (MCHEmul::PhysicalStorage* ps, size_t ss, const MCHEmul::Address& a, size_t s)
			: IOExpansionMemory (_IO2_SUBSET, ps, ss, a, s)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
