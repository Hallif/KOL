(**
 @file  protocol.h
 @brief ENet protocol
*)
{$ifndef __ENET_PROTOCOL_H__}
{$define __ENET_PROTOCOL_H__}

{$include'../enet/types_h.inc'}

const
	ENET_PROTOCOL_MINIMUM_MTU             = 576;
	ENET_PROTOCOL_MAXIMUM_MTU             = 4096;
	ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS = 32;
	ENET_PROTOCOL_MINIMUM_WINDOW_SIZE     = 4096;
	ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE     = 65536;
	ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT   = 1;
	ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT   = 255;
	ENET_PROTOCOL_MAXIMUM_PEER_ID         = $FFF;
	ENET_PROTOCOL_MAXIMUM_FRAGMENT_COUNT  = 1024*1024;

typedef{enum}ENetProtocolCommand=enet_uint8;const
	ENET_PROTOCOL_COMMAND_NONE               = 0;
	ENET_PROTOCOL_COMMAND_ACKNOWLEDGE        = 1;
	ENET_PROTOCOL_COMMAND_CONNECT            = 2;
	ENET_PROTOCOL_COMMAND_VERIFY_CONNECT     = 3;
	ENET_PROTOCOL_COMMAND_DISCONNECT         = 4;
	ENET_PROTOCOL_COMMAND_PING               = 5;
	ENET_PROTOCOL_COMMAND_SEND_RELIABLE      = 6;
	ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE    = 7;
	ENET_PROTOCOL_COMMAND_SEND_FRAGMENT      = 8;
	ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED   = 9;
	ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT    = 10;
	ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE = 11;
	ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE_FRAGMENT = 12;
	ENET_PROTOCOL_COMMAND_COUNT              = 13;

	ENET_PROTOCOL_COMMAND_MASK               = $0F;

	ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE = (1 << 7);
	ENET_PROTOCOL_COMMAND_FLAG_UNSEQUENCED = (1 << 6);

	ENET_PROTOCOL_HEADER_FLAG_COMPRESSED = (1 << 14);
	ENET_PROTOCOL_HEADER_FLAG_SENT_TIME  = (1 << 15);
	ENET_PROTOCOL_HEADER_FLAG_MASK       = ENET_PROTOCOL_HEADER_FLAG_COMPRESSED or ENET_PROTOCOL_HEADER_FLAG_SENT_TIME;

	ENET_PROTOCOL_HEADER_SESSION_MASK    = (3 << 12);
	ENET_PROTOCOL_HEADER_SESSION_SHIFT   = 12;

{$define ENET_PACKED:=packed}

typedef ENetProtocolHeader_PTR = ^ENetProtocolHeader;ENetProtocolHeader = ENET_PACKED struct
		peerID:enet_uint16;
		sentTime:enet_uint16;
	end;

typedef ENetProtocolCommandHeader = ENET_PACKED struct
		command:enet_uint8;
		channelID:enet_uint8;
		reliableSequenceNumber:enet_uint16;
	end;

typedef ENetProtocolAcknowledge = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		receivedReliableSequenceNumber:enet_uint16;
		receivedSentTime:enet_uint16;
	end;

typedef ENetProtocolConnect = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		outgoingPeerID:enet_uint16;
		incomingSessionID:enet_uint8;
		outgoingSessionID:enet_uint8;
		mtu:enet_uint32;
		windowSize:enet_uint32;
		channelCount:enet_uint32;
		incomingBandwidth:enet_uint32;
		outgoingBandwidth:enet_uint32;
		packetThrottleInterval:enet_uint32;
		packetThrottleAcceleration:enet_uint32;
		packetThrottleDeceleration:enet_uint32;
		connectID:enet_uint32;
		data:enet_uint32;
	end;

typedef ENetProtocolVerifyConnect = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		outgoingPeerID:enet_uint16;
		incomingSessionID:enet_uint8;
		outgoingSessionID:enet_uint8;
		mtu:enet_uint32;
		windowSize:enet_uint32;
		channelCount:enet_uint32;
		incomingBandwidth:enet_uint32;
		outgoingBandwidth:enet_uint32;
		packetThrottleInterval:enet_uint32;
		packetThrottleAcceleration:enet_uint32;
		packetThrottleDeceleration:enet_uint32;
		connectID:enet_uint32;
	end;

typedef ENetProtocolBandwidthLimit = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		incomingBandwidth:enet_uint32;
		outgoingBandwidth:enet_uint32;
	end;

typedef ENetProtocolThrottleConfigure = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		packetThrottleInterval:enet_uint32;
		packetThrottleAcceleration:enet_uint32;
		packetThrottleDeceleration:enet_uint32;
	end;

typedef ENetProtocolDisconnect = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		data:enet_uint32;
	end;

typedef ENetProtocolPing = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
	end;

typedef ENetProtocolSendReliable = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		dataLength:enet_uint16;
	end;

typedef ENetProtocolSendUnreliable = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		unreliableSequenceNumber:enet_uint16;
		dataLength:enet_uint16;
	end;

typedef ENetProtocolSendUnsequenced = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		unsequencedGroup:enet_uint16;
		dataLength:enet_uint16;
	end;

typedef ENetProtocolSendFragment = ENET_PACKED struct
		header:ENetProtocolCommandHeader;
		startSequenceNumber:enet_uint16;
		dataLength:enet_uint16;
		fragmentCount:enet_uint32;
		fragmentNumber:enet_uint32;
		totalLength:enet_uint32;
		fragmentOffset:enet_uint32;
	end;

typedef ENetProtocol_PTR = ^ENetProtocol;ENetProtocol = ENET_PACKED
	record case ENetProtocolCommand 
	of	ENET_PROTOCOL_COMMAND_NONE					:(header:ENetProtocolCommandHeader)
	;	ENET_PROTOCOL_COMMAND_ACKNOWLEDGE			:(acknowledge:ENetProtocolAcknowledge)
	;	ENET_PROTOCOL_COMMAND_CONNECT				:(connect:ENetProtocolConnect)
	;	ENET_PROTOCOL_COMMAND_VERIFY_CONNECT		:(verifyConnect:ENetProtocolVerifyConnect)
	;	ENET_PROTOCOL_COMMAND_DISCONNECT			:(disconnect:ENetProtocolDisconnect)
	;	ENET_PROTOCOL_COMMAND_PING					:(ping:ENetProtocolPing)
	;	ENET_PROTOCOL_COMMAND_SEND_RELIABLE			:(sendReliable:ENetProtocolSendReliable)
	;	ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE		:(sendUnreliable:ENetProtocolSendUnreliable)
	;	ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED		:(sendUnsequenced:ENetProtocolSendUnsequenced)
	;	ENET_PROTOCOL_COMMAND_SEND_FRAGMENT			:(sendFragment:ENetProtocolSendFragment)
	;	ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT		:(bandwidthLimit:ENetProtocolBandwidthLimit)
	;	ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE	:(throttleConfigure:ENetProtocolThrottleConfigure)
	end;

{$endif~__ENET_PROTOCOL_H__}

