#include "../include/sdlManager.h"
#include "application.h"

CSDLManager::CSDLManager(){
	//std::cout<<"SDL Manager Constructor"<<std::endl;
}

CSDLManager::~CSDLManager(){

}


void CSDLManager::createWindow(int &windowWidth, int &windowHeight){
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "Could not initialize SDL." << std::endl;
        //return 1;
    }

	windowWidth = WINDOW_WIDTH;
	windowHeight = WINDOW_HEIGHT;

    m_windowWidth = WINDOW_WIDTH;
    m_windowHeight = WINDOW_HEIGHT;
    m_windowCenterX = WINDOW_WIDTH/2;
    m_windowCenterY = WINDOW_HEIGHT/2;

    window = SDL_CreateWindow("Vulkan Window", windowWidth, windowHeight, SDL_WINDOW_VULKAN);
    if(window == NULL) {
        std::cout << "Could not create SDL window." << std::endl;
        //return 1;
    }

	
}

void CSDLManager::queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions){
	Uint32 count_instance_extensions = 0;
    const char * const *instance_extensions = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);
    //std::vector<const char*> requiredInstanceExtensions;
    requiredInstanceExtensions.resize(count_instance_extensions);
    for(int i = 0; i < count_instance_extensions; i++) requiredInstanceExtensions[i] = instance_extensions[i];
}

void CSDLManager::createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface){
    if(!SDL_Vulkan_CreateSurface(window, instance->getHandle(), NULL, &surface)) {
        std::cout << "Could not create a Vulkan surface." << std::endl;
        //return 1;
    }
}


