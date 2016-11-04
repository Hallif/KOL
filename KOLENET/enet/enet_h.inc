(**
 @file  enet.h
 @brief ENet public header file
*)
{$ifndef __ENET_ENET_H__}
{$define __ENET_ENET_H__}

{$ifdef __cplusplus}
extern "C"
//{
{$endif}

{.$include <stdlib.h>}

{$ifdef WIN32}
{$include'../enet/win32_h.inc'}
{$else}
{$include'../enet/unix_h.inc'}
{$endif}

{$include'../enet/types_h.inc'}
{$include'../enet/protocol_h.inc'}
{$include'../enet/list_h.inc'}
{$include'../enet/callbacks_h.inc'}

{$define ENET_VERSION_MAJOR:=1}
{$define ENET_VERSION_MINOR:=3}
{$define ENET_VERSION_PATCH:=13}
{$define ENET_VERSION_CREATE:=(((major)<<16) or ((minor)<<8) or (patch))}

typedef ENetVersion = enet_uint32;

{dw	ENetHost_PTR = ^ENetHost;}
{dw	ENetEvent_PTR = ^ENetEvent;}
{dw	ENetPacket_PTR = ^ENetPacket;}

typedef{enum}ENetSocketType=
(	ENET_SOCKET_TYPE_STREAM   = 1
,	ENET_SOCKET_TYPE_DATAGRAM = 2
)	;

const
	ENET_SOCKET_WAIT_NONE      = 0;
	ENET_SOCKET_WAIT_SEND      = (1 << 0);
	ENET_SOCKET_WAIT_RECEIVE   = (1 << 1);
	ENET_SOCKET_WAIT_INTERRUPT = (1 << 2);

typedef{enum}ENetSocketOption=
(	ENET_SOCKOPT_NONBLOCK  = 1
,	ENET_SOCKOPT_BROADCAST = 2
,	ENET_SOCKOPT_RCVBUF    = 3
,	ENET_SOCKOPT_SNDBUF    = 4
,	ENET_SOCKOPT_REUSEADDR = 5
,	ENET_SOCKOPT_RCVTIMEO  = 6
,	ENET_SOCKOPT_SNDTIMEO  = 7
,	ENET_SOCKOPT_ERROR     = 8
,	ENET_SOCKOPT_NODELAY   = 9
)	;

typedef{enum}ENetSocketShutdown=
(	ENET_SOCKET_SHUTDOWN_READ       = 0
,	ENET_SOCKET_SHUTDOWN_WRITE      = 1
,	ENET_SOCKET_SHUTDOWN_READ_WRITE = 2
)	;


(**
 * Portable internet address structure. 
 *
 * The host must be specified in network byte-order, and the port must be in host 
 * byte-order. The constant ENET_HOST_ANY may be used to specify the default 
 * server host. The constant ENET_HOST_BROADCAST may be used to specify the
 * broadcast address (255.255.255.255). This makes sense for enet_host_connect,
 * but not for enet_host_create. Once a server responds to a broadcast, the
 * address is updated from ENET_HOST_BROADCAST to the server's actual IP address.
 *)

(**
 * Packet flag bit constants.
 *
 * The host must be specified in network byte-order, and the port must be in
 * host byte-order. The constant ENET_HOST_ANY may be used to specify the
 * default server host.
 
   @sa ENetPacket
*)
const
   (** packet must be received by the target peer and resend attempts should be
     * made until the packet is delivered *)
	ENET_PACKET_FLAG_RELIABLE    = (1 << 0);
   (** packet will not be sequenced with other packets
     * not supported for reliable packets
     *)
	ENET_PACKET_FLAG_UNSEQUENCED = (1 << 1);
   (** packet will not allocate data, and user must supply it instead *)
	ENET_PACKET_FLAG_NO_ALLOCATE = (1 << 2);
   (** packet will be fragmented using unreliable (instead of reliable) sends
     * if it exceeds the MTU *)
	ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT = (1 << 3);

   (** whether the packet has been sent from all queues it has been entered into *)
	ENET_PACKET_FLAG_SENT = (1<<8);

