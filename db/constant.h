#pragma once
#ifndef _CONSTANT_H
#define _CONSTANT_H
/* FindTime is recarding the times using FindCore */
extern int FindTime;
/* BlockSize is define the node size */
extern const int BlockSize;
/* L is define the LeafNode size, which is controlled by the BlockSize */
extern const int L;
/* M is define the MiddleNode size, which is controlled by the BlockSize */
extern const int M;
/* BufferSize is the upper bound to deal with it and clear it */
extern const int BufferSize;
/* CacheSize is the upper bound to deal with it and clear it */
extern const int CacheSize;
#endif
