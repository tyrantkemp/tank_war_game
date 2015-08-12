//
//  Unit.cpp
//  liuianxing1
//
//  Created by 肖准 on 15/2/11.
//
//

#include "Unit.h"
#include <algorithm>

Unit::Unit(Point& tilepos):Sprite(),m_tilepos(tilepos){
    
    
}
Unit::~Unit(){
    
    mapTagUnit.erase((int)m_tag);
    std::map<Point,Unit*>::iterator it=mapVec2Unit.find(m_tilepos);
    if(it!=mapVec2Unit.end()){
      mapVec2Unit.erase(it);
    }
    //mapVec2Unit.erase(m_unit->m_tilepos);
    
    if(m_camp>0)
        mapEnTagUnit.erase((int)m_tag);
    m_unit=NULL;
   //Sprite::~Sprite();
    
}
/**
 生成单位后初始化单位的重要属性
 */
void Unit::initUnit(UnitType s_type){
    
    m_dead=false;
    
    m_underAttacked=false;
    
    
    m_defense=s_type.s_defense;
    m_blood=s_type.s_blood;   //原始血量
    m_curblood=s_type.s_blood;  //当前血量
    m_damage=s_type.s_damage;    //伤害值
    m_movearea=s_type.s_movearea;  //移动范围
    m_attakarea=s_type.s_attakarea;  //攻击范围
    
    
    m_realpos=tilepoTorealpos(m_tilepos);
    m_strpos=pointTostring(m_tilepos);
    
    
    
    
}

/**
 *  Unit工厂函数
 *
 *  @param filename 创建的单位路径
 *  @param tilepos  创建单位的瓦片坐标
 *
 *  @return Unit
 */
Unit* Unit::create(const std::string& filename,Point& tilepos){
    
    Unit *ret =new Unit(tilepos);
    if(ret!=nullptr&&ret->initWithFile(filename)){
        ret->autorelease();
        
        // return ret;
    }else{
        CC_SAFE_DELETE(ret);
        
    }
    
    return ret;
}
/**
 *  方法函数：设置当前单位的行动范围
 */
void Unit::setMoveArea(){
    vecMoveArea.clear();
    
    Point tilepos=this->m_tilepos;
    //CCLOG("pos %f %f --tilepos: x:%f y:%f",pos.x,pos.y,tilepos.x,tilepos.y);
    
    int steps=m_unit->m_movearea;
    int i=0;
    int j=0;
    
    for (i=tilepos.x-steps; i<=tilepos.x+steps; i++) {
        for (j=tilepos.y-steps; j<=tilepos.y+steps; j++) {
            if(isinMapArea(m_layertilesize, Point(i,j)))
                vecMoveArea.push_back(Point(i,j));
            // .push_Back(Point(i,j));
            
        }
    }
    
    for (int i=0;i<vecMoveArea.size();i++) {
        auto pos=vecMoveArea[i];
        
        auto sp=m_layer->getTileAt(pos);
        if(sp){
            //sp->setColor(ccc3(0, 0, 0));
            sp->setOpacity(200);
            
            //CCLOG("opc: %d",opc);
            //sp->runAction(Blink::create(4, 15));
        }
    }
    
    
    
}
/**
 *  还原移动范围效果
 */
void Unit::RecoverMoveArea(){
    for (int i=0;i<vecMoveArea.size();i++) {
        auto pos=vecMoveArea[i];
        
        auto sp=m_layer->getTileAt(pos);
        if(sp){
            //sp->setColor(ccc3(0, 0, 0));
            sp->setOpacity(-1);
            
            //CCLOG("opc: %d",opc);
            //sp->runAction(Blink::create(4, 15));
        }
    }
}


/**
 *  设置攻击范围效果
 */
