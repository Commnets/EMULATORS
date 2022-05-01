#include <iostream>
#include <global.hpp>
#include <core/UByte.hpp>
#include <core/UBytes.hpp>
#include <core/UInt.hpp>
#include <core/Address.hpp>

void testUByte ()
{
	MCHEmul::UByte a (0xf0);

	for (int i = (int) a.sizeBits () - 1; i >= 0; i--)
		std::cout << ((i != ((int) a.sizeBits () - 1)) ? "-" : "") << a.bit ((size_t) i);
	std::cout << std::endl;

	std::cout << a << std::endl;
	std::cout << a.asString (MCHEmul::UByte::OutputFormat::_BINARY) << std::endl;

	for (int i = 0; i < 8; i++)
		std::cout << a.rotateLeft ().asString (MCHEmul::UByte::OutputFormat::_BINARY) << std::endl;
	std::cout << std::endl;
}

void testUBytes ()
{
	MCHEmul::UBytes a ({ 0x0f, 0xf0, 0x15 });

	for (int i = (int) a.sizeBits () - 1; i >= 0; i--)
		std::cout << ((i != ((int) a.sizeBits () - 1)) ? "-" : "") << a.bit ((size_t) i);
	std::cout << std::endl;

	std::cout << a << std::endl;
	std::cout << a.asString (MCHEmul::UByte::OutputFormat::_BINARY, '-') << std::endl;

	for (int i = 0; i < 8; i++)
		std::cout << a.rotateLeft ().asString (MCHEmul::UByte::OutputFormat::_BINARY, '-') << std::endl;
	std::cout << std::endl;

	for (int i = 0; i < 8; i++)
		std::cout << a.rotateRight ().asString (MCHEmul::UByte::OutputFormat::_BINARY, '-') << std::endl;
	std::cout << std::endl;
}

void testUInt ()
{
	MCHEmul::UInt a ({ 0x0f, 0xf0 });
	MCHEmul::UInt b ({ 0x0f, 0x0f });
	MCHEmul::UInt c ({ 0xff, 0x0f });
	MCHEmul::UInt r = MCHEmul::UInt::fromUnsignedInt (4215);

	std::cout << "(" << a << "),(" << b << "),(" << c << ")" << std::endl;
	std::cout << "(" << a.asUnsignedInt () << "),(" << b.asUnsignedInt () << "),(" << c.asUnsignedInt () << ")" << std::endl;
	std::cout << "(" << r << ")" << std::endl;
	std::cout << std::endl;

	MCHEmul::UInt d = a + b;
	std::cout << "(" << d << ")" << std::endl;
	MCHEmul::UInt e = d + c;
	std::cout << "(" << e << ")" << std::endl;
	std::cout << std::endl;

	MCHEmul::UInt f = b.complement_2 ();
	std::cout << "(" << f << ")" << std::endl;
	MCHEmul::UInt g = a + f;
	bool pos1 = g.size () > a.size ();
	std::cout << "(" << (pos1 ? "+" : "-") << g << ")" << std::endl;
	std::cout << std::endl;

	MCHEmul::UInt h = a.complement_2 ();
	std::cout << "(" << h << ")" << std::endl;
	MCHEmul::UInt i = h + b;
	bool pos2 = i.size () > a.size ();
	std::cout << "(" << i << ")" << std::endl;
	std::cout << "(" << (pos2 ? "+" : "-") << i.complement_2 () << ")" << std::endl;
	std::cout << std::endl;

	if (a > b) std::cout << "(" << a << " bigger " << b << ")" << std::endl;
	if (b > a) std::cout << "(" << b << " bigger " << a << ")" << std::endl;
	if (a > g) std::cout << "(" << a << " bigger " << g << ")" << std::endl;
}

void testAddress ()
{
	MCHEmul::Address a ({ 0x00, 0xfA }, { 0x00, 0xfd });
	MCHEmul::Address b ({ 0x00, 0x05 }, { 0x00, 0xfd }, false);

	std::cout << "(" << a << ")" <<  std::endl;
	std::cout << "(" << b << ")" <<  std::endl;
	std::cout << std::endl;

	for (size_t i = 0; i < 10; i++)
		std::cout << "(" << a.next (i) << ")" << std::endl;
	std::cout << std::endl;

	for (size_t i = 0; i < 10; i++)
		std::cout << "(" << b.previous (i) << ")" << std::endl;
	std::cout << std::endl;

	MCHEmul::Address c ({ 0xfa }, { 0xff }, true);
	MCHEmul::Address d ({ 0x05 }, { 0xff }, true);

	for (size_t i = 0; i < 10; i++)
		std::cout << "(" << c.next (i) << ")" << std::endl;
	std::cout << std::endl;

	for (size_t i = 0; i < 10; i++)
		std::cout << "(" << d.previous (i) << ")" << std::endl;
	std::cout << std::endl;
}

int main()
{
	testUByte ();
	testUBytes ();
	testUInt ();
	testAddress ();
}
