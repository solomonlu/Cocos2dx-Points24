#ifndef __VICTORY_LAYER_H__
#define __VICTORY_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class MainScene;

class VictoryLayer: public cocos2d::LayerColor
{
public:
    virtual bool init(MainScene* mainScene,const std::string& formula,bool multiplay,bool win);
	virtual void update(float dt);
	void playAgainButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);

	void setOpponentReady();
private:
	cocos2d::EventListenerTouchOneByOne* _listener;
	MainScene* _mainScene;
	bool _multiplay;
	bool _win;
	bool _selfReady;
	bool _opponentReady;
};

#endif // __VICTORY_LAYER_H__
