(**
 @file  peer.c
 @brief ENet peer management functions
*)
{.$include <string.h>}
{.$define ENET_BUILDING_LIB:=1}
{$include'enet/enet_h.inc'}

(** @defgroup peer ENet peer functions
    @//{
*)

(** Configures throttle parameter for a peer.

    Unreliable packets are dropped by ENet in response to the varying conditions
    of the Internet connection to the peer. The throttle represents a probability
    that an unreliable packet should not be dropped and thus sent by ENet to the peer.
    The lowest mean round trip time from the sending of a reliable packet to the
    receipt of its acknowledgement is measured over an amount of time specified by
    the interval parameter in milliseconds. If a measured round trip time happens to
    be significantly less than the mean round trip time measured over the interval,
    then the throttle probability is increased to allow more traffic by an amount
    specified in the acceleration parameter, which is a ratio to the ENET_PEER_PACKET_THROTTLE_SCALE
    constant. If a measured round trip time happens to be significantly greater than
    the mean round trip time measured over the interval, then the throttle probability
    is decreased to limit traffic by an amount specified in the deceleration parameter, which
    is a ratio to the ENET_PEER_PACKET_THROTTLE_SCALE constant. When the throttle has
    a value of ENET_PEER_PACKET_THROTTLE_SCALE, no unreliable packets are dropped by
    ENet, and so 100% of all unreliable packets will be sent. When the throttle has a
    value of 0, all unreliable packets are dropped by ENet, and so 0% of all unreliable
    packets will be sent. Intermediate values for the throttle represent intermediate
    probabilities between 0% and 100% of unreliable packets being sent. The bandwidth
    limits of the local and foreign hosts are taken into account to determine a
    sensible limit for the throttle probability above which it should not raise even in
    the best of conditions.

    @param peer peer to configure
    @param interval interval, in milliseconds, over which to measure lowest mean RTT; the default value is ENET_PEER_PACKET_THROTTLE_INTERVAL.
    @param acceleration rate at which to increase the throttle probability as mean RTT declines
    @param deceleration rate at which to decrease the throttle probability as mean RTT increases
*)
procedure
enet_peer_throttle_configure (peer:ENetPeer_PTR; interval,acceleration,deceleration:enet_uint32);
var
	command:ENetProtocol;
begin

	peer^.packetThrottleInterval:=interval;
	peer^.packetThrottleAcceleration:=acceleration;
	peer^.packetThrottleDeceleration:=deceleration;

	command.header.command:=ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE or ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE;
	command.header.channelID:=$FF;

	command.throttleConfigure.packetThrottleInterval:=ENET_HOST_TO_NET_32 (interval);
	command.throttleConfigure.packetThrottleAcceleration:=ENET_HOST_TO_NET_32 (acceleration);
	command.throttleConfigure.packetThrottleDeceleration:=ENET_HOST_TO_NET_32 (deceleration);

	enet_peer_queue_outgoing_command(peer ,@ command ,NIL ,0 ,0);
end;

function
enet_peer_throttle (peer:ENetPeer_PTR; rtt:enet_uint32):
int;
begin
	if		(peer^.lastRoundTripTime <= peer^.lastRoundTripTimeVariance)
	then begin
		peer^.packetThrottle:=peer^.packetThrottleLimit;
	end
	else if	(rtt < peer^.lastRoundTripTime)
	then begin
		peer^.packetThrottle += peer^.packetThrottleAcceleration;

		if(peer^.packetThrottle > peer^.packetThrottleLimit)
		then peer^.packetThrottle:=peer^.packetThrottleLimit
		;
		return(1);
	end
	else if	(rtt > (peer^.lastRoundTripTime + 2*peer^.lastRoundTripTimeVariance))
	then begin
		if(peer^.packetThrottle > peer^.packetThrottleDeceleration)
		then peer^.packetThrottle -= peer^.packetThrottleDeceleration
		else peer^.packetThrottle:=0
		;
		return(-1);
	end;

	return(0);
end;

(** Queues a packet to be sent.
    @param peer destination for the packet
    @param channelID channel on which to send
    @param packet packet to send
    @retval 0 on success
    @retval < 0 on failure
*)
function
enet_peer_send (peer:ENetPeer_PTR; channelID:enet_uint8; packet:ENetPacket_PTR):
int;
var
	channel:^ENetChannel;
	command:ENetProtocol;
	fragmentLength:size_t;
var
	fragmentCount:enet_uint32;
	fragmentNumber:enet_uint32;
	fragmentOffset:enet_uint32;
	commandNumber:enet_uint8;
	startSequenceNumber:enet_uint16;
	fragments:ENetList;
	fragment:^ENetOutgoingCommand;
