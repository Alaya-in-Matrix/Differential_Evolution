# README

* Author: lvwenlong_lambda@qq.com
* Last Modified:2016年02月21日 星期日 14时16分44秒 日

## Build and Install

* You need CMake, with a minimum version of 3.2.1
* You need boost, and if you installed boost in a custom directory, you have to specify it by `-DBOOST_ROOT=yore/boost/root`
* By default, CMake will try to install the binary program to `/usr/local/bin`, if you want to install it into a custom directory, you have to specify a prefix by `-DCMAKE_INSTALL_PREFIX=your/install/prefix`, then: 
    * The program would be installed into `your/install/prefix/bin`
    * A DE header would be installed into `your/install/prefix/inc`
    * A DE shared object would be installed into `your/install/prefix/lib`
    * A simple README(this file) would be installed into `your/install/prefix/doc`

Below is an examle to install:

```bash
cmake -DBOOST_ROOT=~/.softwares/boost -DCMAKE_INSTALL_PREFIX=~/mysoft/de-hspice
make install
```

You can also specify the build type(debug or release) manually by `-DCMAKE_BUILD_TYPE=release` or `-DCMAKE_BUILD_TYPE=debug`

## Note

* When you're implementing your own mutation strategy, you need to consider that `de.f()` might give a random number, for example, 
if you use `DERandomF` as DE solver
* This program is not optimized for performance on the assumption that the function evaluation is the most expensive part
