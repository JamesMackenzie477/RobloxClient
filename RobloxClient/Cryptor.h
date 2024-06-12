#pragma once
#include <string>
#include "defs.h"

#define BYTE0(x)   BYTEn(x,  0)
#define DWORDn(x, n)   (*((_DWORD*)&(x)+n))
#define DWORD0(x)   DWORDn(x,  0)
#define DWORD1(x)   DWORDn(x,  1)
#define DWORD2(x)   DWORDn(x,  2)
#define DWORD3(x)   DWORDn(x,  3)
#define DWORD4(x)   DWORDn(x,  4)

// Defines the class Roblox uses to encrypt packets.
class Cryptor
{
private:
	// Stores the class attributes.
	char Attributes[0x26C];
	// bool HasSource; // 0x258
public:
	// Creates a cryptor object.
	Cryptor();
	// Sets the cryptor source.
	int Set(char* Source);
	// Encrypts the given buffer.
	// Returns the size of the encrypted bytes.
	size_t * Encrypt(void * InputBuffer, size_t InputSize, void * OutputBuffer, size_t * OutputSize, void * Key);
};

class Builder
{
public:
	int v24 = 0xCE6DD971;
	short v25 = 0x58BF;
	short Unk = 0;
	int v26 = 0;
	void sub_1061990(int a2, size_t Size);
};

/*class Key
{
public:
	char Buffer[2508] = { 0 };
	unsigned int GetValue();
};*/