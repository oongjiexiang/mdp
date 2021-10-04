package com.example.mdp_android_grp15;

import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.appcompat.app.AppCompatActivity;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import androidx.viewpager.widget.ViewPager;

import com.example.mdp_android_grp15.ui.main.BluetoothConnectionService;
import com.example.mdp_android_grp15.ui.main.BluetoothPopUp;
import com.example.mdp_android_grp15.ui.main.BluetoothChatFragment;
import com.example.mdp_android_grp15.ui.main.ControlFragment;
import com.example.mdp_android_grp15.ui.main.GridMap;
import com.example.mdp_android_grp15.ui.main.MapTabFragment;
import com.example.mdp_android_grp15.ui.main.ReconfigureFragment;
import com.example.mdp_android_grp15.ui.main.SectionsPagerAdapter;
import com.google.android.material.tabs.TabLayout;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.math.BigInteger;
import java.nio.charset.Charset;
import java.util.ResourceBundle;
import java.util.UUID;




public class MainActivity extends AppCompatActivity {

    // Declaration Variables
    private static SharedPreferences sharedPreferences;
    private static SharedPreferences.Editor editor;
    private static Context context;

    private static GridMap gridMap;
    private ControlFragment controlFragment;
    static TextView xAxisTextView, yAxisTextView, directionAxisTextView;
    static TextView robotStatusTextView, bluetoothStatus, bluetoothDevice, exploreTime;
    static Button f1, f2;
    static Button upBtn, downBtn, leftBtn, rightBtn;
    Button reconfigure;
    ReconfigureFragment reconfigureFragment = new ReconfigureFragment();

    BluetoothConnectionService mBluetoothConnection;
    BluetoothDevice mBTDevice;
    private static UUID myUUID;
    ProgressDialog myDialog;

    int count=0;
    String obstacleID = "";
    String imageID = "";

    private static final String TAG = "Main Activity";
    public static boolean stopTimerFlag = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        // Initialization
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        SectionsPagerAdapter sectionsPagerAdapter = new SectionsPagerAdapter(this, getSupportFragmentManager());
        ViewPager viewPager = findViewById(R.id.view_pager);
        viewPager.setAdapter(sectionsPagerAdapter);
        viewPager.setOffscreenPageLimit(9999);
        TabLayout tabs = findViewById(R.id.tabs);
        tabs.setupWithViewPager(viewPager);
        LocalBroadcastManager.getInstance(this).registerReceiver(messageReceiver, new IntentFilter("incomingMessage"));


        // Set up sharedPreferences
        MainActivity.context = getApplicationContext();
        this.sharedPreferences();
        editor.putString("message", "");
        editor.putString("direction","None");
        editor.putString("connStatus", "Disconnected");
        editor.commit();

        /*Button printMDFStringButton = (Button) findViewById(R.id.printMDFString);
        printMDFStringButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String message = "Explored : " + GridMap.getPublicMDFExploration();
                editor = sharedPreferences.edit();
                editor.putString("message", CommsFragment.getMessageReceivedTextView().getText() + "\n" + message);
                editor.commit();
                refreshMessageReceived();
                message = "Obstacle : " + GridMap.getPublicMDFObstacle() + "0";
                editor.putString("message", CommsFragment.getMessageReceivedTextView().getText() + "\n" + message);
                editor.commit();
                refreshMessageReceived();
            }
        });*/