type ENetPacket_PTR = ^ENetPacket;
{typedef}ENetPacketFreeCallback = procedure(packet:ENetPacket_PTR); ENET_CALLBACK;

(**
 * ENet packet structure.
 *
 * An ENet data packet that may be sent to or received from a peer. The shown 
 * fields should only be read and never modified. The data field contains the 
 * allocated data for the packet. The dataLength fields specifies the length 
 * of the allocated data. The flags field is either 0 (specifying no flags),
 * or a bitwise-or of any combination of the following flags:
 *
 *    ENET_PACKET_FLAG_RELIABLE - packet must be received by the target peer
 *    and resend attempts should be made until the packet is delivered
 *
 *    ENET_PACKET_FLAG_UNSEQUENCED - packet will not be sequenced with other packets 
 *    (not supported for reliable packets)
 *
 *    ENET_PACKET_FLAG_NO_ALLOCATE - packet will not allocate data, and user must supply it instead
 *
 *    ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT - packet will be fragmented using unreliable
 *    (instead of reliable) sends if it exceeds the MTU
 *
 *    ENET_PACKET_FLAG_SENT - whether the packet has been sent from all queues it has been entered into
   @sa ENetPacketFlag
 *)
{typedef}ENetPacket = struct
		referenceCount:size_t;                  (**< internal use only *)
		flags:enet_uint32;                      (**< bitwise-or of ENetPacketFlag constants *)
		data:enet_uint8_ptr;                    (**< allocated data for packet *)
		dataLength:size_t;                      (**< length of data *)
		freeCallback:ENetPacketFreeCallback;    (**< function to be called when the packet is no longer in use *)
		userData:pointer;                       (**< application private data, may be freely modified *)
	end;

typedef ENetAcknowledgement_PTR = ^ENetAcknowledgement;ENetAcknowledgement = struct
		acknowledgementList:ENetListNode;
		sentTime:enet_uint32;
		command:ENetProtocol;
	end;

typedef	ENetOutgoingCommand_PTR = ^ENetOutgoingCommand;ENetOutgoingCommand = struct
		outgoingCommandList:ENetListNode;
		reliableSequenceNumber		:enet_uint16;
		unreliableSequenceNumber	:enet_uint16;
		sentTime				:enet_uint32;
		roundTripTimeout		:enet_uint32;
		roundTripTimeoutLimit	:enet_uint32;
		fragmentOffset			:enet_uint32;
		fragmentLength	:enet_uint16;
		sendAttempts	:enet_uint16;
		command:ENetProtocol;
		packet:ENetPacket_PTR;
	end;

typedef ENetIncomingCommand_PTR = ^ENetIncomingCommand;ENetIncomingCommand = struct
		incomingCommandList:ENetListNode;
		reliableSequenceNumber		:enet_uint16;
		unreliableSequenceNumber	:enet_uint16;
		command:ENetProtocol;
		fragmentCount		:enet_uint32;
		fragmentsRemaining	:enet_uint32;
		fragments:enet_uint32_ptr;
		packet:ENetPacket_PTR;
	end;

typedef{enum}ENetPeerState=
(	ENET_PEER_STATE_DISCONNECTED                = 0
,	ENET_PEER_STATE_CONNECTING                  = 1
,	ENET_PEER_STATE_ACKNOWLEDGING_CONNECT       = 2
,	ENET_PEER_STATE_CONNECTION_PENDING          = 3
,	ENET_PEER_STATE_CONNECTION_SUCCEEDED        = 4
,	ENET_PEER_STATE_CONNECTED                   = 5
,	ENET_PEER_STATE_DISCONNECT_LATER            = 6
,	ENET_PEER_STATE_DISCONNECTING               = 7
,	ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT    = 8
,	ENET_PEER_STATE_ZOMBIE                      = 9
)	;