void Unit::setAttackArea(){
    vecAttackArea.clear();
    
    int i=0;
    int j=0;
    
    for (i=m_tilepos.x-m_attakarea; i<=m_tilepos.x+m_attakarea; i++) {
        for (j=m_tilepos.y-m_attakarea; j<=m_tilepos.y+m_attakarea; j++) {
            if(isinMapArea(m_layertilesize, Point(i,j)))
                vecAttackArea.push_back(Point(i,j));
            // .push_Back(Point(i,j));
            
        }
    }
    
    for (int i=0;i<vecAttackArea.size();i++) {
        auto pos=vecAttackArea[i];
        
        auto sp=m_layer->getTileAt(pos);
        if(sp){
            //sp->setColor(ccc3(0, 0, 0));
            colorAttack= sp->getColor();
            sp->setOpacity(200);
            
            sp->setColor(cocos2d::Color3B::RED);
            
            
            //CCLOG("opc: %d",opc);
            // sp->runAction(Blink::create(2, 15));
        }
    }
    
}
void Unit::RecoverAttackArea(){
    for (int i=0;i<vecAttackArea.size();i++) {
        auto pos=vecAttackArea[i];
        
        auto sp=m_layer->getTileAt(pos);
        if(sp){
            //sp->setColor(ccc3(0, 0, 0));
            sp->setOpacity(-1);
            sp->setColor(colorAttack);
            //CCLOG("opc: %d",opc);
            //sp->runAction(Blink::create(4, 15));
        }
    }
}
bool Unit::isInAttckArea(){
    
    auto uni=mapTagUnit[UNIT_TOUCHED_TAG];
    int attckarea=uni->m_attakarea;
    
    Point pretilepos=uni->m_tilepos;
    
    int x=pretilepos.x;
    int y=pretilepos.y;
    if((x+attckarea>=m_unit->m_tilepos.x)&&(m_unit->m_tilepos.x>=x-attckarea)&&(y+attckarea>=m_unit->m_tilepos.y)&&(m_unit->m_tilepos.y>=y-attckarea)){
        return  true;
    }else
        return  false;
    
}



/**
 *  空中单位攻击，选择飞机停靠位置，即在目标单位周边一环8个位置任选一个没有单位的位置
 *
 *  @param target 目标单位位置
 *
 *  @return 飞机停靠单位位置
 */
