#ifndef _loadg_h
#define _loadg_h

#define UNDEF      -1

#define NULL_CHAR         ((char)   NULL)
#define NULL_CHAR_PTR     ((char *) NULL)

#define Realloc(p,size)   ((p==NULL)?(GetBlock(size)):(realloc(p,size)))

int Hash_Get_Map (char *string, int max_size);

typedef struct node_
{
  int  type;
  bool keep;
  bool abstract;
  bool cont;
  bool noninp;
  bool failtrans;
  bool nondisabling;
  int trate;
  int lrate;
  int lrange;
  int urange;
  int urate;
  char *name;
  char *next;
  int  id;
  struct node_ **succs;
  struct node_ **preds;
  int num_of_succ;
  int num_of_pred;
  int *lower;
  int *upper;
  int *plower;
  int *pupper;
  int *predtype;
  int *weight;
  int ldelay;
  int udelay;
  char **expr;
  char **ruletype;
  bool *disabling;
  double *rate;
  char dir;
  int position;
  struct node_ *signal;
  char *hsl;
  char *transRate;
  char *delayExpr;
  char *priorityExpr;
  ineqADT inequalities;
} Node, *Node_Ptr;

typedef struct mark_
{
  int type;
  int markl;
  int marku;
  int *bstate;
  Node *src;
  Node *des;
  struct mark_ *next;
} mark_elem, *marking_list;
	    
#define TRANSITION_NODE 0
#define PLACE_NODE 1
#define INPUT_NODE 2
#define OUTPUT_NODE 3
#define INTERNAL_NODE 4
#define DUMMY_NODE 5
#define VAR_NODE 6
#define INT_NODE 7

#define EDGE_MARK 0
#define PLACE_MARK 1
#define RATE_MARK 2
#define VAR_MARK 3
#define INT_MARK 4

#define Mark_Src(m) ((m)->src)
#define Mark_Des(m) ((m)->des)
#define Mark_Place(m) ((m)->des)
#define Mark_Next(m) ((m)->next)
#define Mark_Type(m) ((m)->type)
#define Mark_Markl(m) ((m)->markl)
#define Mark_Marku(m) ((m)->marku)
#define Mark_State(m) ((m)->bstate)

#define Node_Type(n) ((n)->type)
#define Node_Keep(n) ((n)->keep)
#define Node_Abstract(n) ((n)->abstract)
#define Node_Cont(n) ((n)->cont)
#define Node_FailTrans(n) ((n)->failtrans)
#define Node_NonDisabling(n) ((n)->nondisabling)
#define Node_LRange(n) ((n)->lrange)
#define Node_URange(n) ((n)->urange)
#define Node_NonInp(n) ((n)->noninp)
#define Node_Name(n) ((n)->name)
#define Node_Succs(n) ((n)->succs)
#define Node_Preds(n) ((n)->preds)
#define Node_Lower(n) ((n)->lower)
#define Node_Upper(n) ((n)->upper)
#define Node_PredType(n) ((n)->predtype)
#define Node_Plower(n) ((n)->plower)
#define Node_Pupper(n) ((n)->pupper)
#define Node_Weight(n) ((n)->weight)
#define Node_Expr(n) ((n)->expr)
#define Node_RuleType(n) ((n)->ruletype)
#define Node_Disabling(n) ((n)->disabling)
#define Node_Rate(n) ((n)->rate)
#define Node_Lrate(n) ((n)->lrate)
#define Node_Urate(n) ((n)->urate)
#define Node_Trate(n) ((n)->trate)
#define Node_Hsl(n) ((n)->hsl)
#define Node_TransRate(n) ((n)->transRate)
#define Node_DelayExpr(n) ((n)->delayExpr)
#define Node_PriorityExpr(n) ((n)->priorityExpr)
#define Node_Ldelay(n) ((n)->ldelay)
#define Node_Udelay(n) ((n)->udelay)
#define Node_Ineq(n) ((n)->inequalities)
#define Node_Num_of_Succ(n) ((n)->num_of_succ)
#define Node_Num_of_Pred(n) ((n)->num_of_pred)
#define Node_Next(n)  ((Node_Ptr)(n)->next)
#define Node_NextL(n)  ((n)->next)
#define Node_Num(n)   ((n)->id)
#define Node_Signal_Ptr(n) ((n)->signal)
#define Node_Dir(n) ((n)->dir)
#define Node_Position(n) ((n)->position)

Node *Enter_Trans (char *name, char *idx, char dir);
Node *Enter_IO (int type, char *name);
void Enter_FailTrans (char *name);
void Enter_NonDisabling (char *name);
void Enter_Keep (char *name);
void Enter_Abstract (char *name);
void Enter_Cont (char *name,int lrange,int urange);
void Enter_Trate (Node *node,int lrate,int urate);
void Enter_NonInp (char *name);
Node *Enter_Place (char *name, int add = 1);
Node *Enter_Variable(char *name);
void Enter_Mark (marking_list *ml, int type, Node *src, Node *des, 
		 int markl, int marku);
void Print_Node (Node *node);
void Print_Marking (marking_list);
void Enter_Pred (Node *pred, Node *succ);
void Enter_Succ (Node *pred, Node *succ, int l, int u, int pt, int pl, int pu,
		 char *hsl, char *expr, char *ruletype, bool disabling,
		 double rate, int weight);
marking_list Dup_Mark_List (marking_list ml);
void Free_Mark_List (marking_list ml);

void Print_IO_List (Node *list);
void Print_Node_List (Node *list);
void Add_IO (Node **list, Node *node);
void Add_Node (Node **list,Node *node);
void Enter_Prop(char *property);
void Enter_Hsl (Node *node, char *str);
void Enter_TransRate (Node *node, char *str);
void Enter_DelayExpr (Node *node, char *str);
void Enter_PriorityExpr (Node *node, char *str);
void Enter_Delay (Node *node, int lower, int upper);
ineqADT Enter_Inequality(Node *node, int type, int constant, int uconstant,
			 char *expr);

extern marking_list initial_marking;
extern Node *io_list;
extern Node* node_list;
extern Node* sstg_list;
extern int num_of_node;
extern int num_of_ios;
extern int num_of_dummys;

#define NAME_MAX_ENTRIES       1000

#define NAME_NULL_PTR     ((Name_Ptr) NULL)

typedef struct name_type_        /* A name entry consists of a   */
{   char              *string;   /* string,                      */
    Node              *node;    /* token type number, and       */
    struct name_type_ *next;     /* pointer to the next name in  */
} Name_Type, *Name_Ptr;  /* the same hash chain          */


#define  Name_String(x)      ((x)->string)   /* Select the string */
#define  Name_Node(x)        ((x)->node)    /* Select the token type */
#define  Name_Next(x)        ((x)->next)     /* Select the next name */


void     Name_Init_Table     ();
void     Name_Print_Stats    ();
Name_Ptr Name_Find_Str       (char *string);
Name_Ptr Name_Enter_Str      (char *string);
Name_Ptr Name_Find_Enter_Str (char *string);
void     Name_Debug_Graph    ();

void Set_Initial_State(char *init_state_char);

bool load_graph(designADT design);

bool load_lpn(designADT design);

int intexpr_gettok(char *expr,char *tokvalue,int maxtok,int *position); 

bool intexpr_L(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals, 
	       eventADT *events,int nevents, int nplaces);


#endif /* loadg.h */
