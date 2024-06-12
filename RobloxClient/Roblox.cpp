#include "Roblox.h"

// Called when the client has connected with a server.
void Roblox::Client::OnConnect()
{

}

// Called when the client recieves the server token.
void Roblox::Client::OnToken()
{

}

// Creates a new Roblox client.
Roblox::Client::Client()
{
	// This code would give you one instance of the peer. Usually you would only want one of these in a particular exe.
	this->Interface = RakNet::RakPeerInterface::GetInstance();
	// The call to Startup starts the network threads. This must be called before calling connect.
	this->Interface->Startup(1, &RakNet::SocketDescriptor(), 1);
	// Generates the client token.
	this->ClientToken = Roblox::Client::GenerateToken();
	// Generates the packet encryption key.
	this->Key = this->GenerateKey();
}

// Deletes the Roblox client.
Roblox::Client::~Client()
{
	// Deletes the rak net interface.
	delete this->Interface;
	// Frees the key buffer.
	free(this->Key);
}

// Connects to the specified Roblox server.
void Roblox::Client::Connect(const char * Address, unsigned short Port)
{
	// Connects to the specified server at the specified port.
	RakNet::ConnectionAttemptResult Result = this->Interface->Connect(Address, Port, Password, sizeof(Password));
	// Checks the result of the connection.
	if (Result == RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED)
	{
		// Enters the connection loop.
		while (true)
		{
			// Handles the packets.
			for (RakNet::Packet* Packet = this->Interface->Receive(); Packet; this->Interface->DeallocatePacket(Packet), Packet = this->Interface->Receive())
			{
				// Finds the appropriate packet handler.
				switch (PACKET_IDENTIFIER(Packet))
				{
				case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
					break;
				case DefaultMessageIDTypes::ID_ALREADY_CONNECTED:
					break;
				case DefaultMessageIDTypes::ID_INCOMPATIBLE_PROTOCOL_VERSION:
					break;
				case DefaultMessageIDTypes::ID_REMOTE_DISCONNECTION_NOTIFICATION:
					break;
				case DefaultMessageIDTypes::ID_REMOTE_CONNECTION_LOST:
					break;
				case DefaultMessageIDTypes::ID_REMOTE_NEW_INCOMING_CONNECTION:
					break;
				case DefaultMessageIDTypes::ID_CONNECTION_BANNED:
					break;
				case DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED:
					break;
				case DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS:
					break;
				case DefaultMessageIDTypes::ID_INVALID_PASSWORD:
					break;
				case DefaultMessageIDTypes::ID_CONNECTION_LOST:
					break;
				case DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED:
					// The client has successfully connected to the server.
					this->OnConnect();
					break;
				case DefaultMessageIDTypes::ID_CONNECTED_PING:
					break;
				case DefaultMessageIDTypes::ID_UNCONNECTED_PING:
					break;
				case Roblox::MessageIDTypes::ID_SERVER_TOKEN:
					// Sets the server identifier.
					this->ServerToken = SERVER_TOKEN(Packet);
					// The client has recieved the server token.
					this->OnToken();
					break;
				default:
					break;
				}
			}
		}
	}
}

