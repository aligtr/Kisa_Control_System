#include "fuzzy_toolbox.h"
/*������� ����������*/
#define	VEL_TERMS_COUNT 3	//����� ������ ��������
#define	CUR_TERMS_COUNT 3	//����� ������ ����
#define	FLUX_TERMS_COUNT 3	//����� ������ ����

/*�������� ����������*/
#define	pVEL_TERMS_COUNT 3	//����� ������ � �� ��������

/*�������*/
#define	pVEL_RULLS_COUNT 3	//����� ������ ��� � �� ��������

int main() {
	/*������� ����������*/
	float indVel = 2;//delta velocity
	float inCur = 8;//current

	/*������������� ������� ��������������*/
	float muVel[VEL_TERMS_COUNT] = { 0,0,0 };
	float muCur[CUR_TERMS_COUNT] = { 0,0,0 };

	/*�������� ����������*/
	float outPVel;// P from velocity PID

	/*�������*/
	rull pVelRull[pVEL_RULLS_COUNT];

	/*������������� ������*/
	term vel_var[VEL_TERMS_COUNT];
	term cur_var[CUR_TERMS_COUNT];
	term flux_var[FLUX_TERMS_COUNT];
	term pVel_var[pVEL_TERMS_COUNT];
	
	/*����� ��������*/
	char i = 0;

	/*�������� ��������������� ����������*//*����������� ������*/
	vel_var[0].min = 0;
	vel_var[0].max = 1.5;
	vel_var[0].shape = TYPE_Z;

	vel_var[1].min = 1;
	vel_var[1].max = 3;
	vel_var[1].shape = TRIANGLE;

	vel_var[2].min = 2.5;
	vel_var[2].max = 4;
	vel_var[2].shape = TYPE_S;

	cur_var[0].min = 2;
	cur_var[0].max = 3;
	cur_var[0].shape = TYPE_Z;

	cur_var[1].min = 3;
	cur_var[1].max = 6;
	cur_var[1].shape = BALL;

	cur_var[2].min = 6;
	cur_var[2].max = 7;
	cur_var[2].shape = TYPE_S;

	pVel_var[0].min = 1000;
	pVel_var[0].max = 2500;
	pVel_var[0].shape = TRAPEZE;

	pVel_var[1].min = 2000;
	pVel_var[1].max = 4000;
	pVel_var[1].shape = BALL;

	pVel_var[2].min = 3500;
	pVel_var[2].max = 5000;
	pVel_var[2].shape = TRAPEZE;

	/*�����������*/
	for (i = 0; i < VEL_TERMS_COUNT; i++) {
		muVel[i] = fuzzifier(vel_var[i], indVel);
	}

	for (i = 0; i < CUR_TERMS_COUNT; i++) {
		muCur[i] = fuzzifier(cur_var[i], inCur);
	}

	/*���� ������ + ����������*/
	//���� � ����, �� � ����, �����, ����� �', ������� �' 
	//� �������� �������������� mu
	//������� ��������� � ��������� (�����) i

	pVelRull[0].mu = fuzzyOr(muVel[0],muCur[2]);			// ���� ���� ������ �������� ��� ������� ���,
	pVelRull[0].term = 0;									// �� � ����
	pVelRull[1].mu = muVel[1];								// ���� ������� ������ ��������,
	pVelRull[1].term = 1;									// �� � ������
	pVelRull[2].mu = fuzzyAnd(fuzzyNot(muCur[2]), muVel[2]);// ���� ������� ������ �������� � ��� �� �������,
	pVelRull[2].term = 2;									// �� � �������

	/*����������+�������������*/
	outPVel = Mamdany_Singlton(pVelRull, pVel_var, pVEL_RULLS_COUNT);

	return 0;
}