        // Toolbar
        Button bluetoothButton = (Button) findViewById(R.id.bluetoothButton);
        bluetoothButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent popup = new Intent(MainActivity.this, BluetoothPopUp.class);
                startActivity(popup);
            }
        });

        // Bluetooth Status
        bluetoothStatus = findViewById(R.id.bluetoothStatus);
        bluetoothDevice = findViewById(R.id.bluetoothConnectedDevice);


        // Map
        gridMap = new GridMap(this);
        gridMap = findViewById(R.id.mapView);
        xAxisTextView = findViewById(R.id.xAxisTextView);
        yAxisTextView = findViewById(R.id.yAxisTextView);
        directionAxisTextView = findViewById(R.id.directionAxisTextView);

        // ControlFragment for Timer
        controlFragment = new ControlFragment();

        // initialize ITEM_LIST and imageBearings strings
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                gridMap.ITEM_LIST.get(i)[j] = "";
                gridMap.imageBearings.get(i)[j] = "";
            }
        }

        // Controller
        upBtn = findViewById(R.id.upBtn);
        downBtn = findViewById(R.id.downBtn);
        leftBtn = findViewById(R.id.leftBtn);
        rightBtn = findViewById(R.id.rightBtn);


        // Robot Status
        robotStatusTextView = findViewById(R.id.robotStatus);

        myDialog = new ProgressDialog(MainActivity.this);
        myDialog.setMessage("Waiting for other device to reconnect...");
        myDialog.setCancelable(false);
        myDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });

        f1 = (Button) findViewById(R.id.f1ActionButton);
        f2 = (Button) findViewById(R.id.f2ActionButton);
        reconfigure = (Button) findViewById(R.id.configureButton);

        if (sharedPreferences.contains("F1")) {
            f1.setContentDescription(sharedPreferences.getString("F1", ""));
            showLog("setText for f1Btn: " + f1.getContentDescription().toString());
        }
        if (sharedPreferences.contains("F2")) {
            f2.setContentDescription(sharedPreferences.getString("F2", ""));
            showLog("setText for f2Btn: " + f2.getContentDescription().toString());
        }

        f1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showLog("Clicked f1Btn");
                if (!f1.getContentDescription().toString().equals("empty"))
                    MainActivity.printMessage(f1.getContentDescription().toString());
                showLog("f1Btn value: " + f1.getContentDescription().toString());
                showLog("Exiting f1Btn");
            }
        });

        f2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showLog("Clicked f2Btn");
                if (!f2.getContentDescription().toString().equals("empty"))
                    MainActivity.printMessage(f2.getContentDescription().toString());
                showLog("f2Btn value: " + f2.getContentDescription().toString());
                showLog("Exiting f2Btn");
            }
        });

        /*reconfigure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showLog("Clicked reconfigureBtn");
                reconfigureFragment.show(getFragmentManager(), "Reconfigure Fragment");
                showLog("Exiting reconfigureBtn");
            }
        });*/
    }

    /*@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.appbar_menu, menu);
        return true;
    }*/

    public static Button getF1() { return f1; }

    public static Button getF2() { return f2; }

    public static GridMap getGridMap() {
        return gridMap;
    }

    public static TextView getRobotStatusTextView() {  return robotStatusTextView; }

    public static Button getUpBtn() { return upBtn; }
    public static Button getDownBtn() { return downBtn; }
    public static Button getLeftBtn() { return leftBtn; }
    public static Button getRightBtn() { return rightBtn; }


    public static TextView getBluetoothStatus() { return bluetoothStatus; }
    public static TextView getConnectedDevice() { return bluetoothDevice; }

    public static void sharedPreferences() {
        sharedPreferences = MainActivity.getSharedPreferences(MainActivity.context);
        editor = sharedPreferences.edit();
    }

    // Send message to bluetooth
    public static void

    printMessage(String message) {
        showLog("Entering printMessage");
        editor = sharedPreferences.edit();

        if (BluetoothConnectionService.BluetoothConnectionStatus == true) {
            byte[] bytes = message.getBytes(Charset.defaultCharset());
            BluetoothConnectionService.write(bytes);
        }
        showLog(message);
        editor.putString("message", BluetoothChatFragment.getMessageReceivedTextView().getText() + "\n" + message);
        editor.commit();
        refreshMessageReceived();
        showLog("Exiting printMessage");
    }

    // Store received message into string


    public static void printMessage(String name, int x, int y) throws JSONException {
        showLog("Entering printMessage");
        sharedPreferences();

        JSONObject jsonObject = new JSONObject();
        String message;

        switch(name) {
//            case "starting":
            case "waypoint":
                jsonObject.put(name, name);
                jsonObject.put("x", x);
                jsonObject.put("y", y);
                message = name + " (" + x + "," + y + ")";
                break;
            default:
                message = "Unexpected default for printMessage: " + name;
                break;
        }
        editor.putString("message", BluetoothChatFragment.getMessageReceivedTextView().getText() + "\n" + message);
        editor.commit();
        if (BluetoothConnectionService.BluetoothConnectionStatus == true) {
            byte[] bytes = message.getBytes(Charset.defaultCharset());
            BluetoothConnectionService.write(bytes);
        }
        showLog("Exiting printMessage");
    }

    public static void refreshMessageReceived() {
        BluetoothChatFragment.getMessageReceivedTextView().setText(sharedPreferences.getString("message", ""));
    }


    public void refreshDirection(String direction) {
        gridMap.setRobotDirection(direction);
        directionAxisTextView.setText(sharedPreferences.getString("direction",""));
        printMessage("Direction is set to " + direction);
    }

    public static void refreshLabel() {
        xAxisTextView.setText(String.valueOf(gridMap.getCurCoord()[0]-1));
        yAxisTextView.setText(String.valueOf(gridMap.getCurCoord()[1]-1));
        directionAxisTextView.setText(sharedPreferences.getString("direction",""));
    }

    public static void receiveMessage(String message) {
        showLog("Entering receiveMessage");
        sharedPreferences();
        editor.putString("message", sharedPreferences.getString("message", "") + "\n" + message);
        editor.commit();
        showLog("Exiting receiveMessage");
    }

    private static void showLog(String message) {
        Log.d(TAG, message);
    }

    private static SharedPreferences getSharedPreferences(Context context) {
        return context.getSharedPreferences("Shared Preferences", Context.MODE_PRIVATE);
    }

    private BroadcastReceiver mBroadcastReceiver5 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            BluetoothDevice mDevice = intent.getParcelableExtra("Device");
            String status = intent.getStringExtra("Status");
            sharedPreferences();

            if(status.equals("connected")){
                try {
                    myDialog.dismiss();
                } catch(NullPointerException e){
                    e.printStackTrace();
                }

                Log.d(TAG, "mBroadcastReceiver5: Device now connected to "+mDevice.getName());
                Toast.makeText(MainActivity.this, "Device now connected to "+mDevice.getName(), Toast.LENGTH_LONG).show();
                editor.putString("connStatus", "Connected to " + mDevice.getName());
//                TextView connStatusTextView = findViewById(R.id.connStatusTextView);
//                connStatusTextView.setText("Connected to " + mDevice.getName());
            }
            else if(status.equals("disconnected")){
                Log.d(TAG, "mBroadcastReceiver5: Disconnected from "+mDevice.getName());
                Toast.makeText(MainActivity.this, "Disconnected from "+mDevice.getName(), Toast.LENGTH_LONG).show();
//                mBluetoothConnection = new BluetoothConnectionService(MainActivity.this);
//                mBluetoothConnection.startAcceptThread();

                editor.putString("connStatus", "Disconnected");
//                TextView connStatusTextView = findViewById(R.id.connStatusTextView);
//                connStatusTextView.setText("Disconnected");

                myDialog.show();
            }
            editor.commit();
        }
    };

    // message handler
    // alg sends x,y,robotDirection
    // alg sends ALG,<obstacle id>
    // rpi sends RPI,<image id>
    BroadcastReceiver messageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String message = intent.getStringExtra("receivedMessage");
            showLog("receivedMessage: message --- " + message);
            ;

            if(message.contains(",")) {
                String[] cmd = message.split(",");

                // check if string is cmd sent by ALG/RPI to get obstacle/image id
                if (cmd[0].equals("ALG") || cmd[0].equals("RPI")) {
                    if(obstacleID == "")
                        obstacleID = cmd[0].equals("ALG") ? cmd[1] : "";
                    if(imageID == "") {
                        imageID = cmd[0].equals("RPI") ? cmd[1] : "";

                    }

                    //TODO check if fn is working when received imageID = -1

                    // call update fn only when both IDs are obtained
                    if (!(obstacleID.equals("") || imageID.equals(""))) {
//                        imageID = (imageID.equals("-1")) ? "" : imageID;
//                        if (imageID.equals("-1")) {
//                            imageID = "";
//                            obstacleID = "";
//                        }
//                        else{
//                            gridMap.updateIDFromRpi(obstacleID, imageID);
//                            obstacleID = "";
//                            imageID = "";
//                        }
                        gridMap.updateIDFromRpi(obstacleID, imageID);
                        obstacleID = "";
                        imageID = "";

                    }
                } else {

                    // alg sends in cm and float e.g. 100.0,100.0,N
                    float x = Integer.parseInt(cmd[0]);
                    float y = Integer.parseInt(cmd[1]);

                    // process received figures to pass into our fn
                    int a = Math.round(x);
                    int b = Math.round(y);
                    a = (a / 10) + 1;
                    b = (b / 10) + 1;

                    String direction = cmd[2];

                    // update robot pos from cmds sent by algo
                    if (cmd.length == 4) {
                        gridMap.performAlgoCommand(a, b, direction);
                    }

//                    else if (cmd.length == 4){
//                        String command = cmd[3];
//                        gridMap.performAlgoTurning(a, b, direction, command);
//                    }
                }
            }
