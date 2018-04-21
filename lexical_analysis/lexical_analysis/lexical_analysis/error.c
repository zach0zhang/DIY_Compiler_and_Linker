#include "lexical_analysis.h"

/******************************************************************
* 异常处理
* stage: 编译阶段还是链接阶段
* level：错误级别
* fmt：参数输出格式
* ap：可变参数列表
*******************************************************************/
void handle_exception(int stage, int level, char *fmt, va_list ap)
{
	char buf[1024];
	vsprintf(buf, fmt, ap);
	if (stage == STAGE_COMPILE)
	{
		if (level == LEVEL_WARNING)
			printf("%s(第%d行): 编译警告: %s!\n", filename, line_num, buf);
		else
		{
			printf("%s(第%d行): 编译错误: %s!\n", filename, line_num, buf);
			exit(-1);
		}
	}
	else
	{
		printf("链接错误: %s!\n", buf);
		exit(-1);
	}
}

/******************************************************************
* 编译警告处理
* fmt：参数输出格式
* ap：可变参数列表
*******************************************************************/
void warning(char *fmt, ...)
{
	va_list ap = NULL;
	va_start(ap, fmt);
	handle_exception(STAGE_COMPILE, LEVEL_WARNING, fmt, ap);
	va_end(ap);
}

/******************************************************************
* 编译错误处理
* fmt：参数输出格式
* ap：可变参数列表
*******************************************************************/
void error(char *fmt, ...)
{
	va_list ap = NULL;
	va_start(ap, fmt);
	handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
	va_end(ap);
}

/******************************************************************
* 链接错误处理
* fmt：参数输出格式
* ap：可变参数列表
*******************************************************************/
void link_error(char *fmt, ...)
{
	va_list ap = NULL;
	va_start(ap, fmt);
	handle_exception(STAGE_LINK, LEVEL_ERROR, fmt, ap);
	va_end(ap);
}

/******************************************************************
* 提示错误，此处缺少某个语法成分
* msg：需要什么语法成分
*******************************************************************/
void expect(char *msg)
{
	error("缺少%s", msg);
}

/******************************************************************
* 功能：取得单词v所代表的源码字符串
* v：单词编号
*******************************************************************/
char *get_tkstr(int v)
{
	if (v > tktable.count)
		return NULL;
	else if (v >= TK_CINT && v <= TK_CSTR)
		return sourcestr.data;
	else
		return ((TkWord*)tktable.data[v])->spelling;
}