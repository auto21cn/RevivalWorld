/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : math.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-04-22
 * Note   : included by Simul_Efun Object
 * Update :
 *  o 2002-00-00
 *
 -----------------------------------------
 */

// 四捨五入
float roff(float r)
{
	return r - floor(r) >= 0.5 ? ceil(r) : floor(r);
}

// 絕對值
float abs(float r)
{
	return r > 0 ? r : -r;
}

// 畢式定理
float pythagorean_thm(mixed r1, mixed r2)
{
	return sqrt(pow(to_float(r1), 2)+pow(to_float(r2), 2));
}

// 大數次方運算
string big_number_pow(mixed number, mixed exponent)
{
	string result = number;

	if( count(exponent, "<", 0) )
		error("exponent must be > 0.");
		
	if( count(exponent, "==", 0) )
		return "1";

	while(count(exponent = count(exponent, "-", 1), ">", 0))
		result = count(result, "*", number);
	
	return result;
}

// 建立次方差級數
int *expdiff_progression(int start, int elem, int diff, int exp)
{
	int *progression;

	if( elem < 0 ) return 0;
	
	progression = allocate(elem);
	
	while(elem--)
		progression[elem] = to_int(start + diff*pow(elem, exp));
	
	return progression;
}

// C 的 a 取 b
string C_ab(int a, int b)
{
	string res="1";
	int i, j;
	
	if( a < 1 || b <= 0 || a <= b ) return a+"";
	
	if( a > 2*b ) 
	{
		i = a-b;
		j = b;
	}
	else
	{
		i = b;
		j = a-b;
	}
	
	while(a>i) res = count(res, "*", a--);

	while(j) res = count(res, "/", j--);
	
	return res;
}

// 將 int 陣列元素全部相加
int int_array_addition(int *arr)
{
	int total;
	
	foreach( int i in arr )
		total+=i;

	return total;
}

// 機率
// ([ elem1 : prob1, elem2 : prob2,... ]) 
mixed probability(mapping data)
{
	array keys = keys(data);
	int *values = values(data);
	int prob, size = sizeof(data);
	
	while(size--)
	{
		prob = int_array_addition(values[0..size]);

		if( random(prob) < values[size] )
			return keys[size];
	}
}
	