Point Unit::getAirPoint(Point& target){
    Point pp;
    
    vector<Point> poss,outpos;
     poss.push_back(Point(target.x-1,target.y-1));
     poss.push_back(Point(target.x,target.y-1));
     poss.push_back(Point(target.x+1,target.y-1));
     poss.push_back(Point(target.x-1,target.y));
     poss.push_back(Point(target.x+1,target.y));
     poss.push_back(Point(target.x-1,target.y+1));
     poss.push_back(Point(target.x,target.y+1));
     poss.push_back(Point(target.x+1,target.y+1));
    
    for (int i=0; i<poss.size(); ++i) {
        if(isinMapArea(m_layertilesize, poss[i])&&(mapVec2Unit.find(poss[i])==mapVec2Unit.end())){
            outpos.push_back(poss[i]);
        }

    }
    //从符合条件的停靠位置中随机选择一个位置
    srand((int)time(NULL));
    if (outpos.size()>0) {
        int j=rand()%outpos.size();
        return outpos[j];
    }else{
        return Vec2::ZERO;
        CCLOG("错误！！没有合适的停靠位置");
    }
    
    
}
void Unit::unitBeenAttacked(Unit* preuni,bool isActive){
   
    // 攻击方或被攻击方已挂 直接跳出
    if(preuni->m_dead||m_dead)
        return;
    //正常状况下 普通地面及海上单位无法攻击空中单位;普通海上单位和陆地单位不能互相攻击
    if(((preuni->m_armytype==LAND||preuni->m_armytype==SEA)&&m_armytype==AIR)||
       (preuni->m_armytype==LAND&&m_armytype==SEA)||(preuni->m_armytype==SEA&&m_armytype==LAND)){
        return;
    }
    
    Point startpos=preuni->m_tilepos;
    Point realstart=tilepoTorealpos(startpos);
    Point endpos;
    if (preuni->m_armytype==AIR) {
        //攻击方为空中单位时 不用遵循A星寻路算法
        
        endpos=getAirPoint(m_tilepos);
        Point realpos=tilepoTorealpos(endpos);
        preuni->runAction(MoveTo::create(0.2f, realpos));
        
    }
   
    //检查被攻击单位是否在攻击单位攻击范围内
    int attckarea=preuni->m_attakarea;
    Point pretilepos=preuni->m_tilepos;
    int x=pretilepos.x;
    int y=pretilepos.y;
    if((x+attckarea>=m_unit->m_tilepos.x)&&(m_unit->m_tilepos.x>=x-attckarea)&&(y+attckarea>=m_unit->m_tilepos.y)&&(m_unit->m_tilepos.y>=y-attckarea)){
        ;
    }else
        return;
    
    
    float damage=preuni->m_damage;
    damage=isActive?damage:(damage/2);//判断是主动攻击还是被动攻击 被动攻击攻击力减半
    float blood=m_curblood-damage;
    if(!m_bloodbar){
        CCLOG("m_bloodbar 不存在!!!");
        exit(0);
    }
    
    showBloodReduce(damage);

    if(blood>0&&!m_dead){
                CCLOG("blood:%f constblood:%f percent:%f damage:%f",blood,m_blood,(float)blood/m_blood*100,damage);
        m_bloodbar->setPercent((float)blood/m_blood*100);
        m_curblood=blood;
    }else{
        //单位死亡
        CCLOG("单位死亡！！！");
        m_dead=true;
    }
    
    
    auto act1=MoveBy::create(0.02, Point(20,0));
    auto act2=MoveBy::create(0.05, Point(-40,0));
    auto act3=MoveBy::create(0.05, Point(40,0));
    auto act4=MoveBy::create(0.02, Point(-20,0));
    Vector<FiniteTimeAction*> accarr;
    accarr.pushBack(act1);
    accarr.pushBack(act2);
    accarr.pushBack(act3);
    accarr.pushBack(act4);
    
    auto act=Sequence::create(accarr);
    
  
    
    if(m_dead){
        auto ac=cocos2d::FadeTo::create(1, 20);
        auto ac2=cocos2d::FadeTo::create(1, 20);
        m_unit->runAction(ac);
        m_bloodbar->runAction(ac2);
        //延时效果
        
        auto f=[&](float a){
            m_unit->removeFromParent();
        };
        m_unit->scheduleOnce(f, 1.0f,"ss");
    
        UNIT_TOUCHED_TAG=(int)preuni->m_tag;
    }else{
        m_unit->runAction(act);;
    }
    //空中单位在攻击完毕后返回机场
    auto func=[=](float tt){
        
        if (preuni->m_armytype==AIR) {
            preuni->runAction(MoveTo::create(0.2f, realstart));
//            std::map<Point,Unit*>::iterator it=mapVec2Unit.find(preuni->m_tilepos);
//            if(it!=mapVec2Unit.end()){
//                mapVec2Unit.erase(it);
//            }
//           // mapVec2Unit.erase(preuni->m_tilepos);
//            mapVec2Unit[startpos]=preuni;
//            preuni->m_tilepos=startpos;
        }
    };
    preuni->scheduleOnce(func, 0.8f, "fly_back");
   
    
}
/**
 *  伤害悬浮显示
 *
 *  @param reduceBlood 单位收到攻击后减少的血量
 */
void Unit::showBloodReduce(float reduceBlood){
    stringstream ss;
    ss<<reduceBlood;
    string tex;
    ss>>tex;
    tex="-"+tex;
    
    auto ttf=LabelTTF::create(tex,"Arial",25);
    ttf->setColor(cocos2d::Color3B::RED);
    m_unit->addChild(ttf);
    auto unitszize=m_unit->getContentSize();
    ttf->setPosition(unitszize.width/2,unitszize.height/5*3);
    auto ac1=FadeTo::create(1.5, 20);
    auto ac2=MoveBy::create(1.5, Point(0,unitszize.height/5*4));
    ttf->runAction(ac1);
    ttf->runAction(ac2);
    
    auto f=[=](float dt){
        ttf->runAction(FadeTo::create(1, 20));
        ttf->removeFromParent();
    };
    ttf->scheduleOnce(f, 1.0f,"s");
    
    


}

void Unit::unitBeenAttackedBack(Unit* preuni){
    
    auto func=[=](float tt){
        m_unit->unitBeenAttacked(preuni,false);
    };
        m_unit->scheduleOnce(func, 0.7f, "fanji");
    
}