begin
	channel:=@ peer^.channels [channelID];
	if	(peer^.state <> ENET_PEER_STATE_CONNECTED)
	or	(channelID >= peer^.channelCount)
	or	(packet^.dataLength > peer^.host^.maximumPacketSize)
	then return(-1)
	;
	fragmentLength:=peer^.mtu - sizeof (ENetProtocolHeader) - sizeof (ENetProtocolSendFragment);
	if assigned(peer^.host^.checksum)
	then fragmentLength -= sizeof (enet_uint32)
	;
	if(packet^.dataLength > fragmentLength)
	then begin
		fragmentCount:=(packet^.dataLength + pred(fragmentLength)) div fragmentLength;
		if(fragmentCount > ENET_PROTOCOL_MAXIMUM_FRAGMENT_COUNT)
		then return(-1)
		;
		if	(	(packet^.flags and (ENET_PACKET_FLAG_RELIABLE or ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT))
			=	ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT
		)
		and	(channel^.outgoingUnreliableSequenceNumber < $FFFF)
		then begin
			commandNumber:=ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE_FRAGMENT;
			startSequenceNumber:=ENET_HOST_TO_NET_16 (succ(channel^.outgoingUnreliableSequenceNumber));
		end
		else begin
			commandNumber:=ENET_PROTOCOL_COMMAND_SEND_FRAGMENT or ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE;
			startSequenceNumber:=ENET_HOST_TO_NET_16 (succ(channel^.outgoingReliableSequenceNumber));
		end;

		enet_list_clear (@ fragments);

		fragmentNumber:=0;fragmentOffset:=0;while
		fragmentOffset < packet^.dataLength{$define continue_:=begin;
		fragmentNumber+=1;fragmentOffset += fragmentLength;continue;end}
		do begin
			if(packet^.dataLength - fragmentOffset) < fragmentLength
			then fragmentLength:=packet^.dataLength - fragmentOffset
			;
			fragment:=ENetOutgoingCommand_PTR(enet_malloc (sizeof (ENetOutgoingCommand)));
			if(fragment = NIL)
			then begin
				while not enet_list_empty (@ fragments)
				do begin
					fragment:=ENetOutgoingCommand_PTR(enet_list_remove (enet_list_begin (@ fragments)));

					enet_free (fragment);
				end;

				return(-1);
			end;

			fragment^.fragmentOffset:=fragmentOffset;
			fragment^.fragmentLength:=fragmentLength;
			fragment^.packet:=packet;
			fragment^.command.header.command:=commandNumber;
			fragment^.command.header.channelID:=channelID;
			fragment^.command.sendFragment.startSequenceNumber:=startSequenceNumber;
			fragment^.command.sendFragment.dataLength:=ENET_HOST_TO_NET_16 (fragmentLength);
			fragment^.command.sendFragment.fragmentCount:=ENET_HOST_TO_NET_32 (fragmentCount);
			fragment^.command.sendFragment.fragmentNumber:=ENET_HOST_TO_NET_32 (fragmentNumber);
			fragment^.command.sendFragment.totalLength:=ENET_HOST_TO_NET_32 (packet^.dataLength);
			fragment^.command.sendFragment.fragmentOffset:=ENET_NET_TO_HOST_32 (fragmentOffset);

			enet_list_insert (enet_list_end (@ fragments) ,fragment)
			;
		continue_{$undef continue_}end
		;
		packet^.referenceCount += fragmentNumber;

		while not enet_list_empty (@ fragments)
		do begin
			fragment:=ENetOutgoingCommand_PTR(enet_list_remove (enet_list_begin (@ fragments)));

			enet_peer_setup_outgoing_command (peer ,fragment);
		end;

		return(0);
	end;

	command.header.channelID:=channelID;

	if		(packet^.flags and (ENET_PACKET_FLAG_RELIABLE or ENET_PACKET_FLAG_UNSEQUENCED)) = ENET_PACKET_FLAG_UNSEQUENCED
	then begin
		command.header.command:=ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED or ENET_PROTOCOL_COMMAND_FLAG_UNSEQUENCED;
		command.sendUnsequenced.dataLength:=ENET_HOST_TO_NET_16 (packet^.dataLength);
	end
	else if	(packet^.flags and ENET_PACKET_FLAG_RELIABLE <> 0)
		or	(channel^.outgoingUnreliableSequenceNumber >= $FFFF)
	then begin
		command.header.command:=ENET_PROTOCOL_COMMAND_SEND_RELIABLE or ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE;
		command.sendReliable.dataLength:=ENET_HOST_TO_NET_16 (packet^.dataLength);
	end
	else begin
		command.header.command:=ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE;
		command.sendUnreliable.dataLength:=ENET_HOST_TO_NET_16 (packet^.dataLength);
	end;

	if enet_peer_queue_outgoing_command (peer ,@ command ,packet ,0 ,packet^.dataLength) = NIL
	then return(-1)
	;
	return(0);
end;

(** Attempts to dequeue any incoming queued packet.
    @param peer peer to dequeue packets from
    @param channelID holds the channel ID of the channel the packet was received on success
    @returns a pointer to the packet, or NULL if there are no available incoming queued packets
*)
function
enet_peer_receive (peer:ENetPeer_PTR; channelID:enet_uint8_ptr):
ENetPacket_PTR;
var
	incomingCommand:^ENetIncomingCommand;
	packet:^ENetPacket;
begin
	if enet_list_empty (@ peer^.dispatchedCommands)
	then return(NIL)
	;
	incomingCommand:=ENetIncomingCommand_PTR(enet_list_remove (enet_list_begin (@ peer^.dispatchedCommands)));

	if(channelID <> NIL)
	then channelID^:=incomingCommand^.command.header.channelID
	;
	packet:=incomingCommand^.packet;

	packet^.referenceCount-=1;

	if(incomingCommand^.fragments <> NIL)
	then enet_free (incomingCommand^.fragments)
	;
	enet_free (incomingCommand);

	peer^.totalWaitingData -= packet^.dataLength;

	return(packet);
end;

{static}procedure
enet_peer_reset_outgoing_commands (queue:ENetList_PTR);
var
	outgoingCommand:^ENetOutgoingCommand;
begin
	while not enet_list_empty (queue)
	do begin
		outgoingCommand:=ENetOutgoingCommand_PTR(enet_list_remove (enet_list_begin (queue)));

		if(outgoingCommand^.packet <> NIL)
		then begin
			outgoingCommand^.packet^.referenceCount-=1;

			if(outgoingCommand^.packet^.referenceCount = 0)
			then enet_packet_destroy (outgoingCommand^.packet)
		end;

		enet_free (outgoingCommand);
	end;
