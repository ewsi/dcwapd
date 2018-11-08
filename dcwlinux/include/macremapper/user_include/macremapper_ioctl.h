#ifndef MACREMAPPER_IOCTL_H_INCLUDED
#define MACREMAPPER_IOCTL_H_INCLUDED

/*

  This file defines all the ioctl() types used for the user
  to kernel calls

*/

#include <asm/ioctl.h>

#include "./macremapper_filter_config.h"

#define MRM_IOCTL_TYPE 77

/* ioctl()s for working with filters... */
#define MRM_GETFILTERCOUNT _IOR  (MRM_IOCTL_TYPE, 0, unsigned)
#define MRM_GETFILTER      _IOWR (MRM_IOCTL_TYPE, 1, struct mrm_filter_config)
#define MRM_SETFILTER      _IOW  (MRM_IOCTL_TYPE, 2, struct mrm_filter_config)
#define MRM_DELETEFILTER   _IOW  (MRM_IOCTL_TYPE, 3, struct mrm_filter_config)

/* ioctl()s for working with MAC address remappings... */
#define MRM_GETREMAPCOUNT  _IOR  (MRM_IOCTL_TYPE, 10, unsigned)
#define MRM_GETREMAP       _IOWR (MRM_IOCTL_TYPE, 11, struct mrm_remap_entry)
#define MRM_SETREMAP       _IOW  (MRM_IOCTL_TYPE, 12, struct mrm_remap_entry)
#define MRM_DELETEREMAP    _IOW  (MRM_IOCTL_TYPE, 13, struct mrm_remap_entry)

/* ioctl() for completely blowing away the running configuration */
#define MRM_WIPERUNCONF    _IO   (MRM_IOCTL_TYPE, 100)


#endif /* #ifndef MACREMAPPER_IOCTL_H_INCLUDED */
