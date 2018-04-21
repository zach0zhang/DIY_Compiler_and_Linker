#include "lexical_analysis.h"


/***********************************************************
*  功能:		初始化动态字符串储存容量
*  pstr:		动态字符串指针
*  initsize:	字符串初始化分配空间
**********************************************************/
void dynstring_init(DynString *pstr, int initsize)
{
	if (pstr != NULL)
	{
		pstr->data = (char *)malloc(sizeof(char) * initsize);
		pstr->count = 0;
		pstr->capacity = initsize;
	}
}

/***********************************************************
*  功能:	释放动态字符串使用的内存空间
*  pstr:	动态字符串指针
**********************************************************/
void dynstring_free(DynString *pstr)
{
	if (pstr != NULL)
	{
		if (pstr->data) free(pstr->data);
		pstr->count = 0;
		pstr->capacity = 0;
	}
}

/***********************************************************
*  功能:	重置动态字符串,先释放,重新初始化
*  pstr:	动态字符串指针
**********************************************************/
void dynstring_reset(DynString *pstr)
{
	dynstring_free(pstr);
	dynstring_init(pstr, 10);//初始化字符串分配10个字节空间
}

/***********************************************************
*  功能:		重新分配字符串容量
*  pstr:		动态字符串指针
*  new_size:	字符串新长度
**********************************************************/
void dynstring_realloc(DynString *pstr, int new_size)
{
	int capacity;
	char *data;
	capacity = pstr->capacity;
	while (capacity < new_size)
	{
		capacity *= 2;//本来分配空间扩大一倍
	}
	data = (char *)realloc(pstr->data, capacity);
	if (!data)//realloc 返回NULL 即分配失败
		error("内存分配失败");
	pstr->capacity = capacity;
	pstr->data = data;
}

/***********************************************************
*  功能:	追加单个字符到动态字符串对象
*  pstr:	动态字符串指针
*  ch:		所要追加的字符
**********************************************************/
void dynstring_chcat(DynString *pstr, int ch)
{
	int count;
	count = pstr->count + 1;
	if (count > pstr->capacity)
		dynstring_realloc(pstr, count);
	pstr->data[count - 1] = ch;
	pstr->count = count;
}