end;

{static}procedure
enet_peer_remove_incoming_commands (queue:ENetList_PTR; startCommand,endCommand:ENetListIterator);
var
	currentCommand:ENetListIterator;
	incomingCommand:^ENetIncomingCommand;
begin
	currentCommand:=startCommand;while
	currentCommand<>endCommand
	do begin
		incomingCommand:=ENetIncomingCommand_PTR(currentCommand);

		currentCommand:=enet_list_next (currentCommand);

		enet_list_remove (@ incomingCommand^.incomingCommandList);

		if(incomingCommand^.packet <> NIL)
		then begin
			incomingCommand^.packet^.referenceCount-=1;

			if(incomingCommand^.packet^.referenceCount = 0)
			then enet_packet_destroy (incomingCommand^.packet)
		end;

		if(incomingCommand^.fragments <> NIL)
		then enet_free (incomingCommand^.fragments)
		;
		enet_free (incomingCommand);
	end;
end;

{static}procedure
enet_peer_reset_incoming_commands (queue:ENetList_PTR);
begin
	enet_peer_remove_incoming_commands (queue ,enet_list_begin (queue) ,enet_list_end (queue));
end;

procedure
enet_peer_reset_queues (peer:ENetPeer_PTR);
var
	channel:^ENetChannel;
begin
	if(peer^.needsDispatch <> 0)
	then begin
		enet_list_remove (@ peer^.dispatchList);

		peer^.needsDispatch:=0;
	end;

	while not enet_list_empty (@ peer^.acknowledgements)
	do enet_free (enet_list_remove (enet_list_begin (@ peer^.acknowledgements)))
	;

	enet_peer_reset_outgoing_commands (@ peer^.sentReliableCommands);
	enet_peer_reset_outgoing_commands (@ peer^.sentUnreliableCommands);
	enet_peer_reset_outgoing_commands (@ peer^.outgoingReliableCommands);
	enet_peer_reset_outgoing_commands (@ peer^.outgoingUnreliableCommands);
	enet_peer_reset_incoming_commands (@ peer^.dispatchedCommands);

	if(peer^.channels <> NIL) and (peer^.channelCount > 0)
	then begin
		channel:=@ peer^.channels [0];while
		channel <@ peer^.channels [peer^.channelCount]{$define continue_:=begin;
		inc(channel);continue;end}
		do begin
			enet_peer_reset_incoming_commands (@ channel^.incomingReliableCommands);
			enet_peer_reset_incoming_commands (@ channel^.incomingUnreliableCommands);
			;
		continue_{$undef continue_}end
		;
		enet_free (peer^.channels);
	end;

	peer^.channels:=NIL;
	peer^.channelCount:=0;
end;

procedure
enet_peer_on_connect (peer:ENetPeer_PTR);
begin
	if	(peer^.state <> ENET_PEER_STATE_CONNECTED)
	and	(peer^.state <> ENET_PEER_STATE_DISCONNECT_LATER)
	then begin
		if(peer^.incomingBandwidth <> 0)
		then peer^.host^.bandwidthLimitedPeers+=1
		;
		peer^.host^.connectedPeers+=1;
	end
end;

procedure
enet_peer_on_disconnect (peer:ENetPeer_PTR);
begin
	if	(peer^.state = ENET_PEER_STATE_CONNECTED)
	or	(peer^.state = ENET_PEER_STATE_DISCONNECT_LATER)
	then begin
		if(peer^.incomingBandwidth <> 0)
		then peer^.host^.bandwidthLimitedPeers-=1
		;
		peer^.host^.connectedPeers-=1;
	end
end;

(** Forcefully disconnects a peer.
    @param peer peer to forcefully disconnect
    @remarks The foreign host represented by the peer is not notified of the disconnection and will timeout
    on its connection to the local host.
*)
procedure
enet_peer_reset (peer:ENetPeer_PTR);
begin
	enet_peer_on_disconnect (peer);

	peer^.outgoingPeerID:=ENET_PROTOCOL_MAXIMUM_PEER_ID;
	peer^.connectID:=0;

	peer^.state:=ENET_PEER_STATE_DISCONNECTED;

	peer^.incomingBandwidth:=0;
	peer^.outgoingBandwidth:=0;
	peer^.incomingBandwidthThrottleEpoch:=0;
	peer^.outgoingBandwidthThrottleEpoch:=0;
	peer^.incomingDataTotal:=0;
	peer^.outgoingDataTotal:=0;
	peer^.lastSendTime:=0;
	peer^.lastReceiveTime:=0;
	peer^.nextTimeout:=0;
	peer^.earliestTimeout:=0;
	peer^.packetLossEpoch:=0;
	peer^.packetsSent:=0;
	peer^.packetsLost:=0;
	peer^.packetLoss:=0;
	peer^.packetLossVariance:=0;
	peer^.packetThrottle:=ENET_PEER_DEFAULT_PACKET_THROTTLE;
	peer^.packetThrottleLimit:=ENET_PEER_PACKET_THROTTLE_SCALE;
	peer^.packetThrottleCounter:=0;
	peer^.packetThrottleEpoch:=0;
	peer^.packetThrottleAcceleration:=ENET_PEER_PACKET_THROTTLE_ACCELERATION;
	peer^.packetThrottleDeceleration:=ENET_PEER_PACKET_THROTTLE_DECELERATION;
	peer^.packetThrottleInterval:=ENET_PEER_PACKET_THROTTLE_INTERVAL;
	peer^.pingInterval:=500{ENET_PEER_PING_INTERVAL};
	peer^.timeoutLimit:=ENET_PEER_TIMEOUT_LIMIT;
	peer^.timeoutMinimum:=ENET_PEER_TIMEOUT_MINIMUM;
	peer^.timeoutMaximum:=ENET_PEER_TIMEOUT_MAXIMUM;
	peer^.lastRoundTripTime:=ENET_PEER_DEFAULT_ROUND_TRIP_TIME;
	peer^.lowestRoundTripTime:=ENET_PEER_DEFAULT_ROUND_TRIP_TIME;
	peer^.lastRoundTripTimeVariance:=0;
	peer^.highestRoundTripTimeVariance:=0;
	peer^.roundTripTime:=ENET_PEER_DEFAULT_ROUND_TRIP_TIME;
	peer^.roundTripTimeVariance:=0;
	peer^.mtu:=peer^.host^.mtu;
	peer^.reliableDataInTransit:=0;
	peer^.outgoingReliableSequenceNumber:=0;
	peer^.windowSize:=ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
	peer^.incomingUnsequencedGroup:=0;
	peer^.outgoingUnsequencedGroup:=0;
	peer^.eventData:=0;
	peer^.totalWaitingData:=0;

	fillChar(peer^.unsequencedWindow ,sizeof (peer^.unsequencedWindow) ,0);

	enet_peer_reset_queues (peer);
