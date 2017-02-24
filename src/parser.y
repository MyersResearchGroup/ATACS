  /************************************************************************
   *                         COMPILER--VHDL                               *
   *                          ---PARSER---                                *
   ************************************************************************/

%{
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <stack>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>
#include "classes.h"
#include "events.h"
#include "hse.hpp"
#include "makedecl.h"


#define EXPANDED_RATE_NETS 2
    
    char token_buffer_1[512], token_buffer_2[512], token_buffer_3[512] ;

    char distrib1[128];
    char distrib2[128];

    int yylex (void*);

    int yyerror(char *s);

    int yywarning(char *s);

    void constructor();

    void destructor();

    //#define YYDEBUG 1

    extern FILE *lg;  // Log file from atacs.c

    // The following externs are defined in compile.c /////////////////////////
    extern SymTab*     table;
    extern tels_table* tel_tb;
    extern vhd_tel_tb  *open_entity_lst;
    extern csp_tel_tb  *open_module_list;
    extern string      cur_dir;
    extern string      cur_file;
    extern int	       mingatedelay, maxgatedelay;
    extern char*       file_scope1;
    extern int	       linenumber;
    extern char*       outpath;
    extern char*       outputname;
    extern string      cur_entity;
    extern bool	       toTEL;
    extern bool	       compiled;
    extern long        PSC;        // Program state counter
    extern bool        pre_compile;
    extern map<string,string> *name_mapping;

    // End of externs from compile.c //////////////////////////////////////////

    /*************************************************************************/
    //extern char     g_lline[];      /* from lexer */
    //extern char     yytext[];
    //extern int      column;
    /*************************************************************************/

    //This function is called when a file is needed to compile in the
    //current compiling file. It pushes the old buffer into the stack
    //and allocates new buffer for the new file and then set the file
    //pointer to the new file.  From parser.l.  extern bool
    //switch_buffer(const char *dir, const char *file_name);

    //This function cleans the buffer of the lexer. From parser.l.
    extern void clean_buffer();

    map < string, map < string, int > > signals;
    map < string, int                 > processChannels;
    string reqSuffix=SEND_SUFFIX ; // Setting up defaults for send
    string ackSuffix=RECEIVE_SUFFIX ; // Setting up defaults for send
    actionADT join=NULL; // Branches of parallel sends/receives will join here.
    int  direction=SENT;

    stack < pair < string, tels_table* > > parsing_stack;

    //Indicates whether the process statement contains a sensitivity list
    int __sensitivity_list__= 0;
    int isENTITY= 0;
    int isTOP= 0;


    // Create a dummy action.
    actionADT dummyE();

    // Create a new dummy action with name $name
    actionADT make_dummy(char *name);

    // Create a timed event-rule structure for an guard.
    TERstructADT Guard(const string & expression);

    // Create a timed event-rule structure for a CV assignment
    TERstructADT Assign(const string & expression);
    
    // Create a timed event-rule structure for an guarded action.
    TERstructADT Guard(const string & expression, actionADT target,
		       const string& data);

    TERstructADT FourPhase(const string & channel, const string & data="");

    TERstructADT signalAssignment(TYPES* target, TYPES* waveform);

    TYPES * Probe(const string & channel);

    void declare(list<pair<string,int> >* ports);

    //Converts an double to a string
    string numToString(double num);
    string intToString(int num);



    %}

%pure_parser


%union{
  int  				     intval;
  double			     floatval;
  char				     *C_str;
  char				     *id;
  long int			     basedlit;
  char				     *charlit;
  char				     *strlit;
  char				     *bitlit;
  char            		     *stringval;
  bool            		     boolval;
  string                             *str;
  TYPES				     *types;
  actionADT       		     actionptr;
  TERstructADT    		     TERSptr;
  delayADT        		     delayval;
  TYPELIST        		     *typelist;
  pair<string, string>               *csp_pair;
  pair<TYPES,telADT>                 *ty_tel2;
  map<string, string>                *csp_map;
  list<actionADT>                    *action_list;
  list<string>                       *str_list;
  list<TYPES>                        *ty_lst;
  list<pair<string,string> >         *str_str_l;
  list<pair<string,int> >            *str_bl_lst;
  list<pair<TYPES,telADT> >          *ty_tel_lst;
  list<pair<TYPES,TYPES> >           *ty_ty_lst;
  pair<string,list<string> >         *stmt_pair;
  list<pair<string,list<string> > >  *simul_lst;
};




%token ABSOLUTE ACCESS AFTER ALIAS ALL AND ARCHITECTURE ARRAY ASSERT ATTRIBUTE
%token VBEGIN BLOCK BODY BUFFER BUS CASE COMPONENT CONFIGURATION CONSTANT
%token DISCONNECT DOWNTO ELSE ELSIF END ENTITY EXIT VFILE FOR
%token FUNCTION PROCEDURE  NATURAL
%token GENERATE GENERIC GUARDED IF IMPURE VIN INITIALIZE INOUT IS LABEL LIBRARY
%token LINKAGE LITERAL LOOP MAP MOD NAND NEW NEXT NOR NOT UNAFFECTED
%token UNITS GROUP OF ON OPEN OR OTHERS VOUT VREJECT INERTIAL XNOR
%token PACKAGE PORT POSTPONED PROCESS PURE RANGE RECORD REGISTER REM
%token REPORT RETURN SELECT SEVERITY SHARED SIGNAL SUBTYPE THEN TO TRANSPORT
%token TYPE UNTIL USE VARIABLE VNULL WAIT WHEN WHILE WITH XOR
%token ARROW EXPON ASSIGN BOX
%token SLL SRL SLA SRA ROL ROR AT INF EOC ASSIGNMENT
%token PROBE RECEIVE SEND VASSIGNMENT
%token INITCHANNEL

/* VHDL-AMS Additions */
%token ACROSS BREAK EQ LIMIT NATURE NOISE PROCEDURAL QUANTITY REFERENCE 
%token SPECTRUM SUBNATURE TERMINAL THROUGH TOLER VDOT ABOVE
/* End Additions */

%token MODULE ENDMODULE BOOL PARA ATACS_DELAY ENDPROC SKIP INIT DEL BOOLEAN
%token SLASH CID CPORT  CINT CARROW PROC RECURSIVE_ERROR GATE ENDGATE
%token GUARD GUARDAND GUARDOR GUARDTO LARROW INCLUDE ENDTB TESTBENCH
%token AWAIT AWAITALL AWAITANY ATACS_DEFINE
%token CONSTRAINT ENDCONSTRAINT ASSUMP ENDASSUMP SUM DIFF
%token <strlit>STRLIT BITLIT
%token <intval> INT  VBOOL
%token <basedlit> BASEDLIT
%token <floatval> REAL 
%token <actionptr> VID ACTION
%token <charlit> CHARLIT

/* VHDL-AMS Additions */
%type <C_str> concurrent_break_statement
%type <C_str> procedural_head in_out_symbol
%type <TERSptr> break_statement 
 //%type <C_str> neg
%type <types> terminal_aspect subnature_indication scalar_nature_definition
%type <types> procedural_declarative_item array_nature_definition
%type <types> record_nature_definition nature_definition
%type <types> andor_ams
%type <simul_lst> simul_statement
%type <simul_lst> simul_elsifthen sequence_of_simul_statements
%type <simul_lst> simul_case_statement_alternative simul_else_part
%type <simul_lst> simultaneous_if_statement simultaneous_case_statement
%type <simul_lst> simple_simultaneous_statement

%type <str_list> across_aspect through_aspect 
%type <str_list> record_nature_alternative
%type <str_bl_lst> interface_terminal_declaration
%type <str_bl_lst> interface_quantity_declaration
%type <TERSptr> simultaneous_procedural_statement
%type <TERSptr> simultaneous_statement rate_assignment
%type <TERSptr>  when_symbol 
%type <str> break_element break_elements

/* End Additions */

%type <C_str> alias_name signal_mode block_label designator mode
%type <C_str> idparan suffix relational_operator direction sign
%type <C_str> multiplying_operator adding_operator
%type <C_str> enumeration_literal label label_symbol
%type <C_str> package_tail endofentity architail
%type <C_str> entity_descr package_body_head comp_decl_tail process_head
%type <types> type_definition scalar_type_definition composite_type_definition
%type <types> access_type_definition    file_type_definition
%type <types> integer_floating_type_definition
%type <types> array_type_definition      record_type_definition
%type <types> unconstrainted_array_definition  constrained_array_definition
%type <types> subtype_indication
%type <types> subtype_indication1 subtype_indication_list
%type <types>  enumeration_type_definition
%type <types> allocator  secondary_unit_declaration
%type <types> element_association base_unit_declaration
%type <types> choice  qualified_expression
%type <types> elarep   aggregate  sub_body_head discrete_range
%type <types> xorexpr  literal  physical_literal_no_default
%type <typelist> range_constraint  range
%type <typelist> gen_association_list  gen_association_list_1
%type <typelist> ee physical_literal
%type <typelist> sud physical_type_definition discrete_range1
%type <typelist> gen_association_element
%type <typelist> index_constraint disdis isd index_subtype_definition
%type <types> subprogram_specification  subprogram_declaration
%type <types> attribute_name al_decl_head  assign_exp
%type <types> waveform_element  waveform cmpnt_stmt_body
%type <types> expression  relation  target probedChannel shift_expression
%type <types> simple_expression terms  term factor
%type <types> primary   orexpr andexpr
%type <types> insl_name  name3  name2
%type <types> name condition_clause iteration_scheme //condition
%type <types> andor_stmt and_stmts or_stmts target_factors target_terms
%type <str_list> mark selected_name
%type <TERSptr> block_statement  block_statement_part
%type <TERSptr> loop_statement concurrent_statement
%type <TERSptr> process_statement
%type <TERSptr>  process_statement_part
%type <TERSptr> wait_statement else_part conditional_signal_assignment
%type <TERSptr> timeout_clause 
%type <TERSptr> concurrent_signal_assignment_statement
%type <TERSptr> if_statement case_statement
%type <TERSptr> signal_assignment_statement variable_assignment_statement
%type <TERSptr> assign_statements assign_statement assign_stmt
%type <TERSptr> vassign_statements vassign_statement vassign_stmt
%type <TERSptr> communicationStatement parallelCommunication
%type <TERSptr> fourPhase guardish_statement assertion_statement
%type <TERSptr> guard_statement guard_and_statement guard_or_statement
%type <TERSptr> await_statement await_all_statement await_any_statement
%type <str_bl_lst> interface_list interface_declaration  port_clause
%type <str_bl_lst> interface_signal_declaration
%type <str_bl_lst> interface_unknown_declaration component_declaration_tail
%type <str_list> identifier_list
%type <str_str_l> association_list
%type <str_str_l> port_map_aspect generic_map_aspect map_aspects
%type <csp_pair> association_element
%type <str> formal_part
%type <types> actual_part
%type <ty_tel_lst> case_statement_alternative elsifthen sequence_of_statements
%type <ty_ty_lst> waveform_head conditional_waveforms selected_waveforms
%type <ty_lst> choices
%type <ty_tel2> sequential_statement

%type <TERSptr> process body action skip or_constraint and_constraint
%type <TERSptr> guardstruct guardcmdset guardcmd leakyexprs leakyexpr
%type <TERSptr> expr conjunct csp_literal actionexpr prs constraint
%type <csp_pair> formal_2_actual
%type <csp_map> port_map_list
%type <id> file_name
%type <str> leveliteral levelexpr levelconjunct path_name filename
%type <delayval> delay
%type <intval> CINT BOOL CPORT
%type <boolval> INIT
%type <action_list> id_list
%type <str_bl_lst> booldecl decl decls delaydecl constructs component_instances
%type <str_bl_lst> definedecl
%type <floatval> sing_dist

%nonassoc   <actionptr> ID

%nonassoc '>' '<' '=' GE LE NEQ
%left '+' '-' '&'  '|'  PARA
%left MED_PRECEDENCE  ';' ':'
%left '*' '/' MOD REM ARROW
%nonassoc EXPON ABSOLUTE NOT MAX_PRECEDENCE

%start start_point



%%



/* Design File */

start_point    : {
                   constructor();
                 }
                 design_file
                 {
		   destructor();
		 }
               | csp_compiler { destructor(); }
               | recursive_errors { return 1; }
               ;
design_file
               : design_unit {}
               | design_file design_unit { }
               ;
design_unit
               : context_clause library_unit  { }
               ;
context_clause
               :
               | context_clause context_item
               ;
context_item
               : library_clause
               | use_clause
                 {
		   //string f_name = string(".");
		 //for(list<string>::iterator b=$1->begin(); b!=$1->end(); b++)
		   //   f_name += ('/' + *b);
		   //f_name += ".vhd";		
		   //if(switch_buffer(f_name) == false){
		   //destructor();
		   //yy_abort();
		   //return 1;
		   //}
		 }
               ;
library_unit
               : primary_unit    {}
               | secondary_unit  {}
               ;
primary_unit
               : entity_declaration
               | configuration_declaration
               | package_declaration
               ;
secondary_unit
               : architecture_body  {}
               | package_body    { }
               ;
library_clause
               : library_list ';'
               ;
library_list
               : LIBRARY VID
                 { }
               | library_list ',' VID
                 {}
               ;



/* Library Unit Declarations */

entity_declaration
             : ENTITY VID
               {  		
		 new_action_table($2->label);
		 new_event_table($2->label);
		 new_rule_table($2->label);
		 new_conflict_table($2->label);
		 delete tel_tb;
		 tel_tb = new tels_table;
		 tel_tb->set_id($2->label);
		 tel_tb->set_type($2->label);
		 table->new_design_unit($2->label);
		 cur_entity = $2->label;
	       }			
               entity_descr
               {
		 if(open_entity_lst->insert($2->label,*tel_tb)==false)
		   err_msg("error: multiple declaration of entity '",
			   $2->label, "'");
		 		
		 if($4 && !strcmp_nocase($2->label,$4))
		   err_msg("warning: '", $4, "' undeclared as an entity name");
		 // delete tel_tb;
		 //tel_tb=0;
	       }
	       ;
entity_descr
               : IS entity_header entity_declarative_part END endofentity ';'
                 { $$ = $5; }
	       | IS entity_header entity_declarative_part
                 VBEGIN  entity_statement_part END endofentity ';'
                 { $$ = $7; }
               ;
endofentity
               : { $$ = 0; }
               | ENTITY { $$ = 0; }
               | VID { $$ = $1->label; }
               | ENTITY VID { $$ = $2->label; }
               ;
entity_header
               :
               | generic_clause
               | port_clause {declare($1);}
               | generic_clause port_clause {declare($2);}
               ;

generic_clause
               : GENERIC '(' interface_list ')' ';'
               ;
port_clause
               : PORT '(' interface_list ')' ';'
                 { $$= $3; }
               ;
entity_declarative_part
               :
               | entity_declarative_part entity_declarative_item
               ;
entity_declarative_item
               : subprogram_declaration {}
               | subprogram_body {}
               | type_declaration
               | subtype_declaration
               | constant_declaration
               | signal_declaration
               | file_declaration
               | alias_declaration
               | attribute_declaration
               | label_declaration /* Unknown */
               | attribute_specification
               | initialization_specification /* Unknown */
               | disconnection_specification
               | use_clause {}
               | variable_declaration /* Unknown */
               | group_template_declaration
               | group_declaration
	       /* Added for AMS */
	       | step_limit_specification
	       | nature_declaration
	       | subnature_declaration
	       | quantity_declaration
	       | terminal_declaration
	       
               ;
group_template_declaration
               : GROUP VID IS '(' gtp_body ')' ';'
               ;
gtp_body       : gtp_body ',' entity_class box_symbol
               | entity_class box_symbol
               ;
box_symbol     :
               | BOX
               ;
group_declaration
               : GROUP VID ':' VID '(' gd_body ')' ';'
               ;
gd_body        : gd_body ',' VID
               | gd_body ',' CHARLIT
               | VID {}
               | CHARLIT {}
               ;
entity_statement_part
               :
               | entity_statement_part entity_statement
               ;
entity_statement
               : concurrent_assertion_statement
               | concurrent_procedure_call
               | process_statement {}
               ;
architecture_body
               : ARCHITECTURE VID OF VID IS
                 {
		   signals[cur_entity].clear();
		   delete tel_tb;
		   const tels_table& TT=open_entity_lst->find(Tolower($4->label));
		   if(TT.empty())
		     err_msg("entity '",$4->label,"' undeclared");
		   tel_tb = new tels_table(TT);
		   tel_tb->set_type(Tolower($4->label));

		   if(table->design_unit(Tolower($4->label)) == false){
		     err_msg("entity '",$4->label,"' undeclared");
		   }
		   table->new_tb();
                 }
                 architecture_declarative_part
                 VBEGIN architecture_statement_part END architail ';'
                 {
		   if($11 && strcmp_nocase($11, $2->label) == false)
		     err_msg("warning: '", $2->label,
			     "' undeclared as an architecture name");

		   if(open_entity_lst->insert(Tolower($4->label),
					      Tolower($2->label),tel_tb)==2)
		     err_msg("error: multiple declaration of architecture '",
			     $2->label, "'");
		   table->delete_tb();	
		 }
               ;
architail
              : { $$ = 0; }
              | ARCHITECTURE VID {   $$ = $2->label; }
              | ARCHITECTURE { $$ = 0; }
              | VID { $$ = $1->label; }
              ;
architecture_declarative_part
              :
              | architecture_declarative_part block_declarative_item
              ;
architecture_statement_part
              :
              | architecture_statement_part concurrent_statement
              /* Added for AMS */
              | architecture_statement_part simultaneous_statement
              ;
configuration_declaration
               : CONFIGURATION VID OF VID config_tail
               ;
config_tail
               : /*IS  configuration_declarative_part block_configuration
                 END configtail VID ';'
		 |*/ IS  configuration_declarative_part block_configuration
                 END configtail ';'
               ;
configtail
               :
               | CONFIGURATION
               | VID {}
               | CONFIGURATION VID {}
               ;
configuration_declarative_item
               : use_clause {}
               | attribute_specification
               | group_declaration
               ;

configuration_declarative_part
               :
               | configuration_declarative_part configuration_declarative_item
               ;
block_configuration
              : FOR block_specification us ci END FOR ';'
              ;

block_specification
              : VID { }
              | VID '(' www ')' {}
              ;
www
              : discrete_range {}
              | INT {}
              | REAL {}
              ;
us
              :
              | us use_clause {}
              ;
ci
              :
              | ci configuration_item
              ;
configuration_item
              : block_configuration
              | component_configuration
              ;
component_configuration
               : FOR component_specification END FOR ';'
               | FOR component_specification binding_indication ';'
                 END FOR ';'
               | FOR component_specification block_configuration END FOR ';'
               | FOR component_specification binding_indication ';'
                 block_configuration END FOR ';'
               ;
package_declaration
               : PACKAGE VID
                 { table->new_design_unit($2->label); }
                 package_tail
                 {
		   if($4 && strcmp_nocase($2->label, $4)==false)
		     err_msg("warning: '", $2->label,
			     "' undeclared as a package label");
		 }
               ;
package_tail
               : IS package_declarative_part END packagetail ';'{ $$ = 0; }
               | IS package_declarative_part
                 END packagetail VID ';' { $$ = $5->label; }
               ;
packagetail
               :
               | PACKAGE
               ;
package_declarative_part
               :
               | package_declarative_part package_declarative_item
               ;
package_declarative_item
               : subprogram_declaration {}
               | type_declaration
               | subtype_declaration
               | constant_declaration
               | signal_declaration
               | file_declaration
               | alias_declaration
               | component_declaration
               | attribute_declaration
               | attribute_specification
               | initialization_specification
               | disconnection_specification
               | use_clause {}
               | variable_declaration
               | group_template_declaration
               | group_declaration
	       /* Added for AMS */
	       | nature_declaration
	       | subnature_declaration
	       | terminal_declaration
               ;
package_body_head
               : PACKAGE BODY VID
                 {
		   /*const char *t = table->find($3->label);
		   if(t == 0 || strcmp_nocase(t, "package"))
		     err_msg("warning: '", $3->label,
			     "' undeclared as a package name");

			     $$ = $3->label;*/
		 }
               ;
package_body
               : package_body_head
                 IS package_body_declarative_part END pac_body_tail ';'
                 {}
               | package_body_head
                 IS package_body_declarative_part END pac_body_tail VID ';'
                 {
		   /*if(strcmp_nocase($1, $6->label))
		     err_msg("warning: '", $6->label,
		     "' is not declared as a package body name");*/
		 }
               ;
pac_body_tail
               :
               | PACKAGE BODY
               ;
package_body_declarative_part
               :
               | package_body_declarative_part package_body_declarative_item
               ;
package_body_declarative_item
               : subprogram_declaration {}
               | subprogram_body
               | type_declaration
               | subtype_declaration
               | constant_declaration
               | file_declaration
               | alias_declaration
               | use_clause {}
               | variable_declaration
               | group_template_declaration
               | group_declaration
               ;



/* Declarations and Specifications */

subprogram_declaration
               : subprogram_specification  ';'
                 {
		   /*if(strcmp_nocase($1->obj_type(), "fct") == 0 &&
		      $1->data_type() == 0)
		     {
		       err_msg("undefined the return type of function '",
			       $1->get_string(), "'");
		       $1->set_data("err");
		     }
		     table->insert($1->get_string(), $1);*/
		 }
               ;
subprogram_specification
               : PROCEDURE designator
                 {
		   /*$$ = new TYPES("prd", 0);
		     $$->set_string($2);*/
		 }
               | PROCEDURE designator '(' interface_list ')'
                 {/*
		   $$ = new TYPES("prd", 0);
		   //$$->set_string($2);
		   //$$->set_list($4);
		  */
		 }
               | func_head FUNCTION designator RETURN mark
                 {
		   /*TYPES *mt = table->lookup($5);
		   if(mt == 0)
		     $$ = new TYPES("fct", 0);
		   else	
		     {
		       $$ = new TYPES1(mt->get_ters(), mt);
		       $$->set_obj("fct");
		     }
		   $$->set_string($3);
		   delete mt;*/
		 }
               | func_head FUNCTION designator '(' interface_list ')'
	         RETURN mark
                 {
		   /*TYPES *mt = table->lookup($8->front());
		   if(mt == 0)
		     $$ = new TYPES("fct", 0);
		   else
		     {
		       $$ = new TYPES1(mt->get_ters(), mt);
		       $$->set_obj("fct");
		     }
		   $$->set_string($3);
		   //$$->set_list($5);
		   delete mt;*/
		 }
                 ;
func_head
               :
               | PURE
               | IMPURE
               ;
sub_body_head
               : subprogram_specification IS
                 {
		   /*$$ = $1;
		   table->new_scope($$->get_string(), "function");
		   TYPELIST *l = $1->get_list();
		   l->iteration_init();
		   while(l->end() == false)
		     {	
		       table->insert(l->value().first, l->value().second);
		       l->next();	
		       }*/
		 }
               ;
subprogram_body
               : sub_body_head subprogram_declarative_part
                 VBEGIN subprogram_statement_part END subprog_tail ';'
                 {
		   //table->delete_scope();
		 }
               | sub_body_head subprogram_declarative_part VBEGIN
		 subprogram_statement_part END subprog_tail designator ';'
                 {
		   //table->delete_scope();
		 }		
               ;
