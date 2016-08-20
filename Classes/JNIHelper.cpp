#include "cocos2d.h"
#include "LayerTagDefine.h"
#include "MainScene.h"
#include "GameStartSence.h"
#include "BluetoothSence.h"
#include "VictoryLayer.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif
USING_NS_CC;

static void EnterBluetoothSence(bool discoverable)
{
	auto currentScene = Director::getInstance()->getRunningScene();
	
	Node* scene = currentScene->getChildByTag(LAYER_TAG_GAME_START);
	if (scene == nullptr)
	{
		CCLOG("not in game start scene!");
		return;
	}

	GameStartSence* layer = dynamic_cast<GameStartSence*>(scene);
	if (layer == nullptr)
	{
		CCLOG("not in game start layer!");
		return;
	}

	layer->readyToReplaceBluetoothSence(discoverable);
}

static void SetBluetoothDiscoverable(bool discoverable)
{
	auto currentScene = Director::getInstance()->getRunningScene();
	
	Node* scene = currentScene->getChildByTag(LAYER_TAG_BLUETOOTH_BLUETOOTH);
	if (scene == nullptr)
	{
		CCLOG("not in bluetooth scene!");
		return;
	}

	BluetoothSence* layer = dynamic_cast<BluetoothSence*>(scene);
	if (layer == nullptr)
	{
		CCLOG("not in bluetooth layer!");
		return;
	}

	layer->setBluetoothDiscoverable(discoverable);
}

//static void SetBluetoothStatus(int status)
//{
//	auto currentScene = Director::getInstance()->getRunningScene();
//	
//	Node* scene = currentScene->getChildByTag(LAYER_TAG_BLUETOOTH_BLUETOOTH);
//	if (scene == nullptr)
//	{
//		CCLOG("not in bluetooth scene!");
//		return;
//	}
//
//	BluetoothSence* layer = dynamic_cast<BluetoothSence*>(scene);
//	if (layer == nullptr)
//	{
//		CCLOG("not in bluetooth layer!");
//		return;
//	}
//
//	layer->setBluetoothStatus(static_cast<BluetoothSence::BluetoothStatusType>(status));
//}

static void SendBluetoothDeviceInfo(const char * info)
{
	auto currentScene = Director::getInstance()->getRunningScene();
	
	Node* scene = currentScene->getChildByTag(LAYER_TAG_BLUETOOTH_BLUETOOTH);
	if (scene == nullptr)
	{
		CCLOG("not in bluetooth scene!");
		return;
	}

	BluetoothSence* layer = dynamic_cast<BluetoothSence*>(scene);
	if (layer == nullptr)
	{
		CCLOG("not in bluetooth layer!");
		return;
	}

	layer->addDevice(info);
}

static void BluetoothDiscoveryFinished()
{
	//MessageBox("test","Discovery Finished!");
}

static void DoAfterConnected(const char * deviceName)
{
	auto currentScene = Director::getInstance()->getRunningScene();
	
	Node* scene = currentScene->getChildByTag(LAYER_TAG_BLUETOOTH_BLUETOOTH);
	if (scene == nullptr)
	{
		CCLOG("not in bluetooth scene!");
		return;
	}

	//MessageBox((std::string("connect to ") + std::string(deviceName)).c_str(),"connected!");

	BluetoothSence* layer = dynamic_cast<BluetoothSence*>(scene);
	if (layer == nullptr)
	{
		CCLOG("not in bluetooth layer!");
		return;
	}

	layer->setBluetoothStatus(BluetoothSence::BLUETOOTH_STATUS_TYPE_CONNECTED);
}