end;

(** Sends a ping request to a peer.
    @param peer destination for the ping request
    @remarks ping requests factor into the mean round trip time as designated by the
    roundTripTime field in the ENetPeer structure. ENet automatically pings all connected
    peers at regular intervals, however, this function may be called to ensure more
    frequent ping requests.
*)
procedure
enet_peer_ping (peer:ENetPeer_PTR);
var
	command:ENetProtocol;
begin
	if(peer^.state <> ENET_PEER_STATE_CONNECTED)
	then return
	;
	command.header.command:=ENET_PROTOCOL_COMMAND_PING or ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE;
	command.header.channelID:=$FF;

	enet_peer_queue_outgoing_command (peer ,@ command ,NIL ,0 ,0);
end;

(** Sets the interval at which pings will be sent to a peer.

    Pings are used both to monitor the liveness of the connection and also to dynamically
    adjust the throttle during periods of low traffic so that the throttle has reasonable
    responsiveness during traffic spikes.

    @param peer the peer to adjust
    @param pingInterval the interval at which to send pings; defaults to ENET_PEER_PING_INTERVAL if 0
*)
procedure
enet_peer_ping_interval (peer:ENetPeer_PTR; pingInterval:enet_uint32);
begin
	if(pingInterval <> 0)
	then peer^.pingInterval:=pingInterval
	else peer^.pingInterval:=500{ENET_PEER_PING_INTERVAL}
end;

(** Sets the timeout parameters for a peer.

    The timeout parameter control how and when a peer will timeout from a failure to acknowledge
    reliable traffic. Timeout values use an exponential backoff mechanism, where if a reliable
    packet is not acknowledge within some multiple of the average RTT plus a variance tolerance,
    the timeout will be doubled until it reaches a set limit. If the timeout is thus at this
    limit and reliable packets have been sent but not acknowledged within a certain minimum time
    period, the peer will be disconnected. Alternatively, if reliable packets have been sent
    but not acknowledged for a certain maximum time period, the peer will be disconnected regardless
    of the current timeout limit value.

    @param peer the peer to adjust
    @param timeoutLimit the timeout limit; defaults to ENET_PEER_TIMEOUT_LIMIT if 0
    @param timeoutMinimum the timeout minimum; defaults to ENET_PEER_TIMEOUT_MINIMUM if 0
    @param timeoutMaximum the timeout maximum; defaults to ENET_PEER_TIMEOUT_MAXIMUM if 0
*)

procedure
enet_peer_timeout (peer:ENetPeer_PTR;timeoutLimit,timeoutMinimum,timeoutMaximum:enet_uint32);
begin
	if(timeoutLimit <> 0)
	then peer^.timeoutLimit:=timeoutLimit
	else peer^.timeoutLimit:=ENET_PEER_TIMEOUT_LIMIT
	;
	if(timeoutMinimum <> 0)
	then peer^.timeoutMinimum:=timeoutMinimum
	else peer^.timeoutMinimum:=ENET_PEER_TIMEOUT_MINIMUM
	;
	if(timeoutMaximum <> 0)
	then peer^.timeoutMaximum:=timeoutMaximum
	else peer^.timeoutMaximum:=ENET_PEER_TIMEOUT_MAXIMUM
end;

(** Force an immediate disconnection from a peer.
    @param peer peer to disconnect
    @param data data describing the disconnection
    @remarks No ENET_EVENT_DISCONNECT event will be generated. The foreign peer is not
    guaranteed to receive the disconnect notification, and is reset immediately upon
    return from this function.
*)
procedure
enet_peer_disconnect_now (peer:ENetPeer_PTR; data:enet_uint32);
var
	command:ENetProtocol;
begin
	if(peer^.state = ENET_PEER_STATE_DISCONNECTED)
	then return
	;
	if	(peer^.state <> ENET_PEER_STATE_ZOMBIE)
	and	(peer^.state <> ENET_PEER_STATE_DISCONNECTING)
	then begin
		enet_peer_reset_queues (peer);

		command.header.command:=ENET_PROTOCOL_COMMAND_DISCONNECT or ENET_PROTOCOL_COMMAND_FLAG_UNSEQUENCED;
		command.header.channelID:=$FF;
		command.disconnect.data:=ENET_HOST_TO_NET_32 (data);

		enet_peer_queue_outgoing_command (peer ,@ command ,NIL ,0 ,0);

		enet_host_flush (peer^.host);
	end;

	enet_peer_reset (peer);
