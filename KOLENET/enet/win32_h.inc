(**
 @file  win32.h
 @brief ENet Win32 header
*)
{$ifndef __ENET_WIN32_H__}
{$define __ENET_WIN32_H__}

typedef ENetSocket = TSocket;

{$define ENET_HOST_TO_NET_16:=htons}
{$define ENET_HOST_TO_NET_32:=htonl}

{$define ENET_NET_TO_HOST_16:=ntohs}
{$define ENET_NET_TO_HOST_32:=ntohl}

typedef ENetBuffer_PTR = ^ENetBuffer;ENetBuffer = struct
		dataLength:size_t;
		data:pointer;
	end;

{$define ENET_CALLBACK:=cdecl}

{$ifdef ENET_DLL}
{$else!ENET_DLL}
{$define ENET_API:=extern}
{$endif~ENET_DLL}

{$endif~__ENET_WIN32_H__}


