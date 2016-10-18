(**
 @file  list.h
 @brief ENet list management 
*)
{$ifndef __ENET_LIST_H__}
{$define __ENET_LIST_H__}

{.$include <stdlib.h>}

type
	ENetListIterator = ^ENetListNode;
	ENetListNode = record
		next:ENetListIterator;
		previous:ENetListIterator;
     end;


typedef ENetList_PTR = ^ENetList;ENetList = struct
		sentinel:ENetListNode;
	end;

procedure enet_list_clear (list:ENetList_PTR);

function enet_list_insert (position:ENetListIterator; data:pointer):ENetListIterator;
function enet_list_remove (position:ENetListIterator):pointer;
function enet_list_move (position:ENetListIterator;dataFirst,dataLast:pointer):ENetListIterator;

function enet_list_size (list:ENetList_PTR):size_t;

{$define enet_list_begin:=ENET_LIST_BEGIN_INLINE}
{$define enet_list_end:=ENET_LIST_END_INLINE}

{$define enet_list_empty:=ENET_LIST_EMPTY_INLINE}

{$define enet_list_next:=ENET_LIST_NEXT_INLINE}
{$define enet_list_previous:=ENET_LIST_PREVIOUS_INLINE}

{$define enet_list_front:=ENET_LIST_FRONT_INLINE}
{$define enet_list_back:=ENET_LIST_BACK_INLINE}

{$endif~__ENET_LIST_H__}

