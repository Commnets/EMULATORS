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

		// To write different things within the debug file is active...
		/** A simple text to the debug file. */
		void writeSimpleLine (const std::string& s) const
							{ if (_active) _file << s << std::endl; } 
		/** The complete debug line has always to indicate where (element), when (usually clock status), 
			what (the real info) and additional data in different formats. \n
			If where.length () > 8 it would be reduced. \n 
			If when == std::numeric_limits <unsigned int> a "-" is shown. */
		inline void writeCompleteLine (const std::string& where, unsigned int when, 
			const std::string& what) const;
		inline void writeCompleteLine (const std::string& where, unsigned int when, 
			const std::string& what, const Strings& dt) const;
		inline void writeCompleteLine (const std::string& where, unsigned int when, 
			const std::string& what, const Attributes& attrs) const;
		/** Just to write a data. */
		void writeLineData (const std::string& dt) const
							{ if (_active) _file << _TABS.substr (0,5) << dt << std::endl; }
		/** Just the data of the previous line. 
			That data will be drawn aligned with the data of the line. */
		inline void writeLineData (const Strings& dt) const;
		/** Or attributes. */
		inline void writeLineData (const Attributes& attrs) const;

		/** To simplify the output of info. \n
			Only valid when it is active. */
		friend DebugFile& operator << (DebugFile& dF, const std::string& i)
							{ if (dF._active) dF._file << i; 
							  return (dF); }

		protected:
		std::string _fileName; // can not be changed.
		bool _addInfo; // can be changed...
		mutable std::ofstream _file;

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

	// ---
	inline void DebugFile::writeCompleteLine (const std::string& where, unsigned int when,
		const std::string& what) const
	{
		if (_active)
		{
			_file << fixLenStr (where, 8, false)
				  << fixLenStr ((when == std::numeric_limits <unsigned int>::max ()) 
					  ? "-" : std::to_string (when), 12, false)
				  << what << std::endl;
		}
	}

	// ---
	inline void DebugFile::writeCompleteLine (const std::string& where, unsigned int when,
		const std::string& what, const Strings& dt) const
	{
		writeCompleteLine (where, when, what);
		writeLineData (dt);
	}

	// ---
	inline void DebugFile::writeCompleteLine (const std::string& where, unsigned int when, 
			const std::string& what, const Attributes& attrs) const
	{
		writeCompleteLine (where, when, what);
		writeLineData (attrs);
	}

	// ---
	inline void DebugFile::writeLineData (const Strings& dt) const
	{
		if (_active)
			for (const auto& i : dt)
				_file << _TABS.substr (0,5) << i << std::endl;
	}

	// ---
	inline void DebugFile::writeLineData (const Attributes& attrs) const
	{ 
		if (_active)
		{
			for (const auto& i : attrs)
				_file << _TABS.substr (0,5) 
					  << fixLenStr (i.first, 20, false) 
					  << ":"
					  << i.second << std::endl;
		}
	}

	/** All classes with debug cvapabilities mut inherit this one too. 
		If it were JAVA this class would be a interface type. */
	class DebugableClass
	{
		public:
		DebugableClass ()
			: _deepDebugFile (nullptr)
							{ /** Nothing else. */ }

		bool deepDebugActive () const
							{ return (_deepDebugFile != nullptr && _deepDebugFile -> active ()); }
		virtual void setDeepDebugFile (DebugFile* dF) // It could be overloaded to extend it below the class...
							{ _deepDebugFile = dF; }
		const DebugFile* deepDebugFile () const
							{ return (_deepDebugFile); }
		DebugFile* deepDebugFile ()
							{ return (_deepDebugFile); }

		protected:
		DebugFile* _deepDebugFile;
	};

/** This define could be used to guarantee that debug is active before
	executing any debug activity. \n
	If must be used inside any class inheriring from DebugableClass. */
#define _IFDEBUG \
	if(deepDebugActive ())
#define _IFDEBUGINELEMENT(C) \
	if(C -> deepDebugActive ())
}

#endif
  
// End of the file
/*@}*/