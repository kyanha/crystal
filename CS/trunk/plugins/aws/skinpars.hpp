/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_NUM = 258,
     TOKEN_STR = 259,
     TOKEN_ATTR = 260,
     TOKEN_SKIN = 261,
     TOKEN_FOR = 262,
     TOKEN_WINDOW = 263,
     TOKEN_FROM = 264,
     TOKEN_COMPONENT = 265,
     TOKEN_CONNECT = 266,
     TOKEN_IS = 267,
     NEG = 268
   };
#endif
#define TOKEN_NUM 258
#define TOKEN_STR 259
#define TOKEN_ATTR 260
#define TOKEN_SKIN 261
#define TOKEN_FOR 262
#define TOKEN_WINDOW 263
#define TOKEN_FROM 264
#define TOKEN_COMPONENT 265
#define TOKEN_CONNECT 266
#define TOKEN_IS 267
#define NEG 268




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 31 "plugins/aws/skinpars.yy"
typedef union YYSTYPE {
  char   *str;			/* For returning titles and handles to items. */
  int     val;			/* For returning numbers                      */
  csRect *rect;			/* For returning rectangular regions          */
  awsKey *key;     		/* For returning keys to various definition items */
  awsComponentNode *comp;	/* for returning windows		      */
  awsKeyContainer *keycont;	/* for returning KeyContainers		      */
  awsSkinNode *skin;		/* for returning skins			      */
} YYSTYPE;
/* Line 1248 of yacc.c.  */
#line 72 "plugins/aws/skinpars.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





