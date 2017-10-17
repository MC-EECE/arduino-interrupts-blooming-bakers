// Turns
const unsigned long period = 100;
const uint32_t freq = 1000;

const int bits = 12;
const int N = (2^bits)-1;
const double f = 440.0;
const double Ts = 1/f;



unsigned long start_time;

// Setup and start timer
// tc is the timer to use (TC0, TC1, TC2)
// channel is the compare channel (0, 1, 2)
// irq is the interrupt request corresponding to the timer and channel
// frequency is the rate at which you want the interrupt to fire in Hz
void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
  TC_SetRA(tc, channel, rc/2); //50% high, 50% low
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
  NVIC_EnableIRQ(irq); // Enable the IRQ with the interrupt controller
}

void setup(){


  // Start timer. Parameters are:

  // TC1 : timer counter. Can be TC0, TC1 or TC2
  // 0   : channel. Can be 0, 1 or 2
  // TC3_IRQn: irq number. See table.
  // 40  : frequency (in Hz)
  // The interrupt service routine is TC3_Handler. See table.
  
  startTimer(TC1, 0, TC3_IRQn, freq);

  // Paramters table:
  // TC0, 0, TC0_IRQn  =>  TC0_Handler()
  // TC0, 1, TC1_IRQn  =>  TC1_Handler()
  // TC0, 2, TC2_IRQn  =>  TC2_Handler()
  // TC1, 0, TC3_IRQn  =>  TC3_Handler()
  // TC1, 1, TC4_IRQn  =>  TC4_Handler()
  // TC1, 2, TC5_IRQn  =>  TC5_Handler()
  // TC2, 0, TC6_IRQn  =>  TC6_Handler()
  // TC2, 1, TC7_IRQn  =>  TC7_Handler()
  // TC2, 2, TC8_IRQn  =>  TC8_Handler()

}

void loop()
{
  //analogWriteResolution(bits); 
  
}

//volatile boolean l;

// This function is called every 1/40 sec.
void TC3_Handler()
{
  static uint16_t n = 0;
  double sig = cos(2.0*PI*f*n*Ts);
  uint16_t Nsig = (uint16_t)(N/2*(sig + 1.0));
  analogWrite(DAC0, Nsig);
  n++;
  
  // You must do TC_GetStatus to "accept" interrupt
  // As parameters use the first two parameters used in startTimer (TC1, 0 in this case)
  TC_GetStatus(TC1, 0);

}
