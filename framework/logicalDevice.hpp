
class CLogicalDevice{
public:
    CLogicalDevice(){
        printf("logicalDevice\n");
    }

    ~CLogicalDevice(){

    }

    VkDevice handle{VK_NULL_HANDLE};
    VkDevice getHandle() const{ return handle;}

    // void createLogicalDevices(CPhysicalDevice &physicalDevice){

    // }

    CDebugger * debugger;
};