end;

(** Request a disconnection from a peer.
    @param peer peer to request a disconnection
    @param data data describing the disconnection
    @remarks An ENET_EVENT_DISCONNECT event will be generated by enet_host_service()
    once the disconnection is complete.
*)
procedure
enet_peer_disconnect (peer:ENetPeer_PTR; data:enet_uint32);
var
	command:ENetProtocol;
begin
	if	(peer^.state = ENET_PEER_STATE_DISCONNECTING)
	or	(peer^.state = ENET_PEER_STATE_DISCONNECTED)
	or	(peer^.state = ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT)
	or	(peer^.state = ENET_PEER_STATE_ZOMBIE)
	then return
	;
	enet_peer_reset_queues (peer);

	command.header.command:=ENET_PROTOCOL_COMMAND_DISCONNECT;
	command.header.channelID:=$FF;
	command.disconnect.data:=ENET_HOST_TO_NET_32 (data);

	if	(peer^.state = ENET_PEER_STATE_CONNECTED)
	or	(peer^.state = ENET_PEER_STATE_DISCONNECT_LATER)
	then command.header.command:=command.header.command or ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE
	else command.header.command:=command.header.command or ENET_PROTOCOL_COMMAND_FLAG_UNSEQUENCED
	;
	enet_peer_queue_outgoing_command(peer ,@ command ,NIL ,0 ,0);

	if	(peer^.state = ENET_PEER_STATE_CONNECTED)
	or	(peer^.state = ENET_PEER_STATE_DISCONNECT_LATER)
	then begin
		enet_peer_on_disconnect (peer);

		peer^.state:=ENET_PEER_STATE_DISCONNECTING;
	end
	else begin
		enet_host_flush (peer^.host);
		enet_peer_reset (peer);
	end
end;

(** Request a disconnection from a peer, but only after all queued outgoing packets are sent.
    @param peer peer to request a disconnection
    @param data data describing the disconnection
    @remarks An ENET_EVENT_DISCONNECT event will be generated by enet_host_service()
    once the disconnection is complete.
*)
procedure
enet_peer_disconnect_later (peer:ENetPeer_PTR; data:enet_uint32);
begin
	if	(	(peer^.state = ENET_PEER_STATE_CONNECTED)
		or	(peer^.state = ENET_PEER_STATE_DISCONNECT_LATER)
	)
	and
	not	(	enet_list_empty (@ peer^.outgoingReliableCommands)
		and	enet_list_empty (@ peer^.outgoingUnreliableCommands)
		and	enet_list_empty (@ peer^.sentReliableCommands)
	)
	then begin
		peer^.state:=ENET_PEER_STATE_DISCONNECT_LATER;
		peer^.eventData:=data;
	end
	else enet_peer_disconnect (peer ,data)
end;

function
enet_peer_queue_acknowledgement (peer:ENetPeer_PTR;{const}command:ENetProtocol_PTR; sentTime:enet_uint16):
ENetAcknowledgement_PTR;
var
	acknowledgement:^ENetAcknowledgement;
	channel:^ENetChannel;
	reliableWindow:enet_uint16;
	currentWindow:enet_uint16;
begin
	if(command^.header.channelID < peer^.channelCount)
	then begin
		channel:=@ peer^.channels [command^.header.channelID];
		reliableWindow:=command^.header.reliableSequenceNumber div ENET_PEER_RELIABLE_WINDOW_SIZE;
		currentWindow:=channel^.incomingReliableSequenceNumber div ENET_PEER_RELIABLE_WINDOW_SIZE;

		if(command^.header.reliableSequenceNumber < channel^.incomingReliableSequenceNumber)
		then reliableWindow += ENET_PEER_RELIABLE_WINDOWS
		;
		if	(reliableWindow >= (currentWindow + pred(ENET_PEER_FREE_RELIABLE_WINDOWS)))
		and	(reliableWindow <= (currentWindow + ENET_PEER_FREE_RELIABLE_WINDOWS))
		then return(NIL)
	end;

	acknowledgement:=ENetAcknowledgement_PTR(enet_malloc (sizeof (ENetAcknowledgement)));

	if(acknowledgement = NIL)
	then return(NIL)
	;
	peer^.outgoingDataTotal += sizeof (ENetProtocolAcknowledge);

	acknowledgement^.sentTime:=sentTime;
	acknowledgement^.command:=command^;

	enet_list_insert (enet_list_end (@ peer^.acknowledgements) ,acknowledgement);

	return(acknowledgement);
end;

procedure
enet_peer_setup_outgoing_command (peer:ENetPeer_PTR; outgoingCommand:ENetOutgoingCommand_PTR);
var
	channel:^ENetChannel;
