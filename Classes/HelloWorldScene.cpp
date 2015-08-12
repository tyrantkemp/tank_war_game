#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
bool ISINMAP=true;

using namespace cocostudio::timeline;

float PI=3.1415927;
std::map<Point,Unit*> mapVec2Unit; //tilepos 和 上面的单位绑定
std::map<int,Unit*> mapTagUnit;   //tag和
std::map<int,Unit*> mapEnTagUnit;
bool isBeenAttacked;

Color3B colorAttack;
std::vector<Point> vecPath;
std::vector<Point> vecAttackArea;
std::vector<Point> vecMoveArea;

bool comeout=false;//默认属性框为未弹出
bool isAIMoveDone;
bool isEnemyMmvoe;
Size m_tilesize;
bool IS_RELATIVE=false;
bool IS_UNIT_TOUCHED=false;
int UNIT_TOUCHED_TAG=-99;
cocos2d::TMXLayer* m_layer;
Size m_layertilesize;
cocos2d::TMXTiledMap* m_map;;
Size vsize;
std::map<string,int> point_sprite;
Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    auto maplayer=Layer::create();
    this->addChild(maplayer,-1);
    
    //添加背景
    bgSprite=Sprite::create("background.png");
    bgSprite->setAnchorPoint(Vec2::ZERO);

    bgSprite->setPosition(Point(-1400,-1200));
    bgOrigin=Vec2(Vec2::ZERO);
    CCLOG("box:(%f,%f) contentsize:(%f,%f)",bgSprite->getBoundingBox().size.width,bgSprite->getBoundingBox().size.height,
          bgSprite->getContentSize().width,bgSprite->getContentSize().height);
    maplayer->addChild(bgSprite);
    
    //添加地图tmx
    m_map=cocos2d::TMXTiledMap::create("tilemap/30*30_45.tmx");
    startpos=Point(-160,-80);
    endpos=Point(-221,-120);
    bgSprite->addChild(m_map,1);
    m_map->setAnchorPoint(Vec2::ZERO);
    //添加地形图层
    m_layer=m_map->getLayer("ground");
    CCLOG("map positon:(%f,%f) anchor:(%f,%f)",m_layer->getPosition().x,m_layer->getPosition().y,m_layer->getAnchorPoint().x,m_layer->getAnchorPoint().y);

    // m_layer->setVisible(true);
    if (!m_layer) {
        CCLOG("no layer found!!!");
        return false;
    }
    m_layertilesize=m_layer->getLayerSize();
    m_tilesize=m_map->getTileSize();
    
    
    m_size=Director::getInstance()->getVisibleSize();//屏幕大小
    vsize=m_map->getContentSize();//地图大小
    
    //添加左边控制层
    auto ctllayer=CSLoader::createNode("Node.csb");
    scroll=dynamic_cast<ScrollView*>(ctllayer->getChildByTag(19));
    addChild(ctllayer,10);
    
    //添加右边控制层
    auto ctlright=CSLoader::createNode("ctl_right.csb");
    scrollright=dynamic_cast<ScrollView*>(ctlright->getChildByTag(5));
    ctlright->setPosition(m_size.width+scrollright->getContentSize().width, 0);
    addChild(ctlright,10);
    
    //添加顶上控制层
    
    
    addtouchmoved();//添加多点触摸监听
    //addtouched();//单点点击监听
    
    //添加回合控制按钮
    auto CBstart=CSLoader::createNode("BtnStart.csb");
    
    CCLOG("cbstart: child %zd",CBstart->getChildrenCount());
    
    CBstart->setPosition(m_size.width/15*14,m_size.height/10*9);
    Button* btnstart=static_cast<ui::Button*>(CBstart->getChildByTag(11));
    if(btnstart){
        btnstart->addTouchEventListener(CC_CALLBACK_2(HelloWorld::btnStartPressed, this));
    }
    else{
        CCLOG("btnstart init error!");
        exit(0);
    }
    
    this->addChild(CBstart,100);
    //添加单位
   autoAddUnit("坦克2_0.png", Point(13,17),TANK_1,0);
    autoAddUnit("步兵3_0.png", Point(15,17),FOOT_1,0);
    autoAddUnit("步兵3_0.png", Point(17,18),FOOT_1,0);
    autoAddUnit("步兵3_0.png", Point(14,17),FOOT_1,0);
    autoAddUnit("步兵3_1.png", Point(20,19), FOOT_1, 1);
    autoAddUnit("坦克2_0.png", Point(23,20), TANK_1, 1);
    autoAddUnit("plane_1.png", Point(14,15), PLANE_1, 0);
    autoAddUnit("plane_1.png", Point(19,19), PLANE_1, 1);
    isEnemyMmvoe=false;

    return true;
}
//添加单位
void HelloWorld::autoAddUnit(const std::string &unitpath, Point relpos,int type,int camp){
    //auto rel_pos=tilepoTorealpos(relpos);//瓦片坐标->像素坐标
    
    if(vsize.width>relpos.x&&vsize.height>relpos.y&&relpos.x>=0&&relpos.y>=0){
        Unit* unit=Unit::create(unitpath, relpos);
        UnitType temp=Unit::getUnitBytype(type);
        unit->initUnit(temp);
        //unit->init2(unit);
        
        //当前节点绑定的血槽
        LoadingBar* m_bloodbar=ui::LoadingBar::create("Life.png");
        m_bloodbar->setPercent(100);
        Size spritesize=unit->getContentSize();
        float factor=spritesize.width/m_bloodbar->getContentSize().width*0.6;
        m_bloodbar->setScaleY(factor);
        m_bloodbar->setScaleX(factor*temp.s_blood/20);
        m_bloodbar->setPosition(Point(spritesize.width/2,spritesize.height));
        unit->m_bloodbar=m_bloodbar;
        unit->addChild(m_bloodbar);
        unit->m_armytype=HelloWorld::getArmyType(type);
        unit->m_type=type;
        mapVec2Unit[unit->m_tilepos]=unit;
        unit->m_tag=m_map->getChildrenCount()+1;
        m_map->addChild(unit,0,(int)unit->m_tag);
        unit->setPosition(unit->m_realpos);
        mapTagUnit[(int)(unit->m_tag)]=unit;
        unit->m_camp=camp;
        if(camp>0){
            mapEnTagUnit[(int)unit->m_tag]=unit;
        }
        unit->m_unit=unit;
        CCLOG("创建单位:%s  攻击力:%f 攻击范围:%d 血量:%f tag:%ld 阵营:%d",unitpath.c_str(),unit->m_damage,unit->m_attakarea,unit->m_blood,unit->m_tag,camp);
    }
    else
    {
        CCLOG("addunit pos out of range！");
        exit(0);
        
    }
    
    
}
int HelloWorld::getArmyType(int type){
    if(type>=10&&type<60){
        return LAND;
    }else if (type>=60&&type<80){
        return AIR;
    }else if (type>=80){
        return SEA;
        
    }
     
  
}
void HelloWorld::addtouchmoved(){
    
    auto lis=EventListenerTouchAllAtOnce::create();
    lis->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
    lis->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
    lis->onTouchesEnded=CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, this);
    
}

