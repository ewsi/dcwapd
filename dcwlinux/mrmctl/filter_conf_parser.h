#ifndef FILTER_CONF_PARSER_H_INCLUDED
#define FILTER_CONF_PARSER_H_INCLUDED

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


struct mrm_filter_config;
int filter_file_load(struct mrm_filter_config * const /* output */, const char * /* fname */);
int filter_file_loadf(struct mrm_filter_config * const /* output */, FILE * const /* f */);

#ifdef __cplusplus
}; //extern "C" {
#endif

#endif /* #ifndef FILTER_CONF_PARSER_H_INCLUDED */
