#ifndef ECR_SETUP_H_
#define ECR_SETUP_H_


///////////////////////////////////////////////////////////////////////////
//----------------------------------- PID ---------------------------------
///////////////////////////////////////////////////////////////////////////
//--------------------------------  Pivot turn  ------------------
#define PWM_FREQ		75000



//-------------------------- Device N1 --------------------
//----------------------------------
#define STEPK           1.725

#define KP              10           // 8
#define KD              20          // 10
#define KI              1         // 12

// curved turns
#define WKP             6
#define WKD             33
#define WKI             10

#define FAST_SPEED     	195		// 180 | 190
#define TURN_SPEED		110		// 220 | 230
#define UTURN_SPEED		35 		// 240 | 255 // 120
#define STARTING_SPEED 	30		// 140 | 140
#define STOPPING_SPEED 	80			// 150 | 150
#define RAMP            120		// 180 | 180 // mm
#define	ACCELERATION_DIFF  FAST_SPEED - STARTING_SPEED


//--- Gyro callibration for N1 i N2 in mpu6050.c ---

//------------------- N1 --------------------
#define GOTO_END_DISTANCE	120		// 80  N2   // 50mm
#define FRONT_CAL			20000
#define SIDE_CALL			20000




//#define AUTO_TUNING					// Calculate connected parameters like start/ stop speed to turn speed and ram to fast speed

//----------- Maze ----------
#define MAZE_WIDTH      	30		// cm
#define WALL_TRESHOLD		8		// ( MAZE_WIDTH * WALL_THRESHOLD )		// MAZE_WIDTH /2 + ROBOT_WIDTH /2 cm = mm
#define WALL_WIDTH			20
#define MAZE_TYE 			1		// Net square Maze

#define WALL_FOLLOWING_TARGET 100

#define RADIUS_TURN         	    ((MAZE_WIDTH*10)-90)/2 // Radius turn size in mm. 100 is the robot width. Finds the center of the maze depending on it's size.
#define INNER_CIRCUMFERENCE         (2*3.14159*RADIUS_TURN)/4 // The inner circumference of the quarter circle which has to be elapsed by the inner wheel.
#define DEGREE_STEP_RADIUS_TURN 	INNER_CIRCUMFERENCE/90
//--------- TURN ANGLES -----------
#define TURN_LEFT_ANGLE     -90
#define TURN_RIGHT_ANGLE    90
#define TURN_BACK_ANGLE     180

//----------- Dog mode ---------
#define MIN_DISTANCE_DOG 50
#define FRONT_DESIRED_DISTANCE_DOG 140
#define FRONT_MAX_DISTANCE_DOG 300
#define KI_DOG 0.04
#define WALL_SIDE_THRESHOLD 150

//--------- Wall following ------
#define WALL_DESIRED_DISTANCE 133



//-------------------------------------------------------------------------

typedef struct {
    unsigned int test;
    unsigned int brightness;
    unsigned int fast_speed;
    unsigned int turn_speed;
    unsigned int uturn_speed;
    unsigned int starting_speed;
    unsigned int stopping_speed;
    unsigned int ramp;
    unsigned int turn_clamp;
    unsigned int kp;
    unsigned int kd;
    unsigned int ki;
    unsigned int wkp;
    unsigned int wkd;
    unsigned int wki;
    unsigned int frontL_callibration;
    unsigned int frontR_callibration;
    unsigned int sideL_callibration;
    unsigned int sideR_callibration;
    unsigned int left_callibration;
    unsigned int right_callibration;
    float   gyro_offset;
    unsigned int chk_sum;
} ST_DEV_SETTINGS; //   15 long * 4  bytes = 60



typedef struct {
	unsigned int maze_width;
	unsigned int wall_width;
	unsigned int maze_type;
	unsigned int goto_end_distance;
	unsigned int start_way;
	unsigned int chk_sum;
} ST_MAZE_SETTINGS;

//===========================================================================
#define STATUS			0
#define DEV_SETTINGS    16           // eeprom address
#define MAZE_SETTINGS	128

extern ST_DEV_SETTINGS dev_settings;
extern ST_MAZE_SETTINGS maze_settings;

extern unsigned int front_a_value;
extern unsigned int front_b_value;


//---------------------------- Factory settings ----------------------
void dev_factory_settings(void);
unsigned char dev_settings_chk(void);
void maze_settings_save(void);
unsigned char maze_settings_chk(void);
unsigned char factorySettings(unsigned char kbd_key);

//--------------------------------
unsigned char fast_spped_set(unsigned char kbd_key);
unsigned char uturn_spped_set(unsigned char kbd_key);
unsigned char turn_spped_set(unsigned char kbd_key);
unsigned char starting_spped_set(unsigned char kbd_key);
unsigned char stopping_spped_set(unsigned char kbd_key);
unsigned char ramp_set(unsigned char kbd_key);
unsigned char turn_clamp_set(unsigned char kbd_key) ;
unsigned char kp_set(unsigned char kbd_key);
unsigned char kd_set(unsigned char kbd_key);
unsigned char ki_set(unsigned char kbd_key);

unsigned char wkp_set(unsigned char kbd_key);
unsigned char wkd_set(unsigned char kbd_key);
unsigned char wki_set(unsigned char kbd_key);

//----------------
unsigned char frontL_callibrate(unsigned char kbd_key);
unsigned char frontR_callibrate(unsigned char kbd_key);
unsigned char sideL_callibrate(unsigned char kbd_key);
unsigned char sideR_callibrate(unsigned char kbd_key);
unsigned char left_callibrate(unsigned char kbd_key);
unsigned char right_callibrate(unsigned char kbd_key);
unsigned char gyro_callibrate(unsigned char kbd_key);

//---------------
unsigned char maze_width_set(unsigned char kbd_key);
unsigned char maze_wallWidth_set(unsigned char kbd_key);
unsigned char gotoEnd_set(unsigned char kbd_key);
unsigned char maze_type_set(unsigned char kbd_key);
unsigned char start_way_set(unsigned char kbd_key);


#endif /*ECR_SETUP_H_*/
