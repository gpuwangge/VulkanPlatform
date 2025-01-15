#ifndef H_DATABUFFER
#define H_DATABUFFER
#include "common.h"
#include "context.h"


//Uniform Naming Rule: PipelineType_DescriptorType_Purpose
enum UniformTypes {
    GRAPHCIS_UNIFORMBUFFER_CUSTOM =   0x00000001, 
    GRAPHCIS_UNIFORMBUFFER_LIGHTING = 0x00000002,
    GRAPHCIS_UNIFORMBUFFER_MVP =      0x00000004, //assume app uses one: MVP or VP
    GRAPHCIS_UNIFORMBUFFER_VP =       0x00000008,
    GRAPHCIS_COMBINEDIMAGE_SAMPLER =  0x00000010,
    
    COMPUTE_UNIFORMBUFFER_CUSTOM =   0x00000020,
    COMPUTE_STORAGEBUFFER_DOUBLE =   0x00000040,
    COMPUTE_STORAGEIMAGE_TEXTURE =   0x00000080,
    COMPUTE_STORAGEIMAGE_SWAPCHAIN = 0x00000100
};

//each line must be aligned to 16 bytes. In shader use vec4 instead of vec3
struct LightAttribute{
    glm::vec4 lightPos;
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    float dimmerSwitch;
};

//this class to create graphics descript layer(static), and non-texture descripter set(static), create sampler(static)
//texture descripter leave to CObject
//also provide function to update non-texture uniform(static) (TODO)


#define LIGHT_NUM 256
struct LightingUniformBufferObject {
    alignas(16) LightAttribute lights[LIGHT_NUM]; //support up to 256 lights: 256*32+16=8208 bytes. Normal uniform buffer size is 65536 bytes(64kb)
    alignas(16) glm::vec4 cameraPos; 

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;//not important, will be reset
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;//VK_SHADER_STAGE_VERTEX_BIT;
        return binding;
    }
};

struct TextureAttributeInfo{
    std::string name;
    int miplevel;
    int samplerid;
    bool enableCubemap;
};

enum VertexStructureTypes{
    NoType,
    TwoDimension,
    ThreeDimension,
    ParticleType
};


struct Vertex3D {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
    glm::vec3 normal;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; 
		bindingDescription.stride = sizeof(Vertex3D);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

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

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex3D, normal);

		return attributeDescriptions;
	}

	bool operator==(const Vertex3D& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
	}
};
// namespace std {
// 	template<> struct hash<Vertex3D> { 
// 		size_t operator()(Vertex3D const& vertex) const {
// 			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
// 		}
// 	};
// }

namespace std {
	//Custom hash function operator() for Vertex3D
	//operator(): input is Vertex3D type hash key, output is std::size_t hash value
	//There are different ways to implement this, choose a method for high quality hash function
	void inline custom_hash_combine(size_t &seed, size_t hash){
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}
	template<> struct hash<Vertex3D> { 
		size_t operator()(Vertex3D const& vertex) const {
            //code from vulkan tutorial, removed this because of android compitability issue
			//return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
            
			//return 0; 
			//explain: this will disable hash algorithm. code still works, but not optimized

			size_t seed = 0;
			custom_hash_combine(seed, hash<float>()(vertex.pos.x));
			custom_hash_combine(seed, hash<float>()(vertex.pos.y));
			custom_hash_combine(seed, hash<float>()(vertex.pos.z));
			custom_hash_combine(seed, hash<float>()(vertex.color.x));
			custom_hash_combine(seed, hash<float>()(vertex.color.y));
			custom_hash_combine(seed, hash<float>()(vertex.color.z));
			custom_hash_combine(seed, hash<float>()(vertex.texCoord.x));
			custom_hash_combine(seed, hash<float>()(vertex.texCoord.y));
			custom_hash_combine(seed, hash<float>()(vertex.normal.x));
			custom_hash_combine(seed, hash<float>()(vertex.normal.y));
			custom_hash_combine(seed, hash<float>()(vertex.normal.z));
			return seed;
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

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Particle);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Particle, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Particle, color);

        return attributeDescriptions;
    }
};

class CWxjBuffer final{
public:
    CWxjBuffer(): m_size(0){}
    ~CWxjBuffer(){}

