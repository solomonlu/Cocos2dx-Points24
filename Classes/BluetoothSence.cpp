#include "BluetoothSence.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include "LayerTagDefine.h"
#include "MainScene.h"
#include "GameStartSence.h"
#include "Util.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

Scene* BluetoothSence::scene(bool discoverable)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
   	BluetoothSence *layer = new BluetoothSence();
    if (layer && layer->init(discoverable))
    {
        layer->autorelease();
    }
    else
    {
        delete layer;
		layer = nullptr;
    }

    // add layer as a child to scene
    scene->addChild(layer,0,LAYER_TAG_BLUETOOTH_BLUETOOTH);

    // return the scene
    return scene;
}

bool BluetoothSence::init(bool discoverable)
{
	if ( !Layer::init() )
    {
        return false;
    }

	_status = BLUETOOTH_STATUS_TYPE_NO_CONNECT;
	_toChangeStatus = BLUETOOTH_STATUS_TYPE_NONE;

	_discoverable = discoverable;
	_resetDiscoverable = true;
	_selfReady = false;
	_opponentReady = false;
	_random4cardsString = "";

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/BluetoothScene.json").c_str()));

	_connectLayout = static_cast<Layout*>(mainPanel->getChildByName("ConnectPanel"));

	// 按钮绑定回调
	_serverModeButton = static_cast<Button*>(_connectLayout->getChildByName("ServerModeButton"));
	_serverModeButton->addTouchEventListener(this, toucheventselector(BluetoothSence::serverModeButtonTouched));

	auto clientModeButton = static_cast<Button*>(_connectLayout->getChildByName("ClientModeButton"));
	clientModeButton->addTouchEventListener(this, toucheventselector(BluetoothSence::clientModeButtonTouched));

	auto readyButton = static_cast<Button*>(_connectLayout->getChildByName("ReadyButton"));
	readyButton->addTouchEventListener(this, toucheventselector(BluetoothSence::readyButtonTouched));

	auto backButton = static_cast<Button*>(_connectLayout->getChildByName("BackButton"));
	backButton->addTouchEventListener(this, toucheventselector(BluetoothSence::backButtonTouched));

	_bluetoothDeviceListView = static_cast<ListView*>(_connectLayout->getChildByName("BluetoothDeviceListView"));
	_bluetoothDeviceListView->addEventListenerListView(this, listvieweventselector(BluetoothSence::connectServerEvent));



	//Layout* serverModePanel = static_cast<Layout*>(_connectLayout->getChildByName("ServerModePanel"));
	//_switchControl = ControlSwitch::create(
	//			Sprite::create("switch-mask.png"),
 //               Sprite::create("switch-on.png"),
 //               Sprite::create("switch-off.png"),
 //               Sprite::create("switch-thumb.png")
 //           );
	//_switchControl->setAnchorPoint(Point(0,0));
	//_switchControl->addTargetWithActionForControlEvents(this, cccontrol_selector(BluetoothSence::valueChanged), Control::EventType::VALUE_CHANGED);
	//serverModePanel->addChild(_switchControl);

	this->addChild(mainPanel);

	// 处理该界面的按键操作
	auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(BluetoothSence::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	scheduleUpdate();
	
	return true;
}

void BluetoothSence::update(float dt)
{
	// 处理可见性
	if (_resetDiscoverable)
	{
		_resetDiscoverable = false;

		if (_discoverable)
		{
			_serverModeButton->setBright(true);
			//_switchControl->setOn(true);
		}
		else
		{
			_serverModeButton->setBright(false);
			//_switchControl->setOn(false);
		}
	}

	// 处理状态变化
	handleBluetoothStatusChange();

	if (!_tobeAddDeviceInfoSet.empty())
	{
		for (auto deviceInfo : _tobeAddDeviceInfoSet)
		{
			if (_addedDeviceInfoSet.find(deviceInfo) != _addedDeviceInfoSet.end())
			{
				continue;
			}

			size_t pos = deviceInfo.find("|");
			std::string deviceName = deviceInfo.substr(0,pos);
			std::string deviceMac = deviceInfo.substr(pos+1,deviceInfo.size());

			Button* custom_button = Button::create("device_info.png", "device_info_pressed.png");
           	custom_button->setName(deviceMac.c_str());
			custom_button->setTitleText(deviceName);
			custom_button->setTitleFontSize(custom_button->getTitleFontSize() * 2);

            _bluetoothDeviceListView->pushBackCustomItem(custom_button);
			_addedDeviceInfoSet.insert(deviceInfo);
		}

		_tobeAddDeviceInfoSet.clear();
	}

	// 处理跳转到游戏界面的响应
	if (_status == BLUETOOTH_STATUS_TYPE_CONNECTED && _selfReady == true && _opponentReady == true)
	{
		_selfReady = false;
		_opponentReady = false;

		auto newSence = TransitionSlideInR::create(1.f,MainScene::scene(_random4cardsString,true));
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void BluetoothSence::serverModeButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo minfo;
		jobject jobj;    
		bool b = JniHelper::getStaticMethodInfo(minfo,     
			"org/cocos2dx/cpp/BluetoothHelper",  
			"startBluetoothServer",
			"()V");
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
#endif
		//BluetoothServerWaitLayer* layer = new BluetoothServerWaitLayer();
		//layer->init(this);
		//layer->autorelease();
		//this->addChild(layer,0,LAYER_TAG_BLUETOOTH_SERVER);

		//setBluetoothStatus(BLUETOOTH_STATUS_TYPE_WAIT_FOR_CONNECT);
	}
}

void BluetoothSence::clientModeButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo minfo;
		jobject jobj;    
		bool b = JniHelper::getStaticMethodInfo(minfo,     
			"org/cocos2dx/cpp/BluetoothHelper",
			"startBluetoothClient",
			"()V");
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
#endif
		_bluetoothDeviceListView->removeAllItems();
		_addedDeviceInfoSet.clear();
		_tobeAddDeviceInfoSet.clear();
		//BluetoothClientWaitLayer* layer = new BluetoothClientWaitLayer();
		//layer->init();
		//layer->autorelease();
		//this->addChild(layer,0,LAYER_TAG_BLUETOOTH_CLIENT);
	}
}

