//
//  Unit.h
//  liuianxing1
//
//  Created by 肖准 on 15/2/11.
//
//


#ifndef __liuianxing1__Unit__
#define __liuianxing1__Unit__

#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "contains.h"
#include"tools.h"
#include <thread>
using namespace cocos2d;
using namespace cocos2d::ui;

const int NM_SPEED=150;  //正常单位行动速度
const int AI_SPEED=800;  //AI移动速度

struct UnitType{
    float s_damage=1.0;    //伤害值
    int s_movearea=1;  //移动范围
    int s_attakarea=1;  //攻击范围
    float s_blood=10.0;
    float s_defense=1.0;
};

struct SearchNode{
    
    SearchNode(Point &tilepos)
    {
        mPoint=tilepos;
        mIsVisited = false;
        mGValue = 0.0;
        mHValue = 0.0;
        mFvalue=0.0;
    }
    
    Point mPoint;
    SearchNode *ParentNode;
    bool mIsVisited;
    double mGValue;
    double mHValue;
    double mFvalue;
    
};
class Unit:public Sprite{
protected:
    Unit(Point &tilepos);
public:
    
    void initUnit(UnitType type);
       virtual  ~Unit();
    static Unit* create(const std::string& filename,Point&tilepos);

    //移动效果
 
    void setMoveArea();//设定移动范围
    static void RecoverMoveArea();//移动范围还原
    void  AstartMove(Point&startpos,Point&endpos);
    static bool isContainObjectNode(vector<SearchNode> &openlist,Point& endpos);
    static SearchNode getSmallvalueandDelete( vector<SearchNode>& openlist);
    static bool GetAdjPoint(Point& mp,int type,Point& adjp);
    static bool IsSearchNodeArrive(Point & po,Point& endpos);
    static double getGvalue(Point& adjpos,Point& curpos);
    static double getHvalue(Point& adjpos,Point& endpos);
    static SearchNode GetSearchNodeFromOpenLis(Point & adjpoint,vector<SearchNode>& openlist);
    static bool isInMoveArea(Point &tilepos);
    static SearchNode getNodeFromOpenList(vector<SearchNode> &openlist,Point& curpos);


    
    //AI移动

    static Point getAirPoint(Point& target);
    static void EnActions(int tag, Unit* AIUnit);
    static void AIAttack(map<Point,Unit*> mapu,vector<Unit*> vecu,Unit* AIUnit,bool isSearch=false);
    static void AImoveDirection(Unit*AIunit,Unit* target);
    static bool AinBmovearea(Unit* target,Unit* AIunit);
 //    static bool cmpByDistance(Unit*a ,Unit*b){
//       
//    }
//    static bool cmpByBlood(Unit*a,Unit*b){
//    
//    }
//    static bool cmpByDamage(Unit*a,Unit*b){
//    
//    }
    
    
    //根据type获得strut
    static UnitType getUnitBytype(int type);
    //攻击效果
    static void RecoverAttackArea();//攻击范围还原
    void setAttackArea();
    bool isInAttckArea();
    void unitBeenAttacked(Unit* preuni,bool isActive=true);//isActive 表示 是否是主动攻击 被动攻击情况下攻击力减半
    void unitBeenAttackedBack(Unit* preuni);
    void showBloodReduce(float reduceblood);//伤害悬浮显示
    
    
    float totaltime;
    bool m_underAttacked;
    bool m_dead;
    long m_tag;
    int m_camp;   //阵营 0：我方 1-？？ ：地方
    float m_blood;   //原始血量
    float m_curblood;  //当前血量
    float m_defense;
    
    Unit* m_unit;
    int m_type;
    int m_armytype;
    float m_damage;    //伤害值
    int m_movearea;  //移动范围
    int m_attakarea;  //攻击范围
    Point m_realpos;
    std::string m_strpos;
    Point m_tilepos;  //当前节点所在的瓦片坐标
    LoadingBar* m_bloodbar; //当前节点绑定的血槽
    
};
extern bool isAIMoveDone;
extern std::map<int,Unit*> mapEnTagUnit; //敌军 的tag 和 单位绑定
extern std::map<Point,Unit*> mapVec2Unit; //tilepos 和 上面的单位绑定
extern std::map<int,Unit*> mapTagUnit;
#endif /* defined(__liuianxing1__Unit__) */
