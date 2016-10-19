(**
 @file callbacks.c
 @brief ENet callback functions
*)
{.$define ENET_BUILDING_LIB 1}
{$include'enet/enet_h.inc'}

const callbacks:ENetCallbacks = (malloc:malloc; free:free; no_memory:abort);


function
enet_linked_version ({void}):
ENetVersion;
begin
	return(ENET_VERSION);
end;

function
enet_malloc (size:size_t):
pointer;
var
	memory:pointer;
begin
	memory:=callbacks.malloc (size);

	if(memory = NIL)
	then callbacks.no_memory ()
	else fillChar(memory^ , size ,0)
	;
	return(memory);
end;

procedure
enet_free (memory:pointer);
begin
	callbacks.free (memory);
end;

