(**
 @file host.c
 @brief ENet host management functions
*)
{.$define ENET_BUILDING_LIB 1}
{.$include <string.h>}
{$include'enet/enet_h.inc'}

(** @defgroup host ENet host functions
    @//
*)

(** Creates a host for communicating to peers.

    @param address the address at which other peers may connect to this host. If NULL, then no peers may connect to the host.
    @param peerCount the maximum number of peers that should be allocated for the host.
    @param channelLimit the maximum number of channels allowed; if 0, then this is equivalent to ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
    @param incomingBandwidth downstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
    @param outgoingBandwidth upstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.

    @returns the host on success and NULL on failure

    @remarks ENet will strategically drop packets on specific sides of a connection between hosts
    to ensure the host's bandwidth is not overwhelmed. The bandwidth parameters also determine
    the window size of a connection which limits the amount of reliable packets that may be in transit
    at any given time.
*)
function
enet_host_create ({const}address:ENetAddress_PTR; peerCount,channelLimit:size_t; incomingBandwidth,outgoingBandwidth:enet_uint32):
ENetHost_PTR;
ENET_API
var
	host:^ENetHost;
	currentPeer:^ENetPeer;
begin
	if(peerCount > ENET_PROTOCOL_MAXIMUM_PEER_ID)
	then return(NIL)
	;
	host:=ENetHost_PTR(enet_malloc (sizeof (ENetHost)));
	if(host = NIL)
	then return(NIL)
	;
	memset (host ,0 ,sizeof (ENetHost));

	host^.peers:=ENetPeer_PTR(enet_malloc (peerCount*sizeof (ENetPeer)));
	if(host^.peers = NIL)
	then begin
		enet_free (host);

		return(NIL);
	end;
	memset (host^.peers ,0 ,peerCount*sizeof (ENetPeer));

	host^.socket:=enet_socket_create (ENET_SOCKET_TYPE_DATAGRAM);
	if	(host^.socket = ENET_SOCKET_NULL)
	or	(	(address <> NIL)
		and	(enet_socket_bind (host^.socket ,address) < 0)
	)
	then begin
		if(host^.socket <> ENET_SOCKET_NULL)
		then enet_socket_destroy (host^.socket)
		;
		enet_free (host^.peers);
		enet_free (host);

		return(NIL);
	end;

	enet_socket_set_option (host^.socket ,ENET_SOCKOPT_NONBLOCK ,1);
	enet_socket_set_option (host^.socket ,ENET_SOCKOPT_BROADCAST ,1);
	enet_socket_set_option (host^.socket ,ENET_SOCKOPT_RCVBUF ,ENET_HOST_RECEIVE_BUFFER_SIZE);
	enet_socket_set_option (host^.socket ,ENET_SOCKOPT_SNDBUF ,ENET_HOST_SEND_BUFFER_SIZE);

	if	(address <> NIL)
	and	(enet_socket_get_address (host^.socket ,@ host^.address) < 0)
	then host^.address:=address^
	;
	if		(channelLimit = 0)
		or	(channelLimit > ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT)
	then channelLimit:=ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
	else if	(channelLimit < ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT)
	then channelLimit:=ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT
	;
	host^.randomSeed:=enet_uint32 (size_t(host));
	host^.randomSeed += enet_host_random_seed ();
	host^.randomSeed:=(host^.randomSeed << 16) or (host^.randomSeed >> 16);
	host^.channelLimit:=channelLimit;
	host^.incomingBandwidth:=incomingBandwidth;
	host^.outgoingBandwidth:=outgoingBandwidth;
	host^.bandwidthThrottleEpoch:=0;
	host^.recalculateBandwidthLimits:=0;
	host^.mtu:=ENET_HOST_DEFAULT_MTU;
	host^.peerCount:=peerCount;
	host^.commandCount:=0;
	host^.bufferCount:=0;
	host^.checksum:=NIL;
	host^.receivedAddress.host:=ENET_HOST_ANY;
	host^.receivedAddress.port:=0;
	host^.receivedData:=NIL;
	host^.receivedDataLength:=0;

	host^.totalSentData:=0;
	host^.totalSentPackets:=0;
	host^.totalReceivedData:=0;
	host^.totalReceivedPackets:=0;

	host^.connectedPeers:=0;
	host^.bandwidthLimitedPeers:=0;
	host^.duplicatePeers:=ENET_PROTOCOL_MAXIMUM_PEER_ID;
	host^.maximumPacketSize:=ENET_HOST_DEFAULT_MAXIMUM_PACKET_SIZE;
	host^.maximumWaitingData:=ENET_HOST_DEFAULT_MAXIMUM_WAITING_DATA;

	host^.compressor.context:=NIL;
	host^.compressor.compress:=NIL;
	host^.compressor.decompress:=NIL;
	host^.compressor.destroy:=NIL;

	host^.intercept:=NIL;

	enet_list_clear (@ host^.dispatchQueue);

	currentPeer:=@ host^.peers [0];while
	currentPeer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
	inc(currentPeer);continue;end;}
	do begin
		currentPeer^.host:=host;
		currentPeer^.incomingPeerID:=currentPeer - host^.peers;
		currentPeer^.outgoingSessionID:=$FF;
		currentPeer^.incomingSessionID:=$FF;
		currentPeer^.data:=NIL;

		enet_list_clear (@ currentPeer^.acknowledgements);
		enet_list_clear (@ currentPeer^.sentReliableCommands);
		enet_list_clear (@ currentPeer^.sentUnreliableCommands);
		enet_list_clear (@ currentPeer^.outgoingReliableCommands);
		enet_list_clear (@ currentPeer^.outgoingUnreliableCommands);
		enet_list_clear (@ currentPeer^.dispatchedCommands);

		enet_peer_reset (currentPeer)
		;
	continue_{$undef continue_}end
	;
	return(host);
