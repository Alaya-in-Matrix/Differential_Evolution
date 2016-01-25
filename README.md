# README

* Author: lvwenlong_lambda@qq.com
* Last Modified:2016年01月25日 星期一 13时40分49秒 一

## Build and Install

* You need CMake, with a minimum version of 3.2.1
* You need boost, and if you installed boost in a custom directory, you have to specify it by `-DBOOST_ROOT=yore/boost/root`
* By default, CMake will try to install the binary program to `/usr/local/bin`, if you want to install it into a custom directory, you have to specify a prefix by `-DCMAKE_INSTALL_PREFIX=your/install/prefix`, and the program would be installed into `your/install/prefix/bin`

Below is an examle to install:

```bash
cmake -DBOOST_ROOT=~/.softwares/boost -DCMAKE_INSTALL_PREFIX=. .
make install
```
