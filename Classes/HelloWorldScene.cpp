#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
bool ISINMAP=true;

using namespace cocostudio::timeline;
#define PI 3.1415927
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
    //auto mainnode=CSLoader::createNode("MainScene.csb");
    
    
    
    
    //CCLOG("asin:%f",asin(1/sqrt(2.0))*180/PI);
    
    
    auto bg=Sprite::create("background.png");
    bg->setAnchorPoint(Vec2::ZERO);
    
    addChild(bg,-1);
    
    
    m_map=cocos2d::TMXTiledMap::create("tilemap/30*30_45.tmx");
    startpos=Point(-160,-80);
    
    
    endpos=Point(-221,-120);
    
    m_layer=m_map->getLayer("ground");
    // m_layer->setVisible(true);
    
    if (!m_layer) {
        CCLOG("no layer found!!!");
        return false;
    }
    m_layertilesize=m_layer->getLayerSize();
    m_tilesize=m_map->getTileSize();
    
    
    
    
    // CCLOG("layer szie in tiles:x:%f y:%f",m_layer->getLayerSize().width,m_layer->getLayerSize().height);
    
    
    m_map->setAnchorPoint(Point(0.3,0.3));//锚点设置在中心才能缩放正常
    
    CCLOG("anchor x:%f y:%f",m_map->getAnchorPointInPoints().x,m_map->getAnchorPointInPoints().y);
    m_size=Director::getInstance()->getVisibleSize();
    
    
    vsize=m_map->getContentSize();
    
    //auto sizemap=m_map->getContentSize();
    //CCLOG("map size.x=%f, size.y=%f,map positon:x:%f y:%f",vsize.width,vsize.height,m_map->getPositionX(),m_map->getPositionY());
    
    
    addtouchmoved();//多点触摸监听
    //addtouched();//单点点击监听
    
    
    //坐标转换测试
    //    auto po=Point(5,7);
    //    auto pos=tilepoTorealpos(po);
    //    CCLOG("tile 0,0 realpos:x:%f y:%f ",pos.x,pos.y);
    //    auto res=Point(600,520);
    //
    //    auto reser_pos=realposTotilepos(res);
    //    CCLOG("tile x:%f y:%f",reser_pos.x,reser_pos.y);
    
    
    // m_map->setPosition(Point(-sizemap.width/2,0));
    addChild(m_map,1);
    
    
    
    //添加单位
    autoAddUnit("坦克2.png", Point(14,15));
    autoAddUnit("步兵3.png", Point(15,16));
    autoAddUnit("步兵3.png", Point(17,18));
    
    
    
    
    return true;
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
    //查看日志,在多点触摸时，只有一个点
    
    if(touches.size()>=2){
        //  log("world map touches began: size=%lu", touches.size() );
        
        deltax=(touches[0]->getLocation().x+touches[1]->getLocation().x)/2-m_map->getPosition().x;
        deltay=(touches[0]->getLocation().y+touches[1]->getLocation().y)/2-m_map->getPosition().y;
        
        
        //        auto scalepointx=(touches[0]->getLocation().x+touches[1]->getLocation().x)/2;
        //        auto scalepointy=(touches[0]->getLocation().y+touches[1]->getLocation().y)/2;
        //
        //
        //
        //        CCLOG("touch0 x:%f touch0 y:%f  touch1 x:%f touch1 y:%f",touches[0]->getLocation().x,touches[0]->getLocation().y,
        //              touches[1]->getLocation().x,touches[1]->getLocation().y);
        //
        //        auto newscalex=scalepointx/vsize.width;
        //        auto newscaley=scalepointy/vsize.height;
        //
        //        CCLOG("scalepoint x:%f y:%f enwscalex:%f newsscaley:%f",scalepointx,scalepointy,newscalex,newscaley);
        //
        //        m_map->setAnchorPoint(Point(newscalex,newscaley));
        // CCLOG("touches began: deltax:%f deltay:%f",deltax,deltay);
        
    }
    else{
        //单点触摸 获得当前触摸位置的 瓦片信息
        auto realtouch=touches[0]->getLocation();
        auto tilepos=realposTotilepos(realtouch);
        
        if(isinMapArea(m_layertilesize, tilepos)){//判断点击是否在地图内
            
            
            int tileGID=m_layer->getTileGIDAt(tilepos);
            CCLOG("touched pos %d %d titleGID:%d",(int)tilepos.x,(int)tilepos.y,tileGID);
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
            CCLOG("tilepos x:%f y:%f",tilepos.x,tilepos.y);
            
            string strtilepos=pointTostring(tilepos);
            CCLOG("point_sprite.size:%lu",point_sprite.size());
            if (point_sprite.find(strtilepos)==point_sprite.end()) {
                IS_UNIT_TOUCHED=false;
                if (UNIT_TOUCHED_TAG!=-99) {
                    Node* pre_unit=m_map->getChildByTag(UNIT_TOUCHED_TAG);
                    pre_unit->runAction(ScaleTo::create(0.2, 1/1.2));
                    
                }
            }
            else{
                
                int tag=point_sprite.find(strtilepos)->second;
                CCLOG("node found! tag:%d",tag);
                //当点击别的单位或者点击非单位瓦片时 原单位大小还原
                if(tag!=UNIT_TOUCHED_TAG){
                    if (UNIT_TOUCHED_TAG!=-99) {
                        Node* pre_unit=m_map->getChildByTag(UNIT_TOUCHED_TAG);
                        pre_unit->runAction(ScaleTo::create(0.2, 1/1.2));
                        
                    }
                }
                
                 IS_UNIT_TOUCHED=true;
                 UNIT_TOUCHED_TAG=tag;
                
                
                Node *unit=m_map->getChildByTag(tag);
                if(!unit)
                    CCLOG(" unit does init!! m_map children size:%zd",m_map->getChildrenCount());
                auto scaleto=ScaleTo::create(0.2, 1.2);
                
                unitMove(m_layer, tilepos, unit, 1);
                
                
                unit->runAction(scaleto);
              
            }
                
        }
        
    }
    //}
    
}

