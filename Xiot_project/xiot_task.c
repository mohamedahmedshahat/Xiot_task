/*this is the main File of my application (LED-switch) */
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "uart.h"
void switch_init(void);
void EnableInterrupts(void);  // Enable interrupts
void Xiot_function();
void UART_OutDec(unsigned long);
void PLL_Init(void){
// 0) Use RCC2
SYSCTL_RCC2_R |= 0x80000000; // USERCC2
// 1) bypass PLL while initializing
SYSCTL_RCC2_R |= 0x00000800; // BYPASS2, PLL bypass
// 2) select the crystal value and oscillator source
SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0) // clear XTAL field, bits 10-6
+ 0x00000540; // 10101, configure for 16 MHz crystal
SYSCTL_RCC2_R &= ~0x00000070; // configure for main oscillator source
// 3) activate PLL by clearing PWRDN
SYSCTL_RCC2_R &= ~0x00002000;
// 4) set the desired system divider
SYSCTL_RCC2_R |= 0x40000000; // use 400 MHz PLL
SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~ 0x1FC00000) // clear system clock divider
+ (4<<22); // configure for 80 MHz clock
// 5) wait for the PLL to lock by polling PLLLRIS
while((SYSCTL_RIS_R&0x00000040)==0){}; // wait for PLLRIS bit
// 6) enable use of PLL by clearing BYPASS
SYSCTL_RCC2_R &= ~0x00000800;
}
void switch_init(){
SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
GPIO_PORTF_DIR_R &= ~0x10; // (c) make PF4 in (built-in button)
GPIO_PORTF_AFSEL_R &= ~0x10; // disable alt funct on PF4
GPIO_PORTF_DEN_R |= 0x10; // enable digital I/O on PF4
GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
GPIO_PORTF_AMSEL_R &= ~0x10; // disable analog functionality on PF4
GPIO_PORTF_PUR_R |= 0x10; // enable weak pull-up on PF4
GPIO_PORTF_IS_R &= ~0x10; // (d) PF4 is edge-sensitive
GPIO_PORTF_IBE_R &= ~0x10; // PF4 is not both edges
GPIO_PORTF_IEV_R &= ~0x10; // PF4 falling edge event
GPIO_PORTF_ICR_R = 0x10; // (e) clear flag4
GPIO_PORTF_IM_R |= 0x10; // (f) arm interrupt on PF4
NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
NVIC_EN0_R = 0x40000000; // (h) enable interrupt 30 in NVIC
EnableInterrupts(); // (i) Enable global Interrupt flag (I)
 }
void LED_init(void){
SYSCTL_RCGC2_R |= 0x00000002; // (a) activate clock for port F
GPIO_PORTF_DIR_R |=0x02; // (c) make PF1 out 
GPIO_PORTF_AFSEL_R &= ~0x02; // disable alt funct on PF1
GPIO_PORTF_DEN_R |= 0x02; // enable digital I/O on PF1
GPIO_PORTF_PCTL_R &= ~0x000000F0; // configure PF1 as GPIO
GPIO_PORTF_AMSEL_R &= ~0x02; // disable analog functionality on PF1
GPIO_PORTF_DATA_R |=0x02;//default state for LED is High

}
void SysTick_Init(void){ // priority 6
NVIC_ST_CTRL_R = 0; // disable SysTick during setup
NVIC_ST_RELOAD_R = 240000000-1;// reload value
NVIC_ST_CURRENT_R = 0; // any write to current clears it
NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0xC0000000;
NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
}
float temp_data;//global variable to store the decimal value of analog pin from 0 to 4095
unsigned long temp_degree; //variable to be assigned with real temperature value
volatile unsigned char switch_pressed=0;// store the status of the switch and it's volatile cause it's value my be changed indirectly
volatile unsigned char temp_flag=0;// check temp Flag
unsigned long pre_temp_degree;
int main(void){
	//initialization 
	PLL_Init();
	switch_init();
	LED_init();
	ADC0_Init();
	UART_Init();
	SysTick_Init();
while(1){
	
Xiot_function();//call Xiot_function multiple times
	temp_degree=(unsigned long)(temp_data/3.3)*100;//conversion equation of temp sensor with assumption 
	if(temp_degree!=pre_temp_degree){ 		
UART_OutString((unsigned char*)" temperature is--> ");
UART_OutDec(temp_degree);
		pre_temp_degree=temp_degree;
	}
}

}
/*----------Xiot_function--------------*/
//input:switch status
//output:LED(low,High) and serialize it's state to PC
void Xiot_function(){
if(switch_pressed){
	switch_pressed=0;//clear Flag
	GPIO_PORTF_DATA_R ^= 0x02; //LED status changed
	UART_OutString((unsigned char*)" Switch pressed ->");//send switch state
	if((GPIO_PORTF_DATA_R&0x02)==0){
  UART_OutString((unsigned char*)" LED OFF");//send LED status
	}
else{
       UART_OutString((unsigned char*)" LED ON");//send LED status
}
}

}
/*----------switch Handler-----------*/
void GPIOPortF_Handler(void){
	switch_pressed=1;//Flag
	GPIO_PORTF_ICR_R = 0x10; // (e) clear flag4
	
}
/*------------Temp_Handler------------*/
void SysTick_Handler(void){
	temp_data=(ADC0_In()/4096)*3.3;// this is analog voltage at 25 C

}