{$ifndef ENET_BUFFER_MAXIMUM}
{$define ENET_BUFFER_MAXIMUM:=system.succ(2*ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS)}
{$endif}

const
	ENET_HOST_RECEIVE_BUFFER_SIZE          = 256*1024;
	ENET_HOST_SEND_BUFFER_SIZE             = 256*1024;
	ENET_HOST_BANDWIDTH_THROTTLE_INTERVAL  = 1000;
	ENET_HOST_DEFAULT_MTU                  = 1400;
	ENET_HOST_DEFAULT_MAXIMUM_PACKET_SIZE  = 32*1024*1024;
	ENET_HOST_DEFAULT_MAXIMUM_WAITING_DATA = 32*1024*1024;

	ENET_PEER_DEFAULT_ROUND_TRIP_TIME      = 500;
	ENET_PEER_DEFAULT_PACKET_THROTTLE      = 32;
	ENET_PEER_PACKET_THROTTLE_SCALE        = 32;
	ENET_PEER_PACKET_THROTTLE_COUNTER      = 7;
	ENET_PEER_PACKET_THROTTLE_ACCELERATION = 2;
	ENET_PEER_PACKET_THROTTLE_DECELERATION = 2;
	ENET_PEER_PACKET_THROTTLE_INTERVAL     = 5000;
	ENET_PEER_PACKET_LOSS_SCALE            = (1 << 16);
	ENET_PEER_PACKET_LOSS_INTERVAL         = 10000;
	ENET_PEER_WINDOW_SIZE_SCALE            = 64*1024;
	ENET_PEER_TIMEOUT_LIMIT                = 32;
	ENET_PEER_TIMEOUT_MINIMUM              = 5000;
	ENET_PEER_TIMEOUT_MAXIMUM              = 30000;
//	ENET_PEER_PING_INTERVAL                = 500;
	ENET_PEER_UNSEQUENCED_WINDOWS          = 64;
	ENET_PEER_UNSEQUENCED_WINDOW_SIZE      = 1024;
	ENET_PEER_FREE_UNSEQUENCED_WINDOWS     = 32;
	ENET_PEER_RELIABLE_WINDOWS             = 16;
	ENET_PEER_RELIABLE_WINDOW_SIZE         = $1000;
	ENET_PEER_FREE_RELIABLE_WINDOWS        = 8;

typedef ENetChannel_PTR = ^ENetChannel;ENetChannel = struct
		outgoingReliableSequenceNumber:enet_uint16;
		outgoingUnreliableSequenceNumber:enet_uint16;
		usedReliableWindows:enet_uint16;
		reliableWindows:array[0..pred(ENET_PEER_RELIABLE_WINDOWS)]of enet_uint16;
		incomingReliableSequenceNumber:enet_uint16;
		incomingUnreliableSequenceNumber:enet_uint16;
		incomingReliableCommands:ENetList;
		incomingUnreliableCommands:ENetList;
	end;

{up}ENetHost_PTR = ^ENetHost;

