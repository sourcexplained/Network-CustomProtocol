#include "Packet.h"

#include <winsock2.h>

Packet::Packet(){ memset(&header, 0, sizeof(tHeader)); } // reset to 0 
Packet::~Packet() {}

// note that I opted to store things using network standard
short int Packet::PacketType(){ return ntohs(header.type); }
short int Packet::PacketSize(){ return HeaderSize() + BodySize(); }
short int Packet::HeaderSize(){ return sizeof(tHeader); }
short int Packet::BodySize()  { return ntohs(header.size); }

// set functions
void Packet::SetBodySize(short int size){ header.size = htons(size); }
void Packet::SetType(short int type){ header.type = htons(type); }
void Packet::CopyHeader(Packet* other){ header = other->header; }

// General Encode & Decode
void* Packet::Encode(void* iBuffer, short int* ioBuferSize){
	SetBodySize(CalculateBodySize()); // update body size
	iBuffer = EncodeHeader(iBuffer, ioBuferSize);
	iBuffer = EncodeBody(iBuffer, ioBuferSize);
	return iBuffer;
}
void* Packet::Decode(void* iBuffer, short int* ioBuferSize){
	iBuffer = DecodeHeader(iBuffer, ioBuferSize);
	iBuffer = DecodeBody(iBuffer, ioBuferSize);
	return iBuffer;
}

void* Packet::EncodeHeader(void* iBuffer, short int* ioBuferSize){
	if (iBuffer == NULL || *ioBuferSize < sizeof(tHeader)) return NULL;

	memcpy_s(iBuffer, *ioBuferSize, &header, HeaderSize());
	// update location of iBuffer & ioBufferSize
	iBuffer = ((char*)iBuffer) + HeaderSize();
	(*ioBuferSize) -= HeaderSize();
	// return new position
	return iBuffer;
}

void* Packet::DecodeHeader(void* iBuffer, short int* ioBuferSize){
	if (iBuffer == NULL || *ioBuferSize < sizeof(tHeader)) return NULL;

	memcpy_s(&header, HeaderSize(), iBuffer, HeaderSize());
	// update location of iBuffer & ioBufferSize
	iBuffer = ((char*)iBuffer) + HeaderSize();
	(*ioBuferSize) -= HeaderSize();

	// return new position
	return iBuffer;
}

// virtual functions to be overwritten by sub-classes
void* Packet::EncodeBody(void* iBuffer, short int* ioBuferSize){ return iBuffer; }
void* Packet::DecodeBody(void* iBuffer, short int* ioBuferSize){ return iBuffer; }
short int Packet::CalculateBodySize(){ return 0; }
void Packet::Execute(){}
