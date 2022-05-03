#define SPEED_NORMAL 15
#define SPEED_SLOW 30

#define SENSOR_RANGE 2500.0
#define WHITE_VAL 750.0
#define BLACK_VAL 2000.0

void move_straight(float dist);
void move_turn(float deg);
void move_stop(void);
bool check_for_line(void);
void move_to_line(void);
