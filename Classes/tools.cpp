//
//  tools.cpp
//  liuianxing1
//
//  Created by 肖准 on 15/2/3.
//
//

#include "tools.h"

using namespace cocos2d;
using namespace std;
//point （x,y）->  "x_y"
string pointTostring(Point &po){
    stringstream ss;
    ss<<po.x<<"_"<<po.y;
    string temp=ss.str();
    return temp;
    
  
}
//判断瓦片坐标是否在地图范围内
bool isinMapArea(Size layersize,Point po){
    if(layersize.width>=po.x&&layersize.height>=po.y&&po.x>=0&&po.y>=0){
        return true;
    }
    else
        return  false;
    

}
//像素坐标 -> 瓦片坐标
Point realposTotilepos(Point& realtouch){
    
    
    
    Point relative_pos=realtouch;
    //检查 点击点是否在地图坐表内
    if(!IS_RELATIVE){
       relative_pos=m_map->convertToNodeSpace(realtouch);//从当前屏幕坐标获取相对于地图的坐标
       // CCLOG("relative_pos:%f %f",relative_pos.x,relative_pos.y);
        
    }else{
        IS_RELATIVE=false;
        
    }
    
    float x=relative_pos.x;
    float y=relative_pos.y;
    double xie_xian=sqrt(m_tilesize.width*m_tilesize.width/4+m_tilesize.height*m_tilesize.height/4);
   // CCLOG("m_tilesize.x :%f y:%fxie_xian:%f",m_tilesize.width,m_tilesize.height, xie_xian);
    
    
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
    
    CCLOG("out point: d2x:%d d2y:%d  IS_RELATIVE:%d",d2x,d2y,IS_RELATIVE);
    
    return  Point(d2x,d2y);
    
    
    
}
//瓦片坐标 -> 像素坐标  改写 引擎 api
Point tilepoTorealpos(Point& pos){
    
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


