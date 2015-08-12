//
//  tools.h
//  liuianxing1
//
//  Created by 肖准 on 15/2/3.
//
//

#ifndef __liuianxing1__tools__
#define __liuianxing1__tools__

#include <stdio.h>
#include "cocos2d.h"
#include <iostream>
#include <sstream>


using namespace std;
using namespace cocos2d;
extern Size m_tilesize;
extern bool isBeenAttacked;
extern Color3B colorAttack;
extern std::vector<Point> vecPath; //存储行动路径
extern std::vector<Point> vecMoveArea;
extern std::vector<Point> vecAttackArea;
extern bool IS_RELATIVE;  //当前点坐标是否是相对父节点 true:相对  falsel:不想对
extern bool IS_UNIT_TOUCHED; //上一次点击对象是否是unit
extern int UNIT_TOUCHED_TAG; //上次点击对象的tag
extern  cocos2d::TMXLayer* m_layer; //图层
extern  Size m_layertilesize; //图层包含的图块数量
extern cocos2d::TMXTiledMap* m_map;; //地图
extern Size vsize;            //地图大小
extern Size m_tilesize;        //图块大小
extern float PI;




extern std::map<int,int> MapUnitType;  //tag,unittype 映射关系

string pointTostring(Point &po);
bool isinMapArea(Size layersize,Point po);
//像素坐标转为瓦片坐标
Point realposTotilepos(Point& pos);
//瓦片坐标转为像素坐标
Point tilepoTorealpos(Point& pos);
#endif /* defined(__liuianxing1__tools__) */