begin
	channel:=@ peer^.channels [outgoingCommand^.command.header.channelID];

	peer^.outgoingDataTotal += enet_protocol_command_size (outgoingCommand^.command.header.command) + outgoingCommand^.fragmentLength;

	if		(outgoingCommand^.command.header.channelID = $FF)
	then begin
		peer^.outgoingReliableSequenceNumber+=1;

		outgoingCommand^.reliableSequenceNumber:=peer^.outgoingReliableSequenceNumber;
		outgoingCommand^.unreliableSequenceNumber:=0;
	end
	else if	(outgoingCommand^.command.header.command and ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE) <> 0
	then begin
		channel^.outgoingReliableSequenceNumber+=1;
		channel^.outgoingUnreliableSequenceNumber:=0;

		outgoingCommand^.reliableSequenceNumber:=channel^.outgoingReliableSequenceNumber;
		outgoingCommand^.unreliableSequenceNumber:=0;
	end
	else if	(outgoingCommand^.command.header.command and ENET_PROTOCOL_COMMAND_FLAG_UNSEQUENCED) <> 0
	then begin
		peer^.outgoingUnsequencedGroup+=1;

		outgoingCommand^.reliableSequenceNumber:=0;
		outgoingCommand^.unreliableSequenceNumber:=0;
	end
	else begin
		if(outgoingCommand^.fragmentOffset = 0)
		then channel^.outgoingUnreliableSequenceNumber+=1
		;
		outgoingCommand^.reliableSequenceNumber:=channel^.outgoingReliableSequenceNumber;
		outgoingCommand^.unreliableSequenceNumber:=channel^.outgoingUnreliableSequenceNumber;
	end;

	outgoingCommand^.sendAttempts:=0;
	outgoingCommand^.sentTime:=0;
	outgoingCommand^.roundTripTimeout:=0;
	outgoingCommand^.roundTripTimeoutLimit:=0;
	outgoingCommand^.command.header.reliableSequenceNumber:=ENET_HOST_TO_NET_16 (outgoingCommand^.reliableSequenceNumber);

	case(outgoingCommand^.command.header.command and ENET_PROTOCOL_COMMAND_MASK)
	of	ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE:begin
			outgoingCommand^.command.sendUnreliable.unreliableSequenceNumber:=ENET_HOST_TO_NET_16 (outgoingCommand^.unreliableSequenceNumber);
		end
	;	ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED:begin
			outgoingCommand^.command.sendUnsequenced.unsequencedGroup:=ENET_HOST_TO_NET_16 (peer^.outgoingUnsequencedGroup);
		end
	end;

	if(outgoingCommand^.command.header.command and ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE) <> 0
	then enet_list_insert (enet_list_end (@ peer^.outgoingReliableCommands) ,outgoingCommand)
	else enet_list_insert (enet_list_end (@ peer^.outgoingUnreliableCommands) ,outgoingCommand)
end;

function
enet_peer_queue_outgoing_command (peer:ENetPeer_PTR;{const}command:ENetProtocol_PTR; packet:ENetPacket_PTR; offset:enet_uint32; length:enet_uint16):
ENetOutgoingCommand_PTR;
var
	outgoingCommand:ENetOutgoingCommand_PTR;
begin
	outgoingCommand:=ENetOutgoingCommand_PTR(enet_malloc (sizeof (ENetOutgoingCommand)));
	if(outgoingCommand = NIL)
	then return(NIL)
	;
	outgoingCommand^.command:=command^;
	outgoingCommand^.fragmentOffset:=offset;
	outgoingCommand^.fragmentLength:=length;
	outgoingCommand^.packet:=packet;
	if(packet <> NIL)
	then packet^.referenceCount+=1
	;
	enet_peer_setup_outgoing_command (peer ,outgoingCommand);

	return(outgoingCommand);
end;

procedure
enet_peer_dispatch_incoming_unreliable_commands (peer:ENetPeer_PTR; channel:ENetChannel_PTR);
var
	droppedCommand:ENetListIterator;
	startCommand:ENetListIterator;
	currentCommand:ENetListIterator;
	incomingCommand:^ENetIncomingCommand;
	reliableWindow:enet_uint16;
	currentWindow:enet_uint16;
begin
	currentCommand:=enet_list_begin (@ channel^.incomingUnreliableCommands);startCommand:=currentCommand;droppedCommand:=currentCommand;while
	currentCommand<>enet_list_end (@ channel^.incomingUnreliableCommands){$define continue_:=begin;
	currentCommand:=enet_list_next (currentCommand);continue;end}
	do begin
		incomingCommand:=ENetIncomingCommand_PTR(currentCommand);

		if(incomingCommand^.command.header.command and ENET_PROTOCOL_COMMAND_MASK) = ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED
		then continue_
		;
		if(incomingCommand^.reliableSequenceNumber = channel^.incomingReliableSequenceNumber)
		then begin
			if(incomingCommand^.fragmentsRemaining <= 0)
			then begin
				channel^.incomingUnreliableSequenceNumber:=incomingCommand^.unreliableSequenceNumber;
				continue_
			end;

			if		(startCommand <> currentCommand)
			then begin
				enet_list_move (enet_list_end (@ peer^.dispatchedCommands) ,startCommand ,enet_list_previous (currentCommand));

				if(peer^.needsDispatch = 0)
				then begin
					enet_list_insert (enet_list_end (@ peer^.host^.dispatchQueue) ,@ peer^.dispatchList);

					peer^.needsDispatch:=1;
				end;

				droppedCommand:=currentCommand;
			end
			else if	(droppedCommand <> currentCommand)
			then droppedCommand:=enet_list_previous (currentCommand)
		end
		else begin
			reliableWindow:=incomingCommand^.reliableSequenceNumber div ENET_PEER_RELIABLE_WINDOW_SIZE;
			currentWindow:=channel^.incomingReliableSequenceNumber div ENET_PEER_RELIABLE_WINDOW_SIZE;
			if(incomingCommand^.reliableSequenceNumber < channel^.incomingReliableSequenceNumber)
			then reliableWindow += ENET_PEER_RELIABLE_WINDOWS
			;
			if	(reliableWindow >= currentWindow)
			and	(reliableWindow < (currentWindow + pred(ENET_PEER_FREE_RELIABLE_WINDOWS)))
			then break
			;
			droppedCommand:=enet_list_next (currentCommand);

			if(startCommand <> currentCommand)
			then begin
				enet_list_move (enet_list_end (@ peer^.dispatchedCommands) ,startCommand ,enet_list_previous (currentCommand));

				if(peer^.needsDispatch = 0)
				then begin
					enet_list_insert (enet_list_end (@ peer^.host^.dispatchQueue) ,@ peer^.dispatchList);

					peer^.needsDispatch:=1;
				end;
			end;
		end;

		startCommand:=enet_list_next (currentCommand)
		;
	continue_{$undef continue_}end
	;

	if(startCommand <> currentCommand)
	then begin
		enet_list_move (enet_list_end (@ peer^.dispatchedCommands) ,startCommand ,enet_list_previous (currentCommand));

		if(peer^.needsDispatch = 0)
		then begin
			enet_list_insert (enet_list_end (@ peer^.host^.dispatchQueue) ,@ peer^.dispatchList);

			peer^.needsDispatch:=1;
		end;

		droppedCommand:=currentCommand;
	end;

	enet_peer_remove_incoming_commands
