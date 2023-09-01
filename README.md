<p float="left">
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/vulkanLogo.png" alt="alt text" width="200" height="150">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/windowsLogo.png" alt="alt text" width="200" height="150">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/androidLogo.png" alt="alt text" width="300" height="150">  
</p>


# Object-oriented Vulkan Platform for Windows/Android
This project creates a universal Vulkan platform which can be used to develop Vulkan sample tests, and compile them to binary that runs on hybrid environment.     
- Native Vulkan is too hard to use, this wrapper is easy to understand and experiment new stuffs
- Build windows and android binary at the same time, no extra coding! Save some time for dates! (and no need to have Android Java/Kotlin/Gradle knowledge!)
 
<img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/shadowMapAVDDemo.png" alt="alt text" width="150" height="330">  


## How to compile to Windows binary(.exe)
Prepare:  
1. Install Vulkan  
1. Install CMake  
1. Setup GLFW (Or use SDL at your preference)  
1. Setup GLM  
1. [Optional] Download other thirdparty headers: stb_image.h and tiny_obj_loader.h (I already put both files in the "thirdparty" folder in this repo)  
### for all samples
mkdir build  
cd build  
cmake -G "MinGW Makefiles" ..   
make  

### for one sample(SimpleTriangle)
mkdir buildS  
cd buildS  
cmake -G "MinGW Makefiles" -D SINGLE=true ..  
make  

## How to compile to Android binary(.apk)
1. Install Android Studio: https://developer.android.com/  
1. Install NDK: https://developer.android.com/studio/projects/install-ndk  
   You only need to install&setup NDK. You should already installed and configured CMake when installing Vulkan.  
1. [Optional] To verify setup success or not, make sure to run these tests: https://github.com/android/ndk-samples  
   Especially the "hello-vulkan" test.  
1. [Optional] Edit samples/androidSample.h will tell Android Studio which sample to compile    
1. Launch Android Studio, open "androidVulkanSandbox" folder.   Select "Build" > "Build Bundle(s) / APK(s) > Build APK(s)"  
1. [Optional] If you want to run sample on AVD(Android Virtual Device), in the Android Studio, click the green button on the top right corner.  

## Distribution
1. For Android, just distribute the single .apk file, it contains all resources.  
1. For Windows, it is more complicated. Besides the .exe binary, you should include the following resources(if the sample use any):  
models/  
shaders/  
textures/  
Also include the following dynamic libs:  
glfw3.dll  
libgcc_s_seh-1.dll  
libstdc++-6.dll  
vulkan-1.dll  

## Credits
https://github.com/StanislavPetrovV/FurMark  
https://github.com/SaschaWillems/Vulkan  
https://github.com/KhronosGroup/Vulkan-Samples  
https://github.com/nothings/stb/tree/master  
https://github.com/tinyobjloader/tinyobjloader  
https://vulkan-tutorial.com/  
https://web.engr.oregonstate.edu/~mjb/vulkan/  
https://github.com/android/ndk-samples  

## Troubleshooting
1. Windows binary can not run because missing vulkan-1.dll  
This is because Vulkan SDK or Vulkan Runtime is not installed.  
Solution is to install Vulkan SDK(https://vulkan.lunarg.com/sdk/home#windows)  
If you only want to distribute the binary, only download Vulkan Runtime, unzip and copy vulkan-1.dll to the binary folder  
1. Windows binary can not run because missing libstdc++-6.dll or/and libgcc_s_seh-1.dll  
This is because MinGW is not installed. Install MinGW will solve this issue.  
If you only want to distribute the binary, copy libstdc++-6.dll or/and libgcc_s_seh-1.dll from C:\mingw64\bin (or where you install MinGW) to the binary folder  
1. Windows binary has no error message but nothing shows up or it simply CTD(crash to desktop)  
Most likely hardware(GPU) does not support Vulkan; or you are using virtual machine that has no GPU support.  
You can verify this by installing Vulkan SDK, and runing the test under Bin/vkcube.exe.  
Solution is to not use virtual machine, and buy a real physical graphics card(that supports Vulkan, which most will do) at https://www.amazon.com/ (or whatever e-commerce website)  

