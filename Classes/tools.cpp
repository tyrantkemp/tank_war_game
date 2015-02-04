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

bool isinMapArea(Size layersize,Point po){
    if(layersize.width>=po.x&&layersize.height>=po.y&&po.x>=0&&po.y>=0){
        return true;
    }
    else
        return  false;
    

}