subprog_tail
               :
               | PROCEDURE
               | FUNCTION
               ;
subprogram_statement_part
               : {}
               | subprogram_statement_part sequential_statement
                 {}
               ;
designator
               : VID     { $$= $1->label; }
               | STRLIT  { $$= $1; }
               ;
subprogram_declarative_part
               :
               | subprogram_declarative_part subprogram_declarative_item
               ;
subprogram_declarative_item
               : subprogram_declaration {}
               | subprogram_body
               | type_declaration
               | subtype_declaration
               | constant_declaration
               | variable_declaration
               | file_declaration
               | alias_declaration
               | attribute_declaration
               | label_declaration
               | attribute_specification
               | use_clause {}
               | group_template_declaration
               | group_declaration
               ;
type_declaration
               : full_type_declaration
               | incomplete_type_declaration
               ;
full_type_declaration
               : TYPE VID IS type_definition ';'
                 {
		   /*if(strcmp_nocase($4->obj_type(), "uk") == 0)
		     $4->set_obj("typ");

		   $4->set_data($2->label);
		   table->insert($2->label, $4);
		   if($4->get_grp() == TYPES::Physical)
		     {
		       TYPELIST *l = $4->get_list();
		       TYPES *ty = new TYPES("uk", $2->label);
		       l->iteration_init();
		       l->next();
		       l->next();		
		       while(l->end() == false)
			 {
			   const TYPES *tmp = l->value().second;
			   table->insert(tmp->get_string(), ty);
			   l->next();
			 }	
		       delete ty;
		       }*/
		 }
               ;
incomplete_type_declaration
               : TYPE VID ';'
                 {
		   /*TYPES *t = new TYPES("typ", $2->label);
		   table->insert($2->label, t);
		   delete t;*/
		 }
               ;
type_definition
               : scalar_type_definition     { $$= $1; }
               | composite_type_definition  { $$= $1; }
               | access_type_definition     { $$= $1; }
               | file_type_definition       { $$= $1; }
               ;
scalar_type_definition
               : enumeration_type_definition
                 {
		   //$$ = $1;
		 }
               | integer_floating_type_definition  { $$= $1; }
               | physical_type_definition
                 {
		   /*$$ = new TYPES;
		   $$->set_grp(TYPES::Physical);
		   $$->set_list($1);*/
		 }
               ;
composite_type_definition
               : array_type_definition   { $$= $1; }
               | record_type_definition  { $$= $1; }
               ;

/* Added for AMS */
nature_declaration
               : NATURE VID IS nature_definition ';'
               ;

nature_definition
               : scalar_nature_definition
               | array_nature_definition
               | record_nature_definition
               ;
/* End Additions */

constant_declaration
               : CONSTANT identifier_list ':' subtype_indication ';'
                 {}
               | CONSTANT identifier_list ':' subtype_indication
                 ASSIGN expression ';'
                 {
		   for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     string LL = Tolower(*b);
		     makeconstantdecl(action(LL), $6->get_int());
		   }
		 }
               ;
identifier_list
               : VID
                 { $$ = new list<string>;  $$->push_back($1->label); }
               | identifier_list ',' VID
                 { $$ = $1; $$->push_back($3->label); }
               ;
signal_declaration
               : SIGNAL identifier_list ':' subtype_indication signal_kind
                 assign_exp ';' signal_mode
                 {
		   list<string> *sufficies = new list<string>;
		   int type(IN);
		   string assigned($6->get_string());
		   delayADT myBounds;
		   int rise_min(0), rise_max(0), fall_min(-1), fall_max(-1);
		   if("init_channel"==assigned){
		     sufficies->push_back(SEND_SUFFIX);
		     sufficies->push_back(RECEIVE_SUFFIX);
		     TYPELIST arguments($6->get_list());
		     TYPELIST::iterator i(arguments.begin());
		     while(i!=arguments.end()){
		       if("timing"==i->second.get_string()){
			 TYPELIST bounds(i->second.get_list());
			 TYPELIST::iterator j(bounds.begin());
			 int pos(1);
			 while(j != bounds.end()){
			   string formal(j->first);
			   int actual(j->second.get_int());
			   if("rise_min"==formal || "lower"==formal ||
			      formal.empty() && 1==pos){
			     rise_min = actual;
			   }
			   else if("rise_max"==formal || "upper"==formal ||
				   formal.empty() && 2==pos){
			     rise_max = actual;
			   }
			   else if("fall_min"==formal || "lower"==formal ||
				   formal.empty() && 3==pos){
			     fall_min = actual;
			   }
			   else if("fall_max"==formal || "upper"==formal ||
				   formal.empty() && 4==pos){
			     fall_max = actual;
			   }
			   j++;
			   pos++;
			 }
		       }
		       i++;
		     }
		   }
		   else if("active"==assigned || "passive"==assigned){
		     err_msg(assigned,
			     " doesn't make sense in signal declaration");
		   }
		   else{
		     sufficies->push_back("" );
		     if($8 == string("out")){
		       type=OUT|ISIGNAL;
		     }
		   }
		   if(fall_min<0){
		     fall_min = rise_min;
		   }
		   if(fall_max<0){
		     fall_max = rise_max;
		   }
		   assigndelays(&myBounds,
				rise_min,rise_max,NULL,
				fall_min,fall_max,NULL);
		   int init = 0;
		   if($6->get_string() == "'1'"){
		     init = 1;
		   }
		   for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     list<string>::iterator suffix=sufficies->begin();
		     signals[cur_entity][Tolower(*b)] = type;
		     while ( suffix != sufficies->end() ) {
		       string LL = Tolower(*b) + *suffix;
		       makebooldecl(type, action(LL), init, &myBounds);
		       if(!table->insert(LL, make_pair((string)$8,(TYPES*)0))){
			 err_msg("multiple declaration of signal '",*b,"'");
		       }
		       suffix++;
		     }
		     //              if(type==OUT)
		     tel_tb->add_signals(*b, type);
		       //else
		       //tel_tb->add_signals(*b, IN);
		   }
		   delete sufficies;
		 }
               ;
signal_kind
               :
               | REGISTER | BUS
               ;
signal_mode
               : { $$= "out"; }
               | AT VIN  { $$= "in"; }
               | AT VOUT { $$= "out"; }
               ;
variable_declaration
               : var_decl_head VARIABLE identifier_list ':'
                 subtype_indication ';'
                 {
		   for(list<string>::iterator b=$3->begin(); b!=$3->end();b++){
		     if(!table->insert(*b,make_pair((string)"var",(TYPES*)0))){
		       err_msg("multiple declaration of variable '",*b,"'");
		     }
		   }
		   delete $3;
		 }
               | var_decl_head VARIABLE identifier_list ':' subtype_indication
	         ASSIGN expression ';'
                 {
		   for(list<string>::iterator b=$3->begin(); b!=$3->end();b++){
		     if(!table->insert(*b,make_pair((string)"var",(TYPES*)0))){
		       err_msg("multiple declaration of variable '",*b,"'");
		     }
		   }
		   delete $3;
		   delete $7;
		 }
               ;
var_decl_head
               :
               | SHARED
               ;

/* Added for AMS */
terminal_declaration :
                 TERMINAL identifier_list ':' subnature_indication ';'
               ;



quantity_declaration :
                 QUANTITY identifier_list ':' subtype_indication
                 assign_exp ';'
                 {
		   for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   TERstructADT x;
		   actionADT a = action(*($2->begin()));
		   a->type = CONT + VAR + DUMMY;
		   /* TODO: Support span */
		   a->linitval = atoi($5->theExpression.c_str());
		   a->uinitval = atoi($5->theExpression.c_str());
		   x = TERS(a,FALSE,0,0,FALSE);
		   tel_tb->insert(*($2->begin()), x);

		   if (EXPANDED_RATE_NETS==0) {
		     TERstructADT xdot;
		     string dollar2 = (*($2->begin()));
		     actionADT adot = action((dollar2 + "dot").c_str());
		     adot->type = CONT + VAR + DUMMY;
		     adot->linitval = 0;//atoi($5->theExpression.c_str());
		     adot->uinitval = 0;//atoi($5->theExpression.c_str());
		     xdot = TERS(adot,FALSE,0,0,FALSE);
		     tel_tb->insert((dollar2 + "dot").c_str(), xdot);
		   }
		   
		   delete $2;
		 }
               | QUANTITY terminal_aspect ';'
                 {
		 /*for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete $2;*/
		 }
               | QUANTITY across_aspect terminal_aspect ';'
	         {
		   for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete $2;
		 }
               | QUANTITY through_aspect terminal_aspect ';'
	         {
		   for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete $2;
		 }
               | QUANTITY across_aspect through_aspect terminal_aspect ';'
	         {
		   for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete $2;
		 }
               | QUANTITY identifier_list ':' subtype_indication
                 source_aspect ';'
	         {
		   for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete $2;
		 }
               ;

across_aspect :  identifier_list tolerances assign_exp ACROSS {$$ = $1;}
               ;

tolerances :     TOLER expression
               |
               ;

through_aspect :
               identifier_list tolerances assign_exp THROUGH {$$ = $1;}
               ;

terminal_aspect :
                 name 
               | name TO name
               ;

source_aspect :  SPECTRUM simple_expression ',' simple_expression
               | NOISE simple_expression
               ;

/* End Additions */

file_declaration
               : VFILE identifier_list ':' subtype_indication ';'
                 {}
               | VFILE identifier_list ':' subtype_indication
                 {}
                 external_file_association ';'
               ;
file_access_mode
               :
               | OPEN expression
               ;
external_file_association
               : file_access_mode IS file_logical_name
               | file_access_mode IS mode file_logical_name
               ;
file_logical_name
               : name   {}       //expression revised on July/1/1996.
               ;
alias_declaration
               : ALIAS alias_name al_decl_head IS name al_decl_tail ';'
                 {}//   TYPES *t= search( table, $5->name2->label );
		     //table->add_symbol($2, $3);  }
               ;
alias_name
               : VID       { $$= $1->label; }
               | CHARLIT   { $$= $1; }
               | STRLIT    { $$= $1; }
               ;
al_decl_head
               : {}//   $$= new TYPES( "unknown", "unknown", "unknown" ); }
               | ':' sub_indications  {}//  $$= $2;  }
               ;

al_decl_tail   : signature
               |
               ;
signature_symbol
               :
               | signature
               ;
signature
               : '[' signature_body  signature_tail ']'
               | '[' signature_tail ']'
               ;
signature_body
               : name
                 {}//   TYPES *t= search(table, $1->name2->label );
		   //  delete t;
		   //}
               | signature_body ',' name
                 {}
                 /* TYPES *t= search(table,$3->name2->label );
		   delete t;
		 }*/
               ;
signature_tail
               :
               | RETURN name
               ;
recursive_units
               :
               | design_file
               ;
component_declaration
               : COMPONENT VID
                 {
		   parsing_stack.push(make_pair(cur_entity, tel_tb));
		   cur_entity = $2->label;
		   tel_tb = 0;
		   table->push();
		   if(open_entity_lst->find($2->label).empty()){
		     //cout << $2->label<< "   " << (*name_mapping)[$2->label] << endl;
		     if(switch_buffer((*name_mapping)[$2->label] + ".vhd")){
		       new_action_table(cur_entity.c_str());
		       new_event_table(cur_entity.c_str());
		       new_rule_table(cur_entity.c_str());
		       new_conflict_table(cur_entity.c_str());
		     }
		   }
		 }
                 recursive_units
                 {
		   cur_entity = parsing_stack.top().first;
		   tel_tb = parsing_stack.top().second;
		   parsing_stack.pop();
		   table->pop();
		   new_action_table(cur_entity.c_str());
		   new_event_table(cur_entity.c_str());
		   new_rule_table(cur_entity.c_str());
		   new_conflict_table(cur_entity.c_str());
		 }		
                 component_declaration_tail comp_decl_tail
                 {
		   if(open_entity_lst->find(Tolower($2->label)).empty())
		     err_msg("error: component ",$2->label," not found");
		
		   if($7 && strcmp_nocase($2->label, $7)==false)
		     err_msg("warning: '", $7,
			     "' not declared as a component name");
		   if ($6) delete $6;
		 }		
               ;
component_declaration_tail
               : comp_decl_head { $$ = 0; }
               | comp_decl_head generic_clause { $$ = 0; }
               | comp_decl_head port_clause
                 { $$ = $2; }
               | comp_decl_head generic_clause port_clause
	         { $$ = $3; }
               ;
comp_decl_head
               :
               | IS
               ;
comp_decl_tail
               : END COMPONENT ';'
                 { $$ = 0; }
               | END COMPONENT VID ';'
                 { $$ = $3->label; }
               ;
attribute_declaration
              : ATTRIBUTE VID ':' VID ';'
                {
		  yyerror("Attributes not supported.");
		  /*
		  TYPES *t= search( table, $4->label );
		  TYPES *temp= new TYPES( "attribute", t->datatype() );
		  table->add_symbol( $2->label,temp );
		  delete t,temp;
		  */
		}
              ;
attribute_specification
              : ATTRIBUTE VID
                {
		  yyerror("Attributes not supported.");
		  //Type *t= search( table, $2->label );
		    //delete t;
		  }
                OF entity_specification IS expression  ';'
              ;
entity_specification
               : entity_name_list ':' entity_class
               ;
entity_name_list
               : entity_name_sequence
               | OTHERS
               | ALL
               ;
entity_name_sequence
               : entity_designator signature_symbol
               | entity_name_sequence ',' entity_designator signature_symbol
               ;
entity_designator
               : VID
                 {   //TYPES *t= search( table, $1->label );
		     //delete t;
                 }
               | STRLIT
                 {}
               | CHARLIT
                 {}
               ;
entity_class
               : ENTITY | ARCHITECTURE | CONFIGURATION | PROCEDURE | FUNCTION
               | PACKAGE | TYPE | SUBTYPE | CONSTANT | SIGNAL | VARIABLE
               | COMPONENT | LABEL | LITERAL | UNITS | GROUP | VFILE
               | NATURE | SUBNATURE | QUANTITY | TERMINAL /* Added for AMS */
               ;
configuration_specification
               : FOR component_specification binding_indication ';'
               ;
component_specification
               : instantiation_list ':' name
                 { }
               ;
instantiation_list
               : component_list
               | OTHERS
               | ALL
               ;
component_list
               : VID
                 {   //TYPES *t= search( table, $1->label );
		     //delete t;
		   }
               | component_list ',' VID
                 {   //TYPES *t= search( table, $3->label );
		     //delete t;
		   }
               ;
binding_indication
               :
               | USE entity_aspect
               | USE entity_aspect map_aspects
               | map_aspects {}
               ;
entity_aspect
               : ENTITY name
                 {}//   search( table, $2->name2->label );   }
               | CONFIGURATION name
                 {  /* FILE *temp= NULL;//fopen(strcat($2, ".sim"),"r");
		     if( temp )
		         printf("cannot find the configuration");*/
		   }
	       | OPEN
               ;

port_map_aspect: PORT MAP  '(' association_list ')'  { $$= $4; } ;
generic_map_aspect: GENERIC MAP  '(' association_list ')'  { $$= $4; } ;

map_aspects
               : generic_map_aspect
               | port_map_aspect
               | generic_map_aspect port_map_aspect {$$ = $2;}
               ;

disconnection_specification
               : DISCONNECT guarded_signal_specification AFTER expression ';'
               ;
guarded_signal_specification
               : signal_list ':' VID
                 {   //TYPES *t= search( table, $3->label );
		     //delete t;
		   }
               ;
signal_list
               : signal_id_list
               | OTHERS
               | ALL
               ;
signal_id_list
               : VID
                 {}// table->add_symbol( $1->label, "unknown", "unknown" ); }
               | signal_list ',' VID
                 {}//table->add_symbol( $3->label, "unknown", "unknown" ); }
               ;
/* Added for AMS */
step_limit_specification
               : LIMIT guarded_signal_specification WITH expression ';'
               ;

use_clause
               : USE selected_name ';'  { delete $2; }
               ;
initialization_specification
               : INITIALIZE signal_specification TO expression ';'
               ;
signal_specification
               : signal_list ':' mark
               ;
label_declaration
               : VID
                 {}
                 statement_name_list ';'
               ;
statement_name_list
               : statement_name
               | statement_name_list ',' statement_name
               ;
statement_name
               : VID
                 {}//table->add_symbol( $1->label, "unknown", "unknown" );  }
               | label_array
               ;
label_array
               : VID
                 {}// table->add_symbol( $1->label, "unknown", "unknown" ); }
                 index_constraint
               ;

/* Type Definitions */

enumeration_type_definition
               : '(' ee ')'
                 {
		   /*$$ = new TYPES;
		   $$->set_grp(TYPES::Enumeration);
		   $$->set_list($2);
		   delete $2;*/
		 }
               ;
ee
               : enumeration_literal
                 { /*
		   TYPES *ty = new TYPES;
		   $$ = new TYPELIST($1, ty);*/
		 }
               | ee ',' enumeration_literal
                 {/*
		   $$ = $1;
		   TYPES *ty = new TYPES;
		   $$->insert($3, ty);*/
		 }
               ;
enumeration_literal
               : VID
                 {  $$ = $1->label; }
               | CHARLIT
                 {  $$ = $1; }
               ;

integer_floating_type_definition /* int and float type syntactically same */
               : range_constraint  {}
               ;
range_constraint
               : RANGE range { $$ = $2; }
               ;
range
               : attribute_name { yyerror("Attributes not supported."); }
               | simple_expression direction simple_expression
                 {
		   /*if(strcmp_nocase($1->data_type(), $3->data_type()))
		     err_msg("different types used on sides of ", $2);
		   */
		   $$ = new TYPELIST(make_pair(string(), $1));
		   $$->insert($2, $3);
		 }
	       ;
direction
               : TO { $$ = "'to'"; }
               | DOWNTO { $$ = "'downto'"; }
               ;
physical_type_definition
               : range_constraint UNITS  base_unit_declaration  sud  END UNITS
                 {
		   //$$ = new TYPELIST(0, $3);
		   //$$->combine($4);
		 }
               | range_constraint UNITS  base_unit_declaration  sud
                 END UNITS VID
                 {
		   //$$ = new TYPELIST(0, $3);
		   //$$->combine($4);
		 }
               ;
sud
               : { $$ = 0; }
               | sud secondary_unit_declaration
                 {
		   /*if($1)
		     {
		       $$ = $1;
		       $$->insert(0, $2);
		     }
		   else
		   $$ =  new TYPELIST(0, $2); */
		 }
               ;
base_unit_declaration
              : VID ';'
                {
		  //$$ = new TYPES;
		  //$$->set_string($1->label);
		}
              ;
secondary_unit_declaration
              : VID '=' physical_literal ';'
		{		
		  //$$ = new TYPES("uk", "uk");
		  //$$->set_string($1->label);
		}
               ;
array_type_definition
              : unconstrainted_array_definition
                {
		  //$$ = $1;
		  //$$->set_base($$->data_type());
		  //$$->set_data("vec");
		  //$$->set_grp(TYPES::Array);
		}
              | constrained_array_definition
                {
		  //$$ = $1;
		  //$$->set_base($$->data_type());
		  //$$->set_data("vec");
		}
              ;
unconstrainted_array_definition
               : ARRAY '(' isd ')' OF subtype_indication
                 {
		   /*  $$ = $6;
		   $$->set_base($$->data_type());
		   $$->set_list($3);
		   delete $3;*/
		 }
               ;
constrained_array_definition
               : ARRAY index_constraint OF subtype_indication
                 {
		   /*$$ = $4;
		   $2->combine($$->get_list());
		   $$->set_list($2);
		   $$->set_base($$->data_type());
		   $$->set_grp(TYPES::Array);
		   delete $2;*/
		 }
               ;
isd
               : index_subtype_definition
               | isd index_subtype_definition {$$ = $1; $$->combine($2); }
               ;
index_subtype_definition
               : mark RANGE BOX
                 {
		   /*TYPES *ty = table->lookup($1->front());
		   if(ty == 0)
		     {
		       table->print();
		       err_msg("'", $1->front(), "' undeclared.");
		       ty = new TYPES("err", "err");
		     }
		   else if(strcmp_nocase(ty->obj_type(), "typ"))
		     {
		       err_msg("'", $1->front(), "' is not a type name.");
		       ty->set_obj("err");
		       ty->set_data("err");
		     }
		   else if(ty->get_grp() != TYPES::Integer &&
			   ty->get_grp() != TYPES::Enumeration)
		     {
		       err_msg("integers or enumerations required for ",
			       "subscripts of arrays");
		       ty->set_obj("err");
		       ty->set_data("err");
		     }
		   TYPES lower; lower.set_int(0);
		   TYPES upper; upper.set_int(0);
		   TYPELIST tl(0, &lower);
		   tl.insert(0, &upper);
		   ty->set_list(&tl);
		   $$ = new TYPELIST(0, ty);
		   delete ty;*/
		 }
               ;
index_constraint
               : '(' disdis ')'  // Store the subscripts of array declarations
                 { $$ = $2; }    // in typelists.
               ;
disdis
               : discrete_range
                 {
		   $$ = new TYPELIST(make_pair(string(), $1));
		 }
               | disdis ',' discrete_range
                 {
		   $$ = $1;
		   $$->insert(string(), $3);
		 }
               ;
record_type_definition
               : RECORD elde END RECORD
                 {
		   //$$= new TYPES("uk", "RECORD","RECORD");
		 }
               | RECORD elde END RECORD VID
                 {
		   //$$= new TYPES("uk", "RECORD","RECORD");
		 }
               ;
elde
               : element_declaration
               | elde element_declaration
               ;
element_declaration
               : identifier_list ':' subtype_indication ';'
                 {}
               ;
access_type_definition
              : ACCESS subtype_indication                {}
              ;
file_type_definition
               : VFILE OF name {}
               | VFILE size_constraint OF name {}
               ;
size_constraint
               : '(' expression ')'
               ;
subtype_declaration
               : SUBTYPE VID IS sub_indications ';'
                 {
 		   /*if(strcmp_nocase($4->obj_type(), "uk") == 0)
		     $4->set_obj("typ");
		
		   //$4->set_base($4->data_type());
		   //$4->set_data($2->label);
		   //cout<< $2->label<< endl;
		   if(table->insert($2->label, $4) == false)
		     {
		       err_msg("re-declaration of '", $2->label, "'");
		     }
		     delete $4;*/
		 }
               ;

/* Added for AMS */
sub_indications
               : mark tolerances
                 {}
               | mark tolerances more_toler
                 {}
               | mark gen_association_list tolerances
                 {}   
               | mark gen_association_list tolerances more_toler
                 {}
               | subtype_indication1 tolerances
                 {}
               ;

more_toler
               : ACROSS expression THROUGH
               ;

/* End Additions */

subtype_indication
                : mark 
                 {
		   /*$$ = table->lookup($1->front());
		   if($$ == 0)
		     {
		       err_msg("'", $1->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		     }
		     $$->set_string($1->front().c_str());*/
		 }		
                | mark gen_association_list
                 { /*
		   $$ = table->lookup($1->front());
		   if($$ == 0)
		     {
		       err_msg("'", $1->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		     }
		   else
		     {
		       $$->set_list($2);
		       }*/
		 } 
               | subtype_indication1 {}
               ;
