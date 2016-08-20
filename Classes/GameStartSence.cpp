#include "GameStartSence.h"
#include "LayerTagDefine.h"
#include "MainScene.h"
#include "BluetoothSence.h"
#include "Util.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

Scene* GameStartSence::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    GameStartSence *layer = GameStartSence::create();

    // add layer as a child to scene
    scene->addChild(layer,0,LAYER_TAG_GAME_START);

    // return the scene
    return scene;
}

bool GameStartSence::init()
{
	if ( !Layer::init() )
    {
        return false;
    }

	_toReplaceBluetoothSence = false;
	_bluetoothDiscoverable = false;

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/GameStartScene.json").c_str()));

	// 按钮绑定回调
	auto singlePlayerModeButton = static_cast<Button*>(mainPanel->getChildByName("SinglePlayerModeButton"));
	singlePlayerModeButton->addTouchEventListener(this, toucheventselector(GameStartSence::singlePlayerModeButtonTouched));

	auto bluetoothModeButton = static_cast<Button*>(mainPanel->getChildByName("BluetoothModeButton"));
	bluetoothModeButton->addTouchEventListener(this, toucheventselector(GameStartSence::bluetoothModeButtonTouched));

	this->addChild(mainPanel);

	// 处理该界面的按键操作
	auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameStartSence::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	scheduleUpdate();

	//_switchControl = ControlSwitch::create(
	//			Sprite::create("switch-mask.png"),
 //               Sprite::create("switch-on.png"),
 //               Sprite::create("switch-off.png"),
 //               Sprite::create("switch-thumb.png")
 //           );
	//_switchControl->setAnchorPoint(Point(0,0));
	//_switchControl->setOn(true);
	//_switchControl->addTargetWithActionForControlEvents(this, cccontrol_selector(GameStartSence::valueChanged), Control::EventType::VALUE_CHANGED|Control::EventType::TOUCH_DOWN);
	//_switchControl->setPosition(Point(200,150));
	//mainPanel->addChild(_switchControl);
	
	return true;
}

//void GameStartSence::valueChanged(Ref* sender, cocos2d::extension::Control::EventType controlEvent)
//{
//	ControlSwitch* pSwitch = (ControlSwitch*)sender;
//    if (pSwitch->isOn())
//    {
//		//MessageBox("test","switch ON!");
//        //_displayValueLabel->setString("On");
//    } 
//    else
//    {
//		pSwitch->setOn(true);
//		
//		//MessageBox("test","switch OFF!");
//        //_displayValueLabel->setString("Off");
//    }
//}

void GameStartSence::update(float dt)
{
	//if (_switchControl->isOn())
	//{
	//	_switchControl->setOn(false);
	//	//MessageBox("test","switch OFF!");
	//}
	//else
	//{
	//	_switchControl->setOn(true);
	//	//MessageBox("test","switch ON!");
	//}

	if (_toReplaceBluetoothSence != false )
	{
		_toReplaceBluetoothSence = false;

		auto newSence = TransitionSlideInR::create(1.f,BluetoothSence::scene(_bluetoothDiscoverable));
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void GameStartSence::singlePlayerModeButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto newSence = TransitionSlideInR::create(1.f,MainScene::scene(getRandom4CardsString(),false));
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void GameStartSence::bluetoothModeButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo minfo;
		jobject jobj;    
		bool b = JniHelper::getStaticMethodInfo(minfo,     
			"org/cocos2dx/cpp/BluetoothHelper",
			"enableBluetooth",
			"()V");
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
#endif
	}
}

void GameStartSence::onKeyReleased(EventKeyboard::KeyCode key, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_RETURN == key
		|| EventKeyboard::KeyCode::KEY_ESCAPE == key
		|| EventKeyboard::KeyCode::KEY_BACKSPACE == key)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		unsigned long now = time(0);
		if (now - _backKeyPressedTime > 2)
		{
			_backKeyPressedTime = now;
			JniMethodInfo minfo;
			jobject jobj;    
			bool b = JniHelper::getStaticMethodInfo(minfo,     
				"org/cocos2dx/cpp/ToastHelper",  
				"showToast",
				"()V");
			jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		}
		else
		{
			Director::getInstance()->end();
		}
#else
		Director::getInstance()->end();
#endif
    }
}

void GameStartSence::readyToReplaceBluetoothSence(bool bluetoothDiscoverable)
{
	_toReplaceBluetoothSence = true;
	_bluetoothDiscoverable = bluetoothDiscoverable;
}