void HelloWorld::addtouched(){
    auto lis=EventListenerTouchOneByOne::create();
    lis->onTouchBegan=CC_CALLBACK_2(HelloWorld::touch_began, this);
    lis->onTouchMoved=CC_CALLBACK_2(HelloWorld::touch_move, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, this);
    
}

void HelloWorld::onTouchesBegan(const std::vector<Touch*>& touches, Event *event)
{
   
    
    if(touches.size()>=2){
        
        deltax=(touches[0]->getLocation().x+touches[1]->getLocation().x)/2-m_map->getPositionX();
        deltay=(touches[0]->getLocation().y+touches[1]->getLocation().y)/2-m_map->getPositionY();
  
    }
    else{
        
        auto realtouch=touches[0]->getLocation(); //单点触摸 获得当前触摸位置的 像素坐标
        
        auto tilepos=realposTotilepos(realtouch);//获得瓦片坐标
        
        if(isinMapArea(m_layertilesize, tilepos)){//判断点击是否在地图内
            
            //获得当前瓦片的gid 获取地图移动区域和非移动区域
            int tileGID=m_layer->getTileGIDAt(tilepos);
            CCLOG("touched pos %d %d titleGID:%d point_sprite:%lu ",(int)tilepos.x,(int)tilepos.y,tileGID,point_sprite.size());
            if(tileGID){
                auto val=m_map->getPropertiesForGID(tileGID);
                auto map=val.asValueMap();
                int ismoveble=map["ismove"].asInt();
                if(ismoveble){
                    CCLOG("this unit can move!!");
                    //                //添加单位移动操作******************
                    
                    
                }else
                    CCLOG("this unit cannot move~~!!");
                
            }
     
            if (mapVec2Unit.find(tilepos)==mapVec2Unit.end()) {//点击在瓦片上
                
                //点击瓦片，属性边框收缩
                if (comeout) {
                    scroll->runAction(MoveTo::create(0.2, Vec2(-scroll->getContentSize().width, 0)));
                    scrollright->runAction(MoveTo::create(0.2, Vec2(scrollright->getContentSize().width+m_size.width, 0)));
                    comeout=false;
                }
                
                
                   IS_UNIT_TOUCHED=false;
                if (UNIT_TOUCHED_TAG!=-99) { //如果上一次点击的是单位
                     Unit *unit=mapTagUnit[UNIT_TOUCHED_TAG];
                   if( Unit::isInMoveArea(tilepos)){//如果在移动范围内
             
//                       mapVec2Unit.erase(unit->m_tilepos);
//                       mapVec2Unit[tilepos]=unit;
//              
                    
                       //A星寻路
                       auto unit=mapTagUnit[UNIT_TOUCHED_TAG];
                       auto startpos=unit->m_tilepos;
                       unit->AstartMove(startpos,tilepos);
                       Unit::RecoverMoveArea();
                       
                       
                       //移动完成后 确定攻击范围 此时应该有个时间延迟
                       unit->setAttackArea();

                       unit->m_tilepos=tilepos;
                       unit->m_realpos=tilepoTorealpos(tilepos);
                       vecMoveArea.clear();
                    }
                    
                    unit->runAction(ScaleTo::create(0.2, 1/1.2));
                    
                }
            }
            else{//当前点击为单位
                bool isBeenAttacked=false;//当前点击单位未被攻击；
                Unit* unitNow=mapVec2Unit[tilepos];
                unitNow->m_underAttacked=true;
                int  tag=(int)unitNow->m_tag;//获得当前单位的tag
                CCLOG("当前点击的为单位  tag:%ld",unitNow->m_tag);
                
                //此时属性边框弹出，表示上一个点击为单位
                if (comeout==true) {
                    scroll->runAction(MoveBy::create(0.2, Vec2(-scroll->getContentSize().width, 0)));
                    auto func=[&](float tt){
                    scroll->runAction(MoveBy::create(0.2, Vec2(scroll->getContentSize().width, 0)));
                    };
                    scroll->scheduleOnce(func, 0.2, "rollbackleft");
                    
                    scrollright->runAction(MoveBy::create(0.2, Vec2(scrollright->getContentSize().width,0)));
                    auto func1=[&](float tt){
                    scrollright->runAction(MoveBy::create(0.2, Vec2(-scrollright->getContentSize().width, 0)));
                    };
                    scrollright->scheduleOnce(func1, 0.2, "rollbackright");
                    
                }else{
                    scroll->runAction(MoveBy::create(0.2, Vec2(scroll->getContentSize().width, 0)));
                    scrollright->runAction(MoveBy::create(0.2, Vec2(-scrollright->getContentSize().width, 0)));

                    comeout=true;
                }
                
                //当点击别的单位 原单位效果还原
                if(tag!=UNIT_TOUCHED_TAG){
                    if (UNIT_TOUCHED_TAG!=-99) {
                         Unit* pre_unit=mapTagUnit[UNIT_TOUCHED_TAG];
                        //当前点选单位为敌军 且 上一个点选单位为我军
                        if(unitNow->m_camp!=pre_unit->m_camp){
                            //敌军在攻击范围内 则攻击
                            if(unitNow->isInAttckArea()){
                                unitNow->m_underAttacked=true;
                                isBeenAttacked=true;
                                CCLOG("attck!!!!");

//                                //遭到攻击效果
                                auto preuni=mapTagUnit[UNIT_TOUCHED_TAG];
                                unitNow->unitBeenAttacked(preuni);
                                //反击 反击操作
                              //  preuni->unitBeenAttackedBack(unitNow);
                                
                            }
                            
                        }
                        
                        
        
                       
                        pre_unit->runAction(ScaleTo::create(0.2, 1/1.2));
                        Unit::RecoverMoveArea();
                        Unit::RecoverAttackArea();
                        
                    }
                }
                
                IS_UNIT_TOUCHED=true;
                
          
                if(!isBeenAttacked){
                    UNIT_TOUCHED_TAG=tag;
                unitNow->runAction(ScaleTo::create(0.2, 1.2));//单位选中后 0.2秒内扩大1.2
                }
                
                   unitNow->setMoveArea();

            }
            
        }
        
    }
    
    
}

