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

// Tune your own gains and parameters below
// ---------Your code here---------
// Basic PD Controller
#define Kp1 0.0
#define Kd1 0.0
#define Kp2 0.0 
#define Kd2 0.0 
#define Ka 0.0
#define Kb 0.0

// Deadband value
#define DEADBAND 0 

// Saturation/Pulse
#define THRESHOLD 2.0   // Motor command difference to trigger saturation/pulse
#define AMPLITUDE 0   // Amplitude of pulse sine wave (degrees)
#define FREQUENCY 0    // Frequency in Hz

// Latency
#define DELAY_MS 0       // Desired latency in milliseconds
// --------End of your code---------

// ----------DO NOT REVISE----------
#define MAX_VELOCITY 1
#define MOTOR_MASTER 0
#define MOTOR_SLAVE 1
#define MOTOR1_RES 400
#define MOTOR2_RES 4540
#define PI 3.141592653589793
#define TIME_STEP 10 // Time step in milliseconds
#define BUFFER_SIZE (DELAY_MS / TIME_STEP)  // Number of stored states

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

// Clip command within [-1, 1]
float clipCommand(float command) {
    // Limit the velocity to the maximum allowed
    if (command > MAX_VELOCITY) command = MAX_VELOCITY;
    if (command < -MAX_VELOCITY) command = -MAX_VELOCITY;
    return command;
}

void teleoperation(int reverse, int delay) {

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

        // Compute the errors
        float position_error = motor2_position - motor1_position;
        float velocity_error = motor2_velocity - motor1_velocity;

        // Reverse mode
        if (reverse) {
            position_error = -position_error;
            velocity_error = -velocity_error;
        }

        // --------------Your code here--------------

        // Basic PD
        float command1_p = 0
        float command1_d = 0
        float command2_p = 0
        float command2_d = 0

        // With Latency
        if (delay) {
            command1_p = 0
            command1_d = 0
            command2_p = 0
            command2_d = 0
        }

        // Two way teleoperation with deadband
        float command1 = 0;
        float command2 = command2_p + command2_d - Kb * motor2_velocity;;
        if (fabs(position_error) > DEADBAND) {
            command1 = command1_p + command1_d - Ka * motor1_velocity;
        }

        // With Saturation/Pulse
        if (fabs(command1) > THRESHOLD) {
            float theta_ref = AMPLITUDE * sin(2 * PI * FREQUENCY * time);
            command1 = Kp1 * (theta_ref + position_error) + command1_d - Ka * motor1_velocity;
            time += TIME_STEP / 1000;
        } else {
            time = 0;
        }
        // ----------End of your code-----------
        
        // Serial output
        printf("| %7d| %7d| %.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|\n", motor1_position, motor2_position, position_error, clipCommand(command1), clipCommand(command2), command1_p, command1_d, command2_p, command2_d);

        // Set motor commands
        mbot_motor_set_duty(MOTOR_MASTER, clipCommand(command1));
        mbot_motor_set_duty(MOTOR_SLAVE, clipCommand(command2));

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
    int delay = 0; // 0: No delay, 1: with delay

    // Start main
    teleoperation(reverse, delay);
}