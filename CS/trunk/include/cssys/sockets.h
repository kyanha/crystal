/*
    Copyright (C) 1998 by Matze Braun

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef __CS_CSSYS_SOCKETS_H__
#define __CS_CSSYS_SOCKETS_H__

/** \file
 *      For TCP/IP sockets definitions.  Specifically, should define the
 *      following macros, constants, typedefs, and prototypes:
 *	inet_addr(), gethostbyname(), ntohl(), etc.
 *	socket(), listen(), bind(), etc. -- the standard socket functions
 *	csNetworkSocket -- typedef or macro for socket descriptor type
 *	struct sockaddr -- standard socket address type (and cousins)
 *	socklen_t -- typedef or macro
 *	CS_NET_SOCKET_INVALID -- value representing invalid socket
 *	CS_CLOSESOCKET -- name of function to close a socket
 *	CS_IOCTLSOCKET -- name of "ioctl" function for sockets
 *	CS_GETSOCKETERROR -- name of function or variable for socket error code
 */ 

#include <sys/types.h>

#if defined(OS_WIN32)
#  include <winsock.h>
#  ifndef socklen_t
     typedef int socklen_t;
#  endif
#  define CS_NET_SOCKET_INVALID INVALID_SOCKET
#  define CS_IOCTLSOCKET ioctlsocket
#  define CS_CLOSESOCKET closesocket
#  define EWOULDBLOCK WSAEWOULDBLOCK
#  define CS_GETSOCKETERROR ::WSAGetLastError()
#  undef CS_SYSDEF_PROVIDE_SOCKETS
#elif defined (OS_UNIX)
#  include <sys/socket.h>
#  include <unistd.h>
#  define BSD_COMP 1
#  include <sys/ioctl.h>
#  if !defined (OS_SOLARIS) && !defined (OS_BE)
#    include <arpa/inet.h>
#    include <sys/time.h>
#  endif
#  include <netinet/in.h>
#  include <netdb.h>
#endif
#if !defined (CS_IOCTLSOCKET)
#  define CS_IOCTLSOCKET ioctl
#endif
#if !defined (CS_CLOSESOCKET)
#  define CS_CLOSESOCKET close
#endif
#if !defined (CS_GETSOCKETERROR)
#  define CS_GETSOCKETERROR errno
#endif

typedef unsigned int csNetworkSocket;

#if !defined (CS_NET_SOCKET_INVALID)
#  define CS_NET_SOCKET_INVALID ((csNetworkSocket)~0)
#endif

#endif // __CS_CSSYS_SOCKETS_H__

