
  /************************************************************************
   *                         COMPILER--VHDL                               *
   *                          ---PARSER---                                *
   ************************************************************************/

%{

#include<stdio.h>
#include<string.h>  
#include<iostream.h>
#include "symtab.h"
#include "types.h"
#include "aux.h"


SymTab *new_table(SymTab *Table);


int yylex (void*);

int yyerror(char *s);

#define YYDEBUG 1

extern FILE *lg;

extern SymTab *table;

extern FILE *yyin;

extern char *file_scope1;

FILE *oldfile;

SymTab *trans1, *trans2;

cell *l;
cell *h=l= new cell; 


char *pathname, *fileattr;



/*************************************************************************/
//extern char     g_lline[];      /* from lexer */
extern char     yytext[];
//extern int      column;
extern int      linenumber;
/*************************************************************************/

%}

%pure_parser


%union{
  int         intval;
  double      floatval;
  char        *id;
  char        *decimalit;
  char        *basedlit;
  char        *charlit;
  char        *strlit;
  char        *bitlit;
  class Type        *types;
};




%token ABS ACCESS AFTER ALIAS ALL AND ARCHITECTURE ARRAY ASSERT ATTRIBUTE
%token VBEGIN BLOCK BODY BUFFER BUS CASE COMPONENT CONFIGURATION CONSTANT
%token DISCONNECT DOWNTO ELSE ELSIF END ENTITY EXIT VFILE FOR
%token FUNCTION PROCEDURE  NATURAL
%token GENERATE GENERIC GUARDED IF IMPURE IN INITIALIZE INOUT IS LABEL LIBRARY
%token LINKAGE LITERAL LOOP MAP MOD NAND NEW NEXT NOR NOT UNAFFECTED
%token UNITS GROUP OF ON OPEN OR OTHERS OUT VREJECT INERTIAL XNOR
%token PACKAGE PORT POSTPONED PROCESS PURE RANGE RECORD REGISTER REM
%token REPORT RETURN SELECT SEVERITY SHARED SIGNAL SUBTYPE THEN TO TRANSPORT
%token TYPE UNITS UNTIL USE VARIABLE VNULL WAIT WHEN WHILE WITH XOR 
%token INT REAL BASEDLIT CHARLIT STRLIT BITLIT ARROW EXPON ASSIGN BOX
%token SLL SRL SLA SRA ROL ROR  USE_CLAUSE  AT  INF  INPUT  OUTPUT  EOC

%token <id>ID  <strlit>STRLIT

%type <id>designator  CHARLIT 


%type <id> name  mark  selected_name  name2  name3  insl_name   alias_name
%type <id> idparan  suffix 
%type <id> enumeration_literal

%type <types> type_definition   scalar_type_definition    composite_type_definition 
%type <types> access_type_definition    file_type_definition     enumeration_type_definition
%type <types> integer_floating_type_definition   physical_type_definition
%type <types> array_type_definition      record_type_definition  range_constraint  range

%type <types> unconstrainted_array_definition  constrained_array_definition  subtype_indication

%type <types> subtype_indication1  subtype_indication_list  formal_part target  conditional_waveforms

%type <types> expression  relation  simple_expression  terms  term  factor  primary  discrete_range1
%type <types> shift_expression  condition  waveform_element  waveform   allocator  waveform_head
%type <types> selected_waveforms  actual_part  association_element   element_association
%type <types> association_list  choices choice  qualified_expression   elarep   aggregate

%type <types> physical_literal  andexpr orexpr  xorexpr  literal  physical_literal_no_default

%type <types> gen_association_list_1 gen_association_element  assign_exp  attribute_name al_decl_head

%nonassoc '>' '<' '=' GE LE NEQ 
%left '+' '-' '&'
%left MED_PRECEDENCE
%left '*' '/' MOD REM
%nonassoc EXPON ABS NOT MAX_PRECEDENCE

%start start_point

%%

/* Design File */

start_point    : design_file { }
               ;


design_file    : design_file design_unit
               | design_unit 
               ;

design_unit    : context_clause library_unit
//               | context_clause library_unit
               ;

context_clause : 
               | context_clause context_item
               ;

context_item   : library_clause
               | use_clause
               ;

library_unit   : primary_unit
               | secondary_unit
               ;

primary_unit   : entity_declaration
               | configuration_declaration
               | package_declaration
               ;

secondary_unit : architecture_body
               | package_body
               ;


library_clause : library_list ';'
               ;


library_list   : LIBRARY ID    { table->add_symbol($2,Library,UnknownD); }
               | library_list ',' ID
                               { table->add_symbol($3,Library,UnknownD); }
               ;



/* Library Unit Declarations */

entity_declaration
               : ENTITY ID  {  table->add_symbol($2,Entity,table->other_tab);
	                       table= new_table(table);  }
			       
                 entity_descr 
               ;
                 
entity_descr   :IS entity_header entity_declarative_part END endofentity ';'
	       |IS entity_header entity_declarative_part 
                 VBEGIN  entity_statement_part END endofentity ';'
               ;

endofentity    :
               | ENTITY { }
               | ID    { Type *t= search(table,$1); 
			 t->checktype(Entity,UnknownD);
			 delete t;	
		       }
               | ENTITY ID  { Type *t= search(table,$2);
			      t->checktype(Entity,UnknownD);
			      delete t; 
			    } 
               ;

entity_header  :
               | generic_clause
               | port_clause
               | generic_clause port_clause
               ;


generic_clause : GENERIC '(' interface_list ')' ';'
               ;

     
port_clause    : PORT '(' interface_list ')' ';' 
               ;     

     
entity_declarative_part
               :
               | entity_declarative_part entity_declarative_item
               ;


entity_declarative_item
               : subprogram_declaration
               | subprogram_body
               | type_declaration
               | subtype_declaration 
               | constant_declaration
               | signal_declaration
               | file_declaration
               | alias_declaration
               | attribute_declaration
               | label_declaration 
               | attribute_specification
               | initialization_specification 
               | disconnection_specification
               | use_clause
               | variable_declaration
               | group_template_declaration
               | group_declaration
               ;


group_template_declaration
               : GROUP ID     { table->add_symbol($2,Group,UnknownD); }
                 IS '(' gtp_body ')' ';'
               ;


gtp_body       : gtp_body ',' entity_class box_symbol 
               | entity_class box_symbol 
               ;

 
box_symbol     :
               | BOX
               ;


group_declaration
               : GROUP ID  { table->add_symbol($2,Group,UnknownD); }
                 ':' ID    { Type *t= search(table,$5);//table->lookup($5);
			     delete t; }
                 '(' gd_body ')' ';' 
               ;                     


gd_body        : gd_body ',' ID  { Type *t= search(table,$3);
                                   /*table->lookup($3);*/  delete t; } 
               | gd_body ',' CHARLIT
               | ID              { Type *t= search(table,$1);
	                           /*table->lookup($1);*/ delete t; }
               | CHARLIT {} 
               ;


entity_statement_part
               :
               | entity_statement_part entity_statement
               ;


entity_statement
               : concurrent_assertion_statement 
               | concurrent_procedure_call
               | process_statement
                                /* {  SymTab *temp= table->header;
				    delete table;
				    table= temp; }*/
               ;


architecture_body
               : ARCHITECTURE ID 
                       { table->add_symbol($2,Architect,UnknownD); }
			      
                 OF ID      { Type *t= search(table,$5); 
                              if( (t->nametype()!=Entity) && (t->nametype()!=ErrorN) )
                                  errmsg(" the id is not declared as entity");
                              table= new_table(table);  }		        
                 IS  architecture_declarative_part
                 VBEGIN architecture_statement_part END architail ';'
               ;


architail     :
              | ARCHITECTURE ID { Type *t= search(table,$2);
				  t->checktype(Architect,UnknownD);
				  delete t; }
              | ARCHITECTURE 
              | ID { Type *t= search(table,$1);
		     t->checktype(Architect,UnknownD);
		     delete t; }
              ;


architecture_declarative_part
              :
              | architecture_declarative_part block_declarative_item
              ;


architecture_statement_part
              : 
              | architecture_statement_part concurrent_statement
              ;


configuration_declaration
               : CONFIGURATION ID { table->add_symbol($2,Architect,UnknownD); }
                 OF ID { Type *t= search(table,$5);//table->lookup($5);
			 t->checktype(Entity,UnknownD);
			 delete t;   }
                 config_tail
               ;


