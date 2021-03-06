#include <iostream>
#include <CORE/global.hpp>
#include <CORE/UByte.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/UInt.hpp>
#include <CORE/Address.hpp>

void testUByte ()
{
	std::cout << "Testing UByte" << std::endl;

	MCHEmul::UByte a (0xf0);
	std::cout << "(a:" << a << " " << a.asString (MCHEmul::UByte::OutputFormat::_BINARY) << ")" << std::endl;

	std::cout << "->Printing bit by bit" << std::endl;
	for (int i = (int) a.sizeBits () - 1; i >= 0; i--)
		std::cout << ((i != ((int) a.sizeBits () - 1)) ? "-" : "") << a.bit ((size_t) i);
	std::cout << std::endl;

	std::cout << "->Rotating left" << std::endl;
	for (int i = 0; i < 8; i++)
		std::cout << a.rotateLeft ().asString (MCHEmul::UByte::OutputFormat::_BINARY) << std::endl;

	std::cout << "-----------" << std::endl;
}

void testUBytes ()
{
	std::cout << "Testing UBytes" << std::endl;

	MCHEmul::UBytes a ({ 0x0f, 0xf0, 0x15 });
	std::cout << "(a:" << a << " " << a.asString (MCHEmul::UByte::OutputFormat::_BINARY, ',') << ")" << std::endl;

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
	prt (std::cout, "a", a);
	prt (std::cout, "b", b);
	prt (std::cout, "c", c);
	prt (std::cout, "r", r);
	prt (std::cout, "t", t);

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
	MCHEmul::UInt k ({ MCHEmul::UByte::_F });
	for (unsigned int ct = 0; ct < 10; ct++)
	{
		prt (std::cout, "k", k);
		k.resetOverflow (); k.resetCarry ();
		k -= MCHEmul::UInt::_1;
	}

	std::cout << "->Substracting progresively" << std::endl;
	MCHEmul::UInt l ({ MCHEmul::UByte::_0 });
	for (unsigned int ct = 0; ct < 10; ct++)
	{
		prt (std::cout, "l", l);
		k.resetOverflow (); k.resetCarry ();
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

void testAddress ()
{
	std::cout << "Testing Address" << std::endl;

	MCHEmul::Address a ({ 0x00, 0xfA });
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

int main()
{
	testUByte ();
	testUBytes ();
	testUInt ();
	testAddress ();
}
