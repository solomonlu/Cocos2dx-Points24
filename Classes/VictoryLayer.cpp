#include "VictoryLayer.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include "MainScene.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

USING_NS_CC;
using namespace ui;

bool VictoryLayer::init(MainScene* mainScene,const std::string& formula,bool multiplay,bool win)
{
	if ( !LayerColor::initWithColor(Color4B(0, 0, 0, 150)))
    {
        return false;
    }

	_mainScene = mainScene;
	_multiplay = multiplay;
	_win = win;
	_selfReady = false;
	_opponentReady = false;

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/VictoryLayer.json").c_str()));

	if (_multiplay == false 
	 || (_multiplay == true && _win == true))
	{
		auto loseImage = static_cast<ImageView*>(mainPanel->getChildByName("LoseImage"));
		loseImage->setVisible(false);
	}
	else
	{
		auto winImage = static_cast<ImageView*>(mainPanel->getChildByName("WinImage"));
		winImage->setVisible(false);
	}

	auto formulaLabel = static_cast<Text*>(mainPanel->getChildByName("FormulaLabel"));
	formulaLabel->setText(formula);

	// 按钮绑定回调
	auto playAgainButton = static_cast<Button*>(mainPanel->getChildByName("PlayAgainButton"));
	playAgainButton->addTouchEventListener(this, toucheventselector(VictoryLayer::playAgainButtonTouched));

	this->addChild(mainPanel);

	// 模态效果
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	_listener = EventListenerTouchOneByOne::create();
	_listener->onTouchBegan = CC_CALLBACK_2(VictoryLayer::onTouchBegan,this);
	_listener->setSwallowTouches(true);
	dispatcher->addEventListenerWithSceneGraphPriority(_listener,this);

	scheduleUpdate();

	return true;
}

void VictoryLayer::update(float dt)
{
	if (_multiplay && _selfReady == true && _opponentReady == true)
	{
		_mainScene->restart(false);

		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}

bool VictoryLayer::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	return true;
}

void VictoryLayer::playAgainButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		if (_multiplay)
		{
			// 告诉对方自己已经准备好
			rapidjson::Document d1;
			d1.SetObject();
			rapidjson::Document::AllocatorType& allocator = d1.GetAllocator();
			d1.AddMember("cmd", "playAgain", allocator);
  
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
			d1.Accept(write);

			std::string jsonStr = StringUtils::format("%s",buffer.GetString());

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			JniMethodInfo minfo;
			jobject jobj;    
			bool b = JniHelper::getStaticMethodInfo(minfo,     
				"org/cocos2dx/cpp/BluetoothHelper",  
				"sendBluetoothMessage",
				"(Ljava/lang/String;)V");
			jstring jJsonStr = minfo.env->NewStringUTF(jsonStr.c_str());
			jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, jJsonStr);
			minfo.env->DeleteLocalRef(jJsonStr);
	#endif
			_selfReady = true;
		}
		else
		{
			_mainScene->restart(false);

			auto dispatcher = Director::getInstance()->getEventDispatcher();
			dispatcher->removeEventListener(_listener);

			this->removeFromParentAndCleanup(false);
		}
	}
}

void VictoryLayer::setOpponentReady()
{
	_opponentReady = true;
}
