#pragma once

#define TRIANGLE 0
#define BALL 1
#define TRAPEZE 2
#define TYPE_S 3
#define TYPE_Z 4


struct term//терм нечеткой переменной
{
	float min = 0;//начало кривой
	float max = 0;//конец кривой
	char shape = 0;//номер формы кривой терма
};

struct rull //правило нечеткой логики
{
	float mu = 0;//принадлежность к терму переменной 
	char term = 0;//номер терма в данной переменной
};

float fuzzyOr(float A, float B);//нечеткое ИЛИ
float fuzzyAnd(float A, float B);//нечеткое И
float fuzzyNot(float A);//нечеткое НЕ

/*фазификация и мат описание термов*/
float fuzzifier(term Term, float A);//функция фазификатора
float triangle(float min, float max, float A);
float ball(float min, float max, float A);
float trapeze(float min, float max, float A);
float typeS(float min, float max, float A);
float typeZ(float min, float max, float A);

/*дефазификация и обратное мат описание термое*/
float defuzzifier(term termB, float muB);//функция дефазификатора
float detriangle(float min, float max, float A);
float deball(float min, float max, float A);
float detrapeze(float min, float max, float A);
float detypeS(float min, float max, float A);
float detypeZ(float min, float max, float A);

/*нечеткий логический вывод*/
float Mamdany_Singlton(rull* rulls, term *termB, char lenRulls);
/*пока будет только Мамдани. Потом можно добавить чо нить еще типо Ларсена или Такаги-Сугено (не хочу можно не надо)*/