#include "grammar_analysis.h"

TkWord* tk_hashtable[MAXKEY];	// 单词哈希表
DynArray tktable;		// 单词表

/***********************************************************
* 功能:	计算哈希地址
* key:哈希关键字
* MAXKEY:哈希表长度
**********************************************************/
int ELFHash(char *str)
{
	int hash = 0;
	int x = 0;

	while (*str)
	{
		hash = (hash << 4) + *str++;
		x = hash & 0xf0000000;
		if (x)
			hash ^= x >> 24;
		hash &= ~x;
	}
	return hash % MAXKEY;
}

/***********************************************************
* 功能: 将运算符、关键字、常量直接放入单词表
* tp:	 单词指针
**********************************************************/
TkWord* tkword_direct_insert(TkWord* tp)
{
	int keyno;
	dynarray_add(&tktable, tp);
	keyno = ELFHash(tp->spelling);
	tp->next = tk_hashtable[keyno];
	tk_hashtable[keyno] = tp;
	return tp;
}

/***********************************************************
* 功能:	 在单词表中查找单词
*  p:		单词字符串指针
*  keyno:	单词的哈希值
**********************************************************/
TkWord* tkword_find(char *p, int keyno)
{
	TkWord *tp = NULL, *tp1;
	for (tp1 = tk_hashtable[keyno]; tp1; tp1 = tp1->next)
	{
		if (!strcmp(p, tp1->spelling))
		{
			token = tp1->tkcode;
			tp = tp1;
		}
	}
	return tp;
}

/***********************************************************
* 功能:	分配堆内存并将数据初始化为'0'
* size: 分配内存大小
**********************************************************/
void *mallocz(int size)
{
	void *ptr;
	ptr = malloc(size);
	if (!ptr && size)
		error("内存分配失败");
	memset(ptr, 0, size);
	return ptr;
}

/***********************************************************
* 功能:	标识符插入单词表，先查找，查找不到再插入单词表
* p:		单词字符串指针
**********************************************************/
TkWord* tkword_insert(char *p)
{
	TkWord *tp;
	int keyno;
	char *s;
	char *end;
	int length;

	keyno = ELFHash(p);
	tp = tkword_find(p, keyno);
	if (tp == NULL)
	{
		length = strlen(p);
		tp = (TkWord*)mallocz(sizeof(TkWord) + length + 1);
		tp->next = tk_hashtable[keyno];
		tk_hashtable[keyno] = tp;
		dynarray_add(&tktable, tp);
		tp->tkcode = tktable.count - 1;
		s = (char*)tp + sizeof(TkWord);
		tp->spelling = (char *)s;
		for (end = p + length; p < end;)
		{
			*s++ = *p++;
		}
		*s = (char)'\0';
	}
	return tp;
}