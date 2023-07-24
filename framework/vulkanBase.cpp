#include "vulkanBase.h"

CVulkanBase::CVulkanBase(){
    mainCamera.type = Camera::CameraType::firstperson;
    mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    mainCamera.movementSpeed = 1.0f;
    mainCamera.rotationSpeed = 20.0f;
}

CVulkanBase::~CVulkanBase(){  
}

// void CVulkanBase::wxjSetVertices3D(float input[]){

// }

void CVulkanBase::updateUniformBuffer(uint32_t currentFrame, float durationTime) {
    	// UniformBufferObject ubo{};
		// ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		// glm::mat4x4 m = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10.0f);
		// m[1][1] *= -1;
		// ubo.proj = m;
		// ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		UniformBufferObject ubo{};
		ubo.view = mainCamera.matrices.view;
		ubo.proj = mainCamera.matrices.perspective;
		// if (pipelineType == PIPELINE_MIPMAP) {
		     //ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		// }
		// else {
			ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//}

		memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));

    CApplication::updateUniformBuffer(currentFrame, durationTime);
}

void CVulkanBase::update(){
    //printf("triangle update...\n");
    CApplication::update();
}

void CVulkanBase::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    CApplication::recordCommandBuffer(commandBuffer, imageIndex);
}

void CVulkanBase::drawFrame(){
    //printf("triangle drawFrame...\n");
    CApplication::drawFrame();
}

void CVulkanBase::initVulkan(){
    printf("vulkanBase init\n");
    CApplication::initVulkan();
}