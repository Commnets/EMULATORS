/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C16SenseTapeRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/01/2026 \n
 *	Description: To detect whether any key in the tape is pressed in C16.
 *				 It is a physical effect due to the electrical connections in 16.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C16SENSETAPEREGISTERS__
#define __C264_C16SENSETAPEREGISTERS__

#include <COMMODORE/incs.hpp>

namespace C264
{
	/** In the C16 only, the positions between $fd10 and $fd1f are not IO C6529B chip 
		like it happens in the Plus4. However that positions can be used to 
		knowe whether any key is pressed in the datasette. \n
		When a key is pressed there the bit 2 of that positions become 0. */
	class C16SenseTapeRegisters final : public MCHEmul::PhysicalStorageSubset
	{
		public:
		/** The positions where the C6529B1 chip is reachable are constant. */
		C16SenseTapeRegisters 
			(int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual void initialize () override;

		private:
		/** Just to read whether the is or not a key pressed in the tape. */
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Implementation
		void initializeInternalValues ();

		private:
		/** true when a key is pressed in the tape...
			 ...and false when not. */
		bool _cassetteKeyPressed;
		
		// Implmenetation
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
