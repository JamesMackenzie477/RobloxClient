#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include <iostream>
#include "defs.h"
#include "Cryptor.h"
#include "Roblox.h"

using namespace RakNet;
using namespace std;

// Server password contained at the end of the "Connection Request" packet send by RakNet.
// Force client password into studio connection, use params -server and -port also.
// DDoS servers by creating loads of connections.
// Third packet sent by Roblox verifies client, find out how it is generated using hooks.
// Hook all other RakPeer methods to see what exactly happens when the client is joining a server.

// Client presence requests.
// stop requests after GameJoin on random server and see if you are still registered.

// GOAL IS TO SPOOF ROBLOX CLIENT.


#define __int128(I) (*(__m128i*)&I)
#define GET_SERVER_TOKEN(I) std::string((reinterpret_cast<char*>(I) + 10), 35)

// Defines the server address and port to connect to.
#define SERVER_ADDRESS "185.254.71.29"
#define SERVER_PORT 53881
#define USER_ID 19897681
#define NEW_CLIENT_TICKET "8/15/2018 2:56:15 PM;L8F0Z+oExdyI0C2pDaTSMvb83GZ7knKntEm/FvgdDP4OaSjp67t4aflDCBf2aK0G2DzIvQTmsvnNuPthDu6auhjqaw3YwfujEsK6PRd4Qd+4hvRP4U5TjWwnS8hSg2b6hjOFv7hD2oM6IlUF4X1a6LMuThUPUpRrwEqNuQ/Y3ic/MyDUh7kFMJoCH1rFIWRVm59C7OqQebXXOaN77CELrRl0I7SJqamaOWdTGPt0MDbVcnFf4u0jfOLigL6GQ0b6j4NczymgwhyqB0WzztpzmSFFLHPaFe/j7xD0xaovyYtr5vI9fBXvykXb+CYSoWZa7+pf9byK2qjv+dYDYS/+QQ==;EwpCPdXaGUkPZ64JzCH9uywz8umUUT0DnQ6BuEmYB5smyT28HK7vRRXGAoTKJBzfmH8fuoDdqVInE1VKS+Y4/Sf3SrdgxbZiBrBwbPBt9arTZMB8YcLTEe7GmeTgWWpXId+ZO4HJpfwQSORbn7dyStOlCH//LWQNQtRTslGhCd6z7z+jq3XNEhMKQrFg5CRvWO+pCpa615UJ3HIgb0tu1J3Qqp5DkXJJPHeSAIUtXjcqg9aOE6QNReJTyFf5ntL3UiOZ4zq03JWvWGvBBptbuAkmBnPn4DX0kpzeMtmJdQuZikuguZ7vGhR7yFNILIh/yUXpj7qaRxEUE/ZNN4lmEQ==;2"
#define SESSION_ID "{\"SessionId\":\"5cddd0a6-5cf8-4f30-a43e-f554e59d9db0\",\"GameId\":\"0220db30-151d-4bf4-a709-787923117b5d\",\"PlaceId\":146449216,\"ClientIpAddress\":\"82.47.178.125\",\"PlatformTypeId\":5,\"SessionStarted\":\"2018-08-15T19:55:35.1602624Z\",\"BrowserTrackerId\":25967628809,\"PartyId\":null,\"Age\":28.620458330046688,\"Latitude\":50.3964,\"Longitude\":-4.1386,\"CountryId\":2,\"LanguageId\":41}"

// Defines various Roblox packet identifiers.
#define ID_ROBLOX_SCHEMA_VERSION (char)0x90
#define ID_ROBLOX_START_AUTH_THREAD (char)0x92
#define ID_ROBLOX_AUTH (char)0x8A
#define ID_ROBLOX_INITIAL_SPAWN_NAME (char)0x8F
#define ID_ROBLOX_SERVER_TOKEN (char)0x81
#define ID_ROBLOX_REPLICATION (char)0x83

