#define SPEED_NORMAL 10
#define SPEED_SLOW 20

#define SENSOR_RANGE 2500.0
#define WHITE_VAL 750.0
#define BLACK_VAL 2000.0

#define MOTOR_GAIN_P 0.020
#define MOTOR_GAIN_I 0.008
#define MOTOR_GAIN_D 0.002

void move_straight(float dist);
void move_turn(float deg);
void move_stop(void);
bool check_for_line(void);
void move_to_line(void);
