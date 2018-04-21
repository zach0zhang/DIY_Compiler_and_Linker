#include "lexical_analysis.h"

DynString tkstr;		//单词字符串
DynString sourcestr;	//单词源码字符串

char ch;		//当前取到的源码字符
int token;		//单词编码
int tkvalue;	//单词值
int line_num;	//行号


/***********************************************************
* 功能:	词法分析初始化
**********************************************************/
void init_lex()
{
	TkWord *tp;
	static TkWord keywords[] = {
	{ TK_PLUS,		NULL,	  "+" },
	{ TK_MINUS,		NULL,	  "-" },
	{ TK_STAR,		NULL,	  "*" },
	{ TK_DIVIDE,	NULL,	  "/" },
	{ TK_MOD,		NULL,	  "%" },
	{ TK_EQ,		NULL,	  "==" },
	{ TK_NEQ,		NULL,	  "!=" },
	{ TK_LT,		NULL,	  "<" },
	{ TK_LEQ,		NULL,	  "<=" },
	{ TK_GT,		NULL,	  ">" },
	{ TK_GEQ,		NULL,	  ">=" },
	{ TK_ASSIGN,	NULL,	  "=" },
	{ TK_POINTSTO,	NULL,	  "->" },
	{ TK_DOT,		NULL,	  "." },
	{ TK_AND,		NULL,	  "&" },
	{ TK_OPENPA,	NULL,	  "(" },
	{ TK_CLOSEPA,	NULL,	  ")" },
	{ TK_OPENBR,	NULL,	  "[" },
	{ TK_CLOSEBR,	NULL,	  "]" },
	{ TK_BEGIN,		NULL,	  "{" },
	{ TK_END,		NULL,	  "}" },
	{ TK_SEMICOLON,	NULL,	  ";" },
	{ TK_COMMA,		NULL,	  "," },
	{ TK_ELLIPSIS,	NULL,	"..." },
	{ TK_EOF,		NULL,	 "End_Of_File" },
	{ TK_CINT,		NULL,	 	"整型常量" },
	{ TK_CCHAR,		NULL,		"字符常量" },
	{ TK_CSTR,		NULL,		"字符串常量" },
	{ KW_CHAR,		NULL,		"char" },
	{ KW_SHORT,		NULL,		"short" },
	{ KW_INT,		NULL,		"int" },
	{ KW_VOID,		NULL,		"void" },
	{ KW_STRUCT,	NULL,		"struct" },
	{ KW_IF,		NULL,		"if" },
	{ KW_ELSE,		NULL,		"else" },
	{ KW_FOR,		NULL,		"for" },
	{ KW_CONTINUE,	NULL,		"continue" },
	{ KW_BREAK,		NULL,		"break" },
	{ KW_RETURN,	NULL,		"return" },
	{ KW_SIZEOF,	NULL,		"sizeof" },
	{ KW_ALIGN,		NULL,		"__align" },
	{ KW_CDECL,		NULL,		"__cdecl" },
	{ KW_STDCALL,	NULL,		"__stdcall" },
	{ 0,			NULL,		NULL }
	};

	dynarray_init(&tktable, 8);
	for (tp = &keywords[0]; tp->spelling != NULL; tp++)
	{
		tkword_direct_insert(tp);
	}
}

/***********************************************************
*  功能:	忽略空格,TAB及回车
**********************************************************/
void skip_white_space()
{
	while (ch == ' ' || ch == '\t' || ch == '\r')
	{
		if (ch == '\r')
		{
			getch();
			if (ch != '\n')
				return;
			line_num++;
		}
		printf("%c", ch);
		getch();
	}
}

/***********************************************************
*  功能:	解析注释
**********************************************************/
void parse_comment()
{
	getch();
	do
	{
		do
		{
			if (ch == '\n' || ch == '*' || ch == CH_EOF)
				break;
			else
				getch();
		} while (1);
		if (ch == '\n')
		{
			line_num++;
			getch();
		}
		else if (ch == '*')
		{
			getch();
			if (ch == '/')
			{
				getch();
				return;
			}
		}
		else
		{
			error("一直到文件尾未看到配对的注释结束符");
			return;
		}
	} while (1);
}

/***********************************************************
*  功能:	预处理，忽略分隔符及注释
**********************************************************/
void preprocess()
{
	while (1)
	{
		if (ch == ' ' || ch == '\t' || ch == '\r')
			skip_white_space();
		else if (ch == '/')
		{
			//向前多读一个字节看是否是注释开始符，猜错了把多读的字符再放回去
			getch();
			if (ch == '*')
			{
				parse_comment();
			}
			else
			{
				ungetc(ch, fin); //把一个字符退回到输入流中
				ch = '/';
				break;
			}
		}
		else
			break;
	}
}

