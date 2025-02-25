/**
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include <mbot/motor/motor.h>
#include <mbot/defs/mbot_params.h>
#include "config/mbot_classic_config.h"
#include <mbot/encoder/encoder.h>

#define Kp2 0.005 // Proportional gain for control
#define Kd2 0.001  // Derivative gain for control
// #define Kp1 0.00025 // Proportional gain for control
// #define Kd1 0.0005  // Derivative gain for control
#define Kp1 0.0009 // Proportional gain for control
#define Kd1 0.0006  // Derivative gain for control
#define Ka 0.0001
#define Kb 0.0001
// #define Kp 0.02  // Proportional gain for control
// #define Kd 0.005  // Derivative gain for control
// #define MAX_VELOCITY 0.4
#define MAX_VELOCITY 1
#define MOTOR_MASTER 0
#define MOTOR_SLAVE 1
// #define MOTOR1_RES 3000
// #define MOTOR2_RES 3000
#define MOTOR1_RES 64
#define MOTOR2_RES 4540

#define THRESHOLD 0.5
#define PI 3.141592653589793
#define AMPLITUDE 240  // Amplitude of sine wave (degrees)
#define FREQUENCY 20    // Frequency in Hz
#define TIME_STEP 0.01

#define DT 10             // Time step in milliseconds
#define DELAY_MS 100      // Desired delay in milliseconds
#define BUFFER_SIZE (DELAY_MS / DT)  // Number of stored states

// Circular buffer structure
typedef struct {
    float master_pos[BUFFER_SIZE];  // Master position
    float master_vel[BUFFER_SIZE];  // Master velocity
    float slave_pos[BUFFER_SIZE];  // Slave position
    float slave_vel[BUFFER_SIZE];  // Slave velocity
    int head;                    // Index of newest data
} StateBuffer;

// Initialize buffer
void init_buffer(StateBuffer *buffer) {
    buffer->head = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer->master_pos[i] = 0.0;
        buffer->master_vel[i] = 0.0;
        buffer->slave_pos[i] = 0.0;
        buffer->slave_vel[i] = 0.0;
    }
}

// Add new state to buffer
void add_state(StateBuffer *buffer, float master_pos, float master_vel, float slave_pos, float slave_vel) {
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;  // Increment head circularly
    buffer->master_pos[buffer->head] = master_pos;
    buffer->master_vel[buffer->head] = master_vel;
    buffer->slave_pos[buffer->head] = slave_pos;
    buffer->slave_vel[buffer->head] = slave_vel;
}


float clipPosition(int position) {
    // Wrap position to [-180, +180]
    while (position > 180) position -= 360;
    while (position < -180) position += 360;
    return position;
}

float clipCommand(float command) {
    // Limit the velocity to the maximum allowed
    if (command > MAX_VELOCITY) command = MAX_VELOCITY;
    if (command < -MAX_VELOCITY) command = -MAX_VELOCITY;
    return command;
}

void synchronizeMotors(int reverse) {

    int d1, d2, t1, t2 = 0;
    StateBuffer buffer;
    init_buffer(&buffer);
    double time = 0.0;

    while (1) {
        // Read data
        d1 = mbot_encoder_read_delta(MOTOR_MASTER);
        d2 = mbot_encoder_read_delta(MOTOR_SLAVE);
        t1 = mbot_encoder_read_count(MOTOR_MASTER);
        t2 = mbot_encoder_read_count(MOTOR_SLAVE);

        int motor1_position = t1 * 360 / MOTOR1_RES;
        int motor2_position = t2 * 360 / MOTOR2_RES;
        float motor1_velocity = d1 * 360 / MOTOR1_RES;
        float motor2_velocity = d2 * 360 / MOTOR2_RES;

        add_state(&buffer, motor1_position, motor1_velocity, motor2_position, motor2_velocity);


        // Retrieve delayed master state
        int delayed_index = (buffer.head + 1) % BUFFER_SIZE;
        float delayed_master_pos = buffer.master_pos[delayed_index];
        float delayed_master_vel = buffer.master_vel[delayed_index];
        float delayed_slave_pos = buffer.slave_pos[delayed_index];
        float delayed_slave_vel = buffer.slave_vel[delayed_index];

        // Compute the position error
        float position_error = motor2_position - motor1_position;
        float velocity_error = motor2_velocity - motor1_velocity;

        // Reverse behavior
        // if (reverse) {
        //     position_error = -position_error; // Reverse Motor 2 direction
        // }

        float command1_p = Kp1 * position_error;
        float command1_d = Kd1 * velocity_error;
        float command2_p = Kp2 * (-position_error);
        float command2_d = Kd2 * (-velocity_error);

        
        // Compute motor 2 velocity using proportional control
        float command1 = 0;
        float command2 = 0;
        if (fabs(position_error) > 60) {
            command1 = command1_p + command1_d - Ka * motor1_velocity;
            command2 = command2_p + command2_d - Kb * motor2_velocity;
        }

        

        // If control command is too high (motor2 stuck), override with sine wave
        if (fabs(command1) > THRESHOLD) {
            float theta_ref = AMPLITUDE * sin(2 * PI * FREQUENCY * time);
            command1 = Kp1 * (theta_ref + position_error) + command1_d - Ka * motor1_velocity;
            time += TIME_STEP;
        } else {
            time = 0;
        }

        // float command1 = Kp1 * (delayed_slave_pos - motor1_position) + Kd1 * (delayed_slave_vel - motor1_velocity) - Ka * motor1_velocity;
        // float command2 = Kp2 * (delayed_master_pos - motor2_position) + Kd2 * (delayed_master_vel - motor2_velocity) - Kb * motor2_velocity;

        printf("| %7d| %7d| %.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|\n", motor1_position, motor2_position, position_error, clipCommand(command1), clipCommand(command2), command1_p, command1_d, command2_p, command2_d);

        // Set Motor 2's velocity
        mbot_motor_set_duty(MOTOR_MASTER, clipCommand(command1));
        mbot_motor_set_duty(MOTOR_SLAVE, clipCommand(command2));

        // Debugging output
        // printf("\rMotor1 Pos: %d | Motor2 Pos: %d | Motor2 Vel: %d\n", 
        //        motor1_position, motor2_position, motor2_velocity);

        // Small delay to prevent excessive CPU usage
        sleep_ms(100); // 10 ms
    }
}

int main() {
    stdio_init_all();
    mbot_encoder_init();
    sleep_ms(2000);

    mbot_motor_init(MOTOR_MASTER);
    mbot_motor_init(MOTOR_SLAVE);
    sleep_ms(2000);

    
    printf("\033[2J\r");
    printf("Synchronizing Motor 2 to Motor 1. Press Ctrl+C to stop.\n");

    int reverse = 0; // 0: Normal direction, 1: Reverse

    // // Optionally enable reverse mode
    // printf("Enter 1 for reverse mode, 0 for normal mode: ");
    // scanf("%d", &reverse);

    // Start motor synchronization
    synchronizeMotors(reverse);
    
    // return 0;
}