# CMake Tutorial

## 背景知识
直接使用g++命令行编译，如果文件数量很多，那么即使只改动其中一个，也需要全部编译。  
VSCode自带多文件编译系统，也就是task.json，但是用起来不够灵活，无法满足需求。  
对于结构简单的多文件项目，可以使用Makefile。但是，Makefile是平台相关的(比如mac的makefile放到windows下就会有问题)。  
对于复杂一些的多文件项目，使用CMake。而且CMake是平台无关的。  
注意安装CMake的时候，选择下载解压包然后放到c盘下。如果选择安装包，似乎会自动指定MS的编译器。  

## CMake使用步骤
目标： 运行一个基本的编译命令生成exe   
**`1. 安装CMake`**  
https://cmake.org/download/  
https://cmake-3.27.0-windows-x86_64.zip/  
下载后解压复制到C盘根目录下, 把C:\cmake-3.27.0-windows-x86_64\bin添加到Path环境变量中   
验证安装CMake的cmd命令(此cmd仅在windows prompt下生效):
> where cmake

编写基本的CMakeLists.txt(参考下面CMake编程入门章节)。  
建立build文件夹并进入。  
运行cmake的命令:  
> cmake ..

如果要指定编译器：  
> cmake -G "MinGW Makefiles" ..

(这一步会在build文件夹下生成makefile)  
安装编译器(编译器bin目录会被放入环境变量)  
验证安装了编译器的cmd命令： 
> where gcc

进入编译器bin目录，重命名mingw32-make.exe为make.exe  
验证make设置好的cmd命令：
> where make

验证make安装好的另一个cmd命令:   
> make -v

这时候在build目录下运行make，将编译生成exe文件。  
以及在一些目录下生成obj文件。并且对每一个cpp文件都生成单独的obj文件
自己写的.h文件不需要写在CMakeLists.txt里，也不必指定其目录  
如果只修改了某一些.cpp或.h文件，make就会只重新生成相关的.o文件，并连接成exe
记得在make之前先保存所有文件。

**`2. 编译`**  
如上所述，使用make命令就可以编译  
修改源文件后需要重新编译  
修改了cpp文件后，只要文件夹架构没有改变，就不用重新运行cmake命令，而可以直接make。  
在main.cpp中加入了 #include<stdio.h>，也不用cmake，就可以默认找到这些头文件。  

**`3. 使用外部include和lib`**    
CMakeLists.txt下面添加如下代码  
> include_directories(E:\\GitHubRepository\\somefoldername)  
> link_directories("/home/server/third/lib")  

**`4. 如何给make传递参数`**    
CMakeLists.txt code:
```cmake
if(SINGLE)  
    add_executable(simpleTriangle samples/simpleTriangle.cpp)  
else()  
    aux_source_directory(${PROJECT_SOURCE_DIR}/samples SRC)  
    foreach(sampleFile IN LISTS SRC)  
        get_filename_component(sampleName ${sampleFile} NAME_WE)  
        add_executable(${sampleName} ${sampleFile})  
    endforeach()  
endif()
```
Call CMakeLists.txt  
> cmake -G "MinGW Makefiles" -D SINGLE=true ..

**`5. 其他`**    
目前有个问题是，同一段CMakeLists，在laptop Windows环境下生成name.lib，在desktop Windows环境下上生成libname.a。在VSCode terminal和windows command prompt里面都试过。编译器都是MinGW。  
另外，不管名字是什么，  
> target_link_libraries(xxx name)

这种写法都能成功link到lib。并在test环境下都能成功生成exe。所以这个问题并不影响。  

CMake命令，比如target_link_libraries()的参数是存在依赖关系的。比如  
target_link_libraries(A,B,C)里：  
A依赖B+C, B依赖C。  
(A,C,B)会出现B链接失败的错误。  


## Makefile介绍
https://www.bilibili.com/video/BV188411L7d2/?spm_id_from=333.337.search-card.all.click&vd_source=e9d9bc8892014008f20c4e4027b98036  
### Makefile的作用  
假设一个项目工程有如下源文件  
functions.h //内含数学函数和打印函数的定义; #ifndef防止重复include  
factorial.cpp //#include "functions.h"。内含数学函数的实现  
printhello.cpp //#include "functions.h"。内含打印函数的实现  
main.cpp  //#include "functions.h"。main函数里调用数学和打印函数  
该如何编译呢:  
> g++ main.cpp factorial.cpp pringhello.cpp -o main  

现在问题是，在实际的项目中，往往不止三个源文件。使用这种原始的编译方法将导致编译命令无比坑长；并且如果只改动了其中一个源文件，重新编译的时候需要把所有源文件重新编译一次，浪费时间。  

#### 解决办法A：逐个编译  
> g++ main.cpp -c  

这个命令只编译，不链接。结果是产生了main.o文件。同理：  
> g++ main.cpp -c

生成了三个.o文件，然后：  
> g++ *.o -o main  

这样就把所有的.o文件链接到了一起。  
这样带来的问题是，手动输入这些命令非常麻烦。  

