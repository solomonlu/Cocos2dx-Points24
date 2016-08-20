/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.widget.Toast;

import org.cocos2dx.cpp.BluetoothChatService;

import com.solomon.points_24.R;

public class AppActivity extends Cocos2dxActivity {
	//fix PageView green problem
	public Cocos2dxGLSurfaceView onCreateView() { 
        Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this); 
        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8); 
 
        return glSurfaceView; 
    } 
	
    private BluetoothAdapter mBluetoothAdapter = null;
	private BluetoothChatService mChatService = null;
	
	public static final int SHOW_TOAST = 0x0003;
	
	public static final int REQUEST_ENABLE_BLUETOOTH = 0x0101;
	public static final int REQUEST_START_BLUETOOTH_SERVER = 0x0102;
	
	public static final int BLUETOOTH_ENABLE_BLUETOOTH = 0x1001;
	public static final int BLUETOOTH_START_BLUETOOTH_SERVER = 0x1002;
	public static final int BLUETOOTH_START_BLUETOOTH_CLIENT = 0x1003;
	public static final int BLUETOOTH_CONNECT_TO_DEVICE = 0x1004;
	public static final int BLUETOOTH_DEVICE_CONNECTED = 0x1005;
	public static final int BLUETOOTH_SEND_MESSAGE = 0x1006;
	public static final int BLUETOOTH_RECV_MESSAGE = 0x1007;
	public static final int BLUETOOTH_CONNECT_FAILED = 0x1008;
	public static final int BLUETOOTH_CONNECT_LOST = 0x1009;
	
	// must same as c++ part