// Defines the roblox message string identifier.
#define STRING_IDENTIFIER (char)0x81

#define CLIENT_TOKEN_ID (char)0x81
#define SERVER_TOKEN_ID (char)0x82

// Stores the connection password.
const char password[] = { 
	0x37, 0x4f, 0x5e, 0x11, 0x6c, 0x45 
};

// Defines some Roblox packets.

const char StartAuthThread[] = {
	ID_ROBLOX_START_AUTH_THREAD, 0x00
};

const char InitialSpawnName[] = {
	ID_ROBLOX_INITIAL_SPAWN_NAME, 0x00
};

// Defines the cryptor source.
const char Source[] = { 
	0xFE, 0xF9, 0xF0, 0xEB, 0xE2, 0xDD, 0xD4, 0xCF, 0xC6, 0xC1, 0xB8, 0xB3, 0xAA, 0xA5, 0x9C, 0x97
};

// IDA functions. Will decypher these later.

// Various BitStream functions.

int WriteHashedInt64(unsigned int a1, unsigned int a2, BitStream* Stream)
{
	unsigned int v3; // ebx@1
	unsigned int v4; // esi@1
	BitStream* v5; // edi@1
	char v6; // cl@2
	char v7; // cl@2
	int result; // eax@4

	v3 = a1;
	v4 = a2;
	v5 = Stream;
	do
	{
		v6 = v3;
		v3 = __PAIR__(v4, v3) >> 7;
		v7 = v6 & 0x7F;
		v4 >>= 7;
		BYTE3(a2) = v7;
		if (v4 | v3)
			BYTE3(a2) = v7 | 0x80;
		v5->WriteBits((unsigned __int8 *)&a2 + 3, 8u, 1);
		result = v4 | v3;
	} while (v4 | v3);
	return result;
}

int WriteHashedInt64(unsigned __int64 Value, BitStream* Stream)
{
	return WriteHashedInt64(2 * Value ^ (SHIDWORD(Value) >> 31), (Value >> 31) ^ (SHIDWORD(Value) >> 31), Stream);
}

void WriteHashedInt32(int Value, BitStream* Stream)
{
	unsigned int v2; // ebx@1
	char v4; // al@2
	char v5; // al@2

	v2 = Value;
	do
	{
		v4 = v2;
		v2 >>= 7;
		v5 = v4 & 0x7F;
		BYTE3(Value) = v5;
		if (v2)
			BYTE3(Value) = v5 | 0x80;
		Stream->WriteBits((unsigned __int8 *)&Value + 3, 8, 1);
	} while (v2);
}

void WriteString(std::string String, BitStream* Stream)
{
	WriteHashedInt32(String.length(), Stream);
	Stream->Write(String.c_str(), String.length());
}

// Generates an encryption key.

int GenerateEncryptionKey_2(void* Buffer, int Key)
{
	signed int v2; // edx@1
	int result; // eax@1
	char *v4; // ecx@1

	v2 = 623;
	result = Key | 1;
	*((_DWORD *)Buffer + 626) = 0;
	*(_DWORD *)Buffer = Key | 1;
	v4 = (char *)Buffer + 4;
	do
	{
		result *= 69069;
		v4 += 4;
		*((_DWORD *)v4 - 1) = result;
		--v2;
	} while (v2);
	return result;
}

void* GenerateEncryptionKey_1(RakNet::RakPeerInterface* Client)
{
	// Gets the external id of our client, this is what is used to encrypt the message.
	// The result of this function will be the same for the server, this way it can decrypt our messages.
	RakNet::SystemAddress Result = Client->GetExternalID(UNASSIGNED_SYSTEM_ADDRESS);
	// Creates a buffer big enough to store the encryption key.
	void* Buffer = malloc(2508);
	// Generates the encryption key using our clients external id.
	GenerateEncryptionKey_2(Buffer, _mm_cvtsi128_si32(__int128(Result)) ^ _mm_cvtsi128_si32(_mm_srli_si128(__int128(Result), 4)));
	// Returns the buffer address.
	return Buffer;
}