void Unit::AstartMove(Point&startpos,Point&endpos){
    
    vecPath.clear(); //行动路径清空
    vector<SearchNode> openlist;//待检测点队列
    vector<SearchNode> closelist;//已经检测的点队列
    vector<SearchNode> aroundNodes;
    
    SearchNode beginNode= SearchNode(startpos);
    CCLOG("startpos: x:%f Y:%f  endpos: x:%f y:%f",startpos.x,startpos.y,endpos.x,endpos.y);
    openlist.push_back(beginNode);
    beginNode.mIsVisited=true;
    //待检测队列不为空 且 被检测队列中不包含终点坐标
    while (!openlist.empty()&&!Unit::isContainObjectNode(openlist,endpos)) {
        //从openlist中获取权值最小的节点 并在openlist中删除该节点
        SearchNode pNode=Unit::getSmallvalueandDelete(openlist);//获取待选节点中权值最小的节点并从待选队列中删除
        CCLOG("pathnode:x: %f y:%f",pNode.mPoint.x,pNode.mPoint.y);
   
        closelist.push_back(pNode);
        
        for (int i=0; i<=7; i++) { //八个移动方向
            
            Point adjpoint=Point(Vec2::ZERO);
            bool hasadj=Unit::GetAdjPoint(pNode.mPoint,i,adjpoint);//获得周边节点
            if(hasadj==false){ //相邻瓦片必须在图层范围内
                continue;
            }
            if(Unit::IsSearchNodeArrive(adjpoint,endpos) == false || Unit::isContainObjectNode(closelist, adjpoint)== true
               /* ||isNotRightDown(adjpoint,pNode.mPoint,endpos)==false*/){ //临点如果不可达或者已经访问过 则回调继续
                continue;
            }
          
            //如果邻点不在openlist中
            if(!Unit::isContainObjectNode(openlist, adjpoint)){
                SearchNode psearchNode=SearchNode(adjpoint);
                psearchNode.ParentNode=&pNode;
                psearchNode.mHValue=Unit::getHvalue(adjpoint,endpos);
                psearchNode.mGValue=pNode.mGValue+getGvalue(adjpoint,pNode.mPoint);
                psearchNode.mFvalue=psearchNode.mHValue+psearchNode.mGValue;
                openlist.push_back(psearchNode);
                psearchNode.mIsVisited=true;
                
            }else{
                //邻接点在openlist中
                //检查邻接点到当前点的
                SearchNode curNode=Unit::GetSearchNodeFromOpenLis(adjpoint,openlist);
                double tempGvalue=Unit::getHvalue(adjpoint,pNode.mPoint);
                
                if(tempGvalue+pNode.mGValue<curNode.mGValue){
                    curNode.ParentNode=&pNode;
                    curNode.mGValue=tempGvalue+pNode.mGValue;
                    curNode.mFvalue=curNode.mGValue+curNode.mHValue;
                    
                }
       
            }
            
        }
        
    }
    
    //最后从目标格开始, 沿着每一格的父节点移动直到回到起始格, 这就是路径.
    SearchNode end=Unit::GetSearchNodeFromOpenLis(endpos, openlist);
    while(end.ParentNode!=nullptr){
        vecPath.push_back(end.mPoint);
        end=*end.ParentNode;
        CCLOG("parantnode:(%f,%f)",end.mPoint.x,end.mPoint.y);
    
    }
    reverse(vecPath.begin(), vecPath.end());
    
    
    
    
   //此时要保证endpos不会重叠 尤其对于AI移动来说 当目标瓦片已经被别的单位占领时 则只行动到理论行动路线的上一步
    if(!mapVec2Unit[endpos]){
      vecPath.push_back(endpos);
    }else{
        endpos=vecPath[vecPath.size()-1];
    }
    vecPath.push_back(endpos);
    vector<Point> vecRealPath;
    for (vector<Point>::iterator it=vecPath.begin(); it!=vecPath.end(); it++) {
        CCLOG("path:(%f,%f)",it->x,it->y);
        vecRealPath.push_back(tilepoTorealpos(*it));
    }
  
    
    int speed;
    speed=(m_unit->m_camp>0)?AI_SPEED:NM_SPEED;
    totaltime=0.0;
    auto m_startpo=vecRealPath.at(0);
    Vector<FiniteTimeAction* > accarrtemp;
    for (int i=1; i<vecRealPath.size(); ++i) {
        auto duration=vecRealPath[i-1].getDistance(vecRealPath[i]);
        auto time=duration/speed;
        totaltime+=time;
        accarrtemp.pushBack(MoveTo::create(time, vecRealPath[i]));
        
    }
    auto m_accarr=Sequence::create(accarrtemp);
    
    //更新mapvec2unit
    std::map<Point,Unit*>::iterator it=mapVec2Unit.find(m_tilepos);
    if(it!=mapVec2Unit.end()){
        mapVec2Unit.erase(it);
    }
     // mapVec2Unit.erase(m_unit->m_tilepos);
        mapVec2Unit[endpos]=m_unit;
        m_unit->m_tilepos=endpos;
    if(m_accarr){ //在移动路线不为空的情况下
     m_unit->runAction(m_accarr);
    }
    CCLOG("started running······");
    
}


