(**
 @file  packet.c
 @brief ENet packet management functions
*)
{.$include <string.h>}
{.$define ENET_BUILDING_LIB 1}
{$include'enet/enet_h.inc'}

(** @defgroup Packet ENet packet functions
    @//{
*)

(** Creates a packet that may be sent to a peer.
    @param data         initial contents of the packet's data; the packet's data will remain uninitialized if data is NULL.
    @param dataLength   size of the data allocated for this packet
    @param flags        flags for this packet as described for the ENetPacket structure.
    @returns the packet on success, NULL on failure
*)
function
enet_packet_create (data:pointer; dataLength:size_t; flags:enet_uint32):
ENetPacket_PTR;
ENET_API
var
	packet:^ENetPacket;
begin
	packet:=ENetPacket_PTR(enet_malloc (sizeof (ENetPacket)));
	if(packet = NIL)
	then return(NIL)
	;
	if		(flags and ENET_PACKET_FLAG_NO_ALLOCATE <> 0)then packet^.data:=enet_uint8_ptr(data)
	else if	(dataLength <= 0							)then packet^.data:=NIL
	else begin
		packet^.data:=enet_uint8_ptr(enet_malloc (dataLength));
		if(packet^.data = NIL)
		then begin
			enet_free (packet);
			return(NIL);
		end;

		if(data <> NIL)
		then memcpy (packet^.data ,data ,dataLength)
	end;

	packet^.referenceCount:=0;
	packet^.flags:=flags;
	packet^.dataLength:=dataLength;
	packet^.freeCallback:=NIL;
	packet^.userData:=NIL;

	return(packet);
end;

(** Destroys the packet and deallocates its data.
    @param packet packet to be destroyed
*)
procedure
enet_packet_destroy (packet:ENetPacket_PTR);
ENET_API
begin
	if(packet = NIL)
	then return
	;
	if assigned(packet^.freeCallback)
	then packet^.freeCallback (packet)
	;
	if	(packet^.flags and ENET_PACKET_FLAG_NO_ALLOCATE = 0)
	and	(packet^.data <> NIL)
	then enet_free (packet^.data)
	;
	enet_free (packet);
end;

(** Attempts to resize the data in the packet to length specified in the
    dataLength parameter 
    @param packet packet to resize
    @param dataLength new size for the packet data
    @returns 0 on success, < 0 on failure
*)
function
enet_packet_resize (packet:ENetPacket_PTR; dataLength:size_t):
int;
ENET_API
var
	newData:^enet_uint8;
begin
	if	(dataLength <= packet^.dataLength)
	or	(packet^.flags and ENET_PACKET_FLAG_NO_ALLOCATE <> 0)
	then begin
		packet^.dataLength:=dataLength;

		return(0);
	end;

	newData:=enet_uint8_ptr(enet_malloc (dataLength));
	if(newData = NIL)
	then return(-1)
	;
	memcpy (newData ,packet^.data ,packet^.dataLength);
	enet_free (packet^.data);

	packet^.data:=newData;
	packet^.dataLength:=dataLength;

	return(0);
end;

const{static}initializedCRC32:int=0;
var{static}crcTable:array[byte]of enet_uint32;

{static}function
reflect_crc (val:enet_uint32; bits:int):
enet_uint32;
var
	result:unsigned=0;
	bit:int;
begin
	result:=0;
	for bit:=0 to pred(bits)
	do begin
		if(val and 1) <> 0
		then result:=result or (1 << (bits - 1 - bit))
		;
		val:=val >> 1;
	end;

	return(result);
end;

{static}procedure
initialize_crc32 ();
var
	_byte_:int;
	offset:int;
	crc:enet_uint32;
begin
	for _byte_:=0 to pred(256)
	do begin
		crc:=reflect_crc (_byte_ ,8) << 24;
		for offset:=0 to pred(8)
		do begin
			if(crc and $80000000) <> 0
			then crc:=(crc << 1) xor $04c11db7
			else crc:=crc << 1
		end;

		crcTable[_byte_]:=reflect_crc (crc ,32);
	end;

	initializedCRC32:=1;
end;

function
enet_crc32 (buffers:ENetBuffer_PTR; bufferCount:size_t):
enet_uint32;
ENET_API
var
	crc:enet_uint32=$FFFFFFFF;
	data:enet_uint8_ptr{const};
    dataEnd:enet_uint8_ptr{const};
begin
	if(initializedCRC32 = 0)
	then initialize_crc32 ()
	;
	while (bufferCount > 0)
	do begin
		bufferCount -=1;
		data:={const}enet_uint8_ptr(buffers^.data);
		dataEnd:=@ data [buffers^.dataLength];while
		data < dataEnd{$define continue_:=begin;
		inc(data);continue;end}
		do begin
			crc:=(crc >> 8) xor crcTable [(crc and $FF) xor data^{++}]
			;
		continue_{$undef continue_}end
		;
		inc(buffers);
	end;

	return(ENET_HOST_TO_NET_32 (not crc));
end;

(** @ *)
