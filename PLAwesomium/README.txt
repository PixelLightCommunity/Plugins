# PLAwesomium

In order to build this plugin you will need.

1. Awesomium 1.7 RC1 binaries, http://awesomium.com/download/
2. PixelLight, http://www.pixellight.org/site/index.php/page/2.html

Do not forget to include the following files with your application.

* awesomium_process.exe
* awesomium.dll
* icudt.dll

These files are included within the Awesomium binaries.


# Building under Windows using Visual Studio 2010 and the current Git version of PixelLight
1. Add an environment variable "PL_ROOT" pointing to the location of your PixelLight Git repository (e.g. "C:\pixellight\")
2. Add an environment variable "AWESOMIUM_ROOT" pointing to the location of your Awesomium copy (e.g. "C:\awesomium\")
3. Restart Visual Studio in case it's currently opened
4. Open "PLAwesomium.sln" and build it
