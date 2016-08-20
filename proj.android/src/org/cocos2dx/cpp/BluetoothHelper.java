package org.cocos2dx.cpp;

import android.os.Handler;
import android.os.Message;

public class BluetoothHelper 
{
	private static Handler mHandler;
	
	public static void init(Handler handler)  
    {  
		BluetoothHelper.mHandler = handler;
    }
	
	private static void enableBluetooth()
    {
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.BLUETOOTH_ENABLE_BLUETOOTH;
        msg.sendToTarget();
    }
	
	private static void startBluetoothServer()
    {
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.BLUETOOTH_START_BLUETOOTH_SERVER;
        msg.sendToTarget();
    }
	
	private static void startBluetoothClient()
    {
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.BLUETOOTH_START_BLUETOOTH_CLIENT;
        msg.sendToTarget();
    }
	
	private static void connectToDevice(final String deviceInfo)
    {
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.BLUETOOTH_CONNECT_TO_DEVICE;
        msg.obj = deviceInfo;
        msg.sendToTarget();
    }
	
	private static void sendBluetoothMessage(final String bluetoothMsg)
    {
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.BLUETOOTH_SEND_MESSAGE;
        msg.obj = bluetoothMsg;
        msg.sendToTarget();
    }
	
	public static native void EnterBluetoothSence(boolean discoverable);
	public static native void SetBluetoothDiscoverable(boolean discoverable);
	//public static native void SetBluetoothStatus(int status);
	public static native void SendBluetoothDeviceInfo(String info);
	public static native void BluetoothDiscoveryFinished();
	public static native void DoAfterConnected(String deviceName);
	public static native void RecvBluetoothMessage(String msg);
	public static native void BluetoothConnectFailed();
	public static native void BluetoothConnectLost();
}