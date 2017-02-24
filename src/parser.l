  /**************************************************************************
   *                       COMPILER--VHDL                                   *
   *                                                                        *
   *                        ---LEXER---                                     *
   **************************************************************************/

%{


#define YY_DECL int yylex (void *lvalp)
#define YY_NO_UNPUT

#define yylval (*((YYSTYPE*)lvalp))

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include "auxilary.h"
#include "ly.h"
#include "actions.h"
#include "parser.tab.h"
#include <string>

extern const int MAX_FILE_NEST_DEPTH;

extern int linenumber;

//Pointer to the tail of the symbol table. Form compile.c.
extern SymTab *table;

extern int envior;

extern string outpath;

//From compile.c
extern char *file_scope1;

char *file_scope[MAX_FILE_NEST_DEPTH];  //from aux.h

void flush_buffer();

char *Tolower();

char *Getid();

void process_buffer();

char *dirname;
char filename[128];
char fileattr1[128];
char input_file[128];

int linet[10];    //line number stack

FILE *tempfile= NULL;

// Define the maximum recursion depth.
const int max_nest_depth = 10;
 
YY_BUFFER_STATE nested_file[max_nest_depth];

int stack_depth;

int commentdepth;

int syncommentdepth;

//bool switch_buffer(const char *,  const char*);

void clean_buffer();

extern my_list *netlist;

extern int isENTITY;

extern bool ERROR;

ostringstream entity_decl;

extern map<string,string> *name_mapping;

%}


  /* The following are the regular expressions of the vhdl lexer. */ 

