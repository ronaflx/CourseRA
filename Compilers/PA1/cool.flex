/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

 /*
  *  Add Your own definitions here
  */
#include <stdio.h>
static int yyinput();
void count_backspace(const char * const beg, const char * const end) {
	for (const char * ptr = beg;ptr != end;ptr++) {
		if(*ptr == '\n') ++curr_lineno;
	}
}
void read_to_EOF() {
	char ch;
	while( (ch = yyinput()) != EOF ) {
		if(ch == '\n') ++curr_lineno;
	}
}
void read_to_backspace() {
	char ch, pre = 0;
	while( (ch = yyinput()) != EOF ) {
		if(ch == '\n'){
			++curr_lineno;
			if(pre != '\\') break;
		}
		pre = ch;
	}	
}
static int left;
%}

 /*
  * Define names for regular expressions here.
  */

DARROW          =>
ASSIGN		<-
LE		<=
DIGIT		[0-9]
LETTER		[A-Za-z]
SPACE		[\ \f\r\t\v]
NEST_COMMENT	"(*"(.|\n)*"*)"
COMMENT		--.*(\n{0,1})
TYPE		[A-Z][A-Za-z0-9_]*
OBJECT		[a-z][A-Za-z0-9_]*
STRING		\"(.|\n)*\"
QUOTES		\"
LT_COMMENT	"(*"
RT_COMMENT	"*)"
%%

 /*
  *  Nested comments
  */
{NEST_COMMENT} {
	int left = 1, n;
	char pre = 0, * ptr, * yycopy;
	for(ptr = yytext + 2;*ptr && left;++ptr) {
		if(pre == '(' && *ptr == '*') {
			left++;
			pre = 0;
		} else if(pre == '*' && *ptr == ')') {
			left--;
			pre = 0;
		} else {
			pre = *ptr;
		}
	}
	if(left == 0) {
		/* accpected comment */
		count_backspace(yytext, ptr);
		yycopy = strdup(ptr);
		for(n = strlen(yycopy) - 1;n >= 0;--n) {
			unput(yycopy[n]);
		}
		free(yycopy);
	} else {
		/* same with LT_COMMENT */
		count_backspace(yytext, ptr);
		cool_yylval.error_msg = "EOF in comment";
		return (ERROR);
	}
}
{LT_COMMENT} {
	read_to_EOF();
	cool_yylval.error_msg = "EOF in comment";
	return ERROR;
}
{RT_COMMENT} {
	cool_yylval.error_msg = "Unmatch *)";
	return (ERROR);
}
{COMMENT}	{
	count_backspace(yytext, yytext + yyleng);
}
 /*
  *  The multiple-character operators.
  */
"+"			|
"-"			|
"*"			|
"/"			|
"="			|
"<"			|
"."			|
"~"			|
","			|
";"			|
":"			|
"("			|
")"			|
"@"			|
"{"			|
"}"			{
		return yytext[0]; 
}
{ASSIGN}		{ return (ASSIGN); }
{LE}			{ return (LE); }
{DARROW}		{ return (DARROW); }

 /*
 * Keywords are case-insensitive except for the values true and false,
 * which must begin with a lower-case letter.
 */
(?i:class)		{ return (CLASS); }
(?i:else)		{ return (ELSE); }
(?i:fi)			{ return (FI); }
(?i:if)			{ return (IF); }
(?i:in)			{ return (IN); }
(?i:inherits)		{ return (INHERITS); }
(?i:isvoid)		{ return (ISVOID); }
(?i:let)		{ return (LET); }
(?i:loop)		{ return (LOOP); }
(?i:pool)		{ return (POOL); }
(?i:then)		{ return (THEN); }
(?i:while)		{ return (WHILE); }
(?i:case)		{ return (CASE); }
(?i:esac)		{ return (ESAC); }
(?i:new)		{ return (NEW); }
(?i:of)			{ return (OF); }
(?i:not)		{ return (NOT); }
t(?i:rue)	{
	cool_yylval.boolean = 1;
	return (BOOL_CONST); 
}
f(?i:alse)	{ 
	cool_yylval.boolean = 0;
	return (BOOL_CONST); 
}
 /*
 *  String constants (C syntax)
 *  Escape sequence \c is accepted for all characters c. Except for 
 *  \n \t \b \f, the result is c.
 */
{STRING} {
	char pre = 0, * ptr, *yycopy;
	int n, state = 0;
	std::string res;
	for(ptr = yytext + 1;*ptr && state == 0;++ptr) {
		if(pre == '\\') {
			if(*ptr == 'n') {
				res.append(1, '\n');
			} else if(*ptr == 't') {
				res.append(1, '\t');
			} else if(*ptr == 'b') {
				res.append(1, '\b');
			} else if(*ptr == 'f') {
				res.append(1, '\f');
			} else {
				res.append(1, *ptr);
			}
			pre = 0;
		} else if(*ptr == '\"') {
			state = 1;
		} else if(*ptr == '\n') {
			state = 2;
		} else if(*ptr == '\\') {
			pre = *ptr;
		} else {
			pre = *ptr;
			res.append(1, *ptr);
		}
	}

	count_backspace(yytext, ptr);
	yycopy = strdup(ptr);
	for(n = strlen(yycopy) - 1;n >= 0;n--) {
		unput(yycopy[n]);
	}
	free(yycopy);

	if(state == 1) {
		if(res.size() >= MAX_STR_CONST) {
			cool_yylval.error_msg = "String constant too long";
			return (ERROR);
		}
		yycopy = strdup(res.c_str());
		cool_yylval.symbol = stringtable.add_string(yycopy);
		free(yycopy);
		return (STR_CONST);
	} else {
		cool_yylval.error_msg = "Unterminated string constant";
		return (ERROR);
	}
}
{QUOTES} {
	read_to_backspace();
	cool_yylval.error_msg = "Unterminated string constant";
	return (ERROR);
}

 /*
  * constant integer.
  */
{DIGIT}+	{ 
	cool_yylval.symbol = inttable.add_string(yytext);
	return (INT_CONST);
}
 /*
  * indentifier
  */
{TYPE}	{
	cool_yylval.symbol = idtable.add_string(yytext);
	return (TYPEID);
}
{OBJECT}	{
	cool_yylval.symbol = idtable.add_string(yytext);
	return (OBJECTID);
}
 /*
  * eat whitespace.
  * store current line number.
  * set error message.
  */
{SPACE}+  {
	}
[\n]	{ 
	++curr_lineno; 
}
 /*
  * error_msg : nothing match, left_comment is nozero
  */
.	{
	cool_yylval.error_msg = yytext;
	return ERROR;
}

%%

