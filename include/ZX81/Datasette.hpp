/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/06/2024 \n
 *	Description: To emulate the behaviour of the Datasette Unit connected to the ZX81.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_DATASETTE__
#define __ZX81_DATASETTE__

#include <SINCLAIR/incs.hpp>
#include <ZX81/Type.hpp>

namespace ZX81
{
	/** With the io activities running "linked" with the CPU. */
	class Datasette final : public MCHEmul::BasicDatasette
	{
		public:
		static const int _ID = 100;

		Datasette (unsigned int rS);
	};

	/** With the io activities running "in paralell" with the CPU. */
	class DatasetteP final : public MCHEmul::BasicDatasetteP
	{
		public:
		static const int _ID = 1000;

		DatasetteP (unsigned int rS);
	};

	/** This type of datasette "injects" the infomation into the memory when it is simulated. 
		The way it is done, will depend on the type of structure prepared for that, 
		and also on the type of computer that is being simulated. */
	class DatasetteInjection final : public MCHEmul::DatasettePeripheral
	{
		public:
		static const int _ID = 101;

		/** The parameters are the point in the execution where the code has to be injected, and
			the point where to return once the injection has been done. */
		DatasetteInjection (Type t);

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual bool connectData (MCHEmul::FileData* dt) override;

		protected:
		Type _type;

		// Implementation...
		const MCHEmul::Address _addressIn, _addressOut;
	};
}

#endif
  
// End of the file
/*@}*/