void HelloWorld::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{
    auto winSize = Director::getInstance()->getWinSize();
    //缩放
    if(touches.size() > 1)
    {

        
        // 得到当前两触摸点
        auto point1 = touches[0]->getLocation();
        auto point2 = touches[1]->getLocation();
        // 计算两点之间得距离
        auto currDistance = point1.distance(point2);
        // 计算两触摸点上一时刻之间得距离
        auto prevDistance = touches[0]->getPreviousLocation().distance(touches[1]->getPreviousLocation());
        // 两触摸点与原点的差向量，pointVec1和pointVec2是相对于bgSprite的位置
        auto pointVec1 = point1  - bgOrigin;
        auto pointVec2 = point2  - bgOrigin;
        // 两触摸点的相对中点
        auto relMidx = (pointVec1.x + pointVec2.x) / 2 ;
        auto relMidy = (pointVec1.y + pointVec2.y) / 2 ;
        // 计算bgSprite的锚点
        auto anchorX = relMidx / bgSprite->getBoundingBox().size.width;
        auto anchorY = relMidy / bgSprite->getBoundingBox().size.height;
        // 相对屏幕的中点
        auto absMidx = (point2.x + point1.x) / 2 ;
        auto absMidy = (point2.y + point1.y) / 2 ;
        
        // 缩放时，为了避免出现空白的区域，需要做以下的边界处理。
        // 当bgSprite快要进入到屏幕时，修改bgSprite的位置（既absMidx和absMidy）。
        if(  bgOrigin.x > 0)
        {
            absMidx -= bgOrigin.x;
        }
        if( bgOrigin.x < -bgSprite->getBoundingBox().size.width + winSize.width )
        {
            absMidx +=  -bgSprite->getBoundingBox().size.width + winSize.width - bgOrigin.x;
        }
        if( bgOrigin.y > 0 )
        {
            absMidy -= bgOrigin.y;
        }
        if( bgOrigin.y < -bgSprite->getBoundingBox().size.height + winSize.height )
        {
            absMidy +=  -bgSprite->getBoundingBox().size.height + winSize.height - bgOrigin.y;
        }
        // 重设bgSprite锚点和位置
        bgSprite->setAnchorPoint(Vec2(anchorX, anchorY));
        bgSprite->setPosition(Vec2(absMidx, absMidy));
        // 根据两触摸点前后的距离计算缩放倍率
        auto scale = bgSprite->getScale() * ( currDistance / prevDistance);
        // 控制缩放倍率在1～4倍之间，最小倍率不能太小，不让背景将不能填充满整个屏幕。
        scale = MIN(1,MAX(0.6, scale));
        bgSprite->setScale(scale);
        // 更新原点位置
        bgOrigin = Vec2(absMidx, absMidy) - Vec2(bgSprite->getBoundingBox().size.width * anchorX, bgSprite->getBoundingBox().size.height * anchorY) ;
        
        // log("world map touches moved: size=%lu, d1=%f , d2=%f",touches.size() , distance1,distance2 );
    }
    else
    {
        
        // 单点时，touches中只有一个Touch对象，所以通过touches[0]就可以得到触摸对象
        auto touch = touches[0];
        // 计算滑动过程中的滑动增量
        auto diff = touch->getDelta();
        // 得到当前bgSprite的位置
        auto currentPos = bgSprite->getPosition();
        // 得到滑动后bgSprite应该所在的位置
        auto pos = currentPos + diff;
        // 得到此刻bgSprite的尺寸
        auto bgSpriteCurrSize = bgSprite->getBoundingBox().size;
        
        //边界控制，约束pos的位置
        pos.x = MIN(pos.x, bgSpriteCurrSize.width * bgSprite->getAnchorPoint().x);
        pos.x = MAX(pos.x, -bgSpriteCurrSize.width + winSize.width + bgSpriteCurrSize.width * bgSprite->getAnchorPoint().x);
        pos.y = MIN(pos.y, bgSpriteCurrSize.height * bgSprite->getAnchorPoint().y);
        pos.y = MAX(pos.y, -bgSpriteCurrSize.height + winSize.height + bgSpriteCurrSize.height * bgSprite->getAnchorPoint().y);
        // 重设bgSprite位置
        bgSprite->setPosition(pos);
        
        // 更新原点位置
        if( pos.x >= bgSpriteCurrSize.width * bgSprite->getAnchorPoint().x
           || pos.x <= -bgSpriteCurrSize.width + winSize.width + bgSpriteCurrSize.width * bgSprite->getAnchorPoint().x)
        {
            diff.x = 0;
        }
        if( pos.y >= bgSpriteCurrSize.height * bgSprite->getAnchorPoint().y
           || pos.y <= -bgSpriteCurrSize.height + winSize.height + bgSpriteCurrSize.height * bgSprite->getAnchorPoint().y)
        {
            diff.y = 0;
        }
        bgOrigin += diff;
    }
}

void HelloWorld::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    // log("world map touches ended: size=%lu", touches.size() );
}




bool HelloWorld::touch_began(cocos2d::Touch *tou, cocos2d::Event *ev){
    return true;
    
}
void HelloWorld::touch_move(cocos2d::Touch *to, cocos2d::Event *e){
    
    
}
void HelloWorld::btnStartPressed(cocos2d::Ref* psender,Widget::TouchEventType type){
    isEnemyMmvoe=true;
    isAIMoveDone=true;
    if(type==Widget::TouchEventType::BEGAN){
    if(!mapEnTagUnit.empty()){//AI每个单位都开始自己移动
        map<int,Unit*>::iterator it=mapEnTagUnit.begin();
        for(;it!=mapEnTagUnit.end();it++){

            Unit::EnActions(it->first, it->second);

        }
    
    }
    }
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif
    
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
