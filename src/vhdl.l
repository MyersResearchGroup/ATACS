  /**************************************************************************
   *                       COMPILER--VHDL                                   *
   *                                                                        *
   *                        ---LEXER---                                     *
   **************************************************************************/

%{


#define YY_DECL int yylex (void *lvalp)

#define yylval (*((YYSTYPE*)lvalp))


#include<ctype.h>
#include<string.h>
#include<iostream.h>
#include "aux.h"
#include "vhdl.tab.h"

extern const int MAX_FILE_NEST_DEPTH;

extern int linenumber;   //from driver.c

extern char *file_scope1;

char *file_scope[MAX_FILE_NEST_DEPTH];  //from aux.h


void Tolower();

char *Getid();


void findname(char *);

char *dirname, *filename, *fileattr1;

int linet[10];

FILE *tempfile= NULL;


YY_BUFFER_STATE nested_file[10];

int stack_depth= 0;





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

comment1          \--@

selected_name     {id}\.{id}\.{id}\;

use_clause        "use"{ws}*{selected_name}


%%

{ws}             { }

[\n]             { linenumber++;}


{comment1}       { return AT; }

{comment}        { }

<<EOF>>          { stack_depth--;
                   if( stack_depth<0 ) {
                       yyterminate(); }
                   else {
                       linenumber= linet[stack_depth];
                       file_scope1= file_scope[stack_depth]; 
                       yy_delete_buffer( YY_CURRENT_BUFFER );
                       yy_switch_to_buffer( nested_file[stack_depth] ); }
                  }

"\'"             { return yytext[0];}
"<"              { return yytext[0];}     
">"              { return yytext[0];}     
"<="             { return LE;}     
">="             { return GE;}     
"=>"             { return ARROW;}
"**"             { return EXPON;}
"<>"             { return BOX;}
"="              { return yytext[0];}                    
"+"              { return yytext[0];}    
"-"              { return yytext[0];}
"*"              { return yytext[0];}   
"/="             { return NEQ;}       
"("              { return yytext[0];}  
")"              { return yytext[0];}  
"["              { return yytext[0];}  
"]"              { return yytext[0];}  
";"              { return yytext[0];}    
":"              { return yytext[0];}   
":="             { return ASSIGN;}
","              { return yytext[0];}   
"."              { return yytext[0];}
"/"              { return yytext[0];}
"|"              { return yytext[0];}
"&"              { return yytext[0];}
"\\"              { return yytext[0];}


"abs"            {  return ABS;}
"access"         {  return ACCESS;}
"after"          {  return AFTER;}
"alias"          {  return ALIAS;}
"all"            {  return ALL;}
"and"            {  return AND;}
"architecture"   {  return ARCHITECTURE;}
"array"          {  return ARRAY;}
"assert"         {  return ASSERT;}
"attribute"      {  return ATTRIBUTE;}
"begin"          {  return VBEGIN;}
"block"          {  return BLOCK;}
"body"           {  return BODY;}
"buffer"         {  return BUFFER;}
"bus"            {  return BUS;}
"case"           {  return CASE;}
"component"      {  return COMPONENT;}
"configuration"  {  return CONFIGURATION;}
"constant"       {  return CONSTANT;}
"disconnect"     {  return DISCONNECT;}
"downto"         {  return DOWNTO;}
"else"           {  return ELSE;}
"elsif"          {  return ELSIF;}
"end"            {  return END;}
"entity"         {  return ENTITY;}
"exit"           {  return EXIT;}
"file"           {  return VFILE;}
"for"            {  return FOR;}
"function"       {  return FUNCTION;}
"generate"       {  return GENERATE;}
"generic"        {  return GENERIC;}
"guarded"        {  return GUARDED;}
"if"             {  return IF;}
"impure"         {  return IMPURE; }
"in"             {  return IN;}
"inf"            {  return INF; }
"initialize"     {  return INITIALIZE;}
"input"          {  return INPUT; }
"output"         {  return OUTPUT; }
"inout"          {  return INOUT;}
"is"             {  return IS;}
"label"          {  return LABEL;}
"library"        {  return LIBRARY;}
"linkage"        {  return LINKAGE;}
"loop"           {  return LOOP;}
"map"            {  return MAP;}
"mod"            {  return MOD;}
"nand"           {  return NAND;}
"new"            {  return NEW;}
"next"           {  return NEXT;}
"nor"            {  return NOR;}
"not"            {  return NOT;}
"null"           {  return VNULL;}
"of"             {  return OF;}
"on"             {  return ON;}
"open"           {  return OPEN;}
"or"             {  return OR;}
"others"         {  return OTHERS;}
"out"            {  return OUT;}
"package"        {  return PACKAGE;}
"port"           {  return PORT;}
"postponed"      {  return POSTPONED;}
"procedure"      {  return PROCEDURE;}
"process"        {  return PROCESS;}
"pure"           {  return PURE; }
"range"          {  return RANGE;}
"record"         {  return RECORD;}
"register"       {  return REGISTER;}
"rem"            {  return REM;}
"report"         {  return REPORT;}
"return"         {  return RETURN;}
"select"         {  return SELECT;}
"severity"       {  return SEVERITY;}
"shared"         {  return SHARED; }
"signal"         {  return SIGNAL;}
"subtype"        {  return SUBTYPE;}
"then"           {  return THEN;}
"to"             {  return TO;}
"transport"      {  return TRANSPORT;}
"type"           {  return TYPE;}
"units"          {  return UNITS;}
"until"          {  return UNTIL;}
"use"            {  return USE; }
"variable"       {  return VARIABLE;}
"wait"           {  return WAIT;}
"when"           {  return WHEN;}
"while"          {  return WHILE;}
"with"           {  return WITH;}
"xor"            {  return XOR;}
"literal"        {  return LITERAL; }
"group"          {  return GROUP; }
"reject"         {  return VREJECT;}
"inertial"       {  return INERTIAL;}
"unaffected"     {  return UNAFFECTED;}
"xnor"           {  return XNOR;}
"sll"            {  return SLL;}
"srl"            {  return SRL;}
"sla"            {  return SLA;}
"sra"            {  return SRA;}
"rol"            {  return ROL;}
"ror"            {  return ROR;}

                 

{use_clause}     { if( stack_depth < 10 ) {
                       Tolower();
                       findname(yytext);
		       FILE *temp= fopen(strcat(filename,".vhd"),"r"); 
                       file_scope[stack_depth]= filename;
                       nested_file[stack_depth]= YY_CURRENT_BUFFER;
                       if( !temp ){ 
                           cout<<"can not open file\n"; exit(1); } 
                       yyin= temp; 
                       linet[stack_depth]= linenumber;
                       file_scope[stack_depth]= file_scope1; 
                       file_scope1= filename; 
                       linenumber= 1; 
                       yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE)); 
                       stack_depth++; 
                       return USE_CLAUSE;  }
                   else {
                       cout<<"file nested too deeply\n";
                       exit(1);  }
                 }



