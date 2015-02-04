#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "contains.h"
using namespace cocos2d;

static bool IS_UNIT_TOUCHED=false;
static int UNIT_TOUCHED_TAG=-99;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void onTouchesMoved(const std::vector<cocos2d::Touch*> &touchs,cocos2d::Event *ev);
    void onTouchesBegan(const std::vector<Touch*>& touches, Event *event);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    void autoAddUnit(const std::string& unitpath,Point relpos);
    
    void addtouchmoved();
    void addtouched();
    bool touch_began(Touch* tou,Event* ev);
    void touch_move(Touch*to,Event*e);
    void unitMove(TMXLayer* layer,Point pos,Node* nd,unsigned int moves);
    void getAvalableMoveArea(std::vector<Point> & vec,Point po,unsigned int moves);
    
    
    //瓦片坐标转为像素坐标
    Point tilepoTorealpos(Point& pos);
    
    //像素坐标转为瓦片坐标
    Point realposTotilepos(Point& pos);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
    cocos2d::TMXTiledMap* m_map;
    cocos2d::TMXLayer* m_layer;
    
    float deltax;
    float deltay;
    Size vsize;
    Size m_size;
    Size m_layertilesize;
    Size m_tilesize;

    
    Point startpos;
    Point endpos;
    
    std::map<string,int> point_sprite;
    
};

#endif // __HELLOWORLD_SCENE_H__
