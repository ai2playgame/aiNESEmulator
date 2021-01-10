#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "create_sprite.h"
#include "NESFile.h"

int main() {
	NESFile nesFile("Alter_Ego.nes");
	nesFile.make_bmp("Alter_Ego.bmp");
}