bool isNotRightDown(Point& adjpoint,Point& pnode,Point&endpos){
    if((adjpoint.x+1==pnode.x)&&(adjpoint.y==endpos.y)&&(endpos.x>pnode.x)){
        return  false;
    }else
        return  true;
    
}

bool isSlash(Point& apoint,Point& bpoint){
    if (apoint.x-bpoint.x==apoint.y-bpoint.y) {
        return  true;
    }else
        return  false;
    
}
SearchNode Unit::GetSearchNodeFromOpenLis(Point & adjpoint,vector<SearchNode>& openlist){
    //SearchNode objecpos=SearchNode(adjpoint);
    
    for (vector<SearchNode>::iterator it=openlist.begin(); it!=openlist.end(); it++) {
        if(it->mPoint==adjpoint)
            return  *it;
    }
    
    
    
}
/**
 *  获得邻接点和父节点之间的G值，垂直水平方向为1，斜线方向为1.4
 *
 *  @param adjpos
 *  @param curpos
 *
 *  @return
 */
double Unit::getGvalue(Point& adjpos,Point& curpos){
    if(adjpos.x==curpos.x||adjpos.y==curpos.y){
        return 1.0;
    }else
        return 1.4;
}
double Unit::getHvalue(Point& adjpos,Point& endpos){
    // return abs( adjpos.x-endpos.x)+abs(adjpos.y-endpos.y);
    return  (double)hypot(abs( adjpos.x-endpos.x), abs(adjpos.y-endpos.y));
}

bool Unit::IsSearchNodeArrive(Point & tilepo,Point& endpos){
    
    
    if(tilepo==endpos)
        return true;
    
    //判断当前瓦片是否可达
    
    map<Point,Unit*>::iterator it =mapVec2Unit.find(tilepo);
    if(it!=mapVec2Unit.end()){
        return  false;
    }else {
        
        return  true;
    }
    
}


bool Unit::GetAdjPoint(Point& mp,int type,Point &adjp){
    if(type==0){
        adjp=Point(mp.x-1,mp.y);//左
    }else if(type==1){
        adjp=Point(mp.x-1,mp.y+1);//左下
    }else if(type==2){
        adjp=Point(mp.x, mp.y+1);//下
    }else if(type==3){
        adjp=Point(mp.x+1,mp.y+1);//右下
    }else if(type==4){
        adjp=Point(mp.x+1,mp.y);//右
    }else if(type==5){
        adjp=Point(mp.x+1,mp.y-1);
        
    }else if(type==6){
        adjp=Point(mp.x,mp.y-1);//上
        
    }else if(type==7){
        adjp=Point(mp.x-1,mp.y-1);//左上
        
    }
    //CCLOG("adjpoint: x:%f y:%f",adjp.x,adjp.y);
    return isinMapArea(m_layertilesize,adjp);
    
}


SearchNode Unit::getSmallvalueandDelete( vector<SearchNode>& openlist){
    
    SearchNode tempnode=openlist[0];
    vector<SearchNode>::iterator it=openlist.begin();
    int number=0;
    
    for (int i=0; i<openlist.size(); i++) {
        if(tempnode.mHValue+tempnode.mGValue>openlist[i].mHValue+openlist[i].mGValue){
            tempnode=openlist[i];
            number=i;
        }
    }
    
    it=openlist.begin();
    while(--number>=0){
        it++;
    }
    openlist.erase(it);
    //CCLOG("openlist size after : %lu samllnode:(%f,%f)",openlist.size(),tempnode.mPoint.x,tempnode.mPoint.y);
    return  tempnode;
    
}

