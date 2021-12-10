//#include "C:\Users\pedri\Desktop\Trabalho 1\main.h"
#include <16F877A.h>
#device adc=10



#FUSES NOWDT
#FUSES HS
#FUSES NOPUT
#FUSES NOPROTECT
#FUSES NODEBUG
#FUSES BROWNOUT
#FUSES NOLVP
#FUSES NOCPD
#FUSES NOWRT


#use delay(clock=20000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,STREAM=Wireless)

#include "mod_lcd.c"

int16 pontDist = 0;
float tempRef;
float tempSens;


float tempAnterior = 0;
float erro;
float kp = 10;
float ki = 0.2;
float kd = 0.005;
float prop;
float integ = 0;
float deriv;
float PID;

int32 timer;

void pid_control(){
   erro = tempRef - tempSens;
   
   prop = erro * kp;
   
   integ = integ + erro * ki;
   
   deriv = (tempAnterior - tempSens) * kd;
   
   tempAnterior = tempSens;
   
   PID = prop + deriv + integ;
   
   if(PID < 1){
      PID = 0;
      set_pwm2_duty(PID);
   }else{
      set_pwm2_duty((int16)PID);
   }
   
}

void inputDisturbio(){
   
   
   
   set_adc_channel(1);
   delay_us(50);
   
   pontDist = read_adc();
    
   pontDist = pontDist*0.59;
   set_pwm1_duty((int16)pontDist);
   delay_us(50);
}
 
float inputReferencia(){
   
   float pontTempRef;
   
   set_adc_channel(0);
   delay_us(50);
   
   pontTempRef = read_adc() / 10.24;
   
   return pontTempRef;
   
   
}

float inputSensor(){
   float sensTemp;
   
   set_adc_channel(3);
   delay_us(50);
    
   sensTemp = read_adc() / 2.03636;
   
   return sensTemp;
   
   
   
   
}

void main()
{
   
   
   
   setup_adc_ports(ALL_ANALOG);
   setup_adc(ADC_CLOCK_DIV_4);
   
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DIV_BY_16,155,1);

      
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   
   
   
   
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   
   lcd_ini();//Inicia o display
   lcd_escreve('\f'); //Limpa o display
   
      
   while(true){
   delay_ms(1000);
   //timer++;
   
   inputDisturbio();
   
   tempRef = inputReferencia();
   tempSens = inputSensor();
   
   pid_control();
   fprintf(Wireless,"%f %f %f %f %ld\n",tempRef, tempSens, PID, erro, pontDist);
   
   printf(lcd_escreve,"\fREF> %f\nTEM>%fPID>%f", tempRef, tempSens,PID);
   delay_ms(50);
   
  
   
   
   }

}
