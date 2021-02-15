#include "fuzzy_toolbox.h"
/*входные переменные*/
#define	VEL_TERMS_COUNT 3	//число термов скорости
#define	CUR_TERMS_COUNT 3	//число термов тока
#define	FLUX_TERMS_COUNT 3	//число термов поля

/*выходные переменные*/
#define	pVEL_TERMS_COUNT 3	//число термов Р по скорости

/*правила*/
#define	pVEL_RULLS_COUNT 3	//число правил для Р по скорости

int main() {
	/*входные переменные*/
	float indVel = 2;//delta velocity
	float inCur = 8;//current

	/*инициализация функций принадлежности*/
	float muVel[VEL_TERMS_COUNT] = { 0,0,0 };
	float muCur[CUR_TERMS_COUNT] = { 0,0,0 };

	/*выходные переменные*/
	float outPVel;// P from velocity PID

	/*правила*/
	rull pVelRull[pVEL_RULLS_COUNT];

	/*инициализация термов*/
	term vel_var[VEL_TERMS_COUNT];
	term cur_var[CUR_TERMS_COUNT];
	term flux_var[FLUX_TERMS_COUNT];
	term pVel_var[pVEL_TERMS_COUNT];
	
	/*синяя изолента*/
	char i = 0;

	/*описание лингвистических переменных*//*определение термов*/
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

	/*фазификация*/
	for (i = 0; i < VEL_TERMS_COUNT; i++) {
		muVel[i] = fuzzifier(vel_var[i], indVel);
	}

	for (i = 0; i < CUR_TERMS_COUNT; i++) {
		muCur[i] = fuzzifier(cur_var[i], inCur);
	}

	/*блок правил + импликация*/
	//если А мало, то В мало, тогда, заная А', получим В' 
	//с функцией принадлежности mu
	//которая относится к множеству (терму) i

	pVelRull[0].mu = fuzzyOr(muVel[0],muCur[2]);			// ЕСЛИ мала ошибка скорости ИЛИ большой ток,
	pVelRull[0].term = 0;									// ТО П мало
	pVelRull[1].mu = muVel[1];								// ЕСЛИ средняя ошибка скорости,
	pVelRull[1].term = 1;									// ТО П средне
	pVelRull[2].mu = fuzzyAnd(fuzzyNot(muCur[2]), muVel[2]);// ЕСЛИ большая ошибка скорости И ток НЕ большой,
	pVelRull[2].term = 2;									// ТО П большое

	/*композиция+дефазификация*/
	outPVel = Mamdany_Singlton(pVelRull, pVel_var, pVEL_RULLS_COUNT);

	return 0;
}