void BluetoothSence::connectServerEvent(Ref *pSender, cocos2d::ui::ListViewEventType type)
{
	switch (type)
    {
        case LISTVIEW_ONSELECTEDITEM_START:
        {
            ListView* listView = static_cast<ListView*>(pSender);
            CC_UNUSED_PARAM(listView);
            CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
            break;
        }
        case LISTVIEW_ONSELECTEDITEM_END:
        {
			// 每次点击连接服务器，都会在本地创建一个随机的卡牌序列
			// 在准备时发送给远端
			// 当然，这可能会存在2边都点击过“连接”的问题，有待解决
			_random4cardsString = getRandom4CardsString();

            ListView* listView = static_cast<ListView*>(pSender);
            CC_UNUSED_PARAM(listView);
			int selectIndex = listView->getCurSelectedIndex();
            CCLOG("select child end index = %d", selectIndex);

			auto selectDevice = (ui::Button*)listView->getItem(selectIndex);
			std::string deviceName = selectDevice->getTitleText();
			std::string deviceMac = selectDevice->getName();
			std::string strDevice = deviceName + "|" + deviceMac;

			CCLOG("select device = %s", strDevice.c_str());
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			JniMethodInfo minfo;
			jobject jobj;    
			bool b = JniHelper::getStaticMethodInfo(minfo,     
				"org/cocos2dx/cpp/BluetoothHelper",  
				"connectToDevice",
				"(Ljava/lang/String;)V");
			jstring jDeviceInfo = minfo.env->NewStringUTF(strDevice.c_str());
			jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, jDeviceInfo);
			minfo.env->DeleteLocalRef(jDeviceInfo);
#endif
            break;
        }
        default:
            break;
    }
}

void BluetoothSence::readyButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		// 告诉对方自己已经准备好
		rapidjson::Document d1;
		d1.SetObject();
		rapidjson::Document::AllocatorType& allocator = d1.GetAllocator();
		d1.AddMember("cmd", "ready", allocator);
		d1.AddMember("cards", _random4cardsString.c_str(), allocator);
  
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

		// 设置自己为准备好
		_selfReady = true;

		Button* button = static_cast<Button*>(pSender);
		button->setBright(false);
		button->setTouchEnabled(false);

		//MessageBox("test","ready!");
	}
}

