#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/timing/timing.h>

#define PI 3.14159265358979

double sin_table[1257];

static const struct pwm_dt_spec phase_1 = PWM_DT_SPEC_GET(DT_NODELABEL(phase_1));
static const struct pwm_dt_spec phase_2 = PWM_DT_SPEC_GET(DT_NODELABEL(phase_2));
static const struct pwm_dt_spec phase_3 = PWM_DT_SPEC_GET(DT_NODELABEL(phase_3));

static const uint32_t min_pulse = PWM_USEC(0);
static const uint32_t max_pulse = PWM_USEC(90);

uint64_t start_time, end_time, total_ns;

double dt = 0;
double freq = 0;

int counter = 0;

void spin(double angle){
	int index1 = round( ( 100 * angle ) );
	int index2 = round( ( 100 * ( angle + 2 * PI / 3. ) ) );
	int index3 = round( ( 100 * ( angle + 4 * PI / 3. ) ) );

	uint32_t pulse_width_1 = ( sin_table[index1] + 1) * (double) ( max_pulse - min_pulse ) / 2.;
	uint32_t pulse_width_2 = ( sin_table[index2] + 1) * (double) ( max_pulse - min_pulse ) / 2.;
	uint32_t pulse_width_3 = ( sin_table[index3] + 1) * (double) ( max_pulse - min_pulse ) / 2.;

	int ret = pwm_set_pulse_dt(&phase_1, PWM_NSEC(pulse_width_1));
	if (ret < 0) {
		printk("Error %d: failed to set pulse width for phase 1\n", ret);
	}

	ret = pwm_set_pulse_dt(&phase_2, PWM_NSEC(pulse_width_2));
	if (ret < 0) {
		printk("Error %d: failed to set pulse width for phase 2\n", ret);
	}

	ret = pwm_set_pulse_dt(&phase_3, PWM_NSEC(pulse_width_3));
	if (ret < 0) {
		printk("Error %d: failed to set pulse width for phase 3\n", ret);
	}
}

void my_work_handler(struct k_work *work)
{
	end_time = k_uptime_ticks();

    double dt = (end_time - start_time) / 10000000.;

    freq = 1. / dt;

    if(counter % 100 == 0){
		counter = 0;
		printf("Frequency: %" PRIu64 " Hz \n", (uint64_t) freq);
	}
	
	start_time = end_time;

    spin(0);

    counter++;
}

K_WORK_DEFINE(my_work, my_work_handler);

void my_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&my_work);
}


K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);

int main(){

	for(int i = 0 ; i < 1257 ; i++){
		sin_table[i] = sin( (double) 0.01 * i ) ;
	}

    start_time = k_uptime_ticks();

	/* start periodic timer that expires once every second */
	k_timer_start(&my_timer, K_USEC(100), K_USEC(100));

	return 0;
}