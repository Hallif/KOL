(**
 @file callbacks.c
 @brief ENet callback functions
*)
{.$define ENET_BUILDING_LIB 1}
{$include'enet/enet_h.inc'}

const callbacks:ENetCallbacks = (malloc:@ malloc; free:@ free; no_memory:@ abort);

function
enet_initialize_with_callbacks (version:ENetVersion;{const}inits:ENetCallbacks_PTR):
int;
ENET_API
begin
{$define major:=1}
{$define minor:=3}
{$define patch:=0}
	if(version < ENET_VERSION_CREATE{$undef major 1}{$undef minor 3}{$undef patch 0})
	then return(-1)
	;
	if	assigned(inits^.malloc)
	or	assigned(inits^.free)
	then begin
		if	not assigned(inits^.malloc)
		or	not assigned(inits^.free)
		then return(-1)
		;
		callbacks.malloc:=inits^.malloc;
		callbacks.free:=inits^.free;
	end;

	if assigned(inits^.no_memory)
	then callbacks.no_memory:=inits^.no_memory
	;
	return(enet_initialize ());
end;

function
enet_linked_version ({void}):
ENetVersion;
ENET_API
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