/***********************************************************
* 功能:	判断c是否为字母(a-z,A-Z)或下划线(-)
* c:		字符值
**********************************************************/
int is_nodigit(char c)
{
	return (c >= 'a'  && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/***********************************************************
* 功能:	判断c是否为数字
* c:		字符值
**********************************************************/
int is_digit(char c)
{
	return c >= '0'&&c <= '9';
}

/***********************************************************
* 功能:	解析标识符
**********************************************************/
TkWord* parse_identifier()
{
	dynstring_reset(&tkstr);
	dynstring_chcat(&tkstr, ch);
	getch();
	while (is_nodigit(ch) || is_digit(ch))
	{
		dynstring_chcat(&tkstr, ch);
		getch();
	}
	dynstring_chcat(&tkstr, '\0');
	return tkword_insert(tkstr.data);
}

/***********************************************************
* 功能:	解析整型常量
**********************************************************/
void parse_num()
{
	dynstring_reset(&tkstr);
	dynstring_reset(&sourcestr);
	do {
		dynstring_chcat(&tkstr, ch);
		dynstring_chcat(&sourcestr, ch);
		getch();
	} while (is_digit(ch));
	if (ch == '.')
	{
		do {
			dynstring_chcat(&tkstr, ch);
			dynstring_chcat(&sourcestr, ch);
			getch();
		} while (is_digit(ch));
	}
	dynstring_chcat(&tkstr, '\0');
	dynstring_chcat(&sourcestr, '\0');
	tkvalue = atoi(tkstr.data);

}

/***********************************************************
* 功能:	解析字符常量、字符串常量
* sep:		字符常量界符标识为单引号(')
字符串常量界符标识为双引号(")
**********************************************************/
void parse_string(char sep)
{
	char c;
	dynstring_reset(&tkstr);
	dynstring_reset(&sourcestr);
	dynstring_chcat(&sourcestr, sep);
	getch();
	for (;;)
	{
		if (ch == sep)
			break;
		else if (ch == '\\')
		{
			dynstring_chcat(&sourcestr, ch);
			getch();
			switch (ch) //解析转义字符
			{
			case '0':
				c = '\0';
				break;
			case 'a':
				c = '\a';
				break;
			case 'b':
				c = '\b';
				break;
			case 't':
				c = '\t';
				break;
			case 'n':
				c = '\n';
				break;
			case 'v':
				c = '\v';
				break;
			case 'f':
				c = '\f';
				break;
			case 'r':
				c = '\r';
				break;
			case '\"':
				c = '\"';
				break;
			case '\'':
				c = '\'';
				break;
			case '\\':
				c = '\\';
				break;
			default:
				c = ch;
				if (c >= '!'&&c < '~')
					warning("非法转义字符：\'\\%c\'", c);
				else
					warning("非法转义字符：\'\\0x%x\'", c);
				break;
			}
			dynstring_chcat(&tkstr, c);
			dynstring_chcat(&sourcestr, ch);
			getch();
		}
		else
		{
			dynstring_chcat(&tkstr, ch);
			dynstring_chcat(&sourcestr, ch);
			getch();
		}
	}
	dynstring_chcat(&tkstr, '\0');
	dynstring_chcat(&sourcestr, sep);
	dynstring_chcat(&sourcestr, '\0');
	getch();
}

/***********************************************************
*  功能:	取单词
**********************************************************/
void get_token()
{
	preprocess();
	switch (ch)
	{
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
	case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
	case 'o': case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
	case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
	case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
	case '_':
	{
		TkWord* tp;
		tp = parse_identifier();
		token = tp->tkcode;
		break;
	}
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		parse_num();
		token = TK_CINT;
		break;
	case '+':
		getch();
		token = TK_PLUS;
		break;
	case '-':
		getch();
		if (ch == '>')
		{
			token = TK_POINTSTO;
			getch();
		}
		else
			token = TK_MINUS;
		break;
	case '/':
		token = TK_DIVIDE;
		getch();
		break;
	case '%':
		token = TK_MOD;
		getch();
		break;
	case '=':
		getch();
		if (ch == '=')
		{
			token = TK_EQ;
			getch();
		}
		else
			token = TK_ASSIGN;
		break;
	case '!':
		getch();
		if (ch == '=')
		{
			token = TK_NEQ;
			getch();
		}
		else
			error("暂不支持'!'(非操作符)");
		break;
	case '<':
		getch();
		if (ch == '=')
		{
			token = TK_LEQ;
			getch();
		}
		else
			token = TK_LT;
		break;
	case '>':
		getch();
		if (ch == '=')
		{
			token = TK_GEQ;
			getch();
		}
		else
			token = TK_GT;
		break;
	case '.':
		getch();
		if (ch == '.')
		{
			getch();
			if (ch != '.')
				error("省略号拼写错误");
			else
				token = TK_ELLIPSIS;
			getch();
		}
		else
		{
			token = TK_DOT;
		}
		break;
	case '&':
		token = TK_AND;
		getch();
		break;
	case ';':
		token = TK_SEMICOLON;
		getch();
		break;
	case ']':
		token = TK_CLOSEBR;
		getch();
		break;
	case '}':
		token = TK_END;
		getch();
		break;
	case ')':
		token = TK_CLOSEPA;
		getch();
		break;
	case '[':
		token = TK_OPENBR;
		getch();
		break;
	case '{':
		token = TK_BEGIN;
		getch();
		break;
	case ',':
		token = TK_COMMA;
		getch();
		break;
	case '(':
		token = TK_OPENPA;
		getch();
		break;
	case '*':
		token = TK_STAR;
		getch();
		break;
	case '\'':
		parse_string(ch);
		token = TK_CCHAR;
		tkvalue = *(char *)tkstr.data;
		break;
	case '\"':
	{
		parse_string(ch);
		token = TK_CSTR;
		break;
	}
	case EOF:
		token = TK_EOF;
		break;
	default:
		error("不认识的字符:\\x%02x", ch); //上面字符以外的字符，只允许出现在源码字符串，不允许出现的源码的其它位置
		getch();
		break;
	}
}