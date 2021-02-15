#include "fuzzy_toolbox.h"
#include "math.h"
/*нечеткие логические функции*/
float fuzzyOr(float A, float B) {
	if (A > B) return A;
	else return B;
}

float fuzzyAnd(float A, float B) {
	if (A > B) return B;
	else return A;
}

float fuzzyNot(float A) {
	float mu;
	mu = 1 - A;
	return mu;
}

float mean(float a, float b) {
	return (a + b) / 2;
}

/*фазификатор*/
float fuzzifier(term Term, float A) {
	float mu=0;
	switch (Term.shape) {
	case TRIANGLE:
		mu = triangle(Term.min, Term.max, A);
		break;
	case BALL:
		mu = ball(Term.min, Term.max, A);
		break;
	case TRAPEZE:
		mu = trapeze(Term.min, Term.max, A);
		break;
	case TYPE_S:
		mu = typeS(Term.min, Term.max, A);
		break;
	case TYPE_Z:
		mu = typeZ(Term.min, Term.max, A);
		break;
	}
	return mu;
}


/*функции фазификации*/
float triangle(float min, float max, float A) {
	float mu = 0;
	float m = mean(min, max);
	if (A > max || A < min) mu = 0;
	else if (A < m) {
		mu = (A - min) / (max - m);
	}
	else {
		mu = -(A - max) / (max - m);
	}
	return mu;
}
float ball(float min, float max, float A) {
	float mu = 0;
	float m = mean(min, max); 
	if (A > max || A < min) mu = 0;
	else mu = -((A - m) * (A - m)) / (max - m) / (max - m) + 1;
	return mu;
}
float trapeze(float min, float max, float A) {
	float mu = 0;
	float dm = (max - min) / 3;
	if (A > max || A < min) mu = 0;
	else if (A > (min + dm) && A < (max - dm)) mu = 1;
	else if (A < (min + dm) && A > (min)){
		mu = (A - min) / dm;
	}
	else {
		mu = -(A - max) / dm;
	}
	return mu;
}
float typeS(float min, float max, float A) {
	float mu = 0;
	if (A < min) mu = 0;
	else if (A > max) mu = 1;
	else {
		mu = (A - min) / (max - min);
	}
	return mu;
}
float typeZ(float min, float max, float A) {
	float mu = 0;
	if (A > max) mu = 0;
	else if (A < min) mu = 1;
	else {
		mu = -(A - max) / (max - min);
	}
	return mu;
}

/*функции дефазификации */
float detriangle(float min, float max, float muB) {
	float B = 0;
	float m = mean(min, max);
	if (muB == 0) B = 0;
	else B = mean((muB * (max - m) + min), (-muB * (max - m) + max));
	return B;
}
float deball(float min, float max, float muB) {
	float B = 0;
	float m = mean(min, max);
	if (muB == 0) B = 0;
	else B = mean(sqrtf((1 - muB) * (max - m) * (max - m)) + m, -sqrtf((1 - muB) * (max - m) * (max - m)) + m);
	return B;
}
float detrapeze(float min, float max, float muB) {
	float B = 0;
	float dm = (max - min) / 3;
	if (muB == 0) B = 0;
	else if (muB == 1) B = mean(min + dm, max - dm);
	else B = mean((muB * dm + min), (-muB * dm + max));
	return B;
}
float detypeS(float min, float max, float muB) {
	float B = 0;
	if (muB == 0) B = 0;
	else if (muB == 1) B = max;
	else B = mean((muB * (max - min) + min), max);
	return B;
}
float detypeZ(float min, float max, float muB) {
	float B = 0;
	if (muB == 0) B = 0;
	else if (muB == 1) B = min;
	else B = mean((-muB * (max - min) + max), min);
	return B;
}

/*дефазификатор*/
float defuzzifier(term termB, float muB) {
	float B = 0;
	switch (termB.shape) {
	case TRIANGLE:
		B = detriangle(termB.min, termB.max, muB);
		break;
	case BALL:
		B = deball(termB.min, termB.max, muB);
		break;
	case TRAPEZE:
		B = detrapeze(termB.min, termB.max, muB);
		break;
	case TYPE_S:
		B = detypeS(termB.min, termB.max, muB);
		break;
	case TYPE_Z:
		B = detypeZ(termB.min, termB.max, muB);
		break;
	}
	return B;
}

/*функции нечеткого логического вывода*/
/*Мамдани->синглтон*/
float Mamdany_Singlton(rull *rulls, term *termB, char lenRulls) {
	float outB = 0;
	float B = 0;
	char i;
	float sumMu = 0;

	for (i = 0; i < lenRulls; i++) {
		sumMu += rulls[i].mu;
	}

	for (i = 0; i < lenRulls; i++) {
		B = defuzzifier(termB[rulls[i].term], rulls[i].mu);
		outB += B * rulls[i].mu / sumMu;
	}
	return outB;
}