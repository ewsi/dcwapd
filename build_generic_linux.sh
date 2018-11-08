#!/bin/bash

#change to the directory this script is in... (defensive)
cd `dirname "$0"`

#checkout the dependencies needed to build...
rm -Rf deps_dcwapd
mkdir deps_dcwapd
cd deps_dcwapd
DEPDIR="`pwd`"
git clone ssh://git@code.cablelabs.com:7999/dcw/libdcwproto.git
git clone ssh://git@code.cablelabs.com:7999/dcw/libdcwsocket.git
git clone ssh://git@code.cablelabs.com:7999/dcw/macremapper.git
cd -

DEPDIRDEST="$DEPDIR"/install

#build libdcwproto...
cd "$DEPDIR/libdcwproto" && ./configure && make && make install DESTDIR="$DEPDIRDEST"

#build libdcwsocket...
cd "$DEPDIR/libdcwsocket" && ./configure && make && make install DESTDIR="$DEPDIRDEST"

#build macremapper userland stuff...
cd "$DEPDIR/macremapper/userland" && ./configure && make && make install DESTDIR="$DEPDIRDEST"


#configure ourself...
autoreconf --install
export CXXFLAGS="-I$DEPDIRDEST/usr/local/include"
export LIBS="$DEPDIRDEST/usr/local/lib/libdcwproto.a $DEPDIRDEST/usr/local/lib/libdcwsocket.a $DEPDIRDEST/usr/local/lib/libmrmfilterparser.a"
./configure --enable-platform=linuxjsonstatic
make -j
make install DESTDIR="$DEPDIRDEST"

#thats it...
echo ""
echo ""
echo ""
echo ""
echo ""
echo ""
echo ""
echo "The output is here: $DEPDIRDEST/usr/local/bin/dcwapd"