end;

(** Destroys the host and all resources associated with it.
    @param host pointer to the host to destroy
*)
procedure
enet_host_destroy (host:ENetHost_PTR);
ENET_API
var
	currentPeer:^ENetPeer;
begin
	if(host = NIL)
	then return
	;
	enet_socket_destroy (host^.socket);

	currentPeer:=@ host^.peers [0];while
	currentPeer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
	inc(currentPeer);continue;end;}
	do begin
		enet_peer_reset (currentPeer)
		;
	continue_{$undef continue_}end
	;
	if	(host^.compressor.context <> NIL)
	and	assigned(host^.compressor.destroy)
	then host^.compressor.destroy (host^.compressor.context)
	;
	enet_free (host^.peers);
	enet_free (host);
end;

(** Initiates a connection to a foreign host.
    @param host host seeking the connection
    @param address destination for the connection
    @param channelCount number of channels to allocate
    @param data user data supplied to the receiving host 
    @returns a peer representing the foreign host on success, NULL on failure
    @remarks The peer returned will have not completed the connection until enet_host_service()
    notifies of an ENET_EVENT_TYPE_CONNECT event for the peer.
*)
function
enet_host_connect (host:ENetHost_PTR;{const}address:ENetAddress_PTR; channelCount:size_t; data:enet_uint32):
ENetPeer_PTR;
ENET_API
var
	currentPeer:^ENetPeer;
	channel:^ENetChannel;
	command:ENetProtocol;