bool Unit::isContainObjectNode(vector<SearchNode> &openlist,Point& endpos){
    //SearchNode objecpos=SearchNode(endpos);
    
    for (vector<SearchNode>::iterator it=openlist.begin(); it!=openlist.end(); it++) {
        if(it->mPoint==endpos)
            return  true;
    }
    return  false;
    //    if (find(openlist.begin(), openlist.end(), objecpos)!=openlist.end()) {
    //        return  true;
    //    }else {
    //        return  false;
    //    }
    
}

bool Unit::isInMoveArea(Point &tilepos){
    
    if(find(vecMoveArea.begin(), vecMoveArea.end(), tilepos)!=vecMoveArea.end()){
        return true;
    }else
        return  false;
    
    
}
/**
 *
 *  坦克不管轻重还是中型，攻击范围是2，所有步兵的攻击范围是1，轻型火炮150毫米攻击范围3，重型210毫米火炮攻击单位4，飞机8
 *  @param type
 *
 *  @return
 */
UnitType Unit::getUnitBytype(int type){
    UnitType temp;
    switch (type) {
        case FOOT_1:
            
            break;
        case TANK_1:
        {
            temp.s_damage=3;
            temp.s_attakarea=2;
            temp.s_movearea=2;
            temp.s_blood=20;
        }
            break;
        case PLANE_1:{
            temp.s_damage=2;
            temp.s_attakarea=8;
            temp.s_movearea=0;
            temp.s_blood=20;
        }
        default:
            break;
    }
    return  temp;
}

/**
 *  确定AI移动动作
 *
 *  @param tag    AI tag
 *  @param AIUnit AI单位
 */
void Unit::EnActions(int tag, Unit* AIUnit){
  
   
  //先判断攻击范围内是否有敌军
    UnitType ut=getUnitBytype(AIUnit->m_type);
    int attackarea=ut.s_attakarea;
   // int marea=ut.s_movearea;
    Point enpos=AIUnit->m_tilepos;
    
    map<Point,Unit*> mapU;
    vector<Unit*> vecU;
    
    for(int i=enpos.x-attackarea;i<=enpos.x+attackarea;i++){
        for(int j=enpos.y-attackarea;j<=enpos.y+attackarea;j++){
            if(mapVec2Unit.find(Point(i,j))!=mapVec2Unit.end()&&(mapVec2Unit[Point(i,j)]->m_camp!=AIUnit->m_camp) ){

                    mapU[Point(i,j)]=mapVec2Unit[Point(i,j)];//当前AI攻击范围内的攻击单位
                    vecU.push_back(mapVec2Unit[(Point(i,j))]);
                
            }
            
        }
    }
    //分别考虑攻击范围内有敌军和无敌军的状况
    if (!mapU.empty()) {
         AIAttack(mapU,vecU,AIUnit,true);//AI攻击范围内有目标

    }else{//攻击范围内无敌军 则继续寻找
    
        bool isFound=false;
        int area=attackarea;
        while (!isFound) {
            //攻击范围内无敌军 则逐层向外遍历 每次搜索新加的最外一层位置 待改进
            area++;
            //当搜索范围超过整个地图大小时 表示玩家阵营单位已经被消灭
            if(!isinMapArea(m_layertilesize, Point(enpos.x-area,enpos.y-area))
               &&!isinMapArea(m_layertilesize, Point(enpos.x+area,enpos.y-area))
               &&!isinMapArea(m_layertilesize, Point(enpos.x-area,enpos.y+area))
               &&!isinMapArea(m_layertilesize, Point(enpos.x+area,enpos.y+area))){
                break;
               }
            for(int i=enpos.x-area;i<=enpos.x+area;i++){
                for(int j=enpos.y-area;j<=enpos.y+area;j++){
                    //搜索瓦片上有单位 且不在同一阵营
                    if(mapVec2Unit.find(Point(i,j))!=mapVec2Unit.end()&&(mapVec2Unit[Point(i,j)]->m_camp!=AIUnit->m_camp)){
                            isFound=true;
                            mapU[Point(i,j)]=mapVec2Unit[Point(i,j)];//当前AI攻击范围内的攻击单位
                            vecU.push_back(mapVec2Unit[(Point(i,j))]);
                        
                    }
                    
                }
            }
            
        }
        //地图玩家单位已全部被消灭
        if(mapU.size()==0){
            CCLOG("AI WIN!!!!");
            return;
        
        }
        else if(mapU.size()==1) {//攻击范围内只有一个敌军 确定移动方向
            map<Point,Unit*>::iterator it=mapU.begin();
            Unit* target=it->second;
            //在移动范围内向最近敌军移动
            AImoveDirection(AIUnit, target);
            auto func=[=](float dd){
                AIAttack(mapU, vecU, AIUnit);
            };
            target->scheduleOnce(func, AIUnit->totaltime, "AI");
          //  target->uintBeenAttacked(AIUnit);
            
        }else{//攻击范围内有多个敌军 确定移动方向 此时敌军都在同一环内
            Unit* movetarget;
            sort(vecU.begin(),vecU.end(),[&](Unit* a,Unit*b)->bool{
                //                float disA=hypot(abs(a->m_tilepos.x-AIUnit->m_tilepos.x), abs(a->m_tilepos.y-AIUnit->m_tilepos.y));
                //                float disB=hypot(abs(b->m_tilepos.x-AIUnit->m_tilepos.x), abs(b->m_tilepos.y-AIUnit->m_tilepos.y));
                //                return  disA<=disB;
                int curblood_a=a->m_curblood;
                int curblood_b=b->m_curblood;
                return  curblood_a<=curblood_b;
                
            });
            if((*vecU.begin())->m_curblood<=AIUnit->m_damage*2){ //如果AI两炮之内能干掉单位
                movetarget=(*vecU.begin());
                
            }else{
                //AI两炮之内都干不掉 则干攻击力最高的单位
                sort(vecU.begin(),vecU.end(),[&](Unit* a,Unit*b)->bool{
                    int damageA=a->m_damage;
                    int damageB=b->m_damage;
                    return damageA>=damageB;
                });
                movetarget=(*vecU.begin());
                
            }

            AImoveDirection(AIUnit, movetarget);
            auto func=[&](float dd){
                AIAttack(mapU, vecU, AIUnit);
            };
            movetarget->scheduleOnce(func, AIUnit->totaltime, "AI");
            
        }
        
    
    
    }
    
}
/**
 *  确定AI攻击范围内优先攻击哪个目标
 *
 *
 *  @param mapU      候选目标单位
 *  @param vecU      候选目标单位
 *  @param AIUnit    AI单位
 *  @param isSearch  之前是否已经搜索过候选单位
 */
