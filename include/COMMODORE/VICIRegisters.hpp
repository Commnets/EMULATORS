/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICIRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: VICI Registers Emulation
 *	Versions: 1.0 Initial
 *	Based on http://tinyvga.com/6561
 */

#ifndef __COMMODORE_VICIREGISTERS__
#define __COMMODORE_VICIREGISTERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** In the VICI Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class VICIRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		static const int _VICREGS_SUBSET = 1040;
			
		// The different types of graphics that the VICI is ablo to repreoduce...
		enum class GraphicMode
		{
			_CHARMODE = 0,
			_MULTICOLORCHARMODE,
		};

		VICIRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * TODO
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		/** Just to initialize the internal values. */
		void initializeInternalValues ();

		private:
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
