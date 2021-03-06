(**
 @file  types.h
 @brief type definitions for ENet
*)
{$ifndef __ENET_TYPES_H__}
{$define __ENET_TYPES_H__}

typedef enet_uint8_ptr_ptr = ppByte;enet_uint8_ptr = pByte;enet_uint8 = byte;     (**< unsigned 8-bit type  *)
typedef enet_uint16_ptr = pWord;enet_uint16 = word;                               (**< unsigned 16-bit type *)
typedef enet_uint32_ptr = pLongWord;enet_uint32 = longWord;                       (**< unsigned 32-bit type *)

{$endif~__ENET_TYPES_H__}

