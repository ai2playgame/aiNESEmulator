#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "create_sprite.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

int main() {
	std::ifstream ifs("sample1.nes", std::ios::in | std::ios::binary);

	if (!ifs) {
		std::cerr << "cannot open the file!" << std::endl;
		return 1;
	}

	// iNESヘッダの構成: https://wiki.nesdev.com/w/index.php/INES
	struct NesHeader {
		char head[4];
		uint8_t prgRomSize; // Size of PRG ROM in 16 KB units
		uint8_t chrRomSize; // Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
		uint8_t flag6;      // Flags 6 - Mapper, mirroring, battery, trainer
		uint8_t flag7;		// Flags 7 - Mapper, VS/Playchoice, NES 2.0
		uint8_t flag8;      // Flags 8 - PRG-RAM size (rarely used extension)
		uint8_t flag9;		// Flags 9 - TV system (rarely used extension)
		uint8_t flag10;     // Flags 10 - TV system, PRG-RAM presence (unofficial, rarely used extension)
		uint32_t padding;   // 11-15: Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
	};

	NesHeader nesHeader;

	ifs.read((char*)&nesHeader, sizeof(NesHeader));

	uint8_t mapperNumber = ((nesHeader.flag6 & 0xf0) >> 4) | (nesHeader.flag7 & 0xf0);
	uint8_t mirroringType = (nesHeader.flag6 & 0x01) == 0x01;
	uint8_t prgRambackupFlag = (nesHeader.flag6 & 0x02) == 0x02;
	uint8_t trainerFlag = (nesHeader.flag6 & 0x04) == 0x04;
	uint8_t mirrorFlag = (nesHeader.flag6 & 0x08) == 0x08;

	// trainer(optional)読み出し
	uint8_t trainer[512];
	if (trainerFlag) {
		std::cout << "trainerFlag is true" << std::endl;
		ifs.read((char*)trainer, sizeof(trainer));
	}

	// PRGとCHRのページサイズ定数
	constexpr uint32_t PRG_DATA_PAGE_SIZE = 16 * 1024; // 16KB
	constexpr uint32_t CHR_DATA_PAGE_SIZE = 8 * 1024; // 8KB
	
	// PRG読み出し
	uint32_t prgSize = PRG_DATA_PAGE_SIZE * nesHeader.prgRomSize;
	uint8_t* prgRom = new uint8_t[prgSize];
	ifs.read((char*)prgRom, prgSize);

	// CHR読み出し
	uint32_t chrSize = CHR_DATA_PAGE_SIZE * nesHeader.chrRomSize; // 16KB * 1 = 16KB
	uint8_t* chrRom = new uint8_t[chrSize];
	ifs.read((char*)chrRom, chrSize);

	ifs.close();
	
	constexpr int BYTE_PER_SPRITE = 16;
	constexpr int SPRITE_COL_SIZE = 8; // 8列
	constexpr int SPRITE_ROW_SIZE = 8; // 8行
	constexpr int FINAL_SPRITE_Y_COUNT = 50;
	constexpr int FINAL_SPRITE_WIDTH = SPRITE_COL_SIZE * FINAL_SPRITE_Y_COUNT; // スプライト50個を横並べ

	const int spriteNum = chrSize / BYTE_PER_SPRITE; // スプライト枚数
	const int height = SPRITE_ROW_SIZE * (spriteNum / FINAL_SPRITE_Y_COUNT + (spriteNum % FINAL_SPRITE_Y_COUNT ? 1 : 0)); // 最終的なスプライトの縦幅

	uint8_t* sprites = new uint8_t[spriteNum * height];
	uint8_t* spritesPtr = sprites;
	
	uint8_t* chrPtr = chrRom;
	// スプライト数分だけループを回す
	for (int spriteIndex = 0; spriteIndex < spriteNum; ++spriteIndex) {
		for (int i = 0; i < SPRITE_ROW_SIZE; ++i) {
			uint8_t spriteRow[SPRITE_COL_SIZE];
			merge_row(*(chrPtr + i), *(chrPtr + i + SPRITE_COL_SIZE), spriteRow);
			
			const int tmp = (spriteIndex / FINAL_SPRITE_Y_COUNT) * FINAL_SPRITE_WIDTH
								+ (spriteIndex % FINAL_SPRITE_Y_COUNT) + (FINAL_SPRITE_Y_COUNT * i);

			memcpy(spritesPtr + SPRITE_COL_SIZE * tmp, spriteRow, SPRITE_COL_SIZE);
		}
		chrPtr += 16;
	}

	stbi_write_bmp("final.bmp", FINAL_SPRITE_WIDTH, height, sizeof(uint8_t), sprites);

	delete[] prgRom;
	delete[] chrRom;
	delete[] sprites;

}
