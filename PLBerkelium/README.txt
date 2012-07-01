# PLBerkelium

In order to build this plugin you will need.

1. Berkelium binaries, http://berkelium.blao.de/2012-02-15/
2. PixelLight, http://www.pixellight.org/site/index.php/page/2.html

Do not forget to include the following files with your application.

* berkelium.exe
* berkelium.dll
* icudt46.dll
* resources.pak
* wow_helper.exe
* locales\en-US.dll

These files are included within the Berkelium binaries.


# Building under Windows using Visual Studio 2010 and the current Git version of PixelLight
1. Add an environment variable "PL_ROOT" pointing to the location of your PixelLight Git repository (e.g. "C:\pixellight\")
2. Add an environment variable "BERKELIUM_ROOT" pointing to the location of your Berkelium copy (e.g. "C:\berkelium\")
3. Restart Visual Studio in case it's currently opened
4. Open "PLBerkelium.sln" and build it
