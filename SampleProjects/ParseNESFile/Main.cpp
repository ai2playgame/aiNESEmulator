#include <iostream>
#include <fstream>

int main() {
	std::ifstream ifs("sample1.nes", std::ios::in | std::ios::binary);

	if (!ifs) {
		std::cerr << "cannot open the file!" << std::endl;
		return 1;
	}
	
	// iNESƒwƒbƒ_‚Ì\¬: https://wiki.nesdev.com/w/index.php/INES
	struct NesHeader {
		char head[4];
		uint8_t prjRomSize; // Size of PRG ROM in 16 KB units
		uint8_t chrRomSize; // Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
		uint8_t flag6;      // Flags 6 - Mapper, mirroring, battery, trainer
		uint8_t flag7;		// Flags 7 - Mapper, VS/Playchoice, NES 2.0
		uint8_t flag8;      // Flags 8 - PRG-RAM size (rarely used extension)
		uint8_t flag9;		// Flags 9 - TV system (rarely used extension)
		uint8_t flag10;     // Flags 10 - TV system, PRG-RAM presence (unofficial, rarely used extension)
		uint32_t padding;   // 11-15: Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
	};
	
	NesHeader nesHeader;

	char head[4];
	uint8_t PRJSize;
	uint8_t CHRSize;

	std::cout << sizeof(NesHeader) << std::endl;
	ifs.read(head, sizeof(head));
	ifs.read((char*)&PRJSize, sizeof(PRJSize));
	ifs.read((char*)&CHRSize, sizeof(CHRSize));

	std::cout << head << std::endl;
	std::cout << (unsigned)PRJSize << std::endl;
	std::cout << (unsigned)CHRSize << std::endl;

	// ifs.read((char*)&nesHeader, sizeof(NesHeader));
	// ifs.close();

	// std::cout << "head[4]: " << nesHeader.head << std::endl;
	// std::cout << "prjRomSize: " << nesHeader.prjRomSize << std::endl;
	// std::cout << "chrRomSize: " << nesHeader.chrRomSize << std::endl;
}