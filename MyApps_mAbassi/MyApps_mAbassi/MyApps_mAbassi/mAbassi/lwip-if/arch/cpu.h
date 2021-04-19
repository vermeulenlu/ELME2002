/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __CPU_H__
#define __CPU_H__

#ifdef BYTE_ORDER
  #undef BYTE_ORDER
#endif

#if defined (__ICCARM__)
  #if ((__LITTLE_ENDIAN__) != 0)
    #define BYTE_ORDER LITTLE_ENDIAN
  #else
    #define BYTE_ORDER BIG_ENDIAN
  #endif

#elif defined (__CC_ARM)
 #if !defined(__BIG_ENDIAN)
    #define BYTE_ORDER LITTLE_ENDIAN
  #else
    #define BYTE_ORDER BIG_ENDIAN
  #endif

#elif defined (__GNUC__)
  #if ((__BYTE_ORDER__) == (__ORDER_LITTLE_ENDIAN__))
    #define BYTE_ORDER LITTLE_ENDIAN
  #else
    #define BYTE_ORDER BIG_ENDIAN
  #endif

#elif defined (__TASKING__)
  #define BYTE_ORDER LITTLE_ENDIAN

#endif

#endif /* __CPU_H__ */