static void RecvBluetoothMessage(const char * msg)
{
	rapidjson::Document d;

	std::string clearData(msg);
	size_t pos = clearData.rfind("}");
	clearData = clearData.substr(0, pos+1);
	d.Parse<0>(clearData.c_str());
	if (d.HasParseError()) 
	{
        CCLOG("GetParseError %s\n",d.GetParseError());
    }
	std::string strCmd = d["cmd"].GetString();

	if (strCmd == "ready")
	{
		std::string strCards = d["cards"].GetString();

		auto currentScene = Director::getInstance()->getRunningScene();
	
		Node* scene = currentScene->getChildByTag(LAYER_TAG_BLUETOOTH_BLUETOOTH);
		if (scene == nullptr)
		{
			CCLOG("not in bluetooth scene!");
			return;
		}

		BluetoothSence* layer = dynamic_cast<BluetoothSence*>(scene);
		if (layer == nullptr)
		{
			CCLOG("not in bluetooth layer!");
			return;
		}

		layer->setRandom4cardsString(strCards);
		layer->setOpponentReady();
	}
	else if (strCmd == "showLose")
	{
		std::string strCards = d["cards"].GetString();
		std::string strFormula = d["formula"].GetString();

		auto currentScene = Director::getInstance()->getRunningScene();
	
		Node* scene = currentScene->getChildByTag(LAYER_TAG_BLUETOOTH_MAIN);
		if (scene == nullptr)
		{
			CCLOG("not in main scene!");
			return;
		}

		MainScene* layer = dynamic_cast<MainScene*>(scene);
		if (layer == nullptr)
		{
			CCLOG("not in main layer!");
			return;
		}

		layer->setRandom4cardsString(strCards);
		layer->setFormula(strFormula);
		layer->setOpponentWin();
	}
	else if (strCmd == "playAgain")
	{
		auto currentScene = Director::getInstance()->getRunningScene();
	
		Node* scene = currentScene->getChildByTag(LAYER_TAG_BLUETOOTH_MAIN);
		if (scene == nullptr)
		{
			CCLOG("not in main scene!");
			return;
		}

		Node* layer = scene->getChildByTag(LAYER_TAG_BLUETOOTH_VICTORY);
		if (layer == nullptr)
		{
			CCLOG("not in victory layer!");
			return;
		}

		VictoryLayer* victoryLayer = dynamic_cast<VictoryLayer*>(layer);
		if (victoryLayer == nullptr)
		{
			CCLOG("not in victory layer!");
			return;
		}

		victoryLayer->setOpponentReady();
	}
	else 
	{
	}
}

static void BluetoothConnectFailed()
{
	//MessageBox("test","Connect Failed!");
}

static void BluetoothConnectLost()
{
	//MessageBox("test","Connect Lost!");
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
	void Java_org_cocos2dx_cpp_BluetoothHelper_EnterBluetoothSence(JNIEnv *env, jobject thiz, jboolean discoverable)
    {
		if (discoverable == 0)
		{
			EnterBluetoothSence(false);
		}
		else
		{
			EnterBluetoothSence(true);
		}
    }

	void Java_org_cocos2dx_cpp_BluetoothHelper_SetBluetoothDiscoverable(JNIEnv *env, jobject thiz, jboolean discoverable)
    {
		if (discoverable == 0)
		{
			SetBluetoothDiscoverable(false);
		}
		else
		{
			SetBluetoothDiscoverable(true);
		}
    }

	//void Java_org_cocos2dx_cpp_BluetoothHelper_SetBluetoothStatus(JNIEnv *env, jobject thiz, jint status)
 //   {
	//	SetBluetoothStatus(status);
 //   }

    void Java_org_cocos2dx_cpp_BluetoothHelper_SendBluetoothDeviceInfo(JNIEnv *env, jobject thiz, jstring info)
    {
        const char *temp = env->GetStringUTFChars(info, NULL);
        SendBluetoothDeviceInfo(temp);
        env->ReleaseStringUTFChars(info, temp);
    }

	void Java_org_cocos2dx_cpp_BluetoothHelper_BluetoothDiscoveryFinished(JNIEnv *env, jobject thiz)
    {
		BluetoothDiscoveryFinished();
    }

	void Java_org_cocos2dx_cpp_BluetoothHelper_DoAfterConnected(JNIEnv *env, jobject thiz, jstring deviceName)
    {
        const char *temp = env->GetStringUTFChars(deviceName, NULL);
        DoAfterConnected(temp);
        env->ReleaseStringUTFChars(deviceName, temp);
    }

	void Java_org_cocos2dx_cpp_BluetoothHelper_RecvBluetoothMessage(JNIEnv *env, jobject thiz, jstring msg)
    {
        const char *temp = env->GetStringUTFChars(msg, NULL);
        RecvBluetoothMessage(temp);
        env->ReleaseStringUTFChars(msg, temp);
    }

	void Java_org_cocos2dx_cpp_BluetoothHelper_BluetoothConnectFailed(JNIEnv *env, jobject thiz)
    {
		BluetoothConnectFailed();
    }

	void Java_org_cocos2dx_cpp_BluetoothHelper_BluetoothConnectLost(JNIEnv *env, jobject thiz)
    {
		BluetoothConnectLost();
    }
}
#endif



