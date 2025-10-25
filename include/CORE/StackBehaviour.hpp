/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: StackBehaviour.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/10/2025 \n
 *	Description: Defines the behaviour of the stack.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STACKBEHAVIOUR__
#define __MCHEMUL_STACKBEHAVIOUR__

#include <CORE/Memory.hpp>

namespace MCHEmul
{
	class StackBehaviour : public InfoClass
	{
		public:
		/** The configuration of the way the stack works. 
			The parameters can be accessed directly. */
		struct Configuration
		{
			Configuration ()
				: _fromBack (true),
				  _pointToEmpty (true),
				  _detectOverflow (true),
				  _maxSize (-1) // meaning the limit position of the memory...
							{ }

			Configuration (bool b, bool e, bool o, int s)
				: _fromBack (b), 
				  _pointToEmpty (e),
				  _detectOverflow (o),
				  _maxSize (s)
							{ }

			/** To start to insert from the las position backwards. */
			bool _fromBack;
			/** The stack pointer is always pointing to the empty position,
				or it is pointing to the last written one? .*/
			bool _pointToEmpty;
			/* *Is it needed to detect when the stack pointer goes out of limits? */
			bool _detectOverflow;
			/** Which is ths max size of the stack pointer. 
				-1 will mean that that max limit is the limit of the memory where the stacj is defeined. */
			int _maxSize; // Like the max position in address...
		};

		/**
		  * Constructor:
		  * The ones needed by the parent class.
		  * @param cfg	: The way the stack works. Look above.
		  */
		StackBehaviour (int id, Address& bA, size_t s, 
				const Configuration& cfg = Configuration ())
			: MCHEmul::InfoClass ("StackBehaviour"),
			  _id (id),
			  _baseAddress (bA),
			  _stackSize (s),
			  _position (0),
			  _configuration (cfg),
			  _overflow (false),
			  _notUsed (true)
							{ }

		// To get the internal information...
		int id () const
							{ return (_id); }
		const Address& baseAddress () const
							{ return (_baseAddress); }
		/** To get the size of the stack. */
		size_t stackSize () const
							{ return (_stackSize); }
							
		// Managing the positio....
		int position () const
							{ return (_position); }
		/** Take care using this method. No checks are done on regards to the speed. 
			And it is not the same a back-stack than a foward one. \n
			The r parameter is very important:
			In some processor the stack can be located in any place in the memory 
			and there are instructions to set where the stack pointer is pointing to. 
			In that computers the value p received will be absolute and the r must be FALSE. 
			This is because _position is always managed internally as a relative value. */
		void setPosition (int p, bool r = true)
							{ _position = (r) ? p : p - baseAddress ().value (); }
		/** To get where the stack is now pointing. */
		Address currentAddress () const
							{ return (_baseAddress + _position); }
		/** To move to the beginning. */
		void reset ();
		
		/** To get the configuration of the stack. 
			Omnce the configuration is set can not be changed!. */
		const Configuration& configuration () const
							{ return (_configuration); }

		/** To manage the stack. */
		void push (const UBytes& v); // When overflow no more is entered...
		UBytes pull (size_t nV); // When overflow am empty object is returned...

		/** It is true when the stack is empty. */
		bool empty () const
							{ return (_notUsed 
								? true 
								: (_configuration._fromBack 
									? (_position == (int) (_stackSize - 1)) 
									: (_position == 0))); }
		/** It is set to true when something happens after push or pull actions. */
		bool overflow () const
							{ return (_overflow); }
		/** To simplify the way the stack status is enquired. */
		bool operator ! () const
							{ return (_overflow); }

		/** To know whether it has ben already used or not. */
		bool notUsed () const
							{ return (_notUsed); }
		/** This is a very rare method. Use carefully. \n
			The default behaviour of a non-pointing-to-empty stack when it hasn't been used ever,
			but this behaviour is not the good one in some implementations, like Z80. */
		void setNotUsed (bool nU)
							{ _notUsed = nU; }

		/**
		  *	The name of the fields are: \n
		  * MEMORY		= InfoStructure: Information about the memory itself. \n
		  *	BACK		= Attribute: YES when moves from back to the top and NO in the other circunstance. \n
		  *	LAST		= Attribute: YES when the empty space is pointing to the last data. \n
		  *	OVERFLOW	= Attribute: YES when no more elements are admitted. \n
		  *	EMPTY		= Attribute: YES when there is no elements inside. \n
		  *	POSITION	= Attribute: Current position in the stack.
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		// To be overloaded with the specific implemetation....
		// ...the stack can be implemented as a specific part of the memory or not...
		/** To keep the value in the memory. */
		virtual void keep (const Address& a, const UByte& dt) = 0;
		/** TO frecover avalue from the memory. */
		virtual const UByte& recover (const Address& a) const = 0;
		
		// Implementation
		inline void decrementStackBehaviourPosition ();
		inline void incrementStackBehaviourPosition ();

		protected:
		int _id;
		Address _baseAddress;
		size_t _stackSize;
		int _position;
		const Configuration _configuration; // Asjusted at the constructor...

		// Implementation
		bool _overflow;
		bool _notUsed; // Has it ever been used?
	};

	// ---
	inline void StackBehaviour::decrementStackBehaviourPosition ()
	{
		auto SSIZE = [&]() -> int
			{ return ((_configuration._maxSize == -1) 
				? (int) _stackSize : _configuration._maxSize); };

		if (--_position < 0)
		{
			if (_configuration._detectOverflow)
			{
				_overflow = true;
				_position = -1; // To be sure...
			}
			else
				_position += SSIZE ();
		}
	}

	// ---
	inline void StackBehaviour::incrementStackBehaviourPosition ()
	{
		auto SSIZE = [&]() -> int
			{ return ((_configuration._maxSize == -1) 
				? (int) _stackSize : _configuration._maxSize); };

		if (++_position >= SSIZE ())
		{
			if (_configuration._detectOverflow)
			{
				_overflow = true;
				_position = -1; // To be sure...
			}
			else
				_position -= SSIZE ();
		}
	}
}

#endif
  
// End of the file
/*@}*/
