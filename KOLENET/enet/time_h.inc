(**
 @file  time.h
 @brief ENet time constants and macros
*)
{$ifndef __ENET_TIME_H__}
{$define __ENET_TIME_H__}

{$define ENET_TIME_OVERFLOW:=86400000}

{$define ENET_TIME_LESS:=ENET_TIME_LESS_INLINE}
{$define ENET_TIME_GREATER:=ENET_TIME_GREATER_INLINE}
{$define ENET_TIME_LESS_EQUAL:=ENET_TIME_LESS_EQUAL_INLINE}
{$define ENET_TIME_GREATER_EQUAL:=ENET_TIME_GREATER_EQUAL_INLINE}

{$define ENET_TIME_DIFFERENCE:=ENET_TIME_DIFFERENCE_INLINE}

function ENET_TIME_LESS_INLINE(const a, b):boolean; INLINE;
begin
	ENET_TIME_LESS_INLINE:=(enet_uint32(a) - enet_uint32(b)) >= ENET_TIME_OVERFLOW
end;

function ENET_TIME_GREATER_INLINE(const a, b):boolean; INLINE;
begin
	ENET_TIME_GREATER_INLINE:=(enet_uint32(b) - enet_uint32(a)) >= ENET_TIME_OVERFLOW
end;

function ENET_TIME_LESS_EQUAL_INLINE(const a, b):boolean; INLINE;
begin
	ENET_TIME_LESS_EQUAL_INLINE:=not ENET_TIME_GREATER (a, b)
end;

function ENET_TIME_GREATER_EQUAL_INLINE(const a, b):boolean; INLINE;
begin
	ENET_TIME_GREATER_EQUAL_INLINE:=not ENET_TIME_LESS (a, b)
end;

function ENET_TIME_DIFFERENCE_INLINE(const a, b):enet_uint32;
begin
	if enet_uint32(a) - enet_uint32(b) >= ENET_TIME_OVERFLOW
	then ENET_TIME_DIFFERENCE_INLINE:=enet_uint32(b) - enet_uint32(a)
	else ENET_TIME_DIFFERENCE_INLINE:=enet_uint32(a) - enet_uint32(b)
end;

{$endif~__ENET_TIME_H__}