begin
	if		(channelCount < ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT)
	then channelCount:=ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT
	else if	(channelCount > ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT)
	then channelCount:=ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
	;
	currentPeer:=@ host^.peers [0];while
	currentPeer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
	inc(currentPeer);continue;end;}
	do begin
		if(currentPeer^.state = ENET_PEER_STATE_DISCONNECTED)
		then break
		;
	continue_{$undef continue_}end
	;
	if(currentPeer >= @ host^.peers [host^.peerCount])
	then return(NIL)
	;
	currentPeer^.channels:=ENetChannel_PTR(enet_malloc (channelCount*sizeof (ENetChannel)));
	if(currentPeer^.channels = NIL)
	then return(NIL)
	;
	currentPeer^.channelCount:=channelCount;
	currentPeer^.state:=ENET_PEER_STATE_CONNECTING;
	currentPeer^.address:=address^;
	currentPeer^.connectID:=succ(host^.randomSeed);host^.randomSeed+=1;

	if(host^.outgoingBandwidth = 0)
	then currentPeer^.windowSize:=ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE
	else currentPeer^.windowSize:=(host^.outgoingBandwidth div ENET_PEER_WINDOW_SIZE_SCALE)*ENET_PROTOCOL_MINIMUM_WINDOW_SIZE
	;
	if		(currentPeer^.windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE)
	then currentPeer^.windowSize:=ENET_PROTOCOL_MINIMUM_WINDOW_SIZE
	else if	(currentPeer^.windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
	then currentPeer^.windowSize:=ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE
	;
	channel:=@ currentPeer^.channels [0];while
	channel <@ currentPeer^.channels [channelCount]{$define continue_:=begin;
	inc(Channel);continue;end;}
	do begin
		channel^.outgoingReliableSequenceNumber:=0;
		channel^.outgoingUnreliableSequenceNumber:=0;
		channel^.incomingReliableSequenceNumber:=0;
		channel^.incomingUnreliableSequenceNumber:=0;

		enet_list_clear (@ channel^.incomingReliableCommands);
		enet_list_clear (@ channel^.incomingUnreliableCommands);

		channel^.usedReliableWindows:=0;
		memset (addr(channel^.reliableWindows) ,0 ,sizeof (channel^.reliableWindows))
		;
	continue_{$undef continue_}end
	;
	command.header.command:=ENET_PROTOCOL_COMMAND_CONNECT or ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE;
	command.header.channelID:=$FF;
	command.connect.outgoingPeerID:=ENET_HOST_TO_NET_16 (currentPeer^.incomingPeerID);
	command.connect.incomingSessionID:=currentPeer^.incomingSessionID;
	command.connect.outgoingSessionID:=currentPeer^.outgoingSessionID;
	command.connect.mtu:=ENET_HOST_TO_NET_32 (currentPeer^.mtu);
	command.connect.windowSize:=ENET_HOST_TO_NET_32 (currentPeer^.windowSize);
	command.connect.channelCount:=ENET_HOST_TO_NET_32 (channelCount);
	command.connect.incomingBandwidth:=ENET_HOST_TO_NET_32 (host^.incomingBandwidth);
	command.connect.outgoingBandwidth:=ENET_HOST_TO_NET_32 (host^.outgoingBandwidth);
	command.connect.packetThrottleInterval:=ENET_HOST_TO_NET_32 (currentPeer^.packetThrottleInterval);
	command.connect.packetThrottleAcceleration:=ENET_HOST_TO_NET_32 (currentPeer^.packetThrottleAcceleration);
	command.connect.packetThrottleDeceleration:=ENET_HOST_TO_NET_32 (currentPeer^.packetThrottleDeceleration);
	command.connect.connectID:=currentPeer^.connectID;
	command.connect.data:=ENET_HOST_TO_NET_32 (data);

	enet_peer_queue_outgoing_command (currentPeer ,@ command ,NIL ,0 ,0);

	return(currentPeer);
end;

(** Queues a packet to be sent to all peers associated with the host.
    @param host host on which to broadcast the packet
    @param channelID channel on which to broadcast
    @param packet packet to broadcast
*)
procedure
enet_host_broadcast (host:ENetHost_PTR; channelID:enet_uint8; packet:ENetPacket_PTR);
ENET_API
var
	currentPeer:^ENetPeer;
begin
	currentPeer:=@ host^.peers [0];while
	currentPeer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
	inc(currentPeer);continue;end;}
	do begin
		if(currentPeer^.state <> ENET_PEER_STATE_CONNECTED)
		then continue_
		;
		enet_peer_send (currentPeer ,channelID ,packet)
		;
	continue_{$undef continue_}end
	;
	if(packet^.referenceCount = 0)
	then enet_packet_destroy (packet)
end;

(** Sets the packet compressor the host should use to compress and decompress packets.
    @param host host to enable or disable compression for
    @param compressor callbacks for for the packet compressor; if NULL, then compression is disabled
*)
procedure
enet_host_compress (host:ENetHost_PTR;{const}compressor:ENetCompressor_PTR);
ENET_API
begin
	if	(host^.compressor.context <> NIL)
	and	assigned(host^.compressor.destroy)
	then host^.compressor.destroy (host^.compressor.context)
	;
	if(compressor <> NIL)
	then host^.compressor:=compressor^
	else host^.compressor.context:=NIL
