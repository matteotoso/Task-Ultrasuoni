#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "timers.h"                     // ARM.FreeRTOS::RTOS:Timers
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "Ultrasuoni.h"
#include "led.h"
#include "event_groups.h"
#include "eventGroupRullo.h"

extern EventGroupHandle_t xEventGroupRullo;
void TIM4_CH1_Init(void);
void TIM2_CH1_InCapt_SM(void);

#define TRG_TON 12  //tempo per l'impulso del trigger 10us
#define TRG_PRSC 79 //valore prescaler trigger
#define TRG_P 5000000 // periodo trigger 500ms


//funzione per abilitare il pin PB3 come AF
void trgInit(void){

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
  GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6_Msk;
  GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;	
}

//funzione per abilitare il pin PA5 come AF
void echoInit(void){

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE5_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5_Msk;
  GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;
	
}
//funzione per l'inizializzazione dei componenti necessari per il funzionamento dell'ultrasuoni
void ultrsInit(void){
	trgInit();
	echoInit();
	
	xTaskCreate(vTask_SensUltrasuoni, /* Pointer to the function that implements the task.              */
		"Task 1 (SesoreUltrasuoni)",    /* Text name for the task.  This is to facilitate debugging only. */
		50,   /* Stack depth in words.                */
		NULL,  /* We are not using the task parameter. */
		1,     /* This task will run at priority 1.    */
		NULL); /* We are not using the task handle.    */
	
}
//inizializzazione del timer necessario per il funzionamento del trigger( periodo=500ms; impulso=12us)
void TIM4_CH1_Init(){
	//abilitazione registri timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	
	//direzione UP
	TIM4->CR1 &= ~TIM_CR1_DIR;
	
	//impostazione prescaler
	TIM4->PSC =TRG_PRSC;
	
	//impostazione periodo
	TIM4->ARR = TRG_P-1;
	
	//configurazione PWM mode per l'uscita del output 1
	TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM4->CCMR1 |=TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1M_2;
	
	//modifica di CCR1 in ogni istante con PE=0, ad ongli update event con PE=1
//	TIM1->CCMR1 &= ~TIM_CCMR1_OC1PE;
	TIM4->CCMR1 |= TIM_CCMR1_OC1PE;
	
	//Selezione polarità: 0 Attivo alto; 1 attivo basso
	TIM4->CCER &= ~TIM_CCER_CC1P;
	
	//Abilitazione canale 1 complementare
	TIM4->CCER |= TIM_CCER_CC1E;
	
//	//abilitazione OCN1
//	TIM1->BDTR |= TIM_BDTR_MOE;
	
	//valore confronto
	TIM4->CCR1 = TRG_TON-1;
	
	//abilitazione timer1
	TIM4->CR1 |= TIM_CR1_CEN;
}
//inizializzazione timer per il conteggio dei microsecondi per il calcolo della distanza
void TIM2_CH1_InCapt_SM(void){
	//abilitazione registri timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	//prescaler
	TIM2->PSC = 79;
	
	//impostazione della direzione
	//00 output
	//01 input, CC1 su TI1
	//10 input, CC1 su TI2
	//11 CC2 slave
	TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;
	
	//input filter   IC2F[3:0] 0 senza filtro
	TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;
	
	//selezione fronti con CC2NP e CC2P
//	00 rising edge
//	01 falling edge
//	11 entrambi
	TIM2->CCER &= ~ (TIM_CCER_CC1P|TIM_CCER_CC1NP);
	TIM2->CCER |=  (TIM_CCER_CC1P);
	
	TIM2->SMCR &= ~TIM_SMCR_TS;
	TIM2->SMCR |= 4<<4;
	
	TIM2->SMCR &= ~TIM_SMCR_SMS;
	TIM2->SMCR |=4;
	
	
	//input prescaler selezione numero frionti per ogni transizione
	//0 tutti i fronti
	TIM2->CCMR1 &= ~(TIM_CCMR1_IC1PSC);
	
	//abilitazione cattura su ch1
	TIM2->CCER |= TIM_CCER_CC1E;
	
	//abilitazione interrupt
	TIM2->DIER |= TIM_DIER_CC1IE;
	
	//abilitazione timer
	TIM2->CR1 |= TIM_CR1_CEN;
	
	//interrupt
	NVIC_SetPriority(TIM2_IRQn,1);
	NVIC_EnableIRQ(TIM2_IRQn);
}

// ISR per il calcolo della distanza
void TIM2_IRQHandler(void){
	int durata,distanza;
	durata=TIM2->CCR1+1;
	distanza=durata/58;
	if (distanza < 5){
		xEventGroupSetBits(xEventGroupRullo,EV_OBJECT_DETECTED);
	}
}		

		

// task per il funzionamento dell'ultrasuoni
void vTask_SensUltrasuoni(void * pvParameters){
	TIM4_CH1_Init();
	TIM2_CH1_InCapt_SM();
	while(1){
	}
}