subtype_indication1
               : mark range_constraint
                 {/*
		   $$ = table->lookup($1->front());
		   if($$ == 0)
		     {
		       err_msg("'", $1->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		     }
		   else if(strcmp_nocase($$->obj_type(), "typ"))
		     {
		       err_msg("'", $1->front(), "' is not a type name");
		       $$->set_obj("err");
		       $$->set_data("err");
		     }
		     $$->set_list($2);*/
		 }
               | mark mark range_constraint
                 {
		   //$$= search( table, $1->label );
		 }
               | mark mark
                 { /*
		   $$ = table->lookup($2->front());
		   if($$ == 0)
		     {
		       err_msg("'", $2->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		       }*/
		 }
               | mark mark gen_association_list
                 {
		   //$$= search( table, $1->label );
		 }
               ;
discrete_range
               : range
                 {
		   $$ = new TYPES;
		   $$->set_list($1);
		   $1->iteration_init();
		   $$->set_data($1->value().second.data_type());
		 }
               | subtype_indication
               ;
discrete_range1
               : simple_expression direction simple_expression
                 {
		   $$ = new TYPELIST(make_pair(string(), $1));
		   $$->front().second.theExpression = $1->theExpression;
		   $$->insert($2, $3);
     		 }
               | subtype_indication1
                 {
		   $$= new TYPELIST(make_pair(string(), $1));
		 }
               ;

/* Added for AMS */
scalar_nature_definition
               : name ACROSS name THROUGH VID REFERENCE
               ;

array_nature_definition
               : ARRAY '(' isd ')' OF subnature_indication {}
               | ARRAY index_constraint OF subnature_indication {}
               ;

record_nature_definition
               : RECORD record_nature_alternative
                 END RECORD VID 
                 {} 
               ;
record_nature_alternative
               : identifier_list ':' subnature_indication ';'
               | record_nature_alternative
                 identifier_list ':' subnature_indication ';'
               ;

subnature_declaration
               : SUBNATURE VID IS subnature_indication ';'
               ;

subnature_indication
               : mark //tolerances
                 {}		
               | mark gen_association_list //tolerances
                 {} 
               | subnature_indication1 {}//tolerances
               ;

subnature_indication1
               : mark TOLER expression ACROSS expression THROUGH {}
               | mark gen_association_list TOLER expression
                 ACROSS expression THROUGH {}
               ; 

/* End Additions */


/* Concurrent Statements */

concurrent_statement
               : block_statement { $$ = 0; }
               | process_statement { $$ = 0; }
               | concurrent_procedure_call { $$ = 0; }
               | concurrent_assertion_statement { $$ = 0; }
               | concurrent_signal_assignment_statement{ $$ = 0; }
               | component_instantiation_statement { $$ = 0; }
               | generate_statement { $$ = 0; }
               | concurrent_break_statement { $$ = 0; } /* Added for AMS */
               ;
block_statement
              : block_label is_symbol block_header  block_declarative_part
                VBEGIN block_statement_part block_end
                { tel_tb->insert($1, $6); }
              | block_label guarded_exp is_symbol block_header
                block_declarative_part VBEGIN block_statement_part
                block_end
                { tel_tb->insert($1, $7); }
              ;
block_end
              : END BLOCK ';'
              | END BLOCK VID ';'
              ;
guarded_exp
              : '(' expression ')'                 {}
              ;
block_label
              : label BLOCK
              ;
is_symbol
              :
              | IS
              ;
block_header
              :
              | generic_clause
              | generic_clause generic_map_aspect ';'
              | port_clause {}
              | port_clause          { }
                port_map_aspect ';'  { }
              | generic_clause port_clause
              | generic_clause port_clause port_map_aspect ';'
              | generic_clause generic_map_aspect ';' port_clause
              | generic_clause generic_map_aspect ';'
                port_clause port_map_aspect ';'
              ;
block_declarative_part
              :
              | block_declarative_part block_declarative_item
              ;
block_statement_part
               : { $$= TERSempty(); }
               | block_statement_part concurrent_statement
            	 { $$= TERScompose($1, $2, "||"); }
	       | block_statement_part simultaneous_statement //AMS
                 { $$= TERScompose($1, $2, "||");}
               ;
block_declarative_item
              : subprogram_declaration {}
              | subprogram_body
              | type_declaration
              | subtype_declaration
              | constant_declaration
              | signal_declaration
              | file_declaration
              | alias_declaration
              | component_declaration
              | attribute_declaration
              | label_declaration
              | attribute_specification
              | initialization_specification
              | configuration_specification
              | disconnection_specification
              | use_clause {}
              | variable_declaration
              | group_template_declaration
              | group_declaration
	      /* Added for AMS */
	      | step_limit_specification
	      | nature_declaration
	      | subnature_declaration
	      | quantity_declaration
	      | terminal_declaration
              ;
process_statement
              : process_head process_tail
                {
		  table->new_tb();
		  processChannels.clear();
		}
                process_declarative_part VBEGIN process_statement_part
                END postpone process_end
                {
		  map<string,int>::iterator m(processChannels.begin());
		  while(m != processChannels.end()){
		    string name(m->first);
		    int type(m->second);
		    if(type&ACTIVE  && type&SENT||
		       type&PASSIVE && type&RECEIVED){
		      type |= PUSH;
		    }
		    if(type&ACTIVE  && type&RECEIVED ||
		       type&PASSIVE && type&SENT    ){
		      type |= PULL;
		    }
		    signals[cur_entity][name] |= type;
		    action(name+SEND_SUFFIX+"+")->type |= type;
		    action(name+SEND_SUFFIX+"-")->type |= type;
		    action(name+RECEIVE_SUFFIX+"+")->type |= type;
		    action(name+RECEIVE_SUFFIX+"-")->type |= type;
		    m++;
		  }
		  table->delete_tb();
		  $$ = 0;
		  if($6){
		    $$ = TERSmakeloop($6);
		    $$ = TERSrepeat($$,";");
		    char s[255];
		    strcpy(s, rmsuffix($1));
		    if (1) {
		      printf("Compiled process %s\n",s);
		      fprintf(lg,"Compiled process %s\n",s);
		    } 
		    else
		      emitters(outpath, s, $$);
		  }
		  if(tel_tb->insert(($1), $$) == false)
		    err_msg("duplicate process label '", $1, "'");
		}
               ;
process_end
               : PROCESS ';'
               | PROCESS VID ';'
               ;
process_head
               : PROCESS
                 {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   $$ = copy_string(n);
		   //TYPES *t = new TYPES("prc", "uk");
		   //table->new_scope($$, "process", t);
		 }
               | label PROCESS
                 {
		   $$ = copy_string($1);
		   //TYPES *t = new TYPES("prc", "uk");
		   //table->new_scope($$, "process", t);
		 }
               | POSTPONED PROCESS
                 {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   $$ = copy_string(n);
		 }
               | label POSTPONED PROCESS
                 { $$ = copy_string($1); }
 	       ;
postpone
               : { }
               | POSTPONED
               ;
process_tail
               : is_symbol
               | '(' sensitivity_list ')' is_symbol
                 {
		   __sensitivity_list__= 1;
		 }
               ;
sensitivity_list
               : name
                 {
		   //TYPES *t= search( table, $1->getstring());
                   //table->add_symbol( $1->getstring(), t );
                   //delete t;
                 }
               | sensitivity_list ',' name
                 {
		   //TYPES *t= search( table, $3->getstring());
		   //table->add_symbol($3->getstring(), t );
                   //delete t;
		 }
               ;
process_declarative_part
               :
               | process_declarative_part process_declarative_item
               ;
process_declarative_item
               : subprogram_declaration {}
               | subprogram_body
               | type_declaration
               | subtype_declaration
               | constant_declaration
               | variable_declaration
               | file_declaration
               | alias_declaration
               | attribute_declaration
               | label_declaration
               | attribute_specification
               | use_clause {}
               | group_template_declaration
               | group_declaration
               ;
process_statement_part
               : { $$ = 0; }
               | process_statement_part sequential_statement
		 {
		   if($2->second)
		     $$ = TERScompose($1, TERSrename($1,$2->second),";");
		   else
		     $$ = $1;
		   delete $2;
		 }	
               ;
concurrent_procedure_call
               : procedure_call_statement
//: name '(' association_list ')' ';' {}
               | label procedure_call_statement {}
               | POSTPONED procedure_call_statement
               | label POSTPONED procedure_call_statement {}
               ;
component_instantiation_statement
            : label COMPONENT name  ';'
              {
		//SymTab *temp= search1(table,$3->name2->label );
		//delete temp;
		
	      }
            | label cmpnt_stmt_body map_aspects ';'
              {
		string comp_id = $2->str();
		if(open_entity_lst->find(comp_id).empty()){
		  err_msg("error: component '",comp_id.c_str(), "' not found");
		  delete $3;
		  return 1;
		}
		else {
		  const tels_table& TT = open_entity_lst->find(comp_id,
							       string());
		  if(TT.empty()){
		    err_msg("error: no architecture defined for '",
			    comp_id, "'");
		    return 1;
		  }		
	
		  tels_table* rpc = new tels_table(TT);

		  const list<pair<string,int> >& tmp = rpc->signals1();	
		  if(tmp.size() != $3->size())
		    err_msg("incorrect port map of '",$2->str(),"'");
		  else {
		    const vector<pair<string,int> > sig_v(tmp.begin(),
							  tmp.end());
		    const map<string,int>& sig_m = rpc->signals();
		    map<string,int>& ports = tel_tb->port_decl();
		    list<pair<string,string> >::size_type pos = 0;
		    map<string,string> port_maps;
		    list<pair<string,string> >::iterator BB;
		    for(BB = $3->begin(); BB != $3->end(); BB++, pos++){
		      string formal(BB->first);
		      string actual(BB->second);
		      if(formal.empty() && pos < sig_v.size()){
			formal = sig_v[pos].first;
		      }
		      if(sig_m.find(formal) == sig_m.end()){
			err_msg("undeclared formal signal '", formal,"'");
		      }
		      else{
			int type(sig_m.find(formal)->second);
			if(signals[cur_entity].find(actual) ==
			   signals[cur_entity].end()){
			  if(ports.find(actual) ==   ports.end()){
			    err_msg("undeclared actual signal ", actual);
			  }
			  else{
			    ports[actual] |= type;
			  }
			}
			else{
			  const int use(ACTIVE|PASSIVE|SENT|RECEIVED);
			  int overlap(signals[cur_entity][actual] &type & use);
			  if(overlap & ACTIVE){
			    err_msg("Channel ",actual,
				    " is active on both sides!");
			  }
			  if(overlap & PASSIVE){
			    err_msg("Channel ",actual,
				    " is passive on both sides!");
			  }
			  if(overlap & SENT){
			    err_msg("Channel ",actual,
				    " has sends on both sides!");
			  }
			  if(overlap & RECEIVED){
			    err_msg("Channel ",actual,
				    " has receives on both sides!");
			  }
			  if(type&ACTIVE  && type&SENT||
			     type&PASSIVE && type&RECEIVED){
			    type |= PUSH;
			  }
			  if(type&ACTIVE  && type&RECEIVED ||
			     type&PASSIVE && type&SENT    ){
			    type |= PULL;
			  }
			  signals[cur_entity][actual] |= type;
			  action(actual+SEND_SUFFIX+"+")->type |= type;
			  action(actual+SEND_SUFFIX+"-")->type |= type;
			  action(actual+RECEIVE_SUFFIX+"+")->type |= type;
			  action(actual+RECEIVE_SUFFIX+"-")->type |= type;
			}
		      }
		      port_maps[formal]=actual;
		    }
		    my_list *netlist = new my_list(port_maps);
		    string s = $2->str();
		    rpc->set_id($1);
		    rpc->set_type(comp_id);
		    rpc->instantiate(netlist, $1, s);
		    rpc->insert(port_maps);
		    if(tel_tb->insert(($1), rpc) == false)
		      err_msg("duplicate component label '", $1, "'");
		    delete netlist;
		  }		
		  delete $3;
		}
	      }
            ;
cmpnt_stmt_body
               : name
               | COMPONENT name { $$ = $2; }
               | ENTITY name { $$ = $2; }
               | CONFIGURATION name { $$ = $2; }
               ;
concurrent_assertion_statement
               : assertion_statement 
                 { 
		   char n[100];
		   sprintf(n, "ss%ld", PSC);
		   PSC++;
		   printf("Compiled assertion statement %s\n",n);
		   fprintf(lg,"Compiled assertion statement %s\n",n);
		   TERstructADT x;
		   x = TERSmakeloop($1);
		   x = TERSrepeat(x,";");
		   tel_tb->insert(n, x);
		 }
               | label assertion_statement { 
		 char n[100];
		 if ($1)
		   strcpy(n,$1);
		 else {
		   sprintf(n, "ss%ld", PSC);
		   PSC++;
		 }
		 printf("Compiled assertion statement %s\n",n);
		 fprintf(lg,"Compiled assertion statement %s\n",n);
		 TERstructADT x;
		 x = TERSmakeloop($2);
		 x = TERSrepeat(x,";");
		 tel_tb->insert(n, x);
	       }
               | POSTPONED assertion_statement { }
               | label POSTPONED assertion_statement { }
               ;
concurrent_signal_assignment_statement
               : conditional_signal_assignment
                 {
		   char id[200];
		   sprintf(id, "CS%ld", (unsigned long)$1);
		   tel_tb->insert((id), $1);
		 }
               | label conditional_signal_assignment
                 { tel_tb->insert(($1), $2); }
               | selected_signal_assignment { $$ = 0; }
               | label selected_signal_assignment { $$ = 0; }
               | POSTPONED conditional_signal_assignment  { $$ = 0; }
               | label POSTPONED conditional_signal_assignment
                 { $$ = 0; }
               | POSTPONED selected_signal_assignment { $$ = 0; }
               | label POSTPONED selected_signal_assignment { $$ = 0; }
               ;
conditional_signal_assignment
               : target LE options conditional_waveforms ';'
                 {
		   string ENC, EN_r, EN_f;
		   int LL = INFIN, UU = 0;
		   list<pair<TYPES,TYPES> >::iterator b;
		   for(b = $4->begin(); b != $4->end(); b++){
		     if(b->first.grp_id() != TYPES::Error &&
			b->second.grp_id() != TYPES::Error){
		       // Calculate the delay.
		       const TYPELIST *t = b->first.get_list();
		       if(b->first.str() != "unaffected" &&
			  t != 0 && t->empty() == false){
			 if(LL>t->front().second.get_int())
			   LL = t->front().second.get_int();
			 if(UU<t->back().second.get_int())
			   UU = t->back().second.get_int();
			 if (!(t->front().second.get_string().empty())) {
			   actionADT a= action(t->front().second.get_string());
			   LL = a->intval;
			 }
			 if (!(t->back().second.get_string().empty())) {
			   actionADT a = action(t->back().second.get_string());
			   UU = a->intval;
			 }
		       }
		       if(!ENC.size()){
			 if(b->first.str() != "unaffected"){
			   EN_r = logic(bool_relation(b->first.str(), "'1'"),
					b->second.str());
			   EN_f = logic(bool_relation(b->first.str(), "'0'"),
					b->second.str());
			 }
			 ENC = my_not(b->second.str());
		       }
		       else{
			 if(b->first.str() != "unaffected"){
			   EN_r = logic(EN_r,
					logic(logic(bool_relation(b->first.str(),
							     "'1'"),
						    b->second.str()),
					      ENC),
					"|");
			   EN_f = logic(EN_f,
					logic(logic(bool_relation(b->first.str(),
							     "'0'"),
						    b->second.str()),
					      ENC),
					"|");
			 }
			 ENC = logic(ENC, my_not(b->second.str()));
		       }
		     }
		   }		

		   string a_r = $1->get_string() + '+';
		   string a_f = $1->get_string() + '-';
		   actionADT a = action($1->str() + '+');
		   telADT RR, FF, RC, FC;
		   string CC = '[' + EN_r + ']' + 'd';
		   RC = TERS(dummyE(), FALSE, 0,0, TRUE, CC.c_str());
		   RR = TERS(action($1->str()+'+'),FALSE,LL,UU,TRUE);
		   CC = '[' + EN_f + ']' + 'd';
		   FC = TERS(dummyE(), FALSE, 0,0, TRUE, CC.c_str());
		   FF = TERS(action($1->str()+'-'),FALSE,LL,UU,TRUE);
		   if (a->initial) {
		     $$ = TERScompose(TERScompose(FC, FF, ";"),
				      TERScompose(RC, RR, ";"), ";");
		   } else {
		     $$ = TERScompose(TERScompose(RC, RR, ";"),
				      TERScompose(FC, FF, ";"), ";");
		   }
		   $$ = TERSmakeloop($$);
		   $$ = TERSrepeat($$,";");
		   //emitters(0, "tmp", $$);
		 }
               ;
target
               : name
               | aggregate
               ;
options
               :
               | delay_mechanism
               | GUARDED
               | GUARDED delay_mechanism
               ;
delay_mechanism
               : TRANSPORT
               | INERTIAL
               | VREJECT expression INERTIAL
               ;
conditional_waveforms
               : waveform_head waveform
                 {
		   if($1) $$ = $1;
		   else   $$ = $$ = new list<pair<TYPES,TYPES> >;
		   TYPES ty("uk","bool");
		   ty.set_grp(TYPES::Enumeration);
		   ty.set_str("true");
		   $$->push_back(make_pair(*$2,ty));
		 }
               | waveform_head waveform WHEN expression
                 {
		   if($1)
		     $$ = $1;
		   else
		     $$ = new list<pair<TYPES,TYPES> >;
		   if($4->grp_id() != TYPES::Error &&
		      $4->data_type() != string("bool")) {
		     err_msg("expect a boolean expression after 'when'");
		     $4->set_grp(TYPES::Error);
		   }
		   $$->push_back(make_pair(*$2, *$4));
		 }
               ;
waveform_head
               : { $$ = 0; }
               | waveform_head waveform WHEN expression ELSE
                 {
		   if($1)  $$ = $1;
		   else    $$ = new list<pair<TYPES,TYPES> >;
		   if($4->grp_id() != TYPES::Error &&
		      $4->data_type() != string("bool")){
		     err_msg("expect a boolean expression after 'when'");
		     $4->set_grp(TYPES::Error);
		   }
		   $$->push_back(make_pair(*$2, *$4));
		 }
               ;
selected_signal_assignment
               : WITH expression SELECT target LE options
                 selected_waveforms ';'
                 {
		   delete $7;
		   /*telADT ret_t = TERSempty();
		   if($7){		
		     for(list<pair<TYPES*,TYPES*> >::iterator b = $7->begin();
		     b != $7->end(); b++){*/
		
		 }
               ;
selected_waveforms
               : waveform WHEN choices
                 {
		   $$ = 0;
		   if($3){
		     if($3->back().data_type() != "bool")
		       err_msg("expect a boolean expression after 'when'");
		     else{
		       $$ = new list<pair<TYPES,TYPES> >;
		       TYPES ty("uk","bool");
		       ty.set_grp(TYPES::Enumeration);
		       list<string> str_l;
		       for(list<TYPES>::iterator b = $3->begin();
			   b != $3->end(); b++)
			 str_l.push_back(b->str());
		       ty.set_str(logic(str_l, "|"));
		       $$->push_back(make_pair(*$1,ty));
		     }
		   }
		 }
               | selected_waveforms ',' waveform WHEN choices
                 {
		   $$ = 0;
		   if($1 && $5){
		     if($5->back().data_type() != "bool"){
		       err_msg("expect a boolean expression after 'when'");
		       delete $1; delete $5; $1 = 0;
		     }
		     else{
		       TYPES ty("uk","bool");
		       ty.set_grp(TYPES::Enumeration);
		       list<string> str_l;
		       for(list<TYPES>::iterator b = $5->begin();
			   b != $5->end(); b++)
			 str_l.push_back(b->str());
		       ty.set_str(logic(str_l, "|"));
		       $1->push_back(make_pair(*$3,ty));
		     }
		   }
		   $$ = $1;
                 }
                /*| {
		    //$$= new TYPES( "ERROR", "ERROR" );
		    }*/
               ;
generate_statement
               : label generation_head generate_declarative_part
                 concurrent_statement_sequence END GENERATE generate_tail ';'
                 {}
               | label generation_head generate_declarative_part
 	         VBEGIN concurrent_statement_sequence END GENERATE
                 generate_tail ';'
                 {}
               ;
generation_head
               : generation_scheme GENERATE
                 {}
               ;
generate_tail
               :
               | VID
                 {
		   /*TYPES *t= search( table, $1->label );
		     delete t;
		     SymTab *temp= table->header;
		     delete table;
		     table= temp;*/
		 }
               ;
generate_declarative_part
               : generate_declarative_part block_declarative_item
               |
               ;

/* Added for AMS (Simultaneous Statement Parts) */
concurrent_statement_sequence
               : concurrent_statement
                 {}
               | simultaneous_statement
                 {}
               | concurrent_statement_sequence concurrent_statement
	       | concurrent_statement_sequence simultaneous_statement
               ;
generation_scheme
               : FOR parameter_specification
               | IF expression
                 { /*
		   if( !($2.type->isBool() || $2.type->isError()) )
		     err_msg_3( "the 'if' should be followed a boolean",
				"expression", "");*/
		 }
               ;
parameter_specification
               : VID VIN discrete_range
                 {}
	       ;
/* Added for AMS */
concurrent_break_statement
               : label_symbol BREAK break_elements
                 sensitivity_clause when_symbol ';'
               | label_symbol BREAK break_elements when_symbol ';'
                 {
                   TERstructADT breakElem = 0;
                   if ($3) {
		     breakElem = Guard(*$3);
		   }
		   //An assign statement
		   if ($4) {
		     char n[100];
		     if ($1)
		       strcpy(n,$1);
		     else {
		       sprintf(n, "ss%ld", PSC);
		       PSC++;
		     }
		     string condition = $4->O->event->exp;
		     string remainder;

		     //Removes the ]
		     condition = condition.substr(0,condition.size()-1);

		     
		     //change ='0' and ='1' to bool or ~bool
		     while(condition.find("='") != string::npos) {
		       if (condition.find("='1'") != string::npos) {
			 remainder = condition.substr(condition.find("='1'")+
						      4, string::npos);
			 condition = condition.substr(0,
						      condition.find("='1'"));
		       }
		       else if (condition.find("='0'") != string::npos) {
			 remainder = condition.substr(condition.find("='0'")+
						      4, string::npos);
			 condition = condition.substr(1,string::npos);
			 condition = "[~(" +
			   condition.substr(0,condition.find("='0'")) + ")";
		       }
		       condition = condition + remainder;
		     }
		     string action = breakElem->O->event->exp;
		     action = action.substr(1, string::npos);
		     $4->O->event->exp = CopyString((condition+
					  " & "+ action).c_str());
		     
		     
		     TERstructADT x;
		     //x = TERScompose($4, breakElem, ";");
		     x = TERSmakeloop($4);
		     x = TERSrepeat(x,";");
		     char s[255];
		     strcpy(s, rmsuffix(n));
		     printf("Compiled break statement %s\n",s);
		     fprintf(lg,"Compiled break statement %s\n",s);
		     tel_tb->insert(n, x);
		   }
		   //initial conditions
		   else {
		     for (eventsetADT step = breakElem->O; step; step = step->link) {
		       TERstructADT x;

		       //pull apart the exp to get the variable and the number
		       string var = step->event->exp;
		       var = var.substr(1,var.find(":=")-1);
		       string num = step->event->exp;
		       num = num.substr(num.find(":=")+2,string::npos);
		       num = num.substr(0,num.find(']'));

		       actionADT a = action(var);
		       a->type = CONT + VAR + DUMMY;
		       if (num.find(';') == string::npos) {
			 a->linitval = atoi(num.c_str());
			 a->uinitval = atoi(num.c_str());
		       } else {
			 string lval = num.substr(1,num.find(';'));
			 string uval = num.substr(num.find(';')+1,
						  string::npos-1);
			 a->linitval = atoi(lval.c_str());
			 a->uinitval = atoi(uval.c_str());
		       }
		       x = TERS(a,FALSE,0,0,FALSE);
		       tel_tb->insert(var, x);
		     }
		   }
		 }
               ;

