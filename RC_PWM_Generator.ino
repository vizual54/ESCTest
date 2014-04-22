#include <math.h> //known bug in delay()
#define S_TO_US(x)      ((x) * 1000000)
#define SERVOUT 12
#define LED 13
uint16_t rpc_min = 950;
uint16_t rpc_max = 2200;
uint16_t rpc_mid = rpc_min + (rpc_max - rpc_min) / 2;
uint16_t rpc_start = 1100;
uint16_t rpc_out = rpc_min;

int pw = 1000;
int pwi = 0;
int i;

ISR(TIMER1_COMPA_vect)
{
	digitalWrite(SERVOUT, 1);
	delayMicroseconds(rpc_out - 3);
	digitalWrite(SERVOUT, 0);
}

void setup_timer_interrupt()
{
	//set timer1 interrupt
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12);// Set CTC mode, CS12 bit for 256 prescaler
	// set compare match register for 50Hz, 20ms update rate
	OCR1A = F_CPU / (256 * 50) - 1;
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	GTCCR |= (1 << PSRSYNC);// reset the prescaler
	TCNT1 = 0;
}

// must be more than 24 ms
void ramp_up(uint16_t rpc_from_us, uint16_t rpc_to_us, uint32_t ramp_up_time_us)
{
	uint16_t up_sleep_us = (uint16_t)ceil(((float)ramp_up_time_us / ((float)rpc_to_us - (float)rpc_from_us)) - 20.0f);

	while (rpc_out < rpc_to_us)
	{
		rpc_out++;
		delayMicroseconds(up_sleep_us);
	}
}

void ramp_down(uint16_t rpc_from_us, uint16_t rpc_to_us, uint32_t ramp_down_time_us)
{
	uint16_t down_sleep_us = (uint16_t)ceil(((float)ramp_down_time_us / ((float)rpc_from_us - (float)rpc_to_us)) - 20.0f);
		//(ramp_down_time_us / (rpc_from_us - rpc_to_us)) - 20;

	while (rpc_out > rpc_to_us)
	{
		rpc_out--;
		delayMicroseconds(down_sleep_us);
	}
}

void oscilliate(uint16_t amplitude, float frequency, uint32_t time_ms)
{
	uint16_t start_rpc = rpc_out;
	uint32_t time_now = millis();
	uint32_t time = time_now;

	while ((time - time_now) < time_ms)
	{
		time = millis();
		rpc_out = uint16_t((float)start_rpc + (float)amplitude * (float)sin(2.0f * 3.14f * frequency * (float)(time - time_now) / 1000.0f));
	}
}

void sawtooth(uint16_t amplitude, float frequency, uint32_t time_ms)
{
	uint16_t start_rpc = rpc_out;
	uint32_t time_now = millis();
	uint32_t time = time_now;

	while ((time - time_now) < time_ms)
	{
		time = millis();
		rpc_out = uint16_t((float)start_rpc + (float)amplitude * 2.0f * ((float)time * frequency - (float)floor(0.5f + (float)(time - time_now) / 1000.0f * frequency)));
	}
}

void squareTorture(float frequency, uint32_t time_ms)
{
	uint16_t start_rpc = rpc_out;
	uint32_t time_now = millis();
	uint32_t time = time_now;
	randomSeed(100);

	while ((time - time_now) < time_ms)
	{
		time = millis();
		float f = (float)sin(2.0f * 3.14f * frequency * (float)(time - time_now) / 1000.0f);
		
		if (f >= 0)
		{
			f = 1;
		}
		else
		{
			f = -1;
		}

		rpc_out = uint16_t((float)start_rpc + (float)random(10) * 40.0f * f);
	}
}

void toggle_led()
{
	if (digitalRead(LED))
	{
		digitalWrite(LED, 0);
	}
	else
	{
		digitalWrite(LED, 1);
	}
}

