#include "fuzzy_toolbox.h"
/*������� ����������*/
#define	ER_TERMS_COUNT 6	//����� ������ ��������
#define	DER_TERMS_COUNT 6	//����� ������ ����

/*�������� ����������*/
#define	MOMENT_TERMS_COUNT 6	//����� ������ � �� ��������

/*�������*/
#define	MOMENT_RULLS_COUNT 12	//����� ������ ��� � �� ��������
float fuzzyController(float er, float der);
int main() {
	/*������� ����������*/
	float der = 2;//error' = (v-v_t)'
	float er = 8;//error = (v-v_t)
	float TargetMoment = 0;
	TargetMoment = fuzzyController(er, der);
	return 0;
}

float fuzzyController(float er, float der) {
	/*������������� ������� ��������������*/
	float muEr[ER_TERMS_COUNT] = { 0,0,0 };
	float muDEr[DER_TERMS_COUNT] = { 0,0,0 };

	/*�������� ����������*/
	float TargetMoment = 0;

	/*�������*/
	rull MomRull[MOMENT_RULLS_COUNT];

	/*������������� ������*/
	term er_var[ER_TERMS_COUNT];
	term der_var[DER_TERMS_COUNT];
	term moment_var[MOMENT_TERMS_COUNT];

	/*����� ��������*/
	char i = 0;

	/*�������� ��������������� ����������*//*����������� ������*/
	er_var[0].min = -4;
	er_var[0].max = -2.5;
	er_var[0].shape = TYPE_Z;

	er_var[1].min = -3;
	er_var[1].max = -1;
	er_var[1].shape = TRIANGLE;

	er_var[2].min = -1.5;
	er_var[2].max = 0;
	er_var[2].shape = TRIANGLE;

	er_var[3].min = 0;
	er_var[3].max = 1.5;
	er_var[3].shape = TRIANGLE;

	er_var[4].min = 1;
	er_var[4].max = 3;
	er_var[4].shape = TRIANGLE;

	er_var[5].min = 2.5;
	er_var[5].max = 4;
	er_var[5].shape = TYPE_S;

	der_var[0].min = -8;
	der_var[0].max = -5;
	der_var[0].shape = TYPE_Z;

	der_var[1].min = -5.5;
	der_var[1].max = -2.5;
	der_var[1].shape = TRIANGLE;

	der_var[2].min = -3;
	der_var[2].max = 0;
	der_var[2].shape = TRIANGLE;

	der_var[3].min = 0;
	der_var[3].max = 3;
	der_var[3].shape = TRIANGLE;

	der_var[4].min = 2.5;
	der_var[4].max = 5.5;
	der_var[4].shape = TRIANGLE;

	der_var[5].min = 5;
	der_var[5].max = 8;
	der_var[5].shape = TYPE_S;



	moment_var[0].min = -350;
	moment_var[0].max = -200;
	moment_var[0].shape = TYPE_Z;

	moment_var[1].min = -250;
	moment_var[1].max = -100;
	moment_var[1].shape = TRIANGLE;

	moment_var[2].min = -150;
	moment_var[2].max = 0;
	moment_var[2].shape = TRIANGLE;

	moment_var[3].min = 0;
	moment_var[3].max = 150;
	moment_var[3].shape = TRIANGLE;

	moment_var[4].min = 100;
	moment_var[4].max = 250;
	moment_var[4].shape = TRIANGLE;

	moment_var[5].min = 200;
	moment_var[5].max = 350;
	moment_var[5].shape = TYPE_S;

	/*�����������*/
	for (i = 0; i < ER_TERMS_COUNT; i++) {
		muEr[i] = fuzzifier(er_var[i], er);
	}

	for (i = 0; i < DER_TERMS_COUNT; i++) {
		muDEr[i] = fuzzifier(der_var[i], der);
	}

	/*���� ������ + ����������*/
	//���� � ����, �� � ����, �����, ����� �', ������� �' 
	//� �������� �������������� mu
	//������� ��������� � ��������� (�����) i

	//������ � �������� ������ ������ �����
	//������������� ������
	MomRull[0].mu = fuzzyOr(muEr[3], muDEr[5]);			// ���� ���� ������ �������� ��� ������� �������� ������,
	MomRull[0].term = 3;									// �� � ���
	MomRull[1].mu = muEr[4];								// ���� ������� ������,
	MomRull[1].term = 4;									// �� � �������
	MomRull[2].mu = fuzzyAnd(fuzzyNot(muDEr[5]), muEr[5]);// ���� ������� ������ �������� � �������� ������ �� �������,
	MomRull[2].term = 5;									// �� � �������

	//������������� ������
	MomRull[3].mu = fuzzyOr(muEr[2], muDEr[0]);			
	MomRull[3].term = 2;									
	MomRull[4].mu = muEr[1];								
	MomRull[4].term = 1;								
	MomRull[5].mu = fuzzyAnd(fuzzyNot(muDEr[0]), muEr[0]);
	MomRull[5].term = 0;									


	//����� �������� ������ ������� �����
	//������������� ������.�������� �������������
	MomRull[6].mu = fuzzyAnd(muDEr[2], muEr[3]);
	MomRull[6].term = 3;

	MomRull[7].mu = fuzzyAnd(muDEr[2], fuzzyNot(muEr[3]));
	MomRull[7].term = 5;
	
	//������������� ������, �������� �������������
	MomRull[8].mu = fuzzyAnd(muDEr[3], muEr[2]);
	MomRull[8].term = 2;

	MomRull[9].mu = fuzzyAnd(muDEr[3], fuzzyNot(muEr[2]));
	MomRull[9].term = 0;

	//������� �������� ������ ������� �����
	MomRull[10].mu = fuzzyAnd(muDEr[0], fuzzyOr(fuzzyOr(muEr[3],muEr[4]),muEr[5]));
	MomRull[10].term = 5;

	MomRull[11].mu = fuzzyAnd(muDEr[5], fuzzyOr(fuzzyOr(muEr[2], muEr[1]), muEr[0]));
	MomRull[11].term = 0;

	/*����������+�������������*/
	TargetMoment = Mamdany_Singlton(MomRull, moment_var, MOMENT_RULLS_COUNT);

	TargetMoment += 1400;

	return TargetMoment;
}