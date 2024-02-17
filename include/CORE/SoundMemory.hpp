/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundMemory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 21/03/2022 \n
 *	Description: The sound buffer used by the Sound Chips to draw (if needed).
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SOUNDMEMORY__
#define __MCHEMUL_SOUNDMEMORY__

#include <cstring>

namespace MCHEmul
{
	/** It represents a the memory where all things to be sent to the sound system are store. \n
		It is actually a matrix of unsigned char, 
		representing each one the value of a byte in the sampling process. */  
	class SoundMemory
	{
		public:
		SoundMemory () = delete;

		/** The memory can behave in different when filled up,
			depending on the value of the thirs parameter:
			@param ns	:	Number of sampling elements to be stored.
			@param ss	:	Size of every sampling element in bytes. 
							So the size of the buffer will be = ns * ss (in bytes). */
		SoundMemory (size_t ns, size_t ss);

		SoundMemory (const SoundMemory&) = delete;

		SoundMemory& operator = (const SoundMemory&) = delete;

		virtual ~SoundMemory ()
							{ delete (_samplingData); }

		SoundMemory (SoundMemory&&) = delete;

		SoundMemory& operator = (SoundMemory&&) = delete;

		// Size...
		size_t numberElements () const
							{ return (_numberSamples); }
		size_t sizeSample () const
							{ return (_sizeSample); }
		size_t size () const
							{ return (_numberSamples * _sizeSample); }
		const char* samplingData () const
							{ return (_samplingData); }

		// Managing the info of the sampling...
		void initialize ()
							{ for (size_t i = 0; i < (_numberSamples * _sizeSample); 
								_samplingData [i++] = 0);
							  _pointerToSample = 0; }

		/** The parameters are the data buffer elements (minimal expression) and 
			also the number of elements of that buffer. \n
			They don't represent sample data as the sample data is made of chunks not basic elements. \n
			The method returns true when the internal sampling buffer is full and false when not. ºn
			When o = true and more data that capacity still available is wanted to be stored, 
			it is stored back from the beginning. */
		inline bool addSampleData (char* sB, size_t nD, bool o = false);

		private:
		// Represents a circular buffer...
		char* _samplingData;
		size_t _numberSamples;
		size_t _sizeSample;

		// Implementation
		size_t _pointerToSample;
	};

	// ---
	inline bool SoundMemory::addSampleData (char* sB, size_t nD, bool o)
	{ 
		if ((nD % _sizeSample) != 0)
			return (false); // No well defined...
							// The number of bytes received have to be a multiple of the size of the sample...
		
		bool result = false;

		char* cB = sB;
		// Samples to write...
		size_t pS = nD / _sizeSample; 
		while (pS > 0 && ((!result && !o) || o))
		{
			size_t wS = pS;
			if (wS > (_numberSamples - _pointerToSample))
				wS = _numberSamples - _pointerToSample;
			pS -= wS; // Still pending samples to add...

			// Write part of the received data into the buffer...
			std::memcpy ((void*) (_samplingData + (_pointerToSample * _sizeSample)), (void*) cB, wS * _sizeSample);

			cB += (wS * _sizeSample);
			if ((_pointerToSample += wS) >= _numberSamples)
			{ 
				_pointerToSample = 0;

				result = true;
			}
		}

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
