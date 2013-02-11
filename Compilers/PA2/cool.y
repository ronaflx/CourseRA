/*
*  cool.y
*              Parser definition for the COOL language.
*
*/
%{
  #include <iostream>
  #include <cstdio>
  #include "cool-tree.h"
  #include "stringtab.h"
  #include "utilities.h"
  
  extern char *curr_filename;
  
  
  /* Locations */
  #define YYLTYPE int              /* the type of locations */
  #define cool_yylloc curr_lineno  /* use the curr_lineno from the lexer
  for the location of tokens */
    
    extern int node_lineno;          /* set before constructing a tree node
    to whatever you want the line number
    for the tree node to be */
      
      
      #define YYLLOC_DEFAULT(Current, Rhs, N)         \
      Current = Rhs[1];                             \
      node_lineno = Current;
    
    
    #define SET_NODELOC(Current)  \
    node_lineno = Current;
    
    /* IMPORTANT NOTE ON LINE NUMBERS
    *********************************
    * The above definitions and macros cause every terminal in your grammar to 
    * have the line number supplied by the lexer. The only task you have to
    * implement for line numbers to work correctly, is to use SET_NODELOC()
    * before constructing any constructs from non-terminals in your grammar.
    * Example: Consider you are matching on the following very restrictive 
    * (fictional) construct that matches a plus between two integer constants. 
    * (SUCH A RULE SHOULD NOT BE  PART OF YOUR PARSER):
    
    plus_consts	: INT_CONST '+' INT_CONST 
    
    * where INT_CONST is a terminal for an integer constant. Now, a correct
    * action for this rule that attaches the correct line number to plus_const
    * would look like the following:
    
    plus_consts	: INT_CONST '+' INT_CONST 
    {
      // Set the line number of the current non-terminal:
      // ***********************************************
      // You can access the line numbers of the i'th item with @i, just
      // like you acess the value of the i'th exporession with $i.
      //
      // Here, we choose the line number of the last INT_CONST (@3) as the
      // line number of the resulting expression (@$). You are free to pick
      // any reasonable line as the line number of non-terminals. If you 
      // omit the statement @$=..., bison has default rules for deciding which 
      // line number to use. Check the manual for details if you are interested.
      @$ = @3;
      
      
      // Observe that we call SET_NODELOC(@3); this will set the global variable
      // node_lineno to @3. Since the constructor call "plus" uses the value of 
      // this global, the plus node will now have the correct line number.
      SET_NODELOC(@3);
      
      // construct the result node:
      $$ = plus(int_const($1), int_const($3));
    }
    
    */
    
    
    
    void yyerror(char *s);        /*  defined below; called for each parse error */
    extern int yylex();           /*  the entry point to the lexer  */
    
    /************************************************************************/
    /*                DONT CHANGE ANYTHING IN THIS SECTION                  */
    
    Program ast_root;	      /* the result of the parse  */
    Classes parse_results;        /* for use in semantic analysis */
    int omerrs = 0;               /* number of errors in lexing and parsing */
    %}
    
    /* A union of all the types that can be the result of parsing actions. */
    %union {
      Boolean boolean;
      Symbol symbol;
      Program program;
      Class_ class_;
      Classes classes;
      Feature feature;
      Features features;
      Formal formal;
      Formals formals;
      Case case_;
      Cases cases;
      Expression expression;
      Expressions expressions;
      char *error_msg;
    }
    
    /* 
    Declare the terminals; a few have types for associated lexemes.
    The token ERROR is never used in the parser; thus, it is a parse
    error when the lexer returns it.
    
    The integer following token declaration is the numeric constant used
    to represent that token internally.  Typically, Bison generates these
    on its own, but we give explicit numbers to prevent version parity
    problems (bison 1.25 and earlier start at 258, later versions -- at
    257)
    */
    %token CLASS 258 ELSE 259 FI 260 IF 261 IN 262 
    %token INHERITS 263 LET 264 LOOP 265 POOL 266 THEN 267 WHILE 268
    %token CASE 269 ESAC 270 OF 271 DARROW 272 NEW 273 ISVOID 274
    %token <symbol>  STR_CONST 275 INT_CONST 276 
    %token <boolean> BOOL_CONST 277
    %token <symbol>  TYPEID 278 OBJECTID 279 
    %token ASSIGN 280 NOT 281 LE 282 ERROR 283
    
    /*  DON'T CHANGE ANYTHING ABOVE THIS LINE, OR YOUR PARSER WONT WORK       */
    /**************************************************************************/
    
    /* Complete the nonterminal list below, giving a type for the semantic
    value of each non terminal. (See section 3.6 in the bison 
    documentation for details). */
    
    /* Declare types for the grammar's non-terminals. */
    %type <program> program
    %type <classes> class_list
    %type <class_> class
	/* add by ronaflx */
	%type <formals> formal_list
	%type <formal> formal
	%type <expressions> expr_list 
	%type <expressions> actual_list
	%type <expression> expr
	%type <expression> equality_expr
	%type <expression> relational_expr
	%type <expression> additive_expr
	%type <expression> multiplicative_expr
	%type <expression> unary_expr
	%type <expression> primary_expr
    %type <expression> while_expr
    %type <expression> if_expr
	%type <cases> case_list
	%type <case_> case_
	%type <expression> let_
    
    /* You will want to change the following line. */
    %type <features> dummy_feature_list
    %type <feature>  dummy_feature
    
    /* Precedence declarations go here. */
    %right ASSIGN
    %right NOT
	%left '<' '=' LE
	%left '+' '-'
	%left '*' '/'
    %right ISVOID
    %right '~'

    %%
    /* 
    Save the root of the abstract syntax tree in a global variable.
    */
    program
	: class_list
	{
		@$ = @1;
		ast_root = program($1);
	}
    ;
    
    class_list
    : class			/* single class */
    {
		$$ = single_Classes($1);
    	parse_results = $$;
	}
    | class_list class	/* several classes */
    {
		$$ = append_Classes($1, single_Classes($2)); 
    	parse_results = $$;
	}
    | class_list error
    {
    }
    ;
    
    /* If no parent is specified, the class inherits from the Object class. */
    class
	: CLASS TYPEID '{' dummy_feature_list '}' ';'
    {
		$$ = class_($2, idtable.add_string("Object"), $4, stringtable.add_string(curr_filename));
	}
    | CLASS TYPEID '{' error '}' ';' {}
    | CLASS TYPEID INHERITS TYPEID '{' dummy_feature_list '}' ';'
    {
		$$ = class_($2, $4, $6, stringtable.add_string(curr_filename));
	}
    | CLASS TYPEID INHERITS TYPEID '{' error '}' ';' {}
    | CLASS error {}
    ;
    
    /* Feature list may be empty, but no empty features in list. */
    dummy_feature_list
	:		/* empty */
    {
		$$ = nil_Features();
	}
	| dummy_feature
	{
		$$ = single_Features($1);
	}
	| dummy_feature_list dummy_feature
	{
		$$ = append_Features($1, single_Features($2));
	}
    | dummy_feature_list error {}
	;
    
    /* add by ronaflx */

	dummy_feature
	: OBJECTID ':' TYPEID ';'	/* variable */
	{
		$$ = attr($1, $3, no_expr());
	}
	| OBJECTID ':' TYPEID ASSIGN expr ';'	/* variable with assignment */
	{
		$$ = attr($1, $3, $5);
	}
    | OBJECTID ':' error
    {
    }
	| OBJECTID '(' formal_list ')' ':' TYPEID '{' expr '}' ';'	/* function */
	{
		$$ = method($1, $3, $6, $8);
	}
	| OBJECTID '(' formal_list ')' ':' TYPEID '{' error '}' ';'	/* function */ {}
	;

	/* 
	 * Formal list may be empty, but no empty formals in list.
	 * formals are splited by ','
	 */
	formal_list
	:		/* empty */
	{
		$$ = nil_Formals();
	}
	| formal
	{
		$$ = single_Formals($1);
	}
	| formal_list ',' formal
	{
		$$ = append_Formals($1, single_Formals($3));
	}
	;
	formal
	: OBJECTID ':' TYPEID 
	{
		$$ = formal($1, $3);
	}
	;

    /*
     * expressions list
     */
	expr_list	
	: expr ';'
	{
		$$ = single_Expressions($1);
	}
	| expr_list expr ';'
	{
		$$ = append_Expressions($1, single_Expressions($2));
	}
    | expr_list error ';' {}
	;

    /*
     * epxrs contains if_expr, while_expr, case_expr,
     * arithmetic expression and function
     */
	expr
	: OBJECTID ASSIGN expr	/* assigment expression */
	{
		$$ = assign($1, $3);
	}
	| CASE expr OF case_list ESAC
	{
		$$ = typcase($2, $4);
	}
    | if_expr
    {
		$$ = $1;
    }
    | while_expr
    {
		$$ = $1;
	}
    | NOT expr
    {
		$$ = comp($2);
    }
    | equality_expr
    {
		$$ = $1;
    }
    | equality_expr error {}
    ;

    /*
     * equality_expr
     * a = b = c is unallowed
     */
    equality_expr
    : relational_expr
	{
		$$ = $1;
	}
	| relational_expr '=' relational_expr
	{
		$$ = eq($1, $3);
	}
    ;
    relational_expr
    : additive_expr
	{
		$$ = $1;
	}
    | additive_expr LE additive_expr
    {
		$$ = leq($1, $3);
    }
    | additive_expr '<' additive_expr
    {
		$$ = lt($1, $3);
    }
    ;
    additive_expr
    : multiplicative_expr
    {
		$$ = $1;
    }
    | additive_expr '+' multiplicative_expr
    {
		$$ = plus($1, $3);
    }
    | additive_expr '-' multiplicative_expr
    {
		$$ = sub($1, $3);
    }
    ;
    multiplicative_expr
    : unary_expr
    {
		$$ = $1;
	}
	| multiplicative_expr '*' unary_expr
	{
		$$ = mul($1, $3);
	}
    | multiplicative_expr '/' unary_expr
    {
		$$ = divide($1, $3);
    }
    ;
    unary_expr
    : primary_expr
    {
		$$ = $1;
    }
    | NEW TYPEID
    {
		$$ = new_($2);
    }
    | ISVOID primary_expr 
    {
		$$ = isvoid($2);
    }
    | '~' primary_expr
    {
		$$ = neg($2);
    }
    ;
    primary_expr
    : OBJECTID
    {
		$$ = object($1);
    }
    | '(' expr ')'
    {
		$$ = $2;
    }
    | INT_CONST
    {
		$$ = int_const($1);
    }
    | STR_CONST
    {
		$$ = string_const($1);
    }
    | BOOL_CONST
    {
		$$ = bool_const($1);
    }
	| '{' expr_list '}'
	{
		$$ = block($2);
	}
	| primary_expr '@' TYPEID '.' OBJECTID '(' actual_list ')'	/* static member function */
	{
		$$ = static_dispatch($1, $3, $5, $7);
	}
	| primary_expr '.' OBJECTID '(' actual_list ')'	/* call function */
	{
		$$ = dispatch($1, $3, $5);
	}
	| OBJECTID '(' actual_list ')'	/* call function */
	{
		$$ = dispatch(self(), $1, $3);
	}
    | '{' error '}' {}
	| LET let_
	{
		$$ = $2;
	}
    ;

	/* 
	 * Acutal Formal list may be empty, but no empty formals in list.
	 * formals are splited by ','
	 */
	actual_list
	:	/* empty */
	{
		$$ = nil_Expressions();
	}
	| expr
	{
		$$ = single_Expressions($1);
	}
	| actual_list ',' expr
	{
		$$ = append_Expressions($1, single_Expressions($3));
	}

	/* 
	 * if_expr
	 * must have else statement
	 */
    if_expr
	: IF expr THEN expr ELSE expr FI	/* if expression */
	{
		$$ = cond($2, $4, $6);
	}
    ;

	/* 
	 * while_expr
	 */
    while_expr
    : WHILE expr LOOP expr POOL 		/* while expression */
	{
		$$ = loop($2, $4);
	}
    ;

	/* 
	 * case list 
	 */
	case_list
	: case_
	{
		$$ = single_Cases($1);
	}
	| case_list case_
	{
		$$ = append_Cases($1, single_Cases($2));
	}
    | case_list error {}
	;

	/* 
	 * case expr
	 */
	case_
	: OBJECTID ':' TYPEID DARROW expr ';'
	{
		$$ = branch($1, $3, $5);
	}
	;

	/* 
	 * let expr
	 */
	let_
	: OBJECTID ':' TYPEID IN expr
	{
		$$ = let($1, $3, no_expr(), $5);
	}
	| OBJECTID ':' TYPEID ASSIGN expr IN expr
	{
		$$ = let($1, $3, $5, $7);
	}
	| OBJECTID ':' TYPEID ',' let_
	{
		$$ = let($1, $3, no_expr(), $5);
	}
	| OBJECTID ':' TYPEID ASSIGN expr ',' let_
	{
		$$ = let($1, $3, $5, $7);
	}
    | error let_ {}
	;

    /* end of grammar */
    %%
    
    /* This function is called automatically when Bison detects a parse error. */
    void yyerror(char *s)
	{
		extern int curr_lineno;
		/*fprintf(stderr, "\"%s\", line %d: %s at or near ", curr_filename, curr_lineno, s);*/
		cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " \
				<< s << " at or near ";
		print_cool_token(yychar);
		fprintf(stderr, "\n");
		omerrs++;

		if(omerrs>50) {fprintf(stdout, "More than 50 errors\n"); exit(1);}
	}
