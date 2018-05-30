#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define MAXKEY	1024				// 哈希表容量
#define CH_EOF   (-1)				// 文件尾标识


/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*动态字符串定义*/
typedef struct DynString
{
	int count; //字符串长度
	int capacity; //包含该字符串的缓冲区长度
	char *data; //指向字符串的指针
}DynString;
void dynstring_init(DynString *pstr, int initsize);//初始化动态字符串存储容量，用于dynstring_reset()
void dynstring_free(DynString *pstr);//释放动态字符串使用的内存空间,用于dynstring_reset()
void dynstring_realloc(DynString *cstr, int new_size);//重新分配字符串容量
void dynstring_chcat(DynString *cstr, int ch);//字符串中添加字符
void dynstring_reset(DynString *cstr);//重置动态字符串

////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////

/*动态数组定义*/
typedef struct DynArray
{
	int count;//动态数组元素个数
	int capacity;//动态数组缓冲区长度
	void **data;//指向数据指针的数组
}DynArray;

void dynarray_realloc(DynArray *parr, int new_size);//重新分配动态数组容量,用于dynarray_add()函数
void dynarray_add(DynArray *parr, void *data);//追加动态数组元素
void dynarray_init(DynArray *parr, int initsize);//初始化动态数组存储容量
void dynarray_free(DynArray *parr);//释放动态数组使用的内存空间
int dynarray_search(DynArray *parr, int key);//动态数组元素查找

///////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////

/* 错误级别 */
enum e_ErrorLevel
{
	LEVEL_WARNING,
	LEVEL_ERROR,
};

/* 工作阶段 */
enum e_WorkStage
{
	STAGE_COMPILE,
	STAGE_LINK,
};

void handle_exception(int stage, int level, char *fmt, va_list ap);//异常处理：作为编译警告、错误和链接错误调用的功能函数
void warning(char *fmt, ...);//编译警告处理
void error(char *fmt, ...);//错误处理
void expect(char *msg);//提示错误，此处缺少某个语法成分
void skip(int c);//跳过单词v取下一个单词，跳过单词v, 取下一单词, 如果当前单词不是v, 提示错误
void link_error(char *fmt, ...);//链接错误处理
void *get_tkstr(int v);//取得单词v所代表的源码字符串

////////////////////////////////////////////////////////////////////////////////////////////////////

/*单词存储结构定义*/
typedef struct TkWord
{
	int tkcode;//单词编码
	struct TkWord *next; //指向哈希冲突的同义词
	char *spelling; //单词字符串
}TkWord;

int ELFHash(char *str);//计算哈希地址
TkWord* tkword_direct_insert(TkWord* tp);//将运算符、关键字、常量直接放入单词表
TkWord* tkword_find(char *p, int keyno);//在单词表中查找单词,用于tkword_insert()
void *mallocz(int size);//分配堆内存并将数据初始化为'0',用于tkword_insert()
TkWord* tkword_insert(char *p);//标识符插入单词表，先查找，查找不到再插入单词表

/////////////////////////////////////////////////////////////////////////////////////////////////////

/* 单词编码 */
enum e_TokenCode
{
	/* 运算符及分隔符 */
	TK_PLUS,		// + 加号
	TK_MINUS,		// - 减号
	TK_STAR,		// * 星号
	TK_DIVIDE,		// / 除号
	TK_MOD,			// % 求余运算符
	TK_EQ,			// == 等于号
	TK_NEQ,			// != 不等于号
	TK_LT,			// < 小于号
	TK_LEQ,			// <= 小于等于号
	TK_GT,			// > 大于号
	TK_GEQ,			// >= 大于等于号
	TK_ASSIGN,		// = 赋值运算符 
	TK_POINTSTO,	// -> 指向结构体成员运算符
	TK_DOT,			// . 结构体成员运算符
	TK_AND,         // & 地址与运算符
	TK_OPENPA,		// ( 左圆括号
	TK_CLOSEPA,		// ) 右圆括号
	TK_OPENBR,		// [ 左中括号
	TK_CLOSEBR,		// ] 右圆括号
	TK_BEGIN,		// { 左大括号
	TK_END,			// } 右大括号
	TK_SEMICOLON,	// ; 分号    
	TK_COMMA,		// , 逗号
	TK_ELLIPSIS,	// ... 省略号
	TK_EOF,			// 文件结束符

	/* 常量 */
	TK_CINT,		// 整型常量
	TK_CCHAR,		// 字符常量
	TK_CSTR,		// 字符串常量								