(**
 * An ENet peer which data packets may be sent or received from. 
 *
 * No fields should be modified unless otherwise specified. 
 *)
{typedef}ENetPeer = struct
		dispatchList:ENetListNode;
		host:ENetHost_PTR;
		outgoingPeerID:enet_uint16;
		incomingPeerID:enet_uint16;
		connectID:enet_uint32;
		outgoingSessionID:enet_uint8;
		incomingSessionID:enet_uint8;
		address:ENetAddress;                             (**< Internet address of the peer *)
		data:pointer;                                    (**< Application private data, may be freely modified *)
		state:ENetPeerState;
		channels:^ENetChannel;
		channelCount:size_t;                             (**< Number of channels allocated for communication with peer *)
		incomingBandwidth				:enet_uint32;    (**< Downstream bandwidth of the client in bytes/second *)
		outgoingBandwidth				:enet_uint32;    (**< Upstream bandwidth of the client in bytes/second *)
		incomingBandwidthThrottleEpoch	:enet_uint32;
		outgoingBandwidthThrottleEpoch	:enet_uint32;
		incomingDataTotal				:enet_uint32;
		outgoingDataTotal				:enet_uint32;
		lastSendTime					:enet_uint32;
		lastReceiveTime					:enet_uint32;
		nextTimeout						:enet_uint32;
		earliestTimeout					:enet_uint32;
		packetLossEpoch					:enet_uint32;
		packetsSent						:enet_uint32;
		packetsLost						:enet_uint32;
		packetLoss						:enet_uint32;     (**< mean packet loss of reliable packets as a ratio with respect to the constant ENET_PEER_PACKET_LOSS_SCALE *)
		packetLossVariance				:enet_uint32;
		packetThrottle					:enet_uint32;
		packetThrottleLimit				:enet_uint32;
		packetThrottleCounter			:enet_uint32;
		packetThrottleEpoch				:enet_uint32;
		packetThrottleAcceleration		:enet_uint32;
		packetThrottleDeceleration		:enet_uint32;
		packetThrottleInterval			:enet_uint32;
		pingInterval					:enet_uint32;
		timeoutLimit					:enet_uint32;
		timeoutMinimum					:enet_uint32;
		timeoutMaximum					:enet_uint32;
		lastRoundTripTime				:enet_uint32;
		lowestRoundTripTime				:enet_uint32;
		lastRoundTripTimeVariance		:enet_uint32;
		highestRoundTripTimeVariance	:enet_uint32;
		roundTripTime					:enet_uint32;     (**< mean round trip time (RTT), in milliseconds, between sending a reliable packet and receiving its acknowledgement *)
		roundTripTimeVariance			:enet_uint32;
		mtu								:enet_uint32;
		windowSize						:enet_uint32;
		reliableDataInTransit			:enet_uint32;
		outgoingReliableSequenceNumber:enet_uint16;
		acknowledgements			:ENetList;
		sentReliableCommands		:ENetList;
		sentUnreliableCommands		:ENetList;
		outgoingReliableCommands	:ENetList;
		outgoingUnreliableCommands	:ENetList;
		dispatchedCommands			:ENetList;
		needsDispatch:int;
		incomingUnsequencedGroup:enet_uint16;
		outgoingUnsequencedGroup:enet_uint16;
		unsequencedWindow:array[0..pred(ENET_PEER_UNSEQUENCED_WINDOW_SIZE div 32)]of enet_uint32;
		eventData:enet_uint32;
		totalWaitingData:size_t;
	end;

(** An ENet packet compressor for compressing UDP packets before socket sends or receives.
 *)
{typedef}ENetCompressor_PTR = ^ENetCompressor;ENetCompressor = struct
   (** Context data for the compressor. Must be non-NULL. *)
		context:pointer;
   (** Compresses from inBuffers[0:inBufferCount-1], containing inLimit bytes, to outData, outputting at most outLimit bytes. Should return 0 on failure. *)
		compress:function (context:pointer;{const}inBuffers:ENetBuffer_PTR; inBufferCount,inLimit:size_t; outData:enet_uint8_ptr; outLimit:size_t):size_t; ENET_CALLBACK;
   (** Decompresses from inData, containing inLimit bytes, to outData, outputting at most outLimit bytes. Should return 0 on failure. *)
		decompress:function (context:pointer;{const}inData:enet_uint8_ptr; inLimit:size_t; outData:enet_uint8_ptr; outLimit:size_t):size_t; ENET_CALLBACK;
   (** Destroys the context when compression is disabled or the host is destroyed. May be NULL. *)
		destroy:procedure (context:pointer); ENET_CALLBACK;
	end;

(** Callback that computes the checksum of the data held in buffers[0:bufferCount-1] *)
{typedef}ENetChecksumCallback = function({const}buffers:ENetBuffer_PTR; bufferCount:size_t):enet_uint32; ENET_CALLBACK;

