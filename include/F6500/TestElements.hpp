/** \ingroup F6500 */
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

#ifndef __F6500_TESTELMNTS__
#define __F6500_TESTELMNTS__

#include <F6500/C6500.hpp>

namespace F6500
{
	/** A plain memory for simulation. \n
		64 KBytes with nothing. \n
		It is usually used for functional testings. */
	class PlainMemoryTest final : public MCHEmul::Memory
	{
		public:
		PlainMemoryTest ()
			: MCHEmul::Memory (0, basicContent ())
								{ }

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (0))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }

		// Implementation
		MCHEmul::Memory::Content basicContent ();
	};

	/** A Memory very fragmentated. */
	class FragmentatedMemoryTest final : public MCHEmul::Memory
	{
		public:
		FragmentatedMemoryTest ()
			: MCHEmul::Memory (0, basicContent ())
								{ }

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (0))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }

		// Implementation...
		MCHEmul::Memory::Content basicContent ();
	};

	/** A CPU with the previous basic elements. \n
		The Test owns also the memory and destroy it when deleted. */
	class Test6500 final : public C6500
	{
		public:
		Test6500 (MCHEmul::Memory* m);

		~Test6500 ()
			{ delete (_memory); }

		protected:
		// Implementation
		MCHEmul::Memory* _memory;
	};
}

#endif
  
// End of the file
/*@}*/