// Tells the server to start the authorization thread.
uint32_t Roblox::Client::StartAuthThread()
{
	// Tells the server to start the authorization thread.
	return this->Interface->Send(Roblox::StartAuthThread, sizeof(Roblox::StartAuthThread), PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

// Sends authentication to the Roblox server.
uint32_t Roblox::Client::SendAuth(int64_t UserId, std::string ClientTicket, std::string FirstHash, std::string SecondHash, std::string Platform, std::string Unknown, std::string SessionId)
{
	// Creates a stream to build the auth message body.
	Roblox::BitStream Stream;
	// Creates a new instance of the cryptor to encrypt the message.
	Cryptor Crypt;
	// Tells the server that this is an auth packet.
	Stream.Write((char)Roblox::MessageIDTypes::ID_AUTH);
	// Hashes the user id into a 4 byte hash and writes it into the buffer.
	Stream.WriteInt64(UserId);
	// Writes the client ticket into the bit stream.
	Stream.WriteString(ClientTicket);
	// Writes the executable hash to the bitstream (I've yet to find out what this is).
	Stream.WriteString(FirstHash);
	// Writes 0x24 into the buffer (I've yet to find out what this means).
	Stream.Write((int)0x24);
	// Writes another hash to the buffer (I've yet to find out what this is).
	Stream.WriteString(SecondHash);
	// Writes the platform to the buffer.
	Stream.WriteString(Platform);
	// Writes a question mark to the buffer (I've yet to find out what this is).
	Stream.WriteString(Unknown);
	// Writes the session id to the buffer.
	Stream.WriteString(SessionId);
	// Writes the image base address to the buffer.
	Stream.Write((int)GetModuleHandleW(NULL));
	// Sets the cryptor source.
	Crypt.Set(const_cast<char *>(Roblox::Source));
	// Gets the number of bits allocated in the bitstream.
	RakNet::BitSize_t BitSize = Stream.GetNumberOfBitsAllocated();
	// Calculates the size of the data to be encrypted.
	unsigned int Size = ((unsigned int)(BitSize + 7) >> 3) - 1;
	// Reallocates the buffer if needed.
	Stream.AddBitsAndReallocate(((Size + 21) >> 4 << 7) - BitSize + 8);
	// Encrypts the message.
	Crypt.Encrypt(Stream.GetData() + 1, Size, Stream.GetData() + 1, &Size, this->Key);
	// Sends the auth message to the server.
	return this->Interface->Send(reinterpret_cast<const char *>(Stream.GetData()), Size + 1, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

// Sends the client token to the Roblox server.
uint32_t Roblox::Client::SendToken(std::string Username, std::string AvatarUrl, std::string Platform, std::string Language)
{
	// These values change on roblox versions.
	__int64 value1 = 0x6bd784;
	int value2 = 0x8; // may not affect join
	int value3 = 0x0; // may not affect join
	int value4 = 0x88080000;
	short value5 = 0x00C8;
	char value6 = 0x20;
	char value7 = 0x89;

	// Creates the replication packet body.
	Roblox::BitStream Stream;
	// Tells the server that this is a client token message.
	Stream.Write((char)Roblox::MessageIDTypes::ID_CLIENT_TOKEN);
	// Creates the packet body.
	for (int i = 0; i < 10; i++)
	{
		Stream.Write((char)0x5);
		Stream.Write((char)0x0);
		Stream.Write(value1);
		Stream.Write(value2);
		Stream.Write(value3);
	}
	Stream.Write((char)0xB);
	Stream.Write((int)0x0);
	Stream.Write((char)0x2);
	Stream.Write((char)0x81);
	Stream.Write((int)this->ClientToken.length());
	Stream.Write(this->ClientToken.c_str(), this->ClientToken.length());
	Stream.Write(value4);
	Stream.Write((short)0xA4);
	Stream.Write((char)0x1);
	Stream.Write((char)0x18);
	Stream.Write((char)0x81);
	Stream.Write((int)Username.length());
	Stream.Write(Username.c_str(), Username.length());
	Stream.Write((char)0xFF);
	Stream.Write(value5);
	Stream.Write((short)0x2708);
	Stream.WriteString(AvatarUrl);
	Stream.Write((long long)0x09A2F5FC120A001B);
	Stream.WriteString(Platform);
	Stream.Write((char)0x1D);
	Stream.WriteString(Language);
	Stream.Write((short)0x26A2);
	Stream.Write((long long)0xF5FC1228A2F5FC12);
	Stream.Write((char)0xFF);
	Stream.Write((char)0x82);
	Stream.Write((int)this->ServerToken.length());
	Stream.Write(this->ServerToken.c_str(), this->ServerToken.length());
	Stream.Write((int)0xA0000000);
	Stream.Write((short)0x0201);
	Stream.Write(value6);
	Stream.Write((long long)0x09000000de00ffff);
	Stream.Write((char)0x01);
	Stream.Write(value7);
	Stream.Write((int)0x08000000);
	// Sends the identifier message.
	return this->Interface->Send(&Stream, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

// Generates an encryption key using the client's external identifier.
void * Roblox::Client::GenerateKey()
{
	// Gets the external id of our client, this is what is used to encrypt the message.
	// The result of this function will be the same for the server, this way it can decrypt our messages.
	RakNet::SystemAddress Result = this->Interface->GetExternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	// Creates a buffer big enough to store the encryption key.
	void* Buffer = malloc(2508);
	// Generates the encryption key using our clients external id.

	int Key = _mm_cvtsi128_si32(__int128(Result)) ^ _mm_cvtsi128_si32(_mm_srli_si128(__int128(Result), 4));

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

	// Returns the buffer address.
	return Buffer;
}

// Generates a client token.
std::string Roblox::Client::GenerateToken()
{
	// Stores the GUID.
	GUID Guid;
	// Store the GUID as a wide character string.
	WCHAR Dst[64];
	// Stores the GUID as a multi byte string.
	CHAR MultiByteStr;
	// Zeros out the wchar memory block.
	memset(Dst, 0, 0x80u);
	// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface identifiers.
	if (CoCreateGuid(&Guid) != S_OK) return nullptr;
	// Converts a globally unique identifier (GUID) into a string of printable characters.
	if (!StringFromGUID2(Guid, Dst, 64)) return nullptr;
	// Maps a UTF-16 (wide character) string to a new character string.
	if (!WideCharToMultiByte(0, 0, Dst, 64, &MultiByteStr, 64, 0, 0)) return nullptr;
	// Converts the GUID to a standard string.
	std::string Identifier = std::string(&MultiByteStr, strlen(&MultiByteStr));
	// Constructs the final identifier.
	Identifier = "RBX" + Identifier;
	// Removes the symbols from the identifier.
	Identifier.erase(0x28u, 1u);
	Identifier.erase(0x1Bu, 1u);
	Identifier.erase(0x16u, 1u);
	Identifier.erase(0x11u, 1u);
	Identifier.erase(0xCu, 1u);
	Identifier.erase(3u, 1u);
	// Returns the full identifier.
	return Identifier;
}

// Writes an int32 to the bit stream.
void Roblox::BitStream::WriteInt32(int32_t Value)
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
		this->WriteBits((unsigned __int8 *)&Value + 3, 8, 1);
	} while (v2);
}

// Writes an int64 to the bit stream.
void Roblox::BitStream::WriteInt64(int64_t Value)
{
	unsigned int v3; // ebx@1
	unsigned int v4; // esi@1
	char v6; // cl@2
	char v7; // cl@2
	int result; // eax@4
	unsigned int a1 = 2 * Value ^ (SHIDWORD(Value) >> 31);
	unsigned int a2 = (Value >> 31) ^ (SHIDWORD(Value) >> 31);
	v3 = a1;
	v4 = a2;
	do
	{
		v6 = v3;
		v3 = __PAIR__(v4, v3) >> 7;
		v7 = v6 & 0x7F;
		v4 >>= 7;
		BYTE3(a2) = v7;
		if (v4 | v3)
			BYTE3(a2) = v7 | 0x80;
		this->WriteBits((unsigned __int8 *)&a2 + 3, 8u, 1);
		result = v4 | v3;
	} while (v4 | v3);
}

// Writes a string to the bit stream.
void Roblox::BitStream::WriteString(std::string String)
{
	this->WriteInt32(String.length());
	this->Write(String.c_str(), String.length());
}