{up}ENetEvent_PTR = ^ENetEvent;
(** Callback for intercepting received raw UDP packets. Should return 1 to intercept, 0 to ignore, or -1 to propagate an error. *)
{typedef}ENetInterceptCallback = function(host:ENetHost_PTR; event:ENetEvent_PTR):int; ENET_CALLBACK;

(** An ENet host for communicating with peers.
  *
  * No fields should be modified unless otherwise stated.

    @sa enet_host_create()
    @sa enet_host_destroy()
    @sa enet_host_connect()
    @sa enet_host_service()
    @sa enet_host_flush()
    @sa enet_host_broadcast()
    @sa enet_host_compress()
    @sa enet_host_compress_with_range_coder()
    @sa enet_host_channel_limit()
    @sa enet_host_bandwidth_limit()
    @sa enet_host_bandwidth_throttle()
  *)
{typedef}ENetHost = struct
		socket:ENetSocket;
		address:ENetAddress;                              (**< Internet address of the host *)
		incomingBandwidth		:enet_uint32;             (**< downstream bandwidth of the host *)
		outgoingBandwidth		:enet_uint32;             (**< upstream bandwidth of the host *)
		bandwidthThrottleEpoch	:enet_uint32;
		mtu						:enet_uint32;
		randomSeed				:enet_uint32;
		recalculateBandwidthLimits:int;
		peers:^ENetPeer;                                  (**< array of peers allocated for this host *)
		peerCount		:size_t;                          (**< number of peers allocated for this host *)
		channelLimit	:size_t;                          (**< maximum number of channels allowed for connected peers *)
		serviceTime:enet_uint32;
		dispatchQueue:ENetList;
		continueSending:int;
		packetSize:size_t;
		headerFlags:enet_uint16;
		commands:array[0..pred(ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS)]of ENetProtocol;
		commandCount:size_t;
		buffers:array[0..pred(ENET_BUFFER_MAXIMUM)]of ENetBuffer;
		bufferCount:size_t;
		checksum:ENetChecksumCallback;                    (**< callback the user can set to enable packet checksums for this host *)
		compressor:ENetCompressor;
		packetData:array[0..1 ,0..pred(ENET_PROTOCOL_MAXIMUM_MTU)]of enet_uint8;
		receivedAddress:ENetAddress;
		receivedData:enet_uint8_ptr;
		receivedDataLength:size_t;
		totalSentData			:enet_uint32;             (**< total data sent, user should reset to 0 as needed to prevent overflow *)
		totalSentPackets		:enet_uint32;             (**< total UDP packets sent, user should reset to 0 as needed to prevent overflow *)
		totalReceivedData		:enet_uint32;             (**< total data received, user should reset to 0 as needed to prevent overflow *)
		totalReceivedPackets	:enet_uint32;             (**< total UDP packets received, user should reset to 0 as needed to prevent overflow *)
		intercept:ENetInterceptCallback;                  (**< callback the user can set to intercept received raw UDP packets *)
		connectedPeers			:size_t;
		bandwidthLimitedPeers	:size_t;
		duplicatePeers			:size_t;                  (**< optional number of allowed peers from duplicate IPs, defaults to ENET_PROTOCOL_MAXIMUM_PEER_ID *)
		maximumPacketSize		:size_t;                  (**< the maximum allowable packet size that may be sent or received on a peer *)
		maximumWaitingData		:size_t;                  (**< the maximum aggregate amount of buffer space a peer may use waiting for packets to be delivered *)
	end;

(**
 * An ENet event type, as specified in @ref ENetEvent.
 *)
{typedef}{enum}ENetEventType=
   (** no event occurred within the specified time limit *)
