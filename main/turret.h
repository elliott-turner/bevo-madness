#define N1 66
#define N2 50
#define N3 62
#define N4 49

#define STEPS_PER_DEG 11.5
#define NG 100
#define NP 20

#define DELAY_MIN 400
#define DELAY_MAX 1200

void turret_start();
float turret_get_angle();
void turret_set_angle(float angle);
void turret_move_angle(float angle);