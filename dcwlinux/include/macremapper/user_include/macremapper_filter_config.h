#ifndef MACREMAPPER_FILTER_CONFIG_H_INCLUDED
#define MACREMAPPER_FILTER_CONFIG_H_INCLUDED

#ifdef __KERNEL__
  #include <linux/in.h>
  #include <linux/in6.h>
  #include <linux/types.h>
  #include <linux/if.h>
#else
  #include <arpa/inet.h>
  #include <linux/if.h>
#endif

/*

  This file defines all the data structures shared between 
  both kernel and user space functions for the network
  filter data-types

*/

#define MRM_FILTER_MAX_RULES 10
#define MRM_FILTER_NAME_MAX  24


/* filter data types */
struct mrm_ipaddr_filter {
  enum {
    MRMIPFILT_MATCHANY     = 0,
    MRMIPFILT_MATCHSINGLE,
    MRMIPFILT_MATCHSUBNET,
    MRMIPFILT_MATCHRANGE,
  } match_type;

  /* XXX should use system address types for this! */
  union {
    struct in_addr ipaddr4;
    struct in_addr ipaddr4_start;
    struct in6_addr ipaddr6;
    struct in6_addr ipaddr6_start;
  };

  union {
    struct in_addr ipaddr4_mask;
    struct in_addr ipaddr4_end;
    struct in6_addr ipaddr6_mask;
    struct in6_addr ipaddr6_end;
  };
};

struct mrm_ipproto_filter {
  enum {
    /* OR these to form combinations... */
    MRMIPPFILT_MATCHANY     = 0,
    MRMIPPFILT_MATCHFAMILY  = 1,
    MRMIPPFILT_MATCHUDP     = 2,
    MRMIPPFILT_MATCHTCP     = 4,
  } match_type;
};

struct mrm_port_filter {
  enum {
    MRMPORTFILT_MATCHANY     = 0,
    MRMPORTFILT_MATCHSINGLE,
    MRMPORTFILT_MATCHRANGE,
  } match_type;

  union {
    uint16_t portno;
    uint16_t low_portno;
  };
  uint16_t high_portno;
};

struct mrm_filter_rule {
  unsigned payload_size; /* >= this to trigger... 0 = match all */


  /*
    the family field...
    Acceptable values: AF_INET, AF_INET6
    This field is what dictates what type of address is in the "src_ipaddr" field
    This field also dictates the family if "proto.match_type" has "MRMIPPFILT_MATCHFAMILY"
    This field is ignored if both conditions are met:
      . "proto.match_type" does NOT have "MRMIPPFILT_MATCHFAMILY"
      . -- AND --
      . "src_ipaddr.match_type" is "MRMIPFILT_MATCHANY"

  */
  int                        family;
  struct mrm_ipproto_filter  proto;
  struct mrm_ipaddr_filter   src_ipaddr;
  struct mrm_port_filter     src_port;
  struct mrm_port_filter     dst_port;
};

struct mrm_filter_config {
  char                    name[MRM_FILTER_NAME_MAX];
  unsigned                rules_active; /* count of active rules... cant be > MRM_FILTER_MAX_RULES */
  struct mrm_filter_rule  rules[MRM_FILTER_MAX_RULES];
};


/* remap data types */
struct mrm_remap_entry {
  unsigned char   match_macaddr[6];
  unsigned char   replace_macaddr[6];
  char            replace_ifname[IFNAMSIZ];
  char            filter_name[MRM_FILTER_NAME_MAX];
};

#endif /* #ifndef MACREMAPPER_FILTER_CONFIG_H_INCLUDED */