{real}           { Tolower();
                   yylval.decimalit = Getid();
                   return REAL;
                 } 


{int}            { Tolower();
                   yylval.decimalit = Getid();
                   return INT;
                 }
            

{based_lit}      { Tolower();
                   yylval.basedlit = Getid();
                   return BASEDLIT;
                 }

{char_lit}       { yylval.charlit = Getid();
                   return CHARLIT;
                 }

{bit_lit}        { Tolower();
                   yylval.bitlit = Getid();
                   return BITLIT;
                 }
		 
{string_lit}     { yylval.strlit = Getid();
                   return STRLIT;
                 }


{id}             {  Tolower();
                    yylval.id = Getid();
                    return ID; }
		 
.                { cout<<"wrong character\n";}

	       
%%


   /* This function converts all characters in the input to lower case,so makes
      the lexer case-insensitive. */

void Tolower(){

          int i;

          for(i=0;i<yyleng+1;i++){
                 yytext[i]=tolower(yytext[i]);
          }
}


   /* This function appends the '\0' to a identifier's tail.Maybe in future
      this function can be deleted from here. */

char *Getid(){
     char *str=new char[yyleng+1];
     strncpy(str, yytext, yyleng);
     str[yyleng]='\0';
     
     return(str);
}



void findname(char *str)
{
     char temp1[128],temp2[128],temp3[128];

     int n=0, m=0;

     while( str[n]!=' ' ) n++;
     
     while( str[n]==' ' ) n++;
     
     while( str[n]!='.' ) { temp1[m]= str[n]; n++; m++; }  temp1[m]='\0'; dirname= temp1; n++; m= 0;  
                
     while( str[n]!='.' ) { temp2[m]= str[n]; n++; m++; }  temp2[m]='\0'; filename= temp2; n++; m= 0; 
     
     while( str[n]!=';' ) { temp3[m]= str[n]; n++; m++; }  temp3[m]='\0'; fileattr1= temp3;
    
     /*     cout<<"dirname is "<<dirname<<endl;
     cout<<"filename is "<<filename<<endl;
     cout<<"fileattr is "<<fileattr1<<endl; */
}