config_tail    : IS  configuration_declarative_part block_configuration
                 END configtail ID ';'
                         {  Type *t= search(table,$6); //table->lookup($6);
			    t->checktype(Entity,UnknownD);
			    delete t; }
               | IS  configuration_declarative_part block_configuration
                 END configtail ';'
               ;


configtail     :
               | CONFIGURATION
               ;


configuration_declarative_item
               : use_clause
               | attribute_specification
               | group_declaration
               ;


configuration_declarative_part
               :
               | configuration_declarative_part configuration_declarative_item
               ;


block_configuration
              : FOR
                     block_specification us ci 
                END FOR ';'
              ;


block_specification
              : ID       { Type *t= search(table,$1);//table->lookup($1);
		           t->checktype(Architect,UnknownD);
		           delete t; }
              | ID       { Type *t= search(table,$1);//table->lookup($1);
			   delete t; }

                '(' www ')' 
              ;


www           : discrete_range
              | INT
              | REAL 
              ;


us            : 
              | us use_clause
              ;


ci            : 
              | ci configuration_item
              ;


configuration_item
               : block_configuration 
               | component_configuration
               ;


component_configuration
               : FOR component_specification END FOR ';'
               | FOR component_specification binding_indication ';' END FOR ';'
               | FOR component_specification block_configuration    END FOR ';'
               | FOR component_specification binding_indication ';'
                 block_configuration END FOR ';'
               ;

package_declaration
               : PACKAGE ID  { table->add_symbol($2,Package,UnknownD);
	                       table= new_table(table); }
                 package_tail 
               ;


package_tail   : IS package_declarative_part END packagetail ';'
               | IS package_declarative_part END packagetail ID ';'
                            { Type *t= search(table,$5);  
                              t->checktype(Package,UnknownD);
			      delete t;
			      } 
               ;


packagetail    :
               | PACKAGE
               ;


package_declarative_part
               :
               | package_declarative_part package_declarative_item
               ;


package_declarative_item
               : subprogram_declaration 
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
               | use_clause
               | variable_declaration
               | group_template_declaration
               | group_declaration
               ;

package_body   : PACKAGE BODY ID { table->add_symbol($3,UnknownN,UnknownD);
                                   table= new_table(table); }
                 package_body_tail {  }
               ;

package_body_tail
               : IS package_body_declarative_part END pac_body_tail ';'
               | IS package_body_declarative_part END pac_body_tail ID ';'
                              { Type *t= search(table,$5);
				delete t; }
               ;

pac_body_tail  :
               | PACKAGE BODY
               ;


package_body_declarative_part
               :
               | package_body_declarative_part package_body_declarative_item
               ;


package_body_declarative_item
               : subprogram_declaration
               | subprogram_body
               | type_declaration
               | subtype_declaration 
               | constant_declaration
               | file_declaration
               | alias_declaration
               | use_clause
               | variable_declaration
               | group_template_declaration
               | group_declaration
               ;



/* Declarations and Specifications */

subprogram_declaration
               : subprogram_specification ';'
                        {  SymTab *temp= table->header;
			   delete table;
		           table= temp;  }
               ;


subprogram_specification
               : PROCEDURE designator 
                        { table->add_symbol($2,Proc,UnknownD); }
               | PROCEDURE designator 
                        { table->other_tab = new SymTab;
			  SymTab *temp= table;
			  table=table->other_tab;
			  table->header= temp;  }
                 '(' interface_list ')'  
                        {  table->header->add_symbol($2,Proc,table);  }
               | func_head FUNCTION designator RETURN mark
                        { Type *t= search(table,$5);
                          table->add_symbol($3,Function,t->datatype());
                          table->other_tab = new SymTab;
			  SymTab *temp = table;
			  table=table->other_tab;
			  table->header = temp; }
               | func_head FUNCTION designator 
                        { table= new_table(table); }
                 '(' interface_list ')' RETURN mark 
                        {  Type *t= search(table,$9);
                           table->header->add_symbol($3,Function,t->datatype(),table);
                           delete t;  }
               ;


func_head      :
               | PURE
               | IMPURE
               ;


subprogram_body: subprogram_specification IS subprogram_declarative_part
                 VBEGIN subprogram_statement_part END subprog_tail ';' 
                          {  SymTab *temp= table->header;
			     delete table;
		             table= temp;  }
               | subprogram_specification IS subprogram_declarative_part
                 VBEGIN subprogram_statement_part
                          END subprog_tail designator ';'
                          { Type *t= search(table,$8); 
			    delete t; 
                            SymTab *temp= table->header;
			    delete table;
			    table= temp;   }
			    
               ;


subprog_tail   :
               | PROCEDURE 
               | FUNCTION  
               ;


subprogram_statement_part
               : 
               | subprogram_statement_part sequential_statement
               ;

designator     : ID     { $$= $1; }
               | STRLIT { $$= $1; }
               ;


subprogram_declarative_part
               :
               | subprogram_declarative_part subprogram_declarative_item 
               ;


subprogram_declarative_item
               : subprogram_declaration
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
               | use_clause
               | group_template_declaration
               | group_declaration
               ;

type_declaration
               : full_type_declaration
               | incomplete_type_declaration
               ;


full_type_declaration
               : TYPE ID  { table->other_tab= new SymTab;
			    SymTab *temp= table;
			    table= table->other_tab;
			    table->header= temp; }
                 IS type_definition ';'
                          { SymTab *temp= table->header; 
			    table= temp;
                            table->add_symbol($2,TypeN,$5->datatype()); } 
               ;


incomplete_type_declaration
               : TYPE ID ';'   { table->add_symbol($2,TypeN,UnknownD); }
               ;

type_definition: scalar_type_definition     { $$= $1; }
               | composite_type_definition  { $$= $1; }
               | access_type_definition     { $$= $1; } 
               | file_type_definition       { $$= $1; }
               ;

scalar_type_definition
               : enumeration_type_definition       { $$= $1; }
               | integer_floating_type_definition  { $$= $1; }
               | physical_type_definition          { $$= $1; }
               ;

composite_type_definition
               : array_type_definition   { $$= $1; }
               | record_type_definition  { $$= $1; }
               ;

