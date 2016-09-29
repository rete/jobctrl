# procctrl 
# A distributed remote process controller
# Copyright Rémi Ete
# See COPYING.txt for GNU licence

## INSTALL:

The install procedure is managed by [CMake](http://cmake.org)

### Dependencies

* MySQL client cpp dev library
* jsoncpp. Only if you want to build the package with DQMViz
* dim

Optionally :

 * Qt. Developed with version 4.7.4
 * Wt.
 * Doxygen

In the root directory :

```bash
mkdir build
cd build
cmake [-DOPTIONS=...] ..
make
```

where OPTIONS can be :
* BUILD_QT_CLIENT [ON/OFF] to install Qt client 
* BUILD_WT_CLIENT [ON/OFF] to install Wt client (web impl)
* INSTALL_DOC [ON/OFF] to install [doxygen](www.doxygen.org) code documentation

All options area by default set to OFF

### Bug report

You can send emails to <remi.ete@gmail.com>
or use the [github issues interface](https://github.com/rete/procctrl/issues)
