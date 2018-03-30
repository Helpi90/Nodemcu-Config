/*
 * Vectors funcs in ROM-XTOS
 * Author: PV`
 * (c) PV` 2015
 */

#ifndef __VECTORS_H__
#define __VECTORS_H__

/*
PROVIDE ( _DebugExceptionVector = 0x40000010 );
PROVIDE ( _NMIExceptionVector = 0x40000020 );
PROVIDE ( _KernelExceptionVector = 0x40000030 );
PROVIDE ( _UserExceptionVector = 0x40000050 );
PROVIDE ( _DoubleExceptionVector = 0x40000070 );
#PROVIDE ( _ResetVector = 0x40000080 );
PROVIDE ( _ResetHandler = 0x400000a4 );
*/

void _ResetVector(void);

#endif
