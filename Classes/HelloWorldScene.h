#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "contains.h"
#include<thread>
#include<unistd.h>
#include"Unit.h"
using namespace cocos2d;

using namespace cocos2d;
using namespace std;

class Unit;

extern bool isAIMoveDone;
extern bool isBeenAttacked;
extern Color3B colorAttack;
extern std::vector<Point> vecPath; //存储行动路径
extern std::vector<Point> vecMoveArea;
extern std::vector<Point> vecAttackArea;
extern bool IS_RELATIVE;  //当前点坐标是否是相对父节点 true:相对  falsel:不想对
extern bool IS_UNIT_TOUCHED; //上一次点击对象是否是uni
extern int UNIT_TOUCHED_TAG; //上次点击对象的tag
extern  cocos2d::TMXLayer* m_layer; //图层
extern  Size m_layertilesize; //图层包含的图块数量
extern cocos2d::TMXTiledMap* m_map;; //地图
extern Size vsize;            //地图大小
extern Size m_tilesize;        //图块大小
extern std::map<Point,Unit*> mapVec2Unit; //tilepos 和 上面的单位绑定
extern std::map<int,Unit*> mapTagUnit;   //tag和
extern bool isEnemyMmvoe; //是否由敌军移动
extern std::map<int,Unit*> mapEnTagUnit; //敌军 的tag 和 单位绑定
extern  bool comeout;//默认属性框为未弹出


extern float PI;
class HelloWorld : public cocos2d::Layer
{
public:

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    //添加多点触摸监听
    void addtouchmoved();
    void onTouchesMoved(const std::vector<cocos2d::Touch*> &touchs,cocos2d::Event *ev);
    void onTouchesBegan(const std::vector<Touch*>& touches, Event *event);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    
    
    //添加单位
    void autoAddUnit(const std::string& unitpath/*单位图片名称，唯一*/,Point relpos/*添加单位的瓦片坐标*/,int type,int camp);
    
    Unit* getUnit(const std::string& unitpath/*单位图片名称，唯一*/,Point relpos/*添加单位的瓦片坐标*/,int type);
    
    //单点触摸监听
    void addtouched();
    bool touch_began(Touch* tou,Event* ev);
    void touch_move(Touch*to,Event*e);
    //初始化MapUnitProperty 单位名称和unittype对应关系
    void initunityproperty();
    
    void btnStartPressed(cocos2d::Ref* psender,Widget::TouchEventType type);
  
    
    static int getArmyType(int type);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
    Sprite* bgSprite;
    Vec2 bgOrigin;
    float deltax;
    float deltay;
    Size m_size; //director::getinstance->getvisablesize()
    Point startpos;
    Point endpos;
    ScrollView* scroll;
    ScrollView* scrollright;
    ScrollView* scrolltop;
    
    
};

#endif // __HELLOWORLD_SCENE_H__
