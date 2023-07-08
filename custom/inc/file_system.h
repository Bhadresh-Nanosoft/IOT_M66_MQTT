/*
 * file_system.h
 *
 *  Created on: May 9, 2019
 *      Author: SW
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#include "ql_type.h"

extern u32 writenLen;
extern s32 position;
extern s32 filesize;
extern s32 filehandle;
extern u32 writeedlen, readedlen;
//extern u32 fs_length;
//extern s32 file_pointer;
extern void file_system_write(u8 *,s32 ,u32);
extern u32 file_system_read(u8* ,s32 ,u32);


#endif /* FILE_SYSTEM_H_ */