#### 解决办法B：用脚本实现逐个编译  
建立一个Makefile文件，写下如下脚本：(hello目标依赖于三个源文件)  
```Makefile
hello: main.cpp printhello.cpp factorial.cpp  
    g++ -o hello main.cpp printhello.cpp factorial.cpp
```
如何运行：  
在Makefile目录下，输入make加回车。  
这时候会寻找hello这个文件。如果它不存在，就编译生成它；如果hello存在，就比较hello生成的时间和依赖的三个源文件的生成时间：如果有任何源文件比hello更新，就重新编译hello，否则，啥也不做。  
这个方法的问题是编译时间依旧很长。  

#### 解决办法C  
在方案B的基础上，如果源文件更新了，单独编译这个源文件，生成新的目标文件(.o)，然后再链接生成新的hello。  


## CMake编程入门
建立文件CMakeLists.txt  
一共三行：  
```cmake
cmake_minimum_required(VERSION 3.10)
project(hello)
add_executable(hello main.cpp factorial.cpp printhello.cpp)
```
如何运行CMake:  
> cmake .

运行结果, 生成了:  
CMakeFiles目录  
MakeFile  
结论：CMake帮我们生成了MakeFile  
这时候再用
> make

运行编译链接  
好处是CMakeLists.txt可以放到不同的操作系统上，自动生成对应系统和编译器的MakeFile(有跨平台特性)  
一个小窍门：  
一般而言运行cmake .会生成一大堆文件跟源文件混在一起。为了避免这种污染，一般手动建立一个空的build文件夹(想叫别的名字也可以)，然后执行：  
> cd build   
> cmake ..

(..就是往上一层目录里找文件)
现在，所有的中间文件都生成在build里面了


## VS Code环境下与CMake的配合开发环境
https://www.bilibili.com/video/BV13K411M78v?p=2&vd_source=e9d9bc8892014008f20c4e4027b98036  
动机：用纯g++编译多文件工程非常困难。  
方法：配合CMake进行编译，编写一个CMakeLists.txt，构建一个makefile，在生成exe, dll或lib。或者在VS Code里面配置launch.json(for debug)和tasks.json(for build before debug)  

需要预先安装VS Code, mingW64(简称gcc)和CMake  
安装VS Code的Cmake插件：  
cmake (optional，cmake语言提示插件)   
cmake tools  (optional，cmake扩展支持)  

在VS Code下使用CMake手动编译多文件项目
-编写一个简单的CMakeList.txt
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyZoo)
add_executable(my_cmake_zoo main.cpp ClassAnimal.cpp)
```
Ctrl+Shift+p打开搜索，选择CMake: Configure, 再选择GCC 8.1.0  
(这时候会出现一个build文件夹。但其实这个build文件夹删除自己再建个空的也可以)  
> cd build  
> cmake ..

(生成Makefile)
> mingw32-make.exe      

(真正的编译源文件为obj，并链接成my_cmake_zoo.exe。以后每次更新了源文件，执行这个就可以了。)  
Issue：如果系统安装了Microsoft Visual Studio，则cmake ..有可能优先调用cl.exe (特点是在build文件夹里生成了MyZoo.sln项目文件)  
Solution: 使用
> cmake -G "MinGW Makefiles" ..

这个指令强制使用MingW的gcc编译器。只是第一次用这个命令指定，以后cmake ..即可 

### 配置tasks.json
这个文件配置了编译器  
默认的tasks.json是单文件编译的配置，如果不加修改，在多文件项目下编译就会出错。  
主要就是修改"args"这个参数，使其跟手动编译的那个指令一样的参数就可以了。  
修改了tasks.json之后，就可以自动编译多文件项目了！(不必再使用CMake)  

### 配置launch.json
这个文件配置了运行时参数  
"program": 自动生成的用于debug的可执行exe文件  
"preLaunchTask": 运行调试之前的工作：使用g++.exe作为build，其实就是用来生成exe文件的工具。这个名字要跟tasks.json的那个名字对上。  

### VS Code下的调试方法
首先debug的工具也是跟着编译器MinGW安装的：gdb  
验证安装了gdb的cmd命令:
> where gdb

调试器似乎必须结合VS Code, 毕竟，我们要在VS Code里面设置断点。  
settings.json里面会记录使用的"C_Cpp_Runner.debuggerPath": "gdb"  
launch.json里面也有"miDebuggerPath": "gdb"  

最后，有一点注意，右上角的button的调试运行跟左边launch/task没啥关系。  
以下讨论都是关于launch/task的。要启动launch，按键盘上的F5!  
首先，如何调出这两个文件：  
VSCode界面下点击Add Debug Configuration会添加launch.json文件和tasks.json文件。  
launch.json解析：  
"preLaunchTask":     会在执行launch之前先执行这里后面的task。默认会调用task来build。但是我们使用make来编译，就不用这个了，所以把这一行注释掉。（tasks.json就没用了）  
“program”: 想调试的程序，设置成我们通过make生成的那个exe程序。(cmake里面一定要设定Debug模式！)  
如上，以后我们编译就通过make，然后调试就用F5。  
"cwd":   current work directory  

其他launch.json设置参数  
"request" : "launch"   会启动program  
"request": "attach"     会提示附着在一个已经运行中的program上  
关于VSCode的Debug/Release版本问题(尚未验证)：编译的时候的参数, -g是debug模式，-O2是release模式。所以VSCode默认是开debug模式的。  








