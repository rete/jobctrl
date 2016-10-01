# procctrl 
# A distributed remote process controller
# Copyright Rémi Ete
# See COPYING.txt for GNU licence

## INSTALL:

The install procedure is managed by [CMake](http://cmake.org)

### Dependencies

* MySQL client dev library (libmysqlclient-dev)
* Json cpp library (jsoncpp)
* [dim](http://dim.web.cern.ch/dim/) 

Optionally :

 * Qt (qt4-dev)
 * Wt (witty-dev)
 * Doxygen (doxygen)

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
