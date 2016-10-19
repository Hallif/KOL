(**
 @file  unix.h
 @brief ENet Unix header
*)
{$ifndef __ENET_UNIX_H__}
{$define __ENET_UNIX_H__}

{.$include <stdlib.h>}
{.$include <sys/time.h>}
{.$include <sys/types.h>}
{.$include <sys/socket.h>}
{.$include <netinet/in.h>}
{.$include <unistd.h>}

{$define ENET_HOST_TO_NET_16:=htons} (**< macro that converts host to net byte-order of a 16-bit value *)
{$define ENET_HOST_TO_NET_32:=htonl} (**< macro that converts host to net byte-order of a 32-bit value *)

{$define ENET_NET_TO_HOST_16:=ntohs (**< macro that converts net to host byte-order of a 16-bit value *)
{$define ENET_NET_TO_HOST_32:=ntohl} (**< macro that converts net to host byte-order of a 32-bit value *)

typedef ENetBuffer_PTR = ^ENetBuffer;ENetBuffer = struct
		data:pointer;
		dataLength:size_t;
	end;

{$define ENET_CALLBACK:=}

{$define ENET_API:=extern}

{$endif~__ENET_UNIX_H__}

