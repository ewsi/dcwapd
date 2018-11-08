


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>

#include "./macremapper_filter_config.h"
#include "./filter_conf_parser.h"

static void
bitfill(unsigned char * output, int bitson, int size) {
  for (; size > 0; size--, output++, bitson-=8) {
    if (bitson <  1) { (*output) = 0x00; continue; }
    if (bitson >= 8) { (*output) = 0xFF; continue; }
    (*output) = 0xFF;
    (*output) <<= 8 - bitson;
  }
}

static void
and_octets(unsigned char * octets, unsigned char * mask, int count) {
  for (; count > 0; count--, octets++, mask++) {
    (*octets) &= (*mask);
  }
}

static int
parse_ip_field(struct mrm_ipaddr_filter * output, char * const field, int * const enforce_family) {

  char * delim;
  char delim_type;
  void *endoutput;
  int netmaskbits;

  /* is this an easy match any ? */
  if (strcmp(field, "*") == 0) {
    output->match_type = MRMIPFILT_MATCHANY;
    return 1; /* success */
  }

  /* do we have a delimiter indicating a range or subnet ?  */
  if ((delim = strchr(field, '-')) != NULL) {
    delim_type = *delim;
    (*delim) = '\0';
    delim++;
  }
  else if ((delim = strchr(field, '/')) != NULL) {
    delim_type = *delim;
    (*delim) = '\0';
    delim++;
  }
  else {
    delim = NULL;
    delim_type = '\0';
  }

  /* try to parse as IPv4 */
  if (inet_pton(AF_INET, field, (delim_type == '-') ? &output->ipaddr4_start : &output->ipaddr4) == 1) {
    /* successfully parsed an IPv4 address... */
    if (!(((*enforce_family) == AF_INET) || ((*enforce_family) == AF_UNSPEC))) {
      /* enforcing a family other than IPv4... */
      fprintf(stderr, "IP address does not match specified protocol family!\n");
      return 0; /* parse failed */
    }
    (*enforce_family) = AF_INET;
  }
  /* try to parse as IPv6 */
  else if (inet_pton(AF_INET6, field, (delim_type == '-') ? &output->ipaddr6_start : &output->ipaddr6) == 1) {
    if (!(((*enforce_family) == AF_INET6) || ((*enforce_family) == AF_UNSPEC))) {
      /* enforcing a family other than IPv6... */
      fprintf(stderr, "IP address does not match specified protocol family!\n");
      return 0; /* parse failed */
    }
    (*enforce_family) = AF_INET6;
  }
  else {
    fprintf(stderr, "Failed to parse IP address: '%s'\n", field);
    return 0; /* parse failed */
  }

  /* do we need to parse more? */
  switch (delim_type) {
  case '-':
    /* have a range... need to parse the end address... */
    output->match_type = MRMIPFILT_MATCHRANGE;
    switch(*enforce_family) {
    case AF_INET:  endoutput = &output->ipaddr4_end; break;
    case AF_INET6: endoutput = &output->ipaddr6_end; break;
    default:
      fprintf(stderr, "A \"shouldn't get here\" parsing error occured! %d\n", __LINE__);
      return 0; /* defensive; shouldnt get here */
    }
    if (inet_pton(*enforce_family, delim, endoutput) != 1) {
      fprintf(stderr, "Failed to parse the range end IP address!\n");
      return 0; /* failed parsing */
    }
    break;
  case '/':
    output->match_type = MRMIPFILT_MATCHSUBNET;
    netmaskbits = atoi(delim);
    switch(*enforce_family) {
    case AF_INET:
      if ((netmaskbits < 0) || (netmaskbits > 32)) {
        fprintf(stderr, "Invalid netmask bits: /%s\n", delim);
        return 0; /* failed parsing */
      }
      bitfill((void*)&output->ipaddr4_mask, netmaskbits, sizeof(output->ipaddr4_mask));
      and_octets((void*)&output->ipaddr4, (void*)&output->ipaddr4_mask, sizeof(output->ipaddr4));
      break;
    case AF_INET6:
      if ((netmaskbits < 0) || (netmaskbits > 128)) {
        fprintf(stderr, "Invalid netmask bits: /%s\n", delim);
        return 0; /* failed parsing */
      }
      bitfill((void*)&output->ipaddr6_mask, netmaskbits, sizeof(output->ipaddr6_mask));
      and_octets((void*)&output->ipaddr6, (void*)&output->ipaddr6_mask, sizeof(output->ipaddr6));
      break;
    default:
      fprintf(stderr, "A \"shouldn't get here\" parsing error occured! %d\n", __LINE__);
      return 0; /* defensive; shouldnt get here */
    }
    break;
  default:
    output->match_type = MRMIPFILT_MATCHSINGLE;
    break;
  }
  
  return 1; /* successful parse */
}

static int
parse_port_field(struct mrm_port_filter * output, char * const field) {
  char  *delim;
  int    value;

  if (strcmp(field, "*") == 0) {
    output->match_type = MRMPORTFILT_MATCHANY;
    return 1; /* successful easy parse */
  }

  value = atoi(field);
  if ((value < 0) || (value > 65535)) {
    fprintf(stderr, "Invalid port value: %s\n", field);
    return 0; /* parse failed */
  }

  if ((delim = strchr(field, '-')) != NULL) {
    /* range */
    output->match_type = MRMPORTFILT_MATCHRANGE;
    output->low_portno = (uint16_t)value;
    value = atoi(delim + 1);
    if ((value < output->low_portno) || (value > 65535)) {
      fprintf(stderr, "Invalid port range end value: %s\n", field);
      return 0; /* parse failed */
    }
    output->high_portno = (uint16_t)value;
  }
  else {
    /* single */
    output->match_type = MRMPORTFILT_MATCHSINGLE;
    output->portno = (uint16_t)value;
  }

  return 1; /* successful parse */
}

