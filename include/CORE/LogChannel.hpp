/** \ingroup EMULS */
/*@{*/

/**	
 *	@file	
 *	File: LogChannel.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 31/10/2024 \n
 *	Description: To log important things across the system.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_LOGCHANNEL__
#define __MCHEMUL_LOGCHANNEL__

#include <CORE/global.hpp>
#include <fstream>

namespace MCHEmul
{
	/** A channel though were 
		to write the different things. It could be overloaded. */
	class LogChannel
	{
		public:
		LogChannel (int id, 
				unsigned int nL = std::numeric_limits <unsigned int>::max ())
			: _id (id), _numberLines (nL),
			  _counter (0)
							{  }

		LogChannel (const LogChannel&) = delete;
		
		LogChannel (const LogChannel&&) = delete;

		LogChannel& operator = (const LogChannel&) = delete;

		LogChannel& operator = (const LogChannel&&) = delete;

		virtual ~LogChannel ()
							{ }

		int id () const
						{ return (_id); }

		void write (const std::string& str);

		friend LogChannel& operator << (LogChannel& lC, const std::string& str) 
							{ lC.write (str); return (lC); }

		protected:
		// Implememntation...
		/** The method to write really inthe log. */
		virtual void writeImplementation (const std::string& str) = 0;
		/** To define what to to when the channel reaches the max number of lines written. \n
			By default it does nothing. */
		virtual void restartLogChannel () = 0;

		protected:
		int _id;
		unsigned int _numberLines;

		// Implementation
		/** The number of lines written in the file. 
			When the counter reaches the maximum a new file is created instead. */
		mutable unsigned int _counter;
	};

	/** The channel based in standard cout. */
	class LogCoutChannel final : public LogChannel
	{
		public:
		LogCoutChannel (int id)
			: LogChannel (id) // There is no limit in lines...
							{ }

		private:
		virtual void writeImplementation (const std::string& str) override
							{ std::cout << str << std::endl; }
		virtual void restartLogChannel () override // Does nothing...
							{ }
	};
	
	/** The channel based in a file. */
	class LogFileChannel final : public LogChannel
	{
		public:
		LogFileChannel (int id, const std::string& fN, 
			unsigned int nL = std::numeric_limits <unsigned int>::max ());

		~LogFileChannel ()
							{ _logFile.close (); }

		private:
		virtual void writeImplementation (const std::string& str) override;
		virtual void restartLogChannel () override;

		private:
		const std::string _fileName;
		std::ofstream _logFile;
	};

	using LogChannels = std::map <int, LogChannel*>;

	/** The system. 
		There must be only one in the whole operation. */
	class LogSystem final
	{
		public:
		static const int _STDLOGCHANNELID = -1; // It can not be changed...

		static std::unique_ptr <LogSystem> _LOGSYSTEM;

		LogSystem (const LogChannels& lF)
			: _defaultChannelId (_STDLOGCHANNELID),
			  _logChannels (lF),
			  _STDLOGCHANNEL (nullptr)
							{ _STDLOGCHANNEL = new LogCoutChannel (_STDLOGCHANNELID); }

		~LogSystem ();

		// Managing the default channel...
		int defaultChannelId () const
							{ return (_defaultChannelId); }
		void setDefaultChannelId (int id)
							{ _defaultChannelId = id; }

		// Managing the list of log channels of the system...
		bool existsLogChannel (int id) const
							{ return (_logChannels.find (id) != _logChannels.end ()); }
		inline void addLogChannel (LogChannel* lC); // Only if the id is not the standard one...

		const LogChannel* logChannel (int id) const // It returns the standard one when it is not found...
							{ return (existsLogChannel (id) 
								? (*_logChannels.find (id)).second : _STDLOGCHANNEL); }
		LogChannel* logChannel (int id)
							{ return (const_cast <LogChannel*> 
								(const_cast <const LogSystem*> (this) -> logChannel (id))); }

		const LogChannel* standardLogChannel (int id) const
							{ return (_STDLOGCHANNEL); }
		LogChannel* standardLogChannel (int id)
							{ return (_STDLOGCHANNEL); }
		const LogChannel* defaultLogChannel () const // Using the defaultLogChannel...
							{ return (logChannel (_defaultChannelId)); }
		LogChannel* defaultLogChannel () // Using the defaultLogChannel...
							{ return (logChannel (_defaultChannelId)); }
		
		void write (int id, const std::string& str)
							{ logChannel (id) -> write (str); }
		void write (const std::string& str)
							{ defaultLogChannel () -> write (str); }

		private:
		int _defaultChannelId;
		LogChannels _logChannels;

		// Implementation
		/** The standard log channel is based in std::cout and has -1 as Id. */
		LogChannel* _STDLOGCHANNEL;
	};

	//  ---
	inline void LogSystem::addLogChannel (LogChannel* lC) // Only if the i
	{ 
		if (lC -> id () != _STDLOGCHANNELID && _logChannels.find (lC -> id ()) == _logChannels.end ())
			_logChannels.insert (LogChannels::value_type (lC -> id (), lC));
	}

// To simpldy the use ofthe log system...
#define _LOGSYS MCHEmul::LogSystem::_LOGSYSTEM
#define _LOG(s) _LOGSYS -> write (s)
#define _LOGC(i,s) _LOGSYS -> write (i, s)
}

#endif
  
// End of the file
/*@}*/
#pragma once
