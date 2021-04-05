#include "kinematica.h"



double sign(double a){
	if (a>0) return 1;
	else if (a<0) return -1;
	else return 0;
}

void kinematica(uint32_t mode){
	float x,y;
	float Rfl, Rfr, Rrl, Rrr;  
	float Rm;
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
}

void normaliz(uint32_t vel_mean, uint32_t rx_mean, uint32_t dir_mean, uint32_t ry_mean){
	uint32_t d_vel;
	uint32_t d_ry;
	float Rg0;
	uint32_t d_rx;
	uint32_t d_dir;
	
	d_dir=(dir_mean-dir_period_zero);
	d_vel=(vel_mean-vel_period_zero);
	d_rx=(rx_mean-rx_period_zero);
	d_ry=ry_mean-ry_period_zero;
	
	if (fabs(d_dir)<20) d_dir=0;
	if (fabs(d_vel)<20) d_vel=0;
	if (fabs(d_rx)<10) d_rx=0;
	if (fabs(d_ry)<20) d_ry=0;
	
	Rg0=d_dir*k_dir/dir_period_max;
	if (fabs(Rg0)>k_dir) Rg0=sign(Rg0)*k_dir;
	R=L/2/tan(Rg0);
	if (fabs(R)<0.01) R=0.00001*sign(R);
	if (fabs(R)>900) R=10000;
	
	V=(d_vel)/vel_period_max*(d_rx/rx_period_max)*k_vel;
	
	gam=(d_ry)/ry_period_max*k_gam;
	if (fabs(gam)<0.01) gam=0;
	if (fabs(gam)>(pi/2-0.1)) gam=pi/2*sign(gam);
	if (gam!=0) R=100000;
}