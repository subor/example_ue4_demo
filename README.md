# README #

This README would normally document whatever steps are necessary to get your application up and running.

### What is this repository for? ###

* This is the sample project to show how to implement Ruyi SDK in your unreal project, or other cpp-used project. If it's a common cpp project, the config is relatively obviously. Just include the header files in "include" folder and add the lib file path. 
* We built this project with Unreal Engine 4.18 compiled version and by now we only tested our sdk with this version. We'll test more version but probably we won't be able to test with all versions of UE4. So if you encouter any problem with implementing Ruyi SDK on your UE4 version. Please contact us anytime.
* please use Visual Studio 2017 15.3 version or later. Including Windows 10 SDK (10.0.15063.0).

### How do I get set up? ###

* download the file, unzip them
* right click on "RuyiSDKDemo.uproject" and click "Generate Visual Studio project files" then open the "RuyiSDKDemo.sln" file.
or you can just directly open the "RuyiSDKDemo.uproject" file it should do the same process as well.
* the visual studio project pretty much shows the structure of Ruyi SDK. You can also check "Source" folder and the RuyiSDKDemo.Build.cs, and a more detail doc of how to implement Ruyi SDK on bitbucket. 


### Contribution guidelines ###

* You can refer the code in "Source\RuyiSDKDemo\RuyiSDKManager.h" and ".cpp" files. It basically shows how to use Ruyi SDK API.
We suggest calling API in sub-thread or it may cause some user experience problem.
* By now our cpp SDK version is still not complete. We'll update the sample code and SDK as soon as we can

### Who do I talk to? ###

* there will be a developer support email address soon