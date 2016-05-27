/*
  JTAG enable/disable
  Copyright (c) 2016 Ronald S,. Sutherland

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "jtag.h"

void disable_jtag(void)
{
    __asm__ __volatile__ ( \ 
			 "in __tmp_reg__,__SREG__" "\n\t" \ 
			 "cli" "\n\t"				\
			 "out %1, %0" "\n\t"			\
			 "out __SREG__, __tmp_reg__" "\n\t"\ 
			 "out %1, %0" "\n\t"		\
			 : /* no outputs */		\
			 : "r" ((uint8_t)(1<<JTD)), \ 
			   "M" (_SFR_IO_ADDR(MCUCR)) \ 
			 : "r0");		\
}
  
void enable_jtag(void)
{
    __asm__ __volatile__ ( \ 
			 "in __tmp_reg__,__SREG__" "\n\t" \ 
			 "cli" "\n\t" \ 
			 "out %1, %0" "\n\t" \ 
			 "out __SREG__, __tmp_reg__" "\n\t"\ 
			 "out %1, %0" "\n\t" \ 
			 : /* no outputs */ \ 
			 : "r" ((uint8_t)(0)), \ 
			   "M" (_SFR_IO_ADDR(MCUCR)) \ 
			 : "r0");
}

