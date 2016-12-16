# procctrl 

## A multi-host remote process management software
Copyright Rémi Ete

See COPYING.txt for GNU licence

```diff
-WARNING : 
-This software is under development. 
-Do not download it for deployment. Wait for a first release ! 
```
## INSTALL:

The build process is managed by [CMake](http://cmake.org)

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

All options area by default set to OFF.

One can set the dim directory using -DDIMDIR=... in cmake command to point on a particular dim release

### Bug report

You can send emails to <remi.ete@gmail.com>
or use the [github issues interface](https://github.com/rete/procctrl/issues)
