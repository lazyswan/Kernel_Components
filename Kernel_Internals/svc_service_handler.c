#define SVC_SERVICE_HANDLER 0
//Defining svc function: Imagine this like system call in OS; We are assigning functions to each system call number 
#if SVC_SERVICE_HANDLER==1
int __svc(0x00) svc_service_add(int x, int y);
int __svc(0x01) svc_service_sub(int x, int y);
int __svc(0x02) svc_service_mul(int x, int y);
int __svc(0x03) svc_service_div(int x, int y);
void SVC_Handler_C(unsigned int * svc_args);
int x,y,z;
int main(){
	x=1;
	y=5;
	
	z=svc_service_add(x,y);
	
	x=9;
	y=2;
	z=svc_service_sub(x,y);
	
	x=2;
	y=8;
	z=svc_service_mul(x,y);
	
	x=10;
	y=2;
	z=svc_service_div(x,y);
}


__asm void SVC_Handler(void){
	
	//Find which Stack is currently used bit2 of LR, EXE_RETURN
	TST LR, #4			//0b 0100
	ITE EQ 					//If Then Else
	MRSEQ R0, MSP 	// If Value is 0 then copy MSP to RO is used,
	MRSNE R0, PSP		// If value is !0 then copy PSP to RO is used	
	B				__cpp(SVC_Handler_C)
}

/*The Stack Frame is directly passed to the below function.
svc_args[0]-R0
svc_args[1]-R1
svc_args[2]-R3
.
.
.
svc_args[6]-PC
*/
void SVC_Handler_C(unsigned int * svc_args){
	//extract PC from stack frame. ; -2 to get 1st Byte to get svcNumber
	unsigned int svc_number = ((char *)svc_args[6])[-2];	
	switch(svc_number){
		case 0:
			//First 2 Arguments are in R0 and R1
			//Retrun value is also stored in R0;
			svc_args[0]=svc_args[0]+svc_args[1];
		break;
		case 1:
			//First 2 Arguments are in R0 and R1
			//Retrun value is also stored in R0;
			svc_args[0]=svc_args[0]-svc_args[1];
		break;
		case 2:
			//First 2 Arguments are in R0 and R1
			//Retrun value is also stored in R0;
			svc_args[0]=svc_args[0]*svc_args[1];
		break;
		case 3:
			//First 2 Arguments are in R0 and R1
			//Retrun value is also stored in R0;
			svc_args[0]=svc_args[0]/svc_args[1];
		break;
		default:
			break;
		
	}
	
	
}
#endif