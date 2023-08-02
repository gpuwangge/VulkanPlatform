#ifndef H_DATASTRUCTURE
#define H_DATASTRUCTURE
#include "common.h"

struct Vertex3D {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; 
		bindingDescription.stride = sizeof(Vertex3D);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;  //binding is in use in vertex shader(default is using 0)
		attributeDescriptions[0].location = 0; //location is in use in vertex shader
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex3D, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex3D, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex3D, texCoord);

		return attributeDescriptions;
	}

	bool operator==(const Vertex3D& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};
namespace std {
	template<> struct hash<Vertex3D> {
		size_t operator()(Vertex3D const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct Vertex2D {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex2D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex2D, color);

        return attributeDescriptions;
    }

    bool operator==(const Vertex2D& other) const {
        return pos == other.pos && color == other.color;
    }
};



class CWxjBuffer final{
public:
    CWxjBuffer(){}
    ~CWxjBuffer(){}

    VkResult init(IN VkDeviceSize requiredSize, VkBufferUsageFlags usage, VkPhysicalDevice physicalDevice, VkDevice logicalDevice) {
        //HERE_I_AM("Init05DataBuffer");
        //Step1:Create Buffer(create buffer)
        VkResult result = VK_SUCCESS;

        VkBufferCreateInfo  vbci;
        vbci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vbci.pNext = nullptr;
        vbci.flags = 0;
        vbci.size = requiredSize;
        vbci.usage = usage;
        vbci.queueFamilyIndexCount = 0;
        vbci.pQueueFamilyIndices = (const uint32_t *)nullptr;
        vbci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	// can only use CONCURRENT if .queueFamilyIndexCount > 0
        result = vkCreateBuffer(logicalDevice, IN &vbci, PALLOCATOR, OUT &buffer);
        //REPORT("vkCreateBuffer");

        //Step 2:allocate memory(create deviceMemory in gpu)
        VkMemoryRequirements			vmr;
        vkGetBufferMemoryRequirements(logicalDevice, IN buffer, OUT &vmr);		// fills vmr
        //if (Verbose){
        //fprintf(debugger->FpDebug, "Buffer vmr.size = %lld\n", vmr.size);
        //fprintf(debugger->FpDebug, "Buffer vmr.alignment = %lld\n", vmr.alignment);
        //fprintf(debugger->FpDebug, "Buffer vmr.memoryTypeBits = 0x%08x\n", vmr.memoryTypeBits);
        //fflush(debugger->FpDebug);
        //}
         m_size = vmr.size;//vmr.size is different than the input requiredSize, because of alignment reason, vmr.size can be larger

        VkMemoryAllocateInfo			vmai;
        vmai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vmai.pNext = nullptr;
        vmai.allocationSize = vmr.size; 
        vmai.memoryTypeIndex = FindMemoryThatIsHostVisible(vmr.memoryTypeBits, physicalDevice);
        //VkDeviceMemory				vdm;
        result = vkAllocateMemory(logicalDevice, IN &vmai, PALLOCATOR, OUT &deviceMemory);
       
        //REPORT("vkAllocateMemory");
        //deviceMemory = vdm;

        //Step 3: bind memory(bind buffer and deviceMemory)
        result = vkBindBufferMemory(logicalDevice, buffer, IN deviceMemory, 0);		// 0 is the offset
        //REPORT("vkBindBufferMemory");

        return result;
    }

    VkResult fill(IN void * data, VkDevice logicalDevice) {
        //Step 4:copy memory(copy data into deviceMemory)
        void * pGpuMemory;
        vkMapMemory(logicalDevice, IN deviceMemory, 0, VK_WHOLE_SIZE, 0, &pGpuMemory);	// 0 and 0 are offset and flags
        memcpy(pGpuMemory, data, (size_t)m_size);
        vkUnmapMemory(logicalDevice, IN deviceMemory);
        return VK_SUCCESS;
    }

    void DestroyAndFree(VkDevice logicalDevice){
        if(m_size > 0){
            vkDestroyBuffer(logicalDevice, buffer, nullptr);
            vkFreeMemory(logicalDevice, deviceMemory, nullptr);
        }
    }

    VkBuffer		buffer;
    VkDeviceMemory		deviceMemory;

private:
	VkDeviceSize		m_size = 0;

    int FindMemoryByFlagAndType(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits, VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceMemoryProperties	vpdmp;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, OUT &vpdmp);//instance->pickedPhysicalDevice->get()->getHandle()
        for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
            VkMemoryType vmt = vpdmp.memoryTypes[i];
            VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
            if ((memoryTypeBits & (1 << i)) != 0) {
                if ((vmpf & memoryFlagBits) != 0){
                    //fprintf(debugger->FpDebug, "Found given memory flag (0x%08x) and type (0x%08x): i = %d\n", memoryFlagBits, memoryTypeBits, i);
                    return i;
                }
            }
        }

        //fprintf(debugger->FpDebug, "Could not find given memory flag (0x%08x) and type (0x%08x)\n", memoryFlagBits, memoryTypeBits);
        throw  std::runtime_error("Could not find given memory flag and type");
    }

    int FindMemoryThatIsHostVisible(uint32_t memoryTypeBits, VkPhysicalDevice physicalDevice) {
        return FindMemoryByFlagAndType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryTypeBits, physicalDevice);
    }


};

typedef struct MyImageBuffer
{
	VkImage		image;
	VkDeviceMemory		deviceMemory;
	VkDeviceSize		size;
public:
    MyImageBuffer(): size(0){}
} MyImage;

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

#endif