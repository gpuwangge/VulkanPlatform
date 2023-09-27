# VS Code Tutorial

## VS Code介绍
VS Code是文本编辑器，所以是不带编译器的，需要下载一个C++编译器，比如MinGW(x86_64-win32-seh)  
 
## 安装MinGW
下载地址：
https://www.mingw-w64.org/downloads/  
页面底下 Sources这一栏，点击SourceForge。拉到底下，选择x86_64-win32-seh

下载后解压后剪切粘贴到c盘的根目录下。  
然后是配置环境变量——也就是告诉VS Code编译器在什么地方：  
把MinGW的bin路径(c:\mingw64\bin)粘贴到PATH环境变量里面。  
在command prompt里面输入  
> where gcc

就可以知道是否装好了。  
如果安装过Visual Studio，也会有编译器了： cl.exe。而且这个是Windows下VS Code的默认编译器   
(关于posix版本编译器：可以再下载x86_64-posix-seh，然后文件夹加上后缀_posix跟win32版放在一起，如果遇上p线程的问题可以换这个: 改CMakeLists使用posix版g++，但make还是一样的)  


## GitHub+VS Code安装方法
要先安装vscode(安装时勾选允许添加入右键菜单)，再安装git(在Choosing the default editor used by Git这个选项里选择VS Code作为Git's default editor)。  
安装vscode后，再安装必要插件GitLens. 安装后，左侧出现GitLens和GitLens Inspect图标。  
建议安装如下Extensions:  
C/C++  
Better Comments (optional)  
Shader languages support  

## VS Code使用说明
打开VS Code后，点击Open Folder，选择一个你希望保存code的文件夹。  
新建一个名字为xxx.c的文件，就会弹窗问你是否需要安装C/C++ Extension Pack，安装之。  
安装好了之后会提示你选择Default Compiler（gcc or g++）。  
写一个hello world程序，点击运行，会提示选择gcc.exe作为build/debug工具。  
运行结果会显示在底下的TERMINAL窗口。  
这时候注意到左侧自动建立了.vscode文件夹，里面有tasks.json文件。里头包含了编译器和编译参数等信息。可以自行修改该文件改变编译效果。  
如果要添加c_cpp_properties.json，使用如下快捷键：  
> Control+Shift+P

選擇C/C++: Edit Configurations (JSON)  
在"includePath"這一項下添加：
> "${workspaceFolder}/tfliteDist/include/tensorflow/lite"

点击左侧“Run and Debug”按钮，可以配置相关jason文件（选择GDB）将生成launch.json文件。  
在json页面打开的情况下，点击右下角Add Configuration可添加配置。选择C/C++: (gdb) Launch  
(需要修改gdb定位，也就是之前MinGW的bin文件夹内)  
(多文件项目，其实就是建立不同的文件夹。可以新建更多工作区来存放不同代码，不同工作区有不同的.vscode编译和调试配置)  
第一次运行C++文件的时候，会提示选择g++编译器  

## VS Code配置文件
新建文件夹，打开VSCode后打开该文件夹。  
如果有需要的话，拷贝相应的资源(obj, png, dll…)到工作目录内。  
添加源文件和后缀（比如.cpp），VS Code会自动生成.vscode文件夹，内含配置文件   
配置这几个Json文件：   
tasks.json: 跟编译有关的文件设置  
launch.json: 跟运行有关的文件设置  

这两个地方设置编译器：  
```
c_cpp_properties.json  
"compilerPath": "C:\\mingw64\\bin\\gcc.exe",  
settings.json  
 "C_Cpp.default.compilerPath": "C:\\mingw64\\bin\\g++.exe"  
```

### settings.json
说明：这个文件设置VS Code的compiler path and IntelliSense settings  
更新这个文件会自动更新c_cpp_properties.json。但有时候这个文件并不出现，这时候直接修改c_cpp_properties.json即可。  
```
  "C_Cpp_Runner.includePaths": [
    	"C:/VulkanSDK",
   	 "C:/VulkanSDK/GLM",
    	"C:/VulkanSDK/GLFW/include",
   	 //"C:/VulkanSDK/1.2.176.1/Include"
    	"${VULKAN_SDK}/Include"
  ],
```
另一种方法是设置好环境变量，然后再json中使用%VULKAN_SDK%来代替地址  

### tasks.json
说明：告诉Compiler build instructions。即使修改了settings.json，Compiler仍旧不知道includePaths在哪里。(注意VS Code不是IDE，因此VS Code和Compiler是独立存在的)，所以要一同修改task.json。
(如果一开始没有这个文件，只要运行一次代码它就会出现)
注意：不知为何，glfw的MingG64版本只能使用dll版，因此需要拷贝dll文件到开发目录下。
```
            "args": [
                "-std=c++17",
                "-IC:/VulkanSDK",
                //"-IC:/VulkanSDK/1.3.250.1/Include",
                "-I%VULKAN_SDK%/Include",
                "-IC:/VulkanSDK/GLFW/include",
                "-IC:/VulkanSDK/GLM",
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                //"-LC:/VulkanSDK/1.3.250.1/Lib",
                "-L%VULKAN_SDK%/Lib",
                "-LC:/VulkanSDK/GLFW/lib-mingw-w64",
                "-lvulkan-1",
                "-lglfw3dll",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}.exe"
            ],
```

### launch.json
debugger settings。这个Json会自动生成不需要修改。  
这个文件也不一定会出现。  

### c_cpp_properties.json
settings.json修改了之后，这个也会更新。  
如果cpp代码是c++17之后，需要手动把这个信息添加进这个文件。    



## VS Code+MingW64安装设置
—安装篇----


安装VS Code:
(https://code.visualstudio.com/download)
打开VS Code后，点击Open Folder，选择一个你希望保存code的文件夹。
简单教程：
新建一个名字为xxx.c的文件，就会弹窗问你是否需要安装C/C++ Extension Pack，安装之。
安装好了之后会提示你选择Default Compiler（gcc）。
写一个hello world程序，点击运行，会提示选择gcc.exe作为build/debug工具。
运行结果会显示在底下的TERMINAL窗口。
这时候注意到左侧自动建立了.vscode文件夹，里面有tasks.json文件。里头包含了编译器和编译参数等信息。可以自行修改该文件改变编译效果。
点击左侧“Run and Debug”按钮，可以配置相关jason文件（选择GDB）将生成launch.json文件。
在json页面打开的情况下，点击右下角Add Configuration可添加配置。选择C/C++: (gdb) Launch
(需要修改gdb定位，也就是之前MinGW的bin文件夹内)
(多文件项目，其实就是建立不同的文件夹。可以新建更多工作区来存放不同代码，不同工作区有不同的.vscode编译和调试配置)
第一次运行C++文件的时候，会提示选择g++编译器。
建议安装如下Extensions:
C/C++
Better Comments (optional)
Shader languages support


## 设置篇
===09022023更新========
如果不使用debugger，那不不需要设置.vscode。
因为可以自己用gcc/g++/visual studio来编译。
但是vscode的IntelliSense可以自动发现一些错误。以下是设置IntelliSense的方法。
Ctrl+Shift+P 跳出设置command prompt，选择C/C++: Edit Configuration(JSON)，这时候会生成c_cpp_properties.json。
在includePath栏添加INCLUDE，如下所示
            "includePath": [
                "${workspaceFolder}/**",
                "${INCLUDE}"
            ]
添加编译器地址，如下所示：
"compilerPath": "C:/mingw64_posix/bin/g++.exe",
修改intelliSenseMode，如下所示：
intelliSenseMode": "gcc-x64"





1、新建文件夹，打开VSCode后打开该文件夹。
2、添加源文件和后缀（比如.cpp），VS Code会自动生成.vscode文件夹，内含配置文件
接下来要配置这几个Json文件：
===========settings.json======
说明：这个文件设置VS Code的compiler path and IntelliSense settings
更新这个文件会自动更新c_cpp_properties.json。但有时候这个文件并不出现，这时候直接修改c_cpp_properties.json即可。
  "C_Cpp_Runner.includePaths": [
    "C:/VulkanSDK",
    "C:/VulkanSDK/GLM",
    "C:/VulkanSDK/GLFW/include",
    "C:/VulkanSDK/1.3.250.1/Include"
  ],
(另一种方法是设置好环境变量，然后再json中使用%VULKAN_SDK%来代替地址)
===========tasks.json==========
说明：告诉Compiler build instructions。即使修改了settings.json，Compiler仍旧不知道includePaths在哪里。(注意VS Code不是IDE，因此VS Code和Compiler是独立存在的)，所以要一同修改task.json。
(如果一开始没有这个文件，只要运行一次代码它就会出现)
注意：不知为何，glfw的MingG64版本只能使用dll版，因此需要拷贝dll文件到开发目录下。
            "args": [
                "-std=c++17",
                "-IC:/VulkanSDK",
                "-IC:/VulkanSDK/1.3.250.1/Include",
                "-IC:/VulkanSDK/GLFW/include",
                "-IC:/VulkanSDK/GLM",
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-LC:/VulkanSDK/1.3.250.1/Lib",
                "-LC:/VulkanSDK/GLFW/lib-mingw-w64",
                "-lvulkan-1",
                "-lglfw3dll",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}.exe"
            ],
========launch.json======
debugger settings。这个Json会自动生成不需要修改。
这个文件也不一定会出现。
========c_cpp_properties.json======
settings.json修改了之后，这个也会更新。
如果cpp代码是c++17之后，需要手动把这个信息添加进这个文件。
3、如果有需要的话，拷贝相应的资源(obj, png, dll…)到工作目录内。





