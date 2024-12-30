/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: TestElements.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 02/06/2024 \n
 *	Description: Basic elements (including a CPU) to test.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_TESTELMNTS__
#define __FZ80_TESTELMNTS__

#include <FZ80/CZ80.hpp>

namespace FZ80
{
	/** A port to test. \n
		The value returned is always the high byte if the address bus received. \n
		When set it does not do anything. */
	class PortTest : public FZ80::Z80Port
	{
		public:
		PortTest ()
			: FZ80::Z80Port (0, "test")
							{ }

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
								{ return (MCHEmul::UByte ((unsigned char) ((ab & 0xff00) >> 8))); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override
								{ /** does nothing. */ }
	};

	/** A plain memory for simulation. \n
		64 KBytes with nothing. \n
		It is usually used for functional testings. */
	class PlainMemoryTest final : public MCHEmul::Memory
	{
		public:
		static const int _STACKSUBSET = 0;

		PlainMemoryTest ()
			: MCHEmul::Memory (0, basicContent ())
								{ }

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACKSUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }

		// Implementation
		MCHEmul::Memory::Content basicContent ();
	};

	/** A Memory very fragmentated. */
	class FragmentatedMemoryTest final : public MCHEmul::Memory
	{
		public:
		static const int _STACKSUBSET = 0;

		FragmentatedMemoryTest ()
			: MCHEmul::Memory (0, basicContent ())
								{ }

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACKSUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }

		// Implementation...
		MCHEmul::Memory::Content basicContent ();
	};

	/** A CPU with the previous basic elements. \n
		The Test owns also the memory and destroy it when deleted. */
	class TestZ80 final : public CZ80
	{
		public:
		TestZ80 (MCHEmul::Memory* m);

		~TestZ80 ()
			{ delete (_memory); }

		protected:
		// Implementation
		MCHEmul::Memory* _memory;
	};
}

#endif
  
// End of the file
/*@}*/