/* Simultaneous Statements */
/*      Added for AMS      */
simultaneous_statement :
         simul_statement
         {

	   /*
	     for (simul_lst::iterator elem = $1->begin();
	     elem != $1->end() && elem != NULL; elem++) {
	     cout << elem->first << endl;
	     for (list<string>::iterator selem=elem->second.begin();
	     selem != elem->second.end(); selem++) {
	     cout << "\t" << *selem << endl;
	     }
	     }
	   */
		   
	   if ($1) {
	     if ($1->size() == 1) {
	       //Simple Simultaneous Statment
	       //Like a rate assignment
			
	       char n[1000];
	       sprintf(n, "ss%ld", PSC);
	       PSC++;
	       char s[255];
	       strcpy(s, rmsuffix(n)); 

	       printf("Compiled simple simultaneous statement %s\n",s);
	       fprintf(lg,
		       "Compiled simple simultaneous statement %s\n",s);

	       $$ = Guard(*($1->begin()->second.begin()));
	       $$ = TERSmakeloop($$);
	       $$ = TERSrepeat($$,";");
	       tel_tb->insert(n, $$);
	     }
	     else {
		       
	       actionsetADT allActions;
	       eventsetADT allEvents;
	       rulesetADT allRules;
	       conflictsetADT allConflicts;
		       
	       rulesetADT initRules;
	       unsigned int j = 1;
	       int expanded = EXPANDED_RATE_NETS;

	       if (expanded==0 || expanded==2) {
		 actionADT x;
		 x = dummyE();
		 x->maxoccur = 1;
		 allActions = create_action_set(x);
		 allEvents = create_event_set(event(x,1,2,1,NULL,""));
	       }

		       
	       for (simul_lst::iterator elem = $1->begin();
		    elem != $1->end(); elem++) {
		 string level = "[" + elem->first;
		 //level = level.substr(0,level.find(" & true"));

		 for(list<string>::iterator selem=elem->second.begin();
		     selem != elem->second.end(); selem++) {
		   level += " & " +  *selem;
		 }
		 while (level.find(" & true") != string::npos) {
		   level.replace(level.find(" & true"),7,"");
		 }
		 while (level.find("true ") != string::npos) {
		   level.replace(level.find("true "),7,"");
		 }

		 if (expanded==2) {
		   for (simul_lst::iterator elem2 = $1->begin();
			elem2 != $1->end(); elem2++) {
		     if (elem2 != elem) {
		       for(list<string>::iterator selem2=elem2->second.begin();
			   selem2 != elem2->second.end(); selem2++) {
			 unsigned int first,last,ins,curEnd;
			 curEnd=(*selem2).length()-1;
			 while (curEnd > 0) {
			   for (last=curEnd;(*selem2)[last]!=':' 
				  && last>=0;last--);
			   for (first=last;(*selem2)[first]!=' ' 
				  && first>=0;first--);
			   if (first >= 0) {
			     for (ins=level.length()-1;level[ins]==')'
				    && ins>=0;ins--);
			     if (ins >= 0) {
			       level.insert(ins+1," & " + 
					    (*selem2).substr(first+1,
							     last-first-1) +
					    ":=FALSE");
			     }
			   }
			   for (curEnd=first;(*selem2)[curEnd]!='|' 
				  && curEnd>0;curEnd--);
			 }
		       }
		     }
		   }
		 }

		 level += "]";

		 if (expanded==1) {
		   //Creates one event for each of the other events
		   for (unsigned int i = 1; i <= $1->size(); i++) {
		     if (i != j) {
		       eventsetADT curEvent;
		       actionsetADT curAction;
		       actionADT a;
			       
		       if (j == 1 && i == 2) {
			 a = dummyE();
			 a->maxoccur = 1;
			 allActions = create_action_set(a);
			 allEvents =
			   create_event_set(event(a,1,i,j,NULL,
						  level.c_str()));
		       }
		       else {
			 a = dummyE();
			 a->maxoccur = 1;
			 curAction = create_action_set(a);
			 curEvent =
			   create_event_set(event(a,1,i,j,
						  NULL,level.c_str()));
			 allActions = union_actions(allActions,
						    curAction);
			 allEvents = union_events(allEvents,curEvent);
		       }
		     }
		   }
		 }
		 else {
		   eventsetADT curEvent;
		   actionsetADT curAction;
		   actionADT a;
		   a = dummyE();
		   a->maxoccur = 1;
		   curAction = create_action_set(a);
		   curEvent =
		     create_event_set(event(a,1,1,2,
					    NULL,level.c_str()));
		   allActions = union_actions(allActions,
					      curAction);
		   allEvents = union_events(allEvents,curEvent);
		 }
		 if (expanded==1) j++;
	       }

	       bool firstR = true;
	       bool firstRp = true;
	       bool firstC = true;
	       for (eventsetADT toEvent = allEvents;
		    toEvent != NULL; toEvent = toEvent->link) {
		 for (eventsetADT fromEvent = allEvents;
		      fromEvent != NULL; fromEvent = fromEvent->link) {
		   if(fromEvent->event->upper==toEvent->event->lower) {
		     if (toEvent->event->lower == 1) {
		       if (firstRp) {
			 initRules =
			   create_rule_set(rule(fromEvent->event,
						toEvent->event,
						0,0,TRIGGER,true,NULL,
						toEvent->event->exp));
			 firstRp = false;
		       }
		       else {
			 initRules = add_rule(initRules,
					      fromEvent->event,
					      toEvent->event,
					      0,0,TRIGGER,NULL,
					      toEvent->event->exp);
		       }
		     }
		     else {
		       if (firstR) {
			 allRules =
			   create_rule_set(rule(fromEvent->event,
						toEvent->event,
						0,0,TRIGGER,true,NULL,
						toEvent->event->exp));
			 firstR = false;
		       }
		       else {
			 allRules = add_rule(allRules,fromEvent->event,
					     toEvent->event,
					     0,0,TRIGGER,NULL,
					     toEvent->event->exp);
		       }
		     }
		   }
		   if(((fromEvent->event->lower!=toEvent->event->lower &&
			fromEvent->event->upper==toEvent->event->upper) ||
		       (fromEvent->event->upper!=toEvent->event->upper &&
			fromEvent->event->lower==toEvent->event->lower)) ||
		      ((expanded==0 || expanded==2) &&
		       fromEvent->event->upper==toEvent->event->upper &&
		       fromEvent->event->lower==toEvent->event->lower &&
		       fromEvent->event->upper != 1 &&
		       strcmp(fromEvent->event->exp,toEvent->event->exp)))
		     {
		       if (firstC) {
			 allConflicts =
			   create_conflict_set(conflict(fromEvent->
							event,
							toEvent->
							event));
			 firstC = false;
		       }
		       else {
			 allConflicts = add_conflict(allConflicts,
						     fromEvent->event,
						     toEvent->event);
		       }
		     }
		 }
	       }

	       for (simul_lst::iterator elem = $1->begin();
		    elem != $1->end(); elem++) {
		 if (expanded==2) {
		   for(list<string>::iterator selem=elem->second.begin();
		       selem != elem->second.end(); selem++) {
		     eventsetADT curEvent;
		     actionsetADT curAction;
		     actionADT a;
		     unsigned int first,last,curEnd;
		     curEnd=(*selem).length()-1;
		     while (curEnd > 0) {
		       for (last=curEnd;(*selem)[last]!=':' 
			      && last>=0;last--);
		       for (first=last;(*selem)[first]!=' ' 
			      && first>=0;first--);
		       if ((first >= 0)&&(last >= 0)) {
			 a = action((*selem).substr(first+1,last-first-1)+'+');
			 a->type = OUT;
			 a->maxoccur = 1;
			 curAction = create_action_set(a);
			 curEvent =
			   create_event_set(event(a,1,0,0,NULL,NULL));
			 allActions = union_actions(allActions,
						    curAction);
			 allEvents = union_events(allEvents,curEvent);
			 a = action((*selem).substr(first+1,last-first-1)+'-');
			 a->type = OUT;
			 a->maxoccur = 1;
			 curAction = create_action_set(a);
			 curEvent =
			   create_event_set(event(a,1,0,0,NULL,NULL));
			 allActions = union_actions(allActions,
						    curAction);
			 allEvents = union_events(allEvents,curEvent);
		       }
		       for (curEnd=first;(*selem)[curEnd]!='|' 
			      && curEnd>0;curEnd--);
		     } 
		   }
		 }
	       }

	       $$ = new terstruct_tag;
	       $$->A = allActions;
	       $$->O = allEvents;
	       $$->R = allRules;
	       $$->Rp = initRules;
	       if (firstC == true) $$->C = NULL;
	       else $$->C = allConflicts;

		       
	       $$->I = NULL;
	       $$->first = NULL;
	       $$->last = NULL;
	       $$->loop = NULL;
	       $$->CT = NULL;
	       $$->CP = NULL;
	       $$->DPCT = NULL;
	       $$->CPCT = NULL;
	       $$->DTCP = NULL;
	       $$->CTCP = NULL;
	       $$->CPDT = NULL;
	       $$->Cp = NULL;
	       $$->exp = NULL;

	       char n[1000];
	       sprintf(n, "ss%ld", PSC);
	       PSC++;
	       char s[255];
	       strcpy(s,rmsuffix(n));
	       fflush(stdout);
	       tel_tb->insert(n, $$);
	       printf("Compiled simultaneous statement %s\n",s);
	       fprintf(lg,"Compiled simultaneous statement %s\n",s);
		       
	     }

	   }
	 }
                 ;