void HelloWorld::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{
    //缩放
    if(touches.size() > 1)
    {
        //此处注意缩放时两个触点中心和锚点差值应该保持不变 这样缩放才正常
        
        
        
        
        auto distance1 = touches[0]->getPreviousLocation().distance(touches[1]->getPreviousLocation());
        auto distance2 = touches[0]->getLocation().distance(touches[1]->getLocation());
        
        float scale = m_map->getScale() * ( distance2 / distance1);
        scale = MIN(1,MAX(0.8, scale));//最大扩大2倍 最小0.5
        auto newx=(touches[0]->getLocation().x+touches[1]->getLocation().x)/2-deltax;
        auto newy=(touches[0]->getLocation().y+touches[1]->getLocation().y)/2-deltay;
        
        
        m_map->setPosition(newx, newy);
        m_map->setScale(scale);
        
        deltax=(touches[0]->getLocation().x+touches[1]->getLocation().x)/2-m_map->getPosition().x;
        deltay=(touches[0]->getLocation().y+touches[1]->getLocation().y)/2-m_map->getPosition().y;
        
        
        
        // log("world map touches moved: size=%lu, d1=%f , d2=%f",touches.size() , distance1,distance2 );
    }
    else
    {
        
        
        
        //移动地图r
        auto touchnow=touches[0];
        auto diff=touchnow->getDelta();
        auto curr=m_map->getPosition();
        auto sizechange=m_map->getContentSize();
        
        //   CCLOG("map_size:%f %fcurr: x:%f y:%f diff:x:%f y:%f",sizechange.width,sizechange.height,curr.x,curr.y,diff.x,diff.y);
        
        
        //        if (curr.x>startpos.x||curr.y>startpos.y) {
        //            m_map->setPosition(startpos);
        //
        //        }else if (curr.x<endpos.x||curr.y<endpos.y){
        //            m_map->setPosition(endpos);
        //
        //        }
        //
        //        else {
        //        m_map->setPosition(curr+diff);
        //        }
        m_map->setPosition(curr+diff);
    }
}

void HelloWorld::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    // log("world map touches ended: size=%lu", touches.size() );
}