(	ENET_EVENT_TYPE_NONE       = 0

   (** a connection request initiated by enet_host_connect has completed.
     * The peer field contains the peer which successfully connected.
     *)
,	ENET_EVENT_TYPE_CONNECT    = 1

   (** a peer has disconnected. This event is generated on a successful
     * completion of a disconnect initiated by enet_peer_disconnect, if
     * a peer has timed out, or if a connection request intialized by
     * enet_host_connect has timed out. The peer field contains the peer
     * which disconnected. The data field contains user supplied data
     * describing the disconnection, or 0, if none is available.
     *)
,	ENET_EVENT_TYPE_DISCONNECT = 2

   (** a packet has been received from a peer. The peer field specifies the
     * peer which sent the packet. The channelID field specifies the channel
     * number upon which the packet was received. The packet field contains
     * the packet that was received; this packet must be destroyed with
     * enet_packet_destroy after use.
     *)
,	ENET_EVENT_TYPE_RECEIVE    = 3
)	;

(**
 * An ENet event as returned by enet_host_service().
   
   @sa enet_host_service
 *)
{typedef}ENetEvent = struct
		type_:ENetEventType;             (**< type of the event *)
		peer:^ENetPeer;                  (**< peer that generated a connect, disconnect or receive event *)
		channelID:enet_uint8;            (**< channel on the peer that generated the event, if appropriate *)
		data:enet_uint32;                (**< data associated with the event, if appropriate *)
		packet:ENetPacket_PTR;           (**< packet associated with the event, if appropriate *)
	end;

(** @defgroup global ENet global functions
    @//
*)

{++}ENetPeer_PTR = ^ENetPeer;

(**
  Initializes ENet globally. Must be called prior to using any functions in
  ENet.
  @returns 0 on success, < 0 on failure
*)
 function enet_initialize ({void}):int; ENET_API

(**
  Initializes ENet globally and supplies user-overridden callbacks. Must be called prior to using any functions in ENet. Do not use enet_initialize() if you use this variant. Make sure the ENetCallbacks structure is zeroed out so that any additional callbacks added in future versions will be properly ignored.

  @param version the constant ENET_VERSION should be supplied so ENet knows which version of ENetCallbacks struct to use
  @param inits user-overridden callbacks where any NULL callbacks will use ENet's defaults
  @returns 0 on success, < 0 on failure
*)
function enet_initialize_with_callbacks (version:ENetVersion;{const}inits:ENetCallbacks_PTR):int; ENET_API

(**
  Shuts down ENet globally. Should be called when a program that has
  initialized ENet exits.
*)
procedure enet_deinitialize ({void}); ENET_API

(**
  Gives the linked version of the ENet library.
  @returns the version number 
*)
function enet_linked_version ({void}):ENetVersion; ENET_API

(** @ *)

(** @defgroup private ENet private implementation functions *)

(**
  Returns the wall-time in milliseconds. Its initial value is unspecified
  unless otherwise set.
  *)
function enet_time_get ({void}):enet_uint32; ENET_API

(**
  Sets the current wall-time in milliseconds.
  *)
procedure enet_time_set (newTimeBase:enet_uint32); ENET_API

(** @defgroup socket ENet socket functions
    @//
*)
 function        enet_socket_create (_type:ENetSocketType):ENetSocket; ENET_API
 function        enet_socket_bind (socket:ENetSocket;{const}address:ENetAddress_PTR):int; ENET_API
 function        enet_socket_get_address (socket:ENetSocket; address:ENetAddress_PTR):int; ENET_API
 function        enet_socket_listen (socket:ENetSocket; backlog:int):int; ENET_API
 function        enet_socket_accept (socket:ENetSocket; address:ENetAddress_PTR):ENetSocket; ENET_API
 function        enet_socket_connect (socket:ENetSocket;{const}address:ENetAddress_PTR):int; ENET_API
 function        enet_socket_send (socket:ENetSocket;{const}address:ENetAddress_PTR;{const}buffers:ENetBuffer_PTR; bufferCount:size_t):int; ENET_API
 function        enet_socket_receive (socket:ENetSocket; address:ENetAddress_PTR; buffers:ENetBuffer_PTR; bufferCount:size_t):int; ENET_API
 function        enet_socket_wait (socket:ENetSocket; condition:enet_uint32_ptr; timeout:enet_uint32):int; ENET_API
 function        enet_socket_set_option (socket:ENetSocket; option:ENetSocketOption; value:int):int; ENET_API
 function        enet_socket_get_option (socket:ENetSocket; option:ENetSocketOption; value:int_ptr):int; ENET_API
 function        enet_socket_shutdown (socket:ENetSocket; how:ENetSocketShutdown):int; ENET_API
