#pragma once
class Packet
{
public:
	// Header structure
	typedef struct {
		short int size;
		short int type;
	} tHeader;

	Packet();
	~Packet();
private:
	tHeader header;
public:
	short int PacketType();		// return the packet type see Packet Factory Enum

	short int PacketSize();		// return total packet size
	short int HeaderSize();		// return header size
	short int BodySize();       // return payload size

	void SetBodySize(short int size);  // modify the packet header set the body size
	void SetType(short int type);	   // modify the packet header set the packet type

	void CopyHeader(Packet* other);	   // copy the header from another packet

	virtual short int CalculateBodySize(); // (virtual) calculate the payload size 

	// encode & decode 
	// Params: 
	//	iBuffer		 - buffer where the packet will be written or read
	//	ioBufferSize - size of the buffer that is still available to read / write
	//  OUTPUT: new buffer position void* after the encode & decode
	//  Note: ioBufferSize is also updated to care for the data that was written || read		
	void* Encode(void* iBuffer, short int* ioBuferSize);
	void* Decode(void* iBuffer, short int* ioBuferSize);

	void* EncodeHeader(void* iBuffer, short int* ioBuferSize);
	void* DecodeHeader(void* iBuffer, short int* ioBuferSize);

	// The body is the only expectable function that changes from packet to packet
	virtual void* DecodeBody(void* iBuffer, short int* ioBuferSize);
	virtual void* EncodeBody(void* iBuffer, short int* ioBuferSize);

	virtual void Execute(); // Execute is a virtual function 
	// which can be called te perform a packet specific action 
};

