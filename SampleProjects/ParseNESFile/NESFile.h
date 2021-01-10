#pragma once
#include <cstdint>
#include <string_view>

// iNESÉwÉbÉ_ÇÃç\ê¨: https://wiki.nesdev.com/w/index.php/INES
struct NesHeader {
	char prev[4];
	uint8_t prgRomSize; // Size of PRG ROM in 16 KB units
	uint8_t chrRomSize; // Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
	uint8_t flag6;      // Flags 6 - Mapper, mirroring, battery, trainer
	uint8_t flag7;		// Flags 7 - Mapper, VS/Playchoice, NES 2.0
	uint8_t flag8;      // Flags 8 - PRG-RAM size (rarely used extension)
	uint8_t flag9;		// Flags 9 - TV system (rarely used extension)
	uint8_t flag10;     // Flags 10 - TV system, PRG-RAM presence (unofficial, rarely used extension)
	uint32_t padding;   // 11-15: Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
};

class NESFile {
public:
	NESFile(std::string_view path);
	~NESFile();

	bool make_bmp(std::string_view bmpName);

private:
	NesHeader header;

	uint8_t mapperNumber;
	uint8_t mirroringType;
	uint8_t prgRamBackupFlag;
	uint8_t trainerFlag;
	uint8_t mirrorFlag;

	uint32_t prgSize;
	uint8_t* prgRom;
	uint8_t* chrRom;
	uint32_t chrSize;
};
