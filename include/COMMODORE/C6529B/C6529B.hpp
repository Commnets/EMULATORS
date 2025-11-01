/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: C6529B.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: This chip just move a value from to a port. \n
 *				 When it is connected to the data bus (in the computer),
 *				 the port is move to (write) or from (read) the data bus.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_C6529B__
#define __COMMODORE_C6529B__

#include <COMMODORE/C6529B/C6529BRegisters.hpp>

namespace COMMODORE
{
	/** The port data is pulled up by default. \n
		That is: they only can be move down. */
	class C6529B : public MCHEmul::Chip
	{
		public:
		static const int _ID = 108;

		/** Some event than can be used when it is needed to notify 
			the changes in the port balue. */
		static const unsigned int _PORTVALUECHANGED = 230;
	
		C6529B (int id);
		
		virtual bool initialize () override;

		/** Set the data to the port. */
		void setPortValue (const MCHEmul::UByte& v)
							{ _C6529BRegisters -> setPortValue (v); }
		/** Get the data from the port. */
		const MCHEmul::UByte& portValue () const
							{ return (_C6529BRegisters -> portValue ()); }

		/** Notifies when the port value changes. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The Chip info plus:
		  * C6529Registers	= InfoStructure: Info about the registers.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** The memory is used also as the set of registers of the chip. */
		C6529BRegisters* _C6529BRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
