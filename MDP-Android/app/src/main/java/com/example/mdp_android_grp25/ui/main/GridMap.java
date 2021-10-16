package com.example.mdp_android_grp25.ui.main;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ClipData;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.RectF;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.DragEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.Nullable;

import com.example.mdp_android_grp25.MainActivity;
import com.example.mdp_android_grp25.R;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static com.example.mdp_android_grp25.Constants.*;

public class GridMap extends View {

    public GridMap(Context c) {
        super(c);
        initMap();
        setWillNotDraw(false);
    }

    SharedPreferences sharedPreferences;

    private Paint blackPaint = new Paint();
    private Paint whitePaint = new Paint();
    private Paint redPaint = new Paint();
    private Paint obstacleColor = new Paint();
    private Paint robotColor = new Paint();
    private Paint endColor = new Paint();
    private Paint startColor = new Paint();
    private Paint waypointColor = new Paint();
    private Paint unexploredColor = new Paint();
    private Paint exploredColor = new Paint();
    private Paint arrowColor = new Paint();
    private Paint fastestPathColor = new Paint();

    private static JSONObject receivedJsonObject = new JSONObject();
    private static JSONObject backupMapInformation;
    private static String robotDirection = "None";
    private static int[] startCoord = new int[]{-1, -1};
    private static int[] curCoord = new int[]{-1, -1};
    private static int[] oldCoord = new int[]{-1, -1};
    private static int[] waypointCoord = new int[]{-1, -1};
    private static ArrayList<String[]> arrowCoord = new ArrayList<>();
    private static ArrayList<int[]> obstacleCoord = new ArrayList<>();
    private static boolean autoUpdate = false;
    private static boolean canDrawRobot = false;
    private static boolean setWaypointStatus = false;
    private static boolean startCoordStatus = false;
    private static boolean setObstacleStatus = false;
    private static boolean unSetCellStatus = false;
    private static boolean setExploredStatus = false;
    private static boolean validPosition = false;
    private Bitmap arrowBitmap = BitmapFactory.decodeResource(getResources(),
            R.drawable.ic_arrow_error);

    private static final String TAG = "GridMap";
    private static final int COL = 20;
    private static final int ROW = 20;
    private static float cellSize;
    private static Cell[][] cells;

    private boolean mapDrawn = false;
    public static String publicMDFExploration;
    public static String publicMDFObstacle;

    public ArrayList<String[]> ITEM_LIST = new ArrayList<>(Arrays.asList(
            new String[20], new String[20], new String[20], new String[20], new String[20],
            new String[20], new String[20], new String[20], new String[20], new String[20],
            new String[20], new String[20], new String[20], new String[20], new String[20],
            new String[20], new String[20], new String[20], new String[20], new String[20]
    ));
    public ArrayList<String[]> imageBearings = new ArrayList<>(Arrays.asList(
            new String[20], new String[20], new String[20], new String[20], new String[20],
            new String[20], new String[20], new String[20], new String[20], new String[20],
            new String[20], new String[20], new String[20], new String[20], new String[20],
            new String[20], new String[20], new String[20], new String[20], new String[20]
    ));

    static ClipData clipData;
    static Object localState;
    int initialColumn, initialRow;

    public GridMap(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initMap();
        blackPaint.setStyle(Paint.Style.FILL_AND_STROKE);
        whitePaint.setColor(Color.WHITE);
        whitePaint.setTextSize(25);
        whitePaint.setTextAlign(Paint.Align.CENTER);
        redPaint.setColor(Color.RED);
        redPaint.setStrokeWidth(8);
        obstacleColor.setColor(Color.BLACK);
        robotColor.setColor(Color.YELLOW);
        robotColor.setStrokeWidth(2);
        endColor.setColor(Color.RED);
        startColor.setColor(Color.CYAN);
        waypointColor.setColor(Color.GREEN);
        unexploredColor.setColor(Color.LTGRAY);
        exploredColor.setColor(Color.WHITE);
        arrowColor.setColor(Color.BLACK);
        fastestPathColor.setColor(Color.MAGENTA);

        // get shared preferences
        sharedPreferences = getContext().getSharedPreferences("Shared Preferences",
                Context.MODE_PRIVATE);
    }

    private void initMap() {
        setWillNotDraw(false);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        showLog("Entering onDraw");
        showLog("canDrawRobot = " + String.valueOf(getCanDrawRobot()));
        super.onDraw(canvas);
        showLog("Redrawing map");

        // Create cell coords
        Log.d(TAG,"Creating Cell");

        if (!mapDrawn) {
            String[] dummyArrowCoord = new String[3];
            dummyArrowCoord[0] = "1";
            dummyArrowCoord[1] = "1";
            dummyArrowCoord[2] = "dummy";
            arrowCoord.add(dummyArrowCoord);
            this.createCell();
            mapDrawn = true;
        }

        drawIndividualCell(canvas);
        drawHorizontalLines(canvas);
        drawVerticalLines(canvas);
        drawGridNumber(canvas);
        if (getCanDrawRobot())
            drawRobot(canvas, curCoord);
        drawArrow(canvas, arrowCoord);
        drawObstacles(canvas);

        showLog("Exiting onDraw");
    }