end;

(** Limits the maximum allowed channels of future incoming connections.
    @param host host to limit
    @param channelLimit the maximum number of channels allowed; if 0, then this is equivalent to ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
*)
procedure
enet_host_channel_limit (host:ENetHost_PTR; channelLimit:size_t);
ENET_API
begin
	if		(channelLimit = 0)
		or	(channelLimit > ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT)
	then channelLimit:=ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
	else if	(channelLimit < ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT)
	then channelLimit:=ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT
	;
	host^.channelLimit:=channelLimit;
end;

(** Adjusts the bandwidth limits of a host.
    @param host host to adjust
    @param incomingBandwidth new incoming bandwidth
    @param outgoingBandwidth new outgoing bandwidth
    @remarks the incoming and outgoing bandwidth parameters are identical in function to those
    specified in enet_host_create().
*)
procedure
enet_host_bandwidth_limit (host:ENetHost_PTR; incomingBandwidth,outgoingBandwidth:enet_uint32);
ENET_API
begin
	host^.incomingBandwidth:=incomingBandwidth;
	host^.outgoingBandwidth:=outgoingBandwidth;
	host^.recalculateBandwidthLimits:=1;
end;

procedure
enet_host_bandwidth_throttle (host:ENetHost_PTR);
var
	timeCurrent:enet_uint32;
	elapsedTime:enet_uint32;
	peersRemaining:enet_uint32;
	dataTotal:enet_uint32=not(enet_uint32(0));
	bandwidth:enet_uint32=not(enet_uint32(0));
	throttle:enet_uint32=0;
	bandwidthLimit:enet_uint32=0;
	needsAdjustment:int;
	peer:^ENetPeer;
	command:ENetProtocol;
	peerBandwidth:enet_uint32=0;
