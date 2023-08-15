# Object-oriented Vulkan Platform for Windows/Android
This project creates a universal Vulkan platform that can easily develop Vulkan sample tests and compile them for both Windows&Android platforms.  

## How to compile to Windows exe binary
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

## How to compile to Android APK
1. Install Android Studio: https://developer.android.com/  
1. Install NDK: https://developer.android.com/studio/projects/install-ndk  
   You only need to install&setup NDK. You should already installed and configured CMake when installing Vulkan.  
1. To verify setup success or not, make sure to run these tests: https://github.com/android/ndk-samples  
   Especially the "hello-vulkan" project.  
  



## Construction
=========Concet of InFlight========  
InFlight: means fast




=========Concetp of Compute Shader========
Workgroup: it is a 3d array matrix. one element in this matrix is called workitem
Ex: 6x4x3=72 workitem parallel
You can have many workgroups, but workgroups can not run parallel


=========Platform Structure=========
application: contian a instance unique smart pointer
instance: containn a physical devices unique smart pointer
physical devices: containn  a logical devices unique smart pointer array。同时physical device需要访问呢父类instance,使用forward declaration定义instance，并在头文件中创建一个指针。同时，在physical device cpp文件中include instance.h。
logical devices

Life of a Vulkan Sample:
1.application creates instance
2.instance enumerates extension properties (the number of supported extensions is related to the driver version?)
3.instance gets required extension. (for example, GLFW need 2 extensions: VK_KHR_surface, VK_KHR_win32_surface)
4.application creates GLFW window surface
5.instance querys GPUs(physical devices）: vkEnumeratePhysicalDevices() will create all physical devices(a vector of VkPhysicalDevice). 
instance also creates a array of unique smart pointer with CPhysicalClass, intialized with the VkPhysicalDevice objects from step5
6.instance picks one suitable gpu among all the physical devices
describe a physical device we need: 
find queue family supported of a physical device: findQueueFamilies(surface)				-> ex: we need a queue family that has graphics and compute family, and graphics family, and present family
find surface supported of a physical device: vkGetPhysicalDeviceSurfaceSupportKHR()			-> to see if the surface is supported by the graphics queue, if yes, then it means it supports present family
find extension supported of a physical device: checkDeviceExtensionSupport()				-> ex: we need deviceExtension to be VK_KHR_SWAPCHAIN_EXTENSION_NAME
find swapChain supported of a physical device: querySwapChainSupport(surface)				->swapChain must support format and presentMode
return a physical device(a unique smart pointer) that support all this.


===========Vulkan多队列同步机制： Fences and Semaphores=============
https://www.youtube.com/watch?v=GiKbGWI4M-Y
动画演示Fence、Semaphore(12分钟开始)(24分钟是重点)

https://zhuanlan.zhihu.com/p/449222522
同步的目的是什么：最大化使用CPU和GPU的资源，减少两者等待的时间。

同步机制有两种
I、单队列(queue)内同步(采用barriers, events and subpass dependencies实现)
II、多队列同步机制(Fence, Semaphore)
Semaphore：用于同步GPU间的任务。比如有两个Queue都有command batch(同时submit的一些cmd组成一个batch)。
Queue1 signals Semaphore，并且Queue2 waits on Semaphore。
结果就是Queue1的command batch执行完毕后，Queue2的command才开始execute。
Fence: 用于同步GPU-CPU之间的任务。比如如果需要用swap buffer展示下一帧，就可以用fence莱霍智什么时候去做swap操作。

在Vulkan中，资源读写的同步由app程序员负责。
Vulkan在record command buffer后，需要调用vkQueueSubmit()函数(分别对graphics and compute queue调用)，这时候GPU才会开始执行传入的命令。
Vulkan下GPU的执行顺序：
同时提交的submit，先提交的先执行。
同一个sumit内，如果有很多个command buffer(存在不同的VkSubmitInfo里),index小的先执行。
同一个VkSubmitInfo内，如果有很多commands，也是index小的先执行。
renderpass and subpass?

---Fence---
Fence配合vkQueueSubmit(Fence)使用。这时候Fence处于被设置状态。
vkResetFence会把Fence恢复。
vkWaitForFences会在Fence恢复之前阻塞CPU。这个函数可以理解成一个状态查询，它可能返回VK_TIMEOUT或VK_SUCCESS。

---Semaphore---
Semaphore也是配合vkQueueSubmit(VksubmitInfo)使用，只不过它是在VksubmitInfo结构中。
在VksubmitInfo结构中有三个参数：
pWaitSemaphores: 指向一些Semaphore。队列的指令会等待这些Semaphre被通知了之后才开始执行。
pWaitDstStageMask: 表示提交的所有命令在执行到本指针指向的位置时停下。需要等待pWaitSemaphores所指向的所有Semaphore恢复。 
pSignalSemaphores: 此次提交的命令全部接收后，本指针指向的所有Semapore都会被通知。
首先，我们确定一个Stage，比如VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT，因为这一步是Graphics Queue的几乎是最后一步，到这一步就说明本frame的image render好了。
然后，pSignalSemaphores指向renderFinishedSemaphores，表示render finish的信号灯变更成绿灯。
最后，pWaitSemaphores指向的imageAvailableSemaphores收到了通知，表示Presentation Queue的image可以present了。
以上Semaphore技巧也叫Binary Semaphore。
另外有Timeline Semaphore技巧，区别是增加了64-bit integer来指示payload。

结论：Fence用于阻塞CPU直到Queue中的命令执行结束(GPU、CPU之前同步)。
Semaphre用于不同的命令提交之间的同步(GPU、GPU之前同步)。

举例：在simple_triangle这个实例中，使用了2个frame，Semaphore四个，Fence两个
imageAvailableSemaphore x2 被pWaitSemaphores指向
renderFinishedSemaphore x2 被pSignalSemaphores指向(两个合起来表示的逻辑就是render完成后，通知image可用了。)
inFlightFence x2 赋值是VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
在drawFrame()函数中的分配：
VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
submitInfo.waitSemaphoreCount = 1;
submitInfo.pWaitSemaphores = waitSemaphores;
submitInfo.pWaitDstStageMask = waitStages;
和
VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
submitInfo.signalSemaphoreCount = 1;
submitInfo.pSignalSemaphores = signalSemaphores;
和
vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);
解读：
对于Fence
一开始vkWaitForFences(fence)试图把CPU挡住，但这时候fence没设置，所以挡不住。
然后vkResetFence(Fence)会把Fence恢复。
最后vkQueueSubmit(Fence)设置Fence。
下一次运行到drawFrame()的相同frame index的时候，vkWaitForFences(fence)有可能挡住CPU(取决于GPU是否已经执行完毕了上一个command queue里的commands)
对于Semaphore
两个Semaphore合起来表示的逻辑就是render完成后，通知image可用了。
vkAcquireNextImageKHR()的执行依赖image是否可用。



==========Clean Up====================
哪些变量需要Clean：
对于base:
descriptorPool
swapChain
CommandBuffers
renderPass
framebuffer
shadermodule
imageView, image and image memory for depth
pipelineCache
command pool
semaphore and fense
logical device
instance
event, surface and window
all other misc(layer, keyboard...)
对于Test: 
1.Pipeline相关(pipeline, pipelineLayout, descriptorSetLayout)
2.Buffer相关(buffer, memory) for vertex, index and uniform buffer
3.Sync Object相关(fence, semaphore)