void Unit::AIAttack(map<Point,Unit*> mapU,vector<Unit*> vecU,Unit* AIUnit,bool isSearch){
    UnitType ut=getUnitBytype(AIUnit->m_type);
    int attackarea=ut.s_attakarea;
   
    Point enpos=AIUnit->m_tilepos;
    if(!isSearch){
    for(int i=enpos.x-attackarea;i<=enpos.x+attackarea;i++){
        for(int j=enpos.y-attackarea;j<=enpos.y+attackarea;j++){
            if(mapVec2Unit.find(Point(i,j))!=mapVec2Unit.end()){
                if (mapVec2Unit[Point(i,j)]->m_camp!=AIUnit->m_camp) {
                    mapU[Point(i,j)]=mapVec2Unit[Point(i,j)];//当前AI攻击范围内的攻击单位
                    vecU.push_back(mapVec2Unit[(Point(i,j))]);
                }
            }
            
        }
    }
    }
    if(mapU.size()==0){
        return;
    }else if (mapU.size()==1) {//攻击范围内只有一个敌军
        map<Point,Unit*>::iterator it=mapU.begin();
        Unit* target=it->second;
        target->unitBeenAttacked(AIUnit);

        //AIUnit->unitBeenAttackedBack(target);
        
    }else{//攻击范围内有多个敌军
        
        //先按当前血量排序
        
        sort(vecU.begin(),vecU.end(),[&](Unit* a,Unit*b)->bool{
            //                float disA=hypot(abs(a->m_tilepos.x-AIUnit->m_tilepos.x), abs(a->m_tilepos.y-AIUnit->m_tilepos.y));
            //                float disB=hypot(abs(b->m_tilepos.x-AIUnit->m_tilepos.x), abs(b->m_tilepos.y-AIUnit->m_tilepos.y));
            //                return  disA<=disB;
            int curblood_a=a->m_curblood;
            int curblood_b=b->m_curblood;
            return  curblood_a<=curblood_b;
            
        });
        if((*vecU.begin())->m_curblood<=AIUnit->m_damage*2){ //如果AI两炮之内能干掉单位
            
            (*vecU.begin())->unitBeenAttacked(AIUnit);
   
           AIUnit->unitBeenAttackedBack((*vecU.begin()));
            
        
        }else{
            //AI两炮之内都干不掉 则干攻击力最高的单位
            sort(vecU.begin(),vecU.end(),[&](Unit* a,Unit*b)->bool{
                int damageA=a->m_damage;
                int damageB=b->m_damage;
                return damageA>=damageB;
            });
            
            (*vecU.begin())->unitBeenAttacked(AIUnit);
          
            AIUnit->unitBeenAttackedBack((*vecU.begin()));
            
        }
        
        
        
    }

}
/**
 *  确定AI的移动方向后进行移动
 *  此处有两种情况 1、目标在移动范围之外，攻击范围之外 2、目标在移动范围之内，攻击范围之外
 *  @param Unit*AIunit AI单位
 *  @param target      移动方向上的目标单位
 */
