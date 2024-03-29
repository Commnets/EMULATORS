/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: Cartridge.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2025 \n
 *	Description: To emulate the behaviour of a cartridge in the ZX81 computer
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_CARTRIDGE__
#define __ZX81_CARTRIDGE__

#include <SINCLAIR/incs.hpp>
#include <ZX81/Memory.hpp>
#include <ZX81/EdgeConnectorPeripherals.hpp>

namespace ZX81
{
	class Memory;
	class MemoryView;

	/** Ultramax Cartridge is not still supported. */
	class Cartridge final : public EdgeConnectorPeripheral
	{
		public:
		enum class Type
		{
			_NOTDEFINED = 0

			// TODO
		};

		static const int _ID = 204;

		Cartridge ();

		/** To know the type of cartridge, if any. */
		Type type () const
							{ return (data ()._data.empty () 
								? Type::_NOTDEFINED 
								: (Type) (std::atoi (((*_data._attributes.find ("TYPE")).second).c_str ()))); }

		/** When data os connected attending to the type of cartridge (info in the data received)
			the additional subset of information is created. */
		virtual bool connectData (MCHEmul::FileData* dt) override;

		virtual bool initialize () override
							{ return (true); }
		/** When finish, the additional memory created has to taken off from the memory and destroyed. */
		virtual bool finalize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/** To dump the data of the cartridge into the memory. \n
			The configuartion of the memory is changed. */
		void dumpDataInto (ZX81::Memory* m, MCHEmul::MemoryView* mV);

		private:
		bool dataDumped () const
							{ bool r = _dataDumped; _dataDumped = false; return (r); }

		private:
		// Implementation
		mutable bool _dataDumped;

		/** When inserting a cartridge the structure of the memory is changed,
			and additional subsets of memory are added. \n
			Other way around when the cartridge is unplugged. */
		ZX81::Memory* _memoryRef;
		MCHEmul::MemoryView* _memoryView;
	};
}

#endif
  
// End of the file
/*@}*/
