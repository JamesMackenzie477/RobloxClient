#pragma once
// #include <string>

// Includes various RakNet headers.
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

#include "Cryptor.h"

// Defines various Roblox packet identifiers.
#define ID_ROBLOX_SCHEMA_VERSION (char)0x90
#define ID_ROBLOX_START_AUTH_THREAD (char)0x92
#define ID_ROBLOX_AUTH (char)0x8A
#define ID_ROBLOX_INITIAL_SPAWN_NAME (char)0x8F
#define ID_ROBLOX_REPLICATION (char)0x00

// Allows for extraction of a packet identifier.
#define PACKET_IDENTIFIER(I) ((unsigned char)I->data[0])
// Allows extraction of the server identifier from the given packet.
#define SERVER_TOKEN(I) std::string((reinterpret_cast<char*>(I->data) + 10), 35)
// Treats the given variable as a 16 byte integer.
#define __int128(I) (*reinterpret_cast<__m128i*>(&I))

// Contains various Roblox definitions.
namespace Roblox
{
	// Defines the Roblox packet identifiers.
	enum MessageIDTypes
	{
		ID_SERVER_TOKEN = 0x81,
		ID_CLIENT_TOKEN = 0x83,
		ID_AUTH = 0x8A,
		ID_INITIAL_SPAWN_NAME = 0x8F,
		ID_SCHEMA_VERSION = 0x90,
		ID_START_AUTH_THREAD = 0x92
	};

	// Stores the Roblox server password.
	const char Password[] = {
		0x37, 0x4f, 0x5e, 0x11, 0x6c, 0x45
	};

	// Stores the cryptor source.
	const char Source[] = {
		0xFE, 0xF9, 0xF0, 0xEB, 0xE2, 0xDD, 0xD4, 0xCF, 0xC6, 0xC1, 0xB8, 0xB3, 0xAA, 0xA5, 0x9C, 0x97
	};

	// Defines the start auth thread message.
	const char StartAuthThread[] = {
		Roblox::MessageIDTypes::ID_START_AUTH_THREAD, 0x00
	};

	// Defines the initial spawn name message.
	const char InitialSpawnName[] = {
		Roblox::MessageIDTypes::ID_INITIAL_SPAWN_NAME, 0x00
	};

	// A class that allows interaction with a Roblox server.
	class Client
	{
	private:
		// Stores the client peer instance.
		RakNet::RakPeerInterface* Interface;
		// Stores the client token.
		std::string ClientToken;
		// Stores the server token.
		std::string ServerToken;
		// Stores the encryption key.
		// unsigned char Key[2508];
		void * Key;
		// Called when the client has connected with a server.
		virtual void OnConnect();
		// Called when the client recieves the server token.
		virtual void OnToken();
	public:
		// Creates a new Roblox client.
		Client();
		// Deletes the Roblox client.
		~Client();
		// Connects to the specified Roblox server.
		void Connect(const char * Address, unsigned short Port);
		// Tells the server to start the authorization thread.
		uint32_t StartAuthThread();
		// Sends authentication to the Roblox server.
		uint32_t SendAuth(int64_t UserId, std::string ClientTicket, std::string FirstHash, std::string SecondHash, std::string Platform, std::string Unknown, std::string SessionId);
		// Sends the client token to the Roblox server.
		uint32_t SendToken(std::string Username, std::string AvatarUrl, std::string Platform, std::string Language);
		// Generates an encryption key using the client's external identifier.
		void * GenerateKey();
		// Generates a client token.
		static std::string GenerateToken();
	};

	// Adds a few useful methods to the RakNet BitStream class.
	class BitStream : public RakNet::BitStream
	{
	public:
		// Writes an int32 to the bit stream.
		void WriteInt32(int32_t Value);
		// Writes an int64 to the bit stream.
		void WriteInt64(int64_t Value);
		// Writes a string to the bit stream.
		void WriteString(std::string String);
		// Encrypts the bit stream with the given key.
		// void Encrypt(void * Key);
	};
}