//            else if(message.length() == 1 && message.length() == 2){
//                gridMap.updateIDFromRpi();
//            }
            // TODO: stop timer when receive "END" message
            else if (message.equals("END")) {
                // if wk 8 btn is checked, means running wk 8 challenge and likewise for wk 9
                // end the corresponding timer
                //controlFragment.exploreButton.setChecked(false);
                ToggleButton exploreButton = findViewById(R.id.exploreToggleBtn2);
                if (exploreButton.isChecked()) {
                    showLog("explorebutton is checked");
                    stopTimerFlag = true;
                    exploreButton.setChecked(false);
                    robotStatusTextView.setText("Auto Movement/ImageRecog Stopped");
                }
//                else if (controlFragment.fastestButton.isChecked()) {
//                    controlFragment.fastestButton.setChecked(false);
//                    robotStatusTextView.setText("Fastest Car Stopped");
//                    timerHandler.removeCallbacks(controlFragment.timerRunnableFastest);
//                }



            }

            try {
                if (message.length() > 7 && message.substring(2,6).equals("grid")) {
                    String resultString = "";
                    String amdString = message.substring(11,message.length()-2);
                    showLog("amdString: " + amdString);
                    BigInteger hexBigIntegerExplored = new BigInteger(amdString, 16);
                    String exploredString = hexBigIntegerExplored.toString(2);

                    while (exploredString.length() < 300)
                        exploredString = "0" + exploredString;

                    for (int i=0; i<exploredString.length(); i=i+15) {
                        int j=0;
                        String subString = "";
                        while (j<15) {
                            subString = subString + exploredString.charAt(j+i);
                            j++;
                        }
                        resultString = subString + resultString;
                    }
                    hexBigIntegerExplored = new BigInteger(resultString, 2);
                    resultString = hexBigIntegerExplored.toString(16);

                    JSONObject amdObject = new JSONObject();
                    amdObject.put("explored", "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
                    amdObject.put("length", amdString.length()*4);
                    amdObject.put("obstacle", resultString);
                    JSONArray amdArray = new JSONArray();
                    amdArray.put(amdObject);
                    JSONObject amdMessage = new JSONObject();
                    amdMessage.put("map", amdArray);
                    message = String.valueOf(amdMessage);
                    showLog("Executed for AMD message, message: " + message);
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }

            try {
                if (message.length() > 8 && message.substring(2,7).equals("image")) {
                    JSONObject jsonObject = new JSONObject(message);
                    JSONArray jsonArray = jsonObject.getJSONArray("image");
                    gridMap.drawImageNumberCell(jsonArray.getInt(0),jsonArray.getInt(1),jsonArray.getInt(2));
                    showLog("Image Added for index: " + jsonArray.getInt(0) + "," +jsonArray.getInt(1));
                }
            } catch (JSONException e) {
                showLog("Adding Image Failed");
            }

            if (gridMap.getAutoUpdate() || MapTabFragment.manualUpdateRequest) {
                try {
                    gridMap.setReceivedJsonObject(new JSONObject(message));
                    gridMap.updateMapInformation();
                    MapTabFragment.manualUpdateRequest = false;
                    showLog("messageReceiver: try decode successful");
                } catch (JSONException e) {
                    showLog("messageReceiver: try decode unsuccessful");
                }
            }
            sharedPreferences();
            String receivedText = sharedPreferences.getString("message", "") + "\n" + message;
            editor.putString("message", receivedText);
            editor.commit();
            refreshMessageReceived();
        }
    };

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
        super.onActivityResult(requestCode, resultCode, data);

        switch (requestCode){
            case 1:
                if(resultCode == Activity.RESULT_OK){
                    mBTDevice = (BluetoothDevice) data.getExtras().getParcelable("mBTDevice");
                    myUUID = (UUID) data.getSerializableExtra("myUUID");
                }
        }
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        try{
            LocalBroadcastManager.getInstance(this).unregisterReceiver(messageReceiver);
            LocalBroadcastManager.getInstance(this).unregisterReceiver(mBroadcastReceiver5);
        } catch(IllegalArgumentException e){
            e.printStackTrace();
        }
    }

    @Override
    protected void onPause(){
        super.onPause();
        try{
            LocalBroadcastManager.getInstance(this).unregisterReceiver(mBroadcastReceiver5);
        } catch(IllegalArgumentException e){
            e.printStackTrace();
        }
    }

    @Override
    protected void onResume(){
        super.onResume();
        try{
            IntentFilter filter2 = new IntentFilter("ConnectionStatus");
            LocalBroadcastManager.getInstance(this).registerReceiver(mBroadcastReceiver5, filter2);
        } catch(IllegalArgumentException e){
            e.printStackTrace();
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        showLog("Entering onSaveInstanceState");
        super.onSaveInstanceState(outState);

        outState.putString(TAG, "onSaveInstanceState");
        showLog("Exiting onSaveInstanceState");
    }
}