procedure        enet_socket_destroy (socket:ENetSocket); ENET_API
 function        enet_socketset_select (maxSocket:ENetSocket; readSet,writeSet:ENetSocketSet_PTR; timeout:enet_uint32):int; ENET_API

(** @ *)

(** @defgroup Address ENet address functions
    @//
*)
(** Attempts to resolve the host named by the parameter hostName and sets
    the host field in the address parameter if successful.
    @param address destination to store resolved address
    @param hostName host name to lookup
    @retval 0 on success
    @retval < 0 on failure
    @returns the address of the given hostName in address on success
*)
function enet_address_set_host (address:ENetAddress_PTR;{const}hostName:char_ptr):int; ENET_API

(** Gives the printable form of the IP address specified in the address parameter.
    @param address    address printed
    @param hostName   destination for name, must not be NULL
    @param nameLength maximum length of hostName.
    @returns the null-terminated name of the host in hostName on success
    @retval 0 on success
    @retval < 0 on failure
*)
function enet_address_get_host_ip ({const}address:ENetAddress_PTR; hostName:char_ptr; nameLength:size_t):int; ENET_API

(** Attempts to do a reverse lookup of the host field in the address parameter.
    @param address    address used for reverse lookup
    @param hostName   destination for name, must not be NULL
    @param nameLength maximum length of hostName.
    @returns the null-terminated name of the host in hostName on success
    @retval 0 on success
    @retval < 0 on failure
*)
function enet_address_get_host ({const}address:ENetAddress_PTR; hostName:char_ptr; nameLength:size_t):int; ENET_API

(** @ *)

 function        enet_packet_create ({const}data:pointer; dataLength:size_t; flags:enet_uint32):ENetPacket_PTR; ENET_API
procedure        enet_packet_destroy (packet:ENetPacket_PTR); ENET_API
 function        enet_packet_resize (packet:ENetPacket_PTR; dataLength:size_t):int; ENET_API
 function        enet_crc32 ({const}buffers:ENetBuffer_PTR; bufferCount:size_t):enet_uint32; ENET_API

function         enet_host_create ({const}address:ENetAddress_PTR; peerCount,channelLimit:size_t; incomingBandwidth,outgoingBandwidth:enet_uint32):ENetHost_PTR; ENET_API
procedure        enet_host_destroy (host:ENetHost_PTR); ENET_API
 function        enet_host_connect (host:ENetHost_PTR;{const}address:ENetAddress_PTR; channelCount:size_t; data:enet_uint32):ENetPeer_PTR; ENET_API
 function        enet_host_check_events (host:ENetHost_PTR; event:ENetEvent_PTR):int; ENET_API
 function        enet_host_service (host:ENetHost_PTR; event:ENetEvent_PTR; timeout:enet_uint32):int; ENET_API
procedure        enet_host_flush (host:ENetHost_PTR); ENET_API
procedure        enet_host_broadcast (host:ENetHost_PTR; channelID:enet_uint8; packet:ENetPacket_PTR); ENET_API
procedure        enet_host_compress (host:ENetHost_PTR;{const}compressor:ENetCompressor_PTR); ENET_API
 function        enet_host_compress_with_range_coder (host:ENetHost_PTR):int; ENET_API
