#ifndef __GAME_START_SCENE_H__
#define __GAME_START_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "extensions/cocos-ext.h"

class GameStartSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
	CREATE_FUNC(GameStartSence);

	virtual bool init() override;
	virtual void update(float dt);
	void singlePlayerModeButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void bluetoothModeButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event* event);
	//void valueChanged(Ref* sender, cocos2d::extension::Control::EventType controlEvent);

	void readyToReplaceBluetoothSence(bool bluetoothDiscoverable);
private:
	unsigned long _backKeyPressedTime;

	bool _toReplaceBluetoothSence;
	bool _bluetoothDiscoverable;


	//cocos2d::extension::ControlSwitch* _switchControl;
};

#endif // __GAME_START_SCENE_H__