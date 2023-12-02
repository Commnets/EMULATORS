/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Instructions.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description:	The list of instructions used by any FZX80 family. \n
 *					The no documented instructions have also been included. \n
 *					This file includes many other with the detailed definition per type of instruction.
 *					Follow the definitions at:
 *					https://map.grauw.nl/resources/z80instr.php \n
 *					https://clrhome.org/table/#inc \n
 *					https://wikiti.brandonw.net/index.php?title=Z80_Instruction_Set \n
 *					http://www.z80.info/z80sflag.htm 
 *					http://www.z80.info/zip/z80-documented.pdf // Very important to understand the behaviour final. 
 *															   // Shows mistakes in the original Z80 documentation.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INSTRUCTIONS__
#define __FZX80_INSTRUCTIONS__

#include <FZ80/LD.hpp>
#include <FZ80/STACK.hpp>
#include <FZ80/EX.hpp>
#include <FZ80/INC.hpp>
#include <FZ80/DEC.hpp>
#include <FZ80/ADD.hpp>
#include <FZ80/SUB.hpp>
#include <FZ80/AND.hpp>
#include <FZ80/OR.hpp>
#include <FZ80/XOR.hpp>
#include <FZ80/BITSHIFTLEFT.hpp>
#include <FZ80/BITSHIFTRIGHT.hpp>
#include <FZ80/CP.hpp>
#include <FZ80/ALUCTRL.hpp>
#include <FZ80/BIT.hpp>

#endif
  
// End of the file
/*@}*/