(@	channel^.incomingUnreliableCommands
,	enet_list_begin (@ channel^.incomingUnreliableCommands)
,	droppedCommand
)
end;

procedure
enet_peer_dispatch_incoming_reliable_commands (peer:ENetPeer_PTR; channel:ENetChannel_PTR);
var
	currentCommand:ENetListIterator;
	incomingCommand:^ENetIncomingCommand;
begin
	currentCommand:=enet_list_begin (@ channel^.incomingReliableCommands);while
	currentCommand<>enet_list_end (@ channel^.incomingReliableCommands)
	do begin
		incomingCommand:=ENetIncomingCommand_PTR(currentCommand);

		if	(incomingCommand^.fragmentsRemaining > 0)
		or	(incomingCommand^.reliableSequenceNumber <> enet_uint16(succ(channel^.incomingReliableSequenceNumber)))
		then break
		;
		channel^.incomingReliableSequenceNumber:=incomingCommand^.reliableSequenceNumber;

		if(incomingCommand^.fragmentCount > 0)
		then channel^.incomingReliableSequenceNumber += pred(incomingCommand^.fragmentCount)
		;
	currentCommand:=enet_list_next(currentCommand)end
	;
	if(currentCommand = enet_list_begin (@ channel^.incomingReliableCommands))
	then return
	;
	channel^.incomingUnreliableSequenceNumber:=0;

	enet_list_move
(	enet_list_end (@ peer^.dispatchedCommands)
,	enet_list_begin (@ channel^.incomingReliableCommands)
,	enet_list_previous (currentCommand)
)	;

	if(peer^.needsDispatch = 0)
	then begin
		enet_list_insert (enet_list_end (@ peer^.host^.dispatchQueue) ,@ peer^.dispatchList);

		peer^.needsDispatch:=1;
	end;

	if not enet_list_empty (@ channel^.incomingUnreliableCommands)
	then enet_peer_dispatch_incoming_unreliable_commands (peer ,channel)
end;

function
enet_peer_queue_incoming_command (peer:ENetPeer_PTR;{const}command:ENetProtocol_PTR;{const}data:pointer; dataLength:size_t; flags:enet_uint32; fragmentCount:enet_uint32):
ENetIncomingCommand_PTR;
const{static}dummyCommand:ENetIncomingCommand=
(	incomingCommandList:
	(	next:NIL
	;	previous:NIL
	)
;	reliableSequenceNumber:$0000
;	unreliableSequenceNumber:$0000
;	command:
	(	header:
		(	command:$00
		;	channelID:$00
		;	reliableSequenceNumber:$0000
		)
	)
;	fragmentCount:$00000000
;	fragmentsRemaining:$00000000
;	fragments:NIL
;	packet:NIL
)	;
label discardCommand,notifyError;
var
	channel:^ENetChannel;
	unreliableSequenceNumber:enet_uint32=0;
	reliableSequenceNumber:enet_uint32=0;
	reliableWindow:enet_uint16;
	currentWindow:enet_uint16;
	incomingCommand:^ENetIncomingCommand;
	currentCommand:ENetListIterator;
	packet:ENetPacket_PTR=NIL;
