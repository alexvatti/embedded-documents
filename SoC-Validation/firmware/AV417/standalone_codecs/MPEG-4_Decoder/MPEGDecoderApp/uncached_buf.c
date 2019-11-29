/*
 * Copyright (c) 2005 ARC International (UK) Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * May 2006
 */

#include "uncached_buf.h"
#include "serial_pl01x.h"
#include <stdlib.h>

void *baseVirtualAddress;
void *basePhysicalAddress;
int baseVirtualSize = 1024*1024;


char *uncached_bitstream_alloc(void)
{
    baseVirtualAddress = malloc(baseVirtualSize);
    if (!baseVirtualAddress)
    {
	print("Failed to allocate bitstream input buffer.");
    }
    return baseVirtualAddress;
}

void uncached_bitstream_free(char *p)
{
    free(p);
    baseVirtualAddress = NULL;
}


