#ifndef SERVO
#define SERVO

#ifdef __cplusplus
extern "C"
{
#endif

#define SERVO_ANGLE_MAX 90.0
#define SERVO_ANGLE_MIN -90.0

    void enableServo();
    void disableServo();
    void setServo(float angle);

#ifdef __cplusplus
}
#endif

#endif /* SERVO */