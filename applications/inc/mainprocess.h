#ifndef _MAINPROCESS_H_
#define _MAINPROCESS_H_

#include "user_source.h"
#include "user_task.h"
#include "math.h"

extern float ov_p_th;
extern float ow_p_th;
extern float ov_t_th;
extern float ov_f_th;

extern bool AI_Flage;

typedef struct
{
	int write_index;
	bool buffer_full;
	float buffer[30];

} DropDetector_t;

typedef enum
{
	STATE_WAIT_STABLE,
	STATE_NORMAL_OPERATION,
	STATE_LEAK_CHECK,
	STATE_BLOCKED,
	STATE_SECURITY_CHECK,
	STATE_ADD_GAS
} system_state_t;

extern volatile bool valve_state;

void processGoOn(void);

float calculateStandardDeviation(float *data, int n);

void calculate_Fitting(float *data, int n);

void AutomaticValveShutdown(float input);

void CheckPressureValue(float input_f, float input_b);

void ModuleInit(uint16_t number);

void CheckLeak(void);

void Normal_operation(void);

void check_required_items(void);

void process_flow_samples(float *samples, uint8_t count, float *result);

void Ai_control(void);

void check_security(void);

void back_warn(void);

void check_valve_state(void);

void maintask_open_valve(uint8_t source);

void maintask_close_valve(uint8_t source);

void Add_gas(void);

void check_control_singal();
#endif
