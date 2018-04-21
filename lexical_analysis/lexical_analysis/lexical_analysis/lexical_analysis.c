#include "lexical_analysis.h"

FILE *fin = NULL;
char *filename;
char *outfile;

/***********************************************************
* 功能:	词法着色
**********************************************************/
void color_token(int lex_state)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	char *p;
	switch (lex_state)
	{
	case LEX_NORMAL:
	{
		if (token >= TK_IDENT)
			SetConsoleTextAttribute(h, FOREGROUND_INTENSITY);
		else if (token >= KW_CHAR)
			SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		else if (token >= TK_CINT)
			SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN);
		else
			SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
		p = get_tkstr(token);
		printf("%s", p);
		break;
	}
	case LEX_SEP:
		printf("%c", ch);
		break;
	}
}

/***********************************************************
*  功能:	词法分析测试
**********************************************************/
void test_lex()
{
	do
	{
		get_token();
		color_token(LEX_NORMAL);
	} while (token != TK_EOF);
	printf("\n代码行数: %d行\n", line_num);
}


/***********************************************************
* 功能:	初始化
**********************************************************/
void init()
{
	line_num = 1;
	init_lex();
}


/***********************************************************
* 功能:	扫尾清理工作
**********************************************************/
void cleanup()
{
	int i;
	for (i = TK_IDENT; i < tktable.count; i++)
	{
		free(tktable.data[i]);
	}
	free(tktable.data);
}

/***********************************************************
* 功能:	得到文件扩展名
* fname:	文件名称
**********************************************************/
char *get_file_ext(char *fname)
{
	char *p;
	p = strrchr(fname, '.');
	return p + 1;
}
/***********************************************************
* 功能:	从源文件中读取一个字符
**********************************************************/
void getch()
{
	ch = getc(fin);
}


/***********************************************************
* 功能:	main主函数
**********************************************************/
int main(int argc, char ** argv)
{

	fin = fopen(argv[1], "rb");
	system("pause");
	if (!fin)
	{
		printf("不能打开SC源文件!\n");
		return 0;
	}
	init();
	getch();
	test_lex();
	cleanup();
	fclose(fin);
	printf("%s 词法分析成功!", argv[1]);
	system("pause");
	return 1;
}