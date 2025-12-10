#include "user_time.h"


void time4_int_init(void)
{
  tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);
	
  tmr_period_buffer_enable(TMR4, TRUE);
	
  tmr_base_init(TMR4, 999, 239);

  tmr_primary_mode_select(TMR4, TMR_PRIMARY_SEL_RESET);

 // tmr_counter_enable(TMR6, TRUE);

  crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);

  nvic_irq_enable(TMR4_GLOBAL_IRQn, 8, 0);

  tmr_interrupt_enable(TMR4, TMR_OVF_INT, TRUE); 
}