////////////////////////////////////////////////////////
//
// first, a bunch of stuff that needs to be defined here
// if nested classes worked right on pixelflow aCC, then
// this could probably be cleaned up some
//

#ifndef VRPN_CONNECTIONOLDCOMMONSTUFF_H
#define VRPN_CONNECTIONOLDCOMMONSTUFF_H

#include "vrpn_ConnectionCommonStuff.h"
#include "vrpn_Shared.h"

class vrpn_File_Connection;  // for get_File_Connection()

// NOTE: most users will want to use the vrpn_Synchronized_Connection
// class rather than the default connection class (either will work,
// a regular connection just has 0 as the client-server time offset)

typedef vrpn_int32 (* vrpn_LOGFILTER) (void * userdata, vrpn_HANDLERPARAM p);

// Buffer lengths for TCP and UDP.
// TCP is an arbitrary number that can be changed by the user
// using vrpn_Connection::set_tcp_outbuf_size().
// UDP is set based on Ethernet maximum transmission size;  trying
// to send a message via UDP which is longer than the MTU of any
// intervening physical network may cause untraceable failures,
// so for now we do not expose any way to change the UDP output
// buffer size.  (MTU = 1500 bytes, - 28 bytes of IP+UDP header)

#define vrpn_CONNECTION_TCP_BUFLEN  (64000)
#define vrpn_CONNECTION_UDP_BUFLEN  (1472)

// This is the list of states that a connection can be in
// (possible values for status).
#define LISTEN          (1)
#define CONNECTED       (0)
#define CONNECTION_FAIL     (-1)
#define BROKEN          (-2)
#define DROPPED         (-3)


// different possible statuses
//#define CONNECTED (1)
#define NOT_CONNECTED (0)
//#define ERROR (-1) already defined in some windows lib
	

// System message types
#define vrpn_CONNECTION_SENDER_DESCRIPTION  (-1)
#define vrpn_CONNECTION_TYPE_DESCRIPTION    (-2)
#define vrpn_CONNECTION_UDP_DESCRIPTION     (-3)
#define vrpn_CONNECTION_LOG_DESCRIPTION     (-4)
#define vrpn_CONNECTION_MCAST_DESCRIPTION   (-5)  
#define vrpn_CLIENT_MCAST_CAPABLE_REPLY     (-6)
#define vrpn_CLOCK_QUERY                    (-7)
#define vrpn_CLOCK_REPLY                    (-8)

// What to log
#define vrpn_LOG_NONE               (0)
#define vrpn_LOG_INCOMING           (1<<0)
#define vrpn_LOG_OUTGOING           (1<<1)


#if !defined(_WIN32) || defined (__CYGWIN__)
#define SOCKET int
#endif


// Default port to listen on for a server
#define vrpn_DEFAULT_LISTEN_PORT_NO             (4500)

// If defined, will filter out messages:  if the remote side hasn't
// registered a type, messages of that type won't be sent over the
// link.
//#define vrpn_FILTER_MESSAGES

// Placed here so vrpn_FileConnection can use it too.
struct vrpn_LOGLIST {
  vrpn_HANDLERPARAM data;
  vrpn_LOGLIST * next;
  vrpn_LOGLIST * prev;
};

// HACK
// These structs must be declared outside of vrpn_Connection
// (although we'd like to make them protected/private members)
// because aCC on PixelFlow doesn't handle nested classes correctly.
//   [juliano 7/99: I think this statement is true only if
//                  the nested class is self-referential]

struct vrpnLogFilterEntry {
  vrpn_LOGFILTER filter;   // routine to call
  void * userdata;         // passed along
  vrpnLogFilterEntry * next;
};


//-------------------------------------------------------------------
// UTILITY FUNCTIONS
//-------------------------------------------------------------------

// marshalls a message into the output buffer for a protocol socket
// this got move out of vrpn_Connection and made into a global function
// so that the multicast sender class could have access to it.
vrpn_uint32 vrpn_marshall_message (char * outbuf,vrpn_uint32 outbuf_size,
                       vrpn_uint32 initial_out,
                       vrpn_uint32 len, struct timeval time,
                       vrpn_int32 type, vrpn_int32 sender,
                       const char * buffer);

// 1hz sync connection by default, windowed over last three bounces 
// WARNING:  vrpn_get_connection_by_name() may not be thread safe.
vrpn_ClientConnectionController * vrpn_get_connection_by_name(
	const char * cname,
	const char * local_logfile_name = NULL,
	vrpn_int32 local_log_mode = vrpn_LOG_NONE,
	const char * remote_logfile_name = NULL,
	vrpn_int32 remote_log_mode = vrpn_LOG_NONE,
	vrpn_float64 dFreq = 1.0, 
	vrpn_int32 cSyncWindow = 3);


// Utility routines to parse names (<service>@<location specifier>)
// Both return new char [], and it is the caller's responsibility
// to delete this memory!
char * vrpn_copy_service_name (const char * fullname);
char * vrpn_copy_service_location (const char * fullname);

// Utility routines to parse file specifiers FROM service locations
//   file:<filename>
//   file://<hostname>/<filename>
//   file:///<filename>
char * vrpn_copy_file_name (const char * filespecifier);

// Utility routines to parse host specifiers FROM service locations
//   <hostname>
//   <hostname>:<port number>
//   x-vrpn://<hostname>
//   x-vrpn://<hostname>:<port number>
//   x-vrsh://<hostname>/<server program>,<comma-separated server arguments>
char * vrpn_copy_machine_name (const char * hostspecifier);
vrpn_int32 vrpn_get_port_number (const char * hostspecifier);
char * vrpn_copy_rsh_program (const char * hostspecifier);
char * vrpn_copy_rsh_arguments (const char * hostspecifier);

// Checks the buffer to see if it is a valid VRPN header cookie.
// Returns -1 on total mismatch,
// 1 on minor version mismatch or other acceptable difference,
// and 0 on exact match.
vrpn_int32 check_vrpn_cookie (const char * buffer);

// Returns the size of the magic cookie buffer, plus any alignment overhead.
vrpn_int32 vrpn_cookie_size ();

vrpn_int32 write_vrpn_cookie (
	char * buffer, 
	vrpn_int32 length, 
	vrpn_int32 remote_log_mode);

#endif
