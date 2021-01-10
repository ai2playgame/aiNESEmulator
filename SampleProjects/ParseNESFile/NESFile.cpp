#include "NESFile.h"
#include <fstream>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace {

constexpr uint32_t PRG_PAGE_SIZE = 16 * 1024; //16KB
constexpr uint32_t CHR_PAGE_SIZE = 8 * 1024; // 8KB

constexpr int BYTE_PER_SPRITE = 16;
constexpr int SPRITE_COL_SIZE = 8; // 8列
constexpr int SPRITE_ROW_SIZE = 8; // 8行
constexpr int FINAL_SPRITE_X_COUNT = 50;
constexpr int FINAL_SPRITE_WIDTH = SPRITE_COL_SIZE * FINAL_SPRITE_X_COUNT;

void merge_row(const uint8_t lhs, const uint8_t rhs, uint8_t* buf) {
	for (int i = 7; i >= 0; --i) {
		uint8_t lhs_mask = lhs & (0x1 << i);
		uint8_t rhs_mask = rhs & (0x1 << i);
		
		uint8_t pixVal;
		if (!(lhs_mask) && !(rhs_mask)) // 00
		{
			pixVal = 0;
		}
		else if (lhs_mask && !(rhs_mask)) // 01
		{
			pixVal = 0x55;
		}
		else if (!(lhs_mask) && rhs_mask) // 10
		{
			pixVal = 0xaa;
		}
		else { // 11
			pixVal = 0xff;
		}
		buf[7 - i] = pixVal;
	}
}
}

NESFile::NESFile(std::string_view path)
    : header{}
{
    std::ifstream ifs(path.data(), std::ios::in | std::ios::binary);
    ifs.seekg(0, std::ios_base::end);
    auto fsize = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    // ヘッダ読み出し
    if (fsize - ifs.tellg() < sizeof(header)) {
        throw std::exception("[ERROR] Invalid file size!");
    }
    ifs.read((char*)&header, sizeof(NesHeader));

    // ヘッダ解析
	mapperNumber = ((header.flag6 & 0xf0) >> 4) | (header.flag7 & 0xf0);
	mirroringType = (header.flag6 & 0x01) == 0x01;
	prgRamBackupFlag = (header.flag6 & 0x02) == 0x02;
	trainerFlag = (header.flag6 & 0x04) == 0x04;
	mirrorFlag = (header.flag6 & 0x08) == 0x08;

    // trainer読み出し
    uint8_t trainer[512];
    if (trainerFlag) {
        if (fsize - ifs.tellg() < sizeof(trainer)) {
            throw std::exception("[ERROR] Invalid file size!");
        }
        ifs.read((char*)trainer, sizeof(trainer));
    }

    // PRG読み出し
    prgSize = PRG_PAGE_SIZE * header.prgRomSize;
    uint8_t* prgRomBuf = new uint8_t[prgSize];
    if (fsize - ifs.tellg() < prgSize) {
        throw std::exception("[ERROR] Invalid file size!");
    }
    ifs.read((char*)prgRomBuf, prgSize);
    this->prgRom = prgRomBuf;

    // CHR読み出し
    chrSize = CHR_PAGE_SIZE * header.chrRomSize;
    uint8_t* chrRomBuf = new uint8_t[chrSize];
    if (fsize - ifs.tellg() < chrSize) {
        throw std::exception("[ERROR] Invalid file size!");
    }
    ifs.read((char*)chrRomBuf, chrSize);
    this->chrRom = chrRomBuf;

    ifs.close();
}

NESFile::~NESFile()
{
    delete[] this->prgRom;
    delete[] this->chrRom;
}

bool NESFile::make_bmp(std::string_view bmpName)
{
    const int spriteNum = chrSize / BYTE_PER_SPRITE;
	const int height = SPRITE_ROW_SIZE * (spriteNum / FINAL_SPRITE_X_COUNT + (spriteNum % FINAL_SPRITE_X_COUNT ? 1 : 0)); // 最終的なスプライトの縦幅

	uint8_t* sprites = new uint8_t[spriteNum * height];
	uint8_t* spritesPtr = sprites;
	
	uint8_t* chrPtr = chrRom;
	// スプライト数分だけループを回す
	for (int spriteIndex = 0; spriteIndex < spriteNum; ++spriteIndex) {
		for (int i = 0; i < SPRITE_ROW_SIZE; ++i) {
			uint8_t spriteRow[SPRITE_COL_SIZE];
			merge_row(*(chrPtr + i), *(chrPtr + i + SPRITE_COL_SIZE), spriteRow);
			
			const int offset = (spriteIndex / FINAL_SPRITE_X_COUNT) * FINAL_SPRITE_WIDTH
								+ (spriteIndex % FINAL_SPRITE_X_COUNT) + (FINAL_SPRITE_X_COUNT * i);

			memcpy(spritesPtr + SPRITE_COL_SIZE * offset, spriteRow, SPRITE_COL_SIZE);
		}
		chrPtr += (2 * SPRITE_COL_SIZE);
	}

	stbi_write_bmp(bmpName.data(), FINAL_SPRITE_WIDTH, height, sizeof(uint8_t), sprites);

	delete[] sprites;

    return true;
}

