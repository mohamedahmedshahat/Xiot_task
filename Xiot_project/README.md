# internship_task_Xiot
#instruction for building this app:
1-configure IO pins and initialize it and setting up it's interrputs in NVIC module
2-configure ADC module and it's driver in seperate file
3-configure uart module and it's driver in seperate file
4-configure Systick timer and setting up it's interrupt ISR to sample temp sensor analog signal through ADC driver and serialize it's data through Uart module
5-write down Xiot_function and take switch state as input and based on it, drive led (High,low) also send serial data of led and switch status through uart module to PC through debugger cable
6-configure usp port on PC to receive data from Tiva C and configuring it's data rate through an application(puTTy.ex)



#Assumption i have made:
i assume that the temp sensor will measure temp range from 0 to 100 C and upon that i derived the equation that will calculate the temperature and i calrify that in my code (temperature changes from 0 t 100 c and the input voltage aslo changes from 0 to 3.3 volt which is VDD of Uc) so i assumed the relation is linear and derived the equation upon that