// Creates the client auth token.
unsigned char* CreateAuthToken(RakNet::RakPeerInterface* Client, __int64 UserId, std::string NewClientTicket, std::string FirstHash, std::string SecondHash, std::string Platform, std::string Unknown, std::string SessionId, size_t* BufferSize)
{
	// Creates the auth packet body.
	BitStream* Stream = new BitStream();
	// Tells the server that this is an auth packet.
	Stream->Write(ID_ROBLOX_AUTH);
	// Hashes the user id into a 4 byte hash and writes it into the buffer.
	WriteHashedInt64(UserId, Stream);
	// Writes the client ticket into the bit stream.
	WriteString(NewClientTicket, Stream);
	// Writes the executable hash to the bitstream (I've yet to find out what this is).
	WriteString(FirstHash, Stream);
	// Writes 0x24 into the buffer (I've yet to find out what this means).
	Stream->Write(0x24);
	// Writes another hash to the buffer (I've yet to find out what this is).
	WriteString(SecondHash, Stream);
	// Writes the platform to the buffer.
	WriteString(Platform, Stream);
	// Writes a question mark to the buffer (I've yet to find out what this is).
	WriteString(Unknown, Stream);
	// Writes the session id to the buffer.
	WriteString(SessionId, Stream);
	// Writes the image base address to the buffer.
	Stream->Write((int)GetModuleHandleW(NULL));
	// Generates the packet encryption key.
	void* Key = GenerateEncryptionKey_1(Client);
	// Creates a new instnace of the cryptor.
	Cryptor Cry = Cryptor();
	// Sets the cryptor source.
	Cry.Set((char*)Source);
	// Gets the number of bits allocated in the bitstream.
	RakNet::BitSize_t v3 = Stream->GetNumberOfBitsAllocated(); // could be wrong
	// Calculates the size of the data to be encrypted.
	unsigned int Size = ((unsigned int)(v3 + 7) >> 3) - 1;
	// Reallocates the buffer if needed.
	Stream->AddBitsAndReallocate(((Size + 21) >> 4 << 7) - v3 + 8);
	// Encrypts the packet.
	Cry.Encrypt(Stream->GetData() + 1, Size, Stream->GetData() + 1, &Size, Key);
	// Creates a buffer to store encrypted packet.
	unsigned char* Buffer = new unsigned char[Size + 1];
	// Reads the encrypted packet into the buffer.
	memcpy(Buffer, Stream->GetData(), Size + 1);
	// Sets the buffer size.
	*BufferSize = Size + 1;
	// Returns the client auth token buffer.
	return Buffer;
}

// Generates the RBX token.
std::string CreateInterfaceIdentifier()
{
	// Stores the GUID.
	GUID pguid;
	// Store the GUID as a wide character string.
	WCHAR Dst[64];
	// Stores the GUID as a multi byte string.
	CHAR MultiByteStr;
	// Zeros out the wchar memory block.
	memset(Dst, 0, 0x80u);
	// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface identifiers.
	if (CoCreateGuid(&pguid) != S_OK) return nullptr;
	// Converts a globally unique identifier (GUID) into a string of printable characters.
	if (!StringFromGUID2(pguid, Dst, 64)) return nullptr;
	// Maps a UTF-16 (wide character) string to a new character string.
	if (!WideCharToMultiByte(0, 0, Dst, 64, &MultiByteStr, 64, 0, 0)) return nullptr;
	// Converts the GUID to a standard string.
	std::string identifier = std::string(&MultiByteStr, strlen(&MultiByteStr));
	// Constructs the final identifier.
	identifier = "RBX" + identifier;
	// Removes the symbols from the identifier.
	identifier.erase(0x28u, 1u);
	identifier.erase(0x1Bu, 1u);
	identifier.erase(0x16u, 1u);
	identifier.erase(0x11u, 1u);
	identifier.erase(0xCu, 1u);
	identifier.erase(3u, 1u);
	// Returns the full identifier.
	return identifier;
}