static int
seperate_field(char ** const field_start, char ** const line_ptr) {
  char *field_end;
  char *bracket_start;
  char *bracket_end;

  if ((*line_ptr) == NULL) return 0; /* parse failed */

  (*field_start) = (*line_ptr);

  field_end     = strchr(*field_start, ':');
  bracket_start = strchr(*field_start, '[');
  bracket_end   = (bracket_start == NULL) ? NULL : strchr(bracket_start, ']');

  if ((bracket_start != NULL) && (bracket_end != NULL)) {
    if ((field_end == NULL) || (field_end > bracket_start)) {
      if (bracket_end > bracket_start) {
        /* all the conditions have been met to "de-bracketify" */
        field_end = strchr(bracket_end, ':');
        (*field_start) = bracket_start + 1;
        (*bracket_end) = '\0';
        (*line_ptr) = field_end;
        if ((*line_ptr) != NULL) (*line_ptr)++;
        return 1; /* successful parse */
      }
    }
  }

  if (field_end != NULL) {
    (*field_end) = '\0';
    field_end++;
  }
  (*line_ptr) = field_end;

  return 1; /* successful parse */
}

static int
filter_file_parse_line(struct mrm_filter_rule * const output, char * line) {
  char *packet_size;
  char *src_ipaddr;
  char *src_port;
  char *proto;
  char *dst_port;

  /* line format: <packet_size>:<src_ip>:<src_port>:<proto>:<dst_port> */

  /* try to parse the fields from the line... */
  if (!seperate_field(&packet_size, &line)) return 0; /* parse failed */
  if (!seperate_field(&src_ipaddr,  &line)) return 0; /* parse failed */
  if (!seperate_field(&src_port,    &line)) return 0; /* parse failed */
  if (!seperate_field(&proto,       &line)) return 0; /* parse failed */
  if (!seperate_field(&dst_port,    &line)) return 0; /* parse failed */

  /* if we get here, line parse was successful...
     now start converting into the filter rule struct */

  /* payload size */
  if (strcmp(packet_size, "*") == 0) {
    output->payload_size = 0;
  }
  else {
    output->payload_size = (unsigned)strtoul(packet_size, NULL, 10);
  }

  /* protocol */
  output->family = AF_UNSPEC;
  if ((strncmp(proto, "*", 1) == 0) || (strncasecmp(proto, "any", 3) == 0)) {
    output->proto.match_type = MRMIPPFILT_MATCHANY;
  }
  else if (strncasecmp(proto, "udp", 3) == 0) {
    output->proto.match_type = MRMIPPFILT_MATCHUDP;
  }
  else if (strncasecmp(proto, "tcp", 3) == 0) {
    output->proto.match_type = MRMIPPFILT_MATCHTCP;
  }
  else {
    fprintf(stderr, "Invalid protocol: '%s'\n", proto);
    return 0; /* failed parse */
  }
  switch(proto[strlen(proto) - 1]) {
  case '4':
     output->proto.match_type |= MRMIPPFILT_MATCHFAMILY;
     output->family = AF_INET;
     break;
  case '6':
     output->proto.match_type |= MRMIPPFILT_MATCHFAMILY;
     output->family = AF_INET6;
     break;
  }

  /* source ip address */
  if (!parse_ip_field(&output->src_ipaddr, src_ipaddr, &output->family)) return 0;

  /* source port */
  if (!parse_port_field(&output->src_port, src_port)) return 0;

  /* destination port */
  if (!parse_port_field(&output->dst_port, dst_port)) return 0;

  return 1; /* success */
}



int
filter_file_load(struct mrm_filter_config * const output, const char *fname) {
  FILE *f = NULL;
  int rv;

  if (!fname)
    return -1;

  f = fopen(fname, "r");
  if (f == NULL) {
    perror("Failed to open configuration file");
    return -1;
  }

  rv = filter_file_loadf(output, f);
  fclose(f);
  return rv;
}

int
filter_file_loadf(struct mrm_filter_config * const output, FILE * const f) {
  int linenum;
  char buf[4096], *pos;

  if (f == NULL)
    return -1;

  output->rules_active = 0;
  bzero(output->rules, sizeof(output->rules)); /* defensive */

  for (linenum = 1; fgets(buf, sizeof(buf), f) != NULL; linenum++) {

    if (buf[0] == '#')
      continue;

    if ((pos = strchr(buf, '\n')) != NULL)
      *pos = '\0';

    if (buf[0] == '\0')
      continue;

    if (output->rules_active >= (sizeof(output->rules) / sizeof(output->rules[0]))) {
       /* XXX cleanup !! */
      /* fprintf(stderr, "Rule count exceeds maximum in file: '%s'\n", fname); */
      fprintf(stderr, "Rule count exceeds maximum\n");
      return -1;
    }
    if (!filter_file_parse_line(&output->rules[output->rules_active], buf)) {
       /* XXX cleanup !! */
      /* fprintf(stderr, "Failed to parse line %d in filter configuration file: '%s'\n", linenum, fname); */
      fprintf(stderr, "Failed to parse line %d in filter configuration file\n", linenum);
      return -1;
    }

    output->rules_active++;
  }

  return 0; /* success */
}


