#ifndef __BLUETOOTH_SCENE_H__
#define __BLUETOOTH_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "extensions/cocos-ext.h"

class BluetoothSence : public cocos2d::Layer
{
public:
	enum BluetoothStatusType
	{
		BLUETOOTH_STATUS_TYPE_NONE = 0,				// 空状态
		BLUETOOTH_STATUS_TYPE_NO_CONNECT = 1,		// 未连接
		BLUETOOTH_STATUS_TYPE_WAIT_FOR_CONNECT = 2,	// 等待连接
		BLUETOOTH_STATUS_TYPE_DISCOVERING = 3,		// 搜寻玩家中
		BLUETOOTH_STATUS_TYPE_DISCOVER_FINISH = 4,	// 搜寻结束
		BLUETOOTH_STATUS_TYPE_CONNECTING = 5,       // 连接中
		BLUETOOTH_STATUS_TYPE_CONNECTED = 6			// 已连接
	};

    static cocos2d::Scene* scene(bool discoverable);
	virtual bool init(bool discoverable);
	virtual void update(float dt);
	void serverModeButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void clientModeButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void connectServerEvent(Ref *pSender, cocos2d::ui::ListViewEventType type);
	void readyButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void backButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void valueChanged(Ref* sender, cocos2d::extension::Control::EventType controlEvent);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event* event);

	// 下面几个函数被java调用，不会立即生效，会在update时生效
	void setBluetoothDiscoverable(bool discoverable);
	void setBluetoothStatus(BluetoothStatusType status);
	void addDevice(const std::string& deviceInfo);
	void setOpponentReady();
	void setRandom4cardsString(const std::string& random4cardsString);
private:
	void handleBluetoothStatusChange();

	unsigned long _backKeyPressedTime;

	cocos2d::ui::Button* _serverModeButton;
	cocos2d::ui::Layout* _connectLayout;
	cocos2d::ui::ListView* _bluetoothDeviceListView;
	cocos2d::extension::ControlSwitch* _switchControl;
	
	bool _discoverable;
	bool _resetDiscoverable;
	BluetoothStatusType _status;
	BluetoothStatusType _toChangeStatus;
	std::set<std::string> _tobeAddDeviceInfoSet;
	std::set<std::string> _addedDeviceInfoSet;
	bool _selfReady;
	bool _opponentReady;
	std::string _random4cardsString;
};

#endif // __BLUETOOTH_SCENE_H__