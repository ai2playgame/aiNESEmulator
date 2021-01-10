#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "NESFile.h"

int main() {
	NESFile nesFile("Alter_Ego.nes");
	nesFile.make_bmp("Alter_Ego.bmp");
}
