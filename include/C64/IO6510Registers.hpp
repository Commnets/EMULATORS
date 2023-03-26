/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: IO6510Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: Memory locations 0 & 1.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_IO6150REGISTERS__
#define __C64_IO6150REGISTERS__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class SpecialFunctionsChip;

	class IO6510Registers final : public MCHEmul::ChipRegisters
	{
		public:
		friend SpecialFunctionsChip;

		/** A name when this register is a subset in the main memory. */
		static const int _IO6510REGISTERS_SUBSET = 2040;

		/** Event when the C64 Port IO bits are actualized. */
		static const unsigned int _C64PORTIOBITSACTUALIZED = 300;

		IO6510Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (2); }

		virtual void initialize () override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier) override;

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		void initializeInternalValues ();

		// Methods related with the location of accesible RAM...
		/** Whether the Basic interpreter or RAM is available from $a000 - $bfff location. 
			true when Basic is available and false when RAM. */
		void setLORAM (bool s)
							{ _LORAM = s; }
		bool LORAM () const
							{ return (_LORAM); }
		/** Whether the Kernel or RAM is available from $e000 - $ffff location. 
			true when Kernel is available and false when RAM. */
		void setHIRAM (bool s)
							{ _HIRAM = s; }
		bool HIRAM () const
							{ return (_HIRAM); }
		/** Whether the I/O locations or Character ROM is available from $d000 - $dfff location. 
			true when I/O is available (default) and false when character ROM is available. */
		void setCHAREN (bool s)
							{ _CHAREN = s; }
		bool CHAREN () const
							{ return (_CHAREN); }
							
		// Methods related with the casette...
		/** To know which is the data sent to the line. */
		void setCasetteData (bool s)
							{ _casetteData = s; }
		bool casetteData () const
							{ return (_casetteData); }
		/** To know whether the casette motor is or not running. */
		void setCasetteMotorStopped (bool s)
							{ _casetteMotorStopped = s; }
		bool casetteMotorStopped () const
							{ return (_casetteMotorStopped); }
		/** To know whether there is or not no key pressed at the casette. */
		void setCasetteNoKeyPressed (bool s)
							{ _casetteNoKeyPressed = s; }
		bool casetteNoKeyPressed () const
							{ return (_casetteNoKeyPressed); }

		private:
		bool _CHAREN, _HIRAM, _LORAM;
		bool _casetteData, _casetteNoKeyPressed, _casetteMotorStopped;

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