begin
	channel:=@ peer^.channels[command^.header.channelID];

	if(peer^.state = ENET_PEER_STATE_DISCONNECT_LATER)
	then goto discardCommand
	;
 	if(command^.header.command and ENET_PROTOCOL_COMMAND_MASK) <> ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED
	then begin
		reliableSequenceNumber:=command^.header.reliableSequenceNumber;
		reliableWindow:=reliableSequenceNumber div ENET_PEER_RELIABLE_WINDOW_SIZE;
		currentWindow:=channel^.incomingReliableSequenceNumber div ENET_PEER_RELIABLE_WINDOW_SIZE;

		if(reliableSequenceNumber < channel^.incomingReliableSequenceNumber)
		then reliableWindow += ENET_PEER_RELIABLE_WINDOWS
		;
		if	(reliableWindow < currentWindow)
		or	(reliableWindow >= (currentWindow + pred(ENET_PEER_FREE_RELIABLE_WINDOWS)))
		then goto discardCommand
	end;

	case(command^.header.command and ENET_PROTOCOL_COMMAND_MASK)
	of	ENET_PROTOCOL_COMMAND_SEND_FRAGMENT
	,	ENET_PROTOCOL_COMMAND_SEND_RELIABLE:begin
			if(reliableSequenceNumber = channel^.incomingReliableSequenceNumber)
			then goto discardCommand
			;
			currentCommand:=enet_list_previous (enet_list_end (@ channel^.incomingReliableCommands));while
			currentCommand<>enet_list_end (@ channel^.incomingReliableCommands){$define continue_:=begin;
			currentCommand:=enet_list_previous (currentCommand);continue;end}
			do begin
				incomingCommand:=ENetIncomingCommand_PTR(currentCommand);

				if		(reliableSequenceNumber >= channel^.incomingReliableSequenceNumber)
				then begin
					if(incomingCommand^.reliableSequenceNumber < channel^.incomingReliableSequenceNumber)
					then continue_
				end
				else if	(incomingCommand^.reliableSequenceNumber >= channel^.incomingReliableSequenceNumber)
				then break
				;
				if(incomingCommand^.reliableSequenceNumber <= reliableSequenceNumber)
				then begin
					if(incomingCommand^.reliableSequenceNumber < reliableSequenceNumber)
					then break
					;
					goto discardCommand;
				end;
			continue_{$undef continue_}end
		end
	;	ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE
	,	ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE_FRAGMENT:begin
			unreliableSequenceNumber:=ENET_NET_TO_HOST_16 (command^.sendUnreliable.unreliableSequenceNumber);

			if	(reliableSequenceNumber = channel^.incomingReliableSequenceNumber)
			and	(unreliableSequenceNumber <= channel^.incomingUnreliableSequenceNumber)
			then goto discardCommand
			;
			currentCommand:=enet_list_previous (enet_list_end (@ channel^.incomingUnreliableCommands));while
			currentCommand<>enet_list_end (@ channel^.incomingUnreliableCommands){$define continue_:=begin;
			currentCommand:=enet_list_previous (currentCommand);continue;end}
			do begin
				incomingCommand:=ENetIncomingCommand_PTR(currentCommand);

				if		(command^.header.command and ENET_PROTOCOL_COMMAND_MASK) = ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED
				then continue_
				;
				if(reliableSequenceNumber >= channel^.incomingReliableSequenceNumber)
				then begin
					if(incomingCommand^.reliableSequenceNumber < channel^.incomingReliableSequenceNumber)
					then continue_
				end
				else if	(incomingCommand^.reliableSequenceNumber >= channel^.incomingReliableSequenceNumber)
				then break
				;
				if(incomingCommand^.reliableSequenceNumber < reliableSequenceNumber)
				then break
				;
				if(incomingCommand^.reliableSequenceNumber > reliableSequenceNumber)
				then continue_
				;
				if(incomingCommand^.unreliableSequenceNumber <= unreliableSequenceNumber)
				then begin
					if(incomingCommand^.unreliableSequenceNumber < unreliableSequenceNumber)
					then break
					;
					goto discardCommand;
				end;
			continue_{$undef continue_}end
		end
	;	ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED:begin
			currentCommand:=enet_list_end (@ channel^.incomingUnreliableCommands);
		end
	else
		goto discardCommand;
	end;

	if(peer^.totalWaitingData >= peer^.host^.maximumWaitingData)
	then goto notifyError
	;
	packet:=enet_packet_create (data ,dataLength ,flags);
	if(packet = NIL)
	then goto notifyError
	;
	incomingCommand:=ENetIncomingCommand_PTR(enet_malloc (sizeof (ENetIncomingCommand)));
	if(incomingCommand = NIL)
	then goto notifyError
	;
	incomingCommand^.reliableSequenceNumber:=command^.header.reliableSequenceNumber;
	incomingCommand^.unreliableSequenceNumber:=unreliableSequenceNumber and $FFFF;
	incomingCommand^.command:=command^;
	incomingCommand^.fragmentCount:=fragmentCount;
	incomingCommand^.fragmentsRemaining:=fragmentCount;
	incomingCommand^.packet:=packet;
	incomingCommand^.fragments:=NIL;

	if(fragmentCount > 0)
	then begin
		if(fragmentCount <= ENET_PROTOCOL_MAXIMUM_FRAGMENT_COUNT)
		then incomingCommand^.fragments:=enet_uint32_ptr(enet_malloc ((fragmentCount + 31) div 32*sizeof (enet_uint32)))
		;
		if(incomingCommand^.fragments = NIL)
		then begin
			enet_free (incomingCommand);

			goto notifyError;
		end;
		fillChar(incomingCommand^.fragments^ ,(fragmentCount + 31) div 32*sizeof (enet_uint32) ,0);
	end;

	if(packet <> NIL)
	then begin
		packet^.referenceCount+=1;

		peer^.totalWaitingData += packet^.dataLength;
	end;

	enet_list_insert (enet_list_next (currentCommand) ,incomingCommand);

	case(command^.header.command and ENET_PROTOCOL_COMMAND_MASK)
	of	ENET_PROTOCOL_COMMAND_SEND_FRAGMENT
	,	ENET_PROTOCOL_COMMAND_SEND_RELIABLE:begin
			enet_peer_dispatch_incoming_reliable_commands (peer ,channel);
		end
	else
		enet_peer_dispatch_incoming_unreliable_commands (peer ,channel);
	end;

	return(incomingCommand);

discardCommand:
	if(fragmentCount > 0)
	then goto notifyError
	;
	if(packet <> NIL) and (packet^.referenceCount = 0)
	then enet_packet_destroy (packet)
	;
	return(@ dummyCommand);

notifyError:
	if(packet <> NIL) and (packet^.referenceCount = 0)
	then enet_packet_destroy (packet)
	;
	return(NIL);
end;

(** @ *)