void HelloWorld::autoAddUnit(const std::string &unitpath, Point relpos){
    auto rel_pos=tilepoTorealpos(relpos);
    // CCLOG("rel_pos:x:%f y:%f  maptilessize: %f y:%f",rel_pos.x,rel_pos.y,m_map->getTileSize().width,m_map->getTileSize().height);
    
    
    
    if(vsize.width>relpos.x&&vsize.height>relpos.y){
        auto unit=Sprite::create(unitpath);
        
        auto tag=m_map->getChildrenCount()+1;
        CCLOG("tag:%ld",tag);
        
        m_map->addChild(unit,2,(int)tag);
        string ss=pointTostring(relpos);
        
        
        point_sprite.insert(make_pair(ss, tag));
        
        
        
        
        unit->setPosition(rel_pos);
        
        
        
        
        //        int tileGID=0;
        //        tileGID=m_layer->getTileGIDAt(relpos);
        //        CCLOG("gid %d",tileGID);
        //
        //        if(tileGID){
        //            ValueMap temp;
        //            temp["isMmoveble"]=true;
        //
        //            temp["isMask"]=false;
        //            temp["unit_type"]=unitpath;
        //
        //
        //
        //        }else
        //            CCLOG("add unit-: no gid added!!!");
        //
        
        
        
        
    }
    else
    {
        CCLOG("addunit pos out of range！");
        exit(0);
        
    }
    
    
}

bool HelloWorld::touch_began(cocos2d::Touch *tou, cocos2d::Event *ev){
    return true;
    
}
void HelloWorld::touch_move(cocos2d::Touch *to, cocos2d::Event *e){
    
    
}
//瓦片坐标 -> 像素坐标  改写 引擎 api
Point HelloWorld::tilepoTorealpos(Point& pos){
    
    //以下是45°角交错
    //    float diffX = 0;
    //    if ((int)pos.y % 2 == 0)
    //    {
    //        diffX = m_tilesize.width/2;//m_tilesize 瓦片size
    //    }
    //    return Vec2(pos.x * m_tilesize.width + diffX,
    //                (m_layertilesize.height - pos.y ) * m_tilesize.height/2);//m_layertilesize 瓦片个数
    
    //以下是正常45角
    return  m_layer->getPositionAt(Point(pos.x,pos.y-1));
    
    
}

//像素坐标 -> 瓦片坐标
Point HelloWorld::realposTotilepos(Point& pos){
    
    
    //检查 点击点是否在地图坐表内
    
    
    
    auto relative_pos=m_map->convertToNodeSpace(pos);//从当前屏幕坐标获取相对于地图的坐标
    float x=relative_pos.x;
    float y=relative_pos.y;
    double xie_xian=sqrt(m_tilesize.height*m_tilesize.height/4+m_tilesize.width*m_tilesize.width/4);
    
    
    double angleA=asin((double)m_tilesize.height/2/xie_xian)*180/PI;
    double x_distance=0;
    if (x>vsize.width/2) {
        x_distance=x-vsize.width/2;
    }else{
        x_distance=vsize.width/2-x;
    }
    double y_distance=vsize.height-y;
    double distance=sqrt(x_distance*x_distance+y_distance*y_distance);
    double angle_ACE=asin((double)y_distance/distance)*180/PI;
    double angle_ACD=angle_ACE-angleA;
    double angle_CAD=180-2*angleA-angle_ACD;
    
    double dis_AD=(double)distance/sin(2*angleA*PI/180)*sin(angle_ACD*PI/180);
    double dis_CD=(double)distance/sin(2*angleA*PI/180)*sin(angle_CAD*PI/180);
    int d2x=0;
    int d2y=0;
    
    if (x>vsize.width/2) {
        d2x=dis_CD/xie_xian;
        d2y=dis_AD/xie_xian;
    }else{
        d2y=dis_CD/xie_xian;
        d2x=dis_AD/xie_xian;
    }
    
    return  Point(d2x,d2y);
    
    
    
}


void HelloWorld::unitMove(cocos2d::TMXLayer *layer, Point pos, cocos2d::Node *nd,unsigned int steps){
    
    std::vector<Point> vec_pos;
    getAvalableMoveArea(vec_pos,pos,steps);
    for (int i=0;i<vec_pos.size();i++) {
        auto pos=vec_pos.at(i);
        
        auto sp=layer->getTileAt(pos);
        if(sp){
        sp->runAction(Blink::create(4, 15));
        }
    }
    
}
void HelloWorld::getAvalableMoveArea(std::vector<Point> &vec, Point pos,unsigned int steps){
    int i=0;
    int j=0;
    
    for (i=pos.x-steps; i<=pos.x+steps; i++) {
        for (j=pos.y-steps; j<=pos.y+steps; j++) {
           if(isinMapArea(m_layertilesize, Point(i,j)))
               vec.push_back(Point(i,j));
              // .push_Back(Point(i,j));
                         
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