// Creates the replication message.
BitStream* CreateReplicationMessage(std::string Username, std::string AvatarUrl, std::string Platform, std::string Language, std::string ServerToken)
{
	// These values change on roblox versions.
	__int64 value1 = 0x6bd784;
	int value2 = 0x8; // may not affect join
	int value3 = 0x0; // may not affect join
	int value4 = 0x88080000;
	short value5 = 0x00C8;
	char value6 = 0x20;
	char value7 = 0x89;

	// Generates a client token string.
	std::string ClientToken = CreateInterfaceIdentifier();
	// Creates the replication packet body.
	BitStream* Stream = new BitStream();
	// Tells the server that this is a replication packet.
	Stream->Write(ID_ROBLOX_REPLICATION);
	// Creates the packet body.
	for (int i = 0; i < 10; i++)
	{
		Stream->Write((char)0x5);
		Stream->Write((char)0x0);
		Stream->Write(value1);
		Stream->Write(value2);
		Stream->Write(value3);
	}
	Stream->Write((char)0xB);
	Stream->Write((int)0x0);
	Stream->Write((char)0x2);
	Stream->Write((char)0x81);
	// Writes the client key string to the bit stream.
	Stream->Write((int)ClientToken.length());
	Stream->Write(ClientToken.c_str(), ClientToken.length());
	Stream->Write(value4);
	Stream->Write((short)0xA4);
	Stream->Write((char)0x1);
	Stream->Write((char)0x18);
	Stream->Write((char)0x81);
	// Writes the username string to the bit stream.
	Stream->Write((int)Username.length());
	Stream->Write(Username.c_str(), Username.length());
	Stream->Write((char)0xFF);
	Stream->Write(value5);
	Stream->Write((short)0x2708);
	// Writes the avatar url string to the bit stream.
	WriteString(AvatarUrl, Stream);
	Stream->Write((long long)0x09A2F5FC120A001B);
	// Writes the platform string to the bit stream.
	WriteString(Platform, Stream);
	Stream->Write((char)0x1D);
	// Writes the language string to the bit stream.
	WriteString(Language, Stream);
	Stream->Write((short)0x26A2);
	Stream->Write((long long)0xF5FC1228A2F5FC12);
	Stream->Write((char)0xFF);
	Stream->Write((char)0x82);
	// Writes the server key string to the bit stream.
	Stream->Write((int)ServerToken.length());
	Stream->Write(ServerToken.c_str(), ServerToken.length());
	Stream->Write((int)0xA0000000);
	Stream->Write((short)0x0201);
	Stream->Write(value6);
	Stream->Write((long long)0x09000000de00ffff);
	Stream->Write((char)0x01);
	Stream->Write(value7);
	Stream->Write((int)0x08000000);
	// Returns the bit stream.
	return Stream;
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}

class Joiner : public Roblox::Client
{
private:
	virtual void OnConnect()
	{
		cout << "Connected" << endl;
		// Starts the server auth thread.
		this->StartAuthThread();
		// Sends the user auth to the server.
		this->SendAuth(USER_ID,
			NEW_CLIENT_TICKET,
			"263ff5a46fa1d2a92ef2e2dbd8fd3e4b",
			"9302a9ea773898e118dc40d13aa294666150afe3!521cc11498e24bff2b42c40509e0ab6f",
			"Win32",
			"?",
			SESSION_ID);
	}

	virtual void OnToken()
	{
		cout << "Token" << endl;
		// Sends the user token to the server.
		this->SendToken("hunter24957", "https://api.roblox.com/v1.1/avatar-fetch/?placeId=1547610457&userId=19897681", "Win32", "en-us");
	}
};

