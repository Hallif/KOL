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

(**
 * Portable internet address structure. 
 *
 * The host must be specified in network byte-order, and the port must be in host 
 * byte-order. The constant ENET_HOST_ANY may be used to specify the default 
 * server host. The constant ENET_HOST_BROADCAST may be used to specify the
 * broadcast address (255.255.255.255).  This makes sense for enet_host_connect,
 * but not for enet_host_create.  Once a server responds to a broadcast, the
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

   (** packet must be received by the target peer and resend attempts should be
     * made until the packet is delivered *)

   (** packet will not be sequenced with other packets
     * not supported for reliable packets
     *)

   (** packet will not allocate data, and user must supply it instead *)

   (** packet will be fragmented using unreliable (instead of reliable) sends
     * if it exceeds the MTU *)

   (** whether the packet has been sent from all queues it has been entered into *)

(**
 * ENet packet structure.
 *
 * An ENet data packet that may be sent to or received from a peer. The shown 
 * fields should only be read and never modified. The data field contains the 
 * allocated data for the packet. The dataLength fields specifies the length 
 * of the allocated data.  The flags field is either 0 (specifying no flags), 
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

(**
 * An ENet peer which data packets may be sent or received from. 
 *
 * No fields should be modified unless otherwise specified. 
 *)

(** An ENet packet compressor for compressing UDP packets before socket sends or receives.
 *)

   (** Context data for the compressor. Must be non-NULL. *)

   (** Compresses from inBuffers[0:inBufferCount-1], containing inLimit bytes, to outData, outputting at most outLimit bytes. Should return 0 on failure. *)

   (** Decompresses from inData, containing inLimit bytes, to outData, outputting at most outLimit bytes. Should return 0 on failure. *)

   (** Destroys the context when compression is disabled or the host is destroyed. May be NULL. *)

(** Callback that computes the checksum of the data held in buffers[0:bufferCount-1] *)

(** Callback for intercepting received raw UDP packets. Should return 1 to intercept, 0 to ignore, or -1 to propagate an error. *)

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

(**
 * An ENet event type, as specified in @ref ENetEvent.
 *)

   (** no event occurred within the specified time limit *)

   (** a connection request initiated by enet_host_connect has completed.
     * The peer field contains the peer which successfully connected.
     *)

   (** a peer has disconnected.  This event is generated on a successful
     * completion of a disconnect initiated by enet_peer_disconnect, if
     * a peer has timed out, or if a connection request intialized by
     * enet_host_connect has timed out.  The peer field contains the peer
     * which disconnected. The data field contains user supplied data
     * describing the disconnection, or 0, if none is available.
     *)

   (** a packet has been received from a peer.  The peer field specifies the
     * peer which sent the packet.  The channelID field specifies the channel
     * number upon which the packet was received.  The packet field contains
     * the packet that was received; this packet must be destroyed with
     * enet_packet_destroy after use.
     *)

(**
 * An ENet event as returned by enet_host_service().
   
   @sa enet_host_service
 *)

(** @defgroup global ENet global functions
    @{ 
*)

(**
  Initializes ENet globally.  Must be called prior to using any functions in
  ENet.
  @returns 0 on success, < 0 on failure
*)

(**
  Initializes ENet globally and supplies user-overridden callbacks. Must be called prior to using any functions in ENet. Do not use enet_initialize() if you use this variant. Make sure the ENetCallbacks structure is zeroed out so that any additional callbacks added in future versions will be properly ignored.

  @param version the constant ENET_VERSION should be supplied so ENet knows which version of ENetCallbacks struct to use
  @param inits user-overridden callbacks where any NULL callbacks will use ENet's defaults
  @returns 0 on success, < 0 on failure
*)

(**
  Shuts down ENet globally.  Should be called when a program that has
  initialized ENet exits.
*)

(**
  Gives the linked version of the ENet library.
  @returns the version number 
*)

(** @} *)

(** @defgroup private ENet private implementation functions *)

(**
  Returns the wall-time in milliseconds.  Its initial value is unspecified
  unless otherwise set.
  *)

(**
  Sets the current wall-time in milliseconds.
  *)

(** @defgroup socket ENet socket functions
    @{
*)

(** @} *)

(** @defgroup Address ENet address functions
    @{
*)
(** Attempts to resolve the host named by the parameter hostName and sets
    the host field in the address parameter if successful.
    @param address destination to store resolved address
    @param hostName host name to lookup
    @retval 0 on success
    @retval < 0 on failure
    @returns the address of the given hostName in address on success
*)

(** Gives the printable form of the IP address specified in the address parameter.
    @param address    address printed
    @param hostName   destination for name, must not be NULL
    @param nameLength maximum length of hostName.
    @returns the null-terminated name of the host in hostName on success
    @retval 0 on success
    @retval < 0 on failure
*)

(** Attempts to do a reverse lookup of the host field in the address parameter.
    @param address    address used for reverse lookup
    @param hostName   destination for name, must not be NULL
    @param nameLength maximum length of hostName.
    @returns the null-terminated name of the host in hostName on success
    @retval 0 on success
    @retval < 0 on failure
*)

(** @} *)

{$ifdef __cplusplus}
//}
{$endif}

{$endif~__ENET_ENET_H__}