    VkResult init(IN VkDeviceSize requiredSize, VkBufferUsageFlags usage) {
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
        result = vkCreateBuffer(CContext::GetHandle().GetLogicalDevice(), IN &vbci, PALLOCATOR, OUT &buffer);
        //REPORT("vkCreateBuffer");

        //Step 2:allocate memory(create deviceMemory in gpu)
        VkMemoryRequirements			vmr;
        vkGetBufferMemoryRequirements(CContext::GetHandle().GetLogicalDevice(), IN buffer, OUT &vmr);		// fills vmr
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
        vmai.memoryTypeIndex = FindMemoryThatIsHostVisible(vmr.memoryTypeBits);
        //VkDeviceMemory				vdm;
        result = vkAllocateMemory(CContext::GetHandle().GetLogicalDevice(), IN &vmai, PALLOCATOR, OUT &deviceMemory);
       
        //REPORT("vkAllocateMemory");
        //deviceMemory = vdm;

        //Step 3: bind memory(bind buffer and deviceMemory)
        result = vkBindBufferMemory(CContext::GetHandle().GetLogicalDevice(), buffer, IN deviceMemory, 0);		// 0 is the offset
        //REPORT("vkBindBufferMemory");

        return result;
    }

    VkResult fill(IN void * data) {
        //Step 4:copy memory(copy data into deviceMemory)
        void * pGpuMemory;
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), IN deviceMemory, 0, VK_WHOLE_SIZE, 0, &pGpuMemory);	// 0 and 0 are offset and flags
        memcpy(pGpuMemory, data, (size_t)m_size);
        vkUnmapMemory(CContext::GetHandle().GetLogicalDevice(), IN deviceMemory);
        return VK_SUCCESS;
    }

    void DestroyAndFree(){
        if(m_size != 0){
            vkDestroyBuffer(CContext::GetHandle().GetLogicalDevice(), buffer, nullptr);
            vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), deviceMemory, nullptr);
        }
    }

    VkBuffer		buffer;
    VkDeviceMemory		deviceMemory;

private:
	VkDeviceSize		m_size;

    int FindMemoryByFlagAndType(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits) {
        VkPhysicalDeviceMemoryProperties	vpdmp;
        vkGetPhysicalDeviceMemoryProperties(CContext::GetHandle().GetPhysicalDevice(), OUT &vpdmp);//instance->pickedPhysicalDevice->get()->getHandle()
        for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
            VkMemoryType vmt = vpdmp.memoryTypes[i];
            VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
            if ((memoryTypeBits & (1 << i)) != 0) {
                if (((vmpf & memoryFlagBits) && (vmpf & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != 0){
                    //fprintf(debugger->FpDebug, "Found given memory flag (0x%08x) and type (0x%08x): i = %d\n", memoryFlagBits, memoryTypeBits, i);
                    return i;
                }
            }
        }

        //fprintf(debugger->FpDebug, "Could not find given memory flag (0x%08x) and type (0x%08x)\n", memoryFlagBits, memoryTypeBits);
        throw  std::runtime_error("Could not find given memory flag and type");
    }

    int FindMemoryThatIsHostVisible(uint32_t memoryTypeBits) {
        return FindMemoryByFlagAndType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryTypeBits);
    }


};

struct MVPData{
    alignas(16) glm::mat4 model; //16*4=64 bytes
	alignas(16) glm::mat4 view; //16*4=64 bytes
	alignas(16) glm::mat4 proj; //16*4=64 bytes
    alignas(16) glm::mat4 padding; //: MVP size is 192 bytes, but require a multiple of device limit minUniformBufferOffsetAlignment 256.
    //Each element of pDynamicOffsets which corresponds to a descriptor binding with type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC must be a multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment
};  

#define MVP_NUM 256
struct MVPUniformBufferObject {
	//MVPData *mvpData; //dynamic doesn't work

    //for now, support two groups of mvpData. Each draw only use one mvpData matrices. Use offset to access.
    //Each mvpData is to be aligned to be 256 bytes
    //Support up to 256 (MVP) objects. buffer size is 256*256 = 65536 bytes; Buffer range is 256 bytes(for each object)
    //65536 bytes = 64 kilo bytes
    MVPData mvpData[MVP_NUM]; 

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        return binding;
    }

public:
    MVPUniformBufferObject(){
        
    }   

    void init(int mvpCount){
       // mvpData = new MVPData[mvpCount];
        //std::cout<<"Created mvpData, mvpCount = "<<mvpCount<<std::endl;
    }

   // MVPData getMVP(int mvpId){
    //    return mvpData[mvpId];
    //}

    ~MVPUniformBufferObject(){
        //if(mvpData) delete mvpData;
    }
};

struct VPUniformBufferObject {
	//alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        return binding;
    }
};

struct ModelPushConstants{
    glm::mat4 model;
};

#endif