procedure        enet_host_channel_limit (host:ENetHost_PTR; channelLimit:size_t); ENET_API
procedure        enet_host_bandwidth_limit (host:ENetHost_PTR; incomingBandwidth,outgoingBandwidth:enet_uint32); ENET_API
procedure{extern}enet_host_bandwidth_throttle (host:ENetHost_PTR);
 function{extern}enet_host_random_seed ({void}):enet_uint32;

 function        enet_peer_send (peer:ENetPeer_PTR; channelID:enet_uint8; packet:ENetPacket_PTR):int; ENET_API
 function        enet_peer_receive (peer:ENetPeer_PTR; channelID:enet_uint8_ptr):ENetPacket_PTR; ENET_API
procedure        enet_peer_ping (peer:ENetPeer_PTR); ENET_API
procedure        enet_peer_ping_interval (peer:ENetPeer_PTR; pingInterval:enet_uint32); ENET_API
procedure        enet_peer_timeout (peer:ENetPeer_PTR;timeoutLimit,timeoutMinimum,timeoutMaximum:enet_uint32); ENET_API
procedure        enet_peer_reset (peer:ENetPeer_PTR); ENET_API
procedure        enet_peer_disconnect (peer:ENetPeer_PTR; data:enet_uint32); ENET_API
procedure        enet_peer_disconnect_now (peer:ENetPeer_PTR; data:enet_uint32); ENET_API
procedure        enet_peer_disconnect_later (peer:ENetPeer_PTR; data:enet_uint32); ENET_API
procedure        enet_peer_throttle_configure (peer:ENetPeer_PTR; interval,acceleration,deceleration:enet_uint32);ENET_API
 function{extern}enet_peer_throttle (peer:ENetPeer_PTR; rtt:enet_uint32):int;
procedure{extern}enet_peer_reset_queues (peer:ENetPeer_PTR);
procedure{extern}enet_peer_setup_outgoing_command (peer:ENetPeer_PTR; outgoingCommand:ENetOutgoingCommand_PTR);
 function{extern}enet_peer_queue_outgoing_command (peer:ENetPeer_PTR;{const}command:ENetProtocol_PTR; packet:ENetPacket_PTR; offset:enet_uint32; length:enet_uint16):ENetOutgoingCommand_PTR;
 function{extern}enet_peer_queue_incoming_command (peer:ENetPeer_PTR;{const}command:ENetProtocol_PTR;{const}data:pointer; dataLength:size_t; flags:enet_uint32; fragmentCount:enet_uint32):ENetIncomingCommand_PTR;
 function{extern}enet_peer_queue_acknowledgement (peer:ENetPeer_PTR;{const}command:ENetProtocol_PTR; sentTime:enet_uint16):ENetAcknowledgement_PTR;
procedure{extern}enet_peer_dispatch_incoming_unreliable_commands (peer:ENetPeer_PTR; channel:ENetChannel_PTR);
procedure{extern}enet_peer_dispatch_incoming_reliable_commands (peer:ENetPeer_PTR; channel:ENetChannel_PTR);
procedure{extern}enet_peer_on_connect (peer:ENetPeer_PTR);
procedure{extern}enet_peer_on_disconnect (peer:ENetPeer_PTR);

 function        enet_range_coder_create ({void}):pointer; ENET_API
procedure        enet_range_coder_destroy (context:pointer); ENET_CALLBACK;
 function        enet_range_coder_compress (context:pointer;{const}inBuffers:ENetBuffer_PTR; inBufferCount,inLimit:size_t; outData:enet_uint8_ptr; outLimit:size_t):size_t; ENET_CALLBACK;
 function        enet_range_coder_decompress (context:pointer;{const}inData:enet_uint8_ptr; inLimit:size_t; outData:enet_uint8_ptr; outLimit:size_t):size_t; ENET_CALLBACK;

 function{extern}enet_protocol_command_size (commandNumber:enet_uint8):size_t;

{$ifdef __cplusplus}
//}
{$endif}

{$endif~__ENET_ENET_H__}

