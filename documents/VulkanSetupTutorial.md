# Vulkan Setup Tutorial

## Vulkan SDK 安装
Vulkan SDK下载地址：  
https://vulkan.lunarg.com/sdk/home#windows  
下载最新版本，比如Windows 1.2.176.1 SDK Installer (255mb)  
点击安装。  
验证安装好了的方法: (in command prompt)  
> echo %VULKAN_SDK%

## GLFW安装
由于Vulkan没有提供窗口创建功能，所以在使用Vulkan前还需要下载窗口创建相关的库，这里我们选择GLFW。
https://www.glfw.org/download.html  
下载64-bit Windows binaries (2.8mb)  
解压，更该文件夹为GLFW，并放在VulkanSDK文件夹下。  
GLFW的使用方法：  
> #define GLFW_INCLUDE_VULKAN   
> #include <GLFW/glfw3.h>  

如果不选择GLFW，可以使用SDL  

## GLM安装
下载GLM库，该库是针对OpenGL的数学库，为了方便数学运算。  
https://glm.g-truc.net/0.9.9/index.html  
下载0.9.9.8 (3.3mb)  
同样更名为GLM并放在VulkanSDK文件夹下。  
GLM里面只有glm子文件夹是有用的  

## Visual Studio设置
新建console项目（切换至64位模式）。属性，C++, Additional include directories  
C:\VulkanSDK  
C:\VulkanSDK\GLM  
C:\VulkanSDK\GLFW\include  
C:\VulkanSDK\1.2.176.1\Include  

属性，Linker,  Additional library directories
C:\VulkanSDK\GLFW\lib-vc2017
C:\VulkanSDK\1.2.176.1\Lib

属性，Linker, Input
vulkan-1.lib
glfw3.lib

### 另一种配置方法
先设置好环境变量，然后在include和lib directories里面加入：  
$(INCLUDE)  
$(LIB)  
仍旧需要拷贝vulkan-1.lib和glfw3.lib  
这样就不用每次生成新工程都在vs里设置那一堆目录了。  

## 测试GLFW，Vulkan以及GLM库是否工作良好
VulkanBasic  
运行结果：13  
参考文献：https://zhuanlan.zhihu.com/p/269575620  


## 编译Shader的方法
建立shader.vert和shader.frag两个文件，并把shader内容写好。名字其实可以随意，这是约定俗成的名字。  
建立compile.bat文件，内容如下：   
```
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe shader.vert -o vert.spv 
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe shader.frag -o frag.spv 
pause
```
运行这个文件，会生成两个SPIR-V bytecode文件。  
在Vulkan代码里通过如下方式读取：  
> auto vertShaderCode = readFile("shaders/vert.spv");   
> auto fragShaderCode = readFile("shaders/frag.spv");  

## 补充说明
1. 有的test需要c++17，可以在Visual Studio项目设置里面更改。（C++, Language, C++ Language Standard）  
2. 关于std_image的问题：是个图像解码库。（https://github.com/nothings/stb） 特点：支持png和jpeg  
重点关注如下三个头文件：stb_image.h用于图像加载。stb_image_write.h用于写入图像文件。stb_image_resize.h用于改变图像尺寸。  
总的来说，它还是挺简单易用的，在平常做一些 Demo 以及需要快速实现、验证功能的情况下都可以考虑考虑。  
但是在一些大型的项目中，还是要深思熟虑一些，从性能方面考虑，它肯定不如老牌的图像解码库了，像 libjpeg-turbo 解码用到了 NEON 这样 SIMD （单指令流多数据流）的操作，才是大型项目的首选了。  
(说到图像解码库，最容易想起的就是 libpng 和 libjpeg 这两个老牌图像解码库了。)  
4. 有些test需要设置好shader和texture
5. 关于tiny_obj_load的问题：用来读obj文件。(https://github.com/tinyobjloader/tinyobjloader)只用include一个头文件
> #define TINYOBJLOADER_IMPLEMENTATION  
> #include <tiny_obj_loader.h>  

5. glfw3.h已经包含了vulkan.h，就不用再写了
6. vk_sdk_platform.h
加上这句
> #include <vulkan/vk_sdk_platform.h>

7. 2020 Siggraph的sample目前run的结果是黑屏幕，需要debug。  
8. pragma once与#ifndef的作用有什么区别（作用：为了避免同一个文件被include多次）
#pragma once是编译相关，就是说这个编译系统上能用，但在其他编译系统不一定可以，也就是说移植性差，不过现在基本上已经是每个编译器都有这个定义了。  
#ifndef，#define，#endif这个是C++语言相关，这是C++语言中的宏定义，通过宏定义避免文件多次编译。所以在所有支持C++语言的编译器上都是有效的，如果写的程序要跨平台，最好使用这种方式  
9. VSCode debug经验：如果忘记拷贝dll的话，会导致debugger失效。
10. 编译错误：error: stray ‘\377’ in program
原因：源文件没有用UTF8格式保存
解决：文件另存为UTF8格式，并且在VSCode内需要重新打开。