//	public static final int BLUETOOTH_STATUS_NO_CONNECT = 1;
//    public static final int BLUETOOTH_STATUS_WAIT_FOR_CONNECT = 2;
//    public static final int BLUETOOTH_STATUS_DISCOVERING = 3;
//    public static final int BLUETOOTH_STATUS_DISCOVER_FINISH = 4;
//    public static final int BLUETOOTH_STATUS_CONNECTING = 5;
//    public static final int BLUETOOTH_STATUS_CONNECTED = 6;
	@SuppressLint("HandlerLeak")
	private Handler mHandler = new Handler()
	{
		@Override
		public void handleMessage(Message msg)
		{
			switch(msg.what)
			{
				case SHOW_TOAST:
					showToastImpl();
					break;
				case BLUETOOTH_ENABLE_BLUETOOTH:
					enableBluetoothImpl();
					break;
				case BLUETOOTH_START_BLUETOOTH_SERVER:
					startBluetoothServerImpl();
					break;
				case BLUETOOTH_START_BLUETOOTH_CLIENT:
					startBluetoothClientImpl();
					break;
				case BLUETOOTH_CONNECT_TO_DEVICE:
					String deviceInfo = (String) msg.obj;
					connectToDeviceImpl(deviceInfo);
					break;
				case BLUETOOTH_DEVICE_CONNECTED:
					String ConnectedDeviceName = msg.getData().getString("device_name");
					BluetoothHelper.DoAfterConnected(ConnectedDeviceName);
	                break;
				case BLUETOOTH_SEND_MESSAGE:
					String sendMsg = (String) msg.obj;
					Log.i("Bluetooth", "send message:" + sendMsg);
					sendBluetoothMessageImpl(sendMsg);
					break;
				case BLUETOOTH_RECV_MESSAGE:
	                byte[] readBuf = (byte[]) msg.obj;
	                // construct a string from the buffer
	                String recvMsg = new String(readBuf, 0, msg.arg1);
	                Log.i("Bluetooth", "recv message:" + recvMsg);
	                BluetoothHelper.RecvBluetoothMessage(recvMsg);
	                break;
				case BLUETOOTH_CONNECT_FAILED:
					BluetoothHelper.BluetoothConnectFailed();
				case BLUETOOTH_CONNECT_LOST:
					BluetoothHelper.BluetoothConnectLost();
				default:
					break;
			}
		}
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		ToastHelper.init(mHandler);
		BluetoothHelper.init(mHandler);
		
		/// Register bluetooth receiver
		// Register for broadcasts when a device is discovered
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        this.registerReceiver(mReceiver, filter);
        // Register for broadcasts when discovery has finished
        filter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        this.registerReceiver(mReceiver, filter);
        
        filter = new IntentFilter(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED);
        this.registerReceiver(mReceiver, filter);
	}
	
	@Override
    protected void onDestroy() {
        super.onDestroy();

        /// Unregister bluetooth receiver
        // Unregister broadcast listeners
        this.unregisterReceiver(mReceiver);
    }
	
	private void showToastImpl()
	{
		Toast.makeText(this, R.string.toast_text, Toast.LENGTH_LONG).show();
	}
	
	// maybe should move to another .java file to make struct clear
	private void enableBluetoothImpl()
	{
		Log.i("Bluetooth", "enable bluetooth!");
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		if (mBluetoothAdapter == null)
		{
			Log.e("Bluetooth", "Bluetooth is not available");
			// TODO
			// make c++ known
            return;
        }
		
		if (!mBluetoothAdapter.isEnabled()) 
		{
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BLUETOOTH);
        }
		else 
        {
        	doAfterEnableBluetooth();
        }
	}
	
	private void doAfterEnableBluetooth()
	{
        if (mChatService == null)
        {
        	mChatService = new BluetoothChatService(this, mHandler);
        }
        if (mChatService.getState() == BluetoothChatService.STATE_NONE) 
        {
            mChatService.start();
        }
        
        if (mBluetoothAdapter.getScanMode() == BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE)
        {
        	BluetoothHelper.EnterBluetoothSence(true);
        }
        else
        {
        	BluetoothHelper.EnterBluetoothSence(false);
        }
	}
	
	private void startBluetoothServerImpl()
	{   
        if (mBluetoothAdapter.getScanMode() != BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE)
        {
            Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 120);
            startActivityForResult(discoverableIntent,REQUEST_START_BLUETOOTH_SERVER);
        }
	}
	
	private void doAfterStartBluetoothServer()
	{
		//mChatService.start();
		
		//BluetoothHelper.SetBluetoothStatus(BLUETOOTH_STATUS_WAIT_FOR_CONNECT);
	}
	
	private void startBluetoothClientImpl()
	{
		// If we're already discovering, stop it
        if (mBluetoothAdapter.isDiscovering()) {
        	mBluetoothAdapter.cancelDiscovery();
        }

        // Request discover from BluetoothAdapter
        mBluetoothAdapter.startDiscovery();
        Log.i("Bluetooth", "start discovery!");
	}
	
	private void connectToDeviceImpl(final String deviceInfo)
	{
		String deviceName = deviceInfo.split("\\|")[0];
		String deviceMac = deviceInfo.split("\\|")[1];
		Log.i("Bluetooth", "Connect to device:" + deviceName + " mac:" + deviceMac);
		
		BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(deviceMac);
		mChatService.connect(device);
	}
	
	// The BroadcastReceiver that listens for discovered devices and
    // changes the title when discovery is finished
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.i("Bluetooth", "get action:" + action);
            
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
            	BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            	Log.i("Bluetooth", "found new device:" + device.getName() + ",address:" + device.getAddress());
            	BluetoothHelper.SendBluetoothDeviceInfo(device.getName() + "|" + device.getAddress());
            }
            else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
            	Log.i("Bluetooth", "found finished!");
            	BluetoothHelper.BluetoothDiscoveryFinished();
            }
            else if (BluetoothAdapter.ACTION_SCAN_MODE_CHANGED.equals(action)) {            
            	int newMode = intent.getIntExtra(BluetoothAdapter.EXTRA_SCAN_MODE,-1);
        		Log.i("Bluetooth", "scan mode change to " + newMode);
        		if (newMode == BluetoothAdapter.SCAN_MODE_CONNECTABLE)
        		{
        			BluetoothHelper.SetBluetoothDiscoverable(false);
        		}
        		else if (newMode == BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE)
        		{
        			BluetoothHelper.SetBluetoothDiscoverable(true);
        		}
            }
        }
    };
    
    private void sendBluetoothMessageImpl(final String message) 
    {
        // Check that we're actually connected before trying anything
        if (mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            //Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // Check that there's actually something to send
        if (message.length() > 0) {
            // Get the message bytes and tell the BluetoothChatService to write
            byte[] send = message.getBytes();
            mChatService.write(send);
        }
    }
    
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.d("Bluetooth", "onActivityResult " + requestCode + "," + resultCode);
        switch (requestCode) {
	        case REQUEST_ENABLE_BLUETOOTH:
	            if (resultCode == Activity.RESULT_OK) 
	            {
	            	doAfterEnableBluetooth();
	            } 
	            else 
	            {
	            	Log.e("Bluetooth", "Bluetooth enable failed!");
	            	// TODO
	            	// make c++ known
	            }
	            break;
	        case REQUEST_START_BLUETOOTH_SERVER:
	        	if (resultCode == 120) 
	            {
	        		doAfterStartBluetoothServer();
	            }
	            else 
	            {
	            	Log.e("Bluetooth", "Bluetooth server start failed!");
	            	// TODO
	            	// make c++ known
	            }
	            break;
        }
    }
	
	@Override
	protected void onResume() {
		super.onResume();
		
		if (mBluetoothAdapter != null && mBluetoothAdapter.isEnabled())
		{
			if (mBluetoothAdapter.getScanMode() == BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE)
			{
				BluetoothHelper.SetBluetoothDiscoverable(true);
			}
			else
			{
				BluetoothHelper.SetBluetoothDiscoverable(false);
			}
		}
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		return true;
	}
}
