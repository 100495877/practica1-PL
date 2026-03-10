//  119 , Diego , Iñigo
//  100522343@alumnos.uc3m.es ,  100495877@alumnos.uc3m.es

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define T_NUMBER 	1001
#define T_OPERATOR	1002		
#define T_VARIABLE  	1003  

void ParseYourGrammar () ; 		/// Dummy Parser
void ParseAxiom () ;			/// Prototype for forward reference 		
void ParseE();
void ParseR();
int ParseO();


struct s_tokens {
	int token ;					// Here we store the current token/literal 
	int old_token ; 			// Sometimes we need to check the previous token
	int number ;				// The value of the number 
	int old_number ;			// old number value
	char variable_name [8] ;		/// variable name	
	char old_var_name [8] ;			/// old variable name			
	int token_val ;				// the arithmetic operator
	int old_token_val ;			// old arithmetic operator
} ;

struct s_tokens tokens = {0, 0, 0, -1, "", "", 0, -1}; // contains initial values


int line_counter = 1 ;


void update_old_token () 
{					/// Sometimes we need to check the previous token
	tokens.old_token = tokens.token ;
	tokens.old_number = tokens.number ;
	strcpy (tokens.old_var_name, tokens.variable_name) ;	/// Copy variable names			
	tokens.old_token_val = tokens.token_val ;
}


void init_tokens () 
{ 								///  Not really neccesary
    tokens.token = 0;
    tokens.old_token = 0 ;
    tokens.number = 0 ;
    tokens.old_number = -1 ;
    strcpy (tokens.old_var_name, "") ;			/// erase old variable name
    strcpy (tokens.variable_name, "") ;			/// Erase variable name
    tokens.token_val = 0;
    tokens.old_token_val = -1;
}


int rd_lex ()
{

/// DO NOT MODIFY THE CODE INSIDE THE YYLEX FUNCTION WITHOUT PERMISSION !!!

    int c ;
    int cc ;

    do {
        c = getchar () ;
        if (c == '\n') 
            line_counter++ ;	// info for rd_syntax_error()
    } while (c == '\t' || c == ' ' || c == '\r') ;	/// \r is part of a newline in some Operating Systems

    if (isdigit (c)) {			/// Token Number is [Digit]+
        ungetc (c, stdin) ;		/// This returns one character to the standard input stream    
        update_old_token () ;
        scanf ("%d", &tokens.number) ;
        tokens.token = T_NUMBER ;
        return (tokens.token) ;	// returns the Token for Variable
    }

    if (isalpha(c)) {  /// Token Variable of type Letter[Digit|Letter]? 
        update_old_token () ;
        cc = getchar () ;
        if (isdigit (cc) || isalpha (cc)) {									
            sprintf (tokens.variable_name, "%c%c", c, cc) ;		/// This copies the Letter.Digit|Letter name in the variable name    
        } else {											
            ungetc (cc, stdin) ;									
            sprintf (tokens.variable_name, "%c", c) ;		/// This copies the single Letter name in the variable name
        }													
        tokens.token = T_VARIABLE ;
        return (tokens.token) ;	// returns the Token for Variable
    } 
    
    if (c == '+' || c == '-' || c == '*' || c == '/') {  /// Remember that OTHER SYNBOLS ARE returned as literals
        update_old_token () ;
        tokens.token_val = c ;
        tokens.token = T_OPERATOR ;
        return (tokens.token) ;		// returns the Token for Arithmetic Operators
    }					

    if (c == EOF) {         /// End Of Archive detection for enhanced Batch Processing
        exit (0) ;
    }  
    
    update_old_token () ;
    tokens.token = c ;
    return (tokens.token) ;		// returns a literal
}


void rd_syntax_error (int expected, int token, char *output) 
{
	fprintf (stderr, "ERROR in line %d ", line_counter) ;
	fprintf (stderr, output, expected, token) ;
	
	exit (0) ;
}