void Unit::AImoveDirection(Unit*AIunit,Unit* target){
    Point mpos;
    int direc=0;
    UnitType ut=getUnitBytype(AIunit->m_type);
   // int attackarea=ut.s_attakarea;
    int marea=ut.s_movearea;
    Point enpos=AIunit->m_tilepos;//AI位置
    Point tpos=target->m_tilepos;//目标位置
    Point A(enpos.x-marea,enpos.y-marea);//左上角
    Point B(enpos.x+marea,enpos.y-marea);//右上角
    Point C(enpos.x+marea,enpos.y+marea);//右下角
    Point D(enpos.x-marea,enpos.y+marea);//左下角
    //如果目标位置在AI移动范围之内 则直接移动
    if(AinBmovearea(target,AIunit)){
        AIunit->AstartMove(AIunit->m_tilepos, target->m_tilepos);
        return;
    }
    
    
    if(tpos.x<A.x){
        if(tpos.y<A.y){
            //左上角
            mpos=A;
            direc=0;
        }else if(tpos.y>=A.y&&tpos.y<=D.y){
            //左边
            mpos.y=tpos.y;
            mpos.x=A.x;
            direc=1;
        }else if(tpos.y>D.y){
            //左下角
            mpos=D;
            direc=2;
        }
        
    }else if(tpos.x>=A.x&&tpos.x<=B.x){
        
        if(tpos.y<A.y){
            //上边
            mpos.x=tpos.x;
            mpos.y=A.y;
            direc=7;
        }else if (tpos.y>D.y){
            //下边
            mpos.x=tpos.x;
            mpos.y=D.y;
            direc=3;
        }
    }else if (tpos.x>B.x){
        if(tpos.y<B.y){
            //右上角
            mpos=B;
            direc=6;
            
        }else if(tpos.y>=B.y&&tpos.y<=C.y){
            //右边
            
            mpos.y=tpos.y;
            mpos.x=B.x;
            direc=5;
            
        }else if(tpos.y>C.y){
            //右下角
            mpos=C;
            direc=4;
        }
        
    }
//    Point realmpos=mpos;
//    //保证AI移动不重叠
//    if(mapVec2Unit.find(mpos)!=mapVec2Unit.end()){
//        if(direc==0)
//        realmpos=mpos+Point(0,1);
//        CCLOG("mpos:(%f,%f)",mpos.x,mpos.y);
//    }
    AIunit->AstartMove(AIunit->m_tilepos, mpos);
}
bool Unit::AinBmovearea(Unit* target,Unit* AIunit){
    UnitType ut=Unit::getUnitBytype(AIunit->m_type);
    // int attackarea=ut.s_attakarea;
    int marea=ut.s_movearea;
    auto x=target->m_tilepos.x;
    auto y=target->m_tilepos.y;
    if(x>=AIunit->m_tilepos.x-marea&&x<=AIunit->m_tilepos.x+marea&&y>=AIunit->m_tilepos.y-marea&&y<=AIunit->m_tilepos.y+marea){
        return  true;
    }else {
        return  false;
     }
    
}
