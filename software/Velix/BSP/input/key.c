//
// Created by 18135 on 26-1-17.
//

#include "key.h"

Key key_1,key_2;

//按键初始化
void Key_Init(Key* key, Velix_GpioPort* GPIOx, uint16_t GPIO_Pin){
     key->GPIOx = GPIOx;
     key->GPIO_Pin = GPIO_Pin;
     key->last_state = VELIX_GPIO_IS_SET(GPIOx, GPIO_Pin);
     key->press_cnt = 0;
     key->release_cnt = 0;
     key->click_count = 0;
     key->long_flag = false;
     key->event = KEY_EVENT_NONE;
}

//按键扫描
void Key_Scan(Key* key){

    bool current = (VELIX_GPIO_IS_SET(key->GPIOx, key->GPIO_Pin) != 1);//低电平按下

    if(current){
        //按下
        key->press_cnt++;
        key->release_cnt = 0;

        //长按判定
        if(key->press_cnt >= KEY_LONG_TIME && !key->long_flag){
            key->event = KEY_EVENT_LONG_PRESS;
            key->long_flag = true;
            key->click_count = 0;
        }
    }
    //松开
    else{
        key->release_cnt++;

        //单击判定
        if(!key->long_flag && key->press_cnt > 0 && key->press_cnt < KEY_LONG_TIME)     key->click_count++;

        key->press_cnt = 0;

        //松开后重置长按标志
        if(key->long_flag)      key->long_flag = false;
    }
    //单击/双击判断
    if(key->click_count == 1 && key->release_cnt >= KEY_DOUBLE_TIME){
        key->event = KEY_EVENT_SINGLE_CLICK;
        key->click_count = 0;
        key->release_cnt = 0;
    }
    else if(key->click_count == 2){
        key->event = KEY_EVENT_DOUBLE_CLICK;
        key->click_count = 0;
        key->release_cnt = 0;
    }
    key->last_state = current;
}

//获取时间并清零
KeyEvent key_GetEvent(Key* key){
    const KeyEvent e = key->event;//拷贝事件
    key->event = KEY_EVENT_NONE;//清零事件
    return e;
}

void Key_Handler(Key* key,void(*single_cb)(void),void(*double_cb)(void),void(*long_cb)(void)){
    const KeyEvent e = key_GetEvent(key);
    switch (e) {
        case KEY_EVENT_SINGLE_CLICK:
            if(single_cb)   single_cb();
            break;
        case KEY_EVENT_DOUBLE_CLICK:
            if(double_cb)   double_cb();
            break;
        case KEY_EVENT_LONG_PRESS:
            if(long_cb)     long_cb();
            break;
        default:
            break;
    }
}
