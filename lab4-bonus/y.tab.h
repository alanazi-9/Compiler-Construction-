/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PLUS = 258,
     MINUS = 259,
     DIV = 260,
     MOD = 261,
     MULT = 262,
     EQ = 263,
     LT = 264,
     GT = 265,
     GE = 266,
     LE = 267,
     NOT = 268,
     OR = 269,
     AND = 270,
     IF = 271,
     LET = 272,
     LPAR = 273,
     RPAR = 274,
     CONST = 275,
     GETINT = 276,
     GETBOOL = 277,
     DEFFUN = 278,
     TRUE = 279,
     FALSE = 280,
     ID = 281,
     INT = 282,
     BOOL = 283,
     ERR = 284,
     PRINT = 285,
     EVAL = 286,
     CALL = 287,
     FUNID = 288,
     BOOLID = 289,
     INTID = 290,
     REGID = 291,
     LETID = 292,
     VARID = 293,
     INP = 294,
     COST = 295
   };
#endif
/* Tokens.  */
#define PLUS 258
#define MINUS 259
#define DIV 260
#define MOD 261
#define MULT 262
#define EQ 263
#define LT 264
#define GT 265
#define GE 266
#define LE 267
#define NOT 268
#define OR 269
#define AND 270
#define IF 271
#define LET 272
#define LPAR 273
#define RPAR 274
#define CONST 275
#define GETINT 276
#define GETBOOL 277
#define DEFFUN 278
#define TRUE 279
#define FALSE 280
#define ID 281
#define INT 282
#define BOOL 283
#define ERR 284
#define PRINT 285
#define EVAL 286
#define CALL 287
#define FUNID 288
#define BOOLID 289
#define INTID 290
#define REGID 291
#define LETID 292
#define VARID 293
#define INP 294
#define COST 295




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 15 "lab_yacc.y"
{int val; char* str;}
/* Line 1529 of yacc.c.  */
#line 131 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