void BluetoothSence::backButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto newSence = TransitionSlideInL::create(1.f,GameStartSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void BluetoothSence::valueChanged(Ref* sender, Control::EventType controlEvent)
{
    ControlSwitch* pSwitch = (ControlSwitch*)sender;
    if (pSwitch->isOn())
    {
		//MessageBox("test","switch ON!");
        //_displayValueLabel->setString("On");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo minfo;
		jobject jobj;    
		bool b = JniHelper::getStaticMethodInfo(minfo,     
			"org/cocos2dx/cpp/BluetoothHelper",  
			"startBluetoothServer",
			"()V");
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
#endif
    } 
    else
    {
		pSwitch->setOn(true);
		//MessageBox("test","switch OFF!");
        //_displayValueLabel->setString("Off");
    }
}

void BluetoothSence::onKeyReleased(EventKeyboard::KeyCode key, Event* event)
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

void BluetoothSence::setBluetoothDiscoverable(bool discoverable)
{
	if (_discoverable == discoverable)
	{
		return;
	}
	else
	{
		_discoverable = discoverable;
		_resetDiscoverable = true;
	}
}

void BluetoothSence::setBluetoothStatus(BluetoothStatusType status)
{
	_toChangeStatus = status;
}

void BluetoothSence::addDevice(const std::string& deviceInfo)
{
	if (_tobeAddDeviceInfoSet.find(deviceInfo) == _tobeAddDeviceInfoSet.end())
	{
		_tobeAddDeviceInfoSet.insert(deviceInfo);
	}
}

void BluetoothSence::setOpponentReady()
{
	_opponentReady = true;

	//MessageBox("test","opponent ready!");
}

void BluetoothSence::setRandom4cardsString(const std::string& random4cardsString)
{
	if (random4cardsString == "")
	{
		return;
	}

	if (random4cardsString == _random4cardsString)
	{
		return;
	}

	_random4cardsString = random4cardsString;
}

void BluetoothSence::handleBluetoothStatusChange()
{
	if(_toChangeStatus == BLUETOOTH_STATUS_TYPE_NONE)
	{
		return;
	}

	CCLOG("status change! status before = %d,status after = %d", static_cast<int>(_status),static_cast<int>(_toChangeStatus));

	//if (_status == BLUETOOTH_STATUS_TYPE_NO_CONNECT && _toChangeStatus == BLUETOOTH_STATUS_TYPE_WAIT_FOR_CONNECT)
	//{
	//	_timer = 120;
	//	_statusLabel->setText("2");

	//	_status = _toChangeStatus;
	//	_toChangeStatus = BLUETOOTH_STATUS_TYPE_NONE;
	//}
	if (_status != BLUETOOTH_STATUS_TYPE_CONNECTED && _toChangeStatus == BLUETOOTH_STATUS_TYPE_CONNECTED)
	{
		// 整个页面左移
		auto moveLeftAction = MoveTo::create(1.f,Point(-640,0));
		_connectLayout->runAction(moveLeftAction);

		_status = _toChangeStatus;
		_toChangeStatus = BLUETOOTH_STATUS_TYPE_NONE;
	}
	else
	{
		CCLOG("status change error!");
	}
	//switch (_status)
	//{
	//case BluetoothSence::BLUETOOTH_STATUS_TYPE_NO_CONNECT:
	//	_statusLabel->setText("1");
	//	break;
	//case BluetoothSence::BLUETOOTH_STATUS_TYPE_WAIT_FOR_CONNECT:
	//	_statusLabel->setText("2");
	//	break;
	//case BluetoothSence::BLUETOOTH_STATUS_TYPE_DISCOVERING:
	//	_statusLabel->setText("3");
	//	break;
	//case BluetoothSence::BLUETOOTH_STATUS_TYPE_DISCOVER_FINISH:
	//	_statusLabel->setText("4");
	//	break;
	//case BluetoothSence::BLUETOOTH_STATUS_TYPE_CONNECTING:
	//	_statusLabel->setText("5");
	//	break;
	//case BluetoothSence::BLUETOOTH_STATUS_TYPE_CONNECTED:
	//	_statusLabel->setText("6");
	//	break;
	//default:
	//	break;
	//}
}


