#include "stdafx.h"

#include <iostream>

#include <CORE/incs.hpp>

void testUByte ()
{
	std::cout << "Testing UByte" << std::endl;

	MCHEmul::UByte a (0xf0);
	MCHEmul::UByte b (0x10);
	MCHEmul::UByte c (0x90);
	MCHEmul::UByte d (0x7f);
	MCHEmul::UByte e (0x40);
	MCHEmul::UByte u (0x01);
	std::cout << "(a:" << a << " " << a.asString (MCHEmul::UByte::OutputFormat::_BINARY) << ")" << std::endl;
	std::cout << "(b:" << b << " " << b.asString (MCHEmul::UByte::OutputFormat::_BINARY) << ")" << std::endl;
	std::cout << "(c:" << c << " " << c.asString (MCHEmul::UByte::OutputFormat::_BINARY) << ")" << std::endl;
	std::cout << "(d:" << d << " " << d.asString (MCHEmul::UByte::OutputFormat::_BINARY) << ")" << std::endl;

	std::cout << "->Printing bit by bit" << std::endl;
	for (int i = (int) a.sizeBits () - 1; i >= 0; i--)
		std::cout << ((i != ((int) a.sizeBits () - 1)) ? "-" : "") << a.bit ((size_t) i);
	std::cout << std::endl;

	std::cout << "->Rotating left" << std::endl;
	for (int i = 0; i < 8; i++)
		std::cout << a.rotateLeft ().asString (MCHEmul::UByte::OutputFormat::_BINARY) << std::endl;

	std::cout << "->Adding Binary" << std::endl;
	bool cr = false; bool ov = false;
	MCHEmul::UByte r1 = a.bitAdding (b, false, cr, ov);
	std::cout << "a + b: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY) << "(" 
		<< (cr ? 1 : 0) << "," << (ov ? 1 : 0) << ")" << std::endl;
	r1 = a.bitAdding (c, false, cr, ov);
	std::cout << "a + c: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY) << "(" 
		<< (cr ? 1 : 0) << "," << (ov ? 1 : 0) << ")" << std::endl;
	r1 = b.bitAdding (d, false, cr, ov);
	std::cout << "b + d: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY) << "(" 
		<< (cr ? 1 : 0) << "," << (ov ? 1 : 0) << ")" << std::endl;

	std::cout << "->Simulating Binary Substraction" << std::endl;
	r1 = a.bitAdding (b.complement ().bitAdding (MCHEmul::UByte::_1, false, cr, ov), false, cr, ov);
	std::cout << "a - b: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY) << "(" 
		<< (cr ? "positive" : "negative") << "," << (ov ? 1 : 0) << ")" << std::endl;
	r1 = b.bitAdding (a.complement ().bitAdding (MCHEmul::UByte::_1, false, cr, ov), false, cr, ov);
	std::cout << "b - a: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY) << "(" 
		<< (cr ? "positive" : "negative") << "," << (ov ? 1 : 0) << ")" << std::endl;

	std::cout << "-----------" << std::endl;
}

void testUBytes ()
{
	std::cout << "Testing UBytes" << std::endl;

	MCHEmul::UBytes a ({ 0x0f, 0xf0, 0x15 });
	MCHEmul::UBytes b ({ 0x00, 0x00, 0xf0 });
	MCHEmul::UBytes c ({ 0x00, 0x10, 0xf0 });
	std::cout << "(a:" << a << " " << a.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << ")" << std::endl;
	std::cout << "(b:" << b << " " << b.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << ")" << std::endl;
	std::cout << "(c:" << c << " " << c.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << ")" << std::endl;

	std::cout << "->Printing bit by bit" << std::endl;
	for (int i = (int) a.sizeBits () - 1; i >= 0; i--)
		std::cout << ((i != ((int) a.sizeBits () - 1)) ? "-" : "") << a.bit ((size_t) i);
	std::cout << std::endl;

	std::cout << "->Rotating left" << std::endl;
	for (int i = 0; i < 8; i++)
		std::cout << a.rotateLeft ().asString (MCHEmul::UByte::OutputFormat::_BINARY, '-') << std::endl;

	std::cout << "->Rotating right" << std::endl;
	for (int i = 0; i < 8; i++)
		std::cout << a.rotateRight ().asString (MCHEmul::UByte::OutputFormat::_BINARY, '-') << std::endl;

	std::cout << "->Adding Binary" << std::endl;
	bool cr = false; bool ov = false;
	MCHEmul::UBytes r1 = a.bitAdding (b, false, cr, ov);
	std::cout << "a + b: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << "(" 
		<< (cr ? 1 : 0) << "," << (ov ? 1 : 0) << ")" << std::endl;
	r1 = a.bitAdding (c, false, cr, ov);
	std::cout << "a + c: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << "(" 
		<< (cr ? 1 : 0) << "," << (ov ? 1 : 0) << ")" << std::endl;

	std::cout << "->Simulating Binary Substraction" << std::endl;
	r1 = a.bitAdding (b.complement ().bitAdding (MCHEmul::UBytes ({ MCHEmul::UByte::_1 }), false, cr, ov), false, cr, ov);
	std::cout << "a - b: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << "(" 
		<< (cr ? "positive" : "negative") << "," << (ov ? 1 : 0) << ")" << std::endl;
	r1 = b.bitAdding (a.complement ().bitAdding (MCHEmul::UBytes ({ MCHEmul::UByte::_1 }), false, cr, ov), false, cr, ov);
	std::cout << "b - a: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << "(" 
		<< (cr ? "positive" : "negative") << "," << (ov ? 1 : 0) << ")" << std::endl;
	r1 = r1.complement ().bitAdding (MCHEmul::UBytes ({ MCHEmul::UByte::_1 }), false, cr, ov);
	std::cout << "final result: " << r1 << " " << r1.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << "(" 
		<< (cr ? 1 : 0) << "," << (ov ? 1 : 0) << ")" << std::endl;

	std::cout << "-----------" << std::endl;
}

void testUInt ()
{
	auto prt = [](std::ostream& o, const std::string& nU, const MCHEmul::UInt& u) -> std::ostream&
		{ o << "(" << nU << ":" << u << " V:" << u.overflow () << " C:" << u.carry () 
			<< " unsigned:" << u.asUnsignedInt () << " signed: " << u.asInt () << ")" << std::endl; 
		  return (o); };

	std::cout << "Testing UInt" << std::endl;

	MCHEmul::UInt a ({ 0x0f, 0xf0 });
	MCHEmul::UInt b ({ 0x0f, 0x0f });
	MCHEmul::UInt c ({ 0xff, 0x0f });
	MCHEmul::UInt r = MCHEmul::UInt::fromUnsignedInt (4215);
	MCHEmul::UInt t = MCHEmul::UInt::fromInt (-4215);
	MCHEmul::UInt t1 = MCHEmul::UInt::fromInt (-64215);
	prt (std::cout, "a", a);
	prt (std::cout, "b", b);
	prt (std::cout, "c", c);
	prt (std::cout, "r", r);
	prt (std::cout, "t", t);
	prt (std::cout, "t1", t1);

	std::string n1S = "", n2S = "", op = "";
	while (n1S != "exit")
	{
		std::cout << "N1:"; std::cin >> n1S; 
		MCHEmul::UInt n1 = MCHEmul::UInt::fromStr (n1S, MCHEmul::UInt::_BINARY);
		if (n1S == "exit")
			break;

		prt (std::cout, "N1", n1);
		std::cout << "N2:"; std::cin >> n2S;
		MCHEmul::UInt n2 = MCHEmul::UInt::fromStr (n2S, MCHEmul::UInt::_BINARY);
		prt (std::cout, "N2", n2);
		std::cout << "Op:"; std::cin >> op;
		prt (std::cout, "Result", op == "-" ? n1 - n2 : n1 + n2);
	}

	std::cout << "->Adition with and without overflow" << std::endl;
	MCHEmul::UInt d = a + b;
	prt (std::cout, "a + b", d);
	MCHEmul::UInt e = d + c;
	prt (std::cout, "a + b + c", e);

	std::cout << "->Substracting step by step" << std::endl;
	MCHEmul::UInt f = b.complement_2 ();
	prt (std::cout, "-b", f);
	MCHEmul::UInt g = a + f;
	prt (std::cout, "a - b", g);

	std::cout << "->Substracting step by step" << std::endl;
	MCHEmul::UInt h = a.complement_2 ();
	prt (std::cout, "-a", h);
	MCHEmul::UInt i = h + b;
	prt (std::cout, "(-a) + b", i);

	std::cout << "->Substracting with formula" << std::endl;
	i = a - b;
	prt (std::cout, "a - b", i);
	i = -b + a;
	prt (std::cout, "-b + a", i);

	std::cout << "->Substracting with formula" << std::endl;
	i = b - a;
	prt (std::cout, "b - a", i);
	i = -a + b;
	prt (std::cout, "-a + b", i);

	std::cout << "->Substracting progresively" << std::endl;
	MCHEmul::UInt k ({ MCHEmul::UByte::_FF });
	for (unsigned int ct = 0; ct < 10; ct++)
	{
		prt (std::cout, "k", k);
		k.resetOverflow (); k.resetCarry ();
		k -= MCHEmul::UInt::_1;
	}

	std::cout << "->Adding progresively" << std::endl;
	MCHEmul::UInt l ({ MCHEmul::UByte::_0 });
	for (unsigned int ct = 0; ct < 10; ct++)
	{
		prt (std::cout, "l", l);
		l.resetOverflow (); l.resetCarry ();
		l += MCHEmul::UInt::_1;
	}

	std::cout << "->Out of range" << std::endl;
	prt (std::cout, "-127-64:", MCHEmul::UInt::fromInt (-127) + MCHEmul::UInt::fromInt (-64));
	prt (std::cout, "+127+5:", MCHEmul::UInt::fromInt (127) + MCHEmul::UInt::fromInt (5));

	std::cout << "->Comparations" << std::endl;
	if (a > b) std::cout << "(a:" << a << " bigger than b:" << b << ")" << std::endl;
	else std::cout << "(b:" << b << " bigger than a:" << a << ")" << std::endl;
	if (b > a) std::cout << "(b:" << b << " bigger than a:" << a << ")" << std::endl;
	else std::cout << "(a:" << a << " bigger than b:" << b << ")" << std::endl;
	if (a > c) std::cout << "(a:" << a << " bigger than c:" << c << ")" << std::endl;
	else std::cout << "(c:" << c << " bigger than a:" << a << ")" << std::endl;
	if (a > g) std::cout << "(a:" << a << " bigger than a - b:" << g << ")" << std::endl;
	else std::cout << "(a - b:" << g << " bigger than a:" << a << ")" << std::endl;

	std::cout << "-----------" << std::endl;
}

void testUIntBCD ()
{
	auto ADD_emulator = [](unsigned char u1, unsigned char u2, bool cIn, bool& cOut, bool& o) -> unsigned char
	{ 
		unsigned short t = (u1 & 0xf) + (u2 & 0xf) + (cIn ? 1 : 0);
		if (t > 0x09) 
			t += 0x6;
		t += (u1 & 0xf0) + (u2 & 0xf0);
		if ((t & 0x1f0) > 0x90) 
			t += 0x60;

		cOut = t > 0xff;
		o = !((u1 ^ t) & 0x80) && ((u1 ^ u2) & 0x80);

		return ((unsigned char) t);
	};

	auto SBC_emulator = [](unsigned char u1, unsigned char u2, bool cIn, bool& cOut, bool& o) -> unsigned char
	{ 
		unsigned short t = (u1 & 0xf) - (u2 & 0xf) - (cIn ? 0 : 1);
		if ((t & 0x10) != 0)
			t = ((t - 0x6) & 0xf) | ((u1 & 0xf0) - (u2 & 0xf0) - 0x10);
		else
		t = (t & 0xf) | ((u1 &0xf0 ) - (u2 & 0xf0));
		if ((t & 0x100) !=0)
			t -= 0x60;

		cOut = t < 0x100;
		o = ((u1 ^ t) & 0x80) && ((u1 ^ u2) & 0x80);

		return ((unsigned char) t);
	};

	auto prt = [](std::ostream& o, const std::string& nU, const MCHEmul::UInt& u) -> std::ostream&
		{ o << "(" << nU << ":" << u << " V:" << u.overflow () << " C:" << u.carry () 
			<< " unsigned:" << u.asUnsignedInt () << " signed: " << u.asInt () << ") " << "binary:" 
			<< u.asString (MCHEmul::UByte::OutputFormat::_BINARY,'.') << std::endl; 
		  return (o); };

	std::cout << "Testing UInt BCD" << std::endl;

	MCHEmul::UInt a ({ 0x11 }, true, true /** BCD */);
	MCHEmul::UInt b ({ 0x05 }, true, true);
	MCHEmul::UInt c ({ 0x48 }, true, true);
	MCHEmul::UInt r = MCHEmul::UInt::fromUnsignedInt (72, MCHEmul::UInt::_PACKAGEDBCD);
	MCHEmul::UInt t = MCHEmul::UInt::fromInt (-27, MCHEmul::UInt::_PACKAGEDBCD);
	prt (std::cout, "a", a);
	prt (std::cout, "b", b);
	prt (std::cout, "c", c);
	prt (std::cout, "r", r);
	prt (std::cout, "t", t);

	std::string n1S = "", n2S = "", op = "";
	while (n1S != "exit")
	{
		std::cout << "N1:"; std::cin >> n1S; 
		MCHEmul::UInt n1 = MCHEmul::UInt::fromStr (n1S, MCHEmul::UInt::_PACKAGEDBCD);
		if (n1S == "exit")
			break;

		prt (std::cout, "N1", n1);
		std::cout << "N2:"; std::cin >> n2S;
		MCHEmul::UInt n2 = MCHEmul::UInt::fromStr (n2S, MCHEmul::UInt::_PACKAGEDBCD);
		prt (std::cout, "N2", n2);
		std::cout << "Op:"; std::cin >> op;

		{
			bool cOut, o;
			unsigned char r;
			if (op == "+")
				r = ADD_emulator (n1 [0].value (), n2 [0].value (), false, cOut, o);
			else
				r = SBC_emulator (n1 [0].value (), n2 [0].value (), true, cOut, o);

			std::cout <<"EMULATOR: " << (unsigned int) r << "(" << cOut << "," << o << ")" << std::endl;
		}

		prt (std::cout, "Result", op == "-" ? n1 - n2 : n1 + n2);
	}

	std::cout << "->Adition with and without overflow" << std::endl;
	MCHEmul::UInt d = a + b;
	prt (std::cout, "a + b", d);
	MCHEmul::UInt e = d + c;
	prt (std::cout, "a + b + c", e);
	MCHEmul::UInt f = e + c;
	prt (std::cout, "a + b + 2c", f);

	std::cout << "->Substracting step by step" << std::endl;
	MCHEmul::UInt g = b.complement_2 ();
	prt (std::cout, "-b", g);
	MCHEmul::UInt h = a + g;
	prt (std::cout, "a - b", h);

	MCHEmul::UInt i;
	std::cout << "->Substracting with formula" << std::endl;
	i = a - b;
	prt (std::cout, "a - b", i);
	i = -b + a;
	prt (std::cout, "-b + a", i);

	std::cout << "->Substracting with formula" << std::endl;
	i = b - a;
	prt (std::cout, "b - a", i);
	i = -a + b;
	prt (std::cout, "-a + b", i);

	std::cout << "->Substracting progresively" << std::endl;
	MCHEmul::UInt k ({ MCHEmul::UByte::_90 }, true, true);
	for (unsigned int ct = 0; ct < 10; ct++)
	{
		prt (std::cout, "k", k);
		k.resetOverflow (); k.resetCarry ();
		k -= MCHEmul::UInt ({ MCHEmul::UByte::_1 }, true, true);
	}

	std::cout << "->Substracting progresively" << std::endl;
	MCHEmul::UInt l ({ MCHEmul::UByte::_0 }, true, true);
	for (unsigned int ct = 0; ct < 10; ct++)
	{
		prt (std::cout, "l", l);
		l.resetOverflow (); k.resetCarry ();
		l += MCHEmul::UInt ({ MCHEmul::UByte::_1 }, true, true);
	}

	std::cout << "->Out of range" << std::endl;
	prt (std::cout, "-49-32:", 
		MCHEmul::UInt::fromInt (-49, MCHEmul::UInt::_PACKAGEDBCD) + 
		MCHEmul::UInt::fromInt (-32, MCHEmul::UInt::_PACKAGEDBCD));
	prt (std::cout, "+49+5:", 
		MCHEmul::UInt::fromInt (49, MCHEmul::UInt::_PACKAGEDBCD) + 
		MCHEmul::UInt::fromInt (5, MCHEmul::UInt::_PACKAGEDBCD));

	std::cout << "->Comparations" << std::endl;
	if (a > b) std::cout << "(a:" << a << " bigger than b:" << b << ")" << std::endl;
	else std::cout << "(b:" << b << " bigger than a:" << a << ")" << std::endl;
	if (b > a) std::cout << "(b:" << b << " bigger than a:" << a << ")" << std::endl;
	else std::cout << "(a:" << a << " bigger than b:" << b << ")" << std::endl;
	if (a > c) std::cout << "(a:" << a << " bigger than c:" << c << ")" << std::endl;
	else std::cout << "(c:" << c << " bigger than a:" << a << ")" << std::endl;
	if (a > g) std::cout << "(a:" << a << " bigger than a - b:" << g << ")" << std::endl;
	else std::cout << "(a - b:" << g << " bigger than a:" << a << ")" << std::endl;

	std::cout << "-----------" << std::endl;
}

void testAddress ()
{
	std::cout << "Testing Address" << std::endl;

	MCHEmul::Address a ({ 0x00, 0xfA }, false);
	MCHEmul::Address b ({ 0x00, 0x05 }, false);

	std::cout << "->The addresses to be used" << std::endl;
	std::cout << "(a:" << a << ")" <<  std::endl;
	std::cout << "(b:" << b << ")" <<  std::endl;

	std::cout << "->Iterating over the addresses (next)" << std::endl;
	for (size_t i = 0; i < 10; i++)
		std::cout << "(a:" << a.next (i) << ")" << std::endl;

	std::cout << "->Iterating over the addresses (previous)" << std::endl;
	for (size_t i = 0; i < 10; i++)
		std::cout << "(" << b.previous (i) << ")" << std::endl;

	MCHEmul::Address c ({ 0xfa });
	MCHEmul::Address d ({ 0x05 });

	std::cout << "->The addresses to be used (page 0)" << std::endl;
	std::cout << "(c:" << c << ")" <<  std::endl;
	std::cout << "(d:" << d << ")" <<  std::endl;

	std::cout << "->Iterating over the addresses (next)" << std::endl;
	for (size_t i = 0; i < 10; i++)
		std::cout << "(" << c.next (i) << ")" << std::endl;

	std::cout << "->Iterating over the addresses (previous)" << std::endl;
	for (size_t i = 0; i < 10; i++)
		std::cout << "(" << d.previous (i) << ")" << std::endl;

	std::cout << "-----------" << std::endl;
}

int _tmain (int argc, _TCHAR *argv [])
{
	testUByte ();
	testUBytes ();
	testUInt ();
	testUIntBCD ();
	testAddress ();
	
	return (0);
}