simul_statement :
                 simple_simultaneous_statement
                 {
		   $$ = $1;
		 }
               | simultaneous_if_statement
                 {
		   $$ = $1;
		 }
               | simultaneous_case_statement
	         {
		   $$ = $1;
		   //TYPES ty; ty.set_str("case");
		   //$$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | simultaneous_procedural_statement
	         {
		   $$ = 0;
		   printf("Procedures are not supported\n");
		   //TYPES ty; ty.set_str("proc");
		   //$$ = new pair<TYPES,telADT>(ty, 0);
		 }
               | null_statement
                 {
		   $$ = 0;
		   //TYPES ty; ty.set_str("nil");
		   //$$ = new pair<TYPES,telADT>(ty, 0);
		 }
               ;

sequence_of_simul_statements : { $$ = 0; }
               | sequence_of_simul_statements simul_statement
                 {
		   $$ = new simul_lst;
		   bool simple=true;
		   if ($1) {
		     for (simul_lst::iterator elem = $1->begin();
			  elem != $1->end(); elem++) {
		       if (elem->first != "true") {
			 simple=false;
			 break;
		       }
		     }
		   }
		   if (simple) {
		     if ($2) {
		       for (simul_lst::iterator elem = $2->begin();
			    elem != $2->end(); elem++) {
			 if (elem->first != "true") {
			   simple=false;
			   break;
			 }
		       }
		     }
		   }
		   if (simple) {
		     list<string> expressions;
		     string expression="";
		     bool first=true;
		     if ($1) {
		       for (simul_lst::iterator elem = $1->begin();
			    elem != $1->end(); elem++) {
			 for(list<string>::iterator selem=elem->second.begin();
			     selem != elem->second.end(); selem++) {
			   if (first) {
			     first=false;
			     expression="((";
			   } else 
			     expression+="|(";
			   expression+=(*selem);
			   expression+=")";
			 }
		       }
		     }
		     if ($2) {
		       for (simul_lst::iterator elem = $2->begin();
			    elem != $2->end(); elem++) {
			 for(list<string>::iterator selem=elem->second.begin();
			     selem != elem->second.end(); selem++) {
			   if (first) {
			     first=false;
			     expression="((";
			   } else 
			     expression+="|(";
			   expression+=(*selem);
			   expression+=")";
			 }
		       }
		     }
		     expression+=")";
		     expressions.push_back(expression);
		     $$->push_back(make_pair("true",expressions));
		   } else {
		     if ($1)
		       $$ = $1;
		     for (simul_lst::iterator elem = $2->begin();
			  elem != $2->end(); elem++) {
		       list<string> expressions;
		       for(list<string>::iterator selem=elem->second.begin();
			   selem != elem->second.end(); selem++) {
			 expressions.push_back(elem->first+" & "+(*selem));
		       }
		       $$->push_back(make_pair("true",expressions));
		     }
		   }
		   /*
		   
		   for (simul_lst::iterator elem = $2->begin();
			elem != $2->end(); elem++) {
		     $$->push_back(*elem);
		     }*/
		 }
               ;

simple_simultaneous_statement :
                 label simple_expression EQ simple_expression
                 tolerances ';'
                 {
		   $$ = new simul_lst;
		   list<string> expressions;
		   expressions.push_back($2->theExpression + ":=" +
					 $4->theExpression);
		   $$->push_back(make_pair("true",expressions));

		 }
               | simple_expression EQ simple_expression
                 tolerances ';'
                 {
		   $$ = new simul_lst;
		   list<string> expressions;
		   expressions.push_back($1->theExpression + ":=" +
					 $3->theExpression);
		   $$->push_back(make_pair("true",expressions));
		 }
	       | label mark '\'' VDOT EQ simple_expression ';'
                 {
		   $$ = new simul_lst;
		   list<string> expressions;
		   if (EXPANDED_RATE_NETS==0) {
		     expressions.push_back("~(" + (*$2->begin()) + "dot>=" +
					   $6->theExpression + " & " +
					   (*$2->begin()) + "dot<=" +
					   $6->theExpression + ") & " +
					   *($2->begin()) + "'dot:=" +
					   $6->theExpression);
		   } else if (EXPANDED_RATE_NETS==1) {
		     expressions.push_back(*($2->begin()) + "'dot:=" +
					   $6->theExpression);
		   } else {
		     string temp_str;
		     temp_str =(*$2->begin()) + "dot_";
		     for (int i=0;$6->theExpression[i]!='\0';i++) {
		       if ($6->theExpression[i]=='-')
			 temp_str += "m";
		       else if ($6->theExpression[i]==';')
			 temp_str += "_";
		       else if (($6->theExpression[i]!='{') &&
				($6->theExpression[i]!='}'))
			 temp_str += $6->theExpression[i];
		     } 
		     expressions.push_back("~(" + temp_str + ") & " +
					   *($2->begin()) + "'dot:=" +
					   $6->theExpression + " & " +
					   temp_str + ":=TRUE"); 
		   }
		   $$->push_back(make_pair("true",expressions));
		 }
	       | mark '\'' VDOT EQ simple_expression ';'
                 {
		   $$ = new simul_lst;
		   list<string> expressions;
		   if (EXPANDED_RATE_NETS==0) {
		     expressions.push_back("~(" + (*$1->begin()) + "dot>=" +
					   $5->theExpression + " & " +
					   (*$1->begin()) + "dot<=" +
					   $5->theExpression + ") & " +
					   *($1->begin()) + "'dot:=" +
					   $5->theExpression);
		   } else  if (EXPANDED_RATE_NETS==1) {
		     expressions.push_back(*($1->begin()) + "'dot:=" +
					   $5->theExpression);
		   } else {
		     string temp_str;
		     temp_str =(*$1->begin()) + "dot_";
		     for (int i=0;$5->theExpression[i]!='\0';i++) {
		       if ($5->theExpression[i]=='-')
			 temp_str += "m";
		       else if ($5->theExpression[i]==';')
			 temp_str += "_";
		       else if (($5->theExpression[i]!='{') &&
				($5->theExpression[i]!='}'))
			 temp_str += $5->theExpression[i];
		     } 
		     expressions.push_back("~(" + temp_str + ") & " +
					   *($1->begin()) + "'dot:=" +
					   $5->theExpression + " & " +
					   temp_str + ":=TRUE");
		   }
		   $$->push_back(make_pair("true",expressions));
		 }
               ;

simultaneous_if_statement :
                 label IF expression USE
                 sequence_of_simul_statements
                 simul_elsifthen simul_else_part END USE VID ';'
                 {
		   string exp = "";
		     if($3->data_type() != string("bool"))
		       exp = $3->theExpression;
		     else
		       exp = $3->str();
		     /*
		   if ($3->theExpression.find("=") != string::npos) {
		     if ($3->theExpression.find("\'0\'") != string::npos) {
		       exp = "~";
		     }
		     exp +=
		       $3->theExpression.substr(0,$3->theExpression.find("="));
		   }
		     */

		   $$ = new simul_lst;
		   for (simul_lst::iterator elem = $5->begin();
			elem != $5->end(); elem++) {
		     $$->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		   string notOthers = "";
		   if ($6) {
		     string current = "";
		     string last = "";
		     bool first = true;
		     for (simul_lst::iterator elem = $6->begin();
			  elem != $6->end(); elem++) {
		       if (elem->first != current) {
			 last = notOthers;
			 if (first) {
			   notOthers = "(";
			   first=false;
			 } else {
			   notOthers += " & ";
			 }
			 notOthers += "~(" + elem->first + ")";
			 current=elem->first;
		       }
		       if (last=="") {
			 $$->push_back(make_pair("~(" + exp +
						 ") & " + 
						 elem->first,
						 elem->second));
		       } else {
			 $$->push_back(make_pair("~(" + exp +
						 ") & " + 
						 last + ") & " +
						 elem->first,
						 elem->second));
		       }
		     }
		     notOthers += ") & ";
  		   }
		   if ($7) {
		     for (simul_lst::iterator elem = $7->begin();
			  elem != $7->end(); elem++) {
		       $$->push_back(make_pair("~(" + exp +
					       ") & " + notOthers+elem->first,
					       elem->second));
		     }
		   }
		 }
               | IF expression USE
                 sequence_of_simul_statements
                 simul_elsifthen simul_else_part END USE ';'
                 {
		   string exp = "";
		     if($2->data_type() != string("bool"))
		       exp = $2->theExpression;
		     else
		       exp = $2->str();
		     /*
		   if ($2->theExpression.find("=") != string::npos) {
		     if ($2->theExpression.find("\'0\'") != string::npos) {
		       exp = "~";
		     }
		     exp +=
		       $2->theExpression.substr(0,$2->theExpression.find("="));
		   }
		     */

		   $$ = new simul_lst;
		   for (simul_lst::iterator elem = $4->begin();
			elem != $4->end(); elem++) {
		     $$->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		   string notOthers = "";
		   if ($5) {
		     string current = "";
		     string last = "";
		     bool first = true;
		     for (simul_lst::iterator elem = $5->begin();
			  elem != $5->end(); elem++) {
		       if (elem->first != current) {
			 last = notOthers;
			 if (first) {
			   notOthers = "(";
			   first=false;
			 } else {
			   notOthers += " & ";
			 }
			 notOthers += "~(" + elem->first + ")";
			 current=elem->first;
		       }
		       if (last=="") {
			 $$->push_back(make_pair("~(" + exp +
						 ") & " + 
						 elem->first,
						 elem->second));
		       } else {
			 $$->push_back(make_pair("~(" + exp +
						 ") & " + 
						 last + ") & " +
						 elem->first,
						 elem->second));
		       }
		     }
		     notOthers += ") & ";
  		   }
		   if ($6) {
		     for (simul_lst::iterator elem = $6->begin();
			  elem != $6->end(); elem++) {
		       $$->push_back(make_pair("~(" + exp +
					       ") & " + notOthers+elem->first,
					       elem->second));
		     }
		   }
		 }
	       ;

simul_elsifthen :
                 { $$ = 0; }
               | simul_elsifthen ELSIF expression USE
	         sequence_of_simul_statements
                 {
		   string exp = "";
		     if($3->data_type() != string("bool"))
		       exp = $3->theExpression;
		     else
		       exp = $3->str();
		     /*
		   if ($3->theExpression.find("=") != string::npos) {
		     if ($3->theExpression.find("\'0\'") != string::npos) {
		       exp = "~";
		     }
		     exp +=
		       $3->theExpression.substr(0,$3->theExpression.find("="));
		       }*/
		   $$ = new simul_lst;
/* 		   string notOthers = ""; */
		   if ($1) {
/* 		     notOthers = "("; */
/* 		     bool first = true; */
/* 		     for (simul_lst::iterator elem = $1->begin(); */
/* 			  elem != $1->end(); elem++) { */
/* 		       if (!first) notOthers += " & "; */
/* 		       notOthers += "~(" + elem->first + ")"; */
/* 		       first = false; */
/* 		     } */
/* 		     notOthers += ") & "; */
		     $$ = $1;
		   }
		  
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = $5->begin();
			elem != $5->end(); elem++) {
		     $$->push_back(make_pair(/*notOthers +*/
					     exp /*+ " & " +
						   elem->first*/,
					     elem->second));
		   }
		 }
               ;
simul_else_part :
                 { $$ = 0; }
               | ELSE sequence_of_simul_statements
                 {
		   $$ = $2;
		 }
               ;

simultaneous_case_statement :
                 label_symbol CASE expression USE
                 simul_case_statement_alternative
                 END CASE VID ';'
                 {
		   $$ = $5;
		   
		   for (simul_lst::iterator elem = $$->begin();
			elem != $$->end(); elem++) {
		     if (elem->first[0] == '~') {
		       elem->first = "~(" + $3->theExpression + ")" +
			 elem->first.substr(1,string::npos);
		     }
		     else {
		       elem->first = $3->theExpression + elem->first;
		     }
		   }
		 }
               | label_symbol CASE expression USE
                 simul_case_statement_alternative
                 END CASE ';'
                 {
		   
		   $$ = $5;
		   for (simul_lst::iterator elem = $$->begin();
			elem != $$->end(); elem++) {
		     if (elem->first[0] == '~') {
		       elem->first = "~(" + $3->theExpression + ")" +
			 elem->first.substr(1,string::npos);
		     }
		     else {
		       elem->first = $3->theExpression + elem->first;
		     }
		   }
		 }
               ;

simul_case_statement_alternative :
                 WHEN choices ARROW sequence_of_simul_statements
                 {
		   $$ = new simul_lst;

		   /* These probably aren't going to be as robust
		      as they should be, but work for now */
		   string exp = "";
		   
		   if ($2->begin()->str() == "'0'") 
		     exp = "~";
		   		   
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = $4->begin();
			elem != $4->end(); elem++) {
		     $$->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		 }
               | simul_case_statement_alternative
                 WHEN choices ARROW sequence_of_simul_statements
                 {
		   $$ = $1;

		   string exp = "";
		   
		   if ($3->begin()->str() == "'0'") 
		     exp = "~";
		   
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = $5->begin();
			elem != $5->end(); elem++) {
		     $$->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		 }
               ;

simultaneous_procedural_statement :
                procedural_head is_symbol
                {
		  table->new_tb();
		  processChannels.clear();
		}
                sequence_of_declarative_items VBEGIN
                process_statement_part END procedural_end
                {
		  map<string,int>::iterator m(processChannels.begin());
		  while(m != processChannels.end()){
		    string name(m->first);
		    int type(m->second);
		    if(type&ACTIVE  && type&SENT||
		       type&PASSIVE && type&RECEIVED){
		      type |= PUSH;
		    }
		    if(type&ACTIVE  && type&RECEIVED ||
		       type&PASSIVE && type&SENT    ){
		      type |= PULL;
		    }
		    signals[cur_entity][name] |= type;
		    action(name+SEND_SUFFIX+"+")->type |= type;
		    action(name+SEND_SUFFIX+"-")->type |= type;
		    action(name+RECEIVE_SUFFIX+"+")->type |= type;
		    action(name+RECEIVE_SUFFIX+"-")->type |= type;
		    m++;
		  }
		  table->delete_tb();
		  $$ = 0;
		  if($6){
		    $$ = TERSmakeloop($6);
		    $$ = TERSrepeat($$,";");
		    char s[255];
		    strcpy(s, rmsuffix($1));
		    if (1) {
		      printf("Compiled process %s\n",s);
		      fprintf(lg,"Compiled process %s\n",s);
		    } 
		    else
		      emitters(outpath, s, $$);
		  }
		  if(tel_tb->insert(($1), $$) == false)
		    err_msg("duplicate process label '", $1, "'");
		}
               ;

procedural_end :
                 PROCEDURAL ';'
               | PROCEDURAL VID ';'
               ;

procedural_head :
                 PROCEDURAL
                 {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   $$ = copy_string(n);
		 }
               | label PROCEDURAL
                 {
		   $$ = copy_string($1);
		 }
 	       ;

sequence_of_declarative_items :

               | sequence_of_declarative_items procedural_declarative_item
               ;


procedural_declarative_item :
                 subprogram_declaration {}
               | subprogram_body {}
               | type_declaration {}
               | subtype_declaration {}
               | constant_declaration {}
               | variable_declaration {}
               | alias_declaration {}
               | attribute_declaration {}
               | attribute_specification {}
               | use_clause {}
               | group_template_declaration {}
               | group_declaration {}
               ;


/* Sequential Statements */

sequential_statement
               : wait_statement
                 {
		   /*if(__sensitivity_list__)
		     {
		       err_msg("wait statement must not appear",
			       "in a process statement which has",
			       "a sensitivity list");
		       __sensitivity_list__= 0;
		       }*/
		   TYPES ty; ty.set_str("wait");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | assertion_statement
                 {
		   TYPES ty; ty.set_str("assert");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | signal_assignment_statement
                 {
		   TYPES ty; ty.set_str("sig");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | variable_assignment_statement
                 {
		   TYPES ty; ty.set_str("var");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | procedure_call_statement
                 {
		   TYPES ty; ty.set_str("proc");
		   $$ = new pair<TYPES,telADT>(ty, 0);
		 }
               | if_statement
                 {
		   TYPES ty; ty.set_str("if");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | case_statement
                 {
		   TYPES ty; ty.set_str("case");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | loop_statement
                 {
		   TYPES ty; ty.set_str("loop");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | next_statement
                 {
		   TYPES ty; ty.set_str("next");
		   $$ = new pair<TYPES,telADT>(ty, 0);
		 }
               | exit_statement
                 {
		   TYPES ty; ty.set_str("exit");
		   $$ = new pair<TYPES,telADT>(ty, 0);
		 }
               | return_statement
                 {
		   TYPES ty; ty.set_str("rtn");
		   $$ = new pair<TYPES,telADT>(ty, 0);
		 }
               | null_statement
                 {
		   TYPES ty; ty.set_str("nil");
		   $$ = new pair<TYPES,telADT>(ty, 0);
		 }
               | report_statement
                 {
		   TYPES ty; ty.set_str("rep");
		   $$ = new pair<TYPES,telADT>(ty, 0);
		 }
               | communicationStatement
                 {
		   TYPES ty; ty.set_str("sig");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | assign_statements
                 {
		   TYPES ty; ty.set_str("sig");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | vassign_statements
                 {
		   TYPES ty; ty.set_str("sig");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | guardish_statement
                 {
		   TYPES ty; ty.set_str("wait");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               /* Added for AMS */ 
               | break_statement
                 {
		   TYPES ty; ty.set_str("break");
		   $$ = new pair<TYPES,telADT>(ty, $1);
		 }
               | rate_assignment
                  {
		    TYPES ty; ty.set_str("rate");
		    $$ = new pair<TYPES,telADT>(ty, $1);
		   }
               /* End Additions */
               ;

guardish_statement: guard_statement | guard_or_statement  | guard_and_statement
                  | await_statement | await_any_statement | await_all_statement
                  ;

guard_statement   : GUARD '(' andor_stmt ')' ';'
                    {
		       $$=Guard($3->str());
		    }
                  | GUARD '(' expression ')' ';'
                    {
		      $$ = Guard($3->theExpression);
		    }
                  | GUARD '(' andor_ams ')' ';'
                    {
		      $$ = Guard($3->str());
		    }
		   ;
guard_or_statement :GUARDOR  '('     or_stmts   ')' ';' {$$=Guard($3->str());};
guard_and_statement:GUARDAND '('    and_stmts   ')' ';' {$$=Guard($3->str());};
await_statement    :AWAIT    '(' probedChannel  ')' ';' {$$=Guard($3->str());};
await_any_statement:AWAITANY '(' target_terms   ')' ';' {$$=Guard($3->str());};
await_all_statement:AWAITALL '(' target_factors ')' ';' {$$=Guard($3->str());};

target_factors : probedChannel
               | probedChannel ',' target_factors
                 {
		   string s = $1->str() + '&' + $3->str();
		   $$->set_str(s);
		 }
               ;
target_terms   : probedChannel
               | probedChannel ',' target_terms
                 {
		   string s = $1->str() + '|' + $3->str();
		   $$->set_str(s);
		 }
              ;
probedChannel : target
                 {
		   $$ = new TYPES;
		   $$->set_data("bool");
		   $$->set_str(bool_relation(Probe($1->str())->str(), "'1'", "="));
		 }
               ;

and_stmts      : andor_stmt
               | andor_stmt ',' and_stmts
                 {
		   string s = $1->str() + '&' + $3->str();
		   $$->set_str(s);
		 }
               ;

or_stmts       : andor_stmt
               | andor_stmt ',' or_stmts
                 {
		   string s = $1->str() + '|' + $3->str();
		   $$->set_str(s);
		 }
               ;

andor_stmt :     expression ',' expression
                 {
		   pair<string,TYPES> TT1 = table->lookup($1->str());
		   pair<string,TYPES> TT3 = table->lookup($3->str());
		   $$ = new TYPES;
		   $$->set_data("bool");
		   if(table->lookup($1->get_string()).first == "qty" ||
		      table->lookup($3->get_string()).first == "qty")
		     cout << "WARNING: Use the guard(expr,relop,expr) syntax "
			  << "for quantities.";
		   else if(table->lookup($1->get_string()).first == "var" ||
			   table->lookup($3->get_string()).first == "var")
		     $$->set_str("maybe");
      		   else {
		     $$->set_str(bool_relation($1->str(), $3->str(), "="));
		   }
                 }
             
               ;

andor_ams : expression ',' INT ',' REAL
             {
	       
	       pair<string,TYPES> TT1 = table->lookup($1->str());
	       $$ = new TYPES;
	       $$->set_data("expr");
	       if (table->lookup($1->get_string()).first == "qty") {
		 
		 string relop;
		 switch ($3) {
		 case 0:
		   relop = ">";
		   break;
		 case 1:
		   relop = ">=";
		   break;
		 case 2:
		   relop = "<";
		   break;
		 case 3:
		   relop = "<=";
		   break;
		 case 4:
		   relop = "=";
		   break;
		 default:
		   relop = "=";
		   break;
		 }
		 $$->set_str($1->str() + relop + numToString((int)$5));
	       }
	       else
		   $$->set_str("maybe");
	     }
           ;

wait_statement
               : label_symbol WAIT ';' { $$ = 0; }
               | label_symbol WAIT sensitivity_clause ';' { $$ = 0; }
               | label_symbol WAIT condition_clause ';'
                 {
		   $$ = TERS(dummyE(), FALSE, 0, 0, TRUE, $3->str().c_str());
		 }
	       | label_symbol WAIT timeout_clause ';'  { $$ = $3; }
               | label_symbol WAIT sensitivity_clause condition_clause ';'
                 {$$ = 0; }
               | label_symbol WAIT sensitivity_clause timeout_clause ';'
                 {$$ = 0; }
               | label_symbol WAIT sensitivity_clause condition_clause
                 timeout_clause ';' {$$ = 0; }
               | label_symbol WAIT condition_clause timeout_clause ';'
                 {$$ = 0; }
               ;
condition_clause
               : UNTIL expression
                 {
		   $$ = $2;
		   string s;
		   if($2->data_type() != string("bool"))
		     s = '[' + $$->theExpression + ']';
		     else
		       s = '[' + $$->str() + ']';
		   
		   $$->set_str(s);
		 }
               ;
sensitivity_clause
               : ON sensitivity_list
               ;
timeout_clause
               : FOR expression
                 {
		   int l = 0,  u = INFIN;
		   const TYPELIST *tl = $2->get_list();
		   if(tl != 0){
		     TYPELIST::const_iterator I = tl->begin();
		     if(I != tl->end()) {
		       l = tl->front().second.get_int();
		       I++;
		       if(I!=tl->end())
			 u = tl->back().second.get_int();
		     }
		     $$ = TERS(dummyE(), FALSE, l, u, TRUE);
		   }
		   else{
		     $$ = NULL;
		   }
		 }
               ;
assertion_statement
               : ASSERT expression tralass ';'
               {
		 string expr;

		 if($2->data_type() != string("bool"))
		   expr = "[~(" + $2->theExpression + ")]";
		 else
		   expr = "[~(" + $2->str() + ")]";

		 string LL = "fail+";
		 actionADT a = action(LL);
		 makebooldecl(OUT|ISIGNAL, a, false, NULL);
		 $$ = TERS(a, FALSE, 0, 0, FALSE, expr.c_str());
	       }
               ;
/*
condition
               : expression
                 {
		   if($1->data_type() != string("bool")){
		     //err_msg("non-boolean expression used as condition");
		     warn_msg("non-boolean expression used as condition");
		     $$ = new TYPES("err", "err");
		     $$->set_str("maybe");
                   }
		   else
		     $$ = $1;
		 }
               ;
*/
tralass
               :
               | REPORT expression
               | SEVERITY VID
                 {}
               | REPORT expression SEVERITY VID
                 {}
               ;

communication
               : SEND
                 {
		   reqSuffix = SEND_SUFFIX;
		   ackSuffix = RECEIVE_SUFFIX;
		   join = dummyE();  //Branches of parallel send join here.
		   join->lower = join->upper = 0;
		   direction = SENT;
		 }
               | RECEIVE
                 {
		   reqSuffix = RECEIVE_SUFFIX;
		   ackSuffix = SEND_SUFFIX;
		   join = dummyE();  //Branches of parallel receive join here.
		   join->lower = join->upper = 0;
		   direction = RECEIVED;
		 }
               ;

communicationStatement
               : communication '(' parallelCommunication ')' ';' {$$ = $3;}
               | communication '(' target                ')' ';'
                 {
		   $$ = FourPhase($3->get_string());
		 }
               ;

parallelCommunication
               :                           fourPhase
               | parallelCommunication ',' fourPhase
                 {
		   $$ = TERScompose($1, $3, "||");
    		 }
               ;

fourPhase      : target ',' expression
                 {
		   $$ = FourPhase($1->get_string(),$3->get_string());
		 }
               ;

assign_statements : ASSIGNMENT '(' assign_statement ')' ';' { $$ = $3; }
                  ;

assign_statement : assign_stmt
                 | assign_statement ',' assign_stmt
                   {
		     $$ = TERScompose($1, $3, "||");
    		   }
                 ;

assign_stmt : target ',' expression ',' INT ',' INT
                   {
		     string s;
		     string sb;
		     TERstructADT x,y,z;
		     if($3->get_string() == "'1'") {
		       actionADT a = action($1->get_string() + '+');
		       $$ = TERS(a, FALSE, $5, $7, FALSE);
		     } else if($3->get_string() == "'0'") {
		       actionADT a = action($1->get_string() + '-');
		       $$ = TERS(a, FALSE, $5, $7, FALSE);
		     } else {
		       actionADT a = dummyE();
		       a->list_assigns = addAssign(a->list_assigns, $1->get_string(), $3->get_string());
		       $$ = TERS(a, FALSE, $5, $7, TRUE);
		     } /* else {
		       s = '[' + $3->get_string() + ']';
		       sb = "[~(" + $3->get_string() + ")]";
		       actionADT a = action($1->get_string() + '+');
		       actionADT b = action($1->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(b, FALSE, $5, $7, FALSE, sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		       }*/
		   }
               ;

/* Added for AMS */
rate_assignment :
                  mark '\'' VDOT ASSIGN simple_expression ';'
                  { 
		    if (table->lookup(*($1->begin())).first == "qty") {
		      $$ = Guard(*($1->begin()) + "'dot:=" + 
				 $5->theExpression);
		    }
		    else
		      $$ = 0;
		  }
		  /*
                  {

		    TERstructADT x,y;
		    actionADT a = action(*($1->begin()));
		    a->type = CONT + VAR;
		    x = TERS(a, FALSE, (int)$6, (int)$6, FALSE);
		    
		    string sign = $5;
		    y = Guard(*($1->begin()) + "'dot" + ":=" +
			       sign + numToString($6));
		    $$ = TERScompose(x,y,";");
		    }*/
/*
                | mark '\'' VDOT ASSIGN mark '(' neg REAL ',' neg REAL ')' ';'
                  {
		    TERstructADT x,y;
		    actionADT a = action(*($1->begin()));
		    a->type = CONT + VAR;
		    x = TERS(a, FALSE, (int)$8, (int)$11, FALSE);
		    string sign1 = $7;
		    string sign2 = $10;
		    y = Guard(*($1->begin()) + "'dot" + ":={" +
			       sign1 + numToString($8) + ";" +
			       sign2 + numToString($11) + '}');
		    $$ = TERScompose(x,y,";");
		  }
*/
                ;

/* End Additions */

vassign_statements : VASSIGNMENT '(' vassign_statement ')' ';'
                   {
		     $$ = $3;
		   }
               ;
vassign_statement : vassign_stmt
                 | vassign_statement ',' vassign_stmt
                   {
		     $$ = TERScompose($1, $3, "||");
    		   }
                 ;
vassign_stmt : target ',' expression ',' INT ',' INT
                   {
		     string s;
		     string sb;
		     TERstructADT x,y,z;
		     if($3->get_string() == "'1'") {
		       s = "[~" + $1->get_string() + ']';
		       sb = '[' + $1->get_string() + ']';
		       actionADT a = action($1->get_string() + '+');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,$5,$7,FALSE,sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		     } else if($3->get_string() == "'0'") {
		       s = '[' + $1->get_string() + ']';
		       sb = "[~" + $1->get_string() + ']';
		       actionADT a = action($1->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,$5,$7,FALSE,sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		     } else {
		       actionADT a = dummyE();
		       a->list_assigns = addAssign(a->list_assigns, $1->get_string(), $3->get_string());
		       $$ = TERS(a, FALSE, $5, $7, TRUE);
		     } /* else {
		       s = '[' + $3->get_string() + ']';
		       sb = "[~(" + $3->get_string() + ")]";
		       actionADT a = action($1->get_string() + '+');
		       actionADT b = action($1->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(b, FALSE, $5, $7, FALSE, sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		       }*/
		   }
/*
                   {
		     string s,sb,sc;
		     TERstructADT w,x,y,z;
		     if($3->get_string() == "'1'") {
		       s = "[~" + $1->get_string() + ']';
		       sb = '[' + $1->get_string() + ']';
		       actionADT a = action($1->get_string() + '+');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,$5,$7,FALSE,sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		     } else if($3->get_string() == "'0'") {
		       s = '[' + $1->get_string() + ']';
		       sb = "[~" + $1->get_string() + ']';
		       actionADT a = action($1->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,$5,$7,FALSE,sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		     } else {
		       s = "[(" + $3->get_string() + ")&(~" + $1->get_string()
			 +  ")]";
		       sb = "[(~(" + $3->get_string() + "))&(" +
			  $1->get_string() + ")]";
		       sc = "[((" + $3->get_string() + ")&(" +
                            $1->get_string() + "))|((~(" + $3->get_string() +
                            "))&(~(" + $1->get_string() + ")))]";
		       actionADT a = action($1->get_string() + '+');
		       actionADT b = action($1->get_string() + '-');
		       w = TERS(dummyE(),FALSE,$5,$7,FALSE,sc.c_str());
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(b, FALSE, $5, $7, FALSE, sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose($$,w,"|");
		       $$ = TERScompose(x,$$,";");
		     }
		   }
*/
               ;
signal_assignment_statement
               : target LE delay_mechanism waveform ';'
                 {   /*
		       if(check_type($1, $4))
		     err_msg("left and right sides of",
	                          " '<=' have different types", ";");*/
		   $4->set_str($4->theExpression);
		   $$ = signalAssignment($1,$4);
	         }
               | target LE waveform ';'
                 {
		   $3->set_str($3->theExpression);
		   $$ = signalAssignment($1,$3);
		 }
/*else {
		     $$ = TERSempty();
		     err_msg("input signal '", $1->get_string(),
			     "' used as the target of the assignment");
			     }			 */

                | label target LE delay_mechanism waveform ';'
                  {
		    $5->set_str($5->theExpression);
		    $$ = signalAssignment($2,$5);
		    /*
		    if(check_type($2, $5))
		      err_msg("left and right sides of '<=' have",
			      " different types", ";");
		    $$= TERSempty();*/
	          }

                | label target LE waveform ';'
                  {
		    $4->set_str($4->theExpression);
		    $$ = signalAssignment($2,$4);
		    /*
		    if(check_type($2, $4))
		      err_msg("left and right sides of '<=' have",
	                        " different types", ";");
		    $$= TERSempty();  */
                  }
               ;
waveform
               : waveform_element { $$= $1; }
               | waveform ',' waveform_element
                 {
		   $$ = $1;
		   /*
		   if(!check_type($1, $3))
		     $$= $1;
		   else if($1->isUnknown())
		     $$= $3; */
		 }
               | UNAFFECTED
                 {
                   $$ = new TYPES();
		   $$->set_str("unaffected");
		 }
               ;
waveform_element
               : expression
                 {   	
                   $$ = $1;
		   TYPES lower; lower.set_int(0);
		   TYPES upper; upper.set_int(0);
		   str_ty_lst ll(1, make_pair(string(), lower));
		   ll.push_back(make_pair(string(), upper));
		   $$->set_list(TYPELIST(ll));
                 }
               | expression AFTER expression
                 {
		   $$ = $1;
		   if($3->get_list() == 0){
		     TYPES upper; upper.set_int(INFIN);
		     str_ty_lst LL(1, make_pair(string(), *$3));
		     LL.push_back(make_pair(string(), upper));
		     $$->set_list(TYPELIST(LL));
		   }
		   else
		     $$->set_list($3->get_list());
		   delete $3;
		 }
               ;


variable_assignment_statement
               : label target ASSIGN expression ';'
                 {
		   /*
		   if(check_type($2, $4))
		     err_msg("left and right sides of ':='",
			     " have different types", ";");*/
		   if (table->lookup($2->get_string()).first == "qty") {
		     $$ = Guard($2->get_string() + ":=" + $4->theExpression);
		   }
		   else
		     $$ = 0;
		 }
               | target ASSIGN expression ';'
                 { /*
		   if(check_type($1, $3))
		     err_msg("left and right sides of ':='",
			     " have different types", ";");*/
		   if (table->lookup($1->get_string()).first == "qty") {
		     $$ = Guard($1->get_string() + ":=" + $3->theExpression);
		   }
		   else
		     $$ = 0;
		 }
               ;



/* Adding label_symbol or association list creates conflicts */

procedure_call_statement
               : name ';'  {}
//| name '(' association_list ')' ';' {}
               ;
if_statement
               : label_symbol IF expression THEN sequence_of_statements
                 elsifthen else_part END IF ';'
                 {
		   if($5->size() == 1 && $5->front().first.str() == "wait")
		     $$ = $5->front().second;
		   else {
		     telADT T5 = telcompose(*$5);
		     string EN;

		     //condition vs. expression
		     if($3->data_type() != string("bool"))
		       EN = $3->theExpression;
		     else
		       EN = $3->str();
		     
		     string ENC = my_not(EN);
		     string CC = '['+ EN +']';
		     actionADT AA = dummyE();
		     $$ = TERS(AA, FALSE, 0, 0, TRUE, CC.c_str());
		     $$ = TERScompose($$, T5, ";");
		     if($6){
		       ty_tel_lst::iterator BB;
		       for(BB = $6->begin(); BB != $6->end(); BB++){

			 if (BB->first.data_type() == string("bool"))
			   CC = '['+ logic(ENC, BB->first.str()) +']';
			 else
			   CC = '[' + logic(ENC, BB->first.theExpression)
			     + ']';
			     
			 telADT TT = TERS(dummyE(),FALSE,0,0,TRUE, CC.c_str());
			 telADT TTT = TERScompose(TT, BB->second, ";");
			 $$ = TERScompose($$, TERSrename($$, TTT), "|");
			 if (BB->first.data_type() == string("bool"))
			   ENC = logic(ENC, my_not(BB->first.str()));
			 else
			   ENC = logic(ENC, my_not(BB->first.theExpression));
		       }
		     }
		     CC = '['+ ENC +']';
		     telADT LL = TERS(dummyE(), FALSE,0,0,TRUE, CC.c_str());
		     telADT LLL = TERScompose(LL, $7, ";");
		     $$ = TERScompose($$, TERSrename($$, LLL), "|");
		   }
		   delete $5;
		 }
               | label_symbol IF expression THEN sequence_of_statements
                 elsifthen else_part END IF VID ';'
                 {
		   if($5->size() == 1 && $5->front().first.str() == "wait")
		     $$ = $5->front().second;
		   else {
		     telADT T5 = telcompose(*$5);
		     string EN;

		     //condition vs. expression
		     if($3->data_type() != string("bool"))
		       EN = $3->theExpression;
		     else
		       EN = $3->str();
		     
		     string ENC = my_not(EN);
		     string CC = '['+ EN +']';
		     actionADT AA = dummyE();
		     $$ = TERS(AA, FALSE, 0, 0, TRUE, CC.c_str());
		     $$ = TERScompose($$, T5, ";");
		     if($6){
		       ty_tel_lst::iterator BB;
		       for(BB = $6->begin(); BB != $6->end(); BB++){

			 if (BB->first.data_type() == string("bool"))
			   CC = '['+ logic(ENC, BB->first.str()) +']';
			 else
			   CC = '[' + logic(ENC, BB->first.theExpression)
			     + ']';
			     
			 telADT TT = TERS(dummyE(),FALSE,0,0,TRUE, CC.c_str());
			 telADT TTT = TERScompose(TT, BB->second, ";");
			 $$ = TERScompose($$, TERSrename($$, TTT), "|");
			 if (BB->first.data_type() == string("bool"))
			   ENC = logic(ENC, my_not(BB->first.str()));
			 else
			   ENC = logic(ENC, my_not(BB->first.theExpression));
		       }
		     }
		     CC = '['+ ENC +']';
		     telADT LL = TERS(dummyE(), FALSE,0,0,TRUE, CC.c_str());
		     telADT LLL = TERScompose(LL, $7, ";");
		     $$ = TERScompose($$, TERSrename($$, LLL), "|");
		   }
		   delete $5;
		   if($1 && $10->label && strcmp_nocase($1, $10->label)==false)
		     err_msg($10->label, " is not the if statement label.");
		 }
	       ;
elsifthen
               : { $$ = 0; }
               | elsifthen ELSIF expression THEN sequence_of_statements
                 {
		   if($1) $$ = $1;
		   else $$ = new ty_tel_lst;
		   $$->push_back(make_pair(*$3, telcompose(*$5)));
		   $$->back().first.theExpression = $3->theExpression;
		   delete $5; delete $3;
		 }
               ;
else_part
               : { $$ = 0;} 
               | ELSE sequence_of_statements
                 { $$ = telcompose(*$2); delete $2;}
               ;
label_symbol
               : { $$ = 0; }
               | label
               ;
label
               : VID ':' {  $$ = $1->label;	 }
               ;
sequence_of_statements
               : { $$ = 0; }
               | sequence_of_statements sequential_statement
                 {
		   if($1) $$ = $1;
		   else   $$ = new list<pair<TYPES,telADT> >;
		   $$->push_back(*$2);
		   delete $2;
                 }
               ;
case_statement
               : label_symbol CASE expression IS case_statement_alternative
                 END CASE ';'
                 {
		   $$ = 0;
		   ty_tel_lst::iterator b;
		   string SS = table->lookup($3->get_string()).first;
		   for(b = $5->begin(); b != $5->end(); b++){
		     telADT tt = 0;
		     if(SS == "var")
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE);
		     else {
		       string ss='['+bool_relation($3->str(),b->first.str())+']';
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE, ss.c_str());
		     }
		     telADT T = TERScompose(tt,TERSrename(tt,b->second),";");
		     if($$)
		       $$ = TERScompose($$, TERSrename($$, T), "|");
		     else
		       $$ = T;
		   }
//		   $$ = TERScompose(TERS(dummyE(),FALSE, 0, 0, TRUE), $$, "|");
		   delete $5;
		 }
               | label_symbol CASE expression IS case_statement_alternative
                 END CASE VID ';'
                 {
		   $$ = 0;
		   ty_tel_lst::iterator b;
		   string SS = table->lookup($3->get_string()).first;
		   for(b = $5->begin(); b != $5->end(); b++){
		     telADT tt = 0;
		     if(SS == "var")
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE);
		     else {
		       string ss='['+bool_relation($3->str(),b->first.str())+']';
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE, ss.c_str());
		     }
		     telADT T = TERScompose(tt,TERSrename(tt,b->second),";");
		     if($$)
		       $$ = TERScompose($$, TERSrename($$, T), "|");
		     else
		       $$ = T;
		   }
//		   $$ = TERScompose(TERS(dummyE(),FALSE, 0, 0, TRUE), $$, "|");
		   delete $5;
		 }
               ;
case_statement_alternative
               : WHEN choices ARROW sequence_of_statements
                 {
		   $$ = new list<pair<TYPES,telADT> >;
		   TYPES ty("uk", "bool");
		   if($2){
		     list<string> str_l;
		     for(list<TYPES>::iterator b = $2->begin();
			 b != $2->end(); b++)
		       str_l.push_back(b->str());
		     ty.set_str(logic(str_l, "|"));
		   }
		   else{
		     ty.set_grp(TYPES::Error);
		     ty.set_str("false");
		   }
		   $$->push_back(make_pair(ty, telcompose(*$4)));
		   delete $2; delete $4;
		 }
               | case_statement_alternative
                 WHEN choices ARROW sequence_of_statements
                 {
		   $$ = $1;
		   TYPES ty("uk", "bool");
		   if($3){
		     list<string> str_l;
		     for(list<TYPES>::iterator b = $3->begin();
			 b != $3->end(); b++)
		       str_l.push_back(b->str());
		     ty.set_str(logic(str_l, "|"));
		   }
		   else{
		     ty.set_grp(TYPES::Error);
		     ty.set_str("false");
		   }
		   $$->push_back(make_pair(ty, telcompose(*$5)));
		   delete $3; delete $5;
		 }
               ;
loop_statement
               : label_symbol iteration_scheme LOOP sequence_of_statements
                 END LOOP  ';'
                 {
		   telADT T4 = telcompose(*$4); delete $4;
		   if($2 == 0)
		     $$ = TERSempty();
		   else{
		     actionADT a = dummyE();
		     string ss =  "[" + $2->str() + "]";
		     TERstructADT t3 = TERS(a, FALSE, 0, 0, TRUE, ss.c_str());
		     TERstructADT loop_body =
		       TERScompose(t3, TERSrename(t3, T4), ";");

		     loop_body = TERSmakeloop(loop_body);
		
		     // create the tel structure for loop exit.
		     string s = '['+ my_not($2->str()) + ']';
		     TERstructADT loop_exit = TERS(dummyE(), FALSE, 0, 0,
						   TRUE, s.c_str());
		     // make conflict between loop body and loop exit
		     $$ = TERScompose(loop_body,TERSrename(loop_body,
							   loop_exit), "|");
		       		
		     $$ = TERSrepeat($$,";");
		   }
                 }
               | label_symbol iteration_scheme LOOP sequence_of_statements
                 END LOOP VID ';'
                 {
		   telADT T4 = telcompose(*$4); delete $4;
		   if($2 == 0)
		     $$ = TERSempty();
		   else{
		     string ss =  "[" + $2->str() + "]";
		     telADT t3=TERS(dummyE(),FALSE,0,0,TRUE, ss.c_str());
		     telADT loop_body = TERScompose(t3,TERSrename(t3, T4),";");
		     loop_body = TERSmakeloop(loop_body);
		
		     // create the tel structure for loop exit.
		     string s = '[' + my_not($2->str()) + ']';
		     telADT loop_exit= TERS(dummyE(),FALSE,0,0,TRUE,s.c_str());
		     // make conflict between loop body and loop exit
		     $$ = TERScompose(loop_body,TERSrename(loop_body,
							   loop_exit), "|");
		       		
		     $$ = TERSrepeat($$,";");

		     if($1 && strcmp_nocase($1, $7->label)==false)
		       err_msg($7->label, " is not the loop statement label.");
		   }
                 }
               ;

iteration_scheme
               :
                 {
		   $$ = new TYPES();
		   $$->set_str("true");
		   $$->set_data("bool");
		 }
               | WHILE expression
                 { 	
		   $$ = $2;
		   if(strcmp_nocase($2->data_type(), "bool")==false){
		     warn_msg("non-boolean expression used as condition");
		     $$->set_grp(TYPES::Error);
		     $$->set_str("maybe");
   		   }
		 }
               | FOR VID VIN discrete_range { $$ = 0; }
               ;
next_statement
               : label_symbol NEXT ';' {}
               | label_symbol NEXT VID ';'   { }
               | label_symbol NEXT WHEN expression ';'{  }
               | label_symbol NEXT VID   {}
		 WHEN expression ';' {}
               ;
exit_statement
               : label_symbol EXIT ';'  {}
               | label_symbol EXIT VID ';'  {}
               | label_symbol EXIT WHEN expression ';' {}
               | label_symbol EXIT VID {}
                 WHEN expression ';'   {}
               ;
return_statement
               : label_symbol RETURN ';' {}
               | label_symbol RETURN expression ';'  {}
               ;
null_statement
               : VNULL ';' {}
               | label VNULL ';' {}
               ;
report_statement
               : label_symbol REPORT expression ';'         {}
               | label_symbol REPORT expression SEVERITY VID ';'    {}
               ;
/* Added for AMS */
break_statement
               : label_symbol BREAK break_elements when_symbol ';' 
                 { $$=Guard(*$3); }
               | label_symbol BREAK when_symbol ';' { $$=0; }
               ;
when_symbol
               : {$$ = 0;}
               | WHEN expression
                 {$$ = Guard($2->theExpression);}
               ;

break_elements
               : break_element {$$ = $1;}
               | break_elements ',' break_element
                 {
		   if ($1 && $3) {
		     $$ = new string((*$1) + " & " + (*$3));
		     delete $1;
		     delete $3;
		   } else if ($1) {
		     $$ = $1;
		   } else {
		     $$ = $3;
		   }
		 } 
               ;           

break_element
               : FOR name USE name ARROW expression
                 {$$ = 0;}
               | name ARROW expression
                 {
		   $$ = new string($1->theExpression + ":=" + $3->theExpression); 
		 }
               ;
/* End Additions */

/* Interfaces and Associations */

interface_list
               : interface_declaration
               | interface_list ';' interface_declaration
                 {
		   if($1 && $3){
		     $$ = $3;
		     $$->splice($$->begin(), *$1);
		     delete $1;
		   }
		   else if($3)
		     $$ = $3;
		   else
		     $$ = $1;
		 }
               ;
interface_declaration
               : interface_constant_declaration { $$ = 0; }
               | interface_signal_declaration  { $$ = $1;  }
               | interface_variable_declaration { $$ = 0;  }
               | interface_file_declaration { $$ = 0; }
               | interface_unknown_declaration { $$= $1;  }
               /* Added for AMS */
               | interface_terminal_declaration { $$ = 0;}
               | interface_quantity_declaration { $$ = 0;}
               ;
/* Added for AMS */
interface_terminal_declaration
               : TERMINAL identifier_list ':' subnature_indication {}
               ;

interface_quantity_declaration
               : QUANTITY identifier_list ':' in_out_symbol
                 subtype_indication assign_exp
                 {
		   $$ = new list<pair<string, int> >;
		   for(list<string>::iterator b=$2->begin();b!=$2->end(); b++){
		     int init = 0;
		     if($6->get_string() == "'1'")
		       init = 1;
		     string LL = (*b);
		     actionADT a = action(LL);
		     if(strcmp_nocase($4, "in")){
		       makebooldecl(IN, a, init, NULL);
		       $$->push_back(make_pair(LL, IN));
		     }
		     else{
		       makebooldecl(OUT, a, init, NULL);
		       $$->push_back(make_pair(LL, OUT));
		     }
		   }
		 }
               ;

in_out_symbol
               : VIN { $$ = "in";}
               | VOUT { $$ = "out";}
               | {}
               ;

/* End Additions */

interface_constant_declaration
               : CONSTANT identifier_list ':' subtype_indication assign_exp
                 {}
               | CONSTANT identifier_list ':' VIN subtype_indication assign_exp
                 {}
               ;
interface_signal_declaration
              : SIGNAL identifier_list ':' subtype_indication bus_symbol
                assign_exp
                {
		  //$4->set_obj("in");
		  $$ = new list<pair<string, int> >;
		  for(list<string>::iterator b=$2->begin();b!=$2->end(); b++){
		    int init = 0;
		    if($6->get_string() == "'1'")
		      init = 1;
		    string LL((*b));
		    actionADT a = action(LL);
		    makebooldecl(IN, a, init, NULL);
		    $$->push_back(make_pair(LL, IN));
		  }
		
		  /* for a signal of array
		     else{
		       TYPELIST *index =
			     $4->get_list()->value().second->get_list();
			   index->iteration_init();
			   int len = index->value().second->get_int();
			   index->next();
			   len = len - index->value().second->get_int();
			   int char_len = strlen($$->value().first);
			   char *name = 0;
			   for(int i = 0; i < len; i++)
			     {
			       name = new char[char_len+10];
			       sprintf(name, "%s__%d__+",
				       $$->value().first, i);
			     }
#ifdef DDEBUG
			   assert(name);
#endif
			   actionADT a = action(name, strlen(name));
			   delete [] name;
			
			   makebooldecl(IN, a, init, NULL);
			 }
		
		       $$->next();
		       }*/
		 }
              | SIGNAL identifier_list ':' mode subtype_indication
	        bus_symbol assign_exp
                {
		  /*if(strcmp_nocase($4, "in") == 0)
		    $5->set_obj("in");
		  else if(strcmp_nocase($4, "out") == 0)
		    $5->set_obj("out");
		  else
		  $5->set_obj("in");*/
		  $$ = new list<pair<string, int> >;
		  for(list<string>::iterator b=$2->begin();b!=$2->end(); b++){
		    int init = 0;
		    if($7->get_string() == "'1'")
		      init = 1;
		    string LL = (*b);
		    actionADT a = action(LL);
		    if(strcmp_nocase($4, "in")){
		      makebooldecl(IN, a, init, NULL);
		      $$->push_back(make_pair(LL, IN));
		    }
		    else{
		      makebooldecl(OUT, a, init, NULL);
		      $$->push_back(make_pair(LL, OUT));
		    }
		  }
		
		   /*
		   $$->iteration_init();
		   while($$->end() == false)
		     {
		       TYPES *ty = new TYPES($5);
		       $$->value().second = ty;
		
		       int init = 0;
		       if($7)
			 init = !strcmp_nocase($7->get_string(), "'1'");
		
		       if($5->grp_id() != TYPES::Array)
			 {
			   char *name = copy_string($$->value().first);
			   actionADT a = action(name, strlen(name));
			   delete [] name;
			   if(strcmp_nocase($4, "out") == 0)
			     makebooldecl(OUT, a, init, NULL);
			   else
			     makebooldecl(IN, a, init, NULL);
			 }
		       else
			 {
			   TYPELIST *index =
			     $5->get_list();//->value().second->get_list();
			   index->iteration_init();
			   int len = index->value().second->get_int();
			   index->next();
			   len = len - index->value().second->get_int();
			   int char_len = strlen($$->value().first);
			   char *name = 0;
			   for(int i = 0; i < len; i++)
			     {
			       name = new char[char_len+10];
			       sprintf(name, "%s__%d__+",
				       $$->value().first, i);
			     }
#ifdef DDEBUG
			   assert(name);
#endif
			   actionADT a = action(name, strlen(name));
			   delete [] name;
			   if(strcmp_nocase($4, "out") == 0)
			     makebooldecl(OUT, a, init, NULL);
			   else
			     makebooldecl(IN, a, init, NULL);
			 }
			 		
		       $$->next();
		       }*/
		 }
               ;
interface_variable_declaration
               : VARIABLE identifier_list ':' subtype_indication
                 assign_exp
                 {}
               | VARIABLE identifier_list ':' mode subtype_indication
                 assign_exp
                 {}
               ;


bus_symbol
               :
               | BUS
               ;
assign_exp
               : { $$ = new TYPES; $$->set_str("'0'"); }
               | ASSIGN expression
                 { $$ = $2; }
               ;

interface_unknown_declaration
             : identifier_list ':' subtype_indication bus_symbol assign_exp
               {
		 //$3->set_obj("in");
		 $$ = new list<pair<string,int> >;
		 for(list<string>::iterator b=$1->begin(); b!=$1->end(); b++){
		   int init = 0;
		   if($5->str() == "'1'")
		     init = 1;
		   const string LL = (*b);
		   actionADT a = action(LL);
		   makebooldecl(IN, a, init, NULL);
		   $$->push_back(make_pair(LL, IN));
		 }
		 delete $1;
	       }
             | identifier_list ':' mode subtype_indication
	       bus_symbol assign_exp
               {
		 $$ = new list<pair<string,int> >;
		 list<string> *sufficies = new list<string>;
		 int type(IN);
		 string assigned($6->get_string());
		 delayADT myBounds;
		 int rise_min(0), rise_max(0), fall_min(-1), fall_max(-1);
		 if("init_channel" == assigned ||
		    "active"  == assigned ||
		    "passive" == assigned ){
		   sufficies->push_back(SEND_SUFFIX);
		   sufficies->push_back(RECEIVE_SUFFIX);
		   TYPELIST arguments($6->get_list());
		   TYPELIST::iterator i(arguments.begin());
		   while(i!=arguments.end()){
		     if("timing"==i->second.get_string()){
		       if("sender"==i->first){
			 type |= SENT;
		       }
		       if("receiver"==i->first){
			 type |= RECEIVED;
		       }
		       TYPELIST bounds(i->second.get_list());
		       TYPELIST::iterator j(bounds.begin());
		       int pos(1);
		       while(j != bounds.end()){
			 string formal(j->first);
			 int actual(j->second.get_int());
			 if("rise_min"==formal || "lower"==formal ||
			    formal.empty() && 1==pos){
			   rise_min = actual;
			 }
			 else if("rise_max"==formal || "upper"==formal ||
				 formal.empty() && 2==pos){
			   rise_max = actual;
			 }
			 else if("fall_min"==formal || "lower"==formal ||
				 formal.empty() && 3==pos){
			   fall_min = actual;
			 }
			 else if("fall_max"==formal || "upper"==formal ||
				 formal.empty() && 4==pos){
			   fall_max = actual;
			 }
			 j++;
			 pos++;
		       }
		     }
		     i++;
		   }
		   if("active"==assigned){
		     type |= ACTIVE;
		   }
		   else if("passive"==assigned){
		     type |= PASSIVE;
		   }
		 }
		 else{
		   sufficies->push_back("" );
		   if(strcmp_nocase($3,"out")){
		     type = OUT;
		   }
		 }
		 if(fall_min<0){
		   fall_min = rise_min;
		 }
		 if(fall_max<0){
		   fall_max = rise_max;
		 }
		 assigndelays(&myBounds,
			      rise_min,rise_max,NULL,
			      fall_min,fall_max,NULL);
		 int init = 0;
		 if($6->str() == "'1'"){
		   init = 1;
		 }
		 for(list<string>::iterator b=$1->begin(); b!=$1->end(); b++){
		   $$->push_back(make_pair((*b), type));
		   list<string>::iterator suffix=sufficies->begin();
		   while ( suffix != sufficies->end() ) {
		     makebooldecl(type,action((*b)+*suffix),init,
				  &myBounds);
		     suffix++;
		   }
		 }
		 delete sufficies;
		 delete $1;
	       }
             ;
interface_file_declaration
               : VFILE identifier_list ':' subtype_indication_list
                 {}
               ;
subtype_indication_list
               : subtype_indication
                 {   $$= $1;   }
               | subtype_indication_list ',' subtype_indication
                 {   $$= $3;   }
               ;
mode
               : VIN { $$= "in"; }| VOUT { $$= "out"; }
               | INOUT { $$= "out"; } | BUFFER {$$= "out"; }
               | LINKAGE { $$= "linkage"; }
               ;
gen_association_list
               : '(' gen_association_list_1 ')'
                 {
		   $$ = $2;
      		 }
               ;
gen_association_list_1
               : gen_association_element
               | gen_association_list_1 ',' gen_association_element
                 {
		   
		   $$ = $1;
		   $$->front().second.theExpression += ";" +
		     $3->front().second.theExpression;
		   $$->combine(*$3);
		   delete $3;
		 }
               ;
gen_association_element
               : expression
                 {
		   $$ = new TYPELIST(make_pair(string(), *$1));
		   $$->front().second.theExpression = $1->theExpression;
		   delete $1;
		 }
               | discrete_range1 {  $$ = $1 ;  }
               | formal_part ARROW actual_part
                 {
 		   $$ = new TYPELIST(make_pair(*$1     , *$3));
		   delete $1;  delete $3;
		 }
               | OPEN
                 {
		   //$$= new TYPES("uk","uk", "uk");
		 }
               ;
association_list
               : association_element
                 {
		   $$ = new list<pair<string,string> >;
                   $$->push_back(*$1);   delete $1;
		 }
               | association_list ',' association_element
                 { $$ = $1;   $$->push_back(*$3); delete $3; }
               ;
association_element
               : actual_part
                 { $$ = new pair<string,string>(string(), $1->str()); delete $1; }
               | formal_part ARROW actual_part
                 {
		   $$ = new pair<string,string>(*$1, $3->str());
		   delete $1;  delete $3;
		 }
               ;
formal_part
               : name { $$ = new string($1->str());  delete $1; }
               ;
actual_part
               : expression
               | OPEN  { $$ = new TYPES; }
               ;

/* Expressions */

expression
               : relation andexpr
                 {
		   $$ = $1;
		   $$->set_obj("exp");
		   $$->set_str(logic($1->str(), $2->str(), "&"));
		   $$->theExpression = $$->theExpression + '&' +
		     $2->theExpression;
		   delete $2;
		 }
               | relation orexpr
                 {
		   $$ = $1;
		   $$->set_obj("exp");
		   $$->set_str(logic($1->str(), $2->str(), "|"));
		   $$->theExpression = $$->theExpression + '|' +
		     $2->theExpression;
		   delete $2;
		 }
               | relation xorexpr
                 {
		   string one_bar=("~("+$1->str())+')';
		   string two_bar=("~("+$2->str())+')';
		   $$->set_obj("exp");
		   $$->set_str(logic(logic($1->str(), two_bar, "&"),
		     logic(one_bar, $2->str(), "&"), "|"));
		   delete $2;
		   //$$= new TYPES1(check_logic($1, $2, "'xor'" ));
                   //$$->setTers(TERSempty());
                 }
               | relation NAND relation
                 {
		   $1->set_str(("~("+$1->str())+')');
		   $3->set_str(("~("+$3->str())+')');
		   $$->set_obj("exp");
		   $$->set_str(logic($1->str(), $3->str(), "|"));
		   delete $3;
		   //$$= new TYPES1(check_logic($1, $3, "'nand'"));
                   //$$->setTers( TERSempty());
                 }
               | relation NOR relation
                 {
		   $1->set_str(("~("+$1->str())+')');
		   $3->set_str(("~("+$3->str())+')');
		   $$->set_obj("exp");
		   $$->set_str(logic($1->str(), $3->str(), "&"));
		   delete $3;
		   //$$= new TYPES1(check_logic($1, $3, "'nor'"));
                   //$$->setTers( TERSempty());
                 }
               | relation XNOR relation
                 {
		   string one_bar=("~("+$1->str())+')';
		   string three_bar=("~("+$3->str())+')';
		   $$->set_obj("exp");
		   $$->set_str(logic(logic($1->str(), $3->str(), "&"),
		     logic(one_bar, three_bar, "&"), "|"));
		   delete $3;
		   /* NOTE: should $1 be deleted also??? */
		   //$$= new TYPES1(check_logic($1, $3, "'xnor'"));
                   //$$->setTers(TERSempty());
                 }
               | relation { $$ = $1; }
               ;
andexpr
               : andexpr AND relation
                 {
		   $$ = $1;
		   $$->set_obj("exp");
		   $$->set_str(logic($1->str(), $3->str(), "&"));
		   $$->theExpression = $$->theExpression + '&' +
		     $3->theExpression;
		   delete $3;
                 }
               | AND relation
                 {
		   $$ = $2;
		 }
               ;
orexpr
               : orexpr OR relation
                 {
		   $$ = $1;
		   $$->set_obj("exp");
		   $$->set_str(logic($1->str(), $3->str(), "|"));
		   $$->theExpression = $$->theExpression + '|' +
		     $3->theExpression;
		   delete $3;
                 }
               | OR relation { $$ = $2;  }
               ;
xorexpr
               : xorexpr XOR relation
                 {
		   string one_bar=("~("+$1->str())+')';
		   string three_bar=("~("+$3->str())+')';
		   $$->set_obj("exp");
		   $$->set_str(logic(logic($1->str(), three_bar, "&"),
		     logic(one_bar, $3->str(), "&"), "|"));
		   delete $3;
		   //$$= new TYPES1(check_logic($1, $3, "'xor'"));
		 }
               | XOR relation {  $$ = $2;   }
               ;

relation   
               : shift_expression { $$ = $1; }
               | PROBE '(' target ')' {
		 $$ = Probe($3->str());
	       }
               | shift_expression relational_operator shift_expression
                 {
		   pair<string,TYPES> TT1 = table->lookup($1->str());
		   pair<string,TYPES> TT3 = table->lookup($3->str());
		   /*if(TT1.first == "err" || TT3.first == "err"){
		     if(TT1.first == "err")
		       err_msg("undeclared identifier '",$1->get_string(),"'");
		     if(TT3.first == "err")
		       err_msg("undeclared identifier '",$3->get_string(),"'");
		   }
		   else*/{
		     $$ = new TYPES;
		     if (table->lookup($1->get_string()).first != "qty" &&
			 table->lookup($3->get_string()).first != "qty")
		       $$->set_data("bool");
		     else
		       $$->set_data("expr");
		     
		     if(table->lookup($1->get_string()).first == "var" ||
			table->lookup($3->get_string()).first == "var")
		       $$->set_str("maybe");
       		     else
		       //$$->set_str($1->str() + $2 + $3->str());
		       $$->set_str(bool_relation($1->theExpression, $3->theExpression, $2));
		     $$->theExpression += $1->theExpression + $2 + $3->theExpression;
		     //cout << "@relation " << $$->theExpression << endl;
		   }
		 }
               ;
relational_operator
               : '=' { $$ = "="; }  | NEQ { $$ = "/="; } | '<' { $$ = "<"; }
               | LE  { $$ = "<="; } | '>' { $$ = ">"; }  | GE  { $$ = ">=";}
               ;
shift_expression
               : simple_expression {$$ = $1;}
               | simple_expression shift_operators simple_expression
                 {  /*
		   if( !$3->isInt() ) {
                     TYPES *ty= new TYPES("ERROR", "ERROR", "ERROR");
		     $$= new TYPES1(ty);
		     err_msg("expecting an integer"); }
		   else
		     $$= $1;*/
		 }
               ;
shift_operators
               : SLL | SRL | SRA | SLA | ROL | ROR
               ;
simple_expression
               : terms
               | sign terms
                {
		  $$ = $2;
		  $$->theExpression = $1 + $$->theExpression;
		}
               ;
sign
               : '+' {$$ = "+";} %prec MED_PRECEDENCE
               | '-' {$$ = "-";} %prec MED_PRECEDENCE
               ;
terms
               : term {$$ = $1;}
               | terms adding_operator term
                 {
		   $$ = $1;
		   $$->theExpression = $$->theExpression + $2
		     + $3->theExpression;
		 }
               ;
adding_operator
               : '+' {$$ = "+";} | '-' {$$ = "-";} | '&' {$$ = "&";}
               ;
term
               : term multiplying_operator factor
                 {
		   $$ = $1;
		   $$->theExpression = $$->theExpression + $2 +
		     $3->theExpression;
		 }
               | factor      { $$ = $1;  }
               ;
multiplying_operator
               : '*' {$$ = "*";} | '/' {$$ = "/";}
               | MOD {$$ = "%";} | REM {$$ = "rem";}
               ;
factor
               : primary
               | primary EXPON primary
               | ABSOLUTE primary        { $$ = $2; }
               | NOT  primary
                 {
		   $2->set_str(("~("+$2->theExpression)+')');
		   $$ = $2;
		   $$->theExpression = "~(" + $2->theExpression + ")";
		 }
               ;
primary
               : name
                   {
		     $$=$1;
		     if($$->get_string()=="true" || $$->get_string()=="false"){
		       $$->set_data("bool");
		     }
		   }
//               | attribute_name { yywarning("Attributes not supported."); }
               | literal
               | aggregate
               | qualified_expression
                 { yyerror("Qualified expressions not supported."); }
               | allocator
                 { yyerror("Allocators not supported."); }
               | '(' expression ')'
                 { $$ = $2; }
               ;
allocator
               : NEW mark allocator1      {}// $$= search(table,$2->label); }
               | NEW mark mark allocator1 {}// $$= search(table,$2->label); }
               | NEW qualified_expression { $$= $2; }
               ;
allocator1
               :
               | gen_association_list {}
               ;
qualified_expression
               : mark '\'' '(' expression ')'   { $$= $4; }
               | mark '\'' aggregate            { $$= $3; }
               ;
name
               : mark
                 {
		   $$ = new TYPES();
		   $$->set_str($1->front());
		   $$->theExpression += $1->front();
		 
                 }/*
               | mark '\'' ABOVE '(' REAL ')'
                 {
		   $$ = new TYPES();
		   $$->set_str($1->front());
		   $$->theExpression += $1->front() + ">=" + numToString($5);
		 }
               | mark '\'' ABOVE '(' '-' REAL ')'
                 {
		   $$ = new TYPES();
		   $$->set_str($1->front());
		   $$->theExpression += $1->front() + ">="
		     + numToString($6 * -1);
		     }*/
               | mark '\'' ABOVE '(' simple_expression ')'
                 {
		   $$ = new TYPES();
		   $$->set_str($1->front());
		   $$->theExpression += $1->front() + ">="
		     + $5->theExpression;
		 }
  
               | name2  { $$= $1; }
               ;
mark
               : VID
                 { $$ = new list<string>;  $$->push_back($1->label); }
               | selected_name

               ;
selected_name
               : mark '.' suffix
                 { $$ = $1;  $$->push_back($3); }
               ;
name2
               : name3 {  $$= $1; }
               | attribute_name { $$= $1; }
               ;
name3
               : STRLIT
                 {
		   $$= new TYPES;
		   //$$->set_str(strtok($1, "\""));
      		   //$$->theExpression += $1;
		   
		   char *chptr;
		   chptr = strpbrk($1, "\"");
		   chptr = strtok(chptr+1, "\"");
		   $$->set_str(chptr);
		   $$->theExpression += chptr;
		   //cout << "@STRLIT " << $$->theExpression << endl;
      		 }
               | insl_name { $$= $1; }
               ;
suffix
               : VID { $$= $1->label; }
               | CHARLIT { $$ = $1;  }
               | STRLIT { $$= $1; }
               | ALL  { $$ = "all"; }
               ;



/* Lots of conflicts */

attribute_name
              : mark '\'' idparan
                {
		  $$ = new TYPES();
		  $$->set_str($1->front());
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search(table,$1->label);
                  //$$= t; $$->setstring($1->label);
		}
              | name2 '\'' idparan
                {
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search( table, $1->name );
		  //$$= t;
		}
              | name signature '\'' idparan
                {
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search( table, $1->name2->label );
		  //$$= t;
		}
              ;
idparan
              : VID  { $$= $1->label; }
              | VID '(' expression_list ')'
                { $$= $1->label; }
              | RANGE { $$= "range"; }
              | RANGE '(' expressions ')' {}
              ;

expression_list
              : expression {}
              | expression_list ',' expression {}
              ; 

expressions
              : {}
              | expressions ',' expression
              ;

insl_name
              : mark gen_association_list
                {
		  /*($$ = table->lookup($1->front());
		  if($$ == 0)
		    {
		      err_msg("'", $1->front(), "' undeclared");	
		      $$ = new TYPES("err", "err");
		      }*/
		  $$ = new TYPES;
		  $$->set_list($2);
		  $$->set_str($1->front());
		  if ($1->front()=="delay") {
		    $$->theExpression =
		      "{" + $2->front().second.theExpression + "}";
		  } else if ($1->front()=="selection") {
		    // ZHEN(Done): extract the first integer of $2 and subtract 1 from it
		    // "uniform(0," + $2->front().second.theExpression + ")";
		    //$$->theExpression ="uniform(0," + intToString(atoi((($2->front().second.theExpression).substr(0,($2->front().second.theExpression).find(";")).c_str()))-1) + ")";
		    // "floor(uniform(0," + $2->front().second.theExpression + "))";
		    $$->theExpression ="floor(uniform(0," + intToString(atoi((($2->front().second.theExpression).substr(0,($2->front().second.theExpression).find(";")).c_str()))) + "))";
		    $$->set_str($$->theExpression);
		  } else if (($1->front()=="init_channel") ||
		             ($1->front()=="active") ||
                             ($1->front()=="passive") ||
                             ($1->front()=="timing")) {
		  } else {
		    $$->theExpression =
		      "BIT(" + $1->front() + "," + $2->front().second.theExpression + ")";
		    $$->set_str($$->theExpression);
		    //cout << "$$->theExpression " << $$->theExpression << endl;
		  }
		  /*
		  char s[200] = {'\0'};
		  $2->iteration_init();
		  if($2->value().second->grp_id() == TYPES::Integer)
		    sprintf(s, "%s__%ld__", $1->front().c_str(),
			    $2->value().second->get_int());
			    $$->set_str(s);*/
		}
              | name3 gen_association_list
                {
		  $$ = new TYPES($1);
		  //$$->formal_list= NULL;
                  //$$= $1;
                  $$->set_list($2);
		}
              ;
literal
              : INT
                {
		  $$ = new TYPES("uk", "integer");
		  $$->set_grp(TYPES::Integer);
		  $$->set_int($1);
		  $$->theExpression += intToString($1);
	          $$->set_str($$->theExpression);
                }
              | REAL
                {
                  $$= new TYPES("uk", "real");
		  $$->set_grp(TYPES::Real);
                  $$->set_flt($1);
		  $$->theExpression += numToString($1);
                }
              | BASEDLIT
                {
		  $$ = new TYPES("uk", "integer");
		  $$->set_grp(TYPES::Integer);
		  $$->set_int($1);
		  $$->theExpression += intToString($1);
                }
              | CHARLIT
                {		
		  $$ = new TYPES("chr", "uk");
		  $$->set_string($1);
		  $$->theExpression += $1;
		  // cout << "@CHARLIT " << $$->theExpression << endl;
		}
              | BITLIT
                {
                  $$= new TYPES("uk", "BIT_VECTOR");
                  $$->set_string($1);
		  $$->theExpression += $1;
                }
              | VNULL {}
              | physical_literal_no_default { $$= $1; }
	      ;
physical_literal
              : VID { $$ = new TYPELIST(make_pair($1->label, TYPES())); }
              | INT VID
                {
		  TYPES t("cst", "int");
		  t.set_int($1);
		  $$ = new TYPELIST(make_pair($2->label, t));
		}
              | REAL VID
                {
		  TYPES t("cst", "flt");
		  t.set_flt($1);
		  $$ = new TYPELIST(make_pair($2->label, t));
		}
              | BASEDLIT VID
                {
		  TYPES t("cst", "int");
		  t.set_int($1);
		  $$ = new TYPELIST(make_pair($2->label, t));
		  //TYPES *temp= search(table,$2->label);
		  //$$= new TYPES("CONST", "uk", "uk");
		}
              ;
physical_literal_no_default
              : INT VID
                {
		  //$$ = table->lookup($2->label);
		  $$ = new TYPES("cst", "int");
		  $$->set_int($1);
		}
              | REAL VID
                {
		  $$ = new TYPES("cst", "flt");
		  $$->set_flt($1);
		  /*
		  TYPES *temp = table->lookup($2->label);
		  if(temp == NULL)
		    {
		      err_msg("'", $2->label, "' was not declared before;");
		      temp = new TYPES("err", "err", "err");
		    }
		  $$= new TYPES("CONST", temp->datatype(), temp->subtype());*/
		 }
              | BASEDLIT VID
                {
		  $$ = new TYPES("cst", "int");
		  $$->set_int($1);
		  /*
		  TYPES *temp = table->lookup($2->label);
		  if(temp == NULL)
		    {
		      err_msg("'", $2->label, "' was not declared before;");
		      temp = new TYPES("err", "err", "err");
		    }
		  $$= new TYPES("CONST", temp->datatype(), temp->subtype() );*/
		}
              ;	
aggregate
              : '(' element_association ',' elarep ')'
                {
		  /*if(strcmp_nocase($2->datatype(), $4->datatype())
		     && !($2->isError() || $4->isError()))
		    {
		      $$= new TYPES("err","err", "err");
		      err_msg("wrong types between '(' and ')'");
		    }
		  else
		  */
		    $$= $4;
		}
              | '(' choices ARROW expression ')'
                {
		  //if($2->isUnknown())
		    $$= $4;
		    /*
		  else if(($2->datatype() == $4->datatype()) &&
			  !($2->isError() || $4->isError()))
		    $$= $2;
		  else
		    err_msg("different types on left and right sides",
			    "of '=>'", ";");
		  $$= new TYPES("err", "err", "err");
		    */
		}
              ;
elarep
              : element_association
              | elarep ',' element_association
/*
                {
		  if(strcmp_nocaseyy( $1->datatype(), $3->datatype())
		     && !($1->isError() || $3->isError()))
		    {
		      $$= new TYPES("err", "err", "err");
		      err_msg("wrong types between '(' and ')'");
		    }
		  else
		    $$= $1;
		}
*/
              ;
element_association
              : expression
              | choices ARROW expression
                {
		  //if($1->isUnknown())
		    $$= $3;
		    /*
		  else if(($1->datatype()==$3->datatype()) &&
			  !($1->isError() || $3->isError()))
		    $$= $1;
		  else
		    err_msg("different types on left and right sides",
			    " of '=>' ", ";");
		  $$= new TYPES("err", "err", "err");
	      */
		}
              ;
choices
              : choices '|' choice
                {
		  $$ = $1;
		  if($$ && $3->grp_id() != TYPES::Error){
		    if($$->back().grp_id() != $3->grp_id()){
		      err_msg("type mismatch");
		      delete $1;
		      $$ = 0;
		    }
		    else
		      $$->push_back(*$3);
		  }
		}
              | choice
                {
		  $$ = 0;
		  if($1->grp_id() != TYPES::Error){
		    $$ = new list<TYPES>;
		    $$->push_back(*$1);
		  }
		  delete $1;
		}
              ;
choice
              : simple_expression { $$= $1; }
              | discrete_range1   { $$ = new TYPES(); }
              | OTHERS  { $$= new TYPES(); $$->set_str("OTHERS"); }
              ;
recursive_errors
              : RECURSIVE_ERROR
              ;





/*****************************************************************************
                             ---CSP PARSER---
*****************************************************************************/




csp_compiler
                : csp_compiler basic_module {} //*$$ = $2;*/ }
                | basic_module
		;
basic_module
                : MODULE VID ';'
                  {
		    new_action_table($2->label);
		    new_event_table($2->label);
		    new_rule_table($2->label);
		    new_conflict_table($2->label);
		    tel_tb = new tels_table;
		  }
                  decls constructs ENDMODULE
                  {		
		    map<string,int> sig_list;
		    if($5)
		      for(list<pair<string,int> >::iterator b = $5->begin();
			  b != $5->end(); b++)
			sig_list.insert(*b);
		    if($6)
		      for(list<pair<string,int> >::iterator b = $6->begin();
			  b != $6->end(); b++)
			if(sig_list.find(b->first) == sig_list.end())
			  sig_list.insert(make_pair(b->first, OUT));
		    tel_tb->insert(sig_list);
		    tel_tb->set_id($2->label);
		     tel_tb->set_type($2->label);
		    (*open_module_list)[$2->label] = make_pair(tel_tb, false);
		  }
                | INCLUDE filename ';'
                  {
		    cur_file = *$2;
		    switch_buffer(*$2);
		    delete $2;
		  }
                ;
filename
                : path_name '/' file_name
                  {
		    string f_name;
		    if(*$1 != "error"){
		      if (toTEL)
			f_name = *$1 + '/' + $3 + ".hse";
		      else
			f_name = *$1 + '/' + $3 + ".csp";
		      $$ = new string(f_name);
		      delete $1;
		    }
		    else
		      $$ = $1;
		  }
                | file_name
                  {
		    string f_name;
		    if (toTEL)
		      f_name = cur_dir + '/' + string($1) + ".hse";
		    else
		      f_name = cur_dir + '/' + string($1) + ".csp";
		    $$ = new string(f_name);
		  }
                ;
path_name
                : path_name '/' VID
                  {
		    string ss = *$1 + '/' + $3->label;
		    $$ = new string(ss);
		  }
                | VID
                  { $$ = new string($1->label); }
                | '~'
                  {
		    char *s = getenv("HOME");
		    if(s)
		      $$ = new string(s);
		    else
		      {
			err_msg("home directory not found");
			$$ = new string("error");
		      }
		  }
                | { $$ = &cur_dir; }
                ;

file_name
                : VID
                  { $$ = copy_string(((*name_mapping)[$1->label]).c_str()); }
                | '*'
                  { $$ = "*"; }
                ;
decls		
                : decls decl
                  {
		    if($1 && $2)
		      {
			$1->splice($1->begin(), *$2);
			$$ = $1;
		      }
		    else if($1)
		      $$ = $1;
		    else
		      $$ = $2;
		  }
                | { $$ = 0; }
		;

decl
                : delaydecl
                | definedecl
                | booldecl
//                | portdecl
		;

delaydecl
                : ATACS_DELAY VID '=' delay ';'
                  {
		    makedelaydecl($2,$4);
		    $$ = 0;
		  }
		;

definedecl
                : ATACS_DEFINE VID '=' REAL ';'
                  {
		    makedefinedecl($2,$4);
		    $$ = 0;
		  }
                | ATACS_DEFINE VID '=' '-' REAL ';'
                  {
		    makedefinedecl($2,(-1.0)*$5);
		    $$ = 0;
		  }
		;

booldecl
                : BOOL id_list ';'
                  {
		    initial_stateADT init_st(true, false, false);
		    $$ = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = $2->begin();
			b != $2->end(); b++){
		      (*b)->initial_state = init_st;
		      makebooldecl($1, *b, FALSE, NULL);
		      $$->push_back(make_pair((*b)->label, $1));
		    }
		  }
                | BOOL id_list '=' '{' INIT '}' ';'
                  {
		    initial_stateADT init_st(true, true, false);
		    $$ = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = $2->begin();
			b != $2->end(); b++) {
		      (*b)->initial_state = init_st;
		      makebooldecl($1, *b, $5, NULL);
		      $$->push_back(make_pair((*b)->label, $1));
		    }
		  }
                | BOOL id_list '=' '{' delay '}' ';'
		  {
		    initial_stateADT init_st(true, false, true);
		    $$ = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = $2->begin();
			b != $2->end(); b++)
		      {
			(*b)->initial_state = init_st;
			makebooldecl($1, *b, FALSE, &($5));
			$$->push_back(make_pair((*b)->label, $1));
		      }
		  }
                | BOOL id_list '=' '{' INIT ',' delay '}' ';'
		  {
		    initial_stateADT init_st(true, true, true);
		    $$ = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = $2->begin();
			b != $2->end(); b++)
		      {
			(*b)->initial_state = init_st;
			makebooldecl($1, *b, $5, &($7));
			$$->push_back(make_pair((*b)->label, $1));
		      }
		  }
		;

id_list
               : VID
                 {
		   $$ = new list<actionADT>;
                   $$->push_back($1);
		 }
               | id_list ',' VID
                 {
		   $$ = $1;
                   $$->push_back($3);
		 }
               ;
/*portdecl
                :       CPORT VID ';'
                        { makeportdecl($1,$2,FALSE,0,INFIN,0,INFIN,
				       0,INFIN,0,INFIN,1,1); }
                |       CPORT VID '=' '{' INIT '}' ';'
			{ makeportdecl($1,$2,$5,0,INFIN,0,INFIN,
				      0,INFIN,0,INFIN,1,1); }
                |       CPORT VID '=' '{' delay ',' delay '}' ';'
			{ makeportdecl($1,$2,FALSE,$5.lr,$5.ur,$5.lf,$5.uf,
				       $7.lr,$7.ur,$7.lf,$7.uf,1,1); }
                |       CPORT VID '=' '{' '(' CINT ',' CINT ')' '}' ';'
                        { makeportdecl($1,$2,FALSE,0,INFIN,0,INFIN,
				       0,INFIN,0,INFIN,$6,$8); }
                |       CPORT VID '=' '{' INIT ',' delay ',' delay '}' ';'
			{ makeportdecl($1,$2,$5,$7.lr,$7.ur,$7.lf,$7.uf,
				       $9.lr,$9.ur,$9.lf,$9.uf,1,1); }
                |       CPORT VID '=' '{' INIT ',' '(' CINT ',' CINT ')' '}' ';'
			{ makeportdecl($1,$2,$5,0,INFIN,0,INFIN,
				       0,INFIN,0,INFIN,$8,$10); }
                |       CPORT VID '=' '{' delay ',' delay ','
                                       '(' CINT ',' CINT ')' '}' ';'
			{ makeportdecl($1,$2,FALSE,$5.lr,$5.ur,$5.lf,$5.uf,
				       $7.lr,$7.ur,$7.lf,$7.uf,$10,$12); }
                |       CPORT VID '=' '{' INIT ',' delay ',' delay ','
                                        '(' CINT ',' CINT ')' '}' ';'
			{ makeportdecl($1,$2,$5,$7.lr,$7.ur,$7.lf,$7.uf,
				       $9.lr,$9.ur,$9.lf,$9.uf,$12,$14);}
		;

		*/
delay	
     	        :	'<' CINT ',' CINT ';' CINT ',' CINT '>'
			{ assigndelays(&($$),$2,$4,NULL,$6,$8,NULL); }
     	        |	'<' CINT ',' CINT ',' VID '(' REAL ',' REAL ')'
                        ';' CINT ',' CINT ',' VID '(' REAL ',' REAL ')' '>'
			{ sprintf(distrib1,"%s(%g,%g)",$6->label,$8,$10);
			  sprintf(distrib2,"%s(%g,%g)",$17->label,$19,$21);
                          assigndelays(&($$),$2,$4,distrib1,$13,$15,distrib2);
                        }
     	        |	'<' CINT ',' CINT ',' VID '(' REAL ',' REAL ')'
                        ';' CINT ',' CINT '>'
			{ sprintf(distrib1,"%s(%g,%g)",$6->label,$8,$10);
                          assigndelays(&($$),$2,$4,distrib1,$13,$15,NULL);
                        }
     	        |	'<' CINT ',' CINT ';' CINT ',' CINT ','
                            VID '(' REAL ',' REAL ')' '>'
			{ sprintf(distrib2,"%s(%g,%g)",$10->label,$12,$14);
                          assigndelays(&($$),$2,$4,NULL,$6,$8,distrib2);
                        }
		|	'<' CINT ',' CINT '>'
			{ assigndelays(&($$),$2,$4,NULL,$2,$4,NULL); }
		|	'<' CINT ',' CINT ',' VID '(' REAL ',' REAL ')' '>'
			{ sprintf(distrib1,"%s(%g,%g)",$6->label,$8,$10);
                          assigndelays(&($$),$2,$4,distrib1,$2,$4,distrib1); }
                |       VID
                        { checkdelay($1->label,$1->type);
			  assigndelays(&($$),$1->delay.lr,$1->delay.ur,
				       $1->delay.distr,$1->delay.lf,
                                       $1->delay.uf,$1->delay.distf); }
		;
constructs
            : constructs process
            | constructs component_instances
              {
		if($1 && $2){
		  $$ = $1;
		  $$->splice($$->begin(), *$2);
		}
		else if($1)
		  $$ = $1;
		else
		  $$ = $2;
	      }
            | process { $$ = 0; }
            | component_instances
            ;

component_instances
            : VID VID '(' port_map_list ')' ';'
              {     		
		pair<tels_table*,bool> rp = (*open_module_list)[$1->label];
		if(rp.first != 0){
		  $$ = new list<pair<string,int> >;
		  tels_table* rpc = new tels_table(rp.first);
		  rpc->set_id($2->label);
		  rpc->set_type($1->label);
		  rpc->insert(*$4);

		  my_list sig_mapping(rpc->portmap());
		  rpc->instantiate(&sig_mapping, $2->label, $1->label, true);
		  if(tel_tb->insert($2->label, rpc) == false)
		    err_msg("duplicate component label '", $2->label, "'");

		  const map<string,int> tmp = rpc->signals();
		  for(map<string,string>::iterator b = $4->begin();
		      b != $4->end(); b++){
		    if(tmp.find(b->first) != tmp.end())
		      $$->push_back(make_pair(b->second,
					      tmp.find(b->first)->second));
		    else
		      err_msg("signal '", b->first, "' not found");
		  }
		}
		else {
		  err_msg("module '", $1->label, "' not found");
		  delete $4;
		  return 1;
		}
		delete $4;
	      }
              ;

port_map_list
              : port_map_list ',' formal_2_actual
                { $$ = $1;  $$->insert(*$3);  delete $3; }
              | formal_2_actual
                { $$ = new map<string, string>;  $$->insert(*$1);  delete $1; }
              ;
formal_2_actual
              : VID LARROW VID
                { $$ = new pair<string, string>($1->label, $3->label); }
              ;
process
                : PROC VID ';' body ENDPROC
                  {
		    $$ = TERSrepeat($4,";");
		    if(tel_tb->insert($2->label, $$)==false){
		      err_msg("duplicate process label '", $2->label, "'");
		      delete $$;
		      return 1;
		    }
		    if (1) {
		      printf("Compiled process %s\n",$2->label);
		      fprintf(lg,"Compiled process %s\n",$2->label);
		    } else
		      emitters(0, $2->label, $$);
		    TERSdelete($$);
		  }
                | PROC VID ';' body ':' ENDPROC
                  {
		    $$ = TERSrepeat($4,":");
		    if(tel_tb->insert($2->label, $$)==false){
		      err_msg("duplicate process label '", $2->label, "'");
		      delete $$;
		      return 1;
		    }
		    if (1) {
		      printf("Compiled process %s\n",$2->label);
		      fprintf(lg,"Compiled process %s\n",$2->label);
		    } else
		      emitters(0, $2->label, $$);
		    TERSdelete($$);
		  }
                | TESTBENCH VID ';' body tail
                  {
		    $$ = TERSrepeat($4,";");
		    if(tel_tb->insert($2->label, $$, true)==false) {
		      err_msg("duplicate testbench label '", $2->label, "'");
		      delete $$;
		      return 1;
		    }
                    if (1) {
		      printf("Compiled testbench %s\n",$2->label);
		      fprintf(lg,"Compiled testbench %s\n",$2->label);
		    } else
		      emitters(outpath, $2->label, $$);
		    TERSdelete($$);
		  }
                | TESTBENCH VID ';' body ':' tail
                  {
		    $$ = TERSrepeat($4,":");
		    if(tel_tb->insert($2->label, $$, true)==false) {
		      err_msg("duplicate testbench label '", $2->label, "'");
		      delete $$;
		      return 1;
		    }
                    if (1) {
		      printf("Compiled testbench %s\n",$2->label);
		      fprintf(lg,"Compiled testbench %s\n",$2->label);
		    } else
		      emitters(outpath, $2->label, $$);
		    TERSdelete($$);
		  }
                | GATE VID ';' prs ENDGATE
                  {
                    $$ = TERSrepeat($4,";");
		    if(tel_tb->insert($2->label, $$)==false) {
		      err_msg("duplicate gate label '", $2->label, "'");
		      delete $$;
		      return 1;
		    }
                    if (1) {
		      printf("Compiled gate %s\n",$2->label);
		      fprintf(lg,"Compiled gate %s\n",$2->label);
		    } else
		      emitters(outpath, $2->label, $$);
		    TERSdelete($$);
                  }
                | CONSTRAINT VID ';' constraint ENDCONSTRAINT
                  {
                    $$ = TERSrepeat($4,";");
		    if(tel_tb->insert($2->label, $$)==false) {
		      err_msg("duplicate constant label '", $2->label, "'");
		      delete $$;
		      return 1;
		    }
                    TERSmarkord($$);
                    if (1) {
		      printf("Compiled constraint %s\n",$2->label);
		      fprintf(lg,"Compiled constraint %s\n",$2->label);
		    } else
		      emitters(outpath, $2->label, $$);
		    TERSdelete($$);
                  }
                | ASSUMP VID ';' body ENDASSUMP
                  {
                    $$ = TERSrepeat($4,";");
		    if(tel_tb->insert($2->label, $$)==false) {
		      err_msg("duplicate assumption label '", $2->label, "'");
		      delete $$;
		      return 1;
		    }
                    TERSmarkassump($$);
                    if (1) {
		      printf("Compiled assumption %s\n",$2->label);
		      fprintf(lg,"Compiled assumption %s\n",$2->label);
		    } else
		      emitters(outpath, $2->label, $$);
		    TERSdelete($$);
                  }
                ;
tail
                : ENDTB | END
                ;
prs
                : levelexpr ARROW ACTION levelexpr ARROW ACTION
                  {
		    $3->vacuous=FALSE;
		    $6->vacuous=FALSE;
		    string ss1 = '[' + *$1 + ']' + 'd';
                    $$ = TERS($3, FALSE,$3->lower,$3->upper,FALSE,
			      ss1.c_str(), $3->dist);
		    string ss2 = '[' + *$4 + ']' + 'd';
		    if ((!($3->initial) && !(strchr($3->label,'+'))) ||
			($3->initial && strchr($3->label,'+'))) {
		      $$ = TERScompose(TERS($6, FALSE,$6->lower,$6->upper,
					    FALSE,ss2.c_str(),$6->dist),$$,";");
		    } else {
		      $$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
					       FALSE,ss2.c_str(),$6->dist),";");
		    }
                    $$ = TERSmakeloop($$);
                    delete $1;
                    delete $4;
		  }
                | leakyexprs ARROW ACTION leakyexprs ARROW ACTION
                  {
		    $3->vacuous=FALSE;
		    $6->vacuous=FALSE;
                    $$ = TERScompose($1,TERS($3, FALSE,$3->lower,$3->upper,
                                             FALSE,"[true]",$3->dist),";");
                    $$ = TERScompose($$,TERSrename($$,$4),";");
                    $$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
                                             FALSE,"[true]",$6->dist),";");
                    $$ = TERSmakeloop($$);
		  }
                ;

leakyexprs      : leakyexprs '|' leakyexpr
                  { $$ = TERScompose($1, TERSrename($1,$3),"|"); }
                | leakyexpr
                ; 
leakyexpr       : levelexpr '(' sing_dist ')'
                  {
		    $$ = Guard(*$1);
                    delete $1;
		    TERSmodifydist($$,$3);
		  }
                ;
constraint
                : levelexpr ARROW ACTION
                  {
		    $3->vacuous=FALSE;
		    string ss1 = '[' + *$1 + ']';// + 'd';
                    $$ = TERS($3,FALSE,0,INFIN,FALSE,ss1.c_str(),$3->dist);
                    //$$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
                    //                         FALSE, Cstr, $6->dist),";");
                    $$ = TERSmakeloop($$);
                    delete $1;
		  }
                | levelexpr ARROW '<' CINT ',' CINT '>' ACTION
                  {
		    $8->vacuous=FALSE;
		    string ss1 = '[' + *$1 + ']';// + 'd';
                    $$ = TERS($8,FALSE,$4,$6,FALSE,ss1.c_str(),$8->dist);
                    //$$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
                    //                         FALSE, Cstr, $6->dist),";");
                    $$ = TERSmakeloop($$);
                    delete $1;
		  }
                | ACTION ARROW or_constraint
                  {
		    $1->vacuous=FALSE;
                    $$ = TERS($1,FALSE,0,INFIN,FALSE,"[true]",$1->dist);
                    $$ = TERScompose($$,$3,";");
                    //$$ = TERSmakeloop($$);
		  }
                | ACTION '@' ARROW or_constraint
                  {
		    $1->vacuous=TRUE;
                    $$ = TERS($1,FALSE,0,INFIN,FALSE,"[true]",$1->dist);
                    $$ = TERScompose($$,$4,";");
                    //$$ = TERSmakeloop($$);
		  }
                ;
or_constraint
                : or_constraint '|' and_constraint
                  { $$ = TERScompose($1,TERSrename($1,$3),"#"); }
                | and_constraint
                ;
and_constraint
                : and_constraint '&' action
                  { $$ = TERScompose($1,$3,"||"); }
                | action
                ;
action
                : ACTION
                  { $1->vacuous=FALSE;
		    $$ = TERS($1, FALSE, $1->lower, $1->upper, FALSE,
			      "[true]",$1->dist);
		  }
                | VID 
                  {
                    $$ = TERS(make_dummy($1->label),FALSE,$1->lower,$1->upper, 
			      FALSE,"[true]",$1->dist);
                  }
                | '<' CINT ',' CINT '>' ACTION
                  { $6->vacuous=FALSE;
		    $$ = TERS($6, FALSE, $2, $4, FALSE); }
                | skip
                ;
skip
                : SKIP { $$ = TERSempty(); }//TERS(dummyE(), FALSE, 0, 0, FALSE); }
                ;
body
                : body '|' body
                  { $$ = TERScompose($1, TERSrename($1,$3), "|"); }
                | body ';' body
                  { $$ = TERScompose($1, TERSrename($1,$3), ";"); }
                | body ':' body
                  { $$ = TERScompose($1, TERSrename($1,$3), ":"); }
		| body PARA body
                  { $$ = TERScompose($1, $3, "||"); }
                | '(' body ')' { $$ = $2; }
                | action
		| guardstruct
		;
guardstruct	
                : '[' guardcmdset ']' {  $$=$2;  $$ = TERSrepeat($$,";");  }
                | '[' expr ']'   { $$ = $2; }
                | '*' '[' body ']'  { $$=TERSmakeloop($3); }
                ;
guardcmdset	
                : guardcmdset '|' guardcmd
                  { $$ = TERScompose($1, TERSrename($1,$3),"|"); }
		| guardcmd
		;
guardcmd	
                : expr ARROW body
                  { $$ = TERScompose($1, TERSrename($1,$3),";"); }
                | expr ARROW body ';' '*'
                  {
		    $$ = TERScompose($1, TERSrename($1,$3),";");
		    $$ = TERSmakeloop($$);
		  }
                | expr '(' sing_dist ')' ARROW body
                  {
		    TERSmodifydist($1,$3);
		    $$ = TERScompose($1, TERSrename($1,$6),";");
		  }
                | expr '(' sing_dist ')' ARROW body ';' '*'
                  {
		    TERSmodifydist($1,$3);
		    $$ = TERScompose($1, TERSrename($1,$6),";");
		    $$ = TERSmakeloop($$);
		  }
                | expr ARROW ':' body
                  { $$ = TERScompose($1, TERSrename($1,$4),":"); }
                | expr ARROW ':' body ';' '*'
                  {
		    $$ = TERScompose($1, TERSrename($1,$4),":");
		    $$ = TERSmakeloop($$);
		  }
                | expr '(' sing_dist ')' ARROW ':' body
                  {
		    TERSmodifydist($1,$3);
		    $$ = TERScompose($1, TERSrename($1,$7),":");
		  }
                | expr '(' sing_dist ')' ARROW ':' body ';' '*'
                  {
		    TERSmodifydist($1,$3);
		    $$ = TERScompose($1, TERSrename($1,$7),":");
		    $$ = TERSmakeloop($$);
		  }
		;
/*
sing_dist       : REAL { $$ = $1; }
                | REAL '*' CINT { $$ = $1 * $3; }
                | REAL '*' VID  
                  { checkdefine($3->label,$3->type);
		    $$ = $1 * $3->real_def; }
                | REAL '/' CINT { $$ = $1 / $3; }
                | REAL '/' VID  
                  { checkdefine($3->label,$3->type);
		    $$ = $1 / $3->real_def; }
                ;
*/
sing_dist       : REAL { $$ = $1; }
                | CINT { $$ = (double)$1; }
                | VID  
                  { checkdefine($1->label,$1->type);
		    $$ = $1->real_def; }
                | SUM '(' sing_dist ',' sing_dist ')' { $$ = $3+$5; }
                | DIFF '(' sing_dist ',' sing_dist ')' { $$ = $3-$5; }
                | sing_dist '*' REAL { $$ = $1 * $3; }
                | sing_dist '*' CINT { $$ = $1 * $3; }
                | sing_dist '*' VID  
                  { checkdefine($3->label,$3->type);
		    $$ = $1 * $3->real_def; }
                | sing_dist '*' SUM '(' sing_dist ',' sing_dist ')' 
                  { $$ = $1*($5+$7); }
                | sing_dist '*' DIFF '(' sing_dist ',' sing_dist ')' 
                  { $$ = $1*($5-$7); }       
                | sing_dist '/' REAL { $$ = $1 / $3; }
                | sing_dist '/' CINT { $$ = $1 / $3; }
                | sing_dist '/' VID  
                  { checkdefine($3->label,$3->type);
		    $$ = $1 / $3->real_def; }
                | sing_dist '/' SUM '(' sing_dist ',' sing_dist ')' 
                  { $$ = $1/($5+$7); }
                | sing_dist '/' DIFF '(' sing_dist ',' sing_dist ')' 
                  { $$ = $1/($5-$7); }
                ;
expr
                : actionexpr
                | levelexpr
                  {
		    $$ = Guard(*$1);
                    delete $1;
		  }
                | '<' CINT ',' CINT '>' levelexpr
                  {
                    //char *t = new char[strlen($6) + 3];
                    //sprintf(t, "[%s]", $6);
		    string ss = '[' + *$6 + ']';
                    $$ = TERS(dummyE(), FALSE, $2, $4, FALSE, ss.c_str());
                    delete $6;
                  }
                ;
actionexpr
                : actionexpr '|' conjunct
                  { $$ = TERScompose($1,TERSrename($1,$3),"#"); }
                | conjunct
                ;
levelexpr
                : levelexpr '|' levelconjunct
                  {
		    string ss = *$1 + '|' + *$3;
		    $$ = $$ = new string(ss);
                    delete $3;
		  }
                | levelconjunct
           	;
levelconjunct
                : levelconjunct '&' leveliteral
                  {
		    string ss = *$1 + '&' + *$3;
		    $$ = new string(ss);
                    delete $3;
		  }
                | leveliteral
                ;
leveliteral
                : VID  { $$ = new string($1->label); }
                | '~' VID
                  {
		    string ss = '~' + string($2->label);
		    $$ = new string(ss);
		  }
                | '(' levelexpr ')'
                  {
		    string ss = '(' + *$2 + ')';
		    $$ = new string( ss );
                  }
                | '~' '(' levelexpr ')'
                  {
		    string ss = '(' + *$3 + ')';
		    ss = '~' + ss;
		    $$ = new string( ss );
                  }
                | INIT
                  {
		    if($1 == TRUE)
		      $$ = new string("true");
		    else
		      $$ = new string("false");
		  }
                ;
conjunct
                : conjunct '&' csp_literal
                  { $$ = TERScompose($1,$3,"||"); }
                | csp_literal
                ;
csp_literal
                : ACTION
                  {
		    $1->vacuous=FALSE;
		    $$ = TERS($1,TRUE,$1->lower,$1->upper,FALSE,"[true]",
			      $1->dist);
		  }
                | ACTION '/' CINT
                  {
		    $1->vacuous=FALSE;
		    $$ = TERS($1,TRUE,$1->lower,$1->upper,FALSE,"[true]",
			      $1->dist,$3);
		  }
                | ACTION '@'
                  { $1->vacuous=TRUE;
		    $$ = TERS($1,TRUE,$1->lower,$1->upper,FALSE,"[true]",
			      $1->dist); }
/*
                | ACTION '#'
                  { $$ = probe($1,0); }
                | ACTION '(' CINT ')' '#'
                  { $$ = probe($1,$3); }
*/
                | '(' actionexpr ')'
                  { $$ = $2; }
                | skip
                ;


%%




int yyerror(char* S)
{
  printf("%s\n",S);
  fprintf(lg,"%s\n",S);
  printf("%s: %d: syntax error.\n", file_scope1, linenumber);
  fprintf(lg,"%s: %d: syntax error.\n", file_scope1, linenumber);

  clean_buffer();
  compiled=FALSE;
  return 1;
}

int yywarning(char* S)
{
  printf("%s\n",S);
  fprintf(lg,"%s\n",S);
  //printf("%s: %d: syntax error.\n", file_scope1, linenumber);
  //fprintf(lg,"%s: %d: syntax error.\n", file_scope1, linenumber);

  //clean_buffer();
  //compiled=FALSE;
  return 0;
}


void constructor()
{
  //table = new SymTab;

  PSC = 0;
}


void destructor()
{
  //delete table;
  clean_buffer();
}


actionADT make_dummy(char *name)
{
  char s[1000]; //= string("$") + (int)PSC + '+';
  sprintf(s, "$%s",name);
  actionADT a = action(s, strlen(s));
  a->type = DUMMY;
  return a;
}

actionADT dummyE()
{
  char s[1000]; //= string("$") + (int)PSC + '+';
  sprintf(s, "$%ld+", PSC);
  actionADT a = action(s, strlen(s));
  a->type = DUMMY;
  PSC++;
  return a;
}

//Converts an double into a string
//An int string for now
string numToString(double num) {
  char cnumber[50];
  sprintf(cnumber, "%d", (int)num);
  string toReturn = cnumber;
  return toReturn;
}

//Converts an double into a string
//An int string for now
string intToString(int num) {
  char cnumber[50];
  sprintf(cnumber, "%i", (int)num);
  string toReturn = cnumber;
  return toReturn;
}


// Create a timed event-rule structure for an guard.
TERstructADT Guard(const string & expression){
  string bracketed('[' + expression + ']');
  return TERS(dummyE(),FALSE,0,0,FALSE,bracketed.c_str());
}

TERstructADT Assign(const string & expression) {
  string angled('<' + expression + '>');
  return TERS(dummyE(),FALSE,0,0,FALSE,angled.c_str());
}

// Create a timed event-rule structure for an guarded action.
TERstructADT Guard(const string & expression,
		   actionADT target,
		   const string&data=""){
  string bracketed('[' + expression + ']');
  return TERS(target,FALSE,target->lower,target->upper,FALSE,
	      bracketed.c_str(),NULL,-1,data);
}

TERstructADT FourPhase(const string & channel, const string & data){
  string req(channel+reqSuffix);//channel_send for send,channel_rcv for receive
  string ack(channel+ackSuffix);//channel_rcv for send,channel_send for receive
  TERstructADT result(TERS(++req,FALSE,(++req)->lower,(++req)->upper,FALSE,
			   "[true]",NULL,-1,data));
  result =  TERScompose(result, Guard(    ack,--req,data),";");
  result =  TERScompose(result, Guard('~'+ack, join     ),";");
  int antiType((SENT|RECEIVED)^direction);
  map<string,int>& ports = tel_tb->port_decl();
  if(signals[cur_entity].find(channel) == signals[cur_entity].end()){
    if(ports.find(channel) == ports.end()){
      err_msg("Undeclared channel ", channel);
    }
    else{
      if(ports[channel] & antiType){
	err_msg("send and receive on the same side of channel ", channel);
      }
      ports[channel] |= direction;
    }
  }
  else{
    if(signals[cur_entity][channel] & direction){
      if(direction & SENT){
	err_msg("Attempt to send on both sides of channel ", channel);
      }
      else{
	err_msg("Attempt to receive on both sides of channel ",	channel);
      }
    }
    if(processChannels.find(channel)==processChannels.end()){
      processChannels[channel]=direction;
    }
    else{
      if(processChannels[channel] & antiType){
	err_msg("send and receive on the same side of channel ", channel);
      }
      processChannels[channel] |= direction;
    }
  }
  return result;
}

TYPES * Probe(const string & channel){
  map<string,int>& ports(tel_tb->port_decl());
  if(ports.find(channel)==ports.end()){
    if(signals[cur_entity].find(channel)==
       signals[cur_entity].end()){
      err_msg("Probing undeclared channel ", channel);
    }
    if(signals[cur_entity][channel]&PASSIVE){
      err_msg("Channel ", channel, " is probed on both sides!");
    }
    processChannels[channel] |= PASSIVE;
    signals[cur_entity][channel] |= PASSIVE;
  }
  else{
    if(ports[channel] & ACTIVE){
      err_msg("Attempt to probe active channel ", channel);
    }
    ports[channel] |= PASSIVE;
  }
  TYPES * result = new TYPES;
  result->set_obj("exp");
  result->set_str(logic(channel+SEND_SUFFIX, channel+RECEIVE_SUFFIX, "|"));
  result->set_data("bool");
  return result;
}

actionADT operator++(const string & name){
  actionADT rise(action(name+"+"));
  rise->type &= ~IN;
  rise->type |= OUT;
  return rise;
}

actionADT operator--(const string & name){
  actionADT fall(action(name+"-"));
  fall->type &= ~IN;
  fall->type |= OUT;
  return fall;
}

void declare(list<pair<string,int> >* ports){
  if(ports){
    map<string,int> tmp(ports->begin(), ports->end());	
    tel_tb->insert(tmp);
    tel_tb->insert(*ports);
    for(map<string,int>::iterator b = tmp.begin();  b != tmp.end(); b++){
      if(b->second & IN){
	table->insert(b->first, make_pair((string)"in",(TYPES*)0));
      }
      else{
	table->insert(b->first, make_pair((string)"out",(TYPES*)0));
      }
    }
    delete ports;
  }
}

TERstructADT signalAssignment(TYPES* target, TYPES* waveform){
  string temp;
  if(waveform->get_string() == "'1'")
    temp =  target->get_string() + '+';
  else if(waveform->get_string() == "'0'")
    temp =  target->get_string() + '-';
  else
    temp = '$';// + target->get_string(); //"error";

  int l = 0,  u = INFIN;
  const TYPELIST *tl = waveform->get_list();
  if(tl != 0){
    TYPELIST::const_iterator I = tl->begin();
    if(I != tl->end()) {
      l = tl->front().second.get_int();
      I++;
      if(I!=tl->end())
	u = tl->back().second.get_int();
    }
  }
  actionADT a;
  if (temp[0] == '$') {
    a = dummyE();
    //a->type = DUMMY;
    // cout << "target = " << target->get_string() << " waveform = " << waveform->get_string() << endl;
    // ZHEN(Done): add assignment to action "a" of (target->get_string(),waveform->get_string())
    // a->list_assigns= NULL;
    if (target->get_string().c_str()!=NULL && waveform->get_string().c_str()!=NULL){
      a->list_assigns = addAssign(a->list_assigns, target->get_string(), waveform->get_string());
    }
  } else {
    a = action(temp);
  }
  TERstructADT result(TERS(a, FALSE, l, u, TRUE));
  delete_event_set(result->last);
  result->last = 0;
  return result;
}
