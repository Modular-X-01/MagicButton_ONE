

// using namespace JUGL;

// SSD1306_128x64 driver;

// const int xpin = A3; //Assign pin A3 to x

// const int ypin = A2; //Assign pin A2 to y

// int x， y， x1， y1， r， varx， vary， width， height; //Define variables

// int xy ［2］; //Array to hold (x，y) coordinate

// //Declaration of functions

// void Circle(IScreen& scr);

// void move_right(IScreen& scr);

// void stop_right(IScreen& scr);

// void move_left(IScreen& scr);

// void stop_left(IScreen& scr);

// void move_up(IScreen& scr);

// void stop_up(IScreen& scr);

// void move_down(IScreen& scr);

// void stop_down(IScreen& scr);

// void setup(){

// IScreen& screen = driver; //Make reference to driver

// screen.Begin(); //Initialize screen

// width = screen.GetWidth(); //Get width of screen (128)

// height = screen.GetHeight(); //Get height of screen (64)

// Circle(screen); //Draw circle

// }

// void loop(){

// x1 = analogRead(xpin); //Read x analog data

// y1 = analogRead(ypin); //Read y analog data

// IScreen& screen = driver; //Make reference to driver

// if(x1<500){ //Check if sensor is tilted to the right

// move_right(screen); //Move ball right

// if(varx>=width-1-r ){ //Check if ball reached end of screen

// stop_right(screen); //Stop moving

// }

// }

// if(x1>520){ //Check if sensor is tilted to the left

// move_left(screen); //Move ball left

// if(varx=height-1-rH{ //Check if ball reached end of screen

// stop_up(screen); //Stop moving

// }

// }

// if(y1>510){ //Check if sensor is tilted down

// move_down(screen); //Move ball down

// if(varyr){ //Check if ball is within boundaries

// scr.Flush(); //Display on screen

// }

// }

// void stop_left(IScreen& scr){

// scr.Clear(); //Clear screen

// varx = r; //Update varx

// xy［0］ = varx; //Store new varx value

// scr.FillCircle(Point(5，xy［1］)，r); //Draw circle

// scr.Flush(); //Display on screen

// }

// void move_up(IScreen& scr){

// scr.Clear(); //Clear screen

// vary += 10; //Move ball 10 pixels up， assign value to vary

// xy［1］ = vary; //Store new vary value

// scr.FillCircle(Point(xy［0］，vary)，r); //Draw circle

// if(varyr){ //Check if ball is within boundaries

// scr.Flush(); //Display on screen

// }

// }

// void stop_down(IScreen& scr){

// scr.Clear(); //Clear screen

// vary = r; //Update vary

// xy［1］ = vary; //Store new vary value

// scr.FillCircle(Point(xy［0］，5)，r); //Draw circle

// scr.Flush(); //Display on screen

// }

// void app_motion_

#include "app_motion.h"

static bool f_init = false;

#if HW_ACC == ACC_KXTJ3_1057
#include "kxtj3-1057.h"
KXTJ3 myIMU(0x0E); // Address can be 0x0E or 0x0F
#elif HW_ACC == ACC_QMA7981
#include "qma7981.h"
QMA7981 myIMU;
#elif HW_ACC == ACC_LSM6DS3TR
#include <Adafruit_LSM6DS3.h>
#endif

#if HW_ACC == ACC_KXTJ3_1057
bool app_motion_init(float sampleRate, uint8_t accelRange)
{
    if (f_init == false)
    { // acc
        // float sampleRate = 50; // HZ - Samples per second - 0.781, 1.563, 3.125, 6.25, 12.5, 25, 50, 100, 200, 400, 800, 1600Hz
        // uint8_t accelRange = 2;  // Accelerometer range = 2, 4, 8, 16g
        if (myIMU.begin(sampleRate, accelRange) != 0)
        {
            // u8g2log.print("Failed to initialize IMU.\n");
            return false;
        }
        else
        {
            // u8g2log.println("IMU initialized.\n");
        }
        // myIMU.standby(false);}
    }
    return true;
}

void app_motion_get(motion_t *data)
{

    data->x = myIMU.axisAccel(X);
    data->y = myIMU.axisAccel(Y);
    data->z = myIMU.axisAccel(Z);
}

void app_motion_get_raw(motion_t *data)
{
    int16_t outRAW;
    uint8_t regToRead = 0;

    regToRead = KXTJ3_OUT_X_L;
    myIMU.readRegisterInt16(&outRAW, regToRead);
    data->raw_x = outRAW;

    regToRead = KXTJ3_OUT_Y_L;
    myIMU.readRegisterInt16(&outRAW, regToRead);
    data->raw_y = outRAW;

    regToRead = KXTJ3_OUT_Z_L;
    myIMU.readRegisterInt16(&outRAW, regToRead);
    data->raw_z = outRAW;
}

void app_motion_deinit()
{
    if (f_init)
    {
        myIMU.standby(true);
        Wire.end();
        f_init = false;
    }
}

void app_motion_sleep()
{
    myIMU.standby(true);
}
void app_motion_wakeup()
{
    myIMU.standby(false);
}

#elif HW_ACC == ACC_QMA7981
bool app_motion_init(float sampleRate, uint8_t accelRange)
{
    if (f_init == false)
    {                                         // acc
        myIMU.initialize_default();           // setup acceleromter with default settings
        myIMU.set_full_scale_range(RANGE_2G); // set maximum range (+- 2G)
        myIMU.set_bandwidth(MCLK_DIV_BY_975); // set bandwidth (samples per sec)
                                              // = CLK / DIV_BY = 50k / 975 = 50 samples / sec
    }
    return true;
}

void app_motion_get(motion_t *data)
{

    data->x = myIMU.get_accel_x();
    data->y = myIMU.get_accel_y();
    data->z = myIMU.get_accel_z();
}

void app_motion_get_raw(motion_t *data)
{



    data->raw_x = myIMU.get_accel_x();


    data->raw_y = myIMU.get_accel_y();

   
    data->raw_z = myIMU.get_accel_z();
}

void app_motion_deinit()
{
    if (f_init)
    {
        myIMU.set_mode(MODE_STANDBY);
        Wire.end();
        f_init = false;
    }
}

void app_motion_sleep()
{
    myIMU.set_mode(MODE_STANDBY);
}
void app_motion_wakeup()
{
    myIMU.set_mode(MODE_ACTIVE);
}
#elif HW_ACC == ACC_LSM6DS3TR

#endif





uint8_t app_getDirection()
{
    motion_t data;
    uint8_t dir = 0;
    app_motion_get(&data);
    if (data.x > AXIS_THRESHOLD)
    {
        dir = DIR_RIGHT;
    }
    else if (data.x < (0 - AXIS_THRESHOLD))
    {
        dir = DIR_LEFT;
    }

    if (data.y > AXIS_THRESHOLD)
    {
        dir |= DIR_DOWN;
    }
    else if (data.y < (0 - AXIS_THRESHOLD))
    {
        dir |= DIR_UP;
    }

    return dir;
}