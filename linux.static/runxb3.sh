#!/bin/bash


## XXX this file is really hacky... primarily to get the DCWAPD running on XB3
## to run this thing: 
## $ cd /tmp && (curl http://10.0.0.5:1234/dcwapdrun.tar | tar xf -) && ./runxb3.sh


if [ ! -e /tmp/.xb3dcwapdsetup ]; then
  DATAINTFVLAN=1111
  swctl -c 16 -p 0 -v $DATAINTFVLAN -m 2 -q 1
  swctl -c 16 -p 2 -v $DATAINTFVLAN -m 2 -q 1
  swctl -c 16 -p 7 -v $DATAINTFVLAN -m 2 -q 1
  vconfig add l2sd0 $DATAINTFVLAN
  brctl addbr brdc0
  brctl addif brdc0 l2sd0.$DATAINTFVLAN
  ifconfig l2sd0.$DATAINTFVLAN 0.0.0.0
  ifconfig brdc0 0.0.0.0

  iptables -t nat -F
  iptables -t nat -X
  iptables -t mangle -F
  iptables -t mangle -X
  iptables -t filter -F
  iptables -t filter -X
  iptables -t nat -A POSTROUTING -j MASQUERADE -o erouter0
  iptables -t nat -A POSTROUTING -j MASQUERADE -o wan0
  iptables -P INPUT ACCEPT

  cat - <<END
  ##
  ##
  ## You need to run this on the ATOM core:
  ##
  ##

  DATAINTF=ath1
  DATAINTFVLAN=$DATAINTFVLAN
  vconfig add eth0 \$DATAINTFVLAN
  ifconfig eth0.\$DATAINTFVLAN 0.0.0.0
  iwpriv \$DATAINTF vlanID \$DATAINTFVLAN

  ##
  ##
  ##
  ##
  ##
  ##

END

  touch /tmp/.xb3dcwapdsetup
fi

if [ ! -d /sys/module ]; then
  mount -t sysfs sysfs /sys
fi

/sbin/lsmod 2>&1 | grep macremapper > /dev/null 2>&1
if [ $? != 0 ]; then
  /bin/dmesg -c > /dev/null 2>&1

  /sbin/insmod macremapper.ko

  /sbin/lsmod 2>&1 | grep macremapper > /dev/null 2>&1
  if [ $? != 0 ]; then
    echo "Failed to insert the driver"
    exit 1
  fi
fi

exec ./dcwapd