begin
	timeCurrent:=enet_time_get ();
	elapsedTime:=timeCurrent - host^.bandwidthThrottleEpoch;
	peersRemaining:=enet_uint32(host^.connectedPeers);

	needsAdjustment:=ifThen(host^.bandwidthLimitedPeers > 0 , 1 , 0);

	if(elapsedTime < ENET_HOST_BANDWIDTH_THROTTLE_INTERVAL)
	then return
	;
	host^.bandwidthThrottleEpoch:=timeCurrent;

	if(peersRemaining = 0)
	then return
	;
	if(host^.outgoingBandwidth <> 0)
	then begin
		dataTotal:=0;
		bandwidth:=(host^.outgoingBandwidth*elapsedTime) div 1000;

		peer:=@ host^.peers [0];while
		peer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
		inc(peer);continue;end;}
		do begin
			if	(peer^.state <> ENET_PEER_STATE_CONNECTED)
			and	(peer^.state <> ENET_PEER_STATE_DISCONNECT_LATER)
			then continue_
			;
			dataTotal += peer^.outgoingDataTotal
			;
		continue_{$undef continue_}end
	end;

	while (peersRemaining > 0) and (needsAdjustment <> 0)
	do begin
		needsAdjustment:=0;

		if(dataTotal <= bandwidth)
		then throttle:=ENET_PEER_PACKET_THROTTLE_SCALE
		else throttle:=(bandwidth*ENET_PEER_PACKET_THROTTLE_SCALE) div dataTotal
		;
		peer:=@ host^.peers [0];while
		peer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
		inc(peer);continue;end;}
		do begin
			if	(	(peer^.state <> ENET_PEER_STATE_CONNECTED)
				and	(peer^.state <> ENET_PEER_STATE_DISCONNECT_LATER)
			)
			or	(peer^.incomingBandwidth = 0)
			or	(peer^.outgoingBandwidthThrottleEpoch = timeCurrent)
			then continue_
			;
			peerBandwidth:=(peer^.incomingBandwidth*elapsedTime) div 1000;
			if	(throttle*peer^.outgoingDataTotal) div ENET_PEER_PACKET_THROTTLE_SCALE
			<=	peerBandwidth
			then continue_
			;
			peer^.packetThrottleLimit:=(peerBandwidth*ENET_PEER_PACKET_THROTTLE_SCALE) div peer^.outgoingDataTotal;

			if(peer^.packetThrottleLimit = 0)
			then peer^.packetThrottleLimit:=1
			;
			if(peer^.packetThrottle > peer^.packetThrottleLimit)
			then peer^.packetThrottle:=peer^.packetThrottleLimit
			;
			peer^.outgoingBandwidthThrottleEpoch:=timeCurrent;

			peer^.incomingDataTotal:=0;
			peer^.outgoingDataTotal:=0;

			needsAdjustment:=1;
			peersRemaining-=1;
			bandwidth -= peerBandwidth;
			dataTotal -= peerBandwidth;
			;
		continue_{$undef continue_}end
	end;

	if(peersRemaining > 0)
	then begin
		if(dataTotal <= bandwidth)
		then throttle:=ENET_PEER_PACKET_THROTTLE_SCALE
		else throttle:=(bandwidth*ENET_PEER_PACKET_THROTTLE_SCALE) div dataTotal
		;
		peer:=@ host^.peers [0];while
		peer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
		inc(peer);continue;end;}
		do begin
			if	(	(peer^.state <> ENET_PEER_STATE_CONNECTED)
				and	(peer^.state <> ENET_PEER_STATE_DISCONNECT_LATER)
			)
			or	(peer^.outgoingBandwidthThrottleEpoch = timeCurrent)
			then continue_
			;
			peer^.packetThrottleLimit:=throttle;

			if(peer^.packetThrottle > peer^.packetThrottleLimit)
			then peer^.packetThrottle:=peer^.packetThrottleLimit
			;
			peer^.incomingDataTotal:=0;
			peer^.outgoingDataTotal:=0;
			;
		continue_{$undef continue_}end
	end;

	if(host^.recalculateBandwidthLimits <> 0)
	then begin
		host^.recalculateBandwidthLimits:=0;

		peersRemaining:=enet_uint32(host^.connectedPeers);
		bandwidth:=host^.incomingBandwidth;
		needsAdjustment:=1;

		if(bandwidth = 0)
		then bandwidthLimit:=0
		else while (peersRemaining > 0) and (needsAdjustment <> 0)
		do begin
			needsAdjustment:=0;
			bandwidthLimit:=bandwidth div peersRemaining;

			peer:=@ host^.peers [0];while
			peer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
			inc(peer);continue;end;}
			do begin
				if	(	(peer^.state <> ENET_PEER_STATE_CONNECTED)
					and	(peer^.state <> ENET_PEER_STATE_DISCONNECT_LATER)
				)
				or	(peer^.incomingBandwidthThrottleEpoch = timeCurrent)
				then continue_
				;
				if	(peer^.outgoingBandwidth > 0)
				and	(peer^.outgoingBandwidth >= bandwidthLimit)
				then continue_
				;
				peer^.incomingBandwidthThrottleEpoch:=timeCurrent;

				needsAdjustment:=1;
				peersRemaining-=1;
				bandwidth -= peer^.outgoingBandwidth
				;
			continue_{$undef continue_}end
		end;

		peer:=@ host^.peers [0];while
		peer <@ host^.peers [host^.peerCount]{$define continue_:=begin;
		inc(peer);continue;end;}
		do begin
			if	(peer^.state <> ENET_PEER_STATE_CONNECTED)
			and	(peer^.state <> ENET_PEER_STATE_DISCONNECT_LATER)
			then continue_
			;
			command.header.command:=ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT or ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE;
			command.header.channelID:=$FF;
			command.bandwidthLimit.outgoingBandwidth:=ENET_HOST_TO_NET_32 (host^.outgoingBandwidth);

			if(peer^.incomingBandwidthThrottleEpoch = timeCurrent)
			then command.bandwidthLimit.incomingBandwidth:=ENET_HOST_TO_NET_32 (peer^.outgoingBandwidth)
			else command.bandwidthLimit.incomingBandwidth:=ENET_HOST_TO_NET_32 (bandwidthLimit)
			;
			enet_peer_queue_outgoing_command (peer ,@ command ,NIL ,0 ,0)
			;
		continue_{$undef continue_}end
	end;
end;

(** @ *)