    // draws obstacle cells whenever map refreshes, pos/correct item selection is here
    private void drawObstacles(Canvas canvas) {
        showLog("Entering drawObstacles");

        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                // draw image id
                canvas.drawText(
                    ITEM_LIST.get(19-i)[j],
                    cells[j+1][19-i].startX + ((cells[1][1].endX - cells[1][1].startX) / 2),
                    cells[j+1][i].startY + ((cells[1][1].endY - cells[1][1].startY) / 2) + 10,
                    whitePaint
                );

                // color the face direction
                switch (imageBearings.get(19-i)[j]) {
                    case "North":
                        canvas.drawLine(
                            cells[j + 1][20 - i].startX,
                            cells[j + 1][i].startY,
                            cells[j + 1][20 - i].endX,
                            cells[j + 1][i].startY,
                            redPaint
                        );
                        break;
                    case "South":
                        canvas.drawLine(
                            cells[j + 1][20 - i].startX,
                            cells[j + 1][i].startY + cellSize,
                            cells[j + 1][20 - i].endX,
                            cells[j + 1][i].startY + cellSize,
                            redPaint
                        );
                        break;
                    case "East":
                        canvas.drawLine(
                            cells[j + 1][20 - i].startX + cellSize,
                            cells[j + 1][i].startY,
                            cells[j + 1][20 - i].startX + cellSize,
                            cells[j + 1][i].endY,
                            redPaint
                        );
                        break;
                    case "West":
                        canvas.drawLine(
                            cells[j + 1][20 - i].startX,
                            cells[j + 1][i].startY,
                            cells[j + 1][20 - i].startX,
                            cells[j + 1][i].endY,
                            redPaint
                        );
                        break;
                }
            }
        }
        showLog("Exiting drawObstacles");
    }

    private void drawIndividualCell(Canvas canvas) {
        showLog("Entering drawIndividualCell");
        for (int x = 1; x <= COL; x++)
            for (int y = 0; y < ROW; y++)
                for (int i = 0; i < this.getArrowCoord().size(); i++)
                    if (!cells[x][y].type.equals("image") && cells[x][y].getId() == -1) {
                        canvas.drawRect(
                            cells[x][y].startX,
                            cells[x][y].startY,
                            cells[x][y].endX,
                            cells[x][y].endY,
                            cells[x][y].paint
                        );
                    } else {
                        Paint textPaint = new Paint();
                        textPaint.setTextSize(20);
                        textPaint.setColor(Color.WHITE);
                        textPaint.setTextAlign(Paint.Align.CENTER);
                        canvas.drawRect(
                            cells[x][y].startX,
                            cells[x][y].startY,
                            cells[x][y].endX,
                            cells[x][y].endY,
                            cells[x][y].paint
                        );
                        canvas.drawText(
                            String.valueOf(cells[x][y].getId()),
                            (cells[x][y].startX+cells[x][y].endX)/2,
                            cells[x][y].endY + (cells[x][y].startY-cells[x][y].endY)/4,
                            textPaint
                        );
                    }
        showLog("Exiting drawIndividualCell");
    }

    private void drawHorizontalLines(Canvas canvas) {
        for (int y = 0; y <= ROW; y++)
            canvas.drawLine(
                cells[1][y].startX,
                cells[1][y].startY - (cellSize / 30),
                cells[20][y].endX,
                cells[20][y].startY - (cellSize / 30),
                blackPaint
            );
    }

    private void drawVerticalLines(Canvas canvas) {
        for (int x = 0; x <= COL; x++)
            canvas.drawLine(
                cells[x][0].startX - (cellSize / 30) + cellSize,
                cells[x][0].startY - (cellSize / 30),
                cells[x][0].startX - (cellSize / 30) + cellSize,
                cells[x][19].endY + (cellSize / 30),
                blackPaint
            );
    }

    // this draws the axis numbers
    private void drawGridNumber(Canvas canvas) {
        showLog("Entering drawGridNumber");
        for (int x = 1; x <= COL; x++) {
            if (x > 9)
                canvas.drawText(
                    Integer.toString(x-1),
                    cells[x][20].startX + (cellSize / 5),
                    cells[x][20].startY + (cellSize / 3),
                    blackPaint
                );
            else
                canvas.drawText(
                    Integer.toString(x-1),
                    cells[x][20].startX + (cellSize / 3),
                    cells[x][20].startY + (cellSize / 3),
                    blackPaint
                );
        }
        for (int y = 0; y < ROW; y++) {
            if ((20 - y) > 9)
                canvas.drawText(
                    Integer.toString(19 - y),
                    cells[0][y].startX + (cellSize / 2),
                    cells[0][y].startY + (cellSize / 1.5f),
                    blackPaint
                );
            else
                canvas.drawText(
                    Integer.toString(19 - y),
                    cells[0][y].startX + (cellSize / 1.5f),
                    cells[0][y].startY + (cellSize / 1.5f),
                    blackPaint
                );
        }
        showLog("Exiting drawGridNumber");
    }

    private void drawRobot(Canvas canvas, int[] curCoord) {
        showLog("Entering drawRobot");
        showLog("curCoord[0] = " + curCoord[0] + ", curCoord[1] = " + curCoord[1]);
        int androidRowCoord = curCoord[1];

        if ((androidRowCoord-1) < 0 || androidRowCoord > 19) {
            showLog("row is out of bounds");
            return;
        } else if (curCoord[0] > 20 || curCoord[0] < 2) {
            showLog("col is out of bounds");
            return;
        } else {
            // draws the 2x2 squares in colour robotColor (green)
            // horizontal lines
            for (int y = androidRowCoord - 2; y <= androidRowCoord; y++) {
                canvas.drawLine(
                    cells[curCoord[0] - 1][21 - y - 2].startX,
                    cells[curCoord[0]][21 - y - 2].startY,
                    cells[curCoord[0]][21 - y - 2].endX,
                    cells[curCoord[0]][21 - y - 2].startY,
                    robotColor
                );
            }
            // vertical lines
            for (int x = curCoord[0] - 2; x <= curCoord[0]; x++) {
                canvas.drawLine(
                    cells[x][21 - androidRowCoord - 1].endX,
                    cells[x][21 - androidRowCoord - 1].endY,
                    cells[x][21 - androidRowCoord - 1].endX,
                    cells[x][21 - androidRowCoord - 2].startY,
                    robotColor
                );
            }

            // use cells[initalCol][20 - initialRow] as ref
            switch (this.getRobotDirection()) {
                case "up":
                    canvas.drawLine(
                        cells[curCoord[0] - 1][20 - androidRowCoord].startX,
                        cells[curCoord[0] - 1][20 - androidRowCoord].endY,
                        cells[curCoord[0] - 1][20 - androidRowCoord].endX,
                        cells[curCoord[0]][20 - androidRowCoord - 1].startY,
                        blackPaint
                    );
                    canvas.drawLine(
                        cells[curCoord[0]][21 - androidRowCoord].endX,
                        cells[curCoord[0]][21 - androidRowCoord - 1].endY,
                        cells[curCoord[0] - 1][21 - androidRowCoord].endX,
                        cells[curCoord[0]][21 - androidRowCoord - 2].startY,
                        blackPaint
                    );
                    break;
                case "down":
                    canvas.drawLine(
                        cells[curCoord[0]][21 - androidRowCoord].endX,
                        cells[curCoord[0] - 1][21 - androidRowCoord - 2].startY,
                        cells[curCoord[0] - 1][21 - androidRowCoord].endX,
                        cells[curCoord[0]][21 - androidRowCoord].startY,
                        blackPaint
                    );
                    canvas.drawLine(
                        cells[curCoord[0] - 2][21 - androidRowCoord].endX,
                        cells[curCoord[0] - 1][21 - androidRowCoord - 2].startY,
                        cells[curCoord[0] - 1][21 - androidRowCoord].endX,
                        cells[curCoord[0]][21 - androidRowCoord].startY,
                        blackPaint
                    );
                    break;
                case "right":
                    canvas.drawLine(
                        cells[curCoord[0] - 1][21 - androidRowCoord - 2].startX,
                        cells[curCoord[0] - 1][21 - androidRowCoord - 2].startY,
                        cells[curCoord[0]][21 - androidRowCoord - 1].endX,
                        cells[curCoord[0]][21 - androidRowCoord - 1].startY,
                        blackPaint
                    );
                    canvas.drawLine(
                        cells[curCoord[0] - 1][21 - androidRowCoord - 2].startX,
                        cells[curCoord[0] - 1][21 - androidRowCoord].startY,
                        cells[curCoord[0]][21 - androidRowCoord - 1].endX,
                        cells[curCoord[0]][21 - androidRowCoord - 1].startY,
                        blackPaint
                    );
                    break;
                case "left":
                    canvas.drawLine(
                        cells[curCoord[0]][21 - androidRowCoord - 1].endX,
                        cells[curCoord[0] - 1][21 - androidRowCoord - 1].endY,
                        cells[curCoord[0] - 2][21 - androidRowCoord].endX,
                        cells[curCoord[0]][21 - androidRowCoord - 1].startY,
                        blackPaint
                    );
                    canvas.drawLine(
                        cells[curCoord[0]][21 - androidRowCoord - 1].endX,
                        cells[curCoord[0] - 1][21 - androidRowCoord - 2].startY,
                        cells[curCoord[0] - 2][21 - androidRowCoord].endX,
                        cells[curCoord[0]][21 - androidRowCoord - 1].startY,
                        blackPaint
                    );
                    break;
                default:
                    Toast.makeText(
                        this.getContext(),
                        "Error with drawing robot (unknown direction)",
                        Toast.LENGTH_LONG
                    ).show();
                    break;
            }
        }
        showLog("Exiting drawRobot");
    }

    private ArrayList<String[]> getArrowCoord() {
        return arrowCoord;
    }

    public String getRobotDirection() {
        return robotDirection;
    }

    public void setAutoUpdate(boolean autoUpdate) throws JSONException {
        showLog(String.valueOf(backupMapInformation));
        if (!autoUpdate)
            backupMapInformation = this.getReceivedJsonObject();
        else {
            setReceivedJsonObject(backupMapInformation);
            backupMapInformation = null;
            this.updateMapInformation();
        }
        GridMap.autoUpdate = autoUpdate;
    }

    public JSONObject getReceivedJsonObject() {
        return receivedJsonObject;
    }

    public void setReceivedJsonObject(JSONObject receivedJsonObject) {
        showLog("Entered setReceivedJsonObject");
        GridMap.receivedJsonObject = receivedJsonObject;
        backupMapInformation = receivedJsonObject;
    }

    public boolean getAutoUpdate() {
        return autoUpdate;
    }

    private void setValidPosition(boolean status) {
        validPosition = status;
    }

    public boolean getValidPosition() {
        return validPosition;
    }

    public void setSetObstacleStatus(boolean status) {
        setObstacleStatus = status;
    }

    public boolean getSetObstacleStatus() {
        return setObstacleStatus;
    }

    public void setStartCoordStatus(boolean status) {
        startCoordStatus = status;
    }

    private boolean getStartCoordStatus() {
        return startCoordStatus;
    }

    public void setWaypointStatus(boolean status) {
        setWaypointStatus = status;
    }

    public boolean getCanDrawRobot() {
        return canDrawRobot;
    }

    private void createCell() {
        showLog("Entering cellCreate");
        cells = new Cell[COL + 1][ROW + 1];
        this.calculateDimension();
        cellSize = this.getCellSize();

        for (int x = 0; x <= COL; x++)
            for (int y = 0; y <= ROW; y++)
                cells[x][y] = new Cell(
                    x * cellSize + (cellSize / 30),
                    y * cellSize + (cellSize / 30),
                    (x + 1) * cellSize,
                    (y + 1) * cellSize,
                    unexploredColor,
                    "unexplored"
                );
        showLog("Exiting createCell");
    }

    public void setStartCoord(int col, int row) {
        showLog("Entering setStartCoord");
        startCoord[0] = col;
        startCoord[1] = row;
        String direction = getRobotDirection();
        if(direction.equals("None")) {
            direction = "up";
        }
        if (this.getStartCoordStatus())
            this.setCurCoord(col, row, direction);
        showLog("Exiting setStartCoord");
    }

    private int[] getStartCoord() {
        return startCoord;
    }

    public void setCurCoord(int col, int row, String direction) {
        showLog("Entering setCurCoord");
        // this 2 ifs check setstartingwaypoint
        if (row < 1 || row > 19) {
            showLog("y is out of bounds");
            return;
        }
        if (col > 20 || col < 2) {
            showLog("x is out of bounds");
            return;
        }
        curCoord[0] = col;
        curCoord[1] = row;
        this.setRobotDirection(direction);
        this.updateRobotAxis(col, row, direction);

        row = this.convertRow(row);

        for (int x = col - 1; x <= col; x++)
            for (int y = row - 1; y <= row; y++)
                cells[x][y].setType("robot");

        showLog("Exiting setCurCoord");
    }

    public int[] getCurCoord() {
        return curCoord;
    }

    private void calculateDimension() {
        this.setCellSize(getWidth()/(COL+1));
    }

    private int convertRow(int row) {
        return (20 - row);
    }

    private void setCellSize(float cellSize) {
        GridMap.cellSize = cellSize;
    }

    private float getCellSize() {
        return cellSize;
    }

    private void setOldRobotCoord(int oldCol, int oldRow) {
        showLog("Entering setOldRobotCoord");
        oldCoord[0] = oldCol;
        oldCoord[1] = oldRow;
        oldRow = this.convertRow(oldRow);

        if (oldRow == 0) {
            showLog("oldRow has gone out of grid.");
            return;
        }
        for (int x = oldCol - 1; x <= oldCol; x++)
            for (int y = oldRow - 1; y <= oldRow; y++)
                cells[x][y].setType("explored");
        showLog("Exiting setOldRobotCoord");
    }

    private int[] getOldRobotCoord() {
        return oldCoord;
    }

    private void setArrowCoordinate(int col, int row, String arrowDirection) {
        showLog("Entering setArrowCoordinate");
        int[] obstacleCoord = new int[]{col, row};
        this.getObstacleCoord().add(obstacleCoord);
        String[] arrowCoord = new String[3];
        arrowCoord[0] = String.valueOf(col);
        arrowCoord[1] = String.valueOf(row);
        arrowCoord[2] = arrowDirection;
        this.getArrowCoord().add(arrowCoord);

        row = convertRow(row);
        cells[col][row].setType("arrow");
        showLog("Exiting setArrowCoordinate");
    }

    public void setRobotDirection(String direction) {
        sharedPreferences = getContext().getSharedPreferences("Shared Preferences",
                Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        robotDirection = direction;
        editor.putString("direction", direction);
        editor.commit();
        this.invalidate();;
    }

    private void updateRobotAxis(int col, int row, String direction) {
        TextView xAxisTextView =  ((Activity)this.getContext()).findViewById(R.id.xAxisTextView);
        TextView yAxisTextView =  ((Activity)this.getContext()).findViewById(R.id.yAxisTextView);
        TextView directionAxisTextView =  ((Activity)this.getContext())
                .findViewById(R.id.directionAxisTextView);

        xAxisTextView.setText(String.valueOf(col-1));
        yAxisTextView.setText(String.valueOf(row-1));
        directionAxisTextView.setText(direction);
    }

    private void setWaypointCoord(int col, int row) throws JSONException {
        showLog("Entering setWaypointCoord");
        waypointCoord[0] = col;
        waypointCoord[1] = row;

        row = this.convertRow(row);
        cells[col][row].setType("waypoint");

        MainActivity.printMessage("waypoint", waypointCoord[0]-1, waypointCoord[1]-1);
        showLog("Exiting setWaypointCoord");
    }

    private int[] getWaypointCoord() {
        return waypointCoord;
    }

    public void setObstacleCoord(int col, int row) {
        showLog("Entering setObstacleCoord");
        int[] obstacleCoord = new int[]{col - 1, row - 1};
        GridMap.obstacleCoord.add(obstacleCoord);
        row = this.convertRow(row);
        cells[col][row].setType("obstacle");
        showLog("Exiting setObstacleCoord");
    }

    private ArrayList<int[]> getObstacleCoord() {
        return obstacleCoord;
    }

    private static void showLog(String message) {
        Log.d(TAG, message);
    }

    private void drawArrow(Canvas canvas, ArrayList<String[]> arrowCoord) {
        showLog("Entering drawArrow");
        RectF rect;

        for (int i = 0; i < arrowCoord.size(); i++) {
            if (!arrowCoord.get(i)[2].equals("dummy")) {
                int col = Integer.parseInt(arrowCoord.get(i)[0]);
                int row = convertRow(Integer.parseInt(arrowCoord.get(i)[1]));
                rect = new RectF(col * cellSize, row * cellSize,
                        (col + 1) * cellSize, (row + 1) * cellSize);
                switch (arrowCoord.get(i)[2]) {
                    case "up":
                        arrowBitmap = BitmapFactory.decodeResource(getResources(),
                                R.drawable.ic_arrow_up);
                        break;
                    case "right":
                        arrowBitmap = BitmapFactory.decodeResource(getResources(),
                                R.drawable.ic_arrow_right);
                        break;
                    case "down":
                        arrowBitmap = BitmapFactory.decodeResource(getResources(),
                                R.drawable.ic_arrow_down);
                        break;
                    case "left":
                        arrowBitmap = BitmapFactory.decodeResource(getResources(),
                                R.drawable.ic_arrow_left);
                        break;
                    default:
                        break;
                }
                canvas.drawBitmap(arrowBitmap, null, rect, null);
            }
            showLog("Exiting drawArrow");
        }
    }

    private class Cell {
        float startX, startY, endX, endY;
        Paint paint;
        String type;
        int id = -1;

        private Cell(float startX, float startY, float endX, float endY, Paint paint, String type) {
            this.startX = startX;
            this.startY = startY;
            this.endX = endX;
            this.endY = endY;
            this.paint = paint;
            this.type = type;
        }

        public void setType(String type) {
            this.type = type;
            switch (type) {
                case "obstacle":
                    this.paint = obstacleColor;
                    break;
                case "robot":
                    this.paint = robotColor;
                    break;
                case "end":
                    this.paint = endColor;
                    break;
                case "start":
                    this.paint = startColor;
                    break;
                case "waypoint":
                    this.paint = waypointColor;
                    break;
                case "unexplored":
                    this.paint = unexploredColor;
                    break;
                case "explored":
                    this.paint = exploredColor;
                    break;
                case "arrow":
                    this.paint = arrowColor;
                    break;
                case "fastestPath":
                    this.paint = fastestPathColor;
                    break;
                case "image":
                    this.paint = obstacleColor;
                default:
                    showLog("setType default: " + type);
                    break;
            }
        }

        public void setId(int id) {
            this.id = id;
        }

        public int getId() {
            return this.id;
        }
    }

    int endColumn, endRow;
    String oldItem;
    // drag event to move obstacle
    @Override
    public boolean onDragEvent(DragEvent dragEvent) {
        showLog("Entering onDragEvent");
        clipData = dragEvent.getClipData();
        localState = dragEvent.getLocalState();

        String tempID, tempBearing;
        tempID = tempBearing = "";
        endColumn = endRow = -999;
        oldItem = ITEM_LIST.get(initialRow - 1)[initialColumn - 1];
        showLog("dragEvent.getAction() == " + dragEvent.getAction());
        showLog("dragEvent.getResult() is " + dragEvent.getResult());
        showLog("initialColumn = " + initialColumn + ", initialRow = " + initialRow);

        // drag and drop out of gridmap
        if ((dragEvent.getAction() == DragEvent.ACTION_DRAG_ENDED)
                && (endColumn == -999 || endRow == -999) && dragEvent.getResult() == false) {
            // check if 2 arrays are same, then remove
            for (int i = 0; i < obstacleCoord.size(); i++) {
                if (Arrays.equals(obstacleCoord.get(i), new int[]{initialColumn - 1, initialRow - 1}))
                    obstacleCoord.remove(i);
            }
            cells[initialColumn][20-initialRow].setType("unexplored");
            ITEM_LIST.get(initialRow-1)[initialColumn-1] = "";
            imageBearings.get(initialRow-1)[initialColumn-1] = "";
            showLog(commandMsgGenerator(REMOVE_OBSTACLE));
            // commented out for Wk 8 and Wk 9
//            MainActivity.printMessage(commandMsgGenerator(REMOVE_OBSTACLE));
        }
        // drop within gridmap
        else if (dragEvent.getAction() == DragEvent.ACTION_DROP && this.getAutoUpdate() == false) {
            endColumn = (int) (dragEvent.getX() / cellSize);
            endRow = this.convertRow((int) (dragEvent.getY() / cellSize));

            // if the currently dragged cell is empty, do nothing
            if (ITEM_LIST.get(initialRow-1)[initialColumn-1].equals("")
                    && imageBearings.get(initialRow-1)[initialColumn-1].equals("")) {
                showLog("Cell is empty");
            }
            // if dropped within mapview but outside drawn grids, remove obstacle from lists
            else if (endColumn <= 0 || endRow <= 0) {
                for (int i = 0; i < obstacleCoord.size(); i++) {
                    if (Arrays.equals(obstacleCoord.get(i),
                            new int[]{initialColumn - 1, initialRow - 1}))
                        obstacleCoord.remove(i);
                }
                cells[initialColumn][20-initialRow].setType("unexplored");
                ITEM_LIST.get(initialRow-1)[initialColumn-1] = "";
                imageBearings.get(initialRow-1)[initialColumn-1] = "";
                showLog(commandMsgGenerator(REMOVE_OBSTACLE));
                // commented out for Wk 8 and Wk 9
//                MainActivity.printMessage(commandMsgGenerator(REMOVE_OBSTACLE));
            }
            // if dropped within gridmap, shift it to new position unless already got existing
            else if ((1 <= initialColumn && initialColumn <= 20)
                    && (1 <= initialRow && initialRow <= 20)
                    && (1 <= endColumn && endColumn <= 20)
                    && (1 <= endRow && endRow <= 20)) {
                tempID = ITEM_LIST.get(initialRow-1)[initialColumn-1];
                tempBearing = imageBearings.get(initialRow-1)[initialColumn-1];

                // check if got existing obstacle at drop location
                if (ITEM_LIST.get(endRow-1)[endColumn-1] != ""
                        || imageBearings.get(endRow-1)[endColumn-1] != "") {
                    showLog("An obstacle is already at drop location");
                } else {
                    ITEM_LIST.get(initialRow - 1)[initialColumn - 1] = "";
                    imageBearings.get(initialRow - 1)[initialColumn - 1] = "";
                    ITEM_LIST.get(endRow - 1)[endColumn - 1] = tempID;
                    imageBearings.get(endRow - 1)[endColumn - 1] = tempBearing;

                    setObstacleCoord(endColumn, endRow);
                    for (int i = 0; i < obstacleCoord.size(); i++) {
                        if (Arrays.equals(obstacleCoord.get(i),
                                new int[]{initialColumn - 1, initialRow - 1}))
                            obstacleCoord.remove(i);
                    }
                    cells[initialColumn][20 - initialRow].setType("unexplored");
                    showLog(commandMsgGenerator(MOVE_OBSTACLE));
                    // commented out for Wk 8 and Wk 9
//                    MainActivity.printMessage(commandMsgGenerator(MOVE_OBSTACLE));
                }
            } else {
                showLog("Drag event failed.");
            }
        }
        showLog("initialColumn = " + initialColumn
                + ", initialRow = " + initialRow
                + "\nendColumn = " + endColumn
                + ", endRow = " + endRow);
        this.invalidate();
        return true;
    }

    public void callInvalidate() {
        showLog("Entering callinvalidate");
        this.invalidate();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        showLog("Entering onTouchEvent");
        if (event.getAction() == MotionEvent.ACTION_DOWN && this.getAutoUpdate() == false) {
            int column = (int) (event.getX() / cellSize);
            int row = this.convertRow((int) (event.getY() / cellSize));
            initialColumn = column;
            initialRow = row;

            ToggleButton setStartPointToggleBtn = ((Activity)this.getContext())
                                                    .findViewById(R.id.startpointToggleBtn);
            ToggleButton setWaypointToggleBtn = ((Activity)this.getContext())
                                                    .findViewById(R.id.waypointToggleBtn);
            showLog("event.getX = " + event.getX() + ", event.getY = " + event.getY());
            showLog("row = " + row + ", column = " + column);

            // start drag
            if (MapTabFragment.dragStatus) {
                if (!((1 <= initialColumn && initialColumn <= 20)
                        && (1 <= initialRow && initialRow <= 20))) {
                    return false;
                } else if (ITEM_LIST.get(row - 1)[column - 1].equals("")
                    && imageBearings.get(row - 1)[column - 1].equals("")) {
                    return false;
                }
                DragShadowBuilder dragShadowBuilder = new MyDragShadowBuilder(this);
                this.startDrag(null, dragShadowBuilder, null, 0);
            }

            // start change obstacle
            if (MapTabFragment.changeObstacleStatus) {
                if (!((1 <= initialColumn && initialColumn <= 20)
                        && (1 <= initialRow && initialRow <= 20))) {
                    return false;
                } else if (ITEM_LIST.get(row - 1)[column - 1] == ""
                        && imageBearings.get(row - 1)[column - 1] == "") {
                    return false;
                } else {
                    showLog("Enter change obstacle status");
                    String imageId = ITEM_LIST.get(row -1)[column - 1];
                    String imageBearing = imageBearings.get(row - 1)[column - 1];
                    final int tRow = row;
                    final int tCol = column;

                    AlertDialog.Builder mBuilder = new AlertDialog.Builder(this.getContext());
                    View mView = ((Activity) this.getContext()).getLayoutInflater()
                            .inflate(R.layout.activity_dialog_change_obstacle,
                                    null);
                    mBuilder.setTitle("Change Existing Obstacle ID/Bearing");
                    final Spinner mIDSpinner = mView.findViewById(R.id.imageIDSpinner2);
                    final Spinner mBearingSpinner = mView.findViewById(R.id.bearingSpinner2);

                    ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
                            this.getContext(), R.array.imageID_array,
                            android.R.layout.simple_spinner_item);
                    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                    mIDSpinner.setAdapter(adapter);
                    ArrayAdapter<CharSequence> adapter2 = ArrayAdapter.createFromResource(
                            this.getContext(), R.array.imageBearing_array,
                            android.R.layout.simple_spinner_item);
                    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                    mBearingSpinner.setAdapter(adapter2);

                    // start at current id and bearing
                    if (imageId.equals("")) {
                        mIDSpinner.setSelection(0);
                    } else {
                        mIDSpinner.setSelection(Integer.parseInt(imageId) - 1);
                    }
                    switch (imageBearing) {
                        case "North": mBearingSpinner.setSelection(0);
                            break;
                        case "South": mBearingSpinner.setSelection(1);
                            break;
                        case "East": mBearingSpinner.setSelection(2);
                            break;
                        case "West": mBearingSpinner.setSelection(3);
                    }

                    // do what when user presses ok
                    mBuilder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            String newID = mIDSpinner.getSelectedItem().toString();
                            String newBearing = mBearingSpinner.getSelectedItem().toString();

                            ITEM_LIST.get(tRow - 1)[tCol - 1] = newID;
                            imageBearings.get(tRow - 1)[tCol - 1] = newBearing;
                            showLog("tRow - 1 = " + (tRow - 1));
                            showLog("tCol - 1 = " + (tCol - 1));
                            showLog("newID = " + newID);
                            showLog("newBearing = " + newBearing);

                            callInvalidate();
                        }
                    });

                    // dismiss
                    mBuilder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            dialogInterface.dismiss();
                        }
                    });

                    mBuilder.setView(mView);
                    AlertDialog dialog = mBuilder.create();
                    dialog.show();
                    Window window =  dialog.getWindow();
                    WindowManager.LayoutParams layoutParams = new WindowManager.LayoutParams();
                    layoutParams.width = 150;
                    window.setLayout(layoutParams.WRAP_CONTENT, layoutParams.WRAP_CONTENT);
                }
                showLog("Exit change obstacle");
            }

            // change robot size and make sure its within the grid
            if (startCoordStatus) {
                if (canDrawRobot) {
                    // removes green grids when user changes robot startpoint
                    for (int i = 0; i < 21; i++) {
                        for (int j = 0; j < 21; j++) {
                            if (cells[i][j].type == "robot") {
                                cells[i][j].setType("explored");
                            }
                        }
                    }
                    // don't set robot if obstacles are there
                    int[] startCoord = this.getStartCoord();

                    if (startCoord[0] >= 2 && startCoord[1] >= 2) {
                        showLog("startCoord = " + startCoord[0] + " " + startCoord[1]);
                        for (int x = startCoord[0] - 1; x <= startCoord[0]; x++)
                            for (int y = startCoord[1] - 1; y <= startCoord[1]; y++)
                                cells[x][y].setType("unexplored");
                    }
                }
                else
                    canDrawRobot = true;
                showLog("curCoord[0] = " + curCoord[0] + ", curCoord[1] = " + curCoord[1]);
                showLog("");
                this.setStartCoord(column, row);
                startCoordStatus = false;
                String direction = getRobotDirection();
                if(direction.equals("None")) {
                    direction = "up";
                }
                try {
                    int directionInt = 0;
                    if(direction.equals("up")){
                        directionInt = 0;
                    } else if(direction.equals("left")) {
                        directionInt = 3;
                    } else if(direction.equals("right")) {
                        directionInt = 1;
                    } else if(direction.equals("down")) {
                        directionInt = 2;
                    }
                    showLog("starting " + "(" + String.valueOf(row-1) + ","
                            + String.valueOf(column-1) + "," + String.valueOf(directionInt) + ")");
                } catch (Exception e) {
                    e.printStackTrace();
                }
                updateRobotAxis(column, row, direction);
                if (setStartPointToggleBtn.isChecked())
                    setStartPointToggleBtn.toggle();
                this.invalidate();
                return true;
            }
            if (setWaypointStatus) {
                int[] waypointCoord = this.getWaypointCoord();
                if (waypointCoord[0] >= 1 && waypointCoord[1] >= 1)
                    cells[waypointCoord[0]][this.convertRow(waypointCoord[1])]
                            .setType("unexplored");
                setWaypointStatus = false;
                try {
                    this.setWaypointCoord(column, row);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                if (setWaypointToggleBtn.isChecked())
                    setWaypointToggleBtn.toggle();
                this.invalidate();
                return true;
            }

            // add id and the image bearing, popup to ask for user input
            if (setObstacleStatus) {
                if ((1 <= row && row <= 20) && (1 <= column && column <= 20)) {
                    // get user input from spinners in MapTabFragment static values
                    String imageID = (MapTabFragment.imageID).equals("Nil") ?
                            "" : MapTabFragment.imageID;
                    String imageBearing = MapTabFragment.imageBearing;

                    // after init, at stated col and row, add the id to use as ref to update grid
                    ITEM_LIST.get(row - 1)[column - 1] = imageID;
                    imageBearings.get(row - 1)[column - 1] = imageBearing;

                    // this function affects obstacle turning too
                    this.setObstacleCoord(column, row);
                    showLog(commandMsgGenerator(ADD_OBSTACLE));
                    // commented out for Wk 8 and 9
                    //MainActivity.printMessage(commandMsgGenerator(ADD_OBSTACLE));
                }
                this.invalidate();
                return true;
            }
            if (setExploredStatus) {
                cells[column][20-row].setType("explored");
                this.invalidate();
                return true;
            }

            // added removing imageID and imageBearing
            if (unSetCellStatus) {
                ArrayList<int[]> obstacleCoord = this.getObstacleCoord();
                cells[column][20-row].setType("unexplored");
                for (int i=0; i<obstacleCoord.size(); i++)
                    if (obstacleCoord.get(i)[0] == column && obstacleCoord.get(i)[1] == row)
                        obstacleCoord.remove(i);
                ITEM_LIST.get(row)[column-1] = "";  // remove imageID
                imageBearings.get(row)[column-1] = "";  // remove bearing
                this.invalidate();
                return true;
            }
        }
        showLog("Exiting onTouchEvent");
        return false;
    }

    public void toggleCheckedBtn(String buttonName) {
        ToggleButton setStartPointToggleBtn = ((Activity)this.getContext())
                .findViewById(R.id.startpointToggleBtn);
        ToggleButton setWaypointToggleBtn = ((Activity)this.getContext())
                .findViewById(R.id.waypointToggleBtn);
        ImageButton obstacleImageBtn = ((Activity)this.getContext())
                .findViewById(R.id.addObstacleBtn);

        if (!buttonName.equals("setStartPointToggleBtn"))
            if (setStartPointToggleBtn.isChecked()) {
                this.setStartCoordStatus(false);
                setStartPointToggleBtn.toggle();
            }
        if (!buttonName.equals("setWaypointToggleBtn"))
            if (setWaypointToggleBtn.isChecked()) {
                this.setWaypointStatus(false);
                setWaypointToggleBtn.toggle();
            }
        if (!buttonName.equals("obstacleImageBtn"))
            if (obstacleImageBtn.isEnabled())
                this.setSetObstacleStatus(false);
    }


    public void resetMap() {
        showLog("Entering resetMap");
        TextView robotStatusTextView =  ((Activity)this.getContext())
                .findViewById(R.id.robotStatus);
        Switch manualAutoToggleBtn = ((Activity)this.getContext())
                .findViewById(R.id.autoManualSwitch);
        updateRobotAxis(1, 1, "None");
        robotStatusTextView.setText("Not Available");
        SharedPreferences.Editor editor = sharedPreferences.edit();


        if (manualAutoToggleBtn.isChecked()) {
            manualAutoToggleBtn.toggle();
            manualAutoToggleBtn.setText("MANUAL");
        }
        this.toggleCheckedBtn("None");

        receivedJsonObject = null;
        backupMapInformation = null;
        startCoord = new int[]{-1, -1};
        curCoord = new int[]{-1, -1};
        oldCoord = new int[]{-1, -1};
        robotDirection = "None";
        autoUpdate = false;
        arrowCoord = new ArrayList<>();
        obstacleCoord = new ArrayList<>();
        waypointCoord = new int[]{-1, -1};
        mapDrawn = false;
        canDrawRobot = false;
        validPosition = false;
        Bitmap arrowBitmap = BitmapFactory.decodeResource(getResources(),
                R.drawable.ic_arrow_error);

        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                ITEM_LIST.get(i)[j] = "";
                imageBearings.get(i)[j] = "";
            }
        }

        showLog("Exiting resetMap");
        this.invalidate();
    }


    public void updateMapInformation() throws JSONException {
        showLog("Entering updateMapInformation");
        JSONObject mapInformation = this.getReceivedJsonObject();
        showLog("updateMapInformation --- mapInformation: " + mapInformation);
        JSONArray infoJsonArray;
        JSONObject infoJsonObject;
        String hexStringExplored, hexStringObstacle, exploredString, obstacleString;
        BigInteger hexBigIntegerExplored, hexBigIntegerObstacle;
        String message;

        if (mapInformation == null)
            return;

        for(int i=0; i<mapInformation.names().length(); i++) {
            message = "updateMapInformation Default message";
            switch (mapInformation.names().getString(i)) {
                case "map":
                    infoJsonArray = mapInformation.getJSONArray("map");
                    infoJsonObject = infoJsonArray.getJSONObject(0);

                    hexStringExplored = infoJsonObject.getString("explored");
                    hexBigIntegerExplored = new BigInteger(hexStringExplored, 16);
                    exploredString = hexBigIntegerExplored.toString(2);
                    showLog("updateMapInformation.exploredString: " + exploredString);

                    int x, y;
                    for (int j = 0; j < exploredString.length() - 4; j++) {
                        y = 19 - (j / 15);
                        x = 1 + j - ((19 - y) * 15);
                        if ((String.valueOf(exploredString.charAt(j + 2))).equals("1")
                                && !cells[x][y].type.equals("robot"))
                            cells[x][y].setType("explored");
                        else if ((String.valueOf(exploredString.charAt(j + 2))).equals("0")
                                && !cells[x][y].type.equals("robot"))
                            cells[x][y].setType("unexplored");
                    }

                    int length = infoJsonObject.getInt("length");

                    hexStringObstacle = infoJsonObject.getString("obstacle");
                    showLog("updateMapInformation hexStringObstacle: " + hexStringObstacle);
                    hexBigIntegerObstacle = new BigInteger(hexStringObstacle, 16);
                    showLog("updateMapInformation hexBigIntegerObstacle: " + hexBigIntegerObstacle);
                    obstacleString = hexBigIntegerObstacle.toString(2);
                    while (obstacleString.length() < length) {
                        obstacleString = "0" + obstacleString;
                    }
                    showLog("updateMapInformation obstacleString: " + obstacleString);
                    setPublicMDFExploration(hexStringExplored);
                    setPublicMDFObstacle(hexStringObstacle);

                    int k = 0;
                    for (int row = ROW - 1; row >= 0; row--)
                        for (int col = 1; col <= COL; col++)
                            if ((cells[col][row].type.equals("explored")
                                    || (cells[col][row].type.equals("robot")))
                                    && k < obstacleString.length()) {
                                if ((String.valueOf(obstacleString.charAt(k))).equals("1")) {
                                    this.setObstacleCoord(col, 20 - row);
                                }
                                k++;
                            }

                    int[] waypointCoord = this.getWaypointCoord();
                    if (waypointCoord[0] >= 1 && waypointCoord[1] >= 1)
                        cells[waypointCoord[0]][20 - waypointCoord[1]].setType("waypoint");
                    break;
                case "robotPosition":
                    if (canDrawRobot)
                        this.setOldRobotCoord(curCoord[0], curCoord[1]);
                    infoJsonArray = mapInformation.getJSONArray("robotPosition");

                    for (int row = ROW - 1; row >= 0; row--)
                        for (int col = 1; col <= COL; col++)
                            cells[col][row].setType("unexplored");

                    String direction;
                    if (infoJsonArray.getInt(2) == 90) {
                        direction = "right";
                    } else if (infoJsonArray.getInt(2) == 180) {
                        direction = "down";
                    } else if (infoJsonArray.getInt(2) == 270) {
                        direction = "left";
                    } else {
                        direction = "up";
                    }
                    this.setStartCoord(infoJsonArray.getInt(0),
                            infoJsonArray.getInt(1));
                    this.setCurCoord(infoJsonArray.getInt(0)+2,
                            convertRow(infoJsonArray.getInt(1))-1, direction);
                    canDrawRobot = true;
                    break;
                case "waypoint":
                    infoJsonArray = mapInformation.getJSONArray("waypoint");
                    infoJsonObject = infoJsonArray.getJSONObject(0);
                    this.setWaypointCoord(infoJsonObject.getInt("x"),
                            infoJsonObject.getInt("y"));
                    setWaypointStatus = true;
                    break;
                case "obstacle":
                    infoJsonArray = mapInformation.getJSONArray("obstacle");
                    for (int j = 0; j < infoJsonArray.length(); j++) {
                        infoJsonObject = infoJsonArray.getJSONObject(j);
                        this.setObstacleCoord(infoJsonObject.getInt("x"),
                                infoJsonObject.getInt("y"));
                    }
                    message = "No. of Obstacle: " + String.valueOf(infoJsonArray.length());
                    break;
                case "arrow":
                    infoJsonArray = mapInformation.getJSONArray("arrow");
                    for (int j = 0; j < infoJsonArray.length(); j++) {
                        infoJsonObject = infoJsonArray.getJSONObject(j);
                        if (!infoJsonObject.getString("face").equals("dummy")) {
                            this.setArrowCoordinate(infoJsonObject.getInt("x"),
                                    infoJsonObject.getInt("y"),
                                    infoJsonObject.getString("face"));
                            message = "Arrow:  ("
                                    + String.valueOf(infoJsonObject.getInt("x")) + ","
                                    + String.valueOf(infoJsonObject.getInt("y"))
                                    + "), face: " + infoJsonObject.getString("face");
                        }
                    }
                    break;
                case "move":
                    infoJsonArray = mapInformation.getJSONArray("move");
                    infoJsonObject = infoJsonArray.getJSONObject(0);
                    if (canDrawRobot)
                        moveRobot(infoJsonObject.getString("direction"));
                    message = "moveDirection: " + infoJsonObject.getString("direction");
                    break;
                case "status":
                    String msg = mapInformation.getString("status");
                    printRobotStatus(msg);
                    message = "status: " + msg;
                    break;
                default:
                    message = "Unintended default for JSONObject";
                    break;
            }
            if (!message.equals("updateMapInformation Default message"))
                MainActivity.receiveMessage(message);
        }
        showLog("Exiting updateMapInformation");
        this.invalidate();
    }

    // e.g obstacle is on right side of 2x2 and can turn left and vice versa
    public void moveRobot(String direction) {
        showLog("Entering moveRobot");
        setValidPosition(false);
        int[] curCoord = this.getCurCoord();
        ArrayList<int[]> obstacleCoord = this.getObstacleCoord();
        this.setOldRobotCoord(curCoord[0], curCoord[1]);
        int[] oldCoord = this.getOldRobotCoord();
        String robotDirection = getRobotDirection();
        String backupDirection = robotDirection;

        // check if got obstacle when moving one grid up before turning in each case
        switch (robotDirection) {
            case "up":
                switch (direction) {
                    case "forward":
                        if (curCoord[1] != 19) {
                            curCoord[1] += 1;
                            validPosition = true;
                        }
                        break;
                    case "right":
                        if ((1 < curCoord[1] && curCoord[1] < 19)
                                && (0 < curCoord[0] && curCoord[0] < 20)) {
                            curCoord[1] += 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[1] -= 1;
                            } else {
                                curCoord[0] += 1;
                                robotDirection = "right";
                                validPosition = true;
                            }
                        }
                        break;
                    case "back":
                        if (curCoord[1] != 1) {
                            curCoord[1] -= 1;
                            validPosition = true;
                        }
                        break;
                    case "left":
                        if ((0 < curCoord[1] && curCoord[1] < 19)
                                && (2 < curCoord[0] && curCoord[0] <= 20)) {
                            curCoord[1] += 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[1] -= 1;
                            } else {
                                curCoord[0] -= 1;
                                robotDirection = "left";
                                validPosition = true;
                            }
                        }
                        break;
                    default:
                        robotDirection = "error up";
                        break;
                }
                break;
            case "right":
                switch (direction) {
                    case "forward":
                        if (0 < curCoord[0] && curCoord[0] < 20) {
                            curCoord[0] += 1;
                            validPosition = true;
                        }
                        break;
                    case "right":
                        if ((1 < curCoord[1] && curCoord[1] < 20)
                                && (0 < curCoord[0] && curCoord[0] < 20)) {
                            curCoord[0] += 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[0] -= 1;
                            } else {
                                curCoord[1] -= 1;
                                robotDirection = "down";
                                validPosition = true;
                            }
                        }
                        break;
                    case "back":
                        if (curCoord[0] > 2) {
                            curCoord[0] -= 1;
                            validPosition = true;
                        }
                        break;
                    case "left":
                        if ((0 < curCoord[1] && curCoord[1] < 19)
                                && (0 < curCoord[0] && curCoord[0] < 20)) {
                            curCoord[0] += 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[0] -= 1;
                            } else {
                                curCoord[1] += 1;
                                robotDirection = "up";
                                validPosition = true;
                            }
                        }
                        break;
                    default:
                        robotDirection = "error right";
                }
                break;
            case "down":
                switch (direction) {
                    case "forward":
                        if (curCoord[1] != 1) {
                            curCoord[1] -= 1;
                            validPosition = true;
                        }
                        break;
                    case "right":
                        if ((1 < curCoord[1] && curCoord[1] < 19)
                                && (2 < curCoord[0] && curCoord[0] <= 20)) {
                            curCoord[1] -= 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[1] += 1;
                            } else {
                                curCoord[0] -= 1;
                                robotDirection = "left";
                                validPosition = true;
                            }
                        }
                        break;
                    case "back":
                        if (0 < curCoord[1] && curCoord[1] < 19) {
                            curCoord[1] += 1;
                            validPosition = true;
                        }
                        break;
                    case "left":
                        if ((1 < curCoord[1] && curCoord[1] < 20)
                                && (0 < curCoord[0] && curCoord[0] <= 20)) {
                            curCoord[1] -= 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[1] += 1;
                            } else {
                                curCoord[0] += 1;
                                robotDirection = "right";
                                validPosition = true;
                            }
                        }
                        break;
                    default:
                        robotDirection = "error down";
                }
                break;
            case "left":
                switch (direction) {
                    case "forward":
                        if (curCoord[0] > 2) {
                            curCoord[0] -= 1;
                            validPosition = true;
                        }
                        break;
                    case "right":
                        if ((0 < curCoord[1] && curCoord[1] < 19)
                                && (2 < curCoord[0] && curCoord[0] < 20)) {
                            curCoord[0] -= 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[0] += 1;
                            } else {
                                curCoord[1] += 1;
                                robotDirection = "up";
                                validPosition = true;
                            }
                        }
                        break;
                    case "back":
                        if (curCoord[0] < 20) {
                            curCoord[0] += 1;
                            validPosition = true;
                        }
                        break;
                    case "left":
                        if ((0 < curCoord[1] && curCoord[1] <= 20)
                                && (2 < curCoord[0] && curCoord[0] < 20)) {
                            curCoord[0] -= 1;
                            if (checkObstaclesRightInFront(curCoord, obstacleCoord)) {
                                validPosition = false;
                                curCoord[0] += 1;
                            } else {
                                curCoord[1] -= 1;
                                robotDirection = "down";
                                validPosition = true;
                            }
                        }
                        break;
                    default:
                        robotDirection = "error left";
                }
                break;
            default:
                robotDirection = "error moveCurCoord";
                break;
        }
        showLog("Enter checking for obstacles in destination 2x2 grid");
        if (getValidPosition())
            // check obstacle for new position
            for (int x = curCoord[0] - 1; x <= curCoord[0]; x++) {
                for (int y = curCoord[1] - 1; y <= curCoord[1]; y++) {
                    for (int i = 0; i < obstacleCoord.size(); i++) {
                        showLog("x-1 = " + (x-1) + ", y = " + y);
                        showLog("obstacleCoord.get(" + i + ")[0] = " + obstacleCoord.get(i)[0]
                                + ", obstacleCoord.get(" + i + ")[1] = " + obstacleCoord.get(i)[1]);
                        if (obstacleCoord.get(i)[0] == (x - 1) && obstacleCoord.get(i)[1] == y) {
                            setValidPosition(false);
                            robotDirection = backupDirection;
                            break;
                        } else {
                        }
                    }
                    if (!getValidPosition())
                        break;
                }
                if (!getValidPosition())
                    break;
            }
        showLog("Exit checking for obstacles in destination 2x2 grid");
        if (getValidPosition())
            this.setCurCoord(curCoord[0], curCoord[1], robotDirection);
        else {
            if (direction.equals("forward") || direction.equals("back"))
                robotDirection = backupDirection;
            this.setCurCoord(oldCoord[0], oldCoord[1], robotDirection);
        }
        this.invalidate();
        showLog("Exiting moveRobot");
    }

    public boolean checkObstaclesRightInFront(int[] coord, List<int[]> obstacles) {
        showLog("Enter checking for obstacles directly in front");
        // check obstacle for new position
        for (int x = coord[0] - 1; x <= coord[0]; x++) {
            for (int y = coord[1] - 1; y <= coord[1]; y++) {
                for (int i = 0; i < obstacles.size(); i++) {
                    showLog("x-1 = " + (x-1) + ", y = " + y);
                    showLog("obstacle.get(" + i + ")[0] = " + obstacles.get(i)[0]
                            + ", obstacle.get(" + i + ")[1] = " + obstacles.get(i)[1]);
                    if (obstacles.get(i)[0] == (x - 1) && obstacles.get(i)[1] == y) {
                        return true;
                    } else {
                    }
                }
            }
        }
        showLog("Exit checking for obstacles directly in front");
        return false;   // false means no obstacles
    }

    public void printRobotStatus(String message) {
        TextView robotStatusTextView = ((Activity)this.getContext()).findViewById(R.id.robotStatus);
        robotStatusTextView.setText(message);
    }

    public static void setPublicMDFExploration(String msg) {
        publicMDFExploration = msg;
    }

    public static void setPublicMDFObstacle(String msg) {
        publicMDFObstacle = msg;
    }

    private static class MyDragShadowBuilder extends View.DragShadowBuilder {
        private Point mScaleFactor;

        // Defines the constructor for myDragShadowBuilder
        public MyDragShadowBuilder(View v) {
            // Stores the View parameter passed to myDragShadowBuilder.
            super(v);
        }

        // Defines a callback that sends the drag shadow dimensions and touch point back to the
        // system.
        @Override
        public void onProvideShadowMetrics (Point size, Point touch) {
            // Defines local variables
            int width;
            int height;

            // Sets the width of the shadow to half the width of the original View
            width = (int) (cells[1][1].endX - cells[1][1].startX);

            // Sets the height of the shadow to half the height of the original View
            height = (int) (cells[1][1].endY - cells[1][1].startY);

            // Sets the size parameter's width and height values. These get back to the system
            // through the size parameter.
            size.set(width, height);
            // Sets size parameter to member that will be used for scaling shadow image.
            mScaleFactor = size;

            // Sets the touch point's position to be in the middle of the drag shadow
            touch.set(width / 2, height / 2);
        }

        @Override
        public void onDrawShadow(Canvas canvas) {
            // Draws the ColorDrawable in the Canvas passed in from the system.
            canvas.scale(mScaleFactor.x/(float)getView().getWidth(),
                    mScaleFactor.y/(float)getView().getHeight());
            getView().draw(canvas);
        }

    }

    // week 8 req to update robot pos when alg sends updates
    public void performAlgoCommand(int x, int y, String direction) {
        showLog("Enter performAlgoCommand");
        showLog("x = " + x + "\n" + "y = " + y);
        if ((x > 1 && x < 21) && (y > -1 && y < 20)) {
            showLog("within grid");
            robotDirection = (robotDirection.equals("None")) ? "up" : robotDirection;
            switch (direction) {
                case "N":
                    robotDirection = "up";
                    break;
                case "S":
                    robotDirection = "down";
                    break;
                case "E":
                    robotDirection = "right";
                    break;
                case "W":
                    robotDirection = "left";
                    break;
            }
        }
        // if robot pos was not set initially, don't set as explored before moving to new coord
        if (!(curCoord[0] == -1 && curCoord[1] == -1)) {
            showLog("if robot was not at invalid pos prev");
            if ((curCoord[0] > 1 && curCoord[0] < 21) && (curCoord[1] > -1 && curCoord[1] < 20)) {
                showLog("prev pos was within grid");
                for (int i = curCoord[0] - 1; i <= curCoord[0]; i++) {
                    for (int j = curCoord[1] - 1; j <= curCoord[1]; j++) {
                        cells[i][20 - j - 1].setType("explored");
                    }
                }
            }
        }
        // if robot is still in frame
        if ((x > 1 && x < 21) && (y > -1 && y < 20)) {
            showLog("within grid");
            setCurCoord(x, y, robotDirection);    // set new coords and direction
            canDrawRobot = true;
        }
        // if robot goes out of frame
        else {
            showLog("set canDrawRobot to false");
            canDrawRobot = false;
            curCoord[0] = -1;
            curCoord[1] = -1;
        }
        this.invalidate();
        showLog("Exit performAlgoCommand");
    }

    public void performAlgoTurning(int x, int y, String facing, String cmd) {
        showLog("Enter performAlgoTurning");
        final int i = y;
        final int j = x;
        final String finalFacing = facing;
        int delay = 500;   // add 1000 after each run cos its like a timestamp

        if ((x > 1 && x < 21) && (y > -1 && y < 20)) {
            showLog("within grid");
            robotDirection = (robotDirection.equals("None")) ? "up" : robotDirection;

            // call animation only when the robot is in frame
            if (curCoord[0] != -1 && curCoord[1] != -1) {
                showLog("robot is within grid");
                switch (robotDirection) {
                    case "up":
                        switch (cmd) {
                            case "fl":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 4, i - 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 3, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                // rest of the forward motion
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "fr":
                                // move forward 1 grid
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 4, i - 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 3, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                // rest of the forward motion
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "rl":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i + 3, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i + 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "rr":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i + 3, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                // rest of the forward motion
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i + 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                        }
                        break;
                    case "down":
                        switch (cmd) {
                            case "fl":
                                // move forward 1 grid
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 4, i + 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 3, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                // move 1 grid forward
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "fr":
                                // move forward 1 grid
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 4, i + 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 3, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                // move 1 grid forward
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "rl":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i - 3, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i - 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "rr":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i - 3, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i - 1, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                        }
                        break;

                    case "right":
                        switch (cmd) {
                            case "fl":
                                // move forward 1 grid
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i - 4, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 3, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                // move 1 grid forward
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 2, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 1, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "fr":
                                // move forward 1 grid
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i + 4, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 3, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                // move 1 grid forward
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 2, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 1, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "rl":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 3, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 1, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "rr":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 3, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 2, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 1, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                        }
                        break;
                    case "left":
                        switch (cmd) {
                            case "fl":
                                // move forward 1 grid
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i + 4, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 3, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                // move 1 grid forward
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 2, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 1, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                break;
                            case "fr":
                                // move forward 1 grid
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j + 1, i - 4, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                // turn
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 3, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                // move 1 grid forward
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 2, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 1, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                delay += 500;
                                break;
                            case "rl":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 3, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i + 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i + 1, finalFacing);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                            case "rr":
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 3, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;
                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 2, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j - 1, i - 2, robotDirection);
                                    }
                                }, delay);
                                delay += 500;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i - 1, finalFacing);
                                    }
                                }, delay);
                                delay += 1000;

                                new Handler().postDelayed(new Runnable() {
                                    public void run() {
                                        performAlgoCommand(j, i, finalFacing);
                                    }
                                }, delay);
                                break;
                        }
                        break;
                }
            }
            // if robot was out of frame in prev state
            else {
                showLog("robot was out of grid in prev state");
                performAlgoCommand(x, y, facing);
            }
        }
        // if robot goes out of frame
        else {
            // colour explored before disappearing
            if (!(curCoord[0] == -1 && curCoord[1] == -1)) {
                showLog("if robot was alr in grid");
                if ((curCoord[0] > 1 && curCoord[0] < 21)
                        && (curCoord[1] > -1 && curCoord[1] < 20)) {
                    showLog("pos is good before going out of grid");
                    for (int n = curCoord[0] - 1; n <= curCoord[0]; n++) {
                        for (int m = curCoord[1] - 1; m <= curCoord[1]; m++) {
                            cells[n][20 - m - 1].setType("explored");
                        }
                    }
                }
            }
            showLog("setting canDrawRobot to false");
            canDrawRobot = false;
            curCoord[0] = -1;
            curCoord[1] = -1;
        }
        this.invalidate();
        showLog("Exit performAlgoTurning");
    }

    // week 8 req to send algo obstacle info
    public String getObstacles() {
        String msg = "ALG|";

        for (int i = 0; i < obstacleCoord.size(); i++) {
            showLog("i = " + Integer.toString(i));
            msg +=  (Float.toString((float) (obstacleCoord.get(i)[0] + 0.5)) + ","
                    + Float.toString((float) (obstacleCoord.get(i)[1] + 0.5)) + ","
                    + imageBearings.get(obstacleCoord.get(i)[1])[obstacleCoord.get(i)[0]].charAt(0)
                    + ";");
        }
        msg += "\n";
        return msg;
    }

    // checklist req
    public String commandMsgGenerator (int command) {
        String msg = "";
        switch (command) {
            case ADD_OBSTACLE:
                // format: <target component>|<command>,<image id>,<obstacle coord>,<face direction>
                msg = "ADD_OBSTACLE,"
                        + ITEM_LIST.get(initialRow - 1)[initialColumn - 1] + ","
                        + "(" + (initialColumn - 1) + "," + (initialRow - 1) + "),"
                        + imageBearings.get(initialRow - 1)[initialColumn - 1].charAt(0) + "\n";
                break;
            case REMOVE_OBSTACLE:
                // format: <target component>|<command>,<image id>,<obstacle coord>
                msg = "REMOVE_OBSTACLE,"
                        + oldItem + ","
                        + "(" + (initialColumn - 1) + "," + (initialRow - 1) + ")" + "\n";
                break;
            case MOVE_OBSTACLE:
                // format: <target component>|<command>,<old coord>,<new coord>
                msg = "MOVE_OBSTACLE,"
                        + "(" + (initialColumn - 1) + "," + (initialRow - 1) + "),"
                        + "(" + (endColumn - 1) + "," + (endRow - 1) + ")" + "\n";
                break;
            case ROBOT_MOVING:
                // format: <target component>|<command>,<string>
                msg = "MSG," + "Move robot" + "\n";
                break;
            case START_AUTO_MOVE:
                // format: <target component>|<command>,<string>
                msg = "MSG," + "Start auto movement." + "\n";
                break;
            case START_FASTEST_CAR:
                // format: <target component>|<command>,<string>
                msg = "MSG," + "Start fastest car." + "\n";
        }
        return msg;
    }

    // wk 8 task
    public boolean updateIDFromRpi(String obstacleID, String imageID) {
        showLog("starting updateIDFromRpi");
        int x = obstacleCoord.get(Integer.parseInt(obstacleID) - 1)[0];
        int y = obstacleCoord.get(Integer.parseInt(obstacleID) - 1)[1];
        ITEM_LIST.get(y)[x] = (imageID.equals("-1")) ? "" : imageID;
        this.invalidate();
        return true;
    }
}