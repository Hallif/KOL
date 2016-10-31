(**
 @file list.c
 @brief ENet linked list functions
*)
{.$define ENET_BUILDING_LIB 1}
{$include'enet/enet_h.inc'}

(**
    @defgroup list ENet linked list utility functions
    @ingroup private
    @
*)

function ENET_LIST_BEGIN_INLINE (list:ENetList_PTR):ENetListIterator; INLINE;
begin
	ENET_LIST_BEGIN_INLINE:=list^.sentinel.next;
end;

function ENET_LIST_END_INLINE (list:ENetList_PTR):ENetListIterator; INLINE;
begin
	ENET_LIST_END_INLINE:=@ list^.sentinel;
end;

function ENET_LIST_EMPTY_INLINE (list:ENetList_PTR):boolean; INLINE;
begin
	ENET_LIST_EMPTY_INLINE:=list^.sentinel.next =@ list^.sentinel;
end;

function ENET_LIST_NEXT_INLINE (iterator:ENetListIterator):ENetListIterator; INLINE;
begin
	ENET_LIST_NEXT_INLINE:=iterator^.next;
end;

function  ENET_LIST_PREVIOUS_INLINE (iterator:ENetListIterator):ENetListIterator; INLINE;
begin
	ENET_LIST_PREVIOUS_INLINE:=iterator^.previous;
end;

function ENET_LIST_FRONT_INLINE (list:ENetList_PTR):pointer{ENetListIterator}; INLINE;
begin
	ENET_LIST_FRONT_INLINE:=list^.sentinel.next;
end;

function ENET_LIST_BACK_INLINE (list:ENetList_PTR):pointer{ENetListIterator}; INLINE;
begin
	ENET_LIST_BACK_INLINE:=list^.sentinel.previous;
end;

procedure
enet_list_clear (list:ENetList_PTR);
begin;assert(list<>NIL);
	list^.sentinel.next:=@ list^.sentinel;
	list^.sentinel.previous:=@ list^.sentinel;
end;

function
enet_list_insert (position:ENetListIterator; data:pointer):
ENetListIterator;
var
	result:ENetListIterator;
begin;assert(position<>NIL);
	ENetListIterator(result):=ENetListIterator(data);assert(result<>NIL);

	result^.previous:=position^.previous;
	result^.next:=position;

	result^.previous^.next:=result;
	position^.previous:=result;

	return(result);
end;

function
enet_list_remove (position:ENetListIterator):
pointer;
begin;assert(position<>NIL);
	position^.previous^.next:=position^.next;
	position^.next^.previous:=position^.previous;

	return(position);
end;

function
enet_list_move (position:ENetListIterator; dataFirst,dataLast:pointer):
ENetListIterator;
var
	first:ENetListIterator;
	last:ENetListIterator;
begin;assert(position<>NIL);
	first:=ENetListIterator(dataFirst);assert(first<>NIL);
	last:=ENetListIterator(dataLast);assert(last<>NIL);

	first^.previous^.next:=last^.next;
	last^.next^.previous:=first^.previous;

	first^.previous:=position^.previous;
	last^.next:=position;

	first^.previous^.next:=first;
	position^.previous:=last;

	return(first);
end;

function
enet_list_size (list:ENetList_PTR):
size_t;
var
	position:ENetListIterator;
	size:size_t=0;
begin;assert(list<>NIL);
	position:=enet_list_begin (list);while
	position<>enet_list_end (list){$define continue_:=begin;
	position:=enet_list_next (position);continue;end;}
	do begin
		size+=1
		;
	continue_{$undef continue_}end
	;
	return(size);
end;

(** @ *)