	/* 关键字 */
	KW_CHAR,		// char关键字
	KW_SHORT,		// short关键字
	KW_INT,			// int关键字
	KW_VOID,		// void关键字  
	KW_STRUCT,		// struct关键字   
	KW_IF,			// if关键字
	KW_ELSE,		// else关键字
	KW_FOR,			// for关键字
	KW_CONTINUE,	// continue关键字
	KW_BREAK,		// break关键字   
	KW_RETURN,		// return关键字
	KW_SIZEOF,		// sizeof关键字

	KW_ALIGN,		// __align关键字	
	KW_CDECL,		// __cdecl关键字 standard c call
	KW_STDCALL,     // __stdcall关键字 pascal c call

	/* 标识符 */
	TK_IDENT
};

/* 词法状态 */
enum e_LexState
{
	LEX_NORMAL,
	LEX_SEP
};

void init_lex();//词法分析初始化
void skip_white_space();//忽略空格,TAB及回车
void parse_comment();//解析注释
void preprocess();//预处理，忽略分隔符及注释
int is_nodigit(char c);//判断c是否为字母(a-z,A-Z)或下划线(-)
int is_digit(char c);//判断c是否为数字
TkWord* parse_identifier();//解析标识符
void parse_num();//解析整型常量
void parse_string(char sep);//解析字符常量、字符串常量
void get_token();//取单词

//////////////////////////////////////////////////////////////////////////////////////////////////////

void getch();//从源文件中读取一个字符

extern TkWord* tk_hashtable[MAXKEY];// 单词哈希表
extern DynArray tktable;			// 单词动态数组
extern DynString tkstr;		//单词字符串
extern DynString sourcestr;	//单词源码字符串

extern char ch;		//当前取到的源码字符
extern int token;		//单词编码
extern int tkvalue;	//单词值
extern int line_num;	//行号

extern FILE *fin;
extern char *filename;
extern char *outfile;

///////////////////////////////////////////////////////////////////////////////////////////////////////
/* 语法状态 */
enum e_SynTaxState
{
	SNTX_NUL,       // 空状态，没有语法缩进动作
	SNTX_SP,		// 空格 int a; int __stdcall MessageBoxA(); return 1;
	SNTX_LF_HT,		// 换行并缩进，每一个声明、函数定义、语句结束都要置为此状态
	SNTX_DELAY      // 延迟取出下一单词后确定输出格式，取出下一个单词后，根据单词类型单独调用syntax_indent确定格式进行输出 
};

/* 存储类型 */
enum e_StorageClass
{
	SC_GLOBAL = 0x00f0,		// 包括：包括整型常量，字符常量、字符串常量,全局变量,函数定义          
	SC_LOCAL = 0x00f1,		// 栈中变量
	SC_LLOCAL = 0x00f2,       // 寄存器溢出存放栈中
	SC_CMP = 0x00f3,       // 使用标志寄存器
	SC_VALMASK = 0x00ff,       // 存储类型掩码             
	SC_LVAL = 0x0100,       // 左值       
	SC_SYM = 0x0200,       // 符号	

	SC_ANOM = 0x10000000,     // 匿名符号
	SC_STRUCT = 0x20000000,     // 结构体符号
	SC_MEMBER = 0x40000000,     // 结构成员变量
	SC_PARAMS = 0x80000000,     // 函数参数
};

extern int syntax_state;
extern int syntax_level;

void translation_unit();			//解析翻译单位
void external_declaration(int l);	//解析外部声明
int type_specifier();				//类型区分符
//void init_declarator_list(int l);	
void initializer();					//解析初值符
void struct_specifier();			//解析结构区分符
void struct_declaration_list();		//解析结构声明符表
void struct_declaration();			//解析结构声明
void declarator();					//解析声明符
void function_calling_convention();	//解析函数调用约定
void struct_member_alignment();		//解析结构成员对齐
void direct_declarator();			//解析直接声明符
void direct_declarator_postfix();	//直接声明符后缀
void parameter_type_list();			//解析形参类型表
void funcbody();					//解析函数体
void statement();					//解析语句
void compound_statement();			//解析复合语句
int is_type_specifier(int v);		//判断是否为类型区分符			
void expression_statement();		//解析表达式语句
void for_statement();				//解析for语句
void break_statement();				//解析break语句
void continue_statement();			//解析continue语句
void if_statement();				//解析if语句
void return_statement();			//解析return语句
void assignment_expression();		//解析赋值表达式
void expression();					//解析表达式
void equality_expression();			//解析相等类表达式
void relational_expression();		//解析关系表达式
void additive_expression();			//解析加减类表达式
void multiplicative_expression();	//解析乘除类表达式
void unary_expression();			//解析一元表达式
void argument_expression_list();	//解析实参表达式表
void postfix_expression();			//解析后缀表达式
void primary_expression();			//解析初等表达式
void sizeof_expression();			//解析sizeof表达式
void syntax_indent();				//语法缩进