void CSDLManager::eventHandle(){
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        //std::cout << "Debug: while loop: " <<debugCount<< std::endl;
        switch(event.type) {
            case SDL_EVENT_KEY_DOWN:
                //std::cout<<"Key Down"<<event.key.key<<std::endl;
                switch(event.key.key){ //need 6 directions and 6 turns to translate camera
                    case SDLK_LEFT:
                        CApplication::mainCamera.Velocity.x = 1; 
                        break;
                    case SDLK_RIGHT:
                        CApplication::mainCamera.Velocity.x = -1;
                        break;
                    case SDLK_UP:
                        CApplication::mainCamera.Velocity.y = -1;
                        break;
                    case SDLK_DOWN:
                        CApplication::mainCamera.Velocity.y = 1;
                        break;
                    case SDLK_W:
                        CApplication::mainCamera.Velocity.z = 1;
                        break;
                    case SDLK_S:
                        CApplication::mainCamera.Velocity.z = -1;
                        break;
                    case SDLK_A:
                        CApplication::mainCamera.AngularVelocity.y = -100;
                        break;
                    case SDLK_D:
                        CApplication::mainCamera.AngularVelocity.y = 100;
                        break;
                    case SDLK_Q:
                        CApplication::mainCamera.AngularVelocity.z = 100;
                        break;
                    case SDLK_E:
                        CApplication::mainCamera.AngularVelocity.z = -100;
                        break;
                    case SDLK_R:
                        CApplication::mainCamera.AngularVelocity.x = 100;
                        break;
                    case SDLK_F:
                        CApplication::mainCamera.AngularVelocity.x = -100;
                        break;

                    
                    case SDLK_KP_4:
                        CApplication::objectList[0].Velocity.x = -1;
                        break;
                    case SDLK_KP_6:
                        CApplication::objectList[0].Velocity.x = 1;
                        break;
                    case SDLK_KP_5:
                        CApplication::objectList[0].Velocity.z = -1;
                        break;
                    case SDLK_KP_8:
                        CApplication::objectList[0].Velocity.z = 1;
                        break;
                    case SDLK_KP_7:
                        CApplication::objectList[0].Velocity.y = -1;
                        break;
                    case SDLK_KP_9:
                        CApplication::objectList[0].Velocity.y = 1;
                        break;

                    case SDLK_U:
                        CApplication::objectList[0].AngularVelocity.x = 100;
                        break;
                    case SDLK_J:
                        CApplication::objectList[0].AngularVelocity.x = -100;
                        break;
                    case SDLK_I:
                        CApplication::objectList[0].AngularVelocity.y = 100;
                        break;
                    case SDLK_K:
                        CApplication::objectList[0].AngularVelocity.y = -100;
                        break;
                    case SDLK_O:
                        CApplication::objectList[0].AngularVelocity.z = 100;
                        break;
                    case SDLK_L:
                        CApplication::objectList[0].AngularVelocity.z = -100;
                        break;


                    case SDLK_ESCAPE:
                        bStillRunning = false;
                        break; 
                }
                bKeyDown = true;
                break;
            case SDL_EVENT_KEY_UP:
                //std::cout<<"Key Up"<<event.key.scancode<<std::endl;
                switch(event.key.key){
                    case SDLK_LEFT:
                        //CApplication::mainCamera.keys.left = false;
                        //CApplication::mainCamera.SetVelocity(0,0,0);
                        CApplication::mainCamera.Velocity.x = 0;
                        break;
                    case SDLK_RIGHT:
                        //CApplication::mainCamera.keys.right = false;
                        CApplication::mainCamera.Velocity.x = 0;
                        break;
                    case SDLK_UP:
                        //CApplication::mainCamera.keys.up = false;
                        CApplication::mainCamera.Velocity.y = 0;
                        break;
                    case SDLK_DOWN:
                        //CApplication::mainCamera.keys.down = false;
                        CApplication::mainCamera.Velocity.y = 0;
                        break;
                    case SDLK_W:
                        //CApplication::mainCamera.keys.forward = false;
                        CApplication::mainCamera.Velocity.z = 0;
                        break;
                    case SDLK_S:
                        //CApplication::mainCamera.keys.backward = false;
                        CApplication::mainCamera.Velocity.z = 0;
                        break;
                    case SDLK_A:
                        //CApplication::mainCamera.keys.turnLeft = false;
                        CApplication::mainCamera.AngularVelocity.y = 0;
                        break;
                    case SDLK_D:
                        //CApplication::mainCamera.keys.turnRight = false;
                        CApplication::mainCamera.AngularVelocity.y = 0;
                        break;
                    case SDLK_Q:
                        //CApplication::mainCamera.keys.turnLeft = true;
                        CApplication::mainCamera.AngularVelocity.z = 0;
                        break;
                    case SDLK_E:
                        //CApplication::mainCamera.keys.turnRight = true;
                        CApplication::mainCamera.AngularVelocity.z = 0;
                        break;
                    case SDLK_R:
                        //CApplication::mainCamera.keys.turnLeft = true;
                        CApplication::mainCamera.AngularVelocity.x = 0;
                        break;
                    case SDLK_F:
                        //CApplication::mainCamera.keys.turnRight = true;
                        CApplication::mainCamera.AngularVelocity.x = 0;
                        break;

                    case SDLK_KP_4:
                        CApplication::objectList[0].Velocity.x = 0;
                        break;
                    case SDLK_KP_6:
                        CApplication::objectList[0].Velocity.x = 0;
                        break;
                    case SDLK_KP_5:
                        CApplication::objectList[0].Velocity.z = 0;
                        break;
                    case SDLK_KP_8:
                        CApplication::objectList[0].Velocity.z = 0;
                        break;
                    case SDLK_KP_7:
                        CApplication::objectList[0].Velocity.y = 0;
                        break;
                    case SDLK_KP_9:
                        CApplication::objectList[0].Velocity.y = 0;
                        break;

                    case SDLK_U:
                        CApplication::objectList[0].AngularVelocity.x = 0;
                        break;
                    case SDLK_J:
                        CApplication::objectList[0].AngularVelocity.x = 0;
                        break;
                    case SDLK_I:
                        CApplication::objectList[0].AngularVelocity.y = 0;
                        break;
                    case SDLK_K:
                        CApplication::objectList[0].AngularVelocity.y = 0;
                        break;
                    case SDLK_O:
                        CApplication::objectList[0].AngularVelocity.z = 0;
                        break;
                    case SDLK_L:
                        CApplication::objectList[0].AngularVelocity.z = 0;
                        break;
                }
                bKeyDown = false;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                //std::cout<<"Mouse Down: "<<event.pmotion.x<<", "<<event.pmotion.y<<std::endl;
                bMouseDown = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                //std::cout<<"Mouse Up: "<<event.pmotion.x<<", "<<event.pmotion.y<<std::endl;
                bMouseDown = false;
                CApplication::mainCamera.AngularVelocity.x = 0;
                CApplication::mainCamera.AngularVelocity.y = 0;
                bFirstPersonMouseRotate = !bFirstPersonMouseRotate;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                //if(bMouseDown){
                    //SDL_SetRenderTarget(renderer, render_target);
                    //SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 255);
                    //SDL_RenderLine(renderer, previous_touch_x, previous_touch_y, event.pmotion.x, event.pmotion.y);
                    //std::cout<<bMouseDown<<": "<<event.pmotion.x<<", "<<event.pmotion.y<<std::endl;
                    //std::cout<<"Mouse Motion Diff: "<<event.pmotion.x - previous_mouse_x<<", "<<event.pmotion.y - previous_mouse_y<<std::endl;
                //    CApplication::mainCamera.AngularVelocity.x = mouse_sensitive*(previous_mouse_y - event.pmotion.y);
                //    CApplication::mainCamera.AngularVelocity.y = mouse_sensitive*(previous_mouse_x - event.pmotion.x);
                //}
                //previous_mouse_x = event.pmotion.x;
                //previous_mouse_y = event.pmotion.y;

                if(bFirstPersonMouseRotate){
                    CApplication::mainCamera.AngularVelocity.x = mouse_sensitive*(m_windowCenterX - event.pmotion.y);
                    CApplication::mainCamera.AngularVelocity.y = mouse_sensitive*(-m_windowCenterY + event.pmotion.x);
                     
                }
                break;
            case SDL_EVENT_QUIT:
                bStillRunning = false;
                break;
            default:
                // Do nothing.
                break;
        }
    }
}