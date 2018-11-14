
export XPREFIX='/home/jdennis/dcwifi/arrisxb3.recent/build-xb3/./tmp/sysroots/x86_64-linux/usr/bin/arm1176jzstb-rdk-linux-uclibceabi/armeb-rdk-linux-uclibceabi-'

export CC=$XPREFIX"gcc"
export CXX=$XPREFIX"g++"
export CPP=$XPREFIX"cpp"

export XB3BUILD='/home/jdennis/dcwifi/arrisxb3.recent/build-xb3'
export XB3LIBS="$XB3BUILD/tmp/sysroots/arrisxb3arm/usr/lib"

export CXXFLAGS="-I$XB3BUILD/tmp/sysroots/arrisxb3arm/usr/include"
export LIBS="$XB3LIBS/libdcwproto.a $XB3LIBS/libdcwsocket.a $XB3LIBS/libmrmfilterparser.a"
export LDFLAGS="-L/home/jdennis/ccwmove/dcwapd/deplibs"


# ./configure --host=arm --enable-platform=arrisxb3