constant_declaration
               : CONSTANT identifier_list ':' subtype_indication ';'
                          { while(h->next){
			        Type *t= new Type(Constant,$4->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next;    }    }
               | CONSTANT identifier_list ':' subtype_indication
                          { while(h->next){
			        Type *t= new Type(Constant, $4->datatype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next; }  }
                 ASSIGN expression ';'
                          { if( $4->isArray() && ($7->datatype() != $4->Subtype()) 
                                 && !( $7->isError() || $4->isError()) ) { 
				 errmsg(" wrong type value assigned to constant"); }
			     else if( !$4->isArray() &&($7->datatype() != $4->datatype()) 
                                      && !( $7->isError() || $4->isError()) ){ 
                                      errmsg(" wrong type value assigned to constant");  }
			   }
               ;

 

identifier_list: ID       { l->str= $1; 
                            l= l->next= new cell;
                            l->next= NULL;  }                           
               | identifier_list ',' ID 
		          { l->str= $3;
                            l= l->next= new cell;
                            l->next= NULL; }
               ;


signal_declaration
               : SIGNAL identifier_list ':' subtype_indication ';'
                           { while(h->next){
			        Type *t= new Type(Signal, $4->datatype(), $4->Subtype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next;   }  }
               | SIGNAL identifier_list ':' subtype_indication signal_kind ';'
                            { while(h->next){
			        Type *t= new Type(Signal, $4->datatype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next; }  }
               | SIGNAL identifier_list ':' subtype_indication
                           { while(h->next){
			        Type *t= new Type(Signal, $4->datatype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next; }  }
                 ASSIGN expression ';' sigtail 
                           { if( $7->datatype()!=$4->datatype() )
			         errmsg(" wrong type value assigned to signal");  }
               | SIGNAL identifier_list ':' subtype_indication signal_kind
                           { while(h->next){
			        Type *t= new Type(Signal, $4->datatype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next; }  }
                 ASSIGN expression ';' sigtail 
                           { if( $8->datatype()!=$4->datatype() )
			         errmsg(" wrong type value assigned to signal");  }
               ;



sigtail        : 
               | AT INPUT '<' delay_spec '>'
               | AT OUTPUT '<' delay_spec '>'
               ;


delay_spec     : delay_bound ';' delay_bound
               ;


delay_bound    : INT ',' INT
               | INT ',' INF
               ;


signal_kind    : REGISTER | BUS
               ;


variable_declaration
               : var_decl_head VARIABLE identifier_list ':' subtype_indication ';' 
                              { Type *t; 
                                while(h->next){ 
				  if( $5->isArray() ){
				  t= new Type(Variable, $5->Subtype()); }
				else
				    t= new Type(Variable, $5->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next;  }   }                          
               | var_decl_head VARIABLE identifier_list ':' subtype_indication
                           { while(h->next){
                                Type *t;
                                if( $5->isArray() )
			            t= new Type(Variable, $5->Subtype());
				else
				    t= new Type(Variable, $5->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next;  }   }
                 ASSIGN expression ';' 
                           { if( $5->isArray() && ($8->datatype() != $5->Subtype()) 
                                 && !( $8->isError() || $5->isError()) ) { 
				 errmsg(" wrong type value assigned to variable"); }
			     else if( !$5->isArray() && ($8->datatype() != $5->datatype()) 
                                      && !( $8->isError() || $5->isError()) ){ 
			         errmsg(" wrong type value assigned to variable");  }
			   }
               ;


var_decl_head  :
               | SHARED
               ;



file_declaration
               : VFILE identifier_list ':' subtype_indication ';'
                            { while(h->next){
			        Type *t= new Type(FileN, $4->datatype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next;  }  }
               | VFILE identifier_list ':' subtype_indication
                            { while(h->next){
			        Type *t= new Type(FileN, $4->datatype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next;  }  } 
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
                           {  Type *t= search(table,$5); 
			      table->add_symbol($2,$3);  }
               ;


alias_name     : ID        { $$= $1; }
               | CHARLIT   { $$= $1; }
               | STRLIT    { $$= $1; }
               ;


al_decl_head   :                         { $$= new Type(UnknownN,UnknownD,UnknownD); }
               | ':' subtype_indication  { $$= $2; }
               ;


al_decl_tail   : signature
               |
               ;


signature_symbol
               :
               | signature
               ;


signature      : '[' signature_body  signature_tail ']'
               | '[' signature_tail ']' 
               ;


signature_body
               : name   { Type *t= search(table,$1);
			  delete t; }
               | signature_body ',' name
                        { Type *t= search(table,$3);
			  delete t; }
               ;


signature_tail :
               | RETURN name
               ;


component_declaration 
               : COMPONENT ID
                         { table= new_table(table); }
                 component_declaration_tail
                         { SymTab *temp= table; 
                           table= table->header; 
                           table->add_symbol($2,Component,temp);  } 
               ;


component_declaration_tail 
               : comp_decl_head END COMPONENT comp_decl_tail ';'
               | comp_decl_head generic_clause END COMPONENT comp_decl_tail ';'
               | comp_decl_head port_clause END COMPONENT comp_decl_tail ';'
               | comp_decl_head generic_clause port_clause END COMPONENT comp_decl_tail ';'
               ;	

comp_decl_head :
               | IS
               ;


comp_decl_tail :
               | ID { Type *t= search(table,$1);
		      t->checktype(Component,UnknownD);
		      delete t;  }
               ;


attribute_declaration
              : ATTRIBUTE ID ':' ID ';'
                         { Type *t= search(table,$4);
                           Type *temp= new Type(Attribute,t->datatype());
			   table->add_symbol($2,temp);
			   delete t,temp; } 
              ;

attribute_specification
              : ATTRIBUTE ID  { Type *t= search(table,$2);//table->lookup($2);
			        delete t; }
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
               : ID       { Type *t= search(table,$1); 
			    delete t; }
               | STRLIT   {}
               | CHARLIT {}
               ;




entity_class   : ENTITY | ARCHITECTURE | CONFIGURATION | PROCEDURE | FUNCTION
               | PACKAGE | TYPE | SUBTYPE | CONSTANT | SIGNAL | VARIABLE
               | COMPONENT | LABEL | LITERAL | UNITS | GROUP | VFILE
               ;


configuration_specification
               : FOR component_specification binding_indication ';'
               ;


component_specification 
               : instantiation_list ':' name
                        { Type *t= search(table,$3);//table->lookup($3);
			  delete t; }
               ;


instantiation_list
               : component_list
               | OTHERS
               | ALL
               ;

component_list : ID          { Type *t= search(table,$1);//table->lookup($1);
			       delete t; }
               | component_list ',' ID
                             { Type *t= search(table,$3);//table->lookup($3);
			       delete t; }
               ;

binding_indication
               :
               | USE entity_aspect
               | USE entity_aspect generic_map_aspect
               | USE entity_aspect port_map_aspect
               | USE entity_aspect generic_map_aspect port_map_aspect
               | generic_map_aspect
               | port_map_aspect
               | generic_map_aspect port_map_aspect
               ;


entity_aspect  : ENTITY name   { search(table,$2); }
               | CONFIGURATION name  { FILE *temp= NULL;//fopen(strcat($2, ".sim"),"r"); 
                                       if( temp ) 
					   cout<<"can not find the configuration"; }
	       | OPEN
               ;


generic_map_aspect
               : GENERIC MAP '(' association_list ')'
               ;


port_map_aspect: PORT MAP  '(' association_list ')'
               ;
     

disconnection_specification
               : DISCONNECT guarded_signal_specification AFTER expression ';'
               ;


guarded_signal_specification
               : signal_list ':' ID //mark
                           { Type *t= search(table,$3);//table->lookup($3);
			     delete t; }
               ;


signal_list    : signal_id_list
               | OTHERS
               | ALL
               ;


signal_id_list : ID         { table->add_symbol($1,UnknownN,UnknownD); }
               | signal_list ',' ID
                            { table->add_symbol($3,UnknownN,UnknownD); }
               ;


use_clause     : USE_CLAUSE {  yyparse();  } 
               ;

/*
snsn           : name     {  FILE *oldin= yyin; 
                             strcat($1,".vhd"); 
                             FILE *infile= fopen($1,"r"); 
                             if( !infile ) {
			         cout<<" can not open file\n";  fclose(infile);
				 YYABORT; }   
			     else { 
			         yyin= infile;
			         yyparse();  
			         fclose(yyin); }
			     yyin= oldin; }
               | snsn ',' name {}
               ;
	       */

initialization_specification
               : INITIALIZE signal_specification TO expression ';'
               ;

signal_specification
               : signal_list ':' mark
               ;


label_declaration
               : ID   {  table->add_symbol($1,UnknownN,UnknownD); }
                 statement_name_list ';' 
               ;


statement_name_list
               : statement_name
               | statement_name_list ',' statement_name
               ;

statement_name : ID    { table->add_symbol($1,UnknownN,UnknownD); }
               | label_array
               ;

label_array    : ID  { table->add_symbol($1,UnknownN,UnknownD); } index_constraint
               ;

/* Type Definitions */

enumeration_type_definition
               : '(' ee ')'  { $$= new Type(UnknownN,Enumeration); } 
               ;

ee             : enumeration_literal 
                             { table->add_symbol($1,UnknownN,Enumeration); }
               | ee ',' enumeration_literal
                             { table->add_symbol($3,UnknownN,Enumeration); }
                             
               ;

enumeration_literal
               : ID          { $$= $1; }
               | CHARLIT     { $$= $1; }
               ;

integer_floating_type_definition /* int and float type syntactically same */
               : range_constraint 
                             { $$= $1; }
               ;

range_constraint
               : RANGE range { $$= $2; }
               ;


range          : attribute_name { $$= $1; } 
               | simple_expression direction simple_expression 
                                { if( !($1->datatype() == $3->datatype()) && 
                                      !($1->isError() || $3->isError()) ) 
				      errmsg("left and right hand side of 'to' and 'downto' should be integer"); }
	       ;


direction      : TO
               | DOWNTO
               ;



physical_type_definition
               : range_constraint UNITS  base_unit_declaration  sud  END UNITS
                              { $$= new Type(UnknownN,Physic); }
               | range_constraint UNITS  base_unit_declaration  sud  END UNITS ID
                              { $$= new Type(UnknownN,Physic); }                     
               ;

sud            :
               | sud secondary_unit_declaration
               ;

base_unit_declaration
              : ID ';'        { table->add_symbol($1,UnknownN,Physic); }
              ;

secondary_unit_declaration
               : ID           { table->add_symbol($1,UnknownN,Physic); }
                 '=' physical_literal ';'
               ;

array_type_definition
              : unconstrainted_array_definition
                              { $$= new Type($1); }
              | constrained_array_definition
                              { $$= new Type($1); }
              ;

unconstrainted_array_definition
               : ARRAY '(' isd ')' OF subtype_indication
                              { if( $6->isArray() )
				    $$= new Type(UnknownN,Array,$6->Subtype());
			        else
				    $$= new Type(UnknownN,Array,$6->datatype());  } 
               ;

constrained_array_definition
               : ARRAY index_constraint OF subtype_indication
                              { if( $4->isArray() )
				    $$= new Type(UnknownN,Array,$4->Subtype()); 
			        else
				    $$= new Type(UnknownN,Array,$4->datatype());  } 
               ;

isd            : index_subtype_definition
               | isd index_subtype_definition
               ;

index_subtype_definition
               : mark RANGE BOX { Type *t= search(table,$1);
	                           /*if( !(t->isInt() || t->isError()) )
				      errmsg(" the index of array should be integer");*/ }
               ;

index_constraint
               : '(' disdis ')'
               ;

disdis         : discrete_range
               | disdis ',' discrete_range
               ;

record_type_definition
               : RECORD elde END RECORD      { $$= new Type(UnknownN,Record); }
               | RECORD elde END RECORD ID   { $$= new Type(UnknownN,Record); } 
               ;


elde           : element_declaration
               | elde element_declaration
               ;


element_declaration
               : identifier_list ':' subtype_indication ';' 
                              { while( h->next ){
			               table->add_symbol(h->str,UnknownN,$3->datatype());
				       h= h->next; }  }
               ;



access_type_definition
              : ACCESS subtype_indication  { $$= new Type(UnknownN,Access,$2->datatype()); } 
              ;


file_type_definition
               : VFILE OF name {}
               | VFILE size_constraint OF name {}
               ;

size_constraint: '(' expression ')'
               ;

subtype_declaration
               : SUBTYPE ID IS subtype_indication ';'
                                  { Type *t= new Type(TypeN,$4->datatype(),$4->Subtype() );
                                    table->add_symbol($2,t);  }
               ;

subtype_indication 
               : mark                        {  $$= search(table,$1); }
               | mark gen_association_list   {  $$= search(table,$1); }
               | subtype_indication1         {  $$= $1; }
               ;

subtype_indication1 
               : mark range_constraint          { $$= search(table,$1); }
               | mark mark range_constraint     { $$= search(table,$1); }
               | mark mark                      { $$= search(table,$1); }
               | mark mark gen_association_list { $$= search(table,$1); } 
               ;

discrete_range : range {}
               | subtype_indication { /*if( !$1->isInt() )
                                          errmsg("the index of array should be integer");*/ }
               ;

discrete_range1: simple_expression direction simple_expression 
                                    { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError()) ) {
				          $$= new Type(ErrorN,ErrorD);
                                          errmsg(" the types on both sides of 'to' and 'downto' should be same"); }
				      else 
					  $$= $1;  }
               | subtype_indication1{ $$= $1; }
               ;

/* Concurrent Statements */

concurrent_statement
               : block_statement {  SymTab *temp= table->header;
				    delete table;
				    table= temp; }
               | process_statement
                                 {  SymTab *temp= table->header;
				    delete table;
				    table= temp; }
               | concurrent_procedure_call
               | concurrent_assertion_statement 
               | concurrent_signal_assignment_statement
               | component_instantiation_statement
               | generate_statement
               ;


block_statement
              : block_label is_symbol block_header  block_declarative_part 
                VBEGIN block_statement_part END BLOCK ';' {}
              | block_label guarded_exp is_symbol block_header  block_declarative_part 
                VBEGIN block_statement_part END BLOCK ';' {}
              | block_label is_symbol block_header  block_declarative_part 
                VBEGIN block_statement_part END BLOCK ID ';'
                       { Type *t= search(table,$9);
			 delete t; }
              | block_label guarded_exp is_symbol block_header block_declarative_part
                VBEGIN  block_statement_part END BLOCK ID ';'
                       { Type *t= search(table,$10);
			 delete t; }  
              ;

guarded_exp   : '(' expression ')' 
                               { if( !($2->isBool() || $2->isError()) )
                                     errmsg(" guarded block required a bollean expression"); }
              ;


block_label   : label BLOCK { table->other_tab = new SymTab;
			      SymTab *temp= table;
			      table=table->other_tab;
			      table->header = temp;  }
              ;

is_symbol     :
              | IS
              ;


block_header  : 
              | generic_clause
              | generic_clause generic_map_aspect ';'
              | port_clause
              | port_clause          { trans1= table; table= table->header;}
                port_map_aspect ';'  { table= trans1;}
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
               :
               | block_statement_part concurrent_statement
               ;


block_declarative_item
              : subprogram_declaration
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
               | use_clause
              | variable_declaration
              | group_template_declaration
              | group_declaration
              ;


process_statement
               : process_head process_tail process_declarative_part
                 VBEGIN process_statement_part END postpone PROCESS ';'
               | process_head process_tail process_declarative_part
                 VBEGIN process_statement_part END postpone PROCESS ID ';'
                    { Type *t= search(table,$9);//table->lookup($10);
		      delete t; } 
               ;


process_head   : PROCESS     {  table->other_tab = new SymTab;
				SymTab *temp = table;
				table=table->other_tab;
				table->header = temp;  }
               | label PROCESS
                             {  table->other_tab = new SymTab;
				SymTab *temp = table;
				table=table->other_tab;
				table->header = temp;  }
               | POSTPONED PROCESS
                             {  table->other_tab = new SymTab;
				SymTab *temp = table;
				table=table->other_tab;
				table->header = temp;  }
               | label POSTPONED PROCESS
                             {  table->other_tab = new SymTab;
				SymTab *temp = table;
				table=table->other_tab;
				table->header = temp;  }
                      
                   
               ;

postpone       :
               | POSTPONED
               ;


process_tail   : is_symbol
               | '(' sensitivity_list ')' is_symbol
               ;


sensitivity_list
               : name      { Type *t= search(table,$1); table->add_symbol($1,t); } 
               | sensitivity_list ',' name
                           { Type *t= search(table,$3); table->add_symbol($3,t); }  
               ;


process_declarative_part
               :
               | process_declarative_part process_declarative_item
               ;


process_declarative_item
               : subprogram_declaration
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
               | use_clause
               | group_template_declaration
               | group_declaration
               ;


process_statement_part
               : 
               | process_statement_part sequential_statement 
               ;


concurrent_procedure_call
               : procedure_call_statement
               | label procedure_call_statement
               | POSTPONED procedure_call_statement
               | label POSTPONED procedure_call_statement 
               ;


component_instantiation_statement
               : label COMPONENT name  ';'
                          { SymTab *temp= search1(table,$3); delete temp; }
               | label cmpnt_stmt_body generic_map_aspect ';'
	       | label cmpnt_stmt_body port_map_aspect ';'
               | label cmpnt_stmt_body generic_map_aspect  port_map_aspect ';'{}
               ;


cmpnt_stmt_body: name     { trans1= search1(table,$1);
			    if( !trans1 )  
			        errmsg(" can't find component");  }  
               | COMPONENT name
		          { trans1= search1(table,$2);
			    if( !trans1 ) 
               			errmsg(" can't find component");  }
               | ENTITY name  { if( !search1(table,$2) ) {
		                    errmsg(" entity name did not declared. EXIT abnormally!!");
	                            YYABORT; }
	                        else 
				    trans1= search1(table,$2);  }
               | CONFIGURATION name
               ;


concurrent_assertion_statement
               : assertion_statement
               | label assertion_statement 
               | POSTPONED assertion_statement
               | label POSTPONED assertion_statement
               ;


concurrent_signal_assignment_statement
               : conditional_signal_assignment
               | selected_signal_assignment
               | label conditional_signal_assignment {}
               | label selected_signal_assignment {} 
               | POSTPONED conditional_signal_assignment
               | POSTPONED selected_signal_assignment
               | label POSTPONED conditional_signal_assignment{}
               | label POSTPONED selected_signal_assignment {}
               ;


conditional_signal_assignment
               : target LE options conditional_waveforms ';'
                             { if( ($1->datatype() != $4->datatype()) && !($1->isError() || $4->isError()) ) 
                                   errmsg(" the target and the assigned value have different types"); }
               ;


target         : name  { Type *t= search(table,$1);
                         if( t->isArray() )
			     $$= new Type(t->nametype(),t->Subtype()); 
                         else 
			     $$= t; }
               | aggregate  { $$= $1; } 
               ;


options        :
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
                                { if( $1->isUnknown() )
		                      $$= $2; 
     				  else if( ($1->datatype() != $2->datatype()) && !($1->isError() || $2->isError()) ) {
                                      $$= new Type(ErrorN,ErrorD);
				      errmsg(" the assigned values on right side of '<=' have different types"); } }
               | waveform_head waveform WHEN expression
                                { if( !($4->isBool || $4->isError()) ) {
				      errmsg(" the expression after 'when' is not boolean type");
				      $$= new Type(ErrorN,ErrorD); }
                                  else if( $1->isUnknown() )
		                      $$= $2; 
 				  else if( ($1->datatype() != $2->datatype()) && !($1->isError() || $2->isError()) ) {
                                      $$= new Type(ErrorN,ErrorD);
				      errmsg(" the assigned values on right side of '<=' have different types"); } }
               ;


waveform_head  :               { $$= new Type(UnknownN,UnknownD); }
               | waveform_head waveform WHEN expression ELSE
                               { if( !($4->isBool || $4->isError()) ) {
				     errmsg(" the expression after 'when' is not boolean type");
                                     $$= new Type(ErrorN,ErrorD); }
			         if( $1->isUnknown() )
                  		     $$= $2; 
			         else if( ($1->datatype() != $2->datatype()) && !($1->isError() || $2->isError())  ) {
				     $$= new Type(ErrorN,ErrorD);
			             errmsg(" different types on right side of '<='"); } }
               ;



selected_signal_assignment
               : WITH expression SELECT target LE options selected_waveforms ';'
                       { if( ($4->datatype() != $7->datatype()) && !($4->isError() || $7->isError()) )
			     errmsg(" the target and assigned valued have different types");
                         if( ($2->datatype() != $7->Subtype()) && !($2->isError() || ($7->Subtype()==ErrorD)) )
			     errmsg(" the types of expressons after 'when' and 'with' don't match"); }
               ;


selected_waveforms
               : waveform WHEN choices  { $$= new Type(UnknownN,$1->datatype(),$3->datatype()); }  
               | selected_waveforms ',' waveform WHEN choices 
                                { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError()) ) {
				      errmsg(" the values on both sides of '<=' should be same types");
                                      $$= new Type(UnknownN,ErrorD,ErrorD);  }
                                  else if( ($1->Subtype() != $5->datatype()) && 
					  !(($1->Subtype()==ErrorD) || $5->isError() 
                                          || $1->isUnknown() || $5->isUnknown()) ) {
				      errmsg(" the selection after 'when' should be same types");
				      $$= new Type(UnknownN,ErrorD,ErrorD); }
				  else 
				    $$= $1; }
               |   { $$= new Type(ErrorN,ErrorD); }
               ;


generate_statement
               : label generation_head generate_declarative_part
                 concurrent_statement_sequence  END GENERATE generate_tail ';'
               | label generation_head generate_declarative_part
                 VBEGIN concurrent_statement_sequence END GENERATE generate_tail ';'
               ;


generation_head: generation_scheme GENERATE
                         {  table->other_tab = new SymTab;
			    SymTab *temp = table;
			    table=table->other_tab;
			    table->header = temp;  }
               ;


generate_tail  :
               | ID      {  Type *t= search(table,$1);
			    delete t;
			    SymTab *temp= table->header;
			    delete table;
			    table= temp;  }
               ;


generate_declarative_part
               : generate_declarative_part block_declarative_item
               | 
               ;


concurrent_statement_sequence
               : concurrent_statement
               | concurrent_statement_sequence concurrent_statement
               ;


generation_scheme
               : FOR parameter_specification 
               | IF expression  { if( !($2->isBool() || $2->isError()) )
				     errmsg(" the 'if' should be followed a boolean expression"); } 
               ;


parameter_specification
               : ID IN discrete_range  {}
	       ;


/* Sequential Statements */

sequential_statement
               : wait_statement 
               | assertion_statement
               | signal_assignment_statement
               | variable_assignment_statement
               | procedure_call_statement
               | if_statement
               | case_statement
               | loop_statement
               | next_statement
               | exit_statement
               | return_statement
               | null_statement
               | report_statement
               ;


wait_statement : label_symbol WAIT ';'
               | label_symbol WAIT sensitivity_clause ';'
               | label_symbol WAIT condition_clause ';'
               | label_symbol WAIT timeout_clause ';'
               | label_symbol WAIT sensitivity_clause condition_clause ';'
               | label_symbol WAIT sensitivity_clause timeout_clause ';'
               | label_symbol WAIT sensitivity_clause condition_clause timeout_clause ';'
               | label_symbol WAIT condition_clause timeout_clause ';'  
               ;


condition_clause
               : UNTIL expression  { if( !($2->isBool() || $2->isError()) )
                                        errmsg(" the expression after 'until' is not boolean type"); }
               ;


sensitivity_clause
               : ON sensitivity_list
               ;


timeout_clause : FOR expression 
               ;


assertion_statement
               : ASSERT expression
                            { if( !($2->isBool() || $2->isError()) )
				  errmsg(" the 'assert' should be followed a boolean expression"); } 
                 tralass ';'
               ;


condition      : expression { if( !($1->isBool() || $1->isError()) )
				  errmsg(" the expression following 'if' should be boolean type");  }
               ;


tralass        :
               | REPORT expression   
               | SEVERITY ID { table->checkseverity($2); }
               | REPORT expression SEVERITY ID { table->checkseverity($4); }
               ;


signal_assignment_statement
               : target LE delay_mechanism waveform ';'
                                   { if( ($1->datatype() != $4->datatype()) && !($1->isError() || $4->isError()))
                                         errmsg(" left and right sides of '<=' have defierent types"); }
               | target LE waveform ';'
                                   { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError()))
                                         errmsg(" left and right sides of '<=' have defierent types"); }
               | label target LE delay_mechanism waveform ';'
                                   { if( ($2->datatype() != $5->datatype()) && !($2->isError() || $5->isError()))
                                         errmsg(" left and right sides of '<=' have defierent types"); }
               | label target LE waveform ';'
                                   { if( ($2->datatype() != $4->datatype()) && !($2->isError() || $4->isError()))
                                         errmsg(" left and right sides of '<=' have defierent types"); } 
               ;


waveform       : waveform_element              { $$= $1; }
               | waveform ',' waveform_element 
                                    { if( ($1->datatype()== $3->datatype()) || !( $1->isError() || $3->isError()) )
                                          $$= $1; 
				      else if( $1->isUnknown() )
					  $$= $3; }
               | UNAFFECTED   { $$= new Type(UnknownN,UnknownD); }
               ;


waveform_element
               : expression                    { $$= $1; }
               | expression AFTER expression   { if( !($3->isPhysic() || $3->isError()) )
		                                     errmsg("the value following 'after' should be physical type");
		                                 $$= $1; }
               ;


variable_assignment_statement
               : label target ASSIGN  expression ';'
                                 { if( ($2->datatype() != $4->datatype()) && !($2->isError() || $4->isError()))
                                       errmsg(" left and right sides of ':=' have defierent types"); } 
               | target ASSIGN  expression ';' 
                                 { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError()))
                                       errmsg(" left and right sides of ':=' have defierent types"); } 
               ;



/* Adding label_symbol or association list creates conflicts */

procedure_call_statement
               : name ';' { /*Type *t= search(table,$1); delete t;*/ }
               ;

if_statement   : label_symbol IF condition THEN sequence_of_statements elsifthen else_part END IF ID ';'
                       { Type *t= search(table,$10); delete t;  }
               | label_symbol IF condition THEN sequence_of_statements elsifthen else_part END IF ';'
               ;


else_part      : 
               | ELSE sequence_of_statements
               ;


label_symbol   :
               | label
               ;

label          : ID ':' { table->add_symbol($1,UnknownN,UnknownD); }
               ;


elsifthen      :
               | elsifthen ELSIF expression
                            { if( !($3->isBool() || $3->isError()) )
				  errmsg(" the 'elsif' should be followed a boolean type"); }
                 THEN sequence_of_statements
               ;


sequence_of_statements
               :
               | sequence_of_statements sequential_statement 
               ;


case_statement : label_symbol CASE expression IS case_statement_alternative
                 END CASE ID ';'
                       { Type *t= search(table,$8); delete t; }
               | label_symbol  CASE expression IS case_statement_alternative
                 END CASE ';'
               ;


case_statement_alternative
               : WHEN choices ARROW sequence_of_statements
               | case_statement_alternative
                         WHEN choices ARROW sequence_of_statements
               ;

loop_statement : label_symbol iteration_scheme LOOP sequence_of_statements
                 END LOOP  ';'
                            {  SymTab *temp= table->header;
			       delete table;
			       table= temp;  }
               | label_symbol  iteration_scheme LOOP sequence_of_statements
                 END LOOP ID ';'
                            { Type *t= search(table,$7);
			      SymTab *temp= table->header;
			      table= temp;  }
               ;

iteration_scheme
               :           {  table->other_tab = new SymTab;
			      SymTab *temp = table;
			      table=table->other_tab;
			      table->header = temp;  }
               | WHILE expression
                           {  table->other_tab = new SymTab;
			      SymTab *temp = table;
			      table=table->other_tab;
			      table->header = temp; 
                              if( !($2->isBool() || $2->isError()) )
				  errmsg(" the 'when' should be followed a boolean type"); }
               | FOR ID IN discrete_range
                           {  table->other_tab = new SymTab;
			      SymTab *temp = table;
			      table=table->other_tab;
			      table->header = temp; 
			      table->add_symbol($2,UnknownN,Int); }
               ;

next_statement : label_symbol NEXT ';'
               | label_symbol NEXT ID ';'
                       { Type *t= search(table,$3);
			 delete t; }
               | label_symbol NEXT WHEN expression ';'
                            { if( !($4->isBool() || $4->isError()) )
		                  errmsg(" the 'when' should be followed a boolean type"); }
               | label_symbol NEXT ID
                       { Type *t= search(table,$3);
			 delete t; }
		 WHEN expression ';' 
                            { if( !($6->isBool() || $6->isError()) )
		                  errmsg(" the 'when' should be followed a boolean type"); }
               ;

exit_statement : label_symbol EXIT ';'
               | label_symbol EXIT ID ';'
                       { Type *t= search(table,$3);
			 delete t; }
               | label_symbol EXIT WHEN expression ';' 
                                    {if( !($4->isBool() || $4->isError()) )
		                          errmsg(" the 'when' should be followed a boolean type"); }  
               | label_symbol EXIT ID
                       { Type *t= search(table,$3); delete t; }
                 WHEN expression ';' { if( !($6->isBool() || $6->isError()) )
		                          errmsg(" the 'when' should be followed a boolean type"); }
               ;

return_statement
               : label_symbol RETURN ';'
               | label_symbol RETURN expression ';'
               ;


null_statement : label_symbol VNULL ';'
               ;


report_statement
               : label_symbol REPORT expression ';'
               | label_symbol REPORT expression SEVERITY ID ';'
                        { table->checkseverity($5); }
               ;



/* Interfaces and Associations */

interface_list : interface_declaration
               | interface_list ';' interface_declaration
               ;


interface_declaration
               : interface_constant_declaration
               | interface_signal_declaration
               | interface_variable_declaration
               | interface_file_declaration
               | interface_unknown_declaration
               ;


interface_constant_declaration
               : CONSTANT identifier_list ':' subtype_indication
                         { while(h->next){
                                Type *t= new Type(Constant, $4->datatype(), $4->Subtype() );
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next; }			  
			 }
                 assign_exp
                          { if( ($4->datatype() != $6->datatype()) && 
                               !($4->isError() || $6->isError() || $6->isUnknown()) )
		                 errmsg(" wrong type value assigned to a variable"); }
               | CONSTANT identifier_list ':' IN subtype_indication
                         { while(h->next){
                                Type *t= new Type(Constant, $5->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next; }			  
			 }
                 assign_exp
                          { if( ($5->datatype() != $7->datatype()) && 
                               !($5->isError() || $7->isError() || $7->isUnknown()) )
		                 errmsg(" wrong type value assigned to a constant"); }
               ;

interface_signal_declaration
               : SIGNAL identifier_list  ':' subtype_indication
                           { while(h->next){
                                Type *t= new Type(Signal, $4->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next; }
			   }
                 bus_symbol assign_exp 
                          { if( ($4->datatype() != $7->datatype()) && 
                               !($4->isError() || $7->isError() || $7->isUnknown()) )
		                 errmsg(" wrong type value assigned to a signal"); }
               | SIGNAL identifier_list ':' mode subtype_indication
                           { while(h->next){
			        Type *t= new Type(Signal, $5->datatype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next;  }
			   }
                 bus_symbol assign_exp
                           { if( ($5->datatype() != $8->datatype()) && 
                                !($5->isError() || $8->isError() || $8->isUnknown()) )
		                 errmsg(" wrong type value assigned to a signal"); }
               ;


interface_variable_declaration
               : VARIABLE identifier_list ':' subtype_indication
                          { while(h->next){
			        Type *t;
                                if( $4->isArray() )
                                    t= new Type(Variable, $4->Subtype());
				else
				    t= new Type(Variable, $4->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next; }
			   }
                 assign_exp 
                           { if( ($4->datatype() != $6->datatype()) && 
                                !($4->isError() || $6->isError() || $6->isUnknown()) )
		                 errmsg(" wrong type value assigned to a variable"); }
               | VARIABLE identifier_list ':' mode subtype_indication
                          { while(h->next){
			        Type *t= new Type(Variable, $5->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next; }
			   }
                 assign_exp
                          { if( ($5->datatype() != $7->datatype()) && 
                                !($5->isError() || $7->isError() || $7->isUnknown()) )
		                 errmsg(" wrong type value assigned to a variable"); }
               ;


bus_symbol     :
               | BUS
               ;


assign_exp     :                    { $$= new Type(ErrorN,UnknownD); }
               | ASSIGN expression  { $$= $2; }
               ;


interface_unknown_declaration   
               : identifier_list ':' subtype_indication 
                           { while(h->next){ 
			        Type *t= new Type(UnknownN, $3->datatype(), $3->Subtype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next;  }
                            }
                 bus_symbol assign_exp
                          { if( ($3->datatype() != $6->datatype()) && 
                                !($3->isError() || $6->isError() || $6->isUnknown()) )
		                 errmsg(" wrong type value assigned to a object"); }
               | identifier_list ':' mode subtype_indication
                           { while(h->next){ 
                                Type *t= new Type(UnknownN, $4->datatype(), $4->Subtype());
			        table->add_symbol(h->str,t);
				delete t;
				h= h->next;  } 
                             }
                 bus_symbol assign_exp
                            { if( ($4->datatype() != $7->datatype()) && 
                                 !($4->isError() || $7->isError() || $7->isUnknown()) )
		                  errmsg(" wrong type value assigned to a object"); }
               ;


interface_file_declaration
               : VFILE identifier_list ':' subtype_indication_list
                           {  while(h->next){
                                Type *t= new Type(UnknownN,File,$4->datatype());
			        table->add_symbol(h->str,t);
                                delete t;
				h= h->next; }
                            }
               ;


subtype_indication_list
               : subtype_indication { $$= $1; }   //????????????
               | subtype_indication_list ',' subtype_indication { $$= $3; }  //????????


mode           : IN | OUT | INOUT | BUFFER | LINKAGE
               ;


gen_association_list
               : '(' gen_association_list_1 ')'
               ;

gen_association_list_1
               : gen_association_element  
               | gen_association_list_1 ',' gen_association_element
                       {  if( ($1->datatype()!=$3->datatype()) && !($1->isError() || $3->isError()) ) 
			      errmsg(" left and right sides of ',' have different types");  }
               ;  

gen_association_element
               : expression      { if( !$1->isArray() )   $$= $1;
                                   else if( $1->isArray() )
					    $$= new Type($1->nametype(),$1->Subtype(), UnknownD); }
               | discrete_range1 { $$= $1 ;}
               | formal_part ARROW actual_part
                                 { if( ($1->datatype()!=$3->datatype()) && !($1->isError() || $3->isError()) ) {
				       $$= new Type(ErrorN,ErrorD); 
				       errmsg(" left and right sides of '=>' are different"); }
				   else
				       $$= $1; }
               | OPEN { $$= new Type(UnknownN,UnknownD); }
               ;


association_list
               : association_element
               | association_list ',' association_element
               ;

association_element
               : actual_part  {}
               | formal_part ARROW actual_part 
                              { Datatype d1,d3;
			        if( $1->isArray() ) 
				    d1= $1->Subtype();
				else
				    d1= $1->datatype();
				if( $3->isArray() ) 
				    d3= $3->Subtype();
				else
				    d3= $3->datatype();
				if( (d1 != d3) && (d3 !=UnknownD) && !($1->isError() || $3->isError()) ){
				    errmsg("left and right sides of '=>' have different types");}  }
               ;


formal_part    : name { Type *t= search(trans1,$1); 
                        if( t->isArray() ) 
			    $$= new Type(t->nametype(), t->Subtype());
                        else 
			    $$= t; }
               ;


actual_part    : expression { if( $1->isArray() )
                                  $$= new Type($1->nametype(), $1->Subtype());
                              else { 
				$$= $1;  } }
               | OPEN       { $$= new Type(UnknownN,UnknownD); }
               ;



/* Expressions */

expression     : relation andexpr 
                          { if( ($1->datatype() != $2->datatype()) && !($1->isError() || $2->isError()) ) {
                                errmsg(" different types on left and right side of 'and' operator");
                                $$= new Type(ErrorN,ErrorD);  }
                            else 
                                $$= $1;  }
               | relation orexpr
                          { if( ($1->datatype() != $2->datatype()) && !($1->isError() || $2->isError())  ) {
                                errmsg(" different types on left and right side of 'or' operator");
                                $$= new Type(ErrorN,ErrorD); }
                            else 
                                $$= $1;  }
               | relation xorexpr
                          { if( ($1->datatype() != $2->datatype()) && !($1->isError() || $2->isError()) ) {
                                errmsg(" different types on left and right side of 'xor' operator");
                                $$= new Type(ErrorN,ErrorD); }
                            else 
                                $$= $1;  }
               | relation NAND relation
                          { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError()) ) {
                                errmsg(" different types on left and right side of 'nand' operator");
                                $$= new Type(ErrorN,ErrorD); }
                            else 
                                $$= $1;  }
               | relation NOR relation
                          { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError())  ) {
                                errmsg(" different types on left and right side of 'nor' operator");
                                $$= new Type(ErrorN,ErrorD); }
                            else 
                                $$= $1;  }
               | relation XNOR relation
                          { if( ($1->datatype() != $3->datatype())  && !($1->isError() || $3->isError())  ) {
                                errmsg(" different types on left and right side of 'xnor' operator");
                                $$= new Type(ErrorN,ErrorD); }
                            else 
                                $$= $1;  }
               | relation { $$= $1; if( $$->isBool() ) }
               ;

andexpr        : andexpr AND relation
                         { if( $1->datatype() != $3->datatype() && !($1->isError() || $3->isError()) ) {
                               errmsg(" different types on left and right side of 'and' operator");
                               $$= new Type(ErrorN,ErrorD);  }
                           else 
                               $$= $1;  }
               | AND relation  { $$= $2; }
               ;
	       
orexpr         : orexpr OR relation  
                        { if( $1->datatype() != $3->datatype() && !($1->isError() || $3->isError())  ) {
                              errmsg(" different types on left and right side of 'or' operator");
                              $$= new Type(ErrorN,ErrorD); }
                          else 
                              $$= $1;  }
               | OR relation   { $$= $2; }
               ;

xorexpr        : xorexpr XOR relation 
                         { if( $1->datatype() != $3->datatype() && !($1->isError() || $3->isError())  ) {
                               errmsg(" different types on left and right side of 'xor' operator");
                               $$= new Type(ErrorN,ErrorD); }
                           else 
                               $$= $1;  }
               | XOR relation  { $$= $2; }
               ;

relation       : shift_expression   { $$= $1; }
               | shift_expression relational_operator shift_expression
                                 { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError()) ) {
				       errmsg(" different types on left and right side of relation operator"); 
				       $$= new Type(ErrorN,ErrorD); }
				   else { 
				       $$= new Type(UnknownN,Bool);  } }
               ;

relational_operator
               : '=' | NEQ | '<' | LE | '>' | GE
               ;


shift_expression
               : simple_expression   { $$= $1; }
               | simple_expression shift_operators simple_expression
                                     { if( !$3->isInt() ) {
				           $$= new Type(ErrorN,ErrorD);
				           errmsg(" expecting an integer"); } 
				       else 
					   $$= $1;  }
               ;


shift_operators
               : SLL | SRL | SRA | SLA | ROL | ROR
               ;



simple_expression
               : terms      { $$= $1; }
               | sign terms { $$= $2; }
               ;

sign           : '+' %prec MED_PRECEDENCE
               | '-' %prec MED_PRECEDENCE
               ;


terms          : term        { $$= $1; }
               | terms adding_operator term
                             { $$= $1; } 
               ;


adding_operator: '+' | '-' | '&'
               ;


term           : term multiplying_operator factor
                             { $$= $1; }   
               | factor      { $$= $1; }
               ;


multiplying_operator
               : '*' | '/' | MOD | REM
               ;


factor         : primary      { $$= $1; }
               | primary EXPON primary
                              { $$= $1; }
               | ABS  primary { $$= $2; }
               | NOT  primary { $$= $2; }
               ;


primary        : name         { Type *t= search(table,$1);
                                if( t->isArray() )
				    $$= new Type(t->nametype(),t->Subtype()); 
                                else 
				    $$= t; }
               | attribute_name {$$= $1; }
               | literal      { $$= $1; }
               | aggregate    { $$= $1; }
               | qualified_expression 
                              { $$= $1; }
               | allocator    { $$= $1; } 
               | '(' expression ')' 
                              {  $$= $2; }
               ;


allocator      : NEW mark allocator1  
                              { $$= search(table,$2); } 
               | NEW mark mark allocator1 
                              { $$= search(table,$2); }
               | NEW qualified_expression
                              { $$= $2; }
               ;

allocator1     : 
               | gen_association_list
               ;




qualified_expression
               : mark '\'' '(' expression ')' 
                              { $$= $4; } 
               | mark '\'' aggregate 
                              { $$= $3; }
               ;

name           : mark   { $$= $1; }
               | name2  { $$= $1; }
               ;

mark           : ID            { $$= $1; }
               | selected_name { $$= $1; }
               ;


selected_name  : mark '.' suffix  { pathname= $1; $$=$3; }
               ;


name2          : name3 { $$= $1;}
//               | attribute_name { $$= $1; }
               ;

name3          : STRLIT    {   }
               | insl_name { $$= $1; }
               ;


suffix         : ID { $$= $1; }
               | CHARLIT {  }
               | STRLIT { $$= $1; }
               | ALL { fileattr= "all";}
               ;



/* Lots of conflicts */

attribute_name: mark '\'' idparan  { Type *t=search(table,$1);
                                     if( !t->isError() )
				         $$= checkattr(t,$3); 
                                     else
				         $$= t; }
              | name2 '\'' idparan { Type *t=search(table,$1);
                                     if( !t->isError() )
				         $$= checkattr(t,$3); 
                                     else
				         $$= t; }
              | name signature '\'' idparan { Type *t=search(table,$1);
                                     if( !t->isError() )
				         $$= checkattr(t,$4); 
                                     else
				         $$= t; }
              ;

idparan       : ID  { $$= $1; }
              | ID  { $$= $1; }
                '(' expression ')' 
              | RANGE { $$= "range"; }
              | RANGE '(' expression ')' {} 
              ;


insl_name     : mark gen_association_list  { $$= $1; }
              | name3 gen_association_list { $$= $1; }
              ;


literal       : INT      { $$= new Type(UnknownN,Int); }
              | REAL     { $$= new Type(UnknownN,Real);  } 
              | BASEDLIT { $$= new Type(UnknownN,Enumeration); }
              | CHARLIT  { $$= new Type(UnknownN,Enumeration); }
              | BITLIT   { $$= new Type(UnknownN,Enumeration); }
              | VNULL    { $$= new Type(UnknownN,UnknownD); } 
              | physical_literal_no_default { $$= $1; }
	      ;


physical_literal
              : ID            { Type *temp= search(table,$1);
                                if( temp->datatype() != Physic )
				    errmsg("expecting a physical type");
                                else 
                                    $$= temp; }
              | INT ID        { Type *temp= search(table,$2);
                                if( temp->datatype() != Physic )
				    errmsg("expecting a physical type");
                                else 
                                    $$= temp; }
              | REAL ID       { Type *temp= search(table,$2);
                                if( temp->datatype() != Physic )
				    errmsg("expecting a physical type");
                                else 
                                    $$= temp; }
              | BASEDLIT ID   { Type *temp= search(table,$2);
                                if( temp->datatype() != Physic )
				    errmsg("expecting a physical type");
                                else 
                                    $$= temp; }
              ;


physical_literal_no_default
              : INT ID        { Type *t= search(table,$2);
        	                if( !t->isPhysic() ) {
				    errmsg(" expecting a physical type");
				    $$= new Type(ErrorN,ErrorD); }
				else
	                            $$= new Type(UnknownN,Physic,Int); }
              | REAL ID       { $$= new Type(UnknownN,Real); }
              | BASEDLIT ID   { $$= new Type(UnknownN,Enumeration); }
              ;


aggregate     : '(' element_association ',' elarep ')' 
                              { if( ($2->datatype() != $4->datatype()) && !($2->isError() || $4->isError()) ) {
				    $$= new Type(ErrorN,ErrorD);
                                    errmsg(" wrong types between '(' and ')'");  }
			        else
                                    $$= $4; }
              | '(' choices ARROW expression ')'
                              { if( $2->isUnknown() )
				    $$= $4;
				else if( ($2->datatype()==$4->datatype()) && !($2->isError() || $4->isError()) )
				    $$= $2; 
                                else
      				    errmsg(" different types onleft and right sides of '=>' "); 
			            $$= new Type(ErrorN,ErrorD);  }
              ;
     

elarep        : element_association 
                              { $$= $1; }
              | elarep ',' element_association
                              { if( ($1->datatype() != $3->datatype()) && !($1->isError() || $3->isError()) ) {
				    $$= new Type(ErrorN,ErrorD);
                                    errmsg(" wrong types between '(' and ')'");  }
			        else
				    $$= $1; }
              ;


element_association 
              : expression    { $$= $1; }
              | choices ARROW expression 
                              { if( $1->isUnknown() )
					$$= $3;
				    else if( ($1->datatype()==$3->datatype()) && !($1->isError() || $3->isError()) )
				        $$= $1; 
                                    else
      				        errmsg(" different types onleft and right sides of '=>' "); 
			                $$= new Type(ErrorN,ErrorD);  }
              ;

choices       : choices '|' choice{ if( $1->isUnknown() )
					$$= $3;
				    else if( ($1->datatype()==$3->datatype()) && !($1->isError() || $3->isError()) )
				        $$= $1; 
                                    else
      				        errmsg(" different types onleft and right sides of '|' "); }
              | choice            { $$= $1; }
              ;

choice        : simple_expression { $$= $1; }
              | discrete_range1   { $$= $1; }
              | OTHERS            { $$= new Type(UnknownN,UnknownD);  }
              ;




%%
 
//extern char     g_src_name[];




int yyerror( char* s )
{
  
     int token;



//     printf("ERROR\n");

//     printf("Line %d: syntax error at ",linenumber);

       YYSTYPE yylval;

       while(token=yylex(&yylval)){
        
	 
         switch (token) {
	   case ID:
	   printf("%s\n",yylval.id);
	   break;
         case INT:
	   printf("%s\n",yylval.decimalit);
	   break;
         case REAL:
	   printf("%s\n",yylval.decimalit);
	   break;
         case BASEDLIT:
	   printf("%s\n",yylval.basedlit);
	   break;
         case CHARLIT:
	   printf("%s\n",yylval.charlit);
	   break;
         case STRLIT:
	   printf("%s\n",yylval.strlit);
	   break;
         case BITLIT:
	   printf("%s\n",yylval.bitlit);
//	   fprintf(lg,"%s\n",yylval.bitlit);
           break; 
	 case '(':
	   printf("(\n");
	//   fprintf(lg,"(\n"); break;
	 case ')':
	   printf(")\n");
	  // fprintf(lg,")\n"); break;
	 case '*':
	   printf("*\n");
	   //fprintf(lg,"*\n"); break;
	 case '/':
	   printf("/\n");
	  // fprintf(lg,"/\n"); break;
	 case '+':
	   printf("+\n");
	  // fprintf(lg,"+\n"); break;
	 case '-':
	   printf("-\n");
	   //fprintf(lg,"-\n"); break;
	 case ',':
	   printf(",\n");
	   //fprintf(lg,",\n"); break;
	 case '.':
	   printf(".\n");
	   //fprintf(lg,".\n"); break;
	 case ':':
	   printf(":\n");
	   //fprintf(lg,":\n"); break;
	 case ';':
	   printf(";\n");
	   //fprintf(lg,";\n"); break;
	 case '<':
	   printf("<\n");
	   //fprintf(lg,"<\n"); break;
	 case '=':
	   printf("=\n");
	  // fprintf(lg,"=\n"); break;
	 case '>':
	   printf(">\n");
	   //fprintf(lg,">\n"); break;
	 case '\'':
	   printf("\'\n");
	   //fprintf(lg,"\'\n"); break;
	 case '[':
	   printf("[\n");
	  // fprintf(lg,"[\n"); break;
	 case ']':
	   printf("]\n");
	   //fprintf(lg,"]\n"); break;
	 case '|':
	   printf("|\n");
	  // fprintf(lg,"|\n"); break;
	 case '&':
	   printf("&\n");
	  // fprintf(lg,"&\n"); break;
	 case '\\':
	   printf("\\\n");
	   //fprintf(lg,"\\\n"); break;

#define WHAT(x) case x:  printf("%s\n",#x); break;
	   
	   
        WHAT(ABS)
        WHAT(ACCESS)
        WHAT(AFTER)
    	WHAT(ALIAS)
        WHAT(ALL)
        WHAT(AND)
        WHAT(ARCHITECTURE)
        WHAT(ARRAY)
     	WHAT(ASSERT)
	WHAT(ATTRIBUTE)
        WHAT(VBEGIN)      //FAIL IF USE BEGIN
    	WHAT(BLOCK)
        WHAT(BODY)
        WHAT(BUFFER)
        WHAT(BUS)
        WHAT(CASE)
        WHAT(COMPONENT) 
        WHAT(CONFIGURATION)
        WHAT(CONSTANT)
        WHAT(DISCONNECT)
        WHAT(DOWNTO)
        WHAT(ELSE) 
        WHAT(ELSIF)
        WHAT(END)
        WHAT(ENTITY)
        WHAT(EXIT)
        WHAT(VFILE)
        WHAT(FOR) 
        WHAT(FUNCTION)
        WHAT(GENERATE)
        WHAT(GENERIC)
        WHAT(GUARDED)
        WHAT(IF) 
        WHAT(IN)
        WHAT(INITIALIZE)
        WHAT(INOUT)
        WHAT(IS)
        WHAT(LABEL) 
        WHAT(LIBRARY)
        WHAT(LINKAGE)
        WHAT(LOOP)
        WHAT(MAP)
        WHAT(MOD) 
        WHAT(NAND)
        WHAT(NEW)
        WHAT(NEXT)
        WHAT(NOR)
        WHAT(NOT) 
        WHAT(NULL )
        WHAT(OF)
        WHAT(ON)
        WHAT(OPEN)
        WHAT(OR) 
        WHAT(OTHERS)
        WHAT(OUT)
        WHAT(PACKAGE)
        WHAT(PORT)
        WHAT(PROCEDURE) 
        WHAT(PROCESS)
        WHAT(RECORD)
        WHAT(REGISTER)
        WHAT(REM)
        WHAT(REPORT) 
        WHAT(RETURN)
        WHAT(SELECT)
        WHAT(SEVERITY)
        WHAT(SIGNAL)
        WHAT(SUBTYPE) 
        WHAT(THEN)
        WHAT(TO)
        WHAT(TRANSPORT)
        WHAT(TYPE)
        WHAT(UNITS) 
        WHAT(UNTIL)
        WHAT(USE)
        WHAT(VARIABLE)
        WHAT(WAIT)
        WHAT(WHEN) 
        WHAT(WHILE)
        WHAT(WITH)
        WHAT(XOR)


	WHAT(IMPURE)
        WHAT(VNULL)
        WHAT(PURE)
        WHAT(RANGE)
        WHAT(SHARED)
        WHAT(LITERAL)
        WHAT(GROUP)
        WHAT(VREJECT)
        WHAT(INERTIAL)
        WHAT(UNAFFECTED)
        WHAT(XNOR)
        WHAT(SLL)
        WHAT(SRL)
        WHAT(SLA)
        WHAT(SRA)
        WHAT(ROL)
        WHAT(ROR)
        WHAT(ARROW)
        WHAT(EXPON)
        WHAT(ASSIGN)
        WHAT(NEQ)
        WHAT(GE)
        WHAT(LE)
        WHAT(BOX) 
	  default: YYABORT;
	      }
       YYABORT; 
     }
   }
  

/* sdg.y:185: parse error before `}' */



SymTab *new_table(SymTab *Table)
{
       Table->other_tab= new SymTab();
       SymTab *temp= Table;
       Table= Table->other_tab;
       Table->header= temp;
 
       return Table;
}