int main()
{
	Joiner joiner;
	joiner.Connect(SERVER_ADDRESS, SERVER_PORT);

	// This code would give you one instance of the peer. Usually you would only want one of these in a particular exe.
	RakNet::RakPeerInterface* Client = RakNet::RakPeerInterface::GetInstance();
	// The call to Startup starts the network threads. This must be called before calling connect.
	Client->Startup(1, &SocketDescriptor(), 1);
	// Connects to the specified server at the specified port.
	ConnectionAttemptResult Result = Client->Connect(SERVER_ADDRESS, SERVER_PORT, password, sizeof(password));
	// Checks the result of the connection.
	if (Result == CONNECTION_ATTEMPT_STARTED)
	{
		// Enters the connection loop.
		while (true)
		{
			// Handles the packets.
			for (RakNet::Packet* Packet = Client->Receive(); Packet; Client->DeallocatePacket(Packet), Packet = Client->Receive())
			{
				// Extracts the packet identifier.
				unsigned char Identifier = GetPacketIdentifier(Packet);
				// Stores the player auth token.
				unsigned char* Auth;
				// Stores the auth token size.
				size_t AuthSize;
				// Check if this is a network message packet
				switch (Identifier)
				{
				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally
					printf("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_ALREADY_CONNECTED:
					// Connection lost normally
					printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", Packet->guid);
					break;
				case ID_INCOMPATIBLE_PROTOCOL_VERSION:
					printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
					break;
				case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
					printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
					printf("ID_REMOTE_CONNECTION_LOST\n");
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
					printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
					break;
				case ID_CONNECTION_BANNED: // Banned from this server
					printf("We are banned from this server.\n");
					break;
				case ID_CONNECTION_ATTEMPT_FAILED:
					printf("Connection attempt failed\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					// Sorry, the server is full.  I don't do anything here but
					// A real app should tell the user
					printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
					break;
				case ID_INVALID_PASSWORD:
					printf("ID_INVALID_PASSWORD\n");
					break;
				case ID_CONNECTION_LOST:
					// Couldn't deliver a reliable packet - i.e. the other system was abnormally
					// terminated
					printf("ID_CONNECTION_LOST\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					// This tells the client they have connected
					printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", Packet->systemAddress.ToString(true), Packet->guid.ToString());
					printf("My external address is %s\n", Client->GetExternalID(Packet->systemAddress).ToString(true));

					// Now that we are connected we can start talking with the server.

					// Tells the server information about us.
					// peer->Send(client_information, sizeof(client_information), PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

					// Starts the authorization thread.
					Client->Send(StartAuthThread, sizeof(StartAuthThread), PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
					// Creates the auth token to send to the server in order to authenticate our account.
					Auth = CreateAuthToken(
						Client,
						USER_ID,
						NEW_CLIENT_TICKET,
						"263ff5a46fa1d2a92ef2e2dbd8fd3e4b",
						"9302a9ea773898e118dc40d13aa294666150afe3!521cc11498e24bff2b42c40509e0ab6f",
						"Win32",
						"?",
						SESSION_ID,
						&AuthSize);
					//cout << (void*)Auth << endl;
					//cout << hex << AuthSize << endl;
					// Verifies we are a legit client.
					Client->Send((const char*)Auth, AuthSize, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
					// Tells the server to spawn our player.
					Client->Send(InitialSpawnName, sizeof(InitialSpawnName), PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
					break;
				case ID_CONNECTED_PING:
				case ID_UNCONNECTED_PING:
					printf("Ping from %s\n", Packet->systemAddress.ToString(true));
					break;
				case 0x81:
					// The server has sent their token.
					// Extracts the token.
					Client->Send(CreateReplicationMessage("hunter24957", "https://api.roblox.com/v1.1/avatar-fetch/?placeId=1547610457&userId=19897681", "Win32", "en-us", GET_SERVER_TOKEN(Packet->data)), PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				default:
					// It's a client, so just show the message
					printf("Got a packet: %s\n", Packet->data);
					break;
				}
			}
		}
	}
}