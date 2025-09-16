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

    CApplication *m_pApp;
    int m_object_count = 0;
    int m_textbox_count = 0;
    int m_light_count = 0;
    std::vector<CObject*> m_pObjects;
    std::vector<CTextbox*> m_pTextboxes;

    void Register(CApplication *p_app);
    virtual void RegisterObject(int startIndex);
    virtual void RegisterTextbox(int startIndex);
    virtual void Update();
};

/******************
* ControlPerfMetric
*******************/
class CControlPerfMetric : public CControlNode{
public:
    CControlPerfMetric();
    ~CControlPerfMetric(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlAttachment
*******************/
class CControlAttachment : public CControlNode{
public:
    CControlAttachment();
    ~CControlAttachment(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlGraphicsUniform
*******************/
class CControlGraphicsUniform : public CControlNode{
public:
    CControlGraphicsUniform();
    ~CControlGraphicsUniform(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

#endif