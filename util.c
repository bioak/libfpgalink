/*
 * Copyright (C) 2009-2012 Chris McClelland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <makestuff.h>
#ifdef WIN32
	#include <Windows.h>
#else
	#define _BSD_SOURCE
	#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>

/*
 * Platform-agnostic millisecond sleep function
 */
DLLEXPORT(void) flSleep(uint32 ms) {
	#ifdef WIN32
		Sleep(ms);
	#else
		usleep(1000*ms);
	#endif
}

/*
 * Allocate a buffer big enough to fit file into, then read the file into it, then write the file
 * length to the location pointed to by 'length'. Naturally, responsibility for the allocated
 * buffer passes to the caller, and must be freed by a call to flFreeFile().
 */
DLLEXPORT(uint8*) flLoadFile(const char *name, uint32 *length) {
	FILE *file;
	uint8 *buffer;
	size_t fileLen;
	size_t returnCode;

	file = fopen(name, "rb");
	if ( !file ) {
		return NULL;
	}
	
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate enough space for an extra byte just in case the file size is odd
	buffer = (uint8 *)malloc(fileLen + 1);
	if ( !buffer ) {
		fclose(file);
		return NULL;
	}
	returnCode = fread(buffer, 1, fileLen, file);
	if ( returnCode == fileLen ) {
		if ( fileLen & 1 ) {
			fileLen++;
		}
		*length = (uint32)fileLen;
	}
	fclose(file);
	return buffer;
}

DLLEXPORT(void) flFreeFile(uint8 *buffer) {
	free((void*)buffer);
}
