/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: DebugFile.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 08/07/2023 \n
 *	Description: Defines Common elements around a debug file.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_DEBUGFILE__
#define __MCHEMUL_DEBUGFILE__

#include <CORE/global.hpp>
#include <fstream>

namespace MCHEmul
{
	class DebugFile
	{
		public:
		DebugFile ()
			: _fileName (),
			  _addInfo (false),
			  _file (),
			  _active (false),
			  _blockDesactivate (false),
			  _desactivateTryWhenBlockDesactivate (false)
							{ }

		DebugFile (const std::string& fN, bool a = true)
			: _fileName (fN),
			  _addInfo (a),
			  _file (),
			  _active (false),
			  _blockDesactivate (false),
			  _desactivateTryWhenBlockDesactivate (false)
							{ /** The file is not opened. */ }

		/** To know the parameters. 
			Two of them can only be modified when the debug file is activated.*/
		const std::string& fileName () const
							{ return (_fileName); }
		bool addInfo () const
							{ return (_addInfo); }
		std::ofstream& file ()
							{ return (_file); }

		/** To activate the debug file. */
		bool active () const
							{ return (_active); }
		bool activate (const std::string &fN, bool a = true /** to add info at the end of the file. */);
		bool desactivate ();

		/** When desactivation is blocked... 
			it can be desactivated. */
		void setBlockDesactivate ()
							{ _blockDesactivate = true; }
		/** To check whether the desactivation has been tried.
			Once it has been checked is retored to false. */
		inline bool desactivateTryWhenBlockDesativate () const;

		/** To simplify the output of info. \n
			Only valid when it is active. */
		friend DebugFile& operator << (DebugFile& dF, const std::string& i)
							{ if (dF._active) dF._file << i; 
							  return (dF); }

		protected:
		std::string _fileName; // can not be changed.
		bool _addInfo; // can be changed...
		std::ofstream _file;

		// Implementation
		mutable bool _active;
		mutable bool _blockDesactivate;
		mutable bool _desactivateTryWhenBlockDesactivate;
	};

	// ---
	inline bool DebugFile::desactivateTryWhenBlockDesativate () const
	{ 
		bool r = _desactivateTryWhenBlockDesactivate;
		
		_desactivateTryWhenBlockDesactivate = false; 
		_blockDesactivate = false;
							  
		return (r); 
	}
}

#endif
  
// End of the file
/*@}*/