#ifndef H_CONTROLNODE
#define H_CONTROLNODE

#include "entity.h"
#include "object.h"
#include "textManager.h"

class CApplication;

/******************
* ControlNode
*******************/

class CControlNode : public CEntity{
    
public:
    CControlNode(){}
    ~CControlNode(){}

    //std::string Name;
    CApplication *m_pApp;
    int m_object_count = 0;
    int m_textbox_count = 0;
    int m_light_count = 0;
    std::vector<CObject*> m_pObjects;
    std::vector<CTextbox*> m_pTextboxes;

    void Register(CApplication *p_app);
    void Update(float deltaTime);
    //void Draw();
};

/******************
* PerfMetric
*******************/

class CPerfMetric : public CControlNode{
    //int m_control_id = 0;
public:
    CPerfMetric();
    ~CPerfMetric(){}

    void RegisterObject(int startIndex);
    void RegisterTextbox(int startIndex);
    void Update();
    //void Draw();
};




#endif