void run_test()
{
	// Start motor, and let it spin for 3s
	Serial.write("Start motor. Wait 3s\n");
	rpc_out = rpc_start;
	delay(3000);
	
	// 10s ramp up and ramp down
	Serial.write("Ramp up to max in 10s.\n");
	toggle_led();
	ramp_up(rpc_start, rpc_max, 10000000);
	toggle_led();
	delay(1000);
	Serial.write("Ramp down to start in 10s.\n");
	toggle_led();
	ramp_down(rpc_max, rpc_start, 10000000);
	toggle_led();
	delay(1000);

	// 5s ramp up and ramp down
	Serial.write("Ramp up to max in 1s.\n");
	toggle_led();
	ramp_up(rpc_start, rpc_max, 1000000);
	toggle_led();
	delay(1000);
	Serial.write("Ramp down to start in 1s.\n");
	toggle_led();
	ramp_down(rpc_max, rpc_start, 1000000);
	toggle_led();
	delay(1000);

	// 150ms ramp up and ramp down to 50%
	Serial.write("Ramp up to max in 100ms.\n");
	toggle_led();
	ramp_up(rpc_start, rpc_max, 150000);
	toggle_led();
	delay(1000);
	Serial.write("Ramp down to start in 100ms.\n");
	toggle_led();
	ramp_down(rpc_max, rpc_mid, 150000);
	toggle_led();
	delay(1000);
		
	// Oscilliate at 1Hz, 2Hz, 3Hz, 4Hz for 5s
	Serial.write("Oscillate for 5s in 0.5Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	oscilliate(400, 0.5, 5000);
	toggle_led();
	Serial.write("Oscillate for 5s in 1Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	oscilliate(400, 1, 5000);
	toggle_led();
	Serial.write("Oscillate for 5s in 2Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	oscilliate(400, 2, 5000);
	toggle_led();
	Serial.write("Oscillate for 5s in 4Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	oscilliate(400, 4, 5000);
	toggle_led();
	
	/*
	Serial.write("Wait 3s on 50%\n");
	rpc_out = rpc_mid;
	delay(3000);

	// Sawtooth at 1Hz
	Serial.write("Sawtooth for 5s in 0.5Hz\n");
	sawtooth(400, 0.5, 5000);
	*/
	
	Serial.write("Wait 3s on 50%\n");
	rpc_out = rpc_mid;
	delay(3000);

	// Square wave
	Serial.write("Torture motor for 5s in 0,5Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	squareTorture(0.5, 5000);
	toggle_led();
	Serial.write("Torture motor for 5s in 1Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	squareTorture(1, 5000);
	toggle_led();
	rpc_out = rpc_mid;
	Serial.write("Torture motor for 5s in 2Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	squareTorture(2, 5000);
	toggle_led();
	Serial.write("Torture motor for 5s in 5Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	squareTorture(5, 5000);
	toggle_led();
	Serial.write("Torture motor for 10s in 10Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	squareTorture(10, 5000);
	toggle_led();
	rpc_out = rpc_mid;
	Serial.write("Torture motor for 5s in 15Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	squareTorture(15, 5000);
	toggle_led();
	Serial.write("Torture motor for 5s in 20Hz\n");
	toggle_led();
	rpc_out = rpc_mid;
	squareTorture(20, 5000);
	toggle_led();
	
	Serial.write("Wait 3s at start\n");
	rpc_out = rpc_start;
	delay(3000);
	// Stop motor
	Serial.write("Stop motor\n");
	rpc_out = rpc_min;
}

void setup()
{
	pinMode(SERVOUT, OUTPUT);
	digitalWrite(SERVOUT, 0);

	cli();
	setup_timer_interrupt();
	sei();
	Serial.begin(19200);
	digitalWrite(LED, 1);
	// Wait for esc to boot
	delay(5000);
	while (++i <= 5){
		digitalWrite(LED, 0);
		delay(500);
		digitalWrite(LED, 1);
		delay(500);
	}
	digitalWrite(LED, 0);
	
	Serial.write("Start test.\n");
	run_test();
	Serial.write("Test done.\n");
}

void loop()
{
	toggle_led();
	delay(500);
}
