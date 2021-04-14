#include "kinematica.h"
float gam=0;
float R=9999;
float V=1;
float L=0.945;
float C=0.63;
uint16_t vel_period_max=1710;
uint16_t vel_period_zero=1024;
uint16_t dir_period_max=1710;
uint16_t dir_period_zero=1024;
uint16_t rx_period_zero=340;
uint16_t rx_period_max=1710;
uint16_t ry_period_zero=1024;
uint16_t ry_period_max=340;
float k_vel=5;
float k_dir=pi/2;
float k_gam=pi/2;
float r_wheel=0.2;
char constract;

double sign(double a){
	if (a>0) return 1;
	else if (a<0) return -1;
	else return 0;
}

void normaliz(uint16_t vel_mean, uint16_t rx_mean, uint16_t dir_mean, uint16_t ry_mean){
	float d_vel=0;
	float d_ry=0;
	float Rg0=0;
	float d_rx=0;
	float d_dir=0;
	float vel_coef=0;

	d_dir=(dir_mean-dir_period_zero);
	d_vel=(vel_mean-vel_period_zero);
	d_rx=(rx_mean-rx_period_zero);
	d_ry=ry_mean-ry_period_zero;
	
	if (fabs(d_dir)<20) d_dir=0;
	if (fabs(d_vel)<20) d_vel=0;
	if (fabs(d_rx)<20) d_rx=0;
	if (fabs(d_ry)<20) d_ry=0;
	
	Rg0=d_dir*k_dir/(dir_period_max-dir_period_zero);
	if (fabs(Rg0)>k_dir) Rg0=sign(Rg0)*k_dir;
	R=L/2/tan(Rg0)*10;
	if (fabs(R)<0.1) R=0.00001*sign(R);
	if (fabs(R)>50) R=10000;
	
	V=(d_vel)/((float)(vel_period_max-vel_period_zero));
	vel_coef=(d_rx/((float)(rx_period_max-rx_period_zero)));
	V=V*vel_coef;

	gam=(d_ry)/(ry_period_max-ry_period_zero)*k_gam;
	if (fabs(gam)<0.01) gam=0;
	if (fabs(gam)>(pi/2-0.1)) gam=pi/2*sign(gam);
	if (gam!=0) R=100000;
}

void kinematica(uint32_t mode, Motor_t* Motors, servoTarget_t* Servo){
	float x,y;
	float Rfl, Rfr, Rrl, Rrr;  
	float Rm;
	float gfl, gfr, grl, grr;
	float Vfl, Vfr, Vrl, Vrr;
	mode=2;
	switch(mode)
	{
		case 1:
			k_vel=(constract);// ???????? 3 ??????: 0, V, constract
			break;
		case 2:
			k_vel=5;// ???????? 3 ??????: 0, V, constract
			break;
		case 3:
			k_vel=0;
		break;
	}
	V=V*k_vel;
	x=R*cos(pi/2+gam);
	y=R*sin(pi/2+gam);
	if (R>0){
		gfl=atan2(L/2-x, y-C/2);
		gfr=atan2(L/2-x, y+C/2);
		grl=atan2(-L/2-x, y-C/2);
		grr=atan2(-L/2-x, y+C/2);
	}
	else{
		grr=atan2(L/2+x, -y-C/2);
		grl=atan2(L/2+x, -y+C/2);
		gfr=atan2(-L/2+x, -y-C/2);
		gfl=atan2(-L/2+x, -y+C/2);
	}

	Rfl=fabs(y-C/2)/fabs(cos(gfl));
	Rfr=fabs(y+C/2)/fabs(cos(gfr));
	Rrl=fabs(y-C/2)/fabs(cos(grl));
	Rrr=fabs(y+C/2)/fabs(cos(grr));

	if(Rfl>1000) Rfl=100000;
	if(Rfr>1000) Rfr=100000;
	if(Rrl>1000) Rrl=100000;
	if(Rrr>1000) Rrr=100000;
	Rm=Rfl;
	if (Rm<Rfr) Rm=Rfr;
	if (Rm<Rrl) Rm=Rrl;
	if (Rm<Rrr) Rm=Rrr;

	Vfl=V*Rfl/Rm;
	Vfr=-V*Rfr/Rm;
	Vrl=V*Rrl/Rm;
	Vrr=-V*Rrr/Rm;
	if (fabs(gfl)<0.01) gfl=0;
	if (fabs(gfr)<0.01) gfr=0;
	if (fabs(grl)<0.01) grl=0;
	if (fabs(grr)<0.01) grr=0;
	Motors[0].command=CHANGE_SPEED;
	Motors[0].prevRefImpact=Motors[0].refImpact;
	Motors[0].refImpact=(int16_t)(Vfl*60/r_wheel/2/pi);

	Motors[1].command=CHANGE_SPEED;
	Motors[1].prevRefImpact=Motors[1].refImpact;
	Motors[1].refImpact=(int16_t)(Vfr*60/r_wheel/2/pi);
	
	Motors[2].command=CHANGE_SPEED;
	Motors[2].prevRefImpact=Motors[2].refImpact;
	Motors[2].refImpact=(int16_t)(Vrl*60/r_wheel/2/pi);
	
	Motors[3].command=CHANGE_SPEED;
	Motors[3].prevRefImpact=Motors[3].refImpact;
	Motors[3].refImpact=(int16_t)(Vrr*60/r_wheel/2/pi);

	Servo->targetFrontLeft=gfl;
	Servo->targetFrontRight=gfr;
	Servo->targetRearLeft=grl;
	Servo->targetRearRight=grr;
}