ws                [ \t]
letter            [A-Za-z]
digit             [0-9]
graphic           [\200-\300]
ldg               ({letter}|{digit}|{graphic}|<)
basicid           (({letter}|{graphic})+(_*({ldg})+)*)
id                ({basicid})|(\\{ldg}+(_*({ldg})+)*\\)
int               {digit}+(_?{digit}+)*
expon             E(\+)?{int}|E\-{int}
real              {int}({expon}|\.{int}|\.{int}{expon}) 
bin_digit         [01]
bin_integer       ({bin_digit}+_?)*
octal_digit       [0-7]
octal_integer     ({octal_digit}+_?)*
hec_digit         [0-9a-f]
hec_integer       ({hec_digit}+_?)*
bin_val           2\#{bin_integer}.?{bin_integer}\#{expon}?
octal_val         8\#{octal_integer}.?{octal_integer}\#{expon}?
hec_val           16\#{hec_integer}.?{hec_integer}\#{expon}?
based_lit         {bin_val}|{octal_val}|{hec_val}
char_lit          \'.?\'
d_quote           \"\"?\"
string            [^"\n]*
string_lit        \"({d_quote}*{string})*\"
binary_value      [01]
octal_value       [0-7]
hec_value         [0-9a-f]
binary_lit        b\"({binary_value}+_?)*\"
octal_lit         o\"({octal_value}+_?)*\"
hec_lit           x\"({hec_value}+_?)*\"
bit_lit           {binary_lit}|{octal_lit}|{hec_lit}
comment           \--[^@\n]*
comment1          \--@[^s]
begsyncom         \--@synthesis_off
endsyncom         \--@synthesis_on
selected_name     {id}\.{id}\.{id}\;
use_clause        "use"{ws}*{selected_name}

%s              vhdlmode

%x		norm comment linecomment syncomment

LETTER		[a-zA-Z]
DIGIT		[0-9]
ID 	        ({LETTER}({LETTER}|{DIGIT}|_)*)
DECINT	        ({DIGIT}+)
MOD             ("module")
ENDMOD          ("endmodule")
PROC            ("process")
ENDPROC         ("endprocess")
ENDTBB          ("endtestbench")
PAS             ("passive")
ACT             ("active")
BOOLEAN         ("boolean")
INPUT           ("input")
OUTPUT          ("output")
DEL             ("delay")
DEF             ("define")
T               ("true")
F               ("false")
INF             ("inf"|"infinity")
SKIP            ("skip")
SLASH           ("\\")
GATE            ("gate")
ENDGATE         ("endgate")
CONSTRAINT      ("constraint")
ENDCONSTRAINT   ("endconstraint")
ASSUMP          ("assumption")
ENDASSUMP       ("endassumption")

%%

"/*" /*start of comment*/		 { BEGIN comment; ++commentdepth; }
<norm>"/*"	 { BEGIN comment; ++commentdepth; }
{begsyncom}      { BEGIN syncomment; ++syncommentdepth; }
<norm>{begsyncom} { BEGIN syncomment; ++syncommentdepth; }
"//"		 { BEGIN linecomment; }
<norm>"//"	 { BEGIN linecomment; }
{MOD}		 { BEGIN norm; return(MODULE);    }

{ws}             { process_buffer(); }

[\r]             {  }

[\n]             {   linenumber++;  }

{comment1}       {   return AT; }

<syncomment>{begsyncom} { ++syncommentdepth; }

<syncomment>(\n) { linenumber++; }
<syncomment>(.)	 { }
<syncomment>{endsyncom} { if(--syncommentdepth == 0) BEGIN vhdlmode; }

{comment}        { }

<<EOF>>          { stack_depth--;
                   if (stack_depth<0) 
                     return 0;
                   else 
                     {
                       delete [] file_scope1;
                       linenumber = linet[stack_depth];
                       file_scope1 = file_scope[stack_depth]; 
                       yy_delete_buffer(YY_CURRENT_BUFFER);
                       yy_switch_to_buffer(nested_file[stack_depth]); 
                     } 
                  }


"\'"             {    process_buffer();
                      return yytext[0];}
"<"              {    process_buffer();
                      return yytext[0];}     
">"              {    process_buffer();
                      return yytext[0];}     
"<="             {    process_buffer();
                      return LE;}     
">="             {    process_buffer();
                      return GE;}     
"=>"             {    process_buffer();
                      return ARROW;}
"**"             {    process_buffer(); return EXPON;}
"<>"             {    process_buffer(); return BOX;}
"="              {    process_buffer();
                      return yytext[0];}                    
"+"              {    process_buffer();
                      return yytext[0];}    
"-"              {    process_buffer();
                      return yytext[0];}
"*"              {    process_buffer();
                      return yytext[0];}   
"/="             {    process_buffer();return NEQ;}
"=="             {    process_buffer();return EQ;} //AMS
"("              {    process_buffer();
                      return yytext[0];}  
")"              {    process_buffer();
                      return yytext[0];}  
"["              {    process_buffer();
                      return yytext[0];}  
"]"              {    process_buffer();
                      return yytext[0];}  
";"              {    process_buffer(); return yytext[0]; }    
":"              {    process_buffer();
                      return yytext[0];}   
":="             {    process_buffer();return ASSIGN;}
","              {    process_buffer();
                      return yytext[0];}   
"."              {    process_buffer();
                      return yytext[0];}
"/"              {    process_buffer();
                      return yytext[0];}
"|"              {    process_buffer();
                      return yytext[0];}
"&"              {    process_buffer();
                      return yytext[0];}
"\\"             {    process_buffer();
                      return yytext[0];}

"include"        {    process_buffer();   return INCLUDE; }
"above"          {    process_buffer();   return ABOVE; }
"abs"            {    process_buffer();   return ABSOLUTE; }
"access"         {    process_buffer();   return ACCESS; }
"across"         {    process_buffer();   return ACROSS; } //AMS
"after"          {    process_buffer();   return AFTER; }
"alias"          {    process_buffer();   return ALIAS; }
"all"            {    process_buffer();   return ALL; }
"and"            {    process_buffer();   return AND; }
"architecture"   {    process_buffer();   return ARCHITECTURE; }
"array"          {    process_buffer();   return ARRAY; }
"assert"         {    process_buffer();   return ASSERT; }
"assign"         {    process_buffer();   return ASSIGNMENT; }
"await"          {    process_buffer();   return AWAIT   ;}
"await_all"      {    process_buffer();   return AWAITALL;}
"await_any"      {    process_buffer();   return AWAITANY;}
"vassign"        {    process_buffer();   return VASSIGNMENT; }
"attribute"      {    process_buffer();   return ATTRIBUTE; }
"begin"          {    process_buffer();   return VBEGIN; }
"block"          {    process_buffer();   return BLOCK; }
"body"           {    process_buffer();   return BODY; }
"break"          {    process_buffer();   return BREAK; } //AMS
"buffer"         {    process_buffer();   return BUFFER; }
"bus"            {    process_buffer();   return BUS; }
"case"           {    process_buffer();   return CASE; }
"component"      {    process_buffer();   return COMPONENT; }
"configuration"  {    process_buffer();   return CONFIGURATION; }
"constant"       {    process_buffer();   return CONSTANT; }
"disconnect"     {    process_buffer();   return DISCONNECT; }
"dot"            {    process_buffer();   return VDOT; } //AMS
"downto"         {    process_buffer();   return DOWNTO; }
"else"           {    process_buffer();   return ELSE; }
"elsif"          {    process_buffer();   return ELSIF; }
"end"            {    process_buffer();   return END; }
"entity"         {    process_buffer();   return ENTITY; }
"exit"           {    process_buffer();   return EXIT; }
"file"           {    process_buffer();   return VFILE; }
"for"            {    process_buffer();   return FOR; }
"function"       {    process_buffer();   return FUNCTION; }
"generate"       {    process_buffer();   return GENERATE; }
"generic"        {    process_buffer();   return GENERIC; }
"guard"          {    process_buffer();   return GUARD; }
"guard_and"      {    process_buffer();   return GUARDAND; }
"guard_or"       {    process_buffer();   return GUARDOR; }
"guarded"        {    process_buffer();   return GUARDED; }
"if"             {    process_buffer();   return IF; }
"impure"         {    process_buffer();   return IMPURE; }
"inf"            {    yylval.intval= INFIN; process_buffer(); return INT; }
"infinity"       {    yylval.intval= INFIN; process_buffer(); return INT; }
"in"             {    process_buffer();   return VIN;}
"initialize"     {    process_buffer();   return INITIALIZE;}
"input"          {    yylval.intval = IN; process_buffer();   return VBOOL; }
"output"         {    yylval.intval = OUT; process_buffer();   return VBOOL; }
"inout"          {    process_buffer();   return INOUT;}
"is"             {    process_buffer();   return IS;}
"label"          {    process_buffer();   return LABEL;}
"library"        {    process_buffer();   return LIBRARY;}
"limit"          {    process_buffer();   return LIMIT; } //AMS
"linkage"        {    process_buffer();   return LINKAGE;}
"loop"           {    process_buffer();   return LOOP;}
"map"            {    process_buffer();   return MAP;}
"mod"            {    process_buffer();   return MOD;}
"nand"           {    process_buffer();   return NAND;}
"nature"         {    process_buffer();   return NATURE; } //AMS
"new"            {    process_buffer();   return NEW;}
"next"           {    process_buffer();   return NEXT;}
"noise"          {    process_buffer();   return NOISE; } //AMS
"nor"            {    process_buffer();   return NOR;}
"not"            {    process_buffer();   return NOT;}
"null"           {    process_buffer();   return VNULL;}
"of"             {    process_buffer();   return OF;}
"on"             {    process_buffer();   return ON;}
"open"           {    process_buffer();   return OPEN;}
"or"             {    process_buffer();   return OR;}
"others"         {    process_buffer();   return OTHERS;}
"out"            {    process_buffer();   return VOUT;}
"package"        {    process_buffer();   return PACKAGE;}
"port"           {    process_buffer();   return PORT;}
"postponed"      {    process_buffer();   return POSTPONED;}
"probe"          {    process_buffer();   return PROBE   ;}
"procedural"     {    process_buffer();   return PROCEDURAL ;} //AMS
"procedure"      {    process_buffer();   return PROCEDURE;}
"process"        {    process_buffer();   return PROCESS;}
"pure"           {    process_buffer();   return PURE; }
"quantity"       {    process_buffer();   return QUANTITY; } //AMS
"range"          {    process_buffer();   return RANGE;}
"receive"        {    process_buffer();   return RECEIVE;}
"record"         {    process_buffer();   return RECORD;}
"reference"      {    process_buffer();   return REFERENCE;} //AMS
"register"       {    process_buffer();   return REGISTER;}
"rem"            {    process_buffer();   return REM;}
"report"         {    process_buffer();   return REPORT;}
"return"         {    process_buffer();   return RETURN;}
"select"         {    process_buffer();   return SELECT;}
"send"           {    process_buffer();   return SEND;}
"severity"       {    process_buffer();   return SEVERITY;}
"shared"         {    process_buffer();   return SHARED;}
"signal"         {    process_buffer();   return SIGNAL;}
"skip"           {    process_buffer();   return SKIP; }
"spectrum"       {    process_buffer();   return SPECTRUM; } //AMS
"subnature"      {    process_buffer();   return SUBNATURE; } //AMS
"subtype"        {    process_buffer();   return SUBTYPE;}
"terminal"       {    process_buffer();   return TERMINAL;} //AMS
"then"           {    process_buffer();   return THEN;}
"through"        {    process_buffer();   return THROUGH;} //AMS
"to"             {    process_buffer();   return TO;}
"tolerance"      {    process_buffer();   return TOLER;} //AMS
"transport"      {    process_buffer();   return TRANSPORT;}
"type"           {    process_buffer();   return TYPE;}
"units"          {    process_buffer();   return UNITS;}
"until"          {    process_buffer();   return UNTIL;}
"use"            {    process_buffer();   return USE; }
"variable"       {    process_buffer();   return VARIABLE;}
"wait"           {    process_buffer();   return WAIT;}
"when"           {    process_buffer();   return WHEN;}
"while"          {    process_buffer();   return WHILE;}
"with"           {    process_buffer();   return WITH;}
"xor"            {    process_buffer();   return XOR;}
"literal"        {    process_buffer();   return LITERAL; }
"group"          {    process_buffer();   return GROUP; }
"reject"         {    process_buffer();   return VREJECT;}
"inertial"       {    process_buffer();   return INERTIAL;}
"unaffected"     {    process_buffer();   return UNAFFECTED;}
"xnor"           {    process_buffer();   return XNOR;}
"sll"            {    process_buffer();   return SLL;}
"srl"            {    process_buffer();   return SRL;}
"sla"            {    process_buffer();   return SLA;}
"sra"            {    process_buffer();   return SRA;}
"rol"            {    process_buffer();   return ROL;}
"ror"            {    process_buffer();   return ROR;}

                 

{real}           {  //Tolower();             
                    yylval.floatval=(double)atof(yytext);
                    process_buffer();   return REAL;
                 } 


{int}            {  //Tolower();           
                    yylval.intval=(int)atoi(yytext);
                    process_buffer();   return INT;
                 }
            

{based_lit}      {  //Tolower();             
                    yylval.basedlit = strtol(yytext,NULL,0);
                    process_buffer();   return BASEDLIT;
                 }

{char_lit}       {  yylval.charlit = Getid();       
                    process_buffer();   return CHARLIT;
                 }

{bit_lit}        {  Tolower();            
                    yylval.bitlit = Getid();
                    process_buffer();   return BITLIT;
                 }
		 
{string_lit}     {  
                    Tolower(); 
                    yylval.strlit= yytext; 
                    process_buffer();  return STRLIT; 
                 }


{id}             {  
                    char* c_str = Getid();
                    string orig = c_str;
                    name_mapping->insert(make_pair(Tolower(), orig));
                    yylval.actionptr= action(c_str, strlen(c_str));
                    process_buffer();   
                    delete[] c_str;
                    return VID; 
                  }
		 

.		 { /* No other rule applies, so it's a bad token */
		    fprintf(stderr,"Bad input char `%s' on line %d\n",
		    yytext,linenumber);
       		  }


<norm>[\r]		{}
<norm>[\n]		{ linenumber++; }
<norm>[ \t]		{}
<norm>{DECINT}          { yylval.intval= (int) atoi(yytext);
                          process_buffer();   return(CINT); }
<norm>{real}            { yylval.floatval= (double) atof(yytext);
                          process_buffer();   return(REAL); }
<norm>{INF}             { yylval.intval=INFIN; 
                          process_buffer(); return(CINT); }
<norm>{SLASH}           { process_buffer();   return(SLASH);  }
<norm>[@?!#/&*=<>,;:|([{}\])\~]	{ process_buffer();   return(yytext[0]); }
<norm>"->"		{ process_buffer();   return(ARROW); }
<norm>"||"		{ process_buffer();   return(PARA); }
<norm>{MOD}		{ process_buffer();   return(MODULE); }
<norm>{ENDMOD}		{ process_buffer();   return(ENDMODULE); }
<norm>{GATE}            { process_buffer();   return(GATE);  }
<norm>{ENDGATE}         { process_buffer();   return(ENDGATE);  }
<norm>{CONSTRAINT}      { process_buffer();   return(CONSTRAINT);  }
<norm>{ENDCONSTRAINT}   { process_buffer();   return(ENDCONSTRAINT);  }
<norm>{ASSUMP}          { process_buffer();   return(ASSUMP);  }
<norm>{ENDASSUMP}       { process_buffer();   return(ENDASSUMP);  }
<norm>{PROC}		{ process_buffer();   return(PROC);  }
<norm>{ENDPROC}		{ process_buffer();   return(ENDPROC);  }
<norm>{ENDTBB}  	{ process_buffer();   return(ENDTB);  }
<norm>"testbench"       { process_buffer();   return TESTBENCH; }
<norm>"end"             { process_buffer();   return END; }
<norm>{DEL}		{ process_buffer();   return(ATACS_DELAY);  }
<norm>{DEF}		{ process_buffer();   return(ATACS_DEFINE);  }
<norm>{PAS}		{ process_buffer();   return(CPORT);  }
<norm>{ACT}		{ process_buffer();   return(CPORT);  }
<norm>{BOOLEAN}		{ process_buffer();   return(BOOLEAN); }
<norm>{INPUT}           { yylval.intval = IN; process_buffer(); return(BOOL); }
<norm>"include"         { process_buffer(); return INCLUDE; }
<norm>"sum"             { process_buffer(); return SUM; }
<norm>"diff"            { process_buffer(); return DIFF; }
<norm>{OUTPUT}          { yylval.intval = OUT; process_buffer(); return(BOOL);}
<norm>{SKIP}		{ process_buffer();   return(SKIP);  }
<norm>{T}               { yylval.boolval = TRUE; 
                          process_buffer(); return(INIT);
                        }

<norm>{F}               { yylval.boolval = FALSE;
                          process_buffer();   
                          return(INIT);  }

<norm>{ID}              { 
                          char *c_str = Getid();
                          string orig = c_str;
                          name_mapping->insert(make_pair(Tolower(), orig));
                          yylval.actionptr = action(yytext, yyleng);    
                          process_buffer();  
                          return(VID); 
                        }

<norm>{ID}"+"           { 
                          Tolower();
                          yylval.actionptr = action(yytext, yyleng);
                          process_buffer();
                          return(ACTION); 
                        }

<norm>"=>"              { process_buffer(); return LARROW; }

<norm>{ID}"-"           { 
                          Tolower(); 
                          yylval.actionptr = action(yytext, yyleng);
                          process_buffer();   return(ACTION); 
                        }

<norm>"-"	        { process_buffer();   return(yytext[0]); }
<comment>(\n)		{ linenumber++; }
<comment>(.)		{}
<linecomment>(\n)       { linenumber++; BEGIN norm;}
<linecomment>(.)        {}
<comment>"/*"		{ //Note: this is always active, because of nesting. 
			   ++commentdepth;
			}
<comment>"*/"		{ if(--commentdepth == 0)  BEGIN norm; }
<norm>.			{ //No other rule applies, so it's a bad token 
		   	  fprintf(stderr,"Bad input char `%c' on line %d\n",
			    yytext[0],linenumber);
       			}


	       
%%

void lexinit() 
{
     stack_depth=0;
     commentdepth=0;
     syncommentdepth=0;
     tempfile= NULL;
     BEGIN INITIAL;

}

   /* This function converts all characters in the input to lower case,so makes
      the lexer case-insensitive. */

char * Tolower()
{

  int i;
  for(i=0;i<yyleng+1;i++)
    yytext[i]=tolower(yytext[i]);

  return yytext;
}


   /* This function appends the '\0' to a identifier's tail.Maybe in future
      this function can be deleted from here. */

char *Getid()
{
  char *str=new char[yyleng+1];
  strncpy(str, yytext, yyleng);
  str[yyleng]='\0';
  yytext = str;
  return(str);
}



void process_buffer()
{   
   if(isENTITY)
     entity_decl<< yytext;

   if(ERROR) {
     YY_FLUSH_BUFFER;
     ERROR= 0;
   }
}


void clean_buffer()
{
  YY_FLUSH_BUFFER;
}


bool switch_buffer(const char *file_name)
{
  if(stack_depth < 10){
    //open compiled file
    FILE *infile= fopen(file_name, "r");
    if(infile == 0){ 
      err_msg("cannot open '", file_name, "'");
      //YY_FLUSH_BUFFER;
      return false;
    }       
       
    //set file scope
    file_scope[stack_depth] = file_scope1;
    nested_file[stack_depth] = YY_CURRENT_BUFFER; 
    linet[stack_depth]= linenumber;

    yyin = infile;         
    
    file_scope1 = copy_string(file_name); 
    linenumber = 1;  
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE)); 

    stack_depth++;
  } 
  return true;
}


int yywrap()
{
    return 1;
}
