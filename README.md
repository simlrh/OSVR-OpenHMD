# OSVR-OpenHMD Plugin

This is an OSVR plugin providing Oculus Rift DK1 & DK2 orientation tracking via OpenHMD. OpenHMD and hidapi are embedded so you don't need those libraries installed.

    git clone https://github.com/simlrh/OSVR-OpenHMD
    cd OSVR-OpenHMD
    git submodule init
    git submodule update

Then follow [the standard OSVR plugin build instructions](http://resource.osvr.com/docs/OSVR-Core/TopicWritingDevicePlugin.html).

Tested on Windows, Linux and Mac. 64 bit binaries available on the releases page.
