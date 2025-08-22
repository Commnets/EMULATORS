/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 16/08/2024 \n
 *	Description: To emulate the behaviour of the Datasette Unit connected 
 *				 to almost the SINCLAIR computers using the TZX File Format (@see FileIO class)
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_DATASETTE__
#define __SINCLAIR_DATASETTE__

#include <SINCLAIR/incs.hpp>
#include <SINCLAIR//FileIO.hpp>

namespace SINCLAIR
{
	/** With the io activities running "linked" with the CPU. */
	class Datasette final : public MCHEmul::StandardDatasette
	{
		public:

		/** In this implementation every bit in every byte represents a signal in the EAR (bit 6 port 0xfe).
			Every x TStates (cyles defined as parameter at construction time or modfied later) a bit is read
			The last byte has only y bits to be read. 
			From that point onwars everything read is 0 for a number of miliseconds.
			All these parameters are received as parameter at construction time, but they can be redefined. */
		class TZXDirectRecordingImplementation final : 
			public MCHEmul::StandardDatasette::Implementation
		{
			public:
			TZXDirectRecordingImplementation (unsigned int cR, unsigned char nbl)
				: MCHEmul::StandardDatasette::Implementation (),
				  _cyclesToAction (cR), _bitsLastByte (nbl),
				  _byteInfo (MCHEmul::UByte::_0), _bitManaged (7)
							{ }

			// Manage internal parameters...
			/** To access to the internal values. */
			unsigned int cyclesToAction () const
							{ return (_cyclesToAction); }
			size_t bitsLastByte () const
							{ return (_bitsLastByte); }
			/** Every block the cycles needed before reading the next element could vary. */
			void setParameters (unsigned int cR, unsigned char nbl)
							{ _cyclesToAction = cR; _bitsLastByte = nbl; }

			virtual std::tuple <bool, bool, bool> timeToReadValue (unsigned int cC) override;
			virtual void whenValueRead (unsigned int cC, const MCHEmul::UByte& v) override
							{ _byteInfo = v; _bitManaged = 7; }
			// The parameters to sample might vary...
			virtual void whenReadingNewBlock (const MCHEmul::DataMemoryBlock& dB) override;

			virtual std::tuple <bool, MCHEmul::UByte> timeToWriteValue (unsigned int cC, 
				bool vc, unsigned int ccvc, bool vs) override;
			virtual void whenValueWritten (unsigned int cC, const MCHEmul::UByte& v)
							{ _byteInfo = MCHEmul::UByte::_0; _bitManaged = 0; }
			// The parameters to sample must be fixed!
			virtual void whenCreatingNewBlock (const MCHEmul::DataMemoryBlock& dB) override;

			virtual void initialize (MCHEmul::StandardDatasette::Status st) override;

			private:
			unsigned int _cyclesToAction;
			size_t _bitsLastByte;

			// Implementation
			/** The byte being shifted now. At the beginning is 0. */
			MCHEmul::UByte _byteInfo;
			/** The bit read/written within the previous byte.
				In the last one, this value cannot be bigger that _bitsLastByte. */
			size_t _bitManaged;
		};

		static const int _ID = 100;

		Datasette (unsigned int rS);

		/** The only type admitted in TZX, but only when all blocks as sampling info. */
		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** By default it creates an empty TZX type of file.
			The only block that will be manage is the one reflects exactly the length of the sounds. */
		virtual MCHEmul::FileData* emptyData () const override;
		/** The data is returned in the format that was provided either with connectData or with emptyData.
			The type of data managed in this way is TZX only. */
		virtual MCHEmul::FileData* retrieveData () const override;

		protected:
		virtual MCHEmul::DataMemoryBlock createNewDataBlock () override;

		private:
		/** The original data, before it is converted into 
			the info requested by this peripheral. */
		MCHEmul::ExtendedDataMemoryBlocks _originalData;
	};
}

#endif
  
// End of the file
/*@}*/