void MatchSymbol (int expected_token)
{
	if (tokens.token != expected_token) {
		rd_syntax_error (expected_token, tokens.token, "token %d expected, but %d was read") ;
		exit (0) ;
	} else {
	 	rd_lex () ; 			/// read next Token
	}
}


// #define ParseLParen() 	MatchSymbol ('(') ; // More concise and efficient definitions
// #define ParseRParen() 	MatchSymbol (')') ; ///   rather than using functions
											/// The actual recomendation is to use MatchSymbol in the code rather than theese macros

int ParseO()
{
    int op_leido;

    if (tokens.token == T_OPERATOR) {
        op_leido = tokens.token_val;
        MatchSymbol(T_OPERATOR);
        return op_leido;
    }

    rd_syntax_error(T_OPERATOR, tokens.token,
                    "Error: se esperaba operador (+,-,*,/), pero se leyo %d\n");
    return 0;
}

/*
   E -> (R | d | v
*/
void ParseE()
{
    if (tokens.token == T_NUMBER) {
        printf("%d", tokens.number);
        MatchSymbol(T_NUMBER);
    }
    else if (tokens.token == T_VARIABLE) {
        printf("%s", tokens.variable_name);
        MatchSymbol(T_VARIABLE);
    }
    else if (tokens.token == '(') {
        MatchSymbol('(');
        ParseR();
    }
    else {
        rd_syntax_error(-1, tokens.token,
                        "Error sintactico en E: token inesperado %d\n");
    }
}

/*
   R -> O E E ) | = v E ) | ? E E E )
*/
void ParseR()
{
    if (tokens.token == T_OPERATOR) {
        /* R -> O E E ) */
        int op = ParseO();

        printf("(");
        ParseE();
        printf(" %c ", op);
        ParseE();
        printf(")");

        MatchSymbol(')');
    }
    else if (tokens.token == '=') {
        /* R -> = v E ) */
        MatchSymbol('=');

        if (tokens.token != T_VARIABLE) {
            rd_syntax_error(T_VARIABLE, tokens.token,
                            "Error sintactico: se esperaba variable tras '='\n");
        }

        printf("(");
        printf("%s", tokens.variable_name);
        MatchSymbol(T_VARIABLE);
        printf(" = ");
        ParseE();
        printf(")");

        MatchSymbol(')');
    }
    else if (tokens.token == '?') {
        /* R -> ? E E E ) */
        MatchSymbol('?');

        printf("(");
        ParseE();
        printf(" ? ");
        ParseE();
        printf(" : ");
        ParseE();
        printf(")");

        MatchSymbol(')');
    }
    else {
        rd_syntax_error(-1, tokens.token,
                        "Error sintactico en R: token inesperado %d\n");
    }
}

void ParseYourGrammar()
{
    ParseE();
}

void ParseAxiom()
{
    ParseYourGrammar();

    if (tokens.token == '\n') {
        printf("\n");
        MatchSymbol('\n');
    } else {
        rd_syntax_error('\n', tokens.token,
                        "-- Unexpected Token (Expected:%d, Read:%d) at end of line\n");
    }
}

int main (int argc, char **argv) 
{
// Usage :  drLL     ==> evalute multiple Input Lines until some error appears   NORMAL USAGE
//--        drLL -s  ==> evaluate a single Input Line                            USE ONLY IN CASE your program FREEZES
//          
/// DO NOT MODIFY THE CODE INSIDE THE MAIN FUNCTION WITHOUT PERMISSION !!!

	int flagMultiple = 1 ;
	
	if (argc >= 2) {
		if (strcmp ("-s", argv [1]) == 0) {
			flagMultiple = 0 ;
		}
	}
	
	rd_lex () ;						/// Read first Token only once
	do {
		ParseAxiom () ;		
//		printf ("\n") ;
	} while (flagMultiple) ;
	
	exit (0) ;
}

