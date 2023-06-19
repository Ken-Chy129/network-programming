/**************************
Sample: protocol header definitions used by ping(raw sockets)

Files:
      iphdr.h

Description:
      This file contains various protocol header definitions used by
      the raw socket ping sample.

Compile:
      see ping.cpp

Usage:
      See ping.cpp
 **************************/

#include <pshpack1.h>

typedef struct ip_hdr
{
  unsigned char ip_verlen;
  unsigned char ip_tos;
  unsigned short ip_totallength;
  unsigned short ip_id;
  unsigned short ip_offset;
  unsigned char ip_ttl;
  unsigned char ip_protocol;
  unsigned short ip_checksum;
  unsigned int ip_srcaddr;
  unsigned int ip_dstaddr;
}IPV4_HDR, *PIPV4_HDR, FAR *LPIPV4_HDR;

//ipv4 option header
typedef struct ipv4_option_hdr
{
  unsigned char opt_code;
  unsigned char opt_len;
  unsigned char opt_ptr;
  unsigned long opt_addr[9];
}IPV4_OPTION_HDR, *PIPV4_OPTION_HDR, FAR *LPIPV4_OPTION_HDR;

//icmp header
typedef struct icmp_hdr
{
  unsigned char icmp_type;
  unsigned char icmp_code;
  unsigned short icmp_checksum;
  unsigned short icmp_id;
  unsigned short icmp_sequence;
  unsigned long icmp_timestamp;
}ICMP_HDR, *PICMP_HDR, FAR *LPICMP_HDR;

//UDP header
typedef struct udp_hdr
{
  unsigned short src_portno;
  unsigned short dst_portno;
  unsigned short udp_length;
  unsigned short udp_checksum;
}UDP_HDR, *PUDP_HDR;

//ipv4 option for record route
#define IP_RECORD_ROUTE 0X7

//icmp value(used in the socket call and ipv6 header)
#define IPPROTO_ICMP6 58

//icmp types and codes
#define ICMPV4_ECHO_REQUEST_TYPE    8
#define ICMPV4_ECHO_REQUEST_CODE    0
#define ICMPV4_ECHO_REPLY_TYPE      0
#define ICMPV4_ECHO_REPLY_CODE      0
#define ICMPV4_MINIMUM_HEADER       8

#include <poppack.h>

