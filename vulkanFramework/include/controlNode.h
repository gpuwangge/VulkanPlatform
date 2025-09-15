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

    std::string Name;

    //void Register(CApplication *p_app);
    void Update(float deltaTime);
    //void Draw();
};

/******************
* PerfMetric
*******************/

class CPerfMetric : public CControlNode{
    int m_control_id = 0;
public:
    int m_object_count = 0;
    int m_textbox_count = 0;
    int m_light_count = 0;

    CObject* m_pObject0;
    //CTextbox *m_pTextbox0;
    //CTextbox *m_pTextbox1;
    std::vector<CTextbox*> m_pTextboxes;

    CPerfMetric();
    ~CPerfMetric(){}

    void RegisterObject(CApplication *p_app, CObject *object0);
    void RegisterTextbox(CApplication *p_app, std::vector<CTextbox> *pTextBoxes, int startIndex);
    void Update(float deltaTime);
    //void Draw();
};




#endif