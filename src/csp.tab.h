typedef union		yystacktype
 	  	{
 	  	   int		intval;
 	  	   double	floatval;
 	  	   char		*stringval;
                   actionADT    actionptr;
                   TERstructADT TERSptr;
		   delayADT     delayval;
		   bool         boolval;
 	  	} YYSTYPE;
#define	ID	258
#define	DELAY	259
#define	INT	260
#define	INIT	261
#define	MOD	262
#define	ENDMOD	263
#define	PROC	264
#define	ENDPROC	265
#define	BOOL	266
#define	PORT	267
#define	ARROW	268
#define	PARA	269
#define	DEL	270
#define	PAS	271
#define	ACT	272
#define	BOOLEAN	273
#define	SLASH	274
#define	SKIP	275


extern YYSTYPE yylval;
