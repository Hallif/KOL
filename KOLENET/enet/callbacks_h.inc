(**
 @file  callbacks.h
 @brief ENet callbacks
*)
{$ifndef __ENET_CALLBACKS_H__}
{$define __ENET_CALLBACKS_H__}

{.$include <stdlib.h>}

typedef ENetCallbacks_PTR = ^ENetCallbacks;ENetCallbacks = struct
		malloc:function(size:size_t):pointer; ENET_CALLBACK;
		free:procedure(memory:pointer); ENET_CALLBACK;
		no_memory:procedure(); ENET_CALLBACK;
	end;

(** @defgroup callbacks ENet internal callbacks
    @//{
    @ingroup private
*)

(** @ *)

{$endif~__ENET_CALLBACKS_H__}

