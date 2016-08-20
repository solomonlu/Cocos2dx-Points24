#ifndef __CALC_LAYER_H__
#define __CALC_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class CardSprite;
class MainScene;

class CalcLayer: public cocos2d::LayerColor
{
public:
    virtual bool init(CardSprite* firstCard,CardSprite* secondCard,MainScene* mainScene);
	void cancelButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void addButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void minusButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void multiplyButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void divideButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
private:
	cocos2d::EventListenerTouchOneByOne* _listener;
	CardSprite* _firstCard;
	CardSprite* _secondCard;
	MainScene* _mainScene;
};

#endif // __CALC_LAYER_H__
