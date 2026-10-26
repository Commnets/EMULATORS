/** \ingroup ZXSPECTRUM */
/*@{*/

/**
 *	@file
 *	File: ContendedMemory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 18/10/2026 \n
 *	Description: CPU-visible RAM forwarding Spectrum contention access requests.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_CONTENDEDMEMORY__
#define __ZXSPECTRUM_CONTENDEDMEMORY__

#include <CORE/Stack.hpp>

namespace ZXSPECTRUM
{
	class CZ80;

	/** CPU-visible RAM preserving Stack behavior. \n
		Access notifications are forwarded to the linked Spectrum CPU. \n
		The CPU determines which addresses belong to the contended range. */
	class ContendedMemory final : public MCHEmul::Stack
	{
		public:
		ContendedMemory
			(int id, MCHEmul::PhysicalStorage* ps, size_t pp,
			 const MCHEmul::Address& iA, size_t s,
			 const MCHEmul::Stack::Configuration& cfg =
				MCHEmul::Stack::Configuration ());

		/** Non-owning connection established by the computer before execution. */
		void linkToCPU (CZ80* c)
							{ _cpu = c; }

		protected:
		virtual void whenReading
			(const MCHEmul::Address& a, size_t nB) const override;
		virtual void whenWriting
			(const MCHEmul::Address& a, size_t nB) override;

		private:
		/** The computer owns the CPU. */
		CZ80* _cpu;
	};
}

#endif

// End of the file
/*@}*/
