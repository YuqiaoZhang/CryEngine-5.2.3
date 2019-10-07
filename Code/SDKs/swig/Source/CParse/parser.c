/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 22 "parser.y"

#define yylex yylex

#include "swig.h"
#include "cparse.h"
#include "preprocessor.h"
#include <ctype.h>

/* We do this for portability */
#undef alloca
#define alloca malloc

/* -----------------------------------------------------------------------------
 *                               Externals
 * ----------------------------------------------------------------------------- */

int  yyparse();

/* NEW Variables */

static Node    *top = 0;      /* Top of the generated parse tree */
static int      unnamed = 0;  /* Unnamed datatype counter */
static Hash    *classes = 0;        /* Hash table of classes */
static Hash    *classes_typedefs = 0; /* Hash table of typedef classes: typedef struct X {...} Y; */
static Symtab  *prev_symtab = 0;
static Node    *current_class = 0;
String  *ModuleName = 0;
static Node    *module_node = 0;
static String  *Classprefix = 0;  
static String  *Namespaceprefix = 0;
static int      inclass = 0;
static Node    *currentOuterClass = 0; /* for nested classes */
static char    *last_cpptype = 0;
static int      inherit_list = 0;
static Parm    *template_parameters = 0;
static int      extendmode   = 0;
static int      compact_default_args = 0;
static int      template_reduce = 0;
static int      cparse_externc = 0;
int		ignore_nested_classes = 0;
int		kwargs_supported = 0;
/* -----------------------------------------------------------------------------
 *                            Assist Functions
 * ----------------------------------------------------------------------------- */


 
/* Called by the parser (yyparse) when an error is found.*/
static void yyerror (const char *e) {
  (void)e;
}

/* Copies a node.  Does not copy tree links or symbol table data (except for
   sym:name) */

static Node *copy_node(Node *n) {
  Node *nn;
  Iterator k;
  nn = NewHash();
  Setfile(nn,Getfile(n));
  Setline(nn,Getline(n));
  for (k = First(n); k.key; k = Next(k)) {
    String *ci;
    String *key = k.key;
    char *ckey = Char(key);
    if ((strcmp(ckey,"nextSibling") == 0) ||
	(strcmp(ckey,"previousSibling") == 0) ||
	(strcmp(ckey,"parentNode") == 0) ||
	(strcmp(ckey,"lastChild") == 0)) {
      continue;
    }
    if (Strncmp(key,"csym:",5) == 0) continue;
    /* We do copy sym:name.  For templates */
    if ((strcmp(ckey,"sym:name") == 0) || 
	(strcmp(ckey,"sym:weak") == 0) ||
	(strcmp(ckey,"sym:typename") == 0)) {
      String *ci = Copy(k.item);
      Setattr(nn,key, ci);
      Delete(ci);
      continue;
    }
    if (strcmp(ckey,"sym:symtab") == 0) {
      Setattr(nn,"sym:needs_symtab", "1");
    }
    /* We don't copy any other symbol table attributes */
    if (strncmp(ckey,"sym:",4) == 0) {
      continue;
    }
    /* If children.  We copy them recursively using this function */
    if (strcmp(ckey,"firstChild") == 0) {
      /* Copy children */
      Node *cn = k.item;
      while (cn) {
	Node *copy = copy_node(cn);
	appendChild(nn,copy);
	Delete(copy);
	cn = nextSibling(cn);
      }
      continue;
    }
    /* We don't copy the symbol table.  But we drop an attribute 
       requires_symtab so that functions know it needs to be built */

    if (strcmp(ckey,"symtab") == 0) {
      /* Node defined a symbol table. */
      Setattr(nn,"requires_symtab","1");
      continue;
    }
    /* Can't copy nodes */
    if (strcmp(ckey,"node") == 0) {
      continue;
    }
    if ((strcmp(ckey,"parms") == 0) || (strcmp(ckey,"pattern") == 0) || (strcmp(ckey,"throws") == 0)
	|| (strcmp(ckey,"kwargs") == 0)) {
      ParmList *pl = CopyParmList(k.item);
      Setattr(nn,key,pl);
      Delete(pl);
      continue;
    }
    if (strcmp(ckey,"nested:outer") == 0) { /* don't copy outer classes links, they will be updated later */
      Setattr(nn, key, k.item);
      continue;
    }
    /* Looks okay.  Just copy the data using Copy */
    ci = Copy(k.item);
    Setattr(nn, key, ci);
    Delete(ci);
  }
  return nn;
}

/* -----------------------------------------------------------------------------
 *                              Variables
 * ----------------------------------------------------------------------------- */

static char  *typemap_lang = 0;    /* Current language setting */

static int cplus_mode  = 0;

/* C++ modes */

#define  CPLUS_PUBLIC    1
#define  CPLUS_PRIVATE   2
#define  CPLUS_PROTECTED 3

/* include types */
static int   import_mode = 0;

void SWIG_typemap_lang(const char *tm_lang) {
  typemap_lang = Swig_copy_string(tm_lang);
}

void SWIG_cparse_set_compact_default_args(int defargs) {
  compact_default_args = defargs;
}

int SWIG_cparse_template_reduce(int treduce) {
  template_reduce = treduce;
  return treduce;  
}

/* -----------------------------------------------------------------------------
 *                           Assist functions
 * ----------------------------------------------------------------------------- */

static int promote_type(int t) {
  if (t <= T_UCHAR || t == T_CHAR) return T_INT;
  return t;
}

/* Perform type-promotion for binary operators */
static int promote(int t1, int t2) {
  t1 = promote_type(t1);
  t2 = promote_type(t2);
  return t1 > t2 ? t1 : t2;
}

static String *yyrename = 0;

/* Forward renaming operator */

static String *resolve_create_node_scope(String *cname);


Hash *Swig_cparse_features(void) {
  static Hash   *features_hash = 0;
  if (!features_hash) features_hash = NewHash();
  return features_hash;
}

/* Fully qualify any template parameters */
static String *feature_identifier_fix(String *s) {
  String *tp = SwigType_istemplate_templateprefix(s);
  if (tp) {
    String *ts, *ta, *tq;
    ts = SwigType_templatesuffix(s);
    ta = SwigType_templateargs(s);
    tq = Swig_symbol_type_qualify(ta,0);
    Append(tp,tq);
    Append(tp,ts);
    Delete(ts);
    Delete(ta);
    Delete(tq);
    return tp;
  } else {
    return NewString(s);
  }
}

static void set_access_mode(Node *n) {
  if (cplus_mode == CPLUS_PUBLIC)
    Setattr(n, "access", "public");
  else if (cplus_mode == CPLUS_PROTECTED)
    Setattr(n, "access", "protected");
  else
    Setattr(n, "access", "private");
}

static void restore_access_mode(Node *n) {
  String *mode = Getattr(n, "access");
  if (Strcmp(mode, "private") == 0)
    cplus_mode = CPLUS_PRIVATE;
  else if (Strcmp(mode, "protected") == 0)
    cplus_mode = CPLUS_PROTECTED;
  else
    cplus_mode = CPLUS_PUBLIC;
}

/* Generate the symbol table name for an object */
/* This is a bit of a mess. Need to clean up */
static String *add_oldname = 0;



static String *make_name(Node *n, String *name,SwigType *decl) {
  int destructor = name && (*(Char(name)) == '~');

  if (yyrename) {
    String *s = NewString(yyrename);
    Delete(yyrename);
    yyrename = 0;
    if (destructor  && (*(Char(s)) != '~')) {
      Insert(s,0,"~");
    }
    return s;
  }

  if (!name) return 0;
  return Swig_name_make(n,Namespaceprefix,name,decl,add_oldname);
}

/* Generate an unnamed identifier */
static String *make_unnamed() {
  unnamed++;
  return NewStringf("$unnamed%d$",unnamed);
}

/* Return if the node is a friend declaration */
static int is_friend(Node *n) {
  return Cmp(Getattr(n,"storage"),"friend") == 0;
}

static int is_operator(String *name) {
  return Strncmp(name,"operator ", 9) == 0;
}


/* Add declaration list to symbol table */
static int  add_only_one = 0;

static void add_symbols(Node *n) {
  String *decl;
  String *wrn = 0;

  if (inclass && n) {
    cparse_normalize_void(n);
  }
  while (n) {
    String *symname = 0;
    /* for friends, we need to pop the scope once */
    String *old_prefix = 0;
    Symtab *old_scope = 0;
    int isfriend = inclass && is_friend(n);
    int iscdecl = Cmp(nodeType(n),"cdecl") == 0;
    int only_csymbol = 0;
    
    if (inclass) {
      String *name = Getattr(n, "name");
      if (isfriend) {
	/* for friends, we need to add the scopename if needed */
	String *prefix = name ? Swig_scopename_prefix(name) : 0;
	old_prefix = Namespaceprefix;
	old_scope = Swig_symbol_popscope();
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	if (!prefix) {
	  if (name && !is_operator(name) && Namespaceprefix) {
	    String *nname = NewStringf("%s::%s", Namespaceprefix, name);
	    Setattr(n,"name",nname);
	    Delete(nname);
	  }
	} else {
	  Symtab *st = Swig_symbol_getscope(prefix);
	  String *ns = st ? Getattr(st,"name") : prefix;
	  String *base  = Swig_scopename_last(name);
	  String *nname = NewStringf("%s::%s", ns, base);
	  Setattr(n,"name",nname);
	  Delete(nname);
	  Delete(base);
	  Delete(prefix);
	}
	Namespaceprefix = 0;
      } else {
	/* for member functions, we need to remove the redundant
	   class scope if provided, as in
	   
	   struct Foo {
	   int Foo::method(int a);
	   };
	   
	*/
	String *prefix = name ? Swig_scopename_prefix(name) : 0;
	if (prefix) {
	  if (Classprefix && (Equal(prefix,Classprefix))) {
	    String *base = Swig_scopename_last(name);
	    Setattr(n,"name",base);
	    Delete(base);
	  }
	  Delete(prefix);
	}
      }
    }

    if (!isfriend && (inclass || extendmode)) {
      Setattr(n,"ismember","1");
    }

    if (extendmode) {
      Setattr(n,"isextendmember","1");
    }

    if (!isfriend && inclass) {
      if ((cplus_mode != CPLUS_PUBLIC)) {
	only_csymbol = 1;
	if (cplus_mode == CPLUS_PROTECTED) {
	  Setattr(n,"access", "protected");
	  only_csymbol = !Swig_need_protected(n);
	} else {
	  Setattr(n,"access", "private");
	  /* private are needed only when they are pure virtuals - why? */
	  if ((Cmp(Getattr(n,"storage"),"virtual") == 0) && (Cmp(Getattr(n,"value"),"0") == 0)) {
	    only_csymbol = 0;
	  }
	  if (Cmp(nodeType(n),"destructor") == 0) {
	    /* Needed for "unref" feature */
	    only_csymbol = 0;
	  }
	}
      } else {
	  Setattr(n,"access", "public");
      }
    }
    if (Getattr(n,"sym:name")) {
      n = nextSibling(n);
      continue;
    }
    decl = Getattr(n,"decl");
    if (!SwigType_isfunction(decl)) {
      String *name = Getattr(n,"name");
      String *makename = Getattr(n,"parser:makename");
      if (iscdecl) {	
	String *storage = Getattr(n, "storage");
	if (Cmp(storage,"typedef") == 0) {
	  Setattr(n,"kind","typedef");
	} else {
	  SwigType *type = Getattr(n,"type");
	  String *value = Getattr(n,"value");
	  Setattr(n,"kind","variable");
	  if (value && Len(value)) {
	    Setattr(n,"hasvalue","1");
	  }
	  if (type) {
	    SwigType *ty;
	    SwigType *tmp = 0;
	    if (decl) {
	      ty = tmp = Copy(type);
	      SwigType_push(ty,decl);
	    } else {
	      ty = type;
	    }
	    if (!SwigType_ismutable(ty) || (storage && Strstr(storage, "constexpr"))) {
	      SetFlag(n,"hasconsttype");
	      SetFlag(n,"feature:immutable");
	    }
	    if (tmp) Delete(tmp);
	  }
	  if (!type) {
	    Printf(stderr,"notype name %s\n", name);
	  }
	}
      }
      Swig_features_get(Swig_cparse_features(), Namespaceprefix, name, 0, n);
      if (makename) {
	symname = make_name(n, makename,0);
        Delattr(n,"parser:makename"); /* temporary information, don't leave it hanging around */
      } else {
        makename = name;
	symname = make_name(n, makename,0);
      }
      
      if (!symname) {
	symname = Copy(Getattr(n,"unnamed"));
      }
      if (symname) {
	wrn = Swig_name_warning(n, Namespaceprefix, symname,0);
      }
    } else {
      String *name = Getattr(n,"name");
      SwigType *fdecl = Copy(decl);
      SwigType *fun = SwigType_pop_function(fdecl);
      if (iscdecl) {	
	Setattr(n,"kind","function");
      }
      
      Swig_features_get(Swig_cparse_features(),Namespaceprefix,name,fun,n);

      symname = make_name(n, name,fun);
      wrn = Swig_name_warning(n, Namespaceprefix,symname,fun);
      
      Delete(fdecl);
      Delete(fun);
      
    }
    if (!symname) {
      n = nextSibling(n);
      continue;
    }
    if (cparse_cplusplus) {
      String *value = Getattr(n, "value");
      if (value && Strcmp(value, "delete") == 0) {
	/* C++11 deleted definition / deleted function */
        SetFlag(n,"deleted");
        SetFlag(n,"feature:ignore");
      }
    }
    if (only_csymbol || GetFlag(n,"feature:ignore")) {
      /* Only add to C symbol table and continue */
      Swig_symbol_add(0, n);
    } else if (strncmp(Char(symname),"$ignore",7) == 0) {
      char *c = Char(symname)+7;
      SetFlag(n,"feature:ignore");
      if (strlen(c)) {
	SWIG_WARN_NODE_BEGIN(n);
	Swig_warning(0,Getfile(n), Getline(n), "%s\n",c+1);
	SWIG_WARN_NODE_END(n);
      }
      Swig_symbol_add(0, n);
    } else {
      Node *c;
      if ((wrn) && (Len(wrn))) {
	String *metaname = symname;
	if (!Getmeta(metaname,"already_warned")) {
	  SWIG_WARN_NODE_BEGIN(n);
	  Swig_warning(0,Getfile(n),Getline(n), "%s\n", wrn);
	  SWIG_WARN_NODE_END(n);
	  Setmeta(metaname,"already_warned","1");
	}
      }
      c = Swig_symbol_add(symname,n);

      if (c != n) {
        /* symbol conflict attempting to add in the new symbol */
        if (Getattr(n,"sym:weak")) {
          Setattr(n,"sym:name",symname);
        } else {
          String *e = NewStringEmpty();
          String *en = NewStringEmpty();
          String *ec = NewStringEmpty();
          int redefined = Swig_need_redefined_warn(n,c,inclass);
          if (redefined) {
            Printf(en,"Identifier '%s' redefined (ignored)",symname);
            Printf(ec,"previous definition of '%s'",symname);
          } else {
            Printf(en,"Redundant redeclaration of '%s'",symname);
            Printf(ec,"previous declaration of '%s'",symname);
          }
          if (Cmp(symname,Getattr(n,"name"))) {
            Printf(en," (Renamed from '%s')", SwigType_namestr(Getattr(n,"name")));
          }
          Printf(en,",");
          if (Cmp(symname,Getattr(c,"name"))) {
            Printf(ec," (Renamed from '%s')", SwigType_namestr(Getattr(c,"name")));
          }
          Printf(ec,".");
	  SWIG_WARN_NODE_BEGIN(n);
          if (redefined) {
            Swig_warning(WARN_PARSE_REDEFINED,Getfile(n),Getline(n),"%s\n",en);
            Swig_warning(WARN_PARSE_REDEFINED,Getfile(c),Getline(c),"%s\n",ec);
          } else if (!is_friend(n) && !is_friend(c)) {
            Swig_warning(WARN_PARSE_REDUNDANT,Getfile(n),Getline(n),"%s\n",en);
            Swig_warning(WARN_PARSE_REDUNDANT,Getfile(c),Getline(c),"%s\n",ec);
          }
	  SWIG_WARN_NODE_END(n);
          Printf(e,"%s:%d:%s\n%s:%d:%s\n",Getfile(n),Getline(n),en,
                 Getfile(c),Getline(c),ec);
          Setattr(n,"error",e);
	  Delete(e);
          Delete(en);
          Delete(ec);
        }
      }
    }
    /* restore the class scope if needed */
    if (isfriend) {
      Swig_symbol_setscope(old_scope);
      if (old_prefix) {
	Delete(Namespaceprefix);
	Namespaceprefix = old_prefix;
      }
    }
    Delete(symname);

    if (add_only_one) return;
    n = nextSibling(n);
  }
}


/* add symbols a parse tree node copy */

static void add_symbols_copy(Node *n) {
  String *name;
  int    emode = 0;
  while (n) {
    char *cnodeType = Char(nodeType(n));

    if (strcmp(cnodeType,"access") == 0) {
      String *kind = Getattr(n,"kind");
      if (Strcmp(kind,"public") == 0) {
	cplus_mode = CPLUS_PUBLIC;
      } else if (Strcmp(kind,"private") == 0) {
	cplus_mode = CPLUS_PRIVATE;
      } else if (Strcmp(kind,"protected") == 0) {
	cplus_mode = CPLUS_PROTECTED;
      }
      n = nextSibling(n);
      continue;
    }

    add_oldname = Getattr(n,"sym:name");
    if ((add_oldname) || (Getattr(n,"sym:needs_symtab"))) {
      int old_inclass = -1;
      Node *old_current_class = 0;
      if (add_oldname) {
	DohIncref(add_oldname);
	/*  Disable this, it prevents %rename to work with templates */
	/* If already renamed, we used that name  */
	/*
	if (Strcmp(add_oldname, Getattr(n,"name")) != 0) {
	  Delete(yyrename);
	  yyrename = Copy(add_oldname);
	}
	*/
      }
      Delattr(n,"sym:needs_symtab");
      Delattr(n,"sym:name");

      add_only_one = 1;
      add_symbols(n);

      if (Getattr(n,"partialargs")) {
	Swig_symbol_cadd(Getattr(n,"partialargs"),n);
      }
      add_only_one = 0;
      name = Getattr(n,"name");
      if (Getattr(n,"requires_symtab")) {
	Swig_symbol_newscope();
	Swig_symbol_setscopename(name);
	Delete(Namespaceprefix);
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (strcmp(cnodeType,"class") == 0) {
	old_inclass = inclass;
	inclass = 1;
	old_current_class = current_class;
	current_class = n;
	if (Strcmp(Getattr(n,"kind"),"class") == 0) {
	  cplus_mode = CPLUS_PRIVATE;
	} else {
	  cplus_mode = CPLUS_PUBLIC;
	}
      }
      if (strcmp(cnodeType,"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (strcmp(cnodeType,"extend") == 0) {
	cplus_mode = emode;
      }
      if (Getattr(n,"requires_symtab")) {
	Setattr(n,"symtab", Swig_symbol_popscope());
	Delattr(n,"requires_symtab");
	Delete(Namespaceprefix);
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (add_oldname) {
	Delete(add_oldname);
	add_oldname = 0;
      }
      if (strcmp(cnodeType,"class") == 0) {
	inclass = old_inclass;
	current_class = old_current_class;
      }
    } else {
      if (strcmp(cnodeType,"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (strcmp(cnodeType,"extend") == 0) {
	cplus_mode = emode;
      }
    }
    n = nextSibling(n);
  }
}

/* Check a set of declarations to see if any are pure-abstract */

static List *pure_abstracts(Node *n) {
  List *abstracts = 0;
  while (n) {
    if (Cmp(nodeType(n),"cdecl") == 0) {
      String *decl = Getattr(n,"decl");
      if (SwigType_isfunction(decl)) {
	String *init = Getattr(n,"value");
	if (Cmp(init,"0") == 0) {
	  if (!abstracts) {
	    abstracts = NewList();
	  }
	  Append(abstracts,n);
	  SetFlag(n,"abstract");
	}
      }
    } else if (Cmp(nodeType(n),"destructor") == 0) {
      if (Cmp(Getattr(n,"value"),"0") == 0) {
	if (!abstracts) {
	  abstracts = NewList();
	}
	Append(abstracts,n);
	SetFlag(n,"abstract");
      }
    }
    n = nextSibling(n);
  }
  return abstracts;
}

/* Make a classname */

static String *make_class_name(String *name) {
  String *nname = 0;
  String *prefix;
  if (Namespaceprefix) {
    nname= NewStringf("%s::%s", Namespaceprefix, name);
  } else {
    nname = NewString(name);
  }
  prefix = SwigType_istemplate_templateprefix(nname);
  if (prefix) {
    String *args, *qargs;
    args   = SwigType_templateargs(nname);
    qargs  = Swig_symbol_type_qualify(args,0);
    Append(prefix,qargs);
    Delete(nname);
    Delete(args);
    Delete(qargs);
    nname = prefix;
  }
  return nname;
}

/* Use typedef name as class name */

static void add_typedef_name(Node *n, Node *decl, String *oldName, Symtab *cscope, String *scpname) {
  String *class_rename = 0;
  SwigType *decltype = Getattr(decl, "decl");
  if (!decltype || !Len(decltype)) {
    String *cname;
    String *tdscopename;
    String *class_scope = Swig_symbol_qualifiedscopename(cscope);
    String *name = Getattr(decl, "name");
    cname = Copy(name);
    Setattr(n, "tdname", cname);
    tdscopename = class_scope ? NewStringf("%s::%s", class_scope, name) : Copy(name);
    class_rename = Getattr(n, "class_rename");
    if (class_rename && (Strcmp(class_rename, oldName) == 0))
      Setattr(n, "class_rename", NewString(name));
    if (!classes_typedefs) classes_typedefs = NewHash();
    if (!Equal(scpname, tdscopename) && !Getattr(classes_typedefs, tdscopename)) {
      Setattr(classes_typedefs, tdscopename, n);
    }
    Setattr(n, "decl", decltype);
    Delete(class_scope);
    Delete(cname);
    Delete(tdscopename);
  }
}

/* If the class name is qualified.  We need to create or lookup namespace entries */

static Symtab *set_scope_to_global() {
  Symtab *symtab = Swig_symbol_global_scope();
  Swig_symbol_setscope(symtab);
  return symtab;
}
 
/* Remove the block braces, { and }, if the 'noblock' attribute is set.
 * Node *kw can be either a Hash or Parmlist. */
static String *remove_block(Node *kw, const String *inputcode) {
  String *modified_code = 0;
  while (kw) {
   String *name = Getattr(kw,"name");
   if (name && (Cmp(name,"noblock") == 0)) {
     char *cstr = Char(inputcode);
     size_t len = Len(inputcode);
     if (len && cstr[0] == '{') {
       --len; ++cstr; 
       if (len && cstr[len - 1] == '}') { --len; }
       /* we now remove the extra spaces */
       while (len && isspace((int)cstr[0])) { --len; ++cstr; }
       while (len && isspace((int)cstr[len - 1])) { --len; }
       modified_code = NewStringWithSize(cstr, len);
       break;
     }
   }
   kw = nextSibling(kw);
  }
  return modified_code;
}


static Node *nscope = 0;
static Node *nscope_inner = 0;

/* Remove the scope prefix from cname and return the base name without the prefix.
 * The scopes required for the symbol name are resolved and/or created, if required.
 * For example AA::BB::CC as input returns CC and creates the namespace AA then inner 
 * namespace BB in the current scope. If cname is found to already exist as a weak symbol
 * (forward reference) then the scope might be changed to match, such as when a symbol match 
 * is made via a using reference. */
static String *resolve_create_node_scope(String *cname) {
  Symtab *gscope = 0;
  Node *cname_node = 0;
  int skip_lookup = 0;
  nscope = 0;
  nscope_inner = 0;  

  if (Strncmp(cname,"::",2) == 0)
    skip_lookup = 1;

  cname_node = skip_lookup ? 0 : Swig_symbol_clookup_no_inherit(cname, 0);

  if (cname_node) {
    /* The symbol has been defined already or is in another scope.
       If it is a weak symbol, it needs replacing and if it was brought into the current scope
       via a using declaration, the scope needs adjusting appropriately for the new symbol.
       Similarly for defined templates. */
    Symtab *symtab = Getattr(cname_node, "sym:symtab");
    Node *sym_weak = Getattr(cname_node, "sym:weak");
    if ((symtab && sym_weak) || Equal(nodeType(cname_node), "template")) {
      /* Check if the scope is the current scope */
      String *current_scopename = Swig_symbol_qualifiedscopename(0);
      String *found_scopename = Swig_symbol_qualifiedscopename(symtab);
      int len;
      if (!current_scopename)
	current_scopename = NewString("");
      if (!found_scopename)
	found_scopename = NewString("");
      len = Len(current_scopename);
      if ((len > 0) && (Strncmp(current_scopename, found_scopename, len) == 0)) {
	if (Len(found_scopename) > len + 2) {
	  /* A matching weak symbol was found in non-global scope, some scope adjustment may be required */
	  String *new_cname = NewString(Char(found_scopename) + len + 2); /* skip over "::" prefix */
	  String *base = Swig_scopename_last(cname);
	  Printf(new_cname, "::%s", base);
	  cname = new_cname;
	  Delete(base);
	} else {
	  /* A matching weak symbol was found in the same non-global local scope, no scope adjustment required */
	  assert(len == Len(found_scopename));
	}
      } else {
	String *base = Swig_scopename_last(cname);
	if (Len(found_scopename) > 0) {
	  /* A matching weak symbol was found in a different scope to the local scope - probably via a using declaration */
	  cname = NewStringf("%s::%s", found_scopename, base);
	} else {
	  /* Either:
	      1) A matching weak symbol was found in a different scope to the local scope - this is actually a
	      symbol with the same name in a different scope which we don't want, so no adjustment required.
	      2) A matching weak symbol was found in the global scope - no adjustment required.
	  */
	  cname = Copy(base);
	}
	Delete(base);
      }
      Delete(current_scopename);
      Delete(found_scopename);
    }
  }

  if (Swig_scopename_check(cname)) {
    Node   *ns;
    String *prefix = Swig_scopename_prefix(cname);
    String *base = Swig_scopename_last(cname);
    if (prefix && (Strncmp(prefix,"::",2) == 0)) {
/* I don't think we can use :: global scope to declare classes and hence neither %template. - consider reporting error instead - wsfulton. */
      /* Use the global scope */
      String *nprefix = NewString(Char(prefix)+2);
      Delete(prefix);
      prefix= nprefix;
      gscope = set_scope_to_global();
    }
    if (Len(prefix) == 0) {
      /* Use the global scope, but we need to add a 'global' namespace.  */
      if (!gscope) gscope = set_scope_to_global();
      /* note that this namespace is not the "unnamed" one,
	 and we don't use Setattr(nscope,"name", ""),
	 because the unnamed namespace is private */
      nscope = new_node("namespace");
      Setattr(nscope,"symtab", gscope);;
      nscope_inner = nscope;
      return base;
    }
    /* Try to locate the scope */
    ns = Swig_symbol_clookup(prefix,0);
    if (!ns) {
      Swig_error(cparse_file,cparse_line,"Undefined scope '%s'\n", prefix);
    } else {
      Symtab *nstab = Getattr(ns,"symtab");
      if (!nstab) {
	Swig_error(cparse_file,cparse_line, "'%s' is not defined as a valid scope.\n", prefix);
	ns = 0;
      } else {
	/* Check if the node scope is the current scope */
	String *tname = Swig_symbol_qualifiedscopename(0);
	String *nname = Swig_symbol_qualifiedscopename(nstab);
	if (tname && (Strcmp(tname,nname) == 0)) {
	  ns = 0;
	  cname = base;
	}
	Delete(tname);
	Delete(nname);
      }
      if (ns) {
	/* we will try to create a new node using the namespaces we
	   can find in the scope name */
	List *scopes;
	String *sname;
	Iterator si;
	String *name = NewString(prefix);
	scopes = NewList();
	while (name) {
	  String *base = Swig_scopename_last(name);
	  String *tprefix = Swig_scopename_prefix(name);
	  Insert(scopes,0,base);
	  Delete(base);
	  Delete(name);
	  name = tprefix;
	}
	for (si = First(scopes); si.item; si = Next(si)) {
	  Node *ns1,*ns2;
	  sname = si.item;
	  ns1 = Swig_symbol_clookup(sname,0);
	  assert(ns1);
	  if (Strcmp(nodeType(ns1),"namespace") == 0) {
	    if (Getattr(ns1,"alias")) {
	      ns1 = Getattr(ns1,"namespace");
	    }
	  } else {
	    /* now this last part is a class */
	    si = Next(si);
	    /*  or a nested class tree, which is unrolled here */
	    for (; si.item; si = Next(si)) {
	      if (si.item) {
		Printf(sname,"::%s",si.item);
	      }
	    }
	    /* we get the 'inner' class */
	    nscope_inner = Swig_symbol_clookup(sname,0);
	    /* set the scope to the inner class */
	    Swig_symbol_setscope(Getattr(nscope_inner,"symtab"));
	    /* save the last namespace prefix */
	    Delete(Namespaceprefix);
	    Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	    /* and return the node name, including the inner class prefix */
	    break;
	  }
	  /* here we just populate the namespace tree as usual */
	  ns2 = new_node("namespace");
	  Setattr(ns2,"name",sname);
	  Setattr(ns2,"symtab", Getattr(ns1,"symtab"));
	  add_symbols(ns2);
	  Swig_symbol_setscope(Getattr(ns1,"symtab"));
	  Delete(Namespaceprefix);
	  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	  if (nscope_inner) {
	    if (Getattr(nscope_inner,"symtab") != Getattr(ns2,"symtab")) {
	      appendChild(nscope_inner,ns2);
	      Delete(ns2);
	    }
	  }
	  nscope_inner = ns2;
	  if (!nscope) nscope = ns2;
	}
	cname = base;
	Delete(scopes);
      }
    }
    Delete(prefix);
  }

  return cname;
}
 
/* look for simple typedef name in typedef list */
static String *try_to_find_a_name_for_unnamed_structure(const char *storage, Node *decls) {
  String *name = 0;
  Node *n = decls;
  if (storage && (strcmp(storage, "typedef") == 0)) {
    for (; n; n = nextSibling(n)) {
      if (!Len(Getattr(n, "decl"))) {
	name = Copy(Getattr(n, "name"));
	break;
      }
    }
  }
  return name;
}

/* traverse copied tree segment, and update outer class links*/
static void update_nested_classes(Node *n)
{
  Node *c = firstChild(n);
  while (c) {
    if (Getattr(c, "nested:outer"))
      Setattr(c, "nested:outer", n);
    update_nested_classes(c);
    c = nextSibling(c);
  }
}

/* -----------------------------------------------------------------------------
 * nested_forward_declaration()
 * 
 * Nested struct handling for C++ code if the nested classes are disabled.
 * Create the nested class/struct/union as a forward declaration.
 * ----------------------------------------------------------------------------- */

static Node *nested_forward_declaration(const char *storage, const char *kind, String *sname, String *name, Node *cpp_opt_declarators) {
  Node *nn = 0;

  if (sname) {
    /* Add forward declaration of the nested type */
    Node *n = new_node("classforward");
    Setattr(n, "kind", kind);
    Setattr(n, "name", sname);
    Setattr(n, "storage", storage);
    Setattr(n, "sym:weak", "1");
    add_symbols(n);
    nn = n;
  }

  /* Add any variable instances. Also add in any further typedefs of the nested type.
     Note that anonymous typedefs (eg typedef struct {...} a, b;) are treated as class forward declarations */
  if (cpp_opt_declarators) {
    int storage_typedef = (storage && (strcmp(storage, "typedef") == 0));
    int variable_of_anonymous_type = !sname && !storage_typedef;
    if (!variable_of_anonymous_type) {
      int anonymous_typedef = !sname && (storage && (strcmp(storage, "typedef") == 0));
      Node *n = cpp_opt_declarators;
      SwigType *type = name;
      while (n) {
	Setattr(n, "type", type);
	Setattr(n, "storage", storage);
	if (anonymous_typedef) {
	  Setattr(n, "nodeType", "classforward");
	  Setattr(n, "sym:weak", "1");
	}
	n = nextSibling(n);
      }
      add_symbols(cpp_opt_declarators);

      if (nn) {
	set_nextSibling(nn, cpp_opt_declarators);
      } else {
	nn = cpp_opt_declarators;
      }
    }
  }

  if (!currentOuterClass || !GetFlag(currentOuterClass, "nested")) {
    if (nn && Equal(nodeType(nn), "classforward")) {
      Node *n = nn;
      if (!GetFlag(n, "feature:ignore")) {
	SWIG_WARN_NODE_BEGIN(n);
	Swig_warning(WARN_PARSE_NAMED_NESTED_CLASS, cparse_file, cparse_line,"Nested %s not currently supported (%s ignored)\n", kind, sname ? sname : name);
	SWIG_WARN_NODE_END(n);
      }
    } else {
      Swig_warning(WARN_PARSE_UNNAMED_NESTED_CLASS, cparse_file, cparse_line, "Nested %s not currently supported (ignored).\n", kind);
    }
  }

  return nn;
}


Node *Swig_cparse(File *f) {
  scanner_file(f);
  top = 0;
  yyparse();
  return top;
}

static void single_new_feature(const char *featurename, String *val, Hash *featureattribs, char *declaratorid, SwigType *type, ParmList *declaratorparms, String *qualifier) {
  String *fname;
  String *name;
  String *fixname;
  SwigType *t = Copy(type);

  /* Printf(stdout, "single_new_feature: [%s] [%s] [%s] [%s] [%s] [%s]\n", featurename, val, declaratorid, t, ParmList_str_defaultargs(declaratorparms), qualifier); */

  /* Warn about deprecated features */
  if (strcmp(featurename, "nestedworkaround") == 0)
    Swig_warning(WARN_DEPRECATED_NESTED_WORKAROUND, cparse_file, cparse_line, "The 'nestedworkaround' feature is deprecated.\n");

  fname = NewStringf("feature:%s",featurename);
  if (declaratorid) {
    fixname = feature_identifier_fix(declaratorid);
  } else {
    fixname = NewStringEmpty();
  }
  if (Namespaceprefix) {
    name = NewStringf("%s::%s",Namespaceprefix, fixname);
  } else {
    name = fixname;
  }

  if (declaratorparms) Setmeta(val,"parms",declaratorparms);
  if (!Len(t)) t = 0;
  if (t) {
    if (qualifier) SwigType_push(t,qualifier);
    if (SwigType_isfunction(t)) {
      SwigType *decl = SwigType_pop_function(t);
      if (SwigType_ispointer(t)) {
	String *nname = NewStringf("*%s",name);
	Swig_feature_set(Swig_cparse_features(), nname, decl, fname, val, featureattribs);
	Delete(nname);
      } else {
	Swig_feature_set(Swig_cparse_features(), name, decl, fname, val, featureattribs);
      }
      Delete(decl);
    } else if (SwigType_ispointer(t)) {
      String *nname = NewStringf("*%s",name);
      Swig_feature_set(Swig_cparse_features(),nname,0,fname,val, featureattribs);
      Delete(nname);
    }
  } else {
    /* Global feature, that is, feature not associated with any particular symbol */
    Swig_feature_set(Swig_cparse_features(),name,0,fname,val, featureattribs);
  }
  Delete(fname);
  Delete(name);
}

/* Add a new feature to the Hash. Additional features are added if the feature has a parameter list (declaratorparms)
 * and one or more of the parameters have a default argument. An extra feature is added for each defaulted parameter,
 * simulating the equivalent overloaded method. */
static void new_feature(const char *featurename, String *val, Hash *featureattribs, char *declaratorid, SwigType *type, ParmList *declaratorparms, String *qualifier) {

  ParmList *declparms = declaratorparms;

  /* remove the { and } braces if the noblock attribute is set */
  String *newval = remove_block(featureattribs, val);
  val = newval ? newval : val;

  /* Add the feature */
  single_new_feature(featurename, val, featureattribs, declaratorid, type, declaratorparms, qualifier);

  /* Add extra features if there are default parameters in the parameter list */
  if (type) {
    while (declparms) {
      if (ParmList_has_defaultargs(declparms)) {

        /* Create a parameter list for the new feature by copying all
           but the last (defaulted) parameter */
        ParmList* newparms = CopyParmListMax(declparms, ParmList_len(declparms)-1);

        /* Create new declaration - with the last parameter removed */
        SwigType *newtype = Copy(type);
        Delete(SwigType_pop_function(newtype)); /* remove the old parameter list from newtype */
        SwigType_add_function(newtype,newparms);

        single_new_feature(featurename, Copy(val), featureattribs, declaratorid, newtype, newparms, qualifier);
        declparms = newparms;
      } else {
        declparms = 0;
      }
    }
  }
}

/* check if a function declaration is a plain C object */
static int is_cfunction(Node *n) {
  if (!cparse_cplusplus || cparse_externc)
    return 1;
  if (Swig_storage_isexternc(n)) {
    return 1;
  }
  return 0;
}

/* If the Node is a function with parameters, check to see if any of the parameters
 * have default arguments. If so create a new function for each defaulted argument. 
 * The additional functions form a linked list of nodes with the head being the original Node n. */
static void default_arguments(Node *n) {
  Node *function = n;

  if (function) {
    ParmList *varargs = Getattr(function,"feature:varargs");
    if (varargs) {
      /* Handles the %varargs directive by looking for "feature:varargs" and 
       * substituting ... with an alternative set of arguments.  */
      Parm     *p = Getattr(function,"parms");
      Parm     *pp = 0;
      while (p) {
	SwigType *t = Getattr(p,"type");
	if (Strcmp(t,"v(...)") == 0) {
	  if (pp) {
	    ParmList *cv = Copy(varargs);
	    set_nextSibling(pp,cv);
	    Delete(cv);
	  } else {
	    ParmList *cv =  Copy(varargs);
	    Setattr(function,"parms", cv);
	    Delete(cv);
	  }
	  break;
	}
	pp = p;
	p = nextSibling(p);
      }
    }

    /* Do not add in functions if kwargs is being used or if user wants old default argument wrapping
       (one wrapped method per function irrespective of number of default arguments) */
    if (compact_default_args 
	|| is_cfunction(function) 
	|| GetFlag(function,"feature:compactdefaultargs") 
	|| (GetFlag(function,"feature:kwargs") && kwargs_supported)) {
      ParmList *p = Getattr(function,"parms");
      if (p) 
        Setattr(p,"compactdefargs", "1"); /* mark parameters for special handling */
      function = 0; /* don't add in extra methods */
    }
  }

  while (function) {
    ParmList *parms = Getattr(function,"parms");
    if (ParmList_has_defaultargs(parms)) {

      /* Create a parameter list for the new function by copying all
         but the last (defaulted) parameter */
      ParmList* newparms = CopyParmListMax(parms,ParmList_len(parms)-1);

      /* Create new function and add to symbol table */
      {
	SwigType *ntype = Copy(nodeType(function));
	char *cntype = Char(ntype);
        Node *new_function = new_node(ntype);
        SwigType *decl = Copy(Getattr(function,"decl"));
        int constqualifier = SwigType_isconst(decl);
	String *ccode = Copy(Getattr(function,"code"));
	String *cstorage = Copy(Getattr(function,"storage"));
	String *cvalue = Copy(Getattr(function,"value"));
	SwigType *ctype = Copy(Getattr(function,"type"));
	String *cthrow = Copy(Getattr(function,"throw"));

        Delete(SwigType_pop_function(decl)); /* remove the old parameter list from decl */
        SwigType_add_function(decl,newparms);
        if (constqualifier)
          SwigType_add_qualifier(decl,"const");

        Setattr(new_function,"name", Getattr(function,"name"));
        Setattr(new_function,"code", ccode);
        Setattr(new_function,"decl", decl);
        Setattr(new_function,"parms", newparms);
        Setattr(new_function,"storage", cstorage);
        Setattr(new_function,"value", cvalue);
        Setattr(new_function,"type", ctype);
        Setattr(new_function,"throw", cthrow);

	Delete(ccode);
	Delete(cstorage);
	Delete(cvalue);
	Delete(ctype);
	Delete(cthrow);
	Delete(decl);

        {
          Node *throws = Getattr(function,"throws");
	  ParmList *pl = CopyParmList(throws);
          if (throws) Setattr(new_function,"throws",pl);
	  Delete(pl);
        }

        /* copy specific attributes for global (or in a namespace) template functions - these are not templated class methods */
        if (strcmp(cntype,"template") == 0) {
          Node *templatetype = Getattr(function,"templatetype");
          Node *symtypename = Getattr(function,"sym:typename");
          Parm *templateparms = Getattr(function,"templateparms");
          if (templatetype) {
	    Node *tmp = Copy(templatetype);
	    Setattr(new_function,"templatetype",tmp);
	    Delete(tmp);
	  }
          if (symtypename) {
	    Node *tmp = Copy(symtypename);
	    Setattr(new_function,"sym:typename",tmp);
	    Delete(tmp);
	  }
          if (templateparms) {
	    Parm *tmp = CopyParmList(templateparms);
	    Setattr(new_function,"templateparms",tmp);
	    Delete(tmp);
	  }
        } else if (strcmp(cntype,"constructor") == 0) {
          /* only copied for constructors as this is not a user defined feature - it is hard coded in the parser */
          if (GetFlag(function,"feature:new")) SetFlag(new_function,"feature:new");
        }

        add_symbols(new_function);
        /* mark added functions as ones with overloaded parameters and point to the parsed method */
        Setattr(new_function,"defaultargs", n);

        /* Point to the new function, extending the linked list */
        set_nextSibling(function, new_function);
	Delete(new_function);
        function = new_function;
	
	Delete(ntype);
      }
    } else {
      function = 0;
    }
  }
}

/* -----------------------------------------------------------------------------
 * mark_nodes_as_extend()
 *
 * Used by the %extend to mark subtypes with "feature:extend".
 * template instances declared within %extend are skipped
 * ----------------------------------------------------------------------------- */

static void mark_nodes_as_extend(Node *n) {
  for (; n; n = nextSibling(n)) {
    if (Getattr(n, "template") && Strcmp(nodeType(n), "class") == 0)
      continue;
    /* Fix me: extend is not a feature. Replace with isextendmember? */
    Setattr(n, "feature:extend", "1");
    mark_nodes_as_extend(firstChild(n));
  }
}



/* Line 189 of yacc.c  */
#line 1352 "CParse/parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     HBLOCK = 259,
     POUND = 260,
     STRING = 261,
     WSTRING = 262,
     INCLUDE = 263,
     IMPORT = 264,
     INSERT = 265,
     CHARCONST = 266,
     WCHARCONST = 267,
     NUM_INT = 268,
     NUM_FLOAT = 269,
     NUM_UNSIGNED = 270,
     NUM_LONG = 271,
     NUM_ULONG = 272,
     NUM_LONGLONG = 273,
     NUM_ULONGLONG = 274,
     NUM_BOOL = 275,
     TYPEDEF = 276,
     TYPE_INT = 277,
     TYPE_UNSIGNED = 278,
     TYPE_SHORT = 279,
     TYPE_LONG = 280,
     TYPE_FLOAT = 281,
     TYPE_DOUBLE = 282,
     TYPE_CHAR = 283,
     TYPE_WCHAR = 284,
     TYPE_VOID = 285,
     TYPE_SIGNED = 286,
     TYPE_BOOL = 287,
     TYPE_COMPLEX = 288,
     TYPE_TYPEDEF = 289,
     TYPE_RAW = 290,
     TYPE_NON_ISO_INT8 = 291,
     TYPE_NON_ISO_INT16 = 292,
     TYPE_NON_ISO_INT32 = 293,
     TYPE_NON_ISO_INT64 = 294,
     LPAREN = 295,
     RPAREN = 296,
     COMMA = 297,
     SEMI = 298,
     EXTERN = 299,
     INIT = 300,
     LBRACE = 301,
     RBRACE = 302,
     PERIOD = 303,
     CONST_QUAL = 304,
     VOLATILE = 305,
     REGISTER = 306,
     STRUCT = 307,
     UNION = 308,
     EQUAL = 309,
     SIZEOF = 310,
     MODULE = 311,
     LBRACKET = 312,
     RBRACKET = 313,
     BEGINFILE = 314,
     ENDOFFILE = 315,
     ILLEGAL = 316,
     CONSTANT = 317,
     NAME = 318,
     RENAME = 319,
     NAMEWARN = 320,
     EXTEND = 321,
     PRAGMA = 322,
     FEATURE = 323,
     VARARGS = 324,
     ENUM = 325,
     CLASS = 326,
     TYPENAME = 327,
     PRIVATE = 328,
     PUBLIC = 329,
     PROTECTED = 330,
     COLON = 331,
     STATIC = 332,
     VIRTUAL = 333,
     FRIEND = 334,
     THROW = 335,
     CATCH = 336,
     EXPLICIT = 337,
     STATIC_ASSERT = 338,
     CONSTEXPR = 339,
     THREAD_LOCAL = 340,
     DECLTYPE = 341,
     AUTO = 342,
     NOEXCEPT = 343,
     OVERRIDE = 344,
     FINAL = 345,
     USING = 346,
     NAMESPACE = 347,
     NATIVE = 348,
     INLINE = 349,
     TYPEMAP = 350,
     EXCEPT = 351,
     ECHO = 352,
     APPLY = 353,
     CLEAR = 354,
     SWIGTEMPLATE = 355,
     FRAGMENT = 356,
     WARN = 357,
     LESSTHAN = 358,
     GREATERTHAN = 359,
     DELETE_KW = 360,
     DEFAULT = 361,
     LESSTHANOREQUALTO = 362,
     GREATERTHANOREQUALTO = 363,
     EQUALTO = 364,
     NOTEQUALTO = 365,
     ARROW = 366,
     QUESTIONMARK = 367,
     TYPES = 368,
     PARMS = 369,
     NONID = 370,
     DSTAR = 371,
     DCNOT = 372,
     TEMPLATE = 373,
     OPERATOR = 374,
     CONVERSIONOPERATOR = 375,
     PARSETYPE = 376,
     PARSEPARM = 377,
     PARSEPARMS = 378,
     CAST = 379,
     LOR = 380,
     LAND = 381,
     OR = 382,
     XOR = 383,
     AND = 384,
     RSHIFT = 385,
     LSHIFT = 386,
     MINUS = 387,
     PLUS = 388,
     MODULO = 389,
     SLASH = 390,
     STAR = 391,
     LNOT = 392,
     NOT = 393,
     UMINUS = 394,
     DCOLON = 395
   };
#endif
/* Tokens.  */
#define ID 258
#define HBLOCK 259
#define POUND 260
#define STRING 261
#define WSTRING 262
#define INCLUDE 263
#define IMPORT 264
#define INSERT 265
#define CHARCONST 266
#define WCHARCONST 267
#define NUM_INT 268
#define NUM_FLOAT 269
#define NUM_UNSIGNED 270
#define NUM_LONG 271
#define NUM_ULONG 272
#define NUM_LONGLONG 273
#define NUM_ULONGLONG 274
#define NUM_BOOL 275
#define TYPEDEF 276
#define TYPE_INT 277
#define TYPE_UNSIGNED 278
#define TYPE_SHORT 279
#define TYPE_LONG 280
#define TYPE_FLOAT 281
#define TYPE_DOUBLE 282
#define TYPE_CHAR 283
#define TYPE_WCHAR 284
#define TYPE_VOID 285
#define TYPE_SIGNED 286
#define TYPE_BOOL 287
#define TYPE_COMPLEX 288
#define TYPE_TYPEDEF 289
#define TYPE_RAW 290
#define TYPE_NON_ISO_INT8 291
#define TYPE_NON_ISO_INT16 292
#define TYPE_NON_ISO_INT32 293
#define TYPE_NON_ISO_INT64 294
#define LPAREN 295
#define RPAREN 296
#define COMMA 297
#define SEMI 298
#define EXTERN 299
#define INIT 300
#define LBRACE 301
#define RBRACE 302
#define PERIOD 303
#define CONST_QUAL 304
#define VOLATILE 305
#define REGISTER 306
#define STRUCT 307
#define UNION 308
#define EQUAL 309
#define SIZEOF 310
#define MODULE 311
#define LBRACKET 312
#define RBRACKET 313
#define BEGINFILE 314
#define ENDOFFILE 315
#define ILLEGAL 316
#define CONSTANT 317
#define NAME 318
#define RENAME 319
#define NAMEWARN 320
#define EXTEND 321
#define PRAGMA 322
#define FEATURE 323
#define VARARGS 324
#define ENUM 325
#define CLASS 326
#define TYPENAME 327
#define PRIVATE 328
#define PUBLIC 329
#define PROTECTED 330
#define COLON 331
#define STATIC 332
#define VIRTUAL 333
#define FRIEND 334
#define THROW 335
#define CATCH 336
#define EXPLICIT 337
#define STATIC_ASSERT 338
#define CONSTEXPR 339
#define THREAD_LOCAL 340
#define DECLTYPE 341
#define AUTO 342
#define NOEXCEPT 343
#define OVERRIDE 344
#define FINAL 345
#define USING 346
#define NAMESPACE 347
#define NATIVE 348
#define INLINE 349
#define TYPEMAP 350
#define EXCEPT 351
#define ECHO 352
#define APPLY 353
#define CLEAR 354
#define SWIGTEMPLATE 355
#define FRAGMENT 356
#define WARN 357
#define LESSTHAN 358
#define GREATERTHAN 359
#define DELETE_KW 360
#define DEFAULT 361
#define LESSTHANOREQUALTO 362
#define GREATERTHANOREQUALTO 363
#define EQUALTO 364
#define NOTEQUALTO 365
#define ARROW 366
#define QUESTIONMARK 367
#define TYPES 368
#define PARMS 369
#define NONID 370
#define DSTAR 371
#define DCNOT 372
#define TEMPLATE 373
#define OPERATOR 374
#define CONVERSIONOPERATOR 375
#define PARSETYPE 376
#define PARSEPARM 377
#define PARSEPARMS 378
#define CAST 379
#define LOR 380
#define LAND 381
#define OR 382
#define XOR 383
#define AND 384
#define RSHIFT 385
#define LSHIFT 386
#define MINUS 387
#define PLUS 388
#define MODULO 389
#define SLASH 390
#define STAR 391
#define LNOT 392
#define NOT 393
#define UMINUS 394
#define DCOLON 395




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 1301 "parser.y"

  char  *id;
  List  *bases;
  struct Define {
    String *val;
    String *rawval;
    int     type;
    String *qualifier;
    String *bitfield;
    Parm   *throws;
    String *throwf;
    String *nexcept;
  } dtype;
  struct {
    const char *type;
    String *filename;
    int   line;
  } loc;
  struct {
    char      *id;
    SwigType  *type;
    String    *defarg;
    ParmList  *parms;
    short      have_parms;
    ParmList  *throws;
    String    *throwf;
    String    *nexcept;
  } decl;
  Parm         *tparms;
  struct {
    String     *method;
    Hash       *kwargs;
  } tmap;
  struct {
    String     *type;
    String     *us;
  } ptype;
  SwigType     *type;
  String       *str;
  Parm         *p;
  ParmList     *pl;
  int           intvalue;
  Node         *node;



/* Line 214 of yacc.c  */
#line 1715 "CParse/parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 1727 "CParse/parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  61
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   5102

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  141
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  168
/* YYNRULES -- Number of rules.  */
#define YYNRULES  574
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1120

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   395

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    16,    19,    25,    29,
      32,    34,    36,    38,    40,    42,    44,    46,    49,    51,
      53,    55,    57,    59,    61,    63,    65,    67,    69,    71,
      73,    75,    77,    79,    81,    83,    85,    87,    89,    91,
      92,   100,   106,   110,   116,   122,   126,   129,   132,   138,
     141,   147,   150,   155,   157,   159,   167,   175,   181,   182,
     190,   192,   194,   197,   200,   202,   208,   214,   220,   224,
     229,   233,   241,   250,   256,   260,   262,   264,   268,   270,
     275,   283,   290,   292,   294,   302,   312,   321,   332,   338,
     346,   353,   362,   364,   366,   372,   377,   383,   391,   393,
     397,   404,   411,   420,   422,   425,   429,   431,   434,   438,
     445,   451,   461,   464,   466,   468,   470,   471,   478,   480,
     484,   493,   499,   507,   509,   514,   516,   518,   520,   523,
     526,   530,   532,   534,   536,   539,   541,   543,   545,   557,
     571,   579,   581,   583,   585,   586,   590,   592,   595,   598,
     601,   603,   609,   618,   629,   636,   638,   640,   642,   644,
     646,   648,   649,   659,   660,   669,   671,   675,   680,   681,
     688,   692,   697,   699,   701,   703,   705,   707,   709,   711,
     713,   716,   718,   720,   722,   726,   728,   732,   737,   738,
     745,   746,   752,   758,   761,   762,   763,   771,   773,   775,
     776,   780,   782,   784,   786,   788,   790,   792,   794,   796,
     798,   802,   804,   806,   808,   810,   812,   814,   816,   818,
     820,   827,   834,   842,   851,   860,   869,   879,   887,   893,
     896,   899,   902,   905,   907,   909,   911,   913,   915,   917,
     919,   921,   923,   925,   927,   930,   935,   938,   941,   946,
     949,   955,   957,   959,   961,   964,   966,   968,   971,   973,
     975,   978,   981,   983,   985,   987,   989,   991,   993,   996,
     999,  1002,  1005,  1007,  1010,  1013,  1016,  1019,  1021,  1023,
    1026,  1028,  1032,  1034,  1037,  1045,  1049,  1051,  1054,  1056,
    1060,  1062,  1064,  1066,  1069,  1075,  1078,  1081,  1083,  1086,
    1089,  1091,  1093,  1095,  1097,  1100,  1104,  1108,  1110,  1113,
    1116,  1120,  1125,  1131,  1136,  1142,  1149,  1156,  1161,  1167,
    1173,  1180,  1188,  1197,  1206,  1214,  1222,  1224,  1227,  1231,
    1236,  1242,  1246,  1251,  1256,  1258,  1261,  1266,  1271,  1276,
    1282,  1286,  1291,  1296,  1302,  1304,  1307,  1310,  1313,  1317,
    1321,  1323,  1326,  1329,  1331,  1333,  1336,  1340,  1345,  1349,
    1354,  1357,  1361,  1365,  1370,  1374,  1378,  1381,  1384,  1386,
    1388,  1391,  1393,  1395,  1397,  1399,  1402,  1404,  1407,  1411,
    1413,  1415,  1417,  1420,  1423,  1425,  1427,  1430,  1432,  1437,
    1439,  1441,  1444,  1446,  1448,  1450,  1452,  1454,  1456,  1458,
    1460,  1462,  1464,  1466,  1468,  1470,  1472,  1473,  1476,  1478,
    1480,  1482,  1484,  1486,  1488,  1490,  1492,  1494,  1500,  1504,
    1508,  1510,  1512,  1516,  1518,  1520,  1522,  1524,  1526,  1532,
    1541,  1543,  1545,  1547,  1549,  1553,  1558,  1564,  1570,  1576,
    1583,  1590,  1593,  1596,  1599,  1601,  1603,  1605,  1607,  1609,
    1611,  1613,  1615,  1619,  1623,  1627,  1631,  1635,  1639,  1643,
    1647,  1651,  1655,  1659,  1663,  1667,  1671,  1675,  1679,  1685,
    1688,  1691,  1694,  1697,  1700,  1704,  1706,  1708,  1710,  1711,
    1715,  1717,  1719,  1723,  1724,  1729,  1730,  1737,  1739,  1741,
    1743,  1745,  1747,  1752,  1757,  1759,  1761,  1763,  1765,  1767,
    1769,  1771,  1774,  1777,  1782,  1784,  1786,  1789,  1794,  1796,
    1798,  1801,  1803,  1807,  1811,  1816,  1821,  1825,  1830,  1833,
    1835,  1837,  1841,  1846,  1853,  1856,  1859,  1863,  1865,  1867,
    1869,  1871,  1873,  1875,  1877,  1879,  1881,  1884,  1889,  1891,
    1895,  1898,  1902,  1906,  1909,  1912,  1915,  1918,  1921,  1926,
    1928,  1932,  1934,  1938,  1942,  1945,  1948,  1951,  1954,  1956,
    1959,  1961,  1963,  1965,  1967,  1971,  1973,  1977,  1983,  1985,
    1989,  1993,  1999,  2001,  2003
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     142,     0,    -1,   143,    -1,   121,   236,    43,    -1,   121,
       1,    -1,   122,   236,    43,    -1,   122,     1,    -1,   123,
      40,   233,    41,    43,    -1,   123,     1,    43,    -1,   143,
     144,    -1,   308,    -1,   145,    -1,   182,    -1,   198,    -1,
      43,    -1,     1,    -1,   197,    -1,     1,   120,    -1,   146,
      -1,   148,    -1,   149,    -1,   150,    -1,   151,    -1,   152,
      -1,   155,    -1,   156,    -1,   159,    -1,   160,    -1,   161,
      -1,   162,    -1,   163,    -1,   164,    -1,   167,    -1,   169,
      -1,   172,    -1,   174,    -1,   179,    -1,   180,    -1,   181,
      -1,    -1,    66,   305,   297,    46,   147,   215,    47,    -1,
      98,   178,    46,   176,    47,    -1,    99,   176,    43,    -1,
      62,   294,    54,   259,    43,    -1,    62,   252,   244,   241,
      43,    -1,    62,     1,    43,    -1,    97,     4,    -1,    97,
     302,    -1,    96,    40,   294,    41,    46,    -1,    96,    46,
      -1,    96,    40,   294,    41,    43,    -1,    96,    43,    -1,
     302,    46,   236,    47,    -1,   302,    -1,   153,    -1,   101,
      40,   154,    42,   306,    41,     4,    -1,   101,    40,   154,
      42,   306,    41,    46,    -1,   101,    40,   154,    41,    43,
      -1,    -1,   158,   305,   302,    59,   157,   143,    60,    -1,
       8,    -1,     9,    -1,    94,     4,    -1,    94,    46,    -1,
       4,    -1,    10,    40,   295,    41,   302,    -1,    10,    40,
     295,    41,     4,    -1,    10,    40,   295,    41,    46,    -1,
      56,   305,   295,    -1,    63,    40,   295,    41,    -1,    63,
      40,    41,    -1,    93,    40,   294,    41,   232,   294,    43,
      -1,    93,    40,   294,    41,   232,   252,   244,    43,    -1,
      67,   166,   294,    54,   165,    -1,    67,   166,   294,    -1,
     302,    -1,     4,    -1,    40,   294,    41,    -1,   308,    -1,
     168,   244,   295,    43,    -1,   168,    40,   306,    41,   244,
     288,    43,    -1,   168,    40,   306,    41,   302,    43,    -1,
      64,    -1,    65,    -1,    68,    40,   295,    41,   244,   288,
     170,    -1,    68,    40,   295,    42,   307,    41,   244,   288,
      43,    -1,    68,    40,   295,   171,    41,   244,   288,   170,
      -1,    68,    40,   295,    42,   307,   171,    41,   244,   288,
      43,    -1,    68,    40,   295,    41,   170,    -1,    68,    40,
     295,    42,   307,    41,    43,    -1,    68,    40,   295,   171,
      41,   170,    -1,    68,    40,   295,    42,   307,   171,    41,
      43,    -1,   304,    -1,    43,    -1,   114,    40,   233,    41,
      43,    -1,    42,   295,    54,   307,    -1,    42,   295,    54,
     307,   171,    -1,    69,    40,   173,    41,   244,   288,    43,
      -1,   233,    -1,    13,    42,   236,    -1,    95,    40,   175,
      41,   176,   304,    -1,    95,    40,   175,    41,   176,    43,
      -1,    95,    40,   175,    41,   176,    54,   178,    43,    -1,
     306,    -1,   178,   177,    -1,    42,   178,   177,    -1,   308,
      -1,   252,   243,    -1,    40,   233,    41,    -1,    40,   233,
      41,    40,   233,    41,    -1,   113,    40,   233,    41,   170,
      -1,   100,    40,   296,    41,   300,   103,   237,   104,    43,
      -1,   102,   302,    -1,   184,    -1,   196,    -1,   195,    -1,
      -1,    44,   302,    46,   183,   143,    47,    -1,   188,    -1,
      91,   297,    54,    -1,   118,   103,   207,   104,    91,   297,
      54,   294,    -1,   232,   252,   244,   186,   185,    -1,   232,
      87,   244,   111,   187,   186,   185,    -1,    43,    -1,    42,
     244,   186,   185,    -1,    46,    -1,     1,    -1,   241,    -1,
     250,   241,    -1,   287,   241,    -1,   250,   287,   241,    -1,
     256,    -1,    32,    -1,    30,    -1,    34,   293,    -1,    35,
      -1,   297,    -1,   255,    -1,   232,    87,   297,    54,   189,
      40,   233,    41,   288,   190,   191,    -1,   232,    87,   297,
      54,   189,    40,   233,    41,   288,   111,   252,   190,   191,
      -1,   232,    87,   297,    54,   189,   190,   191,    -1,    57,
      -1,    46,    -1,    43,    -1,    -1,    40,   192,    43,    -1,
      70,    -1,    70,    71,    -1,    70,    52,    -1,    76,   254,
      -1,   308,    -1,   232,   193,   264,   194,    43,    -1,   232,
     193,   264,   194,    46,   266,    47,    43,    -1,   232,   193,
     264,   194,    46,   266,    47,   244,   186,   185,    -1,   232,
     252,    40,   233,    41,   289,    -1,   199,    -1,   203,    -1,
     204,    -1,   211,    -1,   212,    -1,   223,    -1,    -1,   232,
     284,   297,   275,    46,   200,   215,    47,   202,    -1,    -1,
     232,   284,   275,    46,   201,   215,    47,   202,    -1,    43,
      -1,   244,   186,   185,    -1,   232,   284,   297,    43,    -1,
      -1,   118,   103,   207,   104,   205,   206,    -1,   118,   284,
     297,    -1,    44,   118,   284,   297,    -1,   184,    -1,   199,
      -1,   220,    -1,   224,    -1,   204,    -1,   203,    -1,   222,
      -1,   208,    -1,   209,   210,    -1,   308,    -1,   283,    -1,
     236,    -1,    42,   209,   210,    -1,   308,    -1,    91,   297,
      43,    -1,    91,    92,   297,    43,    -1,    -1,    92,   297,
      46,   213,   143,    47,    -1,    -1,    92,    46,   214,   143,
      47,    -1,    92,   294,    54,   297,    43,    -1,   219,   215,
      -1,    -1,    -1,    66,    46,   216,   215,    47,   217,   215,
      -1,   156,    -1,   308,    -1,    -1,     1,   218,   215,    -1,
     182,    -1,   220,    -1,   221,    -1,   224,    -1,   225,    -1,
     226,    -1,   222,    -1,   203,    -1,   199,    -1,   232,   297,
      43,    -1,   211,    -1,   204,    -1,   223,    -1,   180,    -1,
     181,    -1,   229,    -1,   155,    -1,   179,    -1,    43,    -1,
     232,   252,    40,   233,    41,   289,    -1,   138,   299,    40,
     233,    41,   227,    -1,    78,   138,   299,    40,   233,    41,
     228,    -1,   232,   120,   252,   249,    40,   233,    41,   228,
      -1,   232,   120,   252,   129,    40,   233,    41,   228,    -1,
     232,   120,   252,   126,    40,   233,    41,   228,    -1,   232,
     120,   252,   249,   129,    40,   233,    41,   228,    -1,   232,
     120,   252,    40,   233,    41,   228,    -1,    81,    40,   233,
      41,    46,    -1,    83,    40,    -1,    74,    76,    -1,    73,
      76,    -1,    75,    76,    -1,   164,    -1,   150,    -1,   162,
      -1,   167,    -1,   169,    -1,   172,    -1,   160,    -1,   174,
      -1,   148,    -1,   149,    -1,   151,    -1,   288,    43,    -1,
     288,    54,   261,    43,    -1,   288,    46,    -1,   288,    43,
      -1,   288,    54,   259,    43,    -1,   288,    46,    -1,   232,
     230,    76,   269,    43,    -1,   256,    -1,    32,    -1,    30,
      -1,    34,   293,    -1,    35,    -1,   297,    -1,    44,   302,
      -1,    44,    -1,   231,    -1,   231,    85,    -1,   231,    21,
      -1,    77,    -1,    21,    -1,    78,    -1,    79,    -1,    82,
      -1,    84,    -1,    82,    84,    -1,    84,    82,    -1,    77,
      84,    -1,    84,    77,    -1,    85,    -1,    85,    77,    -1,
      77,    85,    -1,    44,    85,    -1,    85,    44,    -1,   308,
      -1,   234,    -1,   236,   235,    -1,   308,    -1,    42,   236,
     235,    -1,   308,    -1,   253,   242,    -1,   118,   103,   284,
     104,   284,   297,   241,    -1,    48,    48,    48,    -1,   238,
      -1,   240,   239,    -1,   308,    -1,    42,   240,   239,    -1,
     308,    -1,   236,    -1,   270,    -1,    54,   259,    -1,    54,
     259,    57,   269,    58,    -1,    54,    46,    -1,    76,   269,
      -1,   308,    -1,   244,   241,    -1,   247,   241,    -1,   241,
      -1,   244,    -1,   247,    -1,   308,    -1,   249,   245,    -1,
     249,   129,   245,    -1,   249,   126,   245,    -1,   246,    -1,
     129,   245,    -1,   126,   245,    -1,   297,   116,   245,    -1,
     249,   297,   116,   245,    -1,   249,   297,   116,   129,   245,
      -1,   297,   116,   129,   245,    -1,   249,    48,    48,    48,
     245,    -1,   249,   129,    48,    48,    48,   245,    -1,   249,
     126,    48,    48,    48,   245,    -1,    48,    48,    48,   246,
      -1,   129,    48,    48,    48,   245,    -1,   126,    48,    48,
      48,   245,    -1,   297,   116,    48,    48,    48,   245,    -1,
     249,   297,   116,    48,    48,    48,   245,    -1,   249,   297,
     116,   129,    48,    48,    48,   245,    -1,   249,   297,   116,
     126,    48,    48,    48,   245,    -1,   297,   116,   129,    48,
      48,    48,   245,    -1,   297,   116,   126,    48,    48,    48,
     245,    -1,   297,    -1,   138,   297,    -1,    40,   297,    41,
      -1,    40,   249,   245,    41,    -1,    40,   297,   116,   245,
      41,    -1,   245,    57,    58,    -1,   245,    57,   269,    58,
      -1,   245,    40,   233,    41,    -1,   297,    -1,   138,   297,
      -1,    40,   249,   246,    41,    -1,    40,   129,   246,    41,
      -1,    40,   126,   246,    41,    -1,    40,   297,   116,   246,
      41,    -1,   246,    57,    58,    -1,   246,    57,   269,    58,
      -1,   246,    40,   233,    41,    -1,   119,     3,    40,   233,
      41,    -1,   249,    -1,   249,   248,    -1,   249,   129,    -1,
     249,   126,    -1,   249,   129,   248,    -1,   249,   126,   248,
      -1,   248,    -1,   129,   248,    -1,   126,   248,    -1,   129,
      -1,   126,    -1,   297,   116,    -1,   249,   297,   116,    -1,
     249,   297,   116,   248,    -1,   248,    57,    58,    -1,   248,
      57,   269,    58,    -1,    57,    58,    -1,    57,   269,    58,
      -1,    40,   247,    41,    -1,   248,    40,   233,    41,    -1,
      40,   233,    41,    -1,   136,   250,   249,    -1,   136,   249,
      -1,   136,   250,    -1,   136,    -1,   251,    -1,   251,   250,
      -1,    49,    -1,    50,    -1,    51,    -1,   253,    -1,   250,
     254,    -1,   254,    -1,   254,   250,    -1,   250,   254,   250,
      -1,   256,    -1,    32,    -1,    30,    -1,    34,   293,    -1,
     193,   297,    -1,    35,    -1,   297,    -1,   284,   297,    -1,
     255,    -1,    86,    40,   297,    41,    -1,   257,    -1,   258,
      -1,   258,   257,    -1,    22,    -1,    24,    -1,    25,    -1,
      28,    -1,    29,    -1,    26,    -1,    27,    -1,    31,    -1,
      23,    -1,    33,    -1,    36,    -1,    37,    -1,    38,    -1,
      39,    -1,    -1,   260,   269,    -1,   261,    -1,   262,    -1,
     263,    -1,   105,    -1,   106,    -1,   294,    -1,   308,    -1,
     150,    -1,   308,    -1,   266,    42,   265,   267,   265,    -1,
     266,    42,   265,    -1,   265,   267,   265,    -1,   265,    -1,
     294,    -1,   294,    54,   268,    -1,   269,    -1,   270,    -1,
     252,    -1,   271,    -1,   302,    -1,    55,    40,   252,   242,
      41,    -1,    55,    48,    48,    48,    40,   252,   242,    41,
      -1,   272,    -1,   303,    -1,    11,    -1,    12,    -1,    40,
     269,    41,    -1,    40,   269,    41,   269,    -1,    40,   269,
     249,    41,   269,    -1,    40,   269,   129,    41,   269,    -1,
      40,   269,   126,    41,   269,    -1,    40,   269,   249,   129,
      41,   269,    -1,    40,   269,   249,   126,    41,   269,    -1,
     129,   269,    -1,   126,   269,    -1,   136,   269,    -1,    13,
      -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,    18,
      -1,    19,    -1,    20,    -1,   269,   133,   269,    -1,   269,
     132,   269,    -1,   269,   136,   269,    -1,   269,   135,   269,
      -1,   269,   134,   269,    -1,   269,   129,   269,    -1,   269,
     127,   269,    -1,   269,   128,   269,    -1,   269,   131,   269,
      -1,   269,   130,   269,    -1,   269,   126,   269,    -1,   269,
     125,   269,    -1,   269,   109,   269,    -1,   269,   110,   269,
      -1,   269,   108,   269,    -1,   269,   107,   269,    -1,   269,
     112,   269,    76,   269,    -1,   132,   269,    -1,   133,   269,
      -1,   138,   269,    -1,   137,   269,    -1,   252,    40,    -1,
      48,    48,    48,    -1,   273,    -1,   308,    -1,   276,    -1,
      -1,    76,   277,   278,    -1,   308,    -1,   279,    -1,   278,
      42,   279,    -1,    -1,   285,   280,   297,   274,    -1,    -1,
     285,   282,   281,   285,   297,   274,    -1,    74,    -1,    73,
      -1,    75,    -1,    71,    -1,    72,    -1,    71,    48,    48,
      48,    -1,    72,    48,    48,    48,    -1,   283,    -1,    52,
      -1,    53,    -1,    78,    -1,   308,    -1,    89,    -1,    90,
      -1,    90,    89,    -1,    89,    90,    -1,    80,    40,   233,
      41,    -1,    88,    -1,   286,    -1,    88,   286,    -1,    88,
      40,   269,    41,    -1,   250,    -1,   287,    -1,   250,   287,
      -1,   308,    -1,   288,   290,    43,    -1,   288,   290,    46,
      -1,    40,   233,    41,    43,    -1,    40,   233,    41,    46,
      -1,    54,   259,    43,    -1,   287,    54,   261,    43,    -1,
      76,   291,    -1,   308,    -1,   292,    -1,   291,    42,   292,
      -1,   292,    48,    48,    48,    -1,   291,    42,   292,    48,
      48,    48,    -1,   297,    40,    -1,   297,    46,    -1,   103,
     237,   104,    -1,   308,    -1,     3,    -1,    89,    -1,    90,
      -1,   294,    -1,   261,    -1,   302,    -1,   295,    -1,   308,
      -1,   299,   298,    -1,   115,   140,   299,   298,    -1,   299,
      -1,   115,   140,   299,    -1,   119,   293,    -1,   115,   140,
     119,    -1,   140,   299,   298,    -1,   140,   299,    -1,   140,
     119,    -1,   117,   299,    -1,   294,   293,    -1,   294,   301,
      -1,   115,   140,   294,   301,    -1,   294,    -1,   115,   140,
     294,    -1,   119,    -1,   115,   140,   119,    -1,   140,   294,
     301,    -1,   140,   294,    -1,   140,   119,    -1,   117,   294,
      -1,   302,     6,    -1,     6,    -1,   303,     7,    -1,     7,
      -1,   302,    -1,    46,    -1,     4,    -1,    40,   306,    41,
      -1,   308,    -1,   295,    54,   307,    -1,   295,    54,   307,
      42,   306,    -1,   295,    -1,   295,    42,   306,    -1,   295,
      54,   153,    -1,   295,    54,   153,    42,   306,    -1,   302,
      -1,   271,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1466,  1466,  1478,  1482,  1485,  1488,  1491,  1494,  1499,
    1504,  1509,  1510,  1511,  1512,  1513,  1523,  1539,  1549,  1550,
    1551,  1552,  1553,  1554,  1555,  1556,  1557,  1558,  1559,  1560,
    1561,  1562,  1563,  1564,  1565,  1566,  1567,  1568,  1569,  1576,
    1576,  1658,  1668,  1679,  1700,  1722,  1733,  1742,  1761,  1767,
    1773,  1778,  1785,  1792,  1796,  1809,  1818,  1833,  1846,  1846,
    1902,  1903,  1910,  1929,  1960,  1964,  1974,  1979,  1997,  2040,
    2046,  2059,  2065,  2091,  2097,  2104,  2105,  2108,  2109,  2116,
    2162,  2208,  2219,  2222,  2249,  2255,  2261,  2267,  2275,  2281,
    2287,  2293,  2301,  2302,  2303,  2306,  2311,  2321,  2357,  2358,
    2393,  2410,  2418,  2431,  2456,  2462,  2466,  2469,  2480,  2485,
    2498,  2510,  2800,  2810,  2817,  2818,  2822,  2822,  2847,  2853,
    2864,  2880,  2940,  2998,  3002,  3025,  3029,  3040,  3047,  3054,
    3061,  3070,  3071,  3072,  3073,  3074,  3075,  3076,  3087,  3092,
    3097,  3104,  3110,  3115,  3118,  3118,  3131,  3134,  3137,  3146,
    3149,  3156,  3178,  3207,  3305,  3357,  3358,  3359,  3360,  3361,
    3362,  3367,  3367,  3614,  3614,  3759,  3760,  3772,  3790,  3790,
    4049,  4055,  4061,  4064,  4067,  4070,  4073,  4076,  4079,  4084,
    4120,  4124,  4127,  4130,  4135,  4139,  4144,  4154,  4185,  4185,
    4214,  4214,  4236,  4263,  4280,  4285,  4280,  4293,  4294,  4295,
    4295,  4311,  4312,  4329,  4330,  4331,  4332,  4333,  4334,  4335,
    4336,  4337,  4338,  4339,  4340,  4341,  4342,  4343,  4344,  4345,
    4354,  4382,  4409,  4440,  4455,  4472,  4490,  4509,  4528,  4535,
    4542,  4549,  4557,  4565,  4568,  4572,  4575,  4576,  4577,  4578,
    4579,  4580,  4581,  4582,  4585,  4592,  4599,  4608,  4617,  4626,
    4638,  4641,  4644,  4645,  4646,  4647,  4649,  4657,  4669,  4670,
    4671,  4672,  4673,  4674,  4675,  4676,  4677,  4678,  4679,  4680,
    4681,  4682,  4683,  4684,  4685,  4686,  4687,  4688,  4695,  4706,
    4710,  4713,  4717,  4721,  4731,  4739,  4747,  4760,  4764,  4767,
    4771,  4775,  4803,  4811,  4823,  4838,  4848,  4857,  4868,  4872,
    4876,  4883,  4900,  4917,  4925,  4933,  4942,  4951,  4955,  4964,
    4975,  4986,  4998,  5008,  5022,  5030,  5039,  5048,  5052,  5061,
    5072,  5083,  5095,  5105,  5115,  5126,  5139,  5146,  5154,  5170,
    5178,  5189,  5200,  5211,  5230,  5238,  5255,  5263,  5270,  5277,
    5288,  5299,  5310,  5330,  5351,  5357,  5363,  5370,  5377,  5386,
    5395,  5398,  5407,  5416,  5423,  5430,  5437,  5447,  5458,  5469,
    5480,  5487,  5494,  5497,  5514,  5524,  5531,  5537,  5542,  5548,
    5552,  5558,  5559,  5560,  5566,  5572,  5576,  5577,  5581,  5588,
    5591,  5592,  5593,  5594,  5595,  5597,  5600,  5603,  5608,  5619,
    5644,  5647,  5701,  5705,  5709,  5713,  5717,  5721,  5725,  5729,
    5733,  5737,  5741,  5745,  5749,  5753,  5759,  5759,  5773,  5789,
    5792,  5798,  5811,  5825,  5826,  5829,  5830,  5834,  5840,  5843,
    5847,  5852,  5860,  5872,  5887,  5888,  5907,  5908,  5912,  5917,
    5922,  5923,  5928,  5941,  5956,  5963,  5980,  5987,  5994,  6001,
    6009,  6017,  6021,  6025,  6031,  6032,  6033,  6034,  6035,  6036,
    6037,  6038,  6041,  6045,  6049,  6053,  6057,  6061,  6065,  6069,
    6073,  6077,  6081,  6085,  6089,  6093,  6107,  6111,  6115,  6121,
    6125,  6129,  6133,  6137,  6153,  6158,  6161,  6166,  6171,  6171,
    6172,  6175,  6192,  6201,  6201,  6219,  6219,  6237,  6238,  6239,
    6243,  6247,  6251,  6255,  6261,  6264,  6268,  6274,  6275,  6278,
    6281,  6284,  6287,  6292,  6297,  6302,  6307,  6312,  6319,  6325,
    6329,  6333,  6341,  6349,  6357,  6366,  6375,  6382,  6391,  6392,
    6395,  6396,  6397,  6398,  6401,  6413,  6419,  6425,  6429,  6430,
    6431,  6434,  6435,  6436,  6439,  6440,  6443,  6448,  6452,  6455,
    6458,  6461,  6466,  6470,  6473,  6480,  6486,  6495,  6500,  6504,
    6507,  6510,  6513,  6518,  6522,  6525,  6528,  6534,  6539,  6542,
    6554,  6557,  6560,  6564,  6569,  6582,  6586,  6591,  6597,  6601,
    6606,  6610,  6617,  6620,  6625
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "HBLOCK", "POUND", "STRING",
  "WSTRING", "INCLUDE", "IMPORT", "INSERT", "CHARCONST", "WCHARCONST",
  "NUM_INT", "NUM_FLOAT", "NUM_UNSIGNED", "NUM_LONG", "NUM_ULONG",
  "NUM_LONGLONG", "NUM_ULONGLONG", "NUM_BOOL", "TYPEDEF", "TYPE_INT",
  "TYPE_UNSIGNED", "TYPE_SHORT", "TYPE_LONG", "TYPE_FLOAT", "TYPE_DOUBLE",
  "TYPE_CHAR", "TYPE_WCHAR", "TYPE_VOID", "TYPE_SIGNED", "TYPE_BOOL",
  "TYPE_COMPLEX", "TYPE_TYPEDEF", "TYPE_RAW", "TYPE_NON_ISO_INT8",
  "TYPE_NON_ISO_INT16", "TYPE_NON_ISO_INT32", "TYPE_NON_ISO_INT64",
  "LPAREN", "RPAREN", "COMMA", "SEMI", "EXTERN", "INIT", "LBRACE",
  "RBRACE", "PERIOD", "CONST_QUAL", "VOLATILE", "REGISTER", "STRUCT",
  "UNION", "EQUAL", "SIZEOF", "MODULE", "LBRACKET", "RBRACKET",
  "BEGINFILE", "ENDOFFILE", "ILLEGAL", "CONSTANT", "NAME", "RENAME",
  "NAMEWARN", "EXTEND", "PRAGMA", "FEATURE", "VARARGS", "ENUM", "CLASS",
  "TYPENAME", "PRIVATE", "PUBLIC", "PROTECTED", "COLON", "STATIC",
  "VIRTUAL", "FRIEND", "THROW", "CATCH", "EXPLICIT", "STATIC_ASSERT",
  "CONSTEXPR", "THREAD_LOCAL", "DECLTYPE", "AUTO", "NOEXCEPT", "OVERRIDE",
  "FINAL", "USING", "NAMESPACE", "NATIVE", "INLINE", "TYPEMAP", "EXCEPT",
  "ECHO", "APPLY", "CLEAR", "SWIGTEMPLATE", "FRAGMENT", "WARN", "LESSTHAN",
  "GREATERTHAN", "DELETE_KW", "DEFAULT", "LESSTHANOREQUALTO",
  "GREATERTHANOREQUALTO", "EQUALTO", "NOTEQUALTO", "ARROW", "QUESTIONMARK",
  "TYPES", "PARMS", "NONID", "DSTAR", "DCNOT", "TEMPLATE", "OPERATOR",
  "CONVERSIONOPERATOR", "PARSETYPE", "PARSEPARM", "PARSEPARMS", "CAST",
  "LOR", "LAND", "OR", "XOR", "AND", "RSHIFT", "LSHIFT", "MINUS", "PLUS",
  "MODULO", "SLASH", "STAR", "LNOT", "NOT", "UMINUS", "DCOLON", "$accept",
  "program", "interface", "declaration", "swig_directive",
  "extend_directive", "$@1", "apply_directive", "clear_directive",
  "constant_directive", "echo_directive", "except_directive", "stringtype",
  "fname", "fragment_directive", "include_directive", "$@2", "includetype",
  "inline_directive", "insert_directive", "module_directive",
  "name_directive", "native_directive", "pragma_directive", "pragma_arg",
  "pragma_lang", "rename_directive", "rename_namewarn",
  "feature_directive", "stringbracesemi", "featattr", "varargs_directive",
  "varargs_parms", "typemap_directive", "typemap_type", "tm_list",
  "tm_tail", "typemap_parm", "types_directive", "template_directive",
  "warn_directive", "c_declaration", "$@3", "c_decl", "c_decl_tail",
  "initializer", "cpp_alternate_rettype", "cpp_lambda_decl",
  "lambda_introducer", "lambda_body", "lambda_tail", "$@4", "c_enum_key",
  "c_enum_inherit", "c_enum_forward_decl", "c_enum_decl",
  "c_constructor_decl", "cpp_declaration", "cpp_class_decl", "@5", "@6",
  "cpp_opt_declarators", "cpp_forward_class_decl", "cpp_template_decl",
  "$@7", "cpp_temp_possible", "template_parms", "templateparameters",
  "templateparameter", "templateparameterstail", "cpp_using_decl",
  "cpp_namespace_decl", "$@8", "$@9", "cpp_members", "$@10", "$@11",
  "$@12", "cpp_member", "cpp_constructor_decl", "cpp_destructor_decl",
  "cpp_conversion_operator", "cpp_catch_decl", "cpp_static_assert",
  "cpp_protection_decl", "cpp_swig_directive", "cpp_end", "cpp_vend",
  "anonymous_bitfield", "anon_bitfield_type", "extern_string",
  "storage_class", "parms", "rawparms", "ptail", "parm", "valparms",
  "rawvalparms", "valptail", "valparm", "def_args", "parameter_declarator",
  "typemap_parameter_declarator", "declarator", "notso_direct_declarator",
  "direct_declarator", "abstract_declarator", "direct_abstract_declarator",
  "pointer", "type_qualifier", "type_qualifier_raw", "type", "rawtype",
  "type_right", "decltype", "primitive_type", "primitive_type_list",
  "type_specifier", "definetype", "$@13", "default_delete",
  "deleted_definition", "explicit_default", "ename",
  "optional_constant_directive", "enumlist", "edecl", "etype", "expr",
  "valexpr", "exprnum", "exprcompound", "ellipsis", "variadic", "inherit",
  "raw_inherit", "$@14", "base_list", "base_specifier", "@15", "@16",
  "access_specifier", "templcpptype", "cpptype", "opt_virtual",
  "virt_specifier_seq", "exception_specification", "cpp_const", "ctor_end",
  "ctor_initializer", "mem_initializer_list", "mem_initializer",
  "template_decl", "identifier", "idstring", "idstringopt", "idcolon",
  "idcolontail", "idtemplate", "idcolonnt", "idcolontailnt", "string",
  "wstring", "stringbrace", "options", "kwargs", "stringnum", "empty", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   141,   142,   142,   142,   142,   142,   142,   142,   143,
     143,   144,   144,   144,   144,   144,   144,   144,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   147,
     146,   148,   149,   150,   150,   150,   151,   151,   152,   152,
     152,   152,   153,   154,   154,   155,   155,   155,   157,   156,
     158,   158,   159,   159,   160,   160,   160,   160,   161,   162,
     162,   163,   163,   164,   164,   165,   165,   166,   166,   167,
     167,   167,   168,   168,   169,   169,   169,   169,   169,   169,
     169,   169,   170,   170,   170,   171,   171,   172,   173,   173,
     174,   174,   174,   175,   176,   177,   177,   178,   178,   178,
     179,   180,   181,   182,   182,   182,   183,   182,   182,   182,
     182,   184,   184,   185,   185,   185,   185,   186,   186,   186,
     186,   187,   187,   187,   187,   187,   187,   187,   188,   188,
     188,   189,   190,   191,   192,   191,   193,   193,   193,   194,
     194,   195,   196,   196,   197,   198,   198,   198,   198,   198,
     198,   200,   199,   201,   199,   202,   202,   203,   205,   204,
     204,   204,   206,   206,   206,   206,   206,   206,   206,   207,
     208,   208,   209,   209,   210,   210,   211,   211,   213,   212,
     214,   212,   212,   215,   216,   217,   215,   215,   215,   218,
     215,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     220,   221,   221,   222,   222,   222,   222,   222,   223,   224,
     225,   225,   225,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   227,   227,   227,   228,   228,   228,
     229,   230,   230,   230,   230,   230,   230,   231,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   233,   234,
     234,   235,   235,   236,   236,   236,   237,   238,   238,   239,
     239,   240,   240,   241,   241,   241,   241,   241,   242,   242,
     242,   243,   243,   243,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   245,   245,   245,   245,
     245,   245,   245,   245,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   246,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   247,   248,   248,
     248,   248,   248,   248,   248,   249,   249,   249,   249,   250,
     250,   251,   251,   251,   252,   253,   253,   253,   253,   254,
     254,   254,   254,   254,   254,   254,   254,   254,   255,   256,
     257,   257,   258,   258,   258,   258,   258,   258,   258,   258,
     258,   258,   258,   258,   258,   258,   260,   259,   259,   261,
     261,   262,   263,   264,   264,   265,   265,   266,   266,   266,
     266,   267,   267,   268,   269,   269,   270,   270,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     270,   270,   270,   270,   271,   271,   271,   271,   271,   271,
     271,   271,   272,   272,   272,   272,   272,   272,   272,   272,
     272,   272,   272,   272,   272,   272,   272,   272,   272,   272,
     272,   272,   272,   272,   273,   274,   274,   275,   277,   276,
     276,   278,   278,   280,   279,   281,   279,   282,   282,   282,
     283,   283,   283,   283,   284,   284,   284,   285,   285,   286,
     286,   286,   286,   287,   287,   287,   287,   287,   288,   288,
     288,   288,   289,   289,   289,   289,   289,   289,   290,   290,
     291,   291,   291,   291,   292,   292,   293,   293,   294,   294,
     294,   295,   295,   295,   296,   296,   297,   297,   297,   297,
     297,   297,   298,   298,   298,   298,   299,   300,   300,   300,
     300,   300,   300,   301,   301,   301,   301,   302,   302,   303,
     303,   304,   304,   304,   305,   305,   306,   306,   306,   306,
     306,   306,   307,   307,   308
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     3,     2,     5,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       7,     5,     3,     5,     5,     3,     2,     2,     5,     2,
       5,     2,     4,     1,     1,     7,     7,     5,     0,     7,
       1,     1,     2,     2,     1,     5,     5,     5,     3,     4,
       3,     7,     8,     5,     3,     1,     1,     3,     1,     4,
       7,     6,     1,     1,     7,     9,     8,    10,     5,     7,
       6,     8,     1,     1,     5,     4,     5,     7,     1,     3,
       6,     6,     8,     1,     2,     3,     1,     2,     3,     6,
       5,     9,     2,     1,     1,     1,     0,     6,     1,     3,
       8,     5,     7,     1,     4,     1,     1,     1,     2,     2,
       3,     1,     1,     1,     2,     1,     1,     1,    11,    13,
       7,     1,     1,     1,     0,     3,     1,     2,     2,     2,
       1,     5,     8,    10,     6,     1,     1,     1,     1,     1,
       1,     0,     9,     0,     8,     1,     3,     4,     0,     6,
       3,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     3,     1,     3,     4,     0,     6,
       0,     5,     5,     2,     0,     0,     7,     1,     1,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       6,     6,     7,     8,     8,     8,     9,     7,     5,     2,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     4,     2,     2,     4,     2,
       5,     1,     1,     1,     2,     1,     1,     2,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     1,     2,     2,     2,     2,     1,     1,     2,
       1,     3,     1,     2,     7,     3,     1,     2,     1,     3,
       1,     1,     1,     2,     5,     2,     2,     1,     2,     2,
       1,     1,     1,     1,     2,     3,     3,     1,     2,     2,
       3,     4,     5,     4,     5,     6,     6,     4,     5,     5,
       6,     7,     8,     8,     7,     7,     1,     2,     3,     4,
       5,     3,     4,     4,     1,     2,     4,     4,     4,     5,
       3,     4,     4,     5,     1,     2,     2,     2,     3,     3,
       1,     2,     2,     1,     1,     2,     3,     4,     3,     4,
       2,     3,     3,     4,     3,     3,     2,     2,     1,     1,
       2,     1,     1,     1,     1,     2,     1,     2,     3,     1,
       1,     1,     2,     2,     1,     1,     2,     1,     4,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     5,     3,     3,
       1,     1,     3,     1,     1,     1,     1,     1,     5,     8,
       1,     1,     1,     1,     3,     4,     5,     5,     5,     6,
       6,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     5,     2,
       2,     2,     2,     2,     3,     1,     1,     1,     0,     3,
       1,     1,     3,     0,     4,     0,     6,     1,     1,     1,
       1,     1,     4,     4,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     4,     1,     1,     2,     4,     1,     1,
       2,     1,     3,     3,     4,     4,     3,     4,     2,     1,
       1,     3,     4,     6,     2,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     4,     1,     3,
       2,     3,     3,     2,     2,     2,     2,     2,     4,     1,
       3,     1,     3,     3,     2,     2,     2,     2,     1,     2,
       1,     1,     1,     1,     3,     1,     3,     5,     1,     3,
       3,     5,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     574,     0,     0,     0,     0,     0,    10,     4,   528,   392,
     400,   393,   394,   397,   398,   395,   396,   381,   399,   380,
     401,   574,   384,   402,   403,   404,   405,     0,   371,   372,
     373,   495,   496,   146,   490,   491,     0,   529,   530,     0,
       0,   574,     0,     0,     0,   369,   574,   376,   387,   379,
     389,   390,   494,     0,   574,   385,   538,     6,     0,     0,
     574,     1,    15,    64,    60,    61,     0,   263,    14,   258,
     574,     0,     0,    82,    83,   574,   574,     0,     0,   262,
     264,   265,     0,   266,   267,   272,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       9,    11,    18,    19,    20,    21,    22,    23,    24,    25,
     574,    26,    27,    28,    29,    30,    31,    32,     0,    33,
      34,    35,    36,    37,    38,    12,   113,   118,   115,   114,
      16,    13,   155,   156,   157,   158,   159,   160,   259,     0,
     277,   574,   382,   527,     0,   148,   147,     0,     0,     0,
       0,     0,   540,   383,     3,   375,   370,   574,     0,   406,
       0,     0,   574,   354,   353,   368,     0,   300,   283,   574,
     307,   574,   350,   344,   334,   297,   377,   391,   386,   546,
       0,     0,   536,     5,     8,     0,   278,   574,   280,    17,
       0,   558,   275,     0,   257,     0,     0,   565,     0,     0,
     374,   574,     0,     0,     0,     0,    78,     0,   574,   270,
     274,   574,   268,   271,   269,   276,   273,     0,     0,   190,
     574,     0,     0,    62,    63,     0,     0,    51,    49,    46,
      47,   574,     0,   574,     0,   574,   574,     0,   112,   574,
     574,     0,     0,     0,     0,     0,     0,     0,   334,   261,
     260,     0,   574,     0,   574,   560,   432,   433,   444,   445,
     446,   447,   448,   449,   450,   451,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   291,     0,   286,   574,   425,
     374,     0,   424,   426,   430,   427,   431,   288,   285,     0,
       0,     0,   541,   539,     0,   378,   574,   354,   353,     0,
       0,   344,   385,     0,   295,   411,   412,   293,     0,   408,
     409,   410,   360,     0,   424,   296,     0,   574,     0,     0,
     309,   352,   326,     0,   308,   351,   366,   367,   335,   298,
     574,     0,   299,   574,     0,     0,   347,   346,   304,   345,
     326,   355,   545,   544,   543,     0,     0,   279,   282,   532,
     531,     0,   533,     0,   557,   116,   568,     0,    68,    45,
       0,   574,   406,    70,     0,     0,     0,    74,     0,     0,
       0,    98,     0,     0,   186,   119,   574,     0,   188,     0,
       0,   103,     0,     0,     0,   107,   301,   302,   303,    42,
       0,   104,   106,   534,     0,   535,    54,     0,    53,     0,
       0,   179,   574,   183,   494,   181,   170,     0,     0,     0,
       0,   531,     0,     0,     0,     0,     0,     0,   326,     0,
       0,   334,   574,   574,   414,   574,   574,   478,     0,   477,
     386,   480,     0,     0,     0,   442,   441,   469,   470,   443,
     472,   471,   526,     0,   287,   290,   473,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   559,   492,   493,   388,   537,     0,
     354,   353,   344,   385,     0,   334,     0,   364,   362,   347,
     346,     0,   334,   355,     0,     0,   407,   361,   574,   344,
     385,     0,   327,   574,     0,     0,   365,     0,   340,     0,
       0,   358,     0,     0,     0,   306,   349,     0,   305,   348,
     356,     0,     0,     0,   310,   542,     7,   574,     0,   171,
     574,     0,     0,   564,     0,     0,    69,    39,    77,     0,
       0,     0,     0,     0,     0,     0,   187,     0,     0,   574,
     574,     0,     0,   108,     0,   574,     0,     0,     0,     0,
       0,   168,     0,   180,   185,    58,     0,     0,     0,     0,
      79,     0,     0,     0,     0,     0,   150,     0,   385,     0,
     504,   499,   500,     0,   127,   574,   505,   574,   574,   163,
     167,     0,   434,     0,     0,   368,     0,   574,     0,   574,
     467,   466,   464,   465,     0,   463,   462,   458,   459,   457,
     461,   460,   453,   452,   456,   455,   454,     0,     0,   355,
     338,   337,   336,   356,     0,   317,     0,     0,     0,   326,
     328,   355,     0,     0,   331,     0,     0,   342,   341,   363,
     359,     0,     0,     0,     0,     0,     0,   311,   357,     0,
       0,     0,   313,   281,    66,    67,    65,     0,   569,   570,
     573,   572,   566,    44,    43,     0,    76,    73,    75,   563,
      93,   562,     0,    88,   574,   561,    92,     0,   572,     0,
       0,    99,   574,   228,   191,   192,     0,   258,     0,     0,
      50,    48,   574,    41,   105,     0,   551,   549,     0,    57,
       0,     0,   110,     0,   574,   574,   574,   574,     0,     0,
     133,   132,   574,   135,   574,   137,   131,   136,   141,     0,
     149,   151,   574,   574,   574,     0,   506,   502,   501,   126,
       0,   123,   125,   121,   128,   574,   129,   497,   479,   481,
     483,   498,     0,   161,   435,     0,     0,   368,   367,     0,
       0,     0,     0,     0,   289,     0,   574,   339,   294,   343,
     329,     0,   319,   333,   332,   318,   314,     0,     0,     0,
       0,     0,   312,     0,     0,     0,   117,     0,     0,   199,
     219,     0,     0,     0,     0,   264,     0,     0,   241,   242,
     234,   243,   217,   197,   239,   235,   233,   236,   237,   238,
     240,   218,   214,   215,   201,   209,   208,   212,   211,     0,
       0,   202,   203,   207,   213,   204,   205,   206,   216,     0,
     277,   574,   508,   509,     0,   511,     0,     0,     0,     0,
      90,   574,     0,   189,   257,     0,   574,   101,     0,   100,
       0,     0,     0,     0,   547,   574,     0,    52,     0,   258,
       0,   172,   173,   177,   176,   169,   174,   178,   175,     0,
     184,     0,     0,    81,   134,     0,   574,   142,     0,   415,
     420,     0,   416,   574,   406,   509,   574,   154,     0,     0,
     574,   130,   574,   488,   487,   489,     0,   485,     0,     0,
     438,   437,   436,     0,     0,   428,     0,   468,   284,   330,
     316,   315,     0,     0,     0,   320,     0,     0,   571,   567,
       0,   194,   231,   230,   232,     0,   229,     0,    40,   193,
     381,   380,   574,   384,     0,     0,     0,   379,   385,     0,
     510,    84,   572,    95,    89,   574,     0,     0,    97,     0,
      71,     0,   109,   552,   550,   556,   555,   554,     0,    55,
      56,     0,   574,     0,    59,    80,   122,     0,   144,   143,
     140,   574,   421,   574,     0,     0,     0,     0,     0,     0,
     519,   503,   507,     0,   482,   574,   574,     0,     0,   440,
     439,   574,   321,     0,     0,   325,   324,   200,     0,     0,
     574,   382,     0,     0,   574,   210,     0,    96,     0,    91,
     574,    86,    72,   102,   548,   553,     0,   120,     0,   574,
       0,   419,     0,   418,   152,   574,     0,   516,     0,   518,
     520,     0,   512,   513,   124,     0,   475,   484,   476,     0,
     165,   164,   574,     0,     0,   323,   322,     0,   574,     0,
     574,     0,     0,     0,     0,     0,    94,    85,     0,   111,
     168,     0,   145,   422,   423,   574,     0,   514,   515,   517,
       0,     0,   524,   525,     0,   574,     0,   162,   429,   195,
       0,   574,     0,   574,   574,   574,     0,   250,   574,    87,
       0,     0,   417,   153,   521,     0,   474,   486,   166,     0,
     574,   221,     0,   574,     0,     0,     0,   574,   220,     0,
     138,     0,   522,   196,   222,     0,   244,   246,     0,   227,
     574,   574,   574,     0,     0,     0,   247,   249,   406,     0,
     225,   224,   223,   574,   139,   523,     0,   245,   226,   248
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,   100,   101,   102,   655,   778,   779,   780,
     781,   107,   396,   397,   782,   783,   696,   110,   111,   784,
     113,   785,   115,   786,   657,   205,   787,   118,   788,   663,
     532,   789,   370,   790,   380,   234,   391,   235,   791,   792,
     793,   794,   520,   126,   723,   573,   704,   127,   709,   858,
     950,  1000,    42,   565,   128,   129,   130,   131,   795,   879,
     732,  1021,   796,   797,   694,   845,   400,   401,   402,   553,
     798,   136,   539,   376,   799,   978,  1079,   900,   800,   801,
     802,   803,   804,   805,   806,   807,  1081,  1094,   808,   915,
     138,   809,   299,   186,   347,   187,   276,   277,   444,   278,
     574,   168,   385,   169,   320,   170,   171,   172,   247,    44,
      45,   279,   200,    47,    48,    49,    50,    51,   307,   308,
     349,   310,   311,   422,   860,   861,   951,  1043,   281,   314,
     283,   284,  1016,  1017,   428,   429,   578,   728,   729,   876,
     966,   877,    52,    53,   730,   576,   813,  1095,   867,   959,
    1009,  1010,   179,    54,   356,   394,    55,   182,    56,   688,
     834,   285,   286,   666,   196,   357,   652,   188
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -892
static const yytype_int16 yypact[] =
{
     726,  4196,  4268,   268,    85,  3686,  -892,  -892,  -892,  -892,
    -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,
    -892,   -13,  -892,  -892,  -892,  -892,  -892,    73,  -892,  -892,
    -892,  -892,  -892,   367,    87,   174,   198,  -892,  -892,   163,
     165,   -13,   140,   264,  4913,   812,  1346,   812,  -892,  -892,
    -892,  1586,  -892,   140,   -13,  -892,   111,  -892,   274,   303,
    4630,  -892,   294,  -892,  -892,  -892,   434,  -892,  -892,    49,
     450,  4340,   459,  -892,  -892,   450,   486,   495,   507,   517,
    -892,  -892,   523,   376,   217,   209,    83,   559,   527,    48,
     543,   508,   349,  4701,  4701,   557,   572,   632,   593,   565,
    -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,
     450,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  1590,  -892,
    -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,
    -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,    28,  4772,
    -892,  2214,  -892,  -892,   604,  -892,  -892,   606,   611,   140,
     221,   420,  -892,  -892,  -892,   812,  -892,  3310,   621,    19,
    2342,  3110,    50,   327,  1113,    30,   140,  -892,  -892,   380,
     351,   380,   392,  1641,   529,  -892,  -892,  -892,  -892,  -892,
     116,   261,  -892,  -892,  -892,   634,  -892,   642,  -892,  -892,
     331,  -892,  -892,   420,   197,   331,   331,  -892,   655,  1659,
    -892,    52,   992,   140,   116,   116,  -892,   331,  4558,  -892,
    -892,  4630,  -892,  -892,  -892,  -892,  -892,   140,   138,  -892,
     157,   665,   116,  -892,  -892,   331,   116,  -892,  -892,  -892,
     711,  4630,   675,   243,   680,   687,   331,   632,   711,  4630,
    4630,   140,   632,   259,   177,   504,   331,   183,   645,  -892,
    -892,  1659,   140,  1717,   735,  -892,  -892,  -892,  -892,  -892,
    -892,  -892,  -892,  -892,  -892,  -892,  3110,   441,  3110,  3110,
    3110,  3110,  3110,  3110,  3110,  -892,   661,  -892,   725,   742,
    1283,  2438,    14,  -892,  -892,   711,   777,  -892,  -892,   741,
     750,   773,  -892,   111,   713,  -892,  3427,    89,    89,   797,
     811,  1047,   712,   819,  -892,  -892,  -892,   802,  3110,  -892,
    -892,  -892,  -892,  4344,  -892,  2438,   828,  3427,   826,   140,
     410,   392,  -892,   834,   410,   392,  -892,   747,  -892,  -892,
    4630,  2470,  -892,  4630,  2598,   841,  1931,  1942,   410,   392,
     775,  1854,  -892,  -892,   111,   849,  4630,  -892,  -892,  -892,
    -892,   854,   711,   140,  -892,  -892,   329,   857,  -892,  -892,
     379,   380,   583,  -892,   858,   861,   867,   862,   745,   869,
     874,  -892,   878,   881,  -892,  -892,  -892,   140,  -892,   885,
     890,  -892,   892,   893,  4701,  -892,  -892,  -892,  -892,  -892,
    4701,  -892,  -892,  -892,   897,  -892,  -892,   781,   204,   899,
     837,  -892,   895,  -892,    78,  -892,  -892,   190,    55,    55,
      55,   338,   831,   904,   395,   905,  1953,  2296,   840,  1854,
     851,    43,   887,   372,  -892,  3499,  1224,  -892,   915,  -892,
     382,  -892,  2181,  4843,   919,  3059,  2188,  -892,  -892,  -892,
    -892,  -892,  -892,  2214,  -892,  -892,  -892,  3110,  3110,  3110,
    3110,  3110,  3110,  3110,  3110,  3110,  3110,  3110,  3110,  3110,
    3110,  3110,  3110,  3110,  -892,  -892,  -892,  -892,  -892,   420,
     464,   464,   440,   852,   471,  -892,   539,  -892,  -892,   464,
     464,   610,   859,    55,    55,  3110,  2438,  -892,  4630,  1053,
       7,   925,  -892,  4630,  2726,   936,  -892,   944,  -892,  4705,
     945,  -892,  4792,   939,   941,   410,   392,   943,   410,   392,
    1778,   946,   948,  2424,   410,  -892,  -892,   642,   383,  -892,
    -892,   331,  1495,  -892,   950,   954,  -892,  -892,  -892,   482,
    1514,  1719,   960,  4630,  1659,   961,  -892,  3788,   963,  -892,
     322,  4701,   476,   970,   964,   687,   492,   975,   331,  4630,
     787,   929,  4630,  -892,  -892,  -892,    55,   387,   186,    16,
    -892,  1888,  4983,   978,  4913,   542,  -892,   980,   831,   997,
     167,   951,   953,    65,  -892,   603,  -892,   380,   962,  -892,
    -892,   998,  3110,  2854,  2982,  3238,    32,  1346,   999,   725,
    1233,  1233,   896,   896,  2311,  2692,  3059,  2701,  3187,  2188,
    1060,  1060,   752,   752,  -892,  -892,  -892,   140,   859,  -892,
    -892,  -892,  -892,   464,   709,   351,  4917,  1005,   801,   859,
    -892,   186,   186,  1007,  -892,  4929,   186,  -892,  -892,  -892,
    -892,   186,  1006,  1009,  1010,  1012,  2552,   410,   392,  1014,
    1015,  1016,   410,  -892,  -892,  -892,   711,  3890,  -892,  1023,
    -892,   204,  1025,  -892,  -892,  2086,  -892,  -892,   711,  -892,
    -892,  -892,  1029,  -892,  1070,   711,  -892,  1020,    68,   814,
    1514,  -892,  1070,  -892,  -892,  -892,  3992,    72,  4843,   786,
    -892,  -892,  4630,  -892,  -892,   930,  -892,   256,   973,  -892,
    1036,  1031,  -892,   140,  1349,   895,  -892,  1070,    61,   186,
    -892,  -892,   -13,  -892,  1224,  -892,  -892,  -892,  -892,   496,
    -892,  -892,  1024,  1985,  4630,  3110,  -892,  -892,  -892,  -892,
    1659,  -892,  -892,  -892,  -892,   380,  -892,  -892,  1043,  -892,
     829,  -892,  2086,  -892,  2438,  3110,  3110,  3238,  3569,  3110,
    1050,  1051,  1055,  1054,  -892,  3110,   380,  -892,  -892,  -892,
    -892,   836,   410,  -892,  -892,   410,   410,   186,   186,  1059,
    1061,  1063,   410,   186,  1066,  1074,  -892,   331,   331,  -892,
    -892,  1062,  1042,  1049,  1052,   995,  1094,   116,  -892,  -892,
    -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,
    -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  1088,
    2086,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  4411,
    1091,  4630,   833,  -892,   787,  -892,  1495,   484,   331,  1058,
    -892,  1070,  1097,  -892,   711,  1659,   134,  -892,  4701,  -892,
    1100,   267,   116,   320,  -892,  2214,   285,  -892,  1095,    49,
     724,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  -892,  4483,
    -892,  4094,  1109,  -892,  -892,    65,  4630,  -892,   581,  -892,
     116,   216,  -892,  4630,   583,  1101,  1078,  -892,  1122,  2803,
    1224,  -892,   962,  -892,  -892,  -892,   140,  -892,  1120,  2086,
    2438,  2438,  2438,  3110,  3110,  -892,  4843,  2566,  -892,  -892,
     410,   410,   186,  1126,  1127,   410,   186,   186,  -892,  -892,
    2086,  -892,  -892,  -892,  -892,   116,  -892,  1137,  -892,  -892,
    1102,  1105,   -13,  1108,  4843,  1112,  1836,  1114,   292,  1148,
    -892,  -892,   711,  1155,  -892,  1070,   791,   787,  -892,  1157,
    -892,  1158,  -892,  -892,   256,  -892,  -892,   256,  1104,  -892,
    -892,   116,  4630,  1659,  -892,  -892,  -892,  1163,  -892,  -892,
    -892,  1024,  1151,  1024,  1368,  1165,  1169,   583,   140,   597,
    -892,  -892,  -892,    65,  -892,  1166,   962,  1558,  1168,  2438,
    2438,  1346,   410,   186,   186,   410,   410,  -892,  2086,  1176,
    4630,  1145,   145,  3110,  3499,  -892,  1179,  -892,  1180,  -892,
    1070,  -892,  -892,  -892,  -892,  -892,  1182,  -892,  1123,  1070,
    1183,  -892,  3110,   116,  -892,  1224,   627,  -892,  1188,  1191,
    1186,   560,  -892,  -892,  -892,  1187,  -892,  -892,  -892,   140,
    -892,  -892,  1224,  1558,  1196,   410,   410,  1192,  4630,  1197,
    4630,  1200,  1203,    10,  2931,  1206,  -892,  -892,  1205,  -892,
    -892,     5,  -892,  -892,  2438,  1024,    65,  -892,  -892,  -892,
     140,  1202,  -892,  -892,  1204,  1166,    65,  -892,  -892,  -892,
    1212,  1070,  1213,  4630,  4630,  4630,  1216,  -892,  1985,  -892,
    4843,   581,  -892,  -892,  1209,  1210,  -892,  -892,  -892,  2086,
    1070,  -892,   764,  1070,  1220,  1225,  1227,  4630,  -892,  1223,
    -892,  1231,  -892,  -892,  -892,   766,  -892,  -892,   583,  -892,
    1070,  1070,  1070,  1240,   581,  1236,  -892,  -892,   583,  1229,
    -892,  -892,  -892,  1070,  -892,  -892,  1244,  -892,  -892,  -892
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -892,  -892,  -344,  -892,  -892,  -892,  -892,    24,    33,     6,
      54,  -892,   767,  -892,    67,    77,  -892,  -892,  -892,    79,
    -892,    88,  -892,    93,  -892,  -892,    95,  -892,    98,  -530,
    -633,   104,  -892,   107,  -892,  -353,   743,   -68,   109,   121,
     123,   125,  -892,   596,  -812,  -686,  -892,  -892,  -892,  -844,
    -701,  -892,  -132,  -892,  -892,  -892,  -892,  -892,    34,  -892,
    -892,   269,    39,    58,  -892,  -892,   352,  -892,   739,   600,
     126,  -892,  -892,  -892,  -685,  -892,  -892,  -892,  -892,   605,
    -892,   607,   128,   614,  -892,  -892,  -892,  -474,  -892,  -892,
    -892,     9,   -48,  -892,   780,     1,   463,  -892,   720,   868,
     -19,  -579,  -892,   339,  1132,  -196,   -96,   850,    -1,    44,
    -892,   -70,     8,   -31,   748,  -532,  1264,  -892,  -346,  -892,
    -144,  -892,  -892,  -892,  -891,  -892,   313,  -892,  1084,  -104,
    -489,  -892,  -892,   262,   888,  -892,  -892,  -892,   452,  -892,
    -892,  -892,  -223,    -3,   361,   758,  -407,   181,   265,  -892,
    -892,   282,   -15,   257,  -119,  -892,   966,  -206,  -146,  -892,
    -190,  1034,  -892,   656,   -11,  -203,  -503,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -575
static const yytype_int16 yytable[] =
{
       6,   199,    43,    58,   293,   140,   142,   252,   742,    46,
      46,   105,   185,   155,   139,   309,   525,   404,   855,   577,
     692,   143,   381,   233,   233,   232,   152,   167,   669,   103,
     706,   544,   537,   650,   342,   344,   819,   282,   104,   132,
     413,   143,   650,   946,   133,   173,   175,   878,   620,   249,
    1065,   857,   223,   316,   143,   191,  -292,   620,     8,   106,
    1001,   300,  1003,   134,   203,   304,   719,   354,    46,   253,
     197,   351,   108,   739,   354,   197,   206,   358,   191,    28,
      29,    30,   109,   364,   112,    61,     8,   468,   368,   156,
     141,   176,     8,   114,   224,   360,   241,   563,   116,   242,
     117,   474,   476,   119,   853,   481,   362,   720,   721,   120,
     197,   722,   121,   250,   122,   909,  1070,   393,  -292,     8,
    -182,   144,  -533,   621,   305,   306,   123,   415,   124,   157,
     125,   135,   699,   137,   192,   147,   254,   387,   515,  1066,
     820,   287,   275,     8,    37,    38,   160,   152,   294,   280,
     329,  1014,   332,   141,  1072,   141,   301,   192,   740,   419,
     371,   741,   143,   372,   326,    46,   165,   193,   725,   175,
      39,   175,    37,    38,   162,   217,   647,   930,    37,    38,
       8,   374,  -182,   383,   963,  1030,     8,   348,   679,     8,
     353,   399,   375,   166,   968,   676,   354,  1071,    39,   295,
     300,   143,    41,   354,    39,    37,    38,   715,   162,   327,
     354,   377,   474,   476,   481,   977,    46,   414,   309,    46,
     143,   300,   148,   414,     8,   318,   414,   166,   180,    37,
      38,   335,   173,   388,  1073,   392,   395,   141,   149,    46,
     405,   403,   410,   355,  1078,  1104,     8,    46,    46,   555,
     549,   181,   424,   215,   431,    39,   571,   572,   953,    41,
     141,   167,     8,   954,     8,   191,    37,    38,   151,    59,
       8,  1031,    37,    38,  1032,    37,    38,   917,   445,   173,
     175,   165,   497,   157,   921,   500,   216,   614,   615,   939,
     987,   158,    39,  1027,   213,   472,    41,   577,    39,   214,
     160,    39,    41,   150,    46,    41,   865,   154,    60,   416,
      37,    38,   417,   923,   233,   319,   489,   183,   648,  1046,
     233,   319,   545,     8,   319,    46,   496,   650,   201,   404,
       8,   940,    37,    38,     8,   985,  1056,   191,    46,   282,
     292,    46,   524,    67,   220,   690,   184,   517,    37,    38,
      37,    38,   851,   229,    46,   191,    37,    38,    39,   410,
     614,   175,   162,   587,   305,   306,   677,   317,  -256,   163,
    1090,   521,   164,   832,    39,   318,     6,   567,    41,   165,
     343,   166,     8,   522,   160,   408,   933,   644,   409,   191,
       8,   330,  1024,   191,  1093,   165,   833,   991,     8,    79,
      80,    81,   554,  1114,    83,   920,    84,    85,   331,    37,
      38,   143,   667,   558,   189,  -413,    37,    38,  -413,   145,
      37,    38,   566,   143,   410,   580,   175,   360,  -574,   645,
     431,   586,   333,    46,   159,   158,   305,   306,   146,   936,
     617,   141,    39,     8,   275,   623,    41,   350,  -413,   334,
     493,   280,   350,   350,  -574,  -574,   161,   246,   427,   350,
     212,   366,   367,   577,   350,   319,   607,   494,    37,    38,
     575,   233,    31,    32,   190,   141,    37,    38,  -574,   379,
     296,   433,   350,   382,    37,    38,   656,     8,   191,   434,
     195,    34,    35,   350,    39,     8,    46,   160,    41,   202,
     411,    46,    39,   350,   296,   408,   162,     8,   409,   423,
      39,   330,   610,   244,    41,   165,   245,   348,   956,   680,
       6,   160,   681,   165,   360,   166,   204,   924,   331,    37,
      38,   165,   158,   710,   671,   207,   856,   140,   361,     6,
     140,    46,   857,   105,   414,   392,   139,   208,   226,   678,
     691,   227,   323,   403,   228,    39,   724,    46,   726,    41,
      46,   103,     8,   211,   898,   899,   479,   222,   167,   480,
     104,   132,   386,    37,    38,   175,   133,   175,   731,   330,
     611,    37,    38,   225,   326,   711,   173,   175,   712,   445,
     420,   106,   426,    37,    38,   134,   331,   236,   577,    39,
    1052,   209,   210,   162,   108,   219,  1053,   685,   825,  1099,
     244,   686,   237,   245,   109,   577,   112,    31,    32,    39,
     165,   948,   166,    41,   949,   114,  1110,  1111,  1112,   738,
     116,   907,   117,   239,   830,   119,    34,    35,   191,  1118,
    1012,   120,   319,  1013,   121,   341,   122,   140,    37,    38,
     330,   612,   288,   105,   289,   810,   139,   159,   123,   290,
     124,   865,   125,   135,   815,   137,   868,   331,   240,   303,
    1047,   103,   815,  1048,    39,   345,   140,   252,    41,   161,
     104,   132,   105,   569,   346,   139,   133,   854,   305,   306,
      46,   570,   571,   572,   140,   554,     6,   815,   359,   667,
     103,   106,   143,   849,   175,   134,   871,   155,   812,   104,
     132,   378,   862,   815,   108,   133,   812,   354,   859,   404,
     309,   384,    46,   389,   109,   175,   112,   888,   842,   390,
     106,   282,   810,   843,   134,   114,   326,   496,     8,   916,
     116,   812,   117,   108,   994,   119,   175,   995,   575,   330,
     747,   120,   844,   109,   121,   112,   122,   812,   233,   979,
     931,   419,  1116,   919,   114,   442,   331,   443,   123,   116,
     124,   117,   125,   135,   119,   137,    31,    32,   350,   916,
     120,   738,   446,   121,   464,   122,   530,   531,   350,   465,
     659,   659,   191,   191,     8,    34,    35,   123,   466,   124,
     810,   125,   135,   687,   137,   350,   254,  1096,   947,  1106,
    1097,   427,  1107,  1008,   467,   955,   971,   469,  1098,    46,
    1108,   815,   547,   548,    37,    38,   143,   942,   483,   827,
     660,   360,   661,   661,   989,   287,   275,   241,   477,   158,
     828,   493,   750,   280,   982,   814,   254,     1,     2,     3,
      39,   140,   478,   822,    41,   817,   818,   105,   494,   485,
     139,    28,    29,    30,    46,   812,   960,   484,   488,   664,
     175,    46,   731,   672,   491,   103,   493,   889,   852,   810,
      37,    38,   495,   165,   104,   132,   461,   462,   463,   503,
     133,   510,   516,   494,   866,   518,   697,   981,   523,   526,
     810,   662,   873,   874,   875,   106,    39,   527,   528,   134,
     162,   533,   143,   569,   575,   534,   529,   244,   108,   535,
     245,   570,   571,   572,   536,   815,   540,   165,   109,   166,
     112,   541,  1029,   542,   543,   826,  1035,   552,   546,   114,
     550,   551,   405,   403,   116,   557,   117,   556,   560,   119,
      46,   862,   167,   862,  1109,   120,   561,   859,   121,   859,
     122,   579,   562,   564,   309,  1018,   731,   588,   609,   812,
     173,   175,   123,   622,   124,   613,   125,   135,   810,   137,
    1060,  1033,  1062,   410,   626,   627,   629,   631,    46,   632,
     815,   633,    46,   653,   639,     8,   640,   654,   191,   815,
    1089,   670,   927,   447,   448,   175,   675,   673,   153,   821,
     682,   683,   174,   321,   325,  1084,  1085,  1086,   689,   178,
     693,   713,   175,   339,   350,   350,   457,   458,   459,   460,
     461,   462,   463,   363,   812,   708,    46,   714,    46,  1103,
     727,   717,   718,   812,   733,   862,   749,   743,   753,   575,
       8,   859,   218,   221,   757,  1018,     8,   758,   759,   870,
     760,   815,   763,   764,   765,   767,   575,   768,   815,   811,
     831,    46,    46,    46,   816,   350,   835,   836,   837,   810,
     815,    37,    38,   815,   248,   872,    71,   157,   934,   935,
     937,   883,   884,   317,   886,    46,   885,   305,   306,   926,
     815,   815,   815,   194,   160,   812,   988,   892,   901,   893,
     160,   894,   812,   815,   896,   291,     8,   952,   902,    28,
      29,    30,   897,   302,   812,   903,   230,   812,   904,   322,
     322,   238,   328,   905,   906,   908,    37,    38,  -198,   340,
     928,   932,    37,    38,   812,   812,   812,   321,   325,   941,
     569,   339,   945,   317,   958,   957,   925,   812,   570,   571,
     572,   323,    39,   961,   929,   248,   162,   967,    39,   365,
     160,  1038,    41,   479,   973,   974,   480,   980,  -253,   479,
    1041,  -252,   480,   373,  -255,   166,   506,   509,   983,   986,
    -251,   319,   459,   460,   461,   462,   463,   818,   997,   174,
     992,   993,    37,    38,   999,  1002,  1006,   406,   996,   412,
     322,   322,  1007,   418,  1015,  1023,  1028,   421,   153,   248,
     430,  -254,  1036,  1037,   352,  1039,  1042,  1040,    39,   352,
     352,  1049,    41,  1050,  1051,  1054,   352,  1058,  1061,  1059,
    1063,   352,  1082,  1064,   313,   315,   174,  1068,  1069,   866,
    1075,   319,  1076,  1080,  1083,   426,  1087,  1091,  1092,   352,
     952,  1100,   473,   475,   475,   990,  1101,   482,  1102,   857,
     352,   398,  1117,    28,    29,    30,   407,   352,   159,  1105,
     352,  1113,   420,   490,  1115,   492,     8,  1119,   684,   649,
     841,   695,  1057,  1005,   998,   850,   324,   643,   938,   846,
     161,   847,   322,   322,   569,   338,  1022,   322,   848,   744,
     705,   589,   570,   571,   572,   177,  1045,  1077,   581,   519,
     321,   325,   339,   157,   964,  -574,   412,  1019,   716,   506,
     509,   158,  1074,  1088,     0,   829,     0,   159,     0,   339,
     160,     0,     0,   538,     0,     0,     0,     0,     0,     8,
     432,     0,   435,   436,   437,   438,   439,   440,   441,   161,
     638,     0,  1022,   457,   458,   459,   460,   461,   462,   463,
      67,     8,    37,    38,   475,   475,   475,   324,     0,   338,
     559,     0,   322,   322,     0,   322,   157,  -574,     0,     0,
       0,   568,   486,   839,   158,     0,     0,     0,    39,     0,
     159,     0,   162,   160,     0,     0,     0,     0,   360,   163,
       0,  1004,   164,     0,     0,   499,   158,     0,   502,   165,
       0,   166,   161,     0,     0,     0,    79,    80,    81,     0,
       0,    83,   776,    84,    85,    37,    38,     0,   608,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   475,
     475,     0,     0,     0,     0,   619,     0,    37,    38,     0,
       0,    39,     0,   638,     0,   162,     0,   840,   505,   508,
       0,     0,   163,   514,     0,   164,   322,     0,     0,   322,
       0,     0,   165,    39,   166,     0,     0,   162,     0,     0,
       0,     0,     0,     0,   244,     0,   248,   245,     0,     0,
     248,   191,     0,     0,   165,     0,   166,     0,   258,   259,
     260,   261,   262,   263,   264,   265,     0,     8,   659,     0,
     191,     0,   475,   248,   322,     0,     0,   322,   707,     0,
       0,   590,   591,   592,   593,   594,   595,   596,   597,   598,
     599,   600,   601,   602,   603,   604,   605,   606,   505,   508,
       0,   514,   646,   174,   360,   352,   651,   660,     0,     0,
     661,     8,   158,   658,   665,   668,     0,     0,     0,   616,
       0,     0,     0,   746,     0,     0,     0,     0,   625,     0,
       0,     0,   352,     0,   665,     0,     0,   322,   322,     0,
       0,   698,   322,     8,     0,     0,     0,   322,   360,     0,
       0,  1020,   322,    37,    38,     0,   158,     0,     9,    10,
      11,    12,    13,    14,    15,    16,     0,    18,     0,    20,
       0,   618,    23,    24,    25,    26,     0,     0,   662,    39,
     243,     0,     0,   162,     0,     0,   248,     0,   158,     0,
     244,     0,   637,   245,     8,   642,     0,    37,    38,     0,
     165,     0,   166,     0,     0,     0,     0,     0,     0,   838,
       0,     0,     8,     0,     0,   322,   734,   596,   599,   606,
       0,     0,     0,    39,     0,     0,     0,   162,     0,    37,
      38,   317,     0,     0,   244,     0,   248,   245,     0,   335,
     618,     0,     0,   637,   165,     0,   166,     0,   160,   360,
       0,     0,     0,     0,   665,    39,     0,   158,     0,   162,
       0,   824,     0,   665,     0,     0,   244,     0,     0,   245,
       8,     0,     8,   322,   322,   191,   165,     0,   166,   322,
      37,    38,   258,   259,   260,   261,   262,   263,   264,   265,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,   751,   752,     0,    39,   425,   755,     0,
      41,     0,     0,   756,     0,   158,     0,   336,   762,     0,
     337,     0,     0,     0,    39,   918,     0,     0,   162,   319,
       0,     8,     0,   248,     0,   244,     0,     0,   245,     0,
       0,   248,     0,     0,     0,   165,     0,   166,     0,   869,
       0,   352,   352,     0,     0,     0,    37,    38,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,   317,   880,
     881,   439,     0,   882,   305,   306,   634,     0,     0,   887,
       0,   751,    39,     0,     0,   160,   162,     0,     0,     8,
       0,     0,   965,   244,     0,     0,   245,     0,   665,     0,
     922,     0,   352,   165,     0,   166,     0,     8,   322,     0,
       0,     0,   322,   322,     0,     0,     0,    37,    38,     0,
       0,     0,     0,   824,     0,     0,   984,     0,     0,     0,
       0,     0,   248,     0,   158,     0,     0,     0,     0,   890,
     891,     8,   248,    39,   414,   895,     0,    41,     0,     0,
       0,     0,   511,     0,   635,     0,     0,   636,     0,   248,
       0,     0,     0,     0,     0,     0,   319,     0,     0,     0,
     248,     0,     0,     0,  1011,    37,    38,     0,   414,     0,
       0,     0,     0,   248,     8,     0,   634,   174,     0,   322,
     322,     0,     0,    37,    38,     8,     0,     0,     0,     0,
     568,    39,     0,     0,     0,   162,     8,     0,     0,     0,
       0,   665,   244,     0,     0,   245,     0,   969,   970,    39,
       0,   317,   165,    41,   166,     0,     0,    37,    38,   504,
     512,     0,   317,   513,     0,  1055,     0,     0,   160,   248,
     507,     0,   319,   414,     0,     0,     0,     0,     0,   160,
       0,   504,     0,    39,     0,     0,     0,    41,     0,     0,
       0,     0,     0,     0,   635,     0,  1011,   636,     0,     0,
      37,    38,     0,     0,   972,   863,   319,     0,   975,   976,
       0,    37,    38,     0,    28,    29,    30,     0,     0,   864,
       0,     0,    37,    38,     0,     0,    39,     0,     0,     0,
      41,     0,     0,     0,     0,     0,     0,    39,     0,     0,
       0,    41,     0,     0,     0,   569,     0,  1034,    39,   319,
       0,     0,    41,   570,   571,   572,     0,     0,     0,     0,
     319,     0,     0,     0,     0,     0,  1044,   769,     0,  -574,
      63,   319,     0,     0,    64,    65,    66,     0,     0,     0,
       0,     0,     0,     0,     0,  1025,  1026,    67,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,     0,     0,     0,   770,
      69,     0,     0,  -574,     0,  -574,  -574,  -574,  -574,  -574,
       0,     0,     0,     0,     0,     0,     0,     0,    71,    72,
      73,    74,   771,    76,    77,    78,  -574,  -574,  -574,   772,
     773,   774,     0,    79,   775,    81,     0,    82,    83,   776,
      84,    85,  -574,  -574,     0,  -574,  -574,    86,     0,     0,
       0,    90,     0,    92,    93,    94,    95,    96,    97,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    98,
       0,  -574,     0,     0,    99,  -574,  -574,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     8,     0,     0,
     191,   255,   582,     0,   777,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,     0,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,   266,     0,     0,     0,     0,     0,
       0,     0,    27,    28,    29,    30,    31,    32,     0,   267,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,    34,    35,     0,   447,   448,
     449,   450,     0,   451,     0,   447,   448,   449,   450,     8,
      36,     0,     0,    37,    38,     0,   452,   583,   454,   455,
     584,   457,   458,   459,   460,   461,   462,   585,   457,   458,
     459,   460,   461,   462,   463,     0,     0,     0,     0,    39,
       0,     0,    40,    41,     0,     0,   414,     0,     0,     0,
     268,     0,     0,   269,   507,     8,   270,   271,   191,   255,
     272,   273,   274,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,   266,     0,     0,    37,    38,   745,     0,     0,
       0,    28,    29,    30,    31,    32,     0,   267,     0,     0,
     312,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    39,    33,    34,    35,    41,     0,     0,   447,   448,
     449,   450,     0,   451,     0,     0,     0,     8,    36,     0,
       0,    37,    38,     0,   319,     0,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   461,   462,   463,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    39,     0,     0,
       0,    41,     0,     0,   414,     0,     0,     0,   268,     0,
       0,   269,   641,     8,   270,   271,   191,   255,   272,   273,
     274,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     266,     0,     0,    37,    38,     0,     0,     0,     0,    28,
      29,    30,    31,    32,     0,   267,     0,     0,   498,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      33,    34,    35,    41,     0,   447,   448,   449,   450,     0,
     451,     0,     0,     0,     0,     8,    36,     0,     0,    37,
      38,     0,   319,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   461,   462,   463,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,     0,     0,     0,    41,
       0,     0,   414,     0,     0,     0,   268,     0,     0,   269,
     761,     8,   270,   271,   191,   255,   272,   273,   274,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,   266,     0,
       0,    37,    38,     0,     0,     0,     0,    28,    29,    30,
      31,    32,     0,   267,     0,     0,   501,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    39,    33,    34,
      35,    41,     0,   447,   448,   449,   450,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,    37,    38,     0,
     319,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    39,     0,     0,     0,    41,     0,     0,
       0,     0,     0,     0,   268,     0,     0,   269,     0,     8,
     270,   271,   191,   255,   272,   273,   274,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,     0,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,   266,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
       0,   267,     0,     0,   624,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,   447,
     448,   449,   450,     0,     0,     0,     0,     0,   447,   448,
     449,   450,    36,     0,     0,    37,    38,     0,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   455,
     456,   457,   458,   459,   460,   461,   462,   463,     0,     0,
       0,    39,     0,     0,   962,    41,     0,     0,     0,     0,
       0,     0,   268,     0,     0,   269,     0,     8,   270,   271,
     191,   255,   272,   273,   274,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,     0,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,   266,   735,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,     0,   267,
     447,   448,   449,   450,     0,   451,     0,     0,     0,     0,
       0,     0,     0,     0,    33,    34,    35,     0,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   461,   462,   463,
      36,     0,     0,    37,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
       0,     0,     0,    41,  1067,     0,     0,     0,     0,     0,
     268,     0,     0,   269,     0,     8,   270,   271,   191,   255,
     272,   273,   274,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,   266,   736,     0,     0,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,     0,   267,   447,   448,
     449,   450,     0,   451,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,    35,     0,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   461,   462,   463,    36,     0,
       0,    37,    38,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    39,     0,     0,
       0,    41,     0,     0,     0,     0,     0,     0,   268,     0,
       0,   269,     0,     8,   270,   271,   191,   255,   272,   273,
     274,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     266,     0,     0,     0,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,     0,   267,   447,   448,   449,   450,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,     0,     0,     0,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,    36,     0,     0,    37,
      38,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,     0,     0,     0,    41,
       0,     0,     0,     0,     0,     0,   268,     0,     0,   269,
       0,     8,   270,   271,   191,   255,   272,   273,   274,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,   266,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,     0,   267,   447,   448,   449,   450,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
      35,     0,     0,     8,     0,     0,   456,   457,   458,   459,
     460,   461,   462,   463,    36,     0,     0,    37,    38,     0,
       0,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     296,     0,     0,    39,     0,     0,     0,    41,    27,    28,
      29,    30,    31,    32,     0,     0,     0,   160,     0,     0,
     270,   271,     0,     0,   737,   273,   274,     0,     0,     0,
      33,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
      38,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,     0,     0,    40,    41,
       8,     0,     0,     0,     0,     0,   297,     0,     0,   298,
       0,     0,     0,     0,     0,     0,   165,     0,     0,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,   296,     0,     0,
       0,     0,     0,     0,     0,    27,    28,    29,    30,    31,
      32,     0,     0,     0,   160,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    33,    34,    35,
       0,     0,     8,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,     0,     0,    37,    38,     0,     0,
       0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,    39,     0,     0,    40,    41,    27,    28,    29,
      30,    31,    32,   470,     0,     0,   471,     0,     0,     0,
       0,     0,     0,   165,     0,     0,     0,     0,     0,    33,
      34,    35,     8,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,    37,    38,
       0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,     0,     0,    39,     0,     0,    40,    41,     0,
       0,    31,    32,     0,     0,   408,     0,     0,   409,     0,
       0,     0,     0,     0,     0,   165,     0,     0,     0,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,     0,    -2,    62,    41,  -574,
      63,     0,     0,     0,    64,    65,    66,     0,     0,     0,
       0,     0,     0,     0,     0,   165,     0,    67,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,     0,     0,     0,    68,
      69,     0,     0,     0,     0,  -574,  -574,  -574,  -574,  -574,
       0,     0,    70,     0,     0,     0,     0,     0,    71,    72,
      73,    74,    75,    76,    77,    78,  -574,  -574,  -574,     0,
       0,     0,     0,    79,    80,    81,     0,    82,    83,     0,
      84,    85,  -574,  -574,     0,  -574,  -574,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    62,
       0,  -574,    63,     0,     0,     0,    64,    65,    66,    98,
       0,  -574,     0,     0,    99,  -574,     0,     0,     0,    67,
    -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,     0,     0,
       0,    68,    69,     0,     0,   674,     0,  -574,  -574,  -574,
    -574,  -574,     0,     0,    70,     0,     0,     0,     0,     0,
      71,    72,    73,    74,    75,    76,    77,    78,  -574,  -574,
    -574,     0,     0,     0,     0,    79,    80,    81,     0,    82,
      83,     0,    84,    85,  -574,  -574,     0,  -574,  -574,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    62,     0,  -574,    63,     0,     0,     0,    64,    65,
      66,    98,     0,  -574,     0,     0,    99,  -574,     0,     0,
       0,    67,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,
       0,     0,     0,    68,    69,     0,     0,   766,     0,  -574,
    -574,  -574,  -574,  -574,     0,     0,    70,     0,     0,     0,
       0,     0,    71,    72,    73,    74,    75,    76,    77,    78,
    -574,  -574,  -574,     0,     0,     0,     0,    79,    80,    81,
       0,    82,    83,     0,    84,    85,  -574,  -574,     0,  -574,
    -574,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    62,     0,  -574,    63,     0,     0,     0,
      64,    65,    66,    98,     0,  -574,     0,     0,    99,  -574,
       0,     0,     0,    67,  -574,  -574,  -574,  -574,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,
    -574,  -574,     0,     0,     0,    68,    69,     0,     0,   823,
       0,  -574,  -574,  -574,  -574,  -574,     0,     0,    70,     0,
       0,     0,     0,     0,    71,    72,    73,    74,    75,    76,
      77,    78,  -574,  -574,  -574,     0,     0,     0,     0,    79,
      80,    81,     0,    82,    83,     0,    84,    85,  -574,  -574,
       0,  -574,  -574,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    62,     0,  -574,    63,     0,
       0,     0,    64,    65,    66,    98,     0,  -574,     0,     0,
      99,  -574,     0,     0,     0,    67,  -574,  -574,  -574,  -574,
    -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,  -574,
    -574,  -574,  -574,  -574,     0,     0,     0,    68,    69,     0,
       0,     0,     0,  -574,  -574,  -574,  -574,  -574,     0,     0,
      70,     0,     0,     0,   944,     0,    71,    72,    73,    74,
      75,    76,    77,    78,  -574,  -574,  -574,     0,     0,     0,
       0,    79,    80,    81,     0,    82,    83,     0,    84,    85,
    -574,  -574,     0,  -574,  -574,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,     7,     0,     8,
       0,     0,     0,     0,     0,     0,     0,    98,     0,  -574,
       0,     0,    99,  -574,     0,     0,     0,     0,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,    27,    28,    29,    30,    31,    32,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,    57,
       0,     8,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    36,     0,     0,    37,    38,     0,     0,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,     0,     0,
       0,    39,     0,     0,    40,    41,    27,    28,    29,    30,
      31,    32,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
      35,   198,     0,     8,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,    37,    38,     0,
       0,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
       0,     0,     0,    39,     0,     0,    40,    41,     0,    28,
      29,    30,    31,    32,     0,     0,     0,     0,     0,     0,
       0,     0,   487,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,     0,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
      38,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,   910,    18,   911,    20,   912,   913,    23,    24,    25,
      26,   447,   448,   449,   450,    39,   451,     0,     0,    41,
      28,    29,    30,    31,    32,     0,     0,     0,     0,   452,
     453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
     463,    33,    34,    35,     0,     0,     8,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,   251,     0,
      37,    38,     0,     0,     0,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,     0,     0,     0,    39,     0,     0,     0,
      41,   914,    28,    29,    30,    31,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,    34,    35,     0,     0,     0,     0,
       0,     8,     0,     0,     0,     0,     0,     0,     0,    36,
     943,   369,    37,    38,     0,     0,     0,     0,     0,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    39,     0,
       0,     0,    41,   914,     0,     0,    27,    28,    29,    30,
      31,    32,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
      35,     0,     0,     8,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,    37,    38,     0,
       0,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
       0,     0,     0,    39,     0,     0,    40,    41,    27,    28,
      29,    30,    31,    32,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,     0,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
      38,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,   231,     0,     0,     0,    39,     0,     0,    40,    41,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,   628,     0,     0,     0,     0,     0,     0,
       0,    33,    34,    35,     0,     8,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
      37,    38,     0,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,   447,   448,   449,   450,    39,   451,     0,     0,
      41,    28,    29,    30,    31,    32,     0,     0,     0,     0,
     452,   453,   454,   455,   456,   457,   458,   459,   460,   461,
     462,   463,    33,    34,    35,     0,     8,     0,     0,     0,
     630,     0,     0,     0,     0,     0,     0,     0,    36,   251,
       0,    37,    38,     0,     0,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,     0,     0,     0,     0,    39,     0,     0,
       0,    41,    28,    29,    30,    31,    32,     0,     0,   447,
     448,   449,   450,     0,   451,     0,     0,     0,     0,     0,
       0,     0,     0,    33,    34,    35,     8,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,    36,
       0,     0,    37,    38,     0,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,     0,     0,     0,     0,     0,    39,     0,
       0,     0,    41,     0,     0,    31,    32,     0,     0,     0,
       0,     0,     0,     0,     0,   748,     0,     0,     0,     0,
       0,     0,     0,    33,    34,    35,     8,   754,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
       0,     0,    37,    38,     0,     9,    10,    11,    12,    13,
      14,    15,    16,   700,    18,   701,    20,   702,   703,    23,
      24,    25,    26,     0,   447,   448,   449,   450,    39,   451,
       0,     0,    41,     0,     0,     0,   447,   448,   449,   450,
       0,   451,   452,   453,   454,   455,   456,   457,   458,   459,
     460,   461,   462,   463,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,     0,     0,     0,    36,
       0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    39,     0,
       0,     0,    41
};

static const yytype_int16 yycheck[] =
{
       0,    71,     1,     2,   150,     5,    21,   139,   587,     1,
       2,     5,    60,    44,     5,   159,   362,   240,   704,   426,
     550,    21,   225,    93,    94,    93,    41,    46,   531,     5,
     562,   384,   376,   522,   180,   181,   669,   141,     5,     5,
     243,    41,   531,   855,     5,    46,    46,   732,    41,    21,
      40,    46,     4,     3,    54,     6,    42,    41,     3,     5,
     951,   157,   953,     5,    75,    46,     1,     6,    60,   139,
      70,   190,     5,    41,     6,    75,    76,   196,     6,    49,
      50,    51,     5,   202,     5,     0,     3,   293,   207,    45,
     103,    47,     3,     5,    46,    40,    99,    54,     5,   110,
       5,   297,   298,     5,    43,   301,    54,    42,    43,     5,
     110,    46,     5,    85,     5,   800,   111,   236,   104,     3,
      42,    48,    54,   116,   105,   106,     5,   246,     5,    40,
       5,     5,   116,     5,    85,    48,   139,   233,   344,   129,
     670,   141,   141,     3,    89,    90,    57,   162,   151,   141,
     169,   963,   171,   103,  1045,   103,   157,    85,   126,   116,
     208,   129,   162,   211,   165,   157,   136,   118,   575,   169,
     115,   171,    89,    90,   119,    92,   520,    43,    89,    90,
       3,    43,   104,   231,   870,    40,     3,   187,   541,     3,
     193,   239,    54,   138,   879,   539,     6,  1041,   115,   155,
     296,   201,   119,     6,   115,    89,    90,    40,   119,   165,
       6,    54,   408,   409,   410,   900,   208,    40,   362,   211,
     220,   317,    48,    40,     3,    48,    40,   138,   117,    89,
      90,    48,   233,   233,  1046,   235,   236,   103,    40,   231,
     240,   240,   243,    46,  1056,  1089,     3,   239,   240,    59,
      46,   140,   252,    44,   254,   115,    89,    90,    42,   119,
     103,   280,     3,    47,     3,     6,    89,    90,   103,     1,
       3,   126,    89,    90,   129,    89,    90,   809,   278,   280,
     280,   136,   330,    40,   814,   333,    77,   483,   484,     4,
     923,    48,   115,   978,    77,   296,   119,   704,   115,    82,
      57,   115,   119,   140,   296,   119,   713,    43,    40,   126,
      89,    90,   129,   816,   384,   138,   317,    43,   521,  1005,
     390,   138,   390,     3,   138,   317,   327,   816,    71,   552,
       3,    46,    89,    90,     3,    43,  1022,     6,   330,   443,
     119,   333,   361,    21,    87,   548,    43,   346,    89,    90,
      89,    90,   696,     4,   346,     6,    89,    90,   115,   360,
     556,   361,   119,   433,   105,   106,    44,    40,    76,   126,
    1071,    42,   129,   117,   115,    48,   376,   425,   119,   136,
     119,   138,     3,    54,    57,   126,   119,     4,   129,     6,
       3,    40,   971,     6,  1079,   136,   140,   927,     3,    77,
      78,    79,   402,  1104,    82,   812,    84,    85,    57,    89,
      90,   411,   531,   414,   120,    43,    89,    90,    46,    52,
      89,    90,   422,   423,   425,    43,   426,    40,    46,    46,
     430,   432,    40,   425,    54,    48,   105,   106,    71,   119,
     488,   103,   115,     3,   443,   493,   119,   190,    76,    57,
      40,   443,   195,   196,   116,   117,    76,   118,    76,   202,
      84,   204,   205,   870,   207,   138,   469,    57,    89,    90,
     426,   541,    52,    53,    40,   103,    89,    90,   140,   222,
      40,    40,   225,   226,    89,    90,     4,     3,     6,    48,
      40,    71,    72,   236,   115,     3,   488,    57,   119,    40,
     243,   493,   115,   246,    40,   126,   119,     3,   129,   252,
     115,    40,    41,   126,   119,   136,   129,   517,   864,    43,
     520,    57,    46,   136,    40,   138,    40,    43,    57,    89,
      90,   136,    48,   564,   533,    40,    40,   537,   199,   539,
     540,   533,    46,   537,    40,   545,   537,    40,    40,   540,
     549,    43,    48,   552,    46,   115,   575,   549,   577,   119,
     552,   537,     3,    40,   767,   768,   126,    40,   587,   129,
     537,   537,   233,    89,    90,   575,   537,   577,   578,    40,
      41,    89,    90,    40,   585,    43,   587,   587,    46,   589,
     251,   537,   253,    89,    90,   537,    57,    40,  1005,   115,
      40,    84,    85,   119,   537,    46,    46,   115,   678,  1083,
     126,   119,    40,   129,   537,  1022,   537,    52,    53,   115,
     136,    40,   138,   119,    43,   537,  1100,  1101,  1102,   585,
     537,   777,   537,    40,   682,   537,    71,    72,     6,  1113,
      43,   537,   138,    46,   537,   116,   537,   647,    89,    90,
      40,    41,    48,   647,    48,   655,   647,    54,   537,    48,
     537,  1068,   537,   537,   664,   537,   714,    57,   103,    48,
      43,   647,   672,    46,   115,    41,   676,   809,   119,    76,
     647,   647,   676,    80,    42,   676,   647,   702,   105,   106,
     682,    88,    89,    90,   694,   695,   696,   697,    43,   818,
     676,   647,   702,   694,   704,   647,   725,   738,   664,   676,
     676,    46,   712,   713,   647,   676,   672,     6,   712,   942,
     864,    46,   714,    43,   647,   725,   647,   746,   694,    42,
     676,   835,   732,   694,   676,   647,   737,   738,     3,   809,
     647,   697,   647,   676,   934,   647,   746,   937,   704,    40,
      41,   647,   694,   676,   647,   676,   647,   713,   828,   905,
     828,   116,  1108,   811,   676,   104,    57,    42,   647,   676,
     647,   676,   647,   647,   676,   647,    52,    53,   521,   849,
     676,   737,    40,   676,     7,   676,    41,    42,   531,    48,
       4,     4,     6,     6,     3,    71,    72,   676,    48,   676,
     800,   676,   676,   546,   676,   548,   809,    43,   856,    43,
      46,    76,    46,   957,    41,   863,   886,   104,    54,   811,
      54,   821,    41,    42,    89,    90,   826,   103,   116,    43,
      43,    40,    46,    46,    43,   835,   835,   840,    41,    48,
      54,    40,    41,   835,   914,   664,   849,   121,   122,   123,
     115,   851,    41,   672,   119,    41,    42,   851,    57,    57,
     851,    49,    50,    51,   856,   821,   866,    48,    40,   530,
     870,   863,   872,   534,    48,   851,    40,    41,   697,   879,
      89,    90,    48,   136,   851,   851,   134,   135,   136,    48,
     851,   116,    43,    57,   713,    41,   557,   912,    41,    41,
     900,   114,    73,    74,    75,   851,   115,    46,    41,   851,
     119,    42,   912,    80,   870,    41,    54,   126,   851,    41,
     129,    88,    89,    90,    43,   925,    41,   136,   851,   138,
     851,    41,   980,    41,    41,   678,   984,    42,    41,   851,
      41,   104,   942,   942,   851,    41,   851,   116,    43,   851,
     942,   951,   971,   953,  1098,   851,   116,   951,   851,   953,
     851,    46,   111,    76,  1108,   965,   966,    48,   116,   925,
     971,   971,   851,    48,   851,   116,   851,   851,   978,   851,
    1028,   982,  1030,   984,    48,    41,    41,    48,   980,    48,
     990,    48,   984,    43,    48,     3,    48,    43,     6,   999,
    1070,    41,   821,   107,   108,  1005,    43,    46,    42,   670,
      40,    47,    46,   163,   164,  1063,  1064,  1065,    43,    53,
      91,    41,  1022,   173,   767,   768,   130,   131,   132,   133,
     134,   135,   136,    41,   990,    57,  1028,    40,  1030,  1087,
      78,    90,    89,   999,    46,  1045,    41,    48,    41,  1005,
       3,  1045,    86,    87,    48,  1055,     3,    48,    48,   720,
      48,  1061,    48,    48,    48,    42,  1022,    42,  1068,    40,
     140,  1063,  1064,  1065,    54,   818,   103,    41,    47,  1079,
    1080,    89,    90,  1083,   118,    42,    62,    40,   831,   832,
     833,    41,    41,    40,    40,  1087,    41,   105,   106,    41,
    1100,  1101,  1102,    69,    57,  1061,   925,    48,    46,    48,
      57,    48,  1068,  1113,    48,   149,     3,   860,    76,    49,
      50,    51,    48,   157,  1080,    76,    92,  1083,    76,   163,
     164,    97,   166,   138,    40,    47,    89,    90,    47,   173,
      43,    41,    89,    90,  1100,  1101,  1102,   297,   298,    54,
      80,   301,    43,    40,    76,    54,   817,  1113,    88,    89,
      90,    48,   115,    41,   825,   199,   119,    47,   115,   203,
      57,   990,   119,   126,    48,    48,   129,    40,    76,   126,
     999,    76,   129,   217,    76,   138,   336,   337,    76,    41,
      76,   138,   132,   133,   134,   135,   136,    42,   941,   233,
      43,    43,    89,    90,    41,    54,    41,   241,   104,   243,
     244,   245,    43,   247,    48,    47,    40,   251,   252,   253,
     254,    76,    43,    43,   190,    43,    43,   104,   115,   195,
     196,    43,   119,    42,    48,    48,   202,    41,    41,    47,
      40,   207,  1061,    40,   160,   161,   280,    41,    43,  1068,
      48,   138,    48,    41,    41,   916,    40,    48,    48,   225,
    1003,    41,   296,   297,   298,   926,    41,   301,    41,    46,
     236,   237,    43,    49,    50,    51,   242,   243,    54,    48,
     246,    41,   943,   317,    48,   319,     3,    43,   545,   522,
     694,   552,  1023,   954,   942,   695,   164,   517,   835,   694,
      76,   694,   336,   337,    80,   173,   967,   341,   694,   589,
     562,   443,    88,    89,    90,    51,  1003,  1055,   430,   353,
     470,   471,   472,    40,   872,    42,   360,   966,   570,   479,
     480,    48,  1050,  1068,    -1,   679,    -1,    54,    -1,   489,
      57,    -1,    -1,   377,    -1,    -1,    -1,    -1,    -1,     3,
     266,    -1,   268,   269,   270,   271,   272,   273,   274,    76,
     510,    -1,  1023,   130,   131,   132,   133,   134,   135,   136,
      21,     3,    89,    90,   408,   409,   410,   245,    -1,   247,
     414,    -1,   416,   417,    -1,   419,    40,   104,    -1,    -1,
      -1,   425,   308,    44,    48,    -1,    -1,    -1,   115,    -1,
      54,    -1,   119,    57,    -1,    -1,    -1,    -1,    40,   126,
      -1,    43,   129,    -1,    -1,   331,    48,    -1,   334,   136,
      -1,   138,    76,    -1,    -1,    -1,    77,    78,    79,    -1,
      -1,    82,    83,    84,    85,    89,    90,    -1,   472,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   483,
     484,    -1,    -1,    -1,    -1,   489,    -1,    89,    90,    -1,
      -1,   115,    -1,   613,    -1,   119,    -1,   118,   336,   337,
      -1,    -1,   126,   341,    -1,   129,   510,    -1,    -1,   513,
      -1,    -1,   136,   115,   138,    -1,    -1,   119,    -1,    -1,
      -1,    -1,    -1,    -1,   126,    -1,   530,   129,    -1,    -1,
     534,     6,    -1,    -1,   136,    -1,   138,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,     3,     4,    -1,
       6,    -1,   556,   557,   558,    -1,    -1,   561,   562,    -1,
      -1,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   461,   462,   463,   416,   417,
      -1,   419,   518,   587,    40,   521,   522,    43,    -1,    -1,
      46,     3,    48,   529,   530,   531,    -1,    -1,    -1,   485,
      -1,    -1,    -1,   607,    -1,    -1,    -1,    -1,   494,    -1,
      -1,    -1,   548,    -1,   550,    -1,    -1,   621,   622,    -1,
      -1,   557,   626,     3,    -1,    -1,    -1,   631,    40,    -1,
      -1,    43,   636,    89,    90,    -1,    48,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    31,    -1,    33,
      -1,   489,    36,    37,    38,    39,    -1,    -1,   114,   115,
      40,    -1,    -1,   119,    -1,    -1,   670,    -1,    48,    -1,
     126,    -1,   510,   129,     3,   513,    -1,    89,    90,    -1,
     136,    -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,   693,
      -1,    -1,     3,    -1,    -1,   699,   582,   583,   584,   585,
      -1,    -1,    -1,   115,    -1,    -1,    -1,   119,    -1,    89,
      90,    40,    -1,    -1,   126,    -1,   720,   129,    -1,    48,
     558,    -1,    -1,   561,   136,    -1,   138,    -1,    57,    40,
      -1,    -1,    -1,    -1,   670,   115,    -1,    48,    -1,   119,
      -1,   677,    -1,   679,    -1,    -1,   126,    -1,    -1,   129,
       3,    -1,     3,   757,   758,     6,   136,    -1,   138,   763,
      89,    90,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    90,
      -1,    -1,    -1,   621,   622,    -1,   115,    40,   626,    -1,
     119,    -1,    -1,   631,    -1,    48,    -1,   126,   636,    -1,
     129,    -1,    -1,    -1,   115,   809,    -1,    -1,   119,   138,
      -1,     3,    -1,   817,    -1,   126,    -1,    -1,   129,    -1,
      -1,   825,    -1,    -1,    -1,   136,    -1,   138,    -1,   715,
      -1,   767,   768,    -1,    -1,    -1,    89,    90,    89,    90,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,   735,
     736,   737,    -1,   739,   105,   106,    48,    -1,    -1,   745,
      -1,   699,   115,    -1,    -1,    57,   119,    -1,    -1,     3,
      -1,    -1,   876,   126,    -1,    -1,   129,    -1,   814,    -1,
     816,    -1,   818,   136,    -1,   138,    -1,     3,   892,    -1,
      -1,    -1,   896,   897,    -1,    -1,    -1,    89,    90,    -1,
      -1,    -1,    -1,   839,    -1,    -1,    40,    -1,    -1,    -1,
      -1,    -1,   916,    -1,    48,    -1,    -1,    -1,    -1,   757,
     758,     3,   926,   115,    40,   763,    -1,   119,    -1,    -1,
      -1,    -1,    48,    -1,   126,    -1,    -1,   129,    -1,   943,
      -1,    -1,    -1,    -1,    -1,    -1,   138,    -1,    -1,    -1,
     954,    -1,    -1,    -1,   958,    89,    90,    -1,    40,    -1,
      -1,    -1,    -1,   967,     3,    -1,    48,   971,    -1,   973,
     974,    -1,    -1,    89,    90,     3,    -1,    -1,    -1,    -1,
     984,   115,    -1,    -1,    -1,   119,     3,    -1,    -1,    -1,
      -1,   927,   126,    -1,    -1,   129,    -1,   883,   884,   115,
      -1,    40,   136,   119,   138,    -1,    -1,    89,    90,    48,
     126,    -1,    40,   129,    -1,  1019,    -1,    -1,    57,  1023,
      48,    -1,   138,    40,    -1,    -1,    -1,    -1,    -1,    57,
      -1,    48,    -1,   115,    -1,    -1,    -1,   119,    -1,    -1,
      -1,    -1,    -1,    -1,   126,    -1,  1050,   129,    -1,    -1,
      89,    90,    -1,    -1,   892,    40,   138,    -1,   896,   897,
      -1,    89,    90,    -1,    49,    50,    51,    -1,    -1,    54,
      -1,    -1,    89,    90,    -1,    -1,   115,    -1,    -1,    -1,
     119,    -1,    -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,
      -1,   119,    -1,    -1,    -1,    80,    -1,   983,   115,   138,
      -1,    -1,   119,    88,    89,    90,    -1,    -1,    -1,    -1,
     138,    -1,    -1,    -1,    -1,    -1,  1002,     1,    -1,     3,
       4,   138,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   973,   974,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    43,
      44,    -1,    -1,    47,    -1,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    -1,    77,    78,    79,    -1,    81,    82,    83,
      84,    85,    86,    87,    -1,    89,    90,    91,    -1,    -1,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,
      -1,   115,    -1,    -1,   118,   119,   120,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,
       6,     7,    41,    -1,   138,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    49,    50,    51,    52,    53,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    71,    72,    -1,   107,   108,
     109,   110,    -1,   112,    -1,   107,   108,   109,   110,     3,
      86,    -1,    -1,    89,    90,    -1,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,    -1,   115,
      -1,    -1,   118,   119,    -1,    -1,    40,    -1,    -1,    -1,
     126,    -1,    -1,   129,    48,     3,   132,   133,     6,     7,
     136,   137,   138,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    89,    90,    76,    -1,    -1,
      -1,    49,    50,    51,    52,    53,    -1,    55,    -1,    -1,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   115,    70,    71,    72,   119,    -1,    -1,   107,   108,
     109,   110,    -1,   112,    -1,    -1,    -1,     3,    86,    -1,
      -1,    89,    90,    -1,   138,    -1,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,
      -1,   119,    -1,    -1,    40,    -1,    -1,    -1,   126,    -1,
      -1,   129,    48,     3,   132,   133,     6,     7,   136,   137,
     138,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    89,    90,    -1,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    55,    -1,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,
      70,    71,    72,   119,    -1,   107,   108,   109,   110,    -1,
     112,    -1,    -1,    -1,    -1,     3,    86,    -1,    -1,    89,
      90,    -1,   138,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,    -1,   119,
      -1,    -1,    40,    -1,    -1,    -1,   126,    -1,    -1,   129,
      48,     3,   132,   133,     6,     7,   136,   137,   138,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    89,    90,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    -1,    55,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,    70,    71,
      72,   119,    -1,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
     138,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   115,    -1,    -1,    -1,   119,    -1,    -1,
      -1,    -1,    -1,    -1,   126,    -1,    -1,   129,    -1,     3,
     132,   133,     6,     7,   136,   137,   138,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      -1,    55,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,   107,
     108,   109,   110,    -1,    -1,    -1,    -1,    -1,   107,   108,
     109,   110,    86,    -1,    -1,    89,    90,    -1,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,    -1,
      -1,   115,    -1,    -1,    41,   119,    -1,    -1,    -1,    -1,
      -1,    -1,   126,    -1,    -1,   129,    -1,     3,   132,   133,
       6,     7,   136,   137,   138,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    -1,    55,
     107,   108,   109,   110,    -1,   112,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    71,    72,    -1,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
      86,    -1,    -1,    89,    90,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,
      -1,    -1,    -1,   119,    43,    -1,    -1,    -1,    -1,    -1,
     126,    -1,    -1,   129,    -1,     3,   132,   133,     6,     7,
     136,   137,   138,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    51,    52,    53,    -1,    55,   107,   108,
     109,   110,    -1,   112,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    -1,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    86,    -1,
      -1,    89,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,
      -1,   119,    -1,    -1,    -1,    -1,    -1,    -1,   126,    -1,
      -1,   129,    -1,     3,   132,   133,     6,     7,   136,   137,
     138,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    55,   107,   108,   109,   110,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,    -1,    -1,    -1,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    86,    -1,    -1,    89,
      90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,    -1,   119,
      -1,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,   129,
      -1,     3,   132,   133,     6,     7,   136,   137,   138,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    -1,    55,   107,   108,   109,   110,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,    -1,    -1,     3,    -1,    -1,   129,   130,   131,   132,
     133,   134,   135,   136,    86,    -1,    -1,    89,    90,    -1,
      -1,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,   115,    -1,    -1,    -1,   119,    48,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    57,    -1,    -1,
     132,   133,    -1,    -1,   136,   137,   138,    -1,    -1,    -1,
      70,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,   118,   119,
       3,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,   129,
      -1,    -1,    -1,    -1,    -1,    -1,   136,    -1,    -1,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    57,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    -1,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,   115,    -1,    -1,   118,   119,    48,    49,    50,
      51,    52,    53,   126,    -1,    -1,   129,    -1,    -1,    -1,
      -1,    -1,    -1,   136,    -1,    -1,    -1,    -1,    -1,    70,
      71,    72,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,   115,    -1,    -1,   118,   119,    -1,
      -1,    52,    53,    -1,    -1,   126,    -1,    -1,   129,    -1,
      -1,    -1,    -1,    -1,    -1,   136,    -1,    -1,    -1,    70,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   115,    -1,     0,     1,   119,     3,
       4,    -1,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   136,    -1,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    43,
      44,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    -1,
      -1,    -1,    -1,    77,    78,    79,    -1,    81,    82,    -1,
      84,    85,    86,    87,    -1,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,     1,
      -1,     3,     4,    -1,    -1,    -1,     8,     9,    10,   113,
      -1,   115,    -1,    -1,   118,   119,    -1,    -1,    -1,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    43,    44,    -1,    -1,    47,    -1,    49,    50,    51,
      52,    53,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    -1,    -1,    -1,    -1,    77,    78,    79,    -1,    81,
      82,    -1,    84,    85,    86,    87,    -1,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,     1,    -1,     3,     4,    -1,    -1,    -1,     8,     9,
      10,   113,    -1,   115,    -1,    -1,   118,   119,    -1,    -1,
      -1,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    43,    44,    -1,    -1,    47,    -1,    49,
      50,    51,    52,    53,    -1,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    -1,    -1,    -1,    -1,    77,    78,    79,
      -1,    81,    82,    -1,    84,    85,    86,    87,    -1,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     1,    -1,     3,     4,    -1,    -1,    -1,
       8,     9,    10,   113,    -1,   115,    -1,    -1,   118,   119,
      -1,    -1,    -1,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    43,    44,    -1,    -1,    47,
      -1,    49,    50,    51,    52,    53,    -1,    -1,    56,    -1,
      -1,    -1,    -1,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    -1,    -1,    -1,    -1,    77,
      78,    79,    -1,    81,    82,    -1,    84,    85,    86,    87,
      -1,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,     1,    -1,     3,     4,    -1,
      -1,    -1,     8,     9,    10,   113,    -1,   115,    -1,    -1,
     118,   119,    -1,    -1,    -1,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    43,    44,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    -1,    -1,
      56,    -1,    -1,    -1,    60,    -1,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    -1,    -1,    -1,
      -1,    77,    78,    79,    -1,    81,    82,    -1,    84,    85,
      86,    87,    -1,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,     1,    -1,     3,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,    -1,   115,
      -1,    -1,   118,   119,    -1,    -1,    -1,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,     1,
      -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,   115,    -1,    -1,   118,   119,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,     1,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      -1,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,   115,    -1,    -1,   118,   119,    -1,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,   107,   108,   109,   110,   115,   112,    -1,    -1,   119,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    70,    71,    72,    -1,    -1,     3,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,
      89,    90,    -1,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,   115,    -1,    -1,    -1,
     119,   120,    49,    50,    51,    52,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,    -1,    -1,    -1,    -1,
      -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    13,    89,    90,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,   115,    -1,
      -1,    -1,   119,   120,    -1,    -1,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      -1,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,   115,    -1,    -1,   118,   119,    48,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,    -1,   115,    -1,    -1,   118,   119,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,    -1,     3,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    -1,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,   107,   108,   109,   110,   115,   112,    -1,    -1,
     119,    49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    70,    71,    72,    -1,     3,    -1,    -1,    -1,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      -1,    89,    90,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,   115,    -1,    -1,
      -1,   119,    49,    50,    51,    52,    53,    -1,    -1,   107,
     108,   109,   110,    -1,   112,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,     3,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    86,
      -1,    -1,    89,    90,    -1,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,    -1,   115,    -1,
      -1,    -1,   119,    -1,    -1,    52,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,     3,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,   107,   108,   109,   110,   115,   112,
      -1,    -1,   119,    -1,    -1,    -1,   107,   108,   109,   110,
      -1,   112,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,    -1,
      -1,    -1,   119
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   121,   122,   123,   142,   143,   308,     1,     3,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    48,    49,    50,
      51,    52,    53,    70,    71,    72,    86,    89,    90,   115,
     118,   119,   193,   236,   250,   251,   253,   254,   255,   256,
     257,   258,   283,   284,   294,   297,   299,     1,   236,     1,
      40,     0,     1,     4,     8,     9,    10,    21,    43,    44,
      56,    62,    63,    64,    65,    66,    67,    68,    69,    77,
      78,    79,    81,    82,    84,    85,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   113,   118,
     144,   145,   146,   148,   149,   150,   151,   152,   155,   156,
     158,   159,   160,   161,   162,   163,   164,   167,   168,   169,
     172,   174,   179,   180,   181,   182,   184,   188,   195,   196,
     197,   198,   199,   203,   204,   211,   212,   223,   231,   232,
     308,   103,   293,   308,    48,    52,    71,    48,    48,    40,
     140,   103,   293,   297,    43,   254,   250,    40,    48,    54,
      57,    76,   119,   126,   129,   136,   138,   241,   242,   244,
     246,   247,   248,   249,   297,   308,   250,   257,   297,   293,
     117,   140,   298,    43,    43,   233,   234,   236,   308,   120,
      40,     6,    85,   118,   302,    40,   305,   308,     1,   252,
     253,   294,    40,   305,    40,   166,   308,    40,    40,    84,
      85,    40,    84,    77,    82,    44,    77,    92,   297,    46,
     294,   297,    40,     4,    46,    40,    40,    43,    46,     4,
     302,    40,   178,   252,   176,   178,    40,    40,   302,    40,
     103,   284,   305,    40,   126,   129,   244,   249,   297,    21,
      85,    87,   193,   252,   284,     7,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    40,    55,   126,   129,
     132,   133,   136,   137,   138,   236,   237,   238,   240,   252,
     253,   269,   270,   271,   272,   302,   303,   308,    48,    48,
      48,   297,   119,   299,   284,   250,    40,   126,   129,   233,
     247,   249,   297,    48,    46,   105,   106,   259,   260,   261,
     262,   263,    58,   269,   270,   269,     3,    40,    48,   138,
     245,   248,   297,    48,   245,   248,   249,   250,   297,   241,
      40,    57,   241,    40,    57,    48,   126,   129,   245,   248,
     297,   116,   299,   119,   299,    41,    42,   235,   308,   261,
     294,   295,   302,   284,     6,    46,   295,   306,   295,    43,
      40,   244,    54,    41,   295,   297,   294,   294,   295,    13,
     173,   233,   233,   297,    43,    54,   214,    54,    46,   294,
     175,   306,   294,   233,    46,   243,   244,   247,   308,    43,
      42,   177,   308,   295,   296,   308,   153,   154,   302,   233,
     207,   208,   209,   236,   283,   308,   297,   302,   126,   129,
     249,   294,   297,   306,    40,   295,   126,   129,   297,   116,
     244,   297,   264,   294,   308,    40,   244,    76,   275,   276,
     297,   308,   269,    40,    48,   269,   269,   269,   269,   269,
     269,   269,   104,    42,   239,   308,    40,   107,   108,   109,
     110,   112,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,     7,    48,    48,    41,   298,   104,
     126,   129,   249,   297,   246,   297,   246,    41,    41,   126,
     129,   246,   297,   116,    48,    57,   269,    58,    40,   249,
     297,    48,   297,    40,    57,    48,   249,   233,    58,   269,
     233,    58,   269,    48,    48,   245,   248,    48,   245,   248,
     116,    48,   126,   129,   245,   298,    43,   236,    41,   297,
     183,    42,    54,    41,   241,   259,    41,    46,    41,    54,
      41,    42,   171,    42,    41,    41,    43,   143,   297,   213,
      41,    41,    41,    41,   176,   178,    41,    41,    42,    46,
      41,   104,    42,   210,   308,    59,   116,    41,   249,   297,
      43,   116,   111,    54,    76,   194,   308,   233,   297,    80,
      88,    89,    90,   186,   241,   250,   286,   287,   277,    46,
      43,   275,    41,   126,   129,   136,   249,   252,    48,   240,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   284,   297,   116,
      41,    41,    41,   116,   246,   246,   269,   233,   245,   297,
      41,   116,    48,   233,    58,   269,    48,    41,    58,    41,
      58,    48,    48,    48,    48,   126,   129,   245,   248,    48,
      48,    48,   245,   235,     4,    46,   302,   143,   306,   153,
     271,   302,   307,    43,    43,   147,     4,   165,   302,     4,
      43,    46,   114,   170,   244,   302,   304,   295,   302,   307,
      41,   236,   244,    46,    47,    43,   143,    44,   232,   176,
      43,    46,    40,    47,   177,   115,   119,   294,   300,    43,
     306,   236,   170,    91,   205,   209,   157,   244,   302,   116,
      30,    32,    34,    35,   187,   255,   256,   297,    57,   189,
     254,    43,    46,    41,    40,    40,   286,    90,    89,     1,
      42,    43,    46,   185,   241,   287,   241,    78,   278,   279,
     285,   308,   201,    46,   269,    41,    41,   136,   250,    41,
     126,   129,   242,    48,   239,    76,   297,    41,    58,    41,
      41,   245,   245,    41,    58,   245,   245,    48,    48,    48,
      48,    48,   245,    48,    48,    48,    47,    42,    42,     1,
      43,    66,    73,    74,    75,    78,    83,   138,   148,   149,
     150,   151,   155,   156,   160,   162,   164,   167,   169,   172,
     174,   179,   180,   181,   182,   199,   203,   204,   211,   215,
     219,   220,   221,   222,   223,   224,   225,   226,   229,   232,
     308,    40,   250,   287,   288,   308,    54,    41,    42,   171,
     170,   244,   288,    47,   302,   252,   294,    43,    54,   304,
     233,   140,   117,   140,   301,   103,    41,    47,   297,    44,
     118,   184,   199,   203,   204,   206,   220,   222,   224,   232,
     210,   143,   288,    43,   293,   186,    40,    46,   190,   150,
     265,   266,   308,    40,    54,   287,   288,   289,   233,   269,
     244,   241,    42,    73,    74,    75,   280,   282,   215,   200,
     269,   269,   269,    41,    41,    41,    40,   269,   241,    41,
     245,   245,    48,    48,    48,   245,    48,    48,   306,   306,
     218,    46,    76,    76,    76,   138,    40,   299,    47,   215,
      30,    32,    34,    35,   120,   230,   252,   256,   297,   233,
     287,   170,   302,   307,    43,   244,    41,   288,    43,   244,
      43,   178,    41,   119,   294,   294,   119,   294,   237,     4,
      46,    54,   103,    87,    60,    43,   185,   233,    40,    43,
     191,   267,   294,    42,    47,   233,   259,    54,    76,   290,
     308,    41,    41,   186,   279,   297,   281,    47,   215,   269,
     269,   252,   245,    48,    48,   245,   245,   215,   216,   299,
      40,   293,   252,    76,    40,    43,    41,   171,   288,    43,
     244,   170,    43,    43,   301,   301,   104,   294,   207,    41,
     192,   265,    54,   265,    43,   244,    41,    43,   261,   291,
     292,   297,    43,    46,   185,    48,   273,   274,   308,   285,
      43,   202,   244,    47,   242,   245,   245,   215,    40,   233,
      40,   126,   129,   249,   269,   233,    43,    43,   288,    43,
     104,   288,    43,   268,   269,   267,   186,    43,    46,    43,
      42,    48,    40,    46,    48,   297,   186,   202,    41,    47,
     233,    41,   233,    40,    40,    40,   129,    43,    41,    43,
     111,   190,   265,   185,   292,    48,    48,   274,   185,   217,
      41,   227,   288,    41,   233,   233,   233,    40,   289,   252,
     191,    48,    48,   215,   228,   288,    43,    46,    54,   228,
      41,    41,    41,   233,   190,    48,    43,    46,    54,   261,
     228,   228,   228,    41,   191,    48,   259,    43,   228,    43
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1464 of yacc.c  */
#line 1466 "parser.y"
    {
                   if (!classes) classes = NewHash();
		   Setattr((yyvsp[(1) - (1)].node),"classes",classes); 
		   Setattr((yyvsp[(1) - (1)].node),"name",ModuleName);
		   
		   if ((!module_node) && ModuleName) {
		     module_node = new_node("module");
		     Setattr(module_node,"name",ModuleName);
		   }
		   Setattr((yyvsp[(1) - (1)].node),"module",module_node);
	           top = (yyvsp[(1) - (1)].node);
               }
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 1478 "parser.y"
    {
                 top = Copy(Getattr((yyvsp[(2) - (3)].p),"type"));
		 Delete((yyvsp[(2) - (3)].p));
               }
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 1482 "parser.y"
    {
                 top = 0;
               }
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 1485 "parser.y"
    {
                 top = (yyvsp[(2) - (3)].p);
               }
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 1488 "parser.y"
    {
                 top = 0;
               }
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 1491 "parser.y"
    {
                 top = (yyvsp[(3) - (5)].pl);
               }
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 1494 "parser.y"
    {
                 top = 0;
               }
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 1499 "parser.y"
    {  
                   /* add declaration to end of linked list (the declaration isn't always a single declaration, sometimes it is a linked list itself) */
                   appendChild((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));
                   (yyval.node) = (yyvsp[(1) - (2)].node);
               }
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 1504 "parser.y"
    {
                   (yyval.node) = new_node("top");
               }
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 1509 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 1510 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 1511 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 1512 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 1513 "parser.y"
    {
                  (yyval.node) = 0;
		  if (cparse_unknown_directive) {
		      Swig_error(cparse_file, cparse_line, "Unknown directive '%s'.\n", cparse_unknown_directive);
		  } else {
		      Swig_error(cparse_file, cparse_line, "Syntax error in input(1).\n");
		  }
		  exit(1);
               }
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 1523 "parser.y"
    { 
                  if ((yyval.node)) {
   		      add_symbols((yyval.node));
                  }
                  (yyval.node) = (yyvsp[(1) - (1)].node); 
	       }
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 1539 "parser.y"
    {
                  (yyval.node) = 0;
                  skip_decl();
               }
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 1549 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 1550 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 1551 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 1552 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 1553 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 1554 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 1555 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 1556 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 1557 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 1558 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 1559 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 1560 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 1561 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 1562 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 1563 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 1564 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 1565 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 1566 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 1567 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 1568 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 1569 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 1576 "parser.y"
    {
               Node *cls;
	       String *clsname;
	       extendmode = 1;
	       cplus_mode = CPLUS_PUBLIC;
	       if (!classes) classes = NewHash();
	       if (!classes_typedefs) classes_typedefs = NewHash();
	       clsname = make_class_name((yyvsp[(3) - (4)].str));
	       cls = Getattr(classes,clsname);
	       if (!cls) {
	         cls = Getattr(classes_typedefs, clsname);
		 if (!cls) {
		   /* No previous definition. Create a new scope */
		   Node *am = Getattr(Swig_extend_hash(),clsname);
		   if (!am) {
		     Swig_symbol_newscope();
		     Swig_symbol_setscopename((yyvsp[(3) - (4)].str));
		     prev_symtab = 0;
		   } else {
		     prev_symtab = Swig_symbol_setscope(Getattr(am,"symtab"));
		   }
		   current_class = 0;
		 } else {
		   /* Previous typedef class definition.  Use its symbol table.
		      Deprecated, just the real name should be used. 
		      Note that %extend before the class typedef never worked, only %extend after the class typdef. */
		   prev_symtab = Swig_symbol_setscope(Getattr(cls, "symtab"));
		   current_class = cls;
		   SWIG_WARN_NODE_BEGIN(cls);
		   Swig_warning(WARN_PARSE_EXTEND_NAME, cparse_file, cparse_line, "Deprecated %%extend name used - the %s name '%s' should be used instead of the typedef name '%s'.\n", Getattr(cls, "kind"), SwigType_namestr(Getattr(cls, "name")), (yyvsp[(3) - (4)].str));
		   SWIG_WARN_NODE_END(cls);
		 }
	       } else {
		 /* Previous class definition.  Use its symbol table */
		 prev_symtab = Swig_symbol_setscope(Getattr(cls,"symtab"));
		 current_class = cls;
	       }
	       Classprefix = NewString((yyvsp[(3) - (4)].str));
	       Namespaceprefix= Swig_symbol_qualifiedscopename(0);
	       Delete(clsname);
	     }
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 1616 "parser.y"
    {
               String *clsname;
	       extendmode = 0;
               (yyval.node) = new_node("extend");
	       Setattr((yyval.node),"symtab",Swig_symbol_popscope());
	       if (prev_symtab) {
		 Swig_symbol_setscope(prev_symtab);
	       }
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
               clsname = make_class_name((yyvsp[(3) - (7)].str));
	       Setattr((yyval.node),"name",clsname);

	       mark_nodes_as_extend((yyvsp[(6) - (7)].node));
	       if (current_class) {
		 /* We add the extension to the previously defined class */
		 appendChild((yyval.node),(yyvsp[(6) - (7)].node));
		 appendChild(current_class,(yyval.node));
	       } else {
		 /* We store the extensions in the extensions hash */
		 Node *am = Getattr(Swig_extend_hash(),clsname);
		 if (am) {
		   /* Append the members to the previous extend methods */
		   appendChild(am,(yyvsp[(6) - (7)].node));
		 } else {
		   appendChild((yyval.node),(yyvsp[(6) - (7)].node));
		   Setattr(Swig_extend_hash(),clsname,(yyval.node));
		 }
	       }
	       current_class = 0;
	       Delete(Classprefix);
	       Delete(clsname);
	       Classprefix = 0;
	       prev_symtab = 0;
	       (yyval.node) = 0;

	     }
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 1658 "parser.y"
    {
                    (yyval.node) = new_node("apply");
                    Setattr((yyval.node),"pattern",Getattr((yyvsp[(2) - (5)].p),"pattern"));
		    appendChild((yyval.node),(yyvsp[(4) - (5)].p));
               }
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 1668 "parser.y"
    {
		 (yyval.node) = new_node("clear");
		 appendChild((yyval.node),(yyvsp[(2) - (3)].p));
               }
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 1679 "parser.y"
    {
		   if (((yyvsp[(4) - (5)].dtype).type != T_ERROR) && ((yyvsp[(4) - (5)].dtype).type != T_SYMBOL)) {
		     SwigType *type = NewSwigType((yyvsp[(4) - (5)].dtype).type);
		     (yyval.node) = new_node("constant");
		     Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
		     Setattr((yyval.node),"type",type);
		     Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
		     if ((yyvsp[(4) - (5)].dtype).rawval) Setattr((yyval.node),"rawval", (yyvsp[(4) - (5)].dtype).rawval);
		     Setattr((yyval.node),"storage","%constant");
		     SetFlag((yyval.node),"feature:immutable");
		     add_symbols((yyval.node));
		     Delete(type);
		   } else {
		     if ((yyvsp[(4) - (5)].dtype).type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value (ignored)\n");
		     }
		     (yyval.node) = 0;
		   }

	       }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 1700 "parser.y"
    {
		 if (((yyvsp[(4) - (5)].dtype).type != T_ERROR) && ((yyvsp[(4) - (5)].dtype).type != T_SYMBOL)) {
		   SwigType_push((yyvsp[(2) - (5)].type),(yyvsp[(3) - (5)].decl).type);
		   /* Sneaky callback function trick */
		   if (SwigType_isfunction((yyvsp[(2) - (5)].type))) {
		     SwigType_add_pointer((yyvsp[(2) - (5)].type));
		   }
		   (yyval.node) = new_node("constant");
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].decl).id);
		   Setattr((yyval.node),"type",(yyvsp[(2) - (5)].type));
		   Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
		   if ((yyvsp[(4) - (5)].dtype).rawval) Setattr((yyval.node),"rawval", (yyvsp[(4) - (5)].dtype).rawval);
		   Setattr((yyval.node),"storage","%constant");
		   SetFlag((yyval.node),"feature:immutable");
		   add_symbols((yyval.node));
		 } else {
		     if ((yyvsp[(4) - (5)].dtype).type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value\n");
		     }
		   (yyval.node) = 0;
		 }
               }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 1722 "parser.y"
    {
		 Swig_warning(WARN_PARSE_BAD_VALUE,cparse_file,cparse_line,"Bad constant value (ignored).\n");
		 (yyval.node) = 0;
	       }
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 1733 "parser.y"
    {
		 char temp[64];
		 Replace((yyvsp[(2) - (2)].str),"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace((yyvsp[(2) - (2)].str),"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", (yyvsp[(2) - (2)].str));
		 Delete((yyvsp[(2) - (2)].str));
                 (yyval.node) = 0;
	       }
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 1742 "parser.y"
    {
		 char temp[64];
		 String *s = NewString((yyvsp[(2) - (2)].id));
		 Replace(s,"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace(s,"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", s);
		 Delete(s);
                 (yyval.node) = 0;
               }
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 1761 "parser.y"
    {
                    skip_balanced('{','}');
		    (yyval.node) = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       }
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 1767 "parser.y"
    {
                    skip_balanced('{','}');
		    (yyval.node) = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               }
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 1773 "parser.y"
    {
		 (yyval.node) = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 1778 "parser.y"
    {
		 (yyval.node) = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 1785 "parser.y"
    {		 
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"value",(yyvsp[(1) - (4)].id));
		 Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (4)].p),"type"));
               }
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 1792 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"value",(yyvsp[(1) - (1)].id));
              }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 1796 "parser.y"
    {
                (yyval.node) = (yyvsp[(1) - (1)].node);
              }
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 1809 "parser.y"
    {
                   Hash *p = (yyvsp[(5) - (7)].node);
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (7)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (7)].node),"type"));
		   Setattr((yyval.node),"section",Getattr(p,"name"));
		   Setattr((yyval.node),"kwargs",nextSibling(p));
		   Setattr((yyval.node),"code",(yyvsp[(7) - (7)].str));
                 }
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 1818 "parser.y"
    {
		   Hash *p = (yyvsp[(5) - (7)].node);
		   String *code;
                   skip_balanced('{','}');
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (7)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (7)].node),"type"));
		   Setattr((yyval.node),"section",Getattr(p,"name"));
		   Setattr((yyval.node),"kwargs",nextSibling(p));
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   code = Copy(scanner_ccode);
		   Setattr((yyval.node),"code",code);
		   Delete(code);
                 }
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 1833 "parser.y"
    {
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (5)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (5)].node),"type"));
		   Setattr((yyval.node),"emitonly","1");
		 }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 1846 "parser.y"
    {
                     (yyvsp[(1) - (4)].loc).filename = Copy(cparse_file);
		     (yyvsp[(1) - (4)].loc).line = cparse_line;
		     scanner_set_location(NewString((yyvsp[(3) - (4)].id)),1);
                     if ((yyvsp[(2) - (4)].node)) { 
		       String *maininput = Getattr((yyvsp[(2) - (4)].node), "maininput");
		       if (maininput)
		         scanner_set_main_input_file(NewString(maininput));
		     }
               }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 1855 "parser.y"
    {
                     String *mname = 0;
                     (yyval.node) = (yyvsp[(6) - (7)].node);
		     scanner_set_location((yyvsp[(1) - (7)].loc).filename,(yyvsp[(1) - (7)].loc).line+1);
		     if (strcmp((yyvsp[(1) - (7)].loc).type,"include") == 0) set_nodeType((yyval.node),"include");
		     if (strcmp((yyvsp[(1) - (7)].loc).type,"import") == 0) {
		       mname = (yyvsp[(2) - (7)].node) ? Getattr((yyvsp[(2) - (7)].node),"module") : 0;
		       set_nodeType((yyval.node),"import");
		       if (import_mode) --import_mode;
		     }
		     
		     Setattr((yyval.node),"name",(yyvsp[(3) - (7)].id));
		     /* Search for the module (if any) */
		     {
			 Node *n = firstChild((yyval.node));
			 while (n) {
			     if (Strcmp(nodeType(n),"module") == 0) {
			         if (mname) {
				   Setattr(n,"name", mname);
				   mname = 0;
				 }
				 Setattr((yyval.node),"module",Getattr(n,"name"));
				 break;
			     }
			     n = nextSibling(n);
			 }
			 if (mname) {
			   /* There is no module node in the import
			      node, ie, you imported a .h file
			      directly.  We are forced then to create
			      a new import node with a module node.
			   */			      
			   Node *nint = new_node("import");
			   Node *mnode = new_node("module");
			   Setattr(mnode,"name", mname);
                           Setattr(mnode,"options",(yyvsp[(2) - (7)].node));
			   appendChild(nint,mnode);
			   Delete(mnode);
			   appendChild(nint,firstChild((yyval.node)));
			   (yyval.node) = nint;
			   Setattr((yyval.node),"module",mname);
			 }
		     }
		     Setattr((yyval.node),"options",(yyvsp[(2) - (7)].node));
               }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 1902 "parser.y"
    { (yyval.loc).type = "include"; }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 1903 "parser.y"
    { (yyval.loc).type = "import"; ++import_mode;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 1910 "parser.y"
    {
                 String *cpps;
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "%%inline directive inside a namespace is disallowed.\n");
		   (yyval.node) = 0;
		 } else {
		   (yyval.node) = new_node("insert");
		   Setattr((yyval.node),"code",(yyvsp[(2) - (2)].str));
		   /* Need to run through the preprocessor */
		   Seek((yyvsp[(2) - (2)].str),0,SEEK_SET);
		   Setline((yyvsp[(2) - (2)].str),cparse_start_line);
		   Setfile((yyvsp[(2) - (2)].str),cparse_file);
		   cpps = Preprocessor_parse((yyvsp[(2) - (2)].str));
		   start_inline(Char(cpps), cparse_start_line);
		   Delete((yyvsp[(2) - (2)].str));
		   Delete(cpps);
		 }
		 
	       }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 1929 "parser.y"
    {
                 String *cpps;
		 int start_line = cparse_line;
		 skip_balanced('{','}');
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "%%inline directive inside a namespace is disallowed.\n");
		   
		   (yyval.node) = 0;
		 } else {
		   String *code;
                   (yyval.node) = new_node("insert");
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   code = Copy(scanner_ccode);
		   Setattr((yyval.node),"code", code);
		   Delete(code);		   
		   cpps=Copy(scanner_ccode);
		   start_inline(Char(cpps), start_line);
		   Delete(cpps);
		 }
               }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 1960 "parser.y"
    {
                 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"code",(yyvsp[(1) - (1)].str));
	       }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 1964 "parser.y"
    {
		 String *code = NewStringEmpty();
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"code",code);
		 if (Swig_insert_file((yyvsp[(5) - (5)].id),code) < 0) {
		   Swig_error(cparse_file, cparse_line, "Couldn't find '%s'.\n", (yyvsp[(5) - (5)].id));
		   (yyval.node) = 0;
		 } 
               }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 1974 "parser.y"
    {
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"code",(yyvsp[(5) - (5)].str));
               }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 1979 "parser.y"
    {
		 String *code;
                 skip_balanced('{','}');
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Delitem(scanner_ccode,0);
		 Delitem(scanner_ccode,DOH_END);
		 code = Copy(scanner_ccode);
		 Setattr((yyval.node),"code", code);
		 Delete(code);
	       }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 1997 "parser.y"
    {
                 (yyval.node) = new_node("module");
		 if ((yyvsp[(2) - (3)].node)) {
		   Setattr((yyval.node),"options",(yyvsp[(2) - (3)].node));
		   if (Getattr((yyvsp[(2) - (3)].node),"directors")) {
		     Wrapper_director_mode_set(1);
		     if (!cparse_cplusplus) {
		       Swig_error(cparse_file, cparse_line, "Directors are not supported for C code and require the -c++ option\n");
		     }
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"dirprot")) {
		     Wrapper_director_protected_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"allprotected")) {
		     Wrapper_all_protected_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"templatereduce")) {
		     template_reduce = 1;
		   }
		   if (Getattr((yyvsp[(2) - (3)].node),"notemplatereduce")) {
		     template_reduce = 0;
		   }
		 }
		 if (!ModuleName) ModuleName = NewString((yyvsp[(3) - (3)].id));
		 if (!import_mode) {
		   /* first module included, we apply global
		      ModuleName, which can be modify by -module */
		   String *mname = Copy(ModuleName);
		   Setattr((yyval.node),"name",mname);
		   Delete(mname);
		 } else { 
		   /* import mode, we just pass the idstring */
		   Setattr((yyval.node),"name",(yyvsp[(3) - (3)].id));   
		 }		 
		 if (!module_node) module_node = (yyval.node);
	       }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 2040 "parser.y"
    {
                 Swig_warning(WARN_DEPRECATED_NAME,cparse_file,cparse_line, "%%name is deprecated.  Use %%rename instead.\n");
		 Delete(yyrename);
                 yyrename = NewString((yyvsp[(3) - (4)].id));
		 (yyval.node) = 0;
               }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 2046 "parser.y"
    {
		 Swig_warning(WARN_DEPRECATED_NAME,cparse_file,cparse_line, "%%name is deprecated.  Use %%rename instead.\n");
		 (yyval.node) = 0;
		 Swig_error(cparse_file,cparse_line,"Missing argument to %%name directive.\n");
	       }
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 2059 "parser.y"
    {
                 (yyval.node) = new_node("native");
		 Setattr((yyval.node),"name",(yyvsp[(3) - (7)].id));
		 Setattr((yyval.node),"wrap:name",(yyvsp[(6) - (7)].id));
	         add_symbols((yyval.node));
	       }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 2065 "parser.y"
    {
		 if (!SwigType_isfunction((yyvsp[(7) - (8)].decl).type)) {
		   Swig_error(cparse_file,cparse_line,"%%native declaration '%s' is not a function.\n", (yyvsp[(7) - (8)].decl).id);
		   (yyval.node) = 0;
		 } else {
		     Delete(SwigType_pop_function((yyvsp[(7) - (8)].decl).type));
		     /* Need check for function here */
		     SwigType_push((yyvsp[(6) - (8)].type),(yyvsp[(7) - (8)].decl).type);
		     (yyval.node) = new_node("native");
	             Setattr((yyval.node),"name",(yyvsp[(3) - (8)].id));
		     Setattr((yyval.node),"wrap:name",(yyvsp[(7) - (8)].decl).id);
		     Setattr((yyval.node),"type",(yyvsp[(6) - (8)].type));
		     Setattr((yyval.node),"parms",(yyvsp[(7) - (8)].decl).parms);
		     Setattr((yyval.node),"decl",(yyvsp[(7) - (8)].decl).type);
		 }
	         add_symbols((yyval.node));
	       }
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 2091 "parser.y"
    {
                 (yyval.node) = new_node("pragma");
		 Setattr((yyval.node),"lang",(yyvsp[(2) - (5)].id));
		 Setattr((yyval.node),"name",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"value",(yyvsp[(5) - (5)].str));
	       }
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 2097 "parser.y"
    {
		(yyval.node) = new_node("pragma");
		Setattr((yyval.node),"lang",(yyvsp[(2) - (3)].id));
		Setattr((yyval.node),"name",(yyvsp[(3) - (3)].id));
	      }
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 2104 "parser.y"
    { (yyval.str) = NewString((yyvsp[(1) - (1)].id)); }
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 2105 "parser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 2108 "parser.y"
    { (yyval.id) = (yyvsp[(2) - (3)].id); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 2109 "parser.y"
    { (yyval.id) = (char *) "swig"; }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 2116 "parser.y"
    {
                SwigType *t = (yyvsp[(2) - (4)].decl).type;
		Hash *kws = NewHash();
		String *fixname;
		fixname = feature_identifier_fix((yyvsp[(2) - (4)].decl).id);
		Setattr(kws,"name",(yyvsp[(3) - (4)].id));
		if (!Len(t)) t = 0;
		/* Special declarator check */
		if (t) {
		  if (SwigType_isfunction(t)) {
		    SwigType *decl = SwigType_pop_function(t);
		    if (SwigType_ispointer(t)) {
		      String *nname = NewStringf("*%s",fixname);
		      if ((yyvsp[(1) - (4)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix, nname,decl,kws,(yyvsp[(2) - (4)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,nname,decl,kws);
		      }
		      Delete(nname);
		    } else {
		      if ((yyvsp[(1) - (4)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix,(fixname),decl,kws,(yyvsp[(2) - (4)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,(fixname),decl,kws);
		      }
		    }
		    Delete(decl);
		  } else if (SwigType_ispointer(t)) {
		    String *nname = NewStringf("*%s",fixname);
		    if ((yyvsp[(1) - (4)].intvalue)) {
		      Swig_name_rename_add(Namespaceprefix,(nname),0,kws,(yyvsp[(2) - (4)].decl).parms);
		    } else {
		      Swig_name_namewarn_add(Namespaceprefix,(nname),0,kws);
		    }
		    Delete(nname);
		  }
		} else {
		  if ((yyvsp[(1) - (4)].intvalue)) {
		    Swig_name_rename_add(Namespaceprefix,(fixname),0,kws,(yyvsp[(2) - (4)].decl).parms);
		  } else {
		    Swig_name_namewarn_add(Namespaceprefix,(fixname),0,kws);
		  }
		}
                (yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 2162 "parser.y"
    {
		String *fixname;
		Hash *kws = (yyvsp[(3) - (7)].node);
		SwigType *t = (yyvsp[(5) - (7)].decl).type;
		fixname = feature_identifier_fix((yyvsp[(5) - (7)].decl).id);
		if (!Len(t)) t = 0;
		/* Special declarator check */
		if (t) {
		  if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push(t,(yyvsp[(6) - (7)].dtype).qualifier);
		  if (SwigType_isfunction(t)) {
		    SwigType *decl = SwigType_pop_function(t);
		    if (SwigType_ispointer(t)) {
		      String *nname = NewStringf("*%s",fixname);
		      if ((yyvsp[(1) - (7)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix, nname,decl,kws,(yyvsp[(5) - (7)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,nname,decl,kws);
		      }
		      Delete(nname);
		    } else {
		      if ((yyvsp[(1) - (7)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix,(fixname),decl,kws,(yyvsp[(5) - (7)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,(fixname),decl,kws);
		      }
		    }
		    Delete(decl);
		  } else if (SwigType_ispointer(t)) {
		    String *nname = NewStringf("*%s",fixname);
		    if ((yyvsp[(1) - (7)].intvalue)) {
		      Swig_name_rename_add(Namespaceprefix,(nname),0,kws,(yyvsp[(5) - (7)].decl).parms);
		    } else {
		      Swig_name_namewarn_add(Namespaceprefix,(nname),0,kws);
		    }
		    Delete(nname);
		  }
		} else {
		  if ((yyvsp[(1) - (7)].intvalue)) {
		    Swig_name_rename_add(Namespaceprefix,(fixname),0,kws,(yyvsp[(5) - (7)].decl).parms);
		  } else {
		    Swig_name_namewarn_add(Namespaceprefix,(fixname),0,kws);
		  }
		}
                (yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 2208 "parser.y"
    {
		if ((yyvsp[(1) - (6)].intvalue)) {
		  Swig_name_rename_add(Namespaceprefix,(yyvsp[(5) - (6)].id),0,(yyvsp[(3) - (6)].node),0);
		} else {
		  Swig_name_namewarn_add(Namespaceprefix,(yyvsp[(5) - (6)].id),0,(yyvsp[(3) - (6)].node));
		}
		(yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 2219 "parser.y"
    {
		    (yyval.intvalue) = 1;
                }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 2222 "parser.y"
    {
                    (yyval.intvalue) = 0;
                }
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 2249 "parser.y"
    {
                    String *val = (yyvsp[(7) - (7)].str) ? NewString((yyvsp[(7) - (7)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (7)].id), val, 0, (yyvsp[(5) - (7)].decl).id, (yyvsp[(5) - (7)].decl).type, (yyvsp[(5) - (7)].decl).parms, (yyvsp[(6) - (7)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 2255 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (9)].id)) ? NewString((yyvsp[(5) - (9)].id)) : 0;
                    new_feature((yyvsp[(3) - (9)].id), val, 0, (yyvsp[(7) - (9)].decl).id, (yyvsp[(7) - (9)].decl).type, (yyvsp[(7) - (9)].decl).parms, (yyvsp[(8) - (9)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 2261 "parser.y"
    {
                    String *val = (yyvsp[(8) - (8)].str) ? NewString((yyvsp[(8) - (8)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (8)].id), val, (yyvsp[(4) - (8)].node), (yyvsp[(6) - (8)].decl).id, (yyvsp[(6) - (8)].decl).type, (yyvsp[(6) - (8)].decl).parms, (yyvsp[(7) - (8)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 2267 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (10)].id)) ? NewString((yyvsp[(5) - (10)].id)) : 0;
                    new_feature((yyvsp[(3) - (10)].id), val, (yyvsp[(6) - (10)].node), (yyvsp[(8) - (10)].decl).id, (yyvsp[(8) - (10)].decl).type, (yyvsp[(8) - (10)].decl).parms, (yyvsp[(9) - (10)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 2275 "parser.y"
    {
                    String *val = (yyvsp[(5) - (5)].str) ? NewString((yyvsp[(5) - (5)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (5)].id), val, 0, 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 2281 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (7)].id)) ? NewString((yyvsp[(5) - (7)].id)) : 0;
                    new_feature((yyvsp[(3) - (7)].id), val, 0, 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 2287 "parser.y"
    {
                    String *val = (yyvsp[(6) - (6)].str) ? NewString((yyvsp[(6) - (6)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (6)].id), val, (yyvsp[(4) - (6)].node), 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 2293 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (8)].id)) ? NewString((yyvsp[(5) - (8)].id)) : 0;
                    new_feature((yyvsp[(3) - (8)].id), val, (yyvsp[(6) - (8)].node), 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 2301 "parser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 2302 "parser.y"
    { (yyval.str) = 0; }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 2303 "parser.y"
    { (yyval.str) = (yyvsp[(3) - (5)].pl); }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 2306 "parser.y"
    {
		  (yyval.node) = NewHash();
		  Setattr((yyval.node),"name",(yyvsp[(2) - (4)].id));
		  Setattr((yyval.node),"value",(yyvsp[(4) - (4)].id));
                }
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 2311 "parser.y"
    {
		  (yyval.node) = NewHash();
		  Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
		  Setattr((yyval.node),"value",(yyvsp[(4) - (5)].id));
                  set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
                }
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 2321 "parser.y"
    {
                 Parm *val;
		 String *name;
		 SwigType *t;
		 if (Namespaceprefix) name = NewStringf("%s::%s", Namespaceprefix, (yyvsp[(5) - (7)].decl).id);
		 else name = NewString((yyvsp[(5) - (7)].decl).id);
		 val = (yyvsp[(3) - (7)].pl);
		 if ((yyvsp[(5) - (7)].decl).parms) {
		   Setmeta(val,"parms",(yyvsp[(5) - (7)].decl).parms);
		 }
		 t = (yyvsp[(5) - (7)].decl).type;
		 if (!Len(t)) t = 0;
		 if (t) {
		   if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push(t,(yyvsp[(6) - (7)].dtype).qualifier);
		   if (SwigType_isfunction(t)) {
		     SwigType *decl = SwigType_pop_function(t);
		     if (SwigType_ispointer(t)) {
		       String *nname = NewStringf("*%s",name);
		       Swig_feature_set(Swig_cparse_features(), nname, decl, "feature:varargs", val, 0);
		       Delete(nname);
		     } else {
		       Swig_feature_set(Swig_cparse_features(), name, decl, "feature:varargs", val, 0);
		     }
		     Delete(decl);
		   } else if (SwigType_ispointer(t)) {
		     String *nname = NewStringf("*%s",name);
		     Swig_feature_set(Swig_cparse_features(),nname,0,"feature:varargs",val, 0);
		     Delete(nname);
		   }
		 } else {
		   Swig_feature_set(Swig_cparse_features(),name,0,"feature:varargs",val, 0);
		 }
		 Delete(name);
		 (yyval.node) = 0;
              }
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 2357 "parser.y"
    { (yyval.pl) = (yyvsp[(1) - (1)].pl); }
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 2358 "parser.y"
    { 
		  int i;
		  int n;
		  Parm *p;
		  n = atoi(Char((yyvsp[(1) - (3)].dtype).val));
		  if (n <= 0) {
		    Swig_error(cparse_file, cparse_line,"Argument count in %%varargs must be positive.\n");
		    (yyval.pl) = 0;
		  } else {
		    String *name = Getattr((yyvsp[(3) - (3)].p), "name");
		    (yyval.pl) = Copy((yyvsp[(3) - (3)].p));
		    if (name)
		      Setattr((yyval.pl), "name", NewStringf("%s%d", name, n));
		    for (i = 1; i < n; i++) {
		      p = Copy((yyvsp[(3) - (3)].p));
		      name = Getattr(p, "name");
		      if (name)
		        Setattr(p, "name", NewStringf("%s%d", name, n-i));
		      set_nextSibling(p,(yyval.pl));
		      Delete((yyval.pl));
		      (yyval.pl) = p;
		    }
		  }
                }
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 2393 "parser.y"
    {
		   (yyval.node) = 0;
		   if ((yyvsp[(3) - (6)].tmap).method) {
		     String *code = 0;
		     (yyval.node) = new_node("typemap");
		     Setattr((yyval.node),"method",(yyvsp[(3) - (6)].tmap).method);
		     if ((yyvsp[(3) - (6)].tmap).kwargs) {
		       ParmList *kw = (yyvsp[(3) - (6)].tmap).kwargs;
                       code = remove_block(kw, (yyvsp[(6) - (6)].str));
		       Setattr((yyval.node),"kwargs", (yyvsp[(3) - (6)].tmap).kwargs);
		     }
		     code = code ? code : NewString((yyvsp[(6) - (6)].str));
		     Setattr((yyval.node),"code", code);
		     Delete(code);
		     appendChild((yyval.node),(yyvsp[(5) - (6)].p));
		   }
	       }
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 2410 "parser.y"
    {
		 (yyval.node) = 0;
		 if ((yyvsp[(3) - (6)].tmap).method) {
		   (yyval.node) = new_node("typemap");
		   Setattr((yyval.node),"method",(yyvsp[(3) - (6)].tmap).method);
		   appendChild((yyval.node),(yyvsp[(5) - (6)].p));
		 }
	       }
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 2418 "parser.y"
    {
		   (yyval.node) = 0;
		   if ((yyvsp[(3) - (8)].tmap).method) {
		     (yyval.node) = new_node("typemapcopy");
		     Setattr((yyval.node),"method",(yyvsp[(3) - (8)].tmap).method);
		     Setattr((yyval.node),"pattern", Getattr((yyvsp[(7) - (8)].p),"pattern"));
		     appendChild((yyval.node),(yyvsp[(5) - (8)].p));
		   }
	       }
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 2431 "parser.y"
    {
		 Hash *p;
		 String *name;
		 p = nextSibling((yyvsp[(1) - (1)].node));
		 if (p && (!Getattr(p,"value"))) {
 		   /* this is the deprecated two argument typemap form */
 		   Swig_warning(WARN_DEPRECATED_TYPEMAP_LANG,cparse_file, cparse_line,
				"Specifying the language name in %%typemap is deprecated - use #ifdef SWIG<LANG> instead.\n");
		   /* two argument typemap form */
		   name = Getattr((yyvsp[(1) - (1)].node),"name");
		   if (!name || (Strcmp(name,typemap_lang))) {
		     (yyval.tmap).method = 0;
		     (yyval.tmap).kwargs = 0;
		   } else {
		     (yyval.tmap).method = Getattr(p,"name");
		     (yyval.tmap).kwargs = nextSibling(p);
		   }
		 } else {
		   /* one-argument typemap-form */
		   (yyval.tmap).method = Getattr((yyvsp[(1) - (1)].node),"name");
		   (yyval.tmap).kwargs = p;
		 }
                }
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 2456 "parser.y"
    {
                 (yyval.p) = (yyvsp[(1) - (2)].p);
		 set_nextSibling((yyval.p),(yyvsp[(2) - (2)].p));
		}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 2462 "parser.y"
    {
                 (yyval.p) = (yyvsp[(2) - (3)].p);
		 set_nextSibling((yyval.p),(yyvsp[(3) - (3)].p));
                }
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 2466 "parser.y"
    { (yyval.p) = 0;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 2469 "parser.y"
    {
                  Parm *parm;
		  SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		  (yyval.p) = new_node("typemapitem");
		  parm = NewParmWithoutFileLineInfo((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).id);
		  Setattr((yyval.p),"pattern",parm);
		  Setattr((yyval.p),"parms", (yyvsp[(2) - (2)].decl).parms);
		  Delete(parm);
		  /*		  $$ = NewParmWithoutFileLineInfo($1,$2.id);
				  Setattr($$,"parms",$2.parms); */
                }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 2480 "parser.y"
    {
                  (yyval.p) = new_node("typemapitem");
		  Setattr((yyval.p),"pattern",(yyvsp[(2) - (3)].pl));
		  /*		  Setattr($$,"multitype",$2); */
               }
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 2485 "parser.y"
    {
		 (yyval.p) = new_node("typemapitem");
		 Setattr((yyval.p),"pattern", (yyvsp[(2) - (6)].pl));
		 /*                 Setattr($$,"multitype",$2); */
		 Setattr((yyval.p),"parms",(yyvsp[(5) - (6)].pl));
               }
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 2498 "parser.y"
    {
                   (yyval.node) = new_node("types");
		   Setattr((yyval.node),"parms",(yyvsp[(3) - (5)].pl));
                   if ((yyvsp[(5) - (5)].str))
		     Setattr((yyval.node),"convcode",NewString((yyvsp[(5) - (5)].str)));
               }
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 2510 "parser.y"
    {
                  Parm *p, *tp;
		  Node *n;
		  Node *outer_class = currentOuterClass;
		  Symtab *tscope = 0;
		  int     specialized = 0;
		  int     variadic = 0;

		  (yyval.node) = 0;

		  tscope = Swig_symbol_current();          /* Get the current scope */

		  /* If the class name is qualified, we need to create or lookup namespace entries */
		  if (!inclass) {
		    (yyvsp[(5) - (9)].str) = resolve_create_node_scope((yyvsp[(5) - (9)].str));
		  }
		  if (nscope_inner && Strcmp(nodeType(nscope_inner), "class") == 0) {
		    outer_class	= nscope_inner;
		  }

		  /*
		    We use the new namespace entry 'nscope' only to
		    emit the template node. The template parameters are
		    resolved in the current 'tscope'.

		    This is closer to the C++ (typedef) behavior.
		  */
		  n = Swig_cparse_template_locate((yyvsp[(5) - (9)].str),(yyvsp[(7) - (9)].p),tscope);

		  /* Patch the argument types to respect namespaces */
		  p = (yyvsp[(7) - (9)].p);
		  while (p) {
		    SwigType *value = Getattr(p,"value");
		    if (!value) {
		      SwigType *ty = Getattr(p,"type");
		      if (ty) {
			SwigType *rty = 0;
			int reduce = template_reduce;
			if (reduce || !SwigType_ispointer(ty)) {
			  rty = Swig_symbol_typedef_reduce(ty,tscope);
			  if (!reduce) reduce = SwigType_ispointer(rty);
			}
			ty = reduce ? Swig_symbol_type_qualify(rty,tscope) : Swig_symbol_type_qualify(ty,tscope);
			Setattr(p,"type",ty);
			Delete(ty);
			Delete(rty);
		      }
		    } else {
		      value = Swig_symbol_type_qualify(value,tscope);
		      Setattr(p,"value",value);
		      Delete(value);
		    }

		    p = nextSibling(p);
		  }

		  /* Look for the template */
		  {
                    Node *nn = n;
                    Node *linklistend = 0;
                    while (nn) {
                      Node *templnode = 0;
                      if (Strcmp(nodeType(nn),"template") == 0) {
                        int nnisclass = (Strcmp(Getattr(nn,"templatetype"),"class") == 0); /* if not a templated class it is a templated function */
                        Parm *tparms = Getattr(nn,"templateparms");
                        if (!tparms) {
                          specialized = 1;
                        } else if (Getattr(tparms,"variadic") && strncmp(Char(Getattr(tparms,"variadic")), "1", 1)==0) {
                          variadic = 1;
                        }
                        if (nnisclass && !variadic && !specialized && (ParmList_len((yyvsp[(7) - (9)].p)) > ParmList_len(tparms))) {
                          Swig_error(cparse_file, cparse_line, "Too many template parameters. Maximum of %d.\n", ParmList_len(tparms));
                        } else if (nnisclass && !specialized && ((ParmList_len((yyvsp[(7) - (9)].p)) < (ParmList_numrequired(tparms) - (variadic?1:0))))) { /* Variadic parameter is optional */
                          Swig_error(cparse_file, cparse_line, "Not enough template parameters specified. %d required.\n", (ParmList_numrequired(tparms)-(variadic?1:0)) );
                        } else if (!nnisclass && ((ParmList_len((yyvsp[(7) - (9)].p)) != ParmList_len(tparms)))) {
                          /* must be an overloaded templated method - ignore it as it is overloaded with a different number of template parameters */
                          nn = Getattr(nn,"sym:nextSibling"); /* repeat for overloaded templated functions */
                          continue;
                        } else {
			  String *tname = Copy((yyvsp[(5) - (9)].str));
                          int def_supplied = 0;
                          /* Expand the template */
			  Node *templ = Swig_symbol_clookup((yyvsp[(5) - (9)].str),0);
			  Parm *targs = templ ? Getattr(templ,"templateparms") : 0;

                          ParmList *temparms;
                          if (specialized) temparms = CopyParmList((yyvsp[(7) - (9)].p));
                          else temparms = CopyParmList(tparms);

                          /* Create typedef's and arguments */
                          p = (yyvsp[(7) - (9)].p);
                          tp = temparms;
                          if (!p && ParmList_len(p) != ParmList_len(temparms)) {
                            /* we have no template parameters supplied in %template for a template that has default args*/
                            p = tp;
                            def_supplied = 1;
                          }

                          while (p) {
                            String *value = Getattr(p,"value");
                            if (def_supplied) {
                              Setattr(p,"default","1");
                            }
                            if (value) {
                              Setattr(tp,"value",value);
                            } else {
                              SwigType *ty = Getattr(p,"type");
                              if (ty) {
                                Setattr(tp,"type",ty);
                              }
                              Delattr(tp,"value");
                            }
			    /* fix default arg values */
			    if (targs) {
			      Parm *pi = temparms;
			      Parm *ti = targs;
			      String *tv = Getattr(tp,"value");
			      if (!tv) tv = Getattr(tp,"type");
			      while(pi != tp && ti && pi) {
				String *name = Getattr(ti,"name");
				String *value = Getattr(pi,"value");
				if (!value) value = Getattr(pi,"type");
				Replaceid(tv, name, value);
				pi = nextSibling(pi);
				ti = nextSibling(ti);
			      }
			    }
                            p = nextSibling(p);
                            tp = nextSibling(tp);
                            if (!p && tp) {
                              p = tp;
                              def_supplied = 1;
                            } else if (p && !tp) { /* Variadic template - tp < p */
			      SWIG_WARN_NODE_BEGIN(nn);
                              Swig_warning(WARN_CPP11_VARIADIC_TEMPLATE,cparse_file, cparse_line,"Only the first variadic template argument is currently supported.\n");
			      SWIG_WARN_NODE_END(nn);
                              break;
                            }
                          }

                          templnode = copy_node(nn);
			  update_nested_classes(templnode); /* update classes nested withing template */
                          /* We need to set the node name based on name used to instantiate */
                          Setattr(templnode,"name",tname);
			  Delete(tname);
                          if (!specialized) {
                            Delattr(templnode,"sym:typename");
                          } else {
                            Setattr(templnode,"sym:typename","1");
                          }
			  /* for now, nested %template is allowed only in the same scope as the template declaration */
                          if ((yyvsp[(3) - (9)].id) && !(nnisclass && ((outer_class && (outer_class != Getattr(nn, "nested:outer")))
			    ||(extendmode && current_class && (current_class != Getattr(nn, "nested:outer")))))) {
			    /*
			       Comment this out for 1.3.28. We need to
			       re-enable it later but first we need to
			       move %ignore from using %rename to use
			       %feature(ignore).

			       String *symname = Swig_name_make(templnode,0,$3,0,0);
			    */
			    String *symname = (yyvsp[(3) - (9)].id);
                            Swig_cparse_template_expand(templnode,symname,temparms,tscope);
                            Setattr(templnode,"sym:name",symname);
                          } else {
                            static int cnt = 0;
                            String *nname = NewStringf("__dummy_%d__", cnt++);
                            Swig_cparse_template_expand(templnode,nname,temparms,tscope);
                            Setattr(templnode,"sym:name",nname);
			    Delete(nname);
                            Setattr(templnode,"feature:onlychildren", "typemap,typemapitem,typemapcopy,typedef,types,fragment");
			    if ((yyvsp[(3) - (9)].id)) {
			      Swig_warning(WARN_PARSE_NESTED_TEMPLATE, cparse_file, cparse_line, "Named nested template instantiations not supported. Processing as if no name was given to %%template().\n");
			    }
                          }
                          Delattr(templnode,"templatetype");
                          Setattr(templnode,"template",nn);
                          Setfile(templnode,cparse_file);
                          Setline(templnode,cparse_line);
                          Delete(temparms);
			  if (outer_class && nnisclass) {
			    SetFlag(templnode, "nested");
			    Setattr(templnode, "nested:outer", outer_class);
			  }
                          add_symbols_copy(templnode);

                          if (Strcmp(nodeType(templnode),"class") == 0) {

                            /* Identify pure abstract methods */
                            Setattr(templnode,"abstracts", pure_abstracts(firstChild(templnode)));

                            /* Set up inheritance in symbol table */
                            {
                              Symtab  *csyms;
                              List *baselist = Getattr(templnode,"baselist");
                              csyms = Swig_symbol_current();
                              Swig_symbol_setscope(Getattr(templnode,"symtab"));
                              if (baselist) {
                                List *bases = Swig_make_inherit_list(Getattr(templnode,"name"),baselist, Namespaceprefix);
                                if (bases) {
                                  Iterator s;
                                  for (s = First(bases); s.item; s = Next(s)) {
                                    Symtab *st = Getattr(s.item,"symtab");
                                    if (st) {
				      Setfile(st,Getfile(s.item));
				      Setline(st,Getline(s.item));
                                      Swig_symbol_inherit(st);
                                    }
                                  }
				  Delete(bases);
                                }
                              }
                              Swig_symbol_setscope(csyms);
                            }

                            /* Merge in %extend methods for this class */

			    /* !!! This may be broken.  We may have to add the
			       %extend methods at the beginning of the class */
                            {
                              String *stmp = 0;
                              String *clsname;
                              Node *am;
                              if (Namespaceprefix) {
                                clsname = stmp = NewStringf("%s::%s", Namespaceprefix, Getattr(templnode,"name"));
                              } else {
                                clsname = Getattr(templnode,"name");
                              }
                              am = Getattr(Swig_extend_hash(),clsname);
                              if (am) {
                                Symtab *st = Swig_symbol_current();
                                Swig_symbol_setscope(Getattr(templnode,"symtab"));
                                /*			    Printf(stdout,"%s: %s %p %p\n", Getattr(templnode,"name"), clsname, Swig_symbol_current(), Getattr(templnode,"symtab")); */
                                Swig_extend_merge(templnode,am);
                                Swig_symbol_setscope(st);
				Swig_extend_append_previous(templnode,am);
                                Delattr(Swig_extend_hash(),clsname);
                              }
			      if (stmp) Delete(stmp);
                            }

                            /* Add to classes hash */
			    if (!classes)
			      classes = NewHash();

			    if (Namespaceprefix) {
			      String *temp = NewStringf("%s::%s", Namespaceprefix, Getattr(templnode,"name"));
			      Setattr(classes,temp,templnode);
			      Delete(temp);
			    } else {
			      String *qs = Swig_symbol_qualifiedscopename(templnode);
			      Setattr(classes, qs,templnode);
			      Delete(qs);
			    }
                          }
                        }

                        /* all the overloaded templated functions are added into a linked list */
                        if (nscope_inner) {
                          /* non-global namespace */
                          if (templnode) {
                            appendChild(nscope_inner,templnode);
			    Delete(templnode);
                            if (nscope) (yyval.node) = nscope;
                          }
                        } else {
                          /* global namespace */
                          if (!linklistend) {
                            (yyval.node) = templnode;
                          } else {
                            set_nextSibling(linklistend,templnode);
			    Delete(templnode);
                          }
                          linklistend = templnode;
                        }
                      }
                      nn = Getattr(nn,"sym:nextSibling"); /* repeat for overloaded templated functions. If a templated class there will never be a sibling. */
                    }
		  }
	          Swig_symbol_setscope(tscope);
		  Delete(Namespaceprefix);
		  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
                }
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 2800 "parser.y"
    {
		  Swig_warning(0,cparse_file, cparse_line,"%s\n", (yyvsp[(2) - (2)].id));
		  (yyval.node) = 0;
               }
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 2810 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node); 
                    if ((yyval.node)) {
   		      add_symbols((yyval.node));
                      default_arguments((yyval.node));
   	            }
                }
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 2817 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 2818 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 2822 "parser.y"
    {
		  if (Strcmp((yyvsp[(2) - (3)].id),"C") == 0) {
		    cparse_externc = 1;
		  }
		}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 2826 "parser.y"
    {
		  cparse_externc = 0;
		  if (Strcmp((yyvsp[(2) - (6)].id),"C") == 0) {
		    Node *n = firstChild((yyvsp[(5) - (6)].node));
		    (yyval.node) = new_node("extern");
		    Setattr((yyval.node),"name",(yyvsp[(2) - (6)].id));
		    appendChild((yyval.node),n);
		    while (n) {
		      SwigType *decl = Getattr(n,"decl");
		      if (SwigType_isfunction(decl) && !Equal(Getattr(n, "storage"), "typedef")) {
			Setattr(n,"storage","externc");
		      }
		      n = nextSibling(n);
		    }
		  } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\".\n", (yyvsp[(2) - (6)].id));
		    (yyval.node) = new_node("extern");
		    Setattr((yyval.node),"name",(yyvsp[(2) - (6)].id));
		    appendChild((yyval.node),firstChild((yyvsp[(5) - (6)].node)));
		  }
                }
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 2847 "parser.y"
    {
		  (yyval.node) = (yyvsp[(1) - (1)].node);
		  SWIG_WARN_NODE_BEGIN((yyval.node));
		  Swig_warning(WARN_CPP11_LAMBDA, cparse_file, cparse_line, "Lambda expressions and closures are not fully supported yet.\n");
		  SWIG_WARN_NODE_END((yyval.node));
		}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 2853 "parser.y"
    {
		  skip_decl();
		  (yyval.node) = new_node("using");
		  Setattr((yyval.node),"name",(yyvsp[(2) - (3)].str));
		  add_symbols((yyval.node));
		  SWIG_WARN_NODE_BEGIN((yyval.node));
		  Swig_warning(WARN_CPP11_ALIAS_DECLARATION, cparse_file, cparse_line, "The 'using' keyword in type aliasing is not fully supported yet.\n");
		  SWIG_WARN_NODE_END((yyval.node));

		  (yyval.node) = 0; /* TODO - ignored for now */
		}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 2864 "parser.y"
    {
		  skip_decl();
		  (yyval.node) = new_node("using");
		  Setattr((yyval.node),"uname",(yyvsp[(8) - (8)].id));
		  Setattr((yyval.node),"name",(yyvsp[(6) - (8)].str));
		  add_symbols((yyval.node));
		  SWIG_WARN_NODE_BEGIN((yyval.node));
		  Swig_warning(WARN_CPP11_ALIAS_TEMPLATE, cparse_file, cparse_line, "The 'using' keyword in template aliasing is not fully supported yet.\n");
		  SWIG_WARN_NODE_END((yyval.node));
		}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 2880 "parser.y"
    {
              (yyval.node) = new_node("cdecl");
	      if ((yyvsp[(4) - (5)].dtype).qualifier) SwigType_push((yyvsp[(3) - (5)].decl).type,(yyvsp[(4) - (5)].dtype).qualifier);
	      Setattr((yyval.node),"type",(yyvsp[(2) - (5)].type));
	      Setattr((yyval.node),"storage",(yyvsp[(1) - (5)].id));
	      Setattr((yyval.node),"name",(yyvsp[(3) - (5)].decl).id);
	      Setattr((yyval.node),"decl",(yyvsp[(3) - (5)].decl).type);
	      Setattr((yyval.node),"parms",(yyvsp[(3) - (5)].decl).parms);
	      Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
	      Setattr((yyval.node),"throws",(yyvsp[(4) - (5)].dtype).throws);
	      Setattr((yyval.node),"throw",(yyvsp[(4) - (5)].dtype).throwf);
	      Setattr((yyval.node),"noexcept",(yyvsp[(4) - (5)].dtype).nexcept);
	      if (!(yyvsp[(5) - (5)].node)) {
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
	      } else {
		Node *n = (yyvsp[(5) - (5)].node);
		/* Inherit attributes */
		while (n) {
		  String *type = Copy((yyvsp[(2) - (5)].type));
		  Setattr(n,"type",type);
		  Setattr(n,"storage",(yyvsp[(1) - (5)].id));
		  n = nextSibling(n);
		  Delete(type);
		}
	      }
	      if ((yyvsp[(4) - (5)].dtype).bitfield) {
		Setattr((yyval.node),"bitfield", (yyvsp[(4) - (5)].dtype).bitfield);
	      }

	      /* Look for "::" declarations (ignored) */
	      if (Strstr((yyvsp[(3) - (5)].decl).id,"::")) {
                /* This is a special case. If the scope name of the declaration exactly
                   matches that of the declaration, then we will allow it. Otherwise, delete. */
                String *p = Swig_scopename_prefix((yyvsp[(3) - (5)].decl).id);
		if (p) {
		  if ((Namespaceprefix && Strcmp(p,Namespaceprefix) == 0) ||
		      (inclass && Strcmp(p,Classprefix) == 0)) {
		    String *lstr = Swig_scopename_last((yyvsp[(3) - (5)].decl).id);
		    Setattr((yyval.node),"name",lstr);
		    Delete(lstr);
		    set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
		  } else {
		    Delete((yyval.node));
		    (yyval.node) = (yyvsp[(5) - (5)].node);
		  }
		  Delete(p);
		} else {
		  Delete((yyval.node));
		  (yyval.node) = (yyvsp[(5) - (5)].node);
		}
	      } else {
		set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
	      }
           }
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 2940 "parser.y"
    {
              (yyval.node) = new_node("cdecl");
	      if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push((yyvsp[(3) - (7)].decl).type,(yyvsp[(6) - (7)].dtype).qualifier);
	      Setattr((yyval.node),"type",(yyvsp[(5) - (7)].node));
	      Setattr((yyval.node),"storage",(yyvsp[(1) - (7)].id));
	      Setattr((yyval.node),"name",(yyvsp[(3) - (7)].decl).id);
	      Setattr((yyval.node),"decl",(yyvsp[(3) - (7)].decl).type);
	      Setattr((yyval.node),"parms",(yyvsp[(3) - (7)].decl).parms);
	      Setattr((yyval.node),"value",(yyvsp[(6) - (7)].dtype).val);
	      Setattr((yyval.node),"throws",(yyvsp[(6) - (7)].dtype).throws);
	      Setattr((yyval.node),"throw",(yyvsp[(6) - (7)].dtype).throwf);
	      Setattr((yyval.node),"noexcept",(yyvsp[(6) - (7)].dtype).nexcept);
	      if (!(yyvsp[(7) - (7)].node)) {
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
	      } else {
		Node *n = (yyvsp[(7) - (7)].node);
		while (n) {
		  String *type = Copy((yyvsp[(5) - (7)].node));
		  Setattr(n,"type",type);
		  Setattr(n,"storage",(yyvsp[(1) - (7)].id));
		  n = nextSibling(n);
		  Delete(type);
		}
	      }
	      if ((yyvsp[(6) - (7)].dtype).bitfield) {
		Setattr((yyval.node),"bitfield", (yyvsp[(6) - (7)].dtype).bitfield);
	      }

	      if (Strstr((yyvsp[(3) - (7)].decl).id,"::")) {
                String *p = Swig_scopename_prefix((yyvsp[(3) - (7)].decl).id);
		if (p) {
		  if ((Namespaceprefix && Strcmp(p,Namespaceprefix) == 0) ||
		      (inclass && Strcmp(p,Classprefix) == 0)) {
		    String *lstr = Swig_scopename_last((yyvsp[(3) - (7)].decl).id);
		    Setattr((yyval.node),"name",lstr);
		    Delete(lstr);
		    set_nextSibling((yyval.node),(yyvsp[(7) - (7)].node));
		  } else {
		    Delete((yyval.node));
		    (yyval.node) = (yyvsp[(7) - (7)].node);
		  }
		  Delete(p);
		} else {
		  Delete((yyval.node));
		  (yyval.node) = (yyvsp[(7) - (7)].node);
		}
	      } else {
		set_nextSibling((yyval.node),(yyvsp[(7) - (7)].node));
	      }
           }
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 2998 "parser.y"
    { 
                   (yyval.node) = 0;
                   Clear(scanner_ccode); 
               }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 3002 "parser.y"
    {
		 (yyval.node) = new_node("cdecl");
		 if ((yyvsp[(3) - (4)].dtype).qualifier) SwigType_push((yyvsp[(2) - (4)].decl).type,(yyvsp[(3) - (4)].dtype).qualifier);
		 Setattr((yyval.node),"name",(yyvsp[(2) - (4)].decl).id);
		 Setattr((yyval.node),"decl",(yyvsp[(2) - (4)].decl).type);
		 Setattr((yyval.node),"parms",(yyvsp[(2) - (4)].decl).parms);
		 Setattr((yyval.node),"value",(yyvsp[(3) - (4)].dtype).val);
		 Setattr((yyval.node),"throws",(yyvsp[(3) - (4)].dtype).throws);
		 Setattr((yyval.node),"throw",(yyvsp[(3) - (4)].dtype).throwf);
		 Setattr((yyval.node),"noexcept",(yyvsp[(3) - (4)].dtype).nexcept);
		 if ((yyvsp[(3) - (4)].dtype).bitfield) {
		   Setattr((yyval.node),"bitfield", (yyvsp[(3) - (4)].dtype).bitfield);
		 }
		 if (!(yyvsp[(4) - (4)].node)) {
		   if (Len(scanner_ccode)) {
		     String *code = Copy(scanner_ccode);
		     Setattr((yyval.node),"code",code);
		     Delete(code);
		   }
		 } else {
		   set_nextSibling((yyval.node),(yyvsp[(4) - (4)].node));
		 }
	       }
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 3025 "parser.y"
    { 
                   skip_balanced('{','}');
                   (yyval.node) = 0;
               }
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 3029 "parser.y"
    {
		   (yyval.node) = 0;
		   if (yychar == RPAREN) {
		       Swig_error(cparse_file, cparse_line, "Unexpected ')'.\n");
		   } else {
		       Swig_error(cparse_file, cparse_line, "Syntax error - possibly a missing semicolon.\n");
		   }
		   exit(1);
               }
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 3040 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(1) - (1)].dtype); 
                   (yyval.dtype).qualifier = 0;
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
		   (yyval.dtype).nexcept = 0;
              }
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 3047 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(2) - (2)].dtype); 
		   (yyval.dtype).qualifier = (yyvsp[(1) - (2)].str);
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
		   (yyval.dtype).nexcept = 0;
	      }
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 3054 "parser.y"
    { 
		   (yyval.dtype) = (yyvsp[(2) - (2)].dtype); 
                   (yyval.dtype).qualifier = 0;
		   (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
		   (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
		   (yyval.dtype).nexcept = (yyvsp[(1) - (2)].dtype).nexcept;
              }
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 3061 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(3) - (3)].dtype); 
                   (yyval.dtype).qualifier = (yyvsp[(1) - (3)].str);
		   (yyval.dtype).throws = (yyvsp[(2) - (3)].dtype).throws;
		   (yyval.dtype).throwf = (yyvsp[(2) - (3)].dtype).throwf;
		   (yyval.dtype).nexcept = (yyvsp[(2) - (3)].dtype).nexcept;
              }
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 3070 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 3071 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 3072 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 3073 "parser.y"
    { (yyval.node) = NewStringf("%s%s",(yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].id)); }
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 3074 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 3075 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].str); }
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 3076 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 3087 "parser.y"
    {
		  (yyval.node) = new_node("lambda");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (11)].str));
		  add_symbols((yyval.node));
	        }
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 3092 "parser.y"
    {
		  (yyval.node) = new_node("lambda");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (13)].str));
		  add_symbols((yyval.node));
		}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 3097 "parser.y"
    {
		  (yyval.node) = new_node("lambda");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (7)].str));
		  add_symbols((yyval.node));
		}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 3104 "parser.y"
    {
		  skip_balanced('[',']');
		  (yyval.node) = 0;
	        }
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 3110 "parser.y"
    {
		  skip_balanced('{','}');
		  (yyval.node) = 0;
		}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 3115 "parser.y"
    {
		  (yyval.pl) = 0;
		}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 3118 "parser.y"
    {
		  skip_balanced('(',')');
		}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 3120 "parser.y"
    {
		  (yyval.pl) = 0;
		}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 3131 "parser.y"
    {
		   (yyval.node) = (char *)"enum";
	      }
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 3134 "parser.y"
    {
		   (yyval.node) = (char *)"enum class";
	      }
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 3137 "parser.y"
    {
		   (yyval.node) = (char *)"enum struct";
	      }
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 3146 "parser.y"
    {
                   (yyval.node) = (yyvsp[(2) - (2)].type);
              }
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 3149 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 3156 "parser.y"
    {
		   SwigType *ty = 0;
		   int scopedenum = (yyvsp[(3) - (5)].id) && !Equal((yyvsp[(2) - (5)].node), "enum");
		   (yyval.node) = new_node("enumforward");
		   ty = NewStringf("enum %s", (yyvsp[(3) - (5)].id));
		   Setattr((yyval.node),"enumkey",(yyvsp[(2) - (5)].node));
		   if (scopedenum)
		     SetFlag((yyval.node), "scopedenum");
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].id));
		   Setattr((yyval.node),"inherit",(yyvsp[(4) - (5)].node));
		   Setattr((yyval.node),"type",ty);
		   Setattr((yyval.node),"sym:weak", "1");
		   add_symbols((yyval.node));
	      }
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 3178 "parser.y"
    {
		  SwigType *ty = 0;
		  int scopedenum = (yyvsp[(3) - (8)].id) && !Equal((yyvsp[(2) - (8)].node), "enum");
                  (yyval.node) = new_node("enum");
		  ty = NewStringf("enum %s", (yyvsp[(3) - (8)].id));
		  Setattr((yyval.node),"enumkey",(yyvsp[(2) - (8)].node));
		  if (scopedenum)
		    SetFlag((yyval.node), "scopedenum");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (8)].id));
		  Setattr((yyval.node),"inherit",(yyvsp[(4) - (8)].node));
		  Setattr((yyval.node),"type",ty);
		  appendChild((yyval.node),(yyvsp[(6) - (8)].node));
		  add_symbols((yyval.node));      /* Add to tag space */

		  if (scopedenum) {
		    Swig_symbol_newscope();
		    Swig_symbol_setscopename((yyvsp[(3) - (8)].id));
		    Delete(Namespaceprefix);
		    Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		  }

		  add_symbols((yyvsp[(6) - (8)].node));      /* Add enum values to appropriate enum or enum class scope */

		  if (scopedenum) {
		    Setattr((yyval.node),"symtab", Swig_symbol_popscope());
		    Delete(Namespaceprefix);
		    Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		  }
               }
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 3207 "parser.y"
    {
		 Node *n;
		 SwigType *ty = 0;
		 String   *unnamed = 0;
		 int       unnamedinstance = 0;
		 int scopedenum = (yyvsp[(3) - (10)].id) && !Equal((yyvsp[(2) - (10)].node), "enum");

		 (yyval.node) = new_node("enum");
		 Setattr((yyval.node),"enumkey",(yyvsp[(2) - (10)].node));
		 if (scopedenum)
		   SetFlag((yyval.node), "scopedenum");
		 Setattr((yyval.node),"inherit",(yyvsp[(4) - (10)].node));
		 if ((yyvsp[(3) - (10)].id)) {
		   Setattr((yyval.node),"name",(yyvsp[(3) - (10)].id));
		   ty = NewStringf("enum %s", (yyvsp[(3) - (10)].id));
		 } else if ((yyvsp[(8) - (10)].decl).id) {
		   unnamed = make_unnamed();
		   ty = NewStringf("enum %s", unnamed);
		   Setattr((yyval.node),"unnamed",unnamed);
                   /* name is not set for unnamed enum instances, e.g. enum { foo } Instance; */
		   if ((yyvsp[(1) - (10)].id) && Cmp((yyvsp[(1) - (10)].id),"typedef") == 0) {
		     Setattr((yyval.node),"name",(yyvsp[(8) - (10)].decl).id);
                   } else {
                     unnamedinstance = 1;
                   }
		   Setattr((yyval.node),"storage",(yyvsp[(1) - (10)].id));
		 }
		 if ((yyvsp[(8) - (10)].decl).id && Cmp((yyvsp[(1) - (10)].id),"typedef") == 0) {
		   Setattr((yyval.node),"tdname",(yyvsp[(8) - (10)].decl).id);
                   Setattr((yyval.node),"allows_typedef","1");
                 }
		 appendChild((yyval.node),(yyvsp[(6) - (10)].node));
		 n = new_node("cdecl");
		 Setattr(n,"type",ty);
		 Setattr(n,"name",(yyvsp[(8) - (10)].decl).id);
		 Setattr(n,"storage",(yyvsp[(1) - (10)].id));
		 Setattr(n,"decl",(yyvsp[(8) - (10)].decl).type);
		 Setattr(n,"parms",(yyvsp[(8) - (10)].decl).parms);
		 Setattr(n,"unnamed",unnamed);

                 if (unnamedinstance) {
		   SwigType *cty = NewString("enum ");
		   Setattr((yyval.node),"type",cty);
		   SetFlag((yyval.node),"unnamedinstance");
		   SetFlag(n,"unnamedinstance");
		   Delete(cty);
                 }
		 if ((yyvsp[(10) - (10)].node)) {
		   Node *p = (yyvsp[(10) - (10)].node);
		   set_nextSibling(n,p);
		   while (p) {
		     SwigType *cty = Copy(ty);
		     Setattr(p,"type",cty);
		     Setattr(p,"unnamed",unnamed);
		     Setattr(p,"storage",(yyvsp[(1) - (10)].id));
		     Delete(cty);
		     p = nextSibling(p);
		   }
		 } else {
		   if (Len(scanner_ccode)) {
		     String *code = Copy(scanner_ccode);
		     Setattr(n,"code",code);
		     Delete(code);
		   }
		 }

                 /* Ensure that typedef enum ABC {foo} XYZ; uses XYZ for sym:name, like structs.
                  * Note that class_rename/yyrename are bit of a mess so used this simple approach to change the name. */
                 if ((yyvsp[(8) - (10)].decl).id && (yyvsp[(3) - (10)].id) && Cmp((yyvsp[(1) - (10)].id),"typedef") == 0) {
		   String *name = NewString((yyvsp[(8) - (10)].decl).id);
                   Setattr((yyval.node), "parser:makename", name);
		   Delete(name);
                 }

		 add_symbols((yyval.node));       /* Add enum to tag space */
		 set_nextSibling((yyval.node),n);
		 Delete(n);

		 if (scopedenum) {
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename((yyvsp[(3) - (10)].id));
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 }

		 add_symbols((yyvsp[(6) - (10)].node));      /* Add enum values to appropriate enum or enum class scope */

		 if (scopedenum) {
		   Setattr((yyval.node),"symtab", Swig_symbol_popscope());
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 }

	         add_symbols(n);
		 Delete(unnamed);
	       }
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 3305 "parser.y"
    {
                   /* This is a sick hack.  If the ctor_end has parameters,
                      and the parms parameter only has 1 parameter, this
                      could be a declaration of the form:

                         type (id)(parms)

			 Otherwise it's an error. */
                    int err = 0;
                    (yyval.node) = 0;

		    if ((ParmList_len((yyvsp[(4) - (6)].pl)) == 1) && (!Swig_scopename_check((yyvsp[(2) - (6)].type)))) {
		      SwigType *ty = Getattr((yyvsp[(4) - (6)].pl),"type");
		      String *name = Getattr((yyvsp[(4) - (6)].pl),"name");
		      err = 1;
		      if (!name) {
			(yyval.node) = new_node("cdecl");
			Setattr((yyval.node),"type",(yyvsp[(2) - (6)].type));
			Setattr((yyval.node),"storage",(yyvsp[(1) - (6)].id));
			Setattr((yyval.node),"name",ty);

			if ((yyvsp[(6) - (6)].decl).have_parms) {
			  SwigType *decl = NewStringEmpty();
			  SwigType_add_function(decl,(yyvsp[(6) - (6)].decl).parms);
			  Setattr((yyval.node),"decl",decl);
			  Setattr((yyval.node),"parms",(yyvsp[(6) - (6)].decl).parms);
			  if (Len(scanner_ccode)) {
			    String *code = Copy(scanner_ccode);
			    Setattr((yyval.node),"code",code);
			    Delete(code);
			  }
			}
			if ((yyvsp[(6) - (6)].decl).defarg) {
			  Setattr((yyval.node),"value",(yyvsp[(6) - (6)].decl).defarg);
			}
			Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].decl).throws);
			Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].decl).throwf);
			Setattr((yyval.node),"noexcept",(yyvsp[(6) - (6)].decl).nexcept);
			err = 0;
		      }
		    }
		    if (err) {
		      Swig_error(cparse_file,cparse_line,"Syntax error in input(2).\n");
		      exit(1);
		    }
                }
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 3357 "parser.y"
    {  (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 3358 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 3359 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 3360 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 3361 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 3362 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 3367 "parser.y"
    {
                   String *prefix;
                   List *bases = 0;
		   Node *scope = 0;
		   String *code;
		   (yyval.node) = new_node("class");
		   Setline((yyval.node),cparse_start_line);
		   Setattr((yyval.node),"kind",(yyvsp[(2) - (5)].id));
		   if ((yyvsp[(4) - (5)].bases)) {
		     Setattr((yyval.node),"baselist", Getattr((yyvsp[(4) - (5)].bases),"public"));
		     Setattr((yyval.node),"protectedbaselist", Getattr((yyvsp[(4) - (5)].bases),"protected"));
		     Setattr((yyval.node),"privatebaselist", Getattr((yyvsp[(4) - (5)].bases),"private"));
		   }
		   Setattr((yyval.node),"allows_typedef","1");

		   /* preserve the current scope */
		   Setattr((yyval.node),"prev_symtab",Swig_symbol_current());
		  
		   /* If the class name is qualified.  We need to create or lookup namespace/scope entries */
		   scope = resolve_create_node_scope((yyvsp[(3) - (5)].str));
		   /* save nscope_inner to the class - it may be overwritten in nested classes*/
		   Setattr((yyval.node), "nested:innerscope", nscope_inner);
		   Setattr((yyval.node), "nested:nscope", nscope);
		   Setfile(scope,cparse_file);
		   Setline(scope,cparse_line);
		   (yyvsp[(3) - (5)].str) = scope;
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].str));

		   if (currentOuterClass) {
		     SetFlag((yyval.node), "nested");
		     Setattr((yyval.node), "nested:outer", currentOuterClass);
		     set_access_mode((yyval.node));
		   }
		   Swig_features_get(Swig_cparse_features(), Namespaceprefix, Getattr((yyval.node), "name"), 0, (yyval.node));
		   /* save yyrename to the class attribute, to be used later in add_symbols()*/
		   Setattr((yyval.node), "class_rename", make_name((yyval.node), (yyvsp[(3) - (5)].str), 0));
		   Setattr((yyval.node), "Classprefix", (yyvsp[(3) - (5)].str));
		   Classprefix = NewString((yyvsp[(3) - (5)].str));
		   /* Deal with inheritance  */
		   if ((yyvsp[(4) - (5)].bases))
		     bases = Swig_make_inherit_list((yyvsp[(3) - (5)].str),Getattr((yyvsp[(4) - (5)].bases),"public"),Namespaceprefix);
		   prefix = SwigType_istemplate_templateprefix((yyvsp[(3) - (5)].str));
		   if (prefix) {
		     String *fbase, *tbase;
		     if (Namespaceprefix) {
		       fbase = NewStringf("%s::%s", Namespaceprefix,(yyvsp[(3) - (5)].str));
		       tbase = NewStringf("%s::%s", Namespaceprefix, prefix);
		     } else {
		       fbase = Copy((yyvsp[(3) - (5)].str));
		       tbase = Copy(prefix);
		     }
		     Swig_name_inherit(tbase,fbase);
		     Delete(fbase);
		     Delete(tbase);
		   }
                   if (strcmp((yyvsp[(2) - (5)].id),"class") == 0) {
		     cplus_mode = CPLUS_PRIVATE;
		   } else {
		     cplus_mode = CPLUS_PUBLIC;
		   }
		   if (!cparse_cplusplus) {
		     set_scope_to_global();
		   }
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename((yyvsp[(3) - (5)].str));
		   Swig_inherit_base_symbols(bases);
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		   cparse_start_line = cparse_line;

		   /* If there are active template parameters, we need to make sure they are
                      placed in the class symbol table so we can catch shadows */

		   if (template_parameters) {
		     Parm *tp = template_parameters;
		     while(tp) {
		       String *tpname = Copy(Getattr(tp,"name"));
		       Node *tn = new_node("templateparm");
		       Setattr(tn,"name",tpname);
		       Swig_symbol_cadd(tpname,tn);
		       tp = nextSibling(tp);
		       Delete(tpname);
		     }
		   }
		   Delete(prefix);
		   inclass = 1;
		   currentOuterClass = (yyval.node);
		   if (cparse_cplusplusout) {
		     /* save the structure declaration to declare it in global scope for C++ to see */
		     code = get_raw_text_balanced('{', '}');
		     Setattr((yyval.node), "code", code);
		     Delete(code);
		   }
               }
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 3460 "parser.y"
    {
		   Node *p;
		   SwigType *ty;
		   Symtab *cscope;
		   Node *am = 0;
		   String *scpname = 0;
		   (void) (yyvsp[(6) - (9)].node);
		   (yyval.node) = currentOuterClass;
		   currentOuterClass = Getattr((yyval.node), "nested:outer");
		   nscope_inner = Getattr((yyval.node), "nested:innerscope");
		   nscope = Getattr((yyval.node), "nested:nscope");
		   Delattr((yyval.node), "nested:innerscope");
		   Delattr((yyval.node), "nested:nscope");
		   if (nscope_inner && Strcmp(nodeType(nscope_inner), "class") == 0) { /* actual parent class for this class */
		     Node* forward_declaration = Swig_symbol_clookup_no_inherit(Getattr((yyval.node),"name"), Getattr(nscope_inner, "symtab"));
		     if (forward_declaration) {
		       Setattr((yyval.node), "access", Getattr(forward_declaration, "access"));
		     }
		     Setattr((yyval.node), "nested:outer", nscope_inner);
		     SetFlag((yyval.node), "nested");
                   }
		   if (!currentOuterClass)
		     inclass = 0;
		   cscope = Getattr((yyval.node), "prev_symtab");
		   Delattr((yyval.node), "prev_symtab");
		   
		   /* Check for pure-abstract class */
		   Setattr((yyval.node),"abstracts", pure_abstracts((yyvsp[(7) - (9)].node)));
		   
		   /* This bit of code merges in a previously defined %extend directive (if any) */
		   {
		     String *clsname = Swig_symbol_qualifiedscopename(0);
		     am = Getattr(Swig_extend_hash(), clsname);
		     if (am) {
		       Swig_extend_merge((yyval.node), am);
		       Delattr(Swig_extend_hash(), clsname);
		     }
		     Delete(clsname);
		   }
		   if (!classes) classes = NewHash();
		   scpname = Swig_symbol_qualifiedscopename(0);
		   Setattr(classes, scpname, (yyval.node));

		   appendChild((yyval.node), (yyvsp[(7) - (9)].node));
		   
		   if (am) 
		     Swig_extend_append_previous((yyval.node), am);

		   p = (yyvsp[(9) - (9)].node);
		   if (p && !nscope_inner) {
		     if (!cparse_cplusplus && currentOuterClass)
		       appendChild(currentOuterClass, p);
		     else
		      appendSibling((yyval.node), p);
		   }
		   
		   if (nscope_inner) {
		     ty = NewString(scpname); /* if the class is declared out of scope, let the declarator use fully qualified type*/
		   } else if (cparse_cplusplus && !cparse_externc) {
		     ty = NewString((yyvsp[(3) - (9)].str));
		   } else {
		     ty = NewStringf("%s %s", (yyvsp[(2) - (9)].id), (yyvsp[(3) - (9)].str));
		   }
		   while (p) {
		     Setattr(p, "storage", (yyvsp[(1) - (9)].id));
		     Setattr(p, "type" ,ty);
		     if (!cparse_cplusplus && currentOuterClass && (!Getattr(currentOuterClass, "name"))) {
		       SetFlag(p, "hasconsttype");
		       SetFlag(p, "feature:immutable");
		     }
		     p = nextSibling(p);
		   }
		   if ((yyvsp[(9) - (9)].node) && Cmp((yyvsp[(1) - (9)].id),"typedef") == 0)
		     add_typedef_name((yyval.node), (yyvsp[(9) - (9)].node), (yyvsp[(3) - (9)].str), cscope, scpname);
		   Delete(scpname);

		   if (cplus_mode != CPLUS_PUBLIC) {
		   /* we 'open' the class at the end, to allow %template
		      to add new members */
		     Node *pa = new_node("access");
		     Setattr(pa, "kind", "public");
		     cplus_mode = CPLUS_PUBLIC;
		     appendChild((yyval.node), pa);
		     Delete(pa);
		   }
		   if (currentOuterClass)
		     restore_access_mode((yyval.node));
		   Setattr((yyval.node), "symtab", Swig_symbol_popscope());
		   Classprefix = Getattr((yyval.node), "Classprefix");
		   Delattr((yyval.node), "Classprefix");
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		   if (cplus_mode == CPLUS_PRIVATE) {
		     (yyval.node) = 0; /* skip private nested classes */
		   } else if (cparse_cplusplus && currentOuterClass && ignore_nested_classes && !GetFlag((yyval.node), "feature:flatnested")) {
		     (yyval.node) = nested_forward_declaration((yyvsp[(1) - (9)].id), (yyvsp[(2) - (9)].id), (yyvsp[(3) - (9)].str), Copy((yyvsp[(3) - (9)].str)), (yyvsp[(9) - (9)].node));
		   } else if (nscope_inner) {
		     /* this is tricky */
		     /* we add the declaration in the original namespace */
		     if (Strcmp(nodeType(nscope_inner), "class") == 0 && cparse_cplusplus && ignore_nested_classes && !GetFlag((yyval.node), "feature:flatnested"))
		       (yyval.node) = nested_forward_declaration((yyvsp[(1) - (9)].id), (yyvsp[(2) - (9)].id), (yyvsp[(3) - (9)].str), Copy((yyvsp[(3) - (9)].str)), (yyvsp[(9) - (9)].node));
		     appendChild(nscope_inner, (yyval.node));
		     Swig_symbol_setscope(Getattr(nscope_inner, "symtab"));
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		     yyrename = Copy(Getattr((yyval.node), "class_rename"));
		     add_symbols((yyval.node));
		     Delattr((yyval.node), "class_rename");
		     /* but the variable definition in the current scope */
		     Swig_symbol_setscope(cscope);
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		     add_symbols((yyvsp[(9) - (9)].node));
		     if (nscope) {
		       (yyval.node) = nscope; /* here we return recreated namespace tower instead of the class itself */
		       if ((yyvsp[(9) - (9)].node)) {
			 appendSibling((yyval.node), (yyvsp[(9) - (9)].node));
		       }
		     } else if (!SwigType_istemplate(ty) && template_parameters == 0) { /* for tempalte we need the class itself */
		       (yyval.node) = (yyvsp[(9) - (9)].node);
		     }
		   } else {
		     Delete(yyrename);
		     yyrename = 0;
		     if (!cparse_cplusplus && currentOuterClass) { /* nested C structs go into global scope*/
		       Node *outer = currentOuterClass;
		       while (Getattr(outer, "nested:outer"))
			 outer = Getattr(outer, "nested:outer");
		       appendSibling(outer, (yyval.node));
		       add_symbols((yyvsp[(9) - (9)].node));
		       set_scope_to_global();
		       Delete(Namespaceprefix);
		       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		       yyrename = Copy(Getattr((yyval.node), "class_rename"));
		       add_symbols((yyval.node));
		       if (!cparse_cplusplusout)
			 Delattr((yyval.node), "nested:outer");
		       Delattr((yyval.node), "class_rename");
		       (yyval.node) = 0;
		     } else {
		       yyrename = Copy(Getattr((yyval.node), "class_rename"));
		       add_symbols((yyval.node));
		       add_symbols((yyvsp[(9) - (9)].node));
		       Delattr((yyval.node), "class_rename");
		     }
		   }
		   Delete(ty);
		   Swig_symbol_setscope(cscope);
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       }
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 3614 "parser.y"
    {
	       String *unnamed;
	       String *code;
	       unnamed = make_unnamed();
	       (yyval.node) = new_node("class");
	       Setline((yyval.node),cparse_start_line);
	       Setattr((yyval.node),"kind",(yyvsp[(2) - (4)].id));
	       if ((yyvsp[(3) - (4)].bases)) {
		 Setattr((yyval.node),"baselist", Getattr((yyvsp[(3) - (4)].bases),"public"));
		 Setattr((yyval.node),"protectedbaselist", Getattr((yyvsp[(3) - (4)].bases),"protected"));
		 Setattr((yyval.node),"privatebaselist", Getattr((yyvsp[(3) - (4)].bases),"private"));
	       }
	       Setattr((yyval.node),"storage",(yyvsp[(1) - (4)].id));
	       Setattr((yyval.node),"unnamed",unnamed);
	       Setattr((yyval.node),"allows_typedef","1");
	       if (currentOuterClass) {
		 SetFlag((yyval.node), "nested");
		 Setattr((yyval.node), "nested:outer", currentOuterClass);
		 set_access_mode((yyval.node));
	       }
	       Swig_features_get(Swig_cparse_features(), Namespaceprefix, 0, 0, (yyval.node));
	       /* save yyrename to the class attribute, to be used later in add_symbols()*/
	       Setattr((yyval.node), "class_rename", make_name((yyval.node),0,0));
	       if (strcmp((yyvsp[(2) - (4)].id),"class") == 0) {
		 cplus_mode = CPLUS_PRIVATE;
	       } else {
		 cplus_mode = CPLUS_PUBLIC;
	       }
	       Swig_symbol_newscope();
	       cparse_start_line = cparse_line;
	       currentOuterClass = (yyval.node);
	       inclass = 1;
	       Classprefix = NewStringEmpty();
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       /* save the structure declaration to make a typedef for it later*/
	       code = get_raw_text_balanced('{', '}');
	       Setattr((yyval.node), "code", code);
	       Delete(code);
	     }
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 3653 "parser.y"
    {
	       String *unnamed;
               List *bases = 0;
	       String *name = 0;
	       Node *n;
	       Classprefix = 0;
	       (yyval.node) = currentOuterClass;
	       currentOuterClass = Getattr((yyval.node), "nested:outer");
	       if (!currentOuterClass)
		 inclass = 0;
	       else
		 restore_access_mode((yyval.node));
	       unnamed = Getattr((yyval.node),"unnamed");
               /* Check for pure-abstract class */
	       Setattr((yyval.node),"abstracts", pure_abstracts((yyvsp[(6) - (8)].node)));
	       n = (yyvsp[(8) - (8)].node);
	       if (cparse_cplusplus && currentOuterClass && ignore_nested_classes && !GetFlag((yyval.node), "feature:flatnested")) {
		 String *name = n ? Copy(Getattr(n, "name")) : 0;
		 (yyval.node) = nested_forward_declaration((yyvsp[(1) - (8)].id), (yyvsp[(2) - (8)].id), 0, name, n);
		 Swig_symbol_popscope();
	         Delete(Namespaceprefix);
		 Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       } else if (n) {
	         appendSibling((yyval.node),n);
		 /* If a proper typedef name was given, we'll use it to set the scope name */
		 name = try_to_find_a_name_for_unnamed_structure((yyvsp[(1) - (8)].id), n);
		 if (name) {
		   String *scpname = 0;
		   SwigType *ty;
		   Setattr((yyval.node),"tdname",name);
		   Setattr((yyval.node),"name",name);
		   Swig_symbol_setscopename(name);
		   if ((yyvsp[(3) - (8)].bases))
		     bases = Swig_make_inherit_list(name,Getattr((yyvsp[(3) - (8)].bases),"public"),Namespaceprefix);
		   Swig_inherit_base_symbols(bases);

		     /* If a proper name was given, we use that as the typedef, not unnamed */
		   Clear(unnamed);
		   Append(unnamed, name);
		   if (cparse_cplusplus && !cparse_externc) {
		     ty = NewString(name);
		   } else {
		     ty = NewStringf("%s %s", (yyvsp[(2) - (8)].id),name);
		   }
		   while (n) {
		     Setattr(n,"storage",(yyvsp[(1) - (8)].id));
		     Setattr(n, "type", ty);
		     if (!cparse_cplusplus && currentOuterClass && (!Getattr(currentOuterClass, "name"))) {
		       SetFlag(n,"hasconsttype");
		       SetFlag(n,"feature:immutable");
		     }
		     n = nextSibling(n);
		   }
		   n = (yyvsp[(8) - (8)].node);

		   /* Check for previous extensions */
		   {
		     String *clsname = Swig_symbol_qualifiedscopename(0);
		     Node *am = Getattr(Swig_extend_hash(),clsname);
		     if (am) {
		       /* Merge the extension into the symbol table */
		       Swig_extend_merge((yyval.node),am);
		       Swig_extend_append_previous((yyval.node),am);
		       Delattr(Swig_extend_hash(),clsname);
		     }
		     Delete(clsname);
		   }
		   if (!classes) classes = NewHash();
		   scpname = Swig_symbol_qualifiedscopename(0);
		   Setattr(classes,scpname,(yyval.node));
		   Delete(scpname);
		 } else { /* no suitable name was found for a struct */
		   Setattr((yyval.node), "nested:unnamed", Getattr(n, "name")); /* save the name of the first declarator for later use in name generation*/
		   while (n) { /* attach unnamed struct to the declarators, so that they would receive proper type later*/
		     Setattr(n, "nested:unnamedtype", (yyval.node));
		     Setattr(n, "storage", (yyvsp[(1) - (8)].id));
		     n = nextSibling(n);
		   }
		   n = (yyvsp[(8) - (8)].node);
		   Swig_symbol_setscopename("<unnamed>");
		 }
		 appendChild((yyval.node),(yyvsp[(6) - (8)].node));
		 /* Pop the scope */
		 Setattr((yyval.node),"symtab",Swig_symbol_popscope());
		 if (name) {
		   Delete(yyrename);
		   yyrename = Copy(Getattr((yyval.node), "class_rename"));
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		   add_symbols((yyval.node));
		   add_symbols(n);
		   Delattr((yyval.node), "class_rename");
		 }else if (cparse_cplusplus)
		   (yyval.node) = 0; /* ignore unnamed structs for C++ */
	         Delete(unnamed);
	       } else { /* unnamed struct w/o declarator*/
		 Swig_symbol_popscope();
	         Delete(Namespaceprefix);
		 Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 add_symbols((yyvsp[(6) - (8)].node));
		 Delete((yyval.node));
		 (yyval.node) = (yyvsp[(6) - (8)].node); /* pass member list to outer class/namespace (instead of self)*/
	       }
              }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 3759 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 3760 "parser.y"
    {
                        (yyval.node) = new_node("cdecl");
                        Setattr((yyval.node),"name",(yyvsp[(1) - (3)].decl).id);
                        Setattr((yyval.node),"decl",(yyvsp[(1) - (3)].decl).type);
                        Setattr((yyval.node),"parms",(yyvsp[(1) - (3)].decl).parms);
			set_nextSibling((yyval.node),(yyvsp[(3) - (3)].node));
                    }
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 3772 "parser.y"
    {
              if ((yyvsp[(1) - (4)].id) && (Strcmp((yyvsp[(1) - (4)].id),"friend") == 0)) {
		/* Ignore */
                (yyval.node) = 0; 
	      } else {
		(yyval.node) = new_node("classforward");
		Setattr((yyval.node),"kind",(yyvsp[(2) - (4)].id));
		Setattr((yyval.node),"name",(yyvsp[(3) - (4)].str));
		Setattr((yyval.node),"sym:weak", "1");
		add_symbols((yyval.node));
	      }
             }
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 3790 "parser.y"
    { 
		   if (currentOuterClass)
		     Setattr(currentOuterClass, "template_parameters", template_parameters);
		    template_parameters = (yyvsp[(3) - (4)].tparms); 
		  }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 3794 "parser.y"
    {
			String *tname = 0;
			int     error = 0;

			/* check if we get a namespace node with a class declaration, and retrieve the class */
			Symtab *cscope = Swig_symbol_current();
			Symtab *sti = 0;
			Node *ntop = (yyvsp[(6) - (6)].node);
			Node *ni = ntop;
			SwigType *ntype = ni ? nodeType(ni) : 0;
			while (ni && Strcmp(ntype,"namespace") == 0) {
			  sti = Getattr(ni,"symtab");
			  ni = firstChild(ni);
			  ntype = nodeType(ni);
			}
			if (sti) {
			  Swig_symbol_setscope(sti);
			  Delete(Namespaceprefix);
			  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
			  (yyvsp[(6) - (6)].node) = ni;
			}

			(yyval.node) = (yyvsp[(6) - (6)].node);
			if ((yyval.node)) tname = Getattr((yyval.node),"name");
			
			/* Check if the class is a template specialization */
			if (((yyval.node)) && (Strchr(tname,'<')) && (!is_operator(tname))) {
			  /* If a specialization.  Check if defined. */
			  Node *tempn = 0;
			  {
			    String *tbase = SwigType_templateprefix(tname);
			    tempn = Swig_symbol_clookup_local(tbase,0);
			    if (!tempn || (Strcmp(nodeType(tempn),"template") != 0)) {
			      SWIG_WARN_NODE_BEGIN(tempn);
			      Swig_warning(WARN_PARSE_TEMPLATE_SP_UNDEF, Getfile((yyval.node)),Getline((yyval.node)),"Specialization of non-template '%s'.\n", tbase);
			      SWIG_WARN_NODE_END(tempn);
			      tempn = 0;
			      error = 1;
			    }
			    Delete(tbase);
			  }
			  Setattr((yyval.node),"specialization","1");
			  Setattr((yyval.node),"templatetype",nodeType((yyval.node)));
			  set_nodeType((yyval.node),"template");
			  /* Template partial specialization */
			  if (tempn && ((yyvsp[(3) - (6)].tparms)) && ((yyvsp[(6) - (6)].node))) {
			    List   *tlist;
			    String *targs = SwigType_templateargs(tname);
			    tlist = SwigType_parmlist(targs);
			    /*			  Printf(stdout,"targs = '%s' %s\n", targs, tlist); */
			    if (!Getattr((yyval.node),"sym:weak")) {
			      Setattr((yyval.node),"sym:typename","1");
			    }
			    
			    if (Len(tlist) != ParmList_len(Getattr(tempn,"templateparms"))) {
			      Swig_error(Getfile((yyval.node)),Getline((yyval.node)),"Inconsistent argument count in template partial specialization. %d %d\n", Len(tlist), ParmList_len(Getattr(tempn,"templateparms")));
			      
			    } else {

			    /* This code builds the argument list for the partial template
			       specialization.  This is a little hairy, but the idea is as
			       follows:

			       $3 contains a list of arguments supplied for the template.
			       For example template<class T>.

			       tlist is a list of the specialization arguments--which may be
			       different.  For example class<int,T>.

			       tp is a copy of the arguments in the original template definition.
       
			       The patching algorithm walks through the list of supplied
			       arguments ($3), finds the position in the specialization arguments
			       (tlist), and then patches the name in the argument list of the
			       original template.
			    */

			    {
			      String *pn;
			      Parm *p, *p1;
			      int i, nargs;
			      Parm *tp = CopyParmList(Getattr(tempn,"templateparms"));
			      nargs = Len(tlist);
			      p = (yyvsp[(3) - (6)].tparms);
			      while (p) {
				for (i = 0; i < nargs; i++){
				  pn = Getattr(p,"name");
				  if (Strcmp(pn,SwigType_base(Getitem(tlist,i))) == 0) {
				    int j;
				    Parm *p1 = tp;
				    for (j = 0; j < i; j++) {
				      p1 = nextSibling(p1);
				    }
				    Setattr(p1,"name",pn);
				    Setattr(p1,"partialarg","1");
				  }
				}
				p = nextSibling(p);
			      }
			      p1 = tp;
			      i = 0;
			      while (p1) {
				if (!Getattr(p1,"partialarg")) {
				  Delattr(p1,"name");
				  Setattr(p1,"type", Getitem(tlist,i));
				} 
				i++;
				p1 = nextSibling(p1);
			      }
			      Setattr((yyval.node),"templateparms",tp);
			      Delete(tp);
			    }
  #if 0
			    /* Patch the parameter list */
			    if (tempn) {
			      Parm *p,*p1;
			      ParmList *tp = CopyParmList(Getattr(tempn,"templateparms"));
			      p = (yyvsp[(3) - (6)].tparms);
			      p1 = tp;
			      while (p && p1) {
				String *pn = Getattr(p,"name");
				Printf(stdout,"pn = '%s'\n", pn);
				if (pn) Setattr(p1,"name",pn);
				else Delattr(p1,"name");
				pn = Getattr(p,"type");
				if (pn) Setattr(p1,"type",pn);
				p = nextSibling(p);
				p1 = nextSibling(p1);
			      }
			      Setattr((yyval.node),"templateparms",tp);
			      Delete(tp);
			    } else {
			      Setattr((yyval.node),"templateparms",(yyvsp[(3) - (6)].tparms));
			    }
  #endif
			    Delattr((yyval.node),"specialization");
			    Setattr((yyval.node),"partialspecialization","1");
			    /* Create a specialized name for matching */
			    {
			      Parm *p = (yyvsp[(3) - (6)].tparms);
			      String *fname = NewString(Getattr((yyval.node),"name"));
			      String *ffname = 0;
			      ParmList *partialparms = 0;

			      char   tmp[32];
			      int    i, ilen;
			      while (p) {
				String *n = Getattr(p,"name");
				if (!n) {
				  p = nextSibling(p);
				  continue;
				}
				ilen = Len(tlist);
				for (i = 0; i < ilen; i++) {
				  if (Strstr(Getitem(tlist,i),n)) {
				    sprintf(tmp,"$%d",i+1);
				    Replaceid(fname,n,tmp);
				  }
				}
				p = nextSibling(p);
			      }
			      /* Patch argument names with typedef */
			      {
				Iterator tt;
				Parm *parm_current = 0;
				List *tparms = SwigType_parmlist(fname);
				ffname = SwigType_templateprefix(fname);
				Append(ffname,"<(");
				for (tt = First(tparms); tt.item; ) {
				  SwigType *rtt = Swig_symbol_typedef_reduce(tt.item,0);
				  SwigType *ttr = Swig_symbol_type_qualify(rtt,0);

				  Parm *newp = NewParmWithoutFileLineInfo(ttr, 0);
				  if (partialparms)
				    set_nextSibling(parm_current, newp);
				  else
				    partialparms = newp;
				  parm_current = newp;

				  Append(ffname,ttr);
				  tt = Next(tt);
				  if (tt.item) Putc(',',ffname);
				  Delete(rtt);
				  Delete(ttr);
				}
				Delete(tparms);
				Append(ffname,")>");
			      }
			      {
				Node *new_partial = NewHash();
				String *partials = Getattr(tempn,"partials");
				if (!partials) {
				  partials = NewList();
				  Setattr(tempn,"partials",partials);
				  Delete(partials);
				}
				/*			      Printf(stdout,"partial: fname = '%s', '%s'\n", fname, Swig_symbol_typedef_reduce(fname,0)); */
				Setattr(new_partial, "partialparms", partialparms);
				Setattr(new_partial, "templcsymname", ffname);
				Append(partials, new_partial);
			      }
			      Setattr((yyval.node),"partialargs",ffname);
			      Swig_symbol_cadd(ffname,(yyval.node));
			    }
			    }
			    Delete(tlist);
			    Delete(targs);
			  } else {
			    /* An explicit template specialization */
			    /* add default args from primary (unspecialized) template */
			    String *ty = Swig_symbol_template_deftype(tname,0);
			    String *fname = Swig_symbol_type_qualify(ty,0);
			    Swig_symbol_cadd(fname,(yyval.node));
			    Delete(ty);
			    Delete(fname);
			  }
			}  else if ((yyval.node)) {
			  Setattr((yyval.node),"templatetype",nodeType((yyvsp[(6) - (6)].node)));
			  set_nodeType((yyval.node),"template");
			  Setattr((yyval.node),"templateparms", (yyvsp[(3) - (6)].tparms));
			  if (!Getattr((yyval.node),"sym:weak")) {
			    Setattr((yyval.node),"sym:typename","1");
			  }
			  add_symbols((yyval.node));
			  default_arguments((yyval.node));
			  /* We also place a fully parameterized version in the symbol table */
			  {
			    Parm *p;
			    String *fname = NewStringf("%s<(", Getattr((yyval.node),"name"));
			    p = (yyvsp[(3) - (6)].tparms);
			    while (p) {
			      String *n = Getattr(p,"name");
			      if (!n) n = Getattr(p,"type");
			      Append(fname,n);
			      p = nextSibling(p);
			      if (p) Putc(',',fname);
			    }
			    Append(fname,")>");
			    Swig_symbol_cadd(fname,(yyval.node));
			  }
			}
			(yyval.node) = ntop;
			Swig_symbol_setscope(cscope);
			Delete(Namespaceprefix);
			Namespaceprefix = Swig_symbol_qualifiedscopename(0);
			if (error || (nscope_inner && Strcmp(nodeType(nscope_inner), "class") == 0)) {
			  (yyval.node) = 0;
			}
			if (currentOuterClass)
			  template_parameters = Getattr(currentOuterClass, "template_parameters");
			else
			  template_parameters = 0;
                }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 4049 "parser.y"
    {
		  Swig_warning(WARN_PARSE_EXPLICIT_TEMPLATE, cparse_file, cparse_line, "Explicit template instantiation ignored.\n");
                  (yyval.node) = 0; 
		}
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 4055 "parser.y"
    {
		  Swig_warning(WARN_PARSE_EXPLICIT_TEMPLATE, cparse_file, cparse_line, "Explicit template instantiation ignored.\n");
                  (yyval.node) = 0; 
                }
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 4061 "parser.y"
    {
		  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 4064 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 4067 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 4070 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 4073 "parser.y"
    {
		  (yyval.node) = 0;
                }
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 4076 "parser.y"
    {
                  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 4079 "parser.y"
    {
                  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 4084 "parser.y"
    {
		   /* Rip out the parameter names */
		  Parm *p = (yyvsp[(1) - (1)].pl);
		  (yyval.tparms) = (yyvsp[(1) - (1)].pl);

		  while (p) {
		    String *name = Getattr(p,"name");
		    if (!name) {
		      /* Hmmm. Maybe it's a 'class T' parameter */
		      char *type = Char(Getattr(p,"type"));
		      /* Template template parameter */
		      if (strncmp(type,"template<class> ",16) == 0) {
			type += 16;
		      }
		      if ((strncmp(type,"class ",6) == 0) || (strncmp(type,"typename ", 9) == 0)) {
			char *t = strchr(type,' ');
			Setattr(p,"name", t+1);
		      } else 
                      /* Variadic template args */
		      if ((strncmp(type,"class... ",9) == 0) || (strncmp(type,"typename... ", 12) == 0)) {
			char *t = strchr(type,' ');
			Setattr(p,"name", t+1);
			Setattr(p,"variadic", "1");
		      } else {
			/*
			 Swig_error(cparse_file, cparse_line, "Missing template parameter name\n");
			 $$.rparms = 0;
			 $$.parms = 0;
			 break; */
		      }
		    }
		    p = nextSibling(p);
		  }
                 }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 4120 "parser.y"
    {
                      set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].pl));
                      (yyval.pl) = (yyvsp[(1) - (2)].p);
                   }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 4124 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 4127 "parser.y"
    {
		    (yyval.p) = NewParmWithoutFileLineInfo(NewString((yyvsp[(1) - (1)].id)), 0);
                  }
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 4130 "parser.y"
    {
                    (yyval.p) = (yyvsp[(1) - (1)].p);
                  }
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 4135 "parser.y"
    {
                         set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].pl));
                         (yyval.pl) = (yyvsp[(2) - (3)].p);
                       }
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 4139 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 186:

/* Line 1464 of yacc.c  */
#line 4144 "parser.y"
    {
                  String *uname = Swig_symbol_type_qualify((yyvsp[(2) - (3)].str),0);
		  String *name = Swig_scopename_last((yyvsp[(2) - (3)].str));
                  (yyval.node) = new_node("using");
		  Setattr((yyval.node),"uname",uname);
		  Setattr((yyval.node),"name", name);
		  Delete(uname);
		  Delete(name);
		  add_symbols((yyval.node));
             }
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 4154 "parser.y"
    {
	       Node *n = Swig_symbol_clookup((yyvsp[(3) - (4)].str),0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Nothing known about namespace '%s'\n", (yyvsp[(3) - (4)].str));
		 (yyval.node) = 0;
	       } else {

		 while (Strcmp(nodeType(n),"using") == 0) {
		   n = Getattr(n,"node");
		 }
		 if (n) {
		   if (Strcmp(nodeType(n),"namespace") == 0) {
		     Symtab *current = Swig_symbol_current();
		     Symtab *symtab = Getattr(n,"symtab");
		     (yyval.node) = new_node("using");
		     Setattr((yyval.node),"node",n);
		     Setattr((yyval.node),"namespace", (yyvsp[(3) - (4)].str));
		     if (current != symtab) {
		       Swig_symbol_inherit(symtab);
		     }
		   } else {
		     Swig_error(cparse_file, cparse_line, "'%s' is not a namespace.\n", (yyvsp[(3) - (4)].str));
		     (yyval.node) = 0;
		   }
		 } else {
		   (yyval.node) = 0;
		 }
	       }
             }
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 4185 "parser.y"
    { 
                Hash *h;
                (yyvsp[(1) - (3)].node) = Swig_symbol_current();
		h = Swig_symbol_clookup((yyvsp[(2) - (3)].str),0);
		if (h && ((yyvsp[(1) - (3)].node) == Getattr(h,"sym:symtab")) && (Strcmp(nodeType(h),"namespace") == 0)) {
		  if (Getattr(h,"alias")) {
		    h = Getattr(h,"namespace");
		    Swig_warning(WARN_PARSE_NAMESPACE_ALIAS, cparse_file, cparse_line, "Namespace alias '%s' not allowed here. Assuming '%s'\n",
				 (yyvsp[(2) - (3)].str), Getattr(h,"name"));
		    (yyvsp[(2) - (3)].str) = Getattr(h,"name");
		  }
		  Swig_symbol_setscope(Getattr(h,"symtab"));
		} else {
		  Swig_symbol_newscope();
		  Swig_symbol_setscopename((yyvsp[(2) - (3)].str));
		}
		Delete(Namespaceprefix);
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             }
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 4203 "parser.y"
    {
                Node *n = (yyvsp[(5) - (6)].node);
		set_nodeType(n,"namespace");
		Setattr(n,"name",(yyvsp[(2) - (6)].str));
                Setattr(n,"symtab", Swig_symbol_popscope());
		Swig_symbol_setscope((yyvsp[(1) - (6)].node));
		(yyval.node) = n;
		Delete(Namespaceprefix);
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		add_symbols((yyval.node));
             }
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 4214 "parser.y"
    {
	       Hash *h;
	       (yyvsp[(1) - (2)].node) = Swig_symbol_current();
	       h = Swig_symbol_clookup("    ",0);
	       if (h && (Strcmp(nodeType(h),"namespace") == 0)) {
		 Swig_symbol_setscope(Getattr(h,"symtab"));
	       } else {
		 Swig_symbol_newscope();
		 /* we don't use "__unnamed__", but a long 'empty' name */
		 Swig_symbol_setscopename("    ");
	       }
	       Namespaceprefix = 0;
             }
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 4226 "parser.y"
    {
	       (yyval.node) = (yyvsp[(4) - (5)].node);
	       set_nodeType((yyval.node),"namespace");
	       Setattr((yyval.node),"unnamed","1");
	       Setattr((yyval.node),"symtab", Swig_symbol_popscope());
	       Swig_symbol_setscope((yyvsp[(1) - (5)].node));
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       add_symbols((yyval.node));
             }
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 4236 "parser.y"
    {
	       /* Namespace alias */
	       Node *n;
	       (yyval.node) = new_node("namespace");
	       Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
	       Setattr((yyval.node),"alias",(yyvsp[(4) - (5)].str));
	       n = Swig_symbol_clookup((yyvsp[(4) - (5)].str),0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Unknown namespace '%s'\n", (yyvsp[(4) - (5)].str));
		 (yyval.node) = 0;
	       } else {
		 if (Strcmp(nodeType(n),"namespace") != 0) {
		   Swig_error(cparse_file, cparse_line, "'%s' is not a namespace\n",(yyvsp[(4) - (5)].str));
		   (yyval.node) = 0;
		 } else {
		   while (Getattr(n,"alias")) {
		     n = Getattr(n,"namespace");
		   }
		   Setattr((yyval.node),"namespace",n);
		   add_symbols((yyval.node));
		   /* Set up a scope alias */
		   Swig_symbol_alias((yyvsp[(2) - (5)].id),Getattr(n,"symtab"));
		 }
	       }
             }
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 4263 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (2)].node);
                   /* Insert cpp_member (including any siblings) to the front of the cpp_members linked list */
		   if ((yyval.node)) {
		     Node *p = (yyval.node);
		     Node *pp =0;
		     while (p) {
		       pp = p;
		       p = nextSibling(p);
		     }
		     set_nextSibling(pp,(yyvsp[(2) - (2)].node));
		     if ((yyvsp[(2) - (2)].node))
		       set_previousSibling((yyvsp[(2) - (2)].node), pp);
		   } else {
		     (yyval.node) = (yyvsp[(2) - (2)].node);
		   }
             }
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 4280 "parser.y"
    { 
	       extendmode = 1;
	       if (cplus_mode != CPLUS_PUBLIC) {
		 Swig_error(cparse_file,cparse_line,"%%extend can only be used in a public section\n");
	       }
             }
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 4285 "parser.y"
    {
	       extendmode = 0;
	     }
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 4287 "parser.y"
    {
	       (yyval.node) = new_node("extend");
	       mark_nodes_as_extend((yyvsp[(4) - (7)].node));
	       appendChild((yyval.node),(yyvsp[(4) - (7)].node));
	       set_nextSibling((yyval.node),(yyvsp[(7) - (7)].node));
	     }
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 4293 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 4294 "parser.y"
    { (yyval.node) = 0;}
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 4295 "parser.y"
    {
	       int start_line = cparse_line;
	       skip_decl();
	       Swig_error(cparse_file,start_line,"Syntax error in input(3).\n");
	       exit(1);
	       }
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 4300 "parser.y"
    { 
		 (yyval.node) = (yyvsp[(3) - (3)].node);
   	     }
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 4311 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 4312 "parser.y"
    { 
                 (yyval.node) = (yyvsp[(1) - (1)].node); 
		 if (extendmode && current_class) {
		   String *symname;
		   symname= make_name((yyval.node),Getattr((yyval.node),"name"), Getattr((yyval.node),"decl"));
		   if (Strcmp(symname,Getattr((yyval.node),"name")) == 0) {
		     /* No renaming operation.  Set name to class name */
		     Delete(yyrename);
		     yyrename = NewString(Getattr(current_class,"sym:name"));
		   } else {
		     Delete(yyrename);
		     yyrename = symname;
		   }
		 }
		 add_symbols((yyval.node));
                 default_arguments((yyval.node));
             }
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 4329 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 4330 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 4331 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 4332 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 4333 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 4334 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 4335 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 4336 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 4337 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 4338 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 4339 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 4340 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 4341 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 4342 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 4343 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 4344 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 219:

/* Line 1464 of yacc.c  */
#line 4345 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 4354 "parser.y"
    {
              if (inclass || extendmode) {
		SwigType *decl = NewStringEmpty();
		(yyval.node) = new_node("constructor");
		Setattr((yyval.node),"storage",(yyvsp[(1) - (6)].id));
		Setattr((yyval.node),"name",(yyvsp[(2) - (6)].type));
		Setattr((yyval.node),"parms",(yyvsp[(4) - (6)].pl));
		SwigType_add_function(decl,(yyvsp[(4) - (6)].pl));
		Setattr((yyval.node),"decl",decl);
		Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].decl).throws);
		Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].decl).throwf);
		Setattr((yyval.node),"noexcept",(yyvsp[(6) - (6)].decl).nexcept);
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
		SetFlag((yyval.node),"feature:new");
		if ((yyvsp[(6) - (6)].decl).defarg)
		  Setattr((yyval.node),"value",(yyvsp[(6) - (6)].decl).defarg);
	      } else {
		(yyval.node) = 0;
              }
              }
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 4382 "parser.y"
    {
               String *name = NewStringf("%s",(yyvsp[(2) - (6)].str));
	       if (*(Char(name)) != '~') Insert(name,0,"~");
               (yyval.node) = new_node("destructor");
	       Setattr((yyval.node),"name",name);
	       Delete(name);
	       if (Len(scanner_ccode)) {
		 String *code = Copy(scanner_ccode);
		 Setattr((yyval.node),"code",code);
		 Delete(code);
	       }
	       {
		 String *decl = NewStringEmpty();
		 SwigType_add_function(decl,(yyvsp[(4) - (6)].pl));
		 Setattr((yyval.node),"decl",decl);
		 Delete(decl);
	       }
	       Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].dtype).throws);
	       Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].dtype).throwf);
	       Setattr((yyval.node),"noexcept",(yyvsp[(6) - (6)].dtype).nexcept);
	       if ((yyvsp[(6) - (6)].dtype).val)
	         Setattr((yyval.node),"value",(yyvsp[(6) - (6)].dtype).val);
	       add_symbols((yyval.node));
	      }
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 4409 "parser.y"
    {
		String *name;
		(yyval.node) = new_node("destructor");
		Setattr((yyval.node),"storage","virtual");
	        name = NewStringf("%s",(yyvsp[(3) - (7)].str));
		if (*(Char(name)) != '~') Insert(name,0,"~");
		Setattr((yyval.node),"name",name);
		Delete(name);
		Setattr((yyval.node),"throws",(yyvsp[(7) - (7)].dtype).throws);
		Setattr((yyval.node),"throw",(yyvsp[(7) - (7)].dtype).throwf);
		Setattr((yyval.node),"noexcept",(yyvsp[(7) - (7)].dtype).nexcept);
		if ((yyvsp[(7) - (7)].dtype).val)
		  Setattr((yyval.node),"value",(yyvsp[(7) - (7)].dtype).val);
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
		{
		  String *decl = NewStringEmpty();
		  SwigType_add_function(decl,(yyvsp[(5) - (7)].pl));
		  Setattr((yyval.node),"decl",decl);
		  Delete(decl);
		}

		add_symbols((yyval.node));
	      }
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 4440 "parser.y"
    {
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));

		 SwigType_add_function((yyvsp[(4) - (8)].type),(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push((yyvsp[(4) - (8)].type),(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",(yyvsp[(4) - (8)].type));
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
              }
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 4455 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));
		 decl = NewStringEmpty();
		 SwigType_add_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 4472 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));
		 decl = NewStringEmpty();
		 SwigType_add_rvalue_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 4490 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (9)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (9)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (9)].id));
		 decl = NewStringEmpty();
		 SwigType_add_pointer(decl);
		 SwigType_add_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(7) - (9)].pl));
		 if ((yyvsp[(9) - (9)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(9) - (9)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(7) - (9)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 4509 "parser.y"
    {
		String *t = NewStringEmpty();
		(yyval.node) = new_node("cdecl");
		Setattr((yyval.node),"type",(yyvsp[(3) - (7)].type));
		Setattr((yyval.node),"name",(yyvsp[(2) - (7)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (7)].id));
		SwigType_add_function(t,(yyvsp[(5) - (7)].pl));
		if ((yyvsp[(7) - (7)].dtype).qualifier) {
		  SwigType_push(t,(yyvsp[(7) - (7)].dtype).qualifier);
		}
		Setattr((yyval.node),"decl",t);
		Setattr((yyval.node),"parms",(yyvsp[(5) - (7)].pl));
		Setattr((yyval.node),"conversion_operator","1");
		add_symbols((yyval.node));
              }
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 4528 "parser.y"
    {
                 skip_balanced('{','}');
                 (yyval.node) = 0;
               }
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 4535 "parser.y"
    {
                skip_balanced('(',')');
                (yyval.node) = 0;
              }
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 4542 "parser.y"
    { 
                (yyval.node) = new_node("access");
		Setattr((yyval.node),"kind","public");
                cplus_mode = CPLUS_PUBLIC;
              }
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 4549 "parser.y"
    { 
                (yyval.node) = new_node("access");
                Setattr((yyval.node),"kind","private");
		cplus_mode = CPLUS_PRIVATE;
	      }
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 4557 "parser.y"
    { 
		(yyval.node) = new_node("access");
		Setattr((yyval.node),"kind","protected");
		cplus_mode = CPLUS_PROTECTED;
	      }
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 4565 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 4568 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 4572 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 4575 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 4576 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 4577 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 4578 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 240:

/* Line 1464 of yacc.c  */
#line 4579 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 4580 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 4581 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 4582 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 4585 "parser.y"
    {
	            Clear(scanner_ccode);
		    (yyval.dtype).val = 0;
		    (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
		    (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
		    (yyval.dtype).nexcept = (yyvsp[(1) - (2)].dtype).nexcept;
               }
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 4592 "parser.y"
    {
	            Clear(scanner_ccode);
		    (yyval.dtype).val = (yyvsp[(3) - (4)].dtype).val;
		    (yyval.dtype).throws = (yyvsp[(1) - (4)].dtype).throws;
		    (yyval.dtype).throwf = (yyvsp[(1) - (4)].dtype).throwf;
		    (yyval.dtype).nexcept = (yyvsp[(1) - (4)].dtype).nexcept;
               }
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 4599 "parser.y"
    { 
		    skip_balanced('{','}'); 
		    (yyval.dtype).val = 0;
		    (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
		    (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
		    (yyval.dtype).nexcept = (yyvsp[(1) - (2)].dtype).nexcept;
	       }
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 4608 "parser.y"
    { 
                     Clear(scanner_ccode);
                     (yyval.dtype).val = 0;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (2)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
                     (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
                     (yyval.dtype).nexcept = (yyvsp[(1) - (2)].dtype).nexcept;
                }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 4617 "parser.y"
    { 
                     Clear(scanner_ccode);
                     (yyval.dtype).val = (yyvsp[(3) - (4)].dtype).val;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (4)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (4)].dtype).throws; 
                     (yyval.dtype).throwf = (yyvsp[(1) - (4)].dtype).throwf; 
                     (yyval.dtype).nexcept = (yyvsp[(1) - (4)].dtype).nexcept; 
               }
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 4626 "parser.y"
    { 
                     skip_balanced('{','}');
                     (yyval.dtype).val = 0;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (2)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws; 
                     (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf; 
                     (yyval.dtype).nexcept = (yyvsp[(1) - (2)].dtype).nexcept; 
               }
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 4638 "parser.y"
    { }
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 4641 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type);
                  /* Printf(stdout,"primitive = '%s'\n", $$);*/
                }
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 4644 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 4645 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 4646 "parser.y"
    { (yyval.type) = NewStringf("%s%s",(yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].id)); }
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 4647 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 4649 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (1)].str);
               }
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 4657 "parser.y"
    {
                   if (strcmp((yyvsp[(2) - (2)].id),"C") == 0) {
		     (yyval.id) = "externc";
                   } else if (strcmp((yyvsp[(2) - (2)].id),"C++") == 0) {
		     (yyval.id) = "extern";
		   } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\".\n", (yyvsp[(2) - (2)].id));
		     (yyval.id) = 0;
		   }
               }
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 4669 "parser.y"
    { (yyval.id) = "extern"; }
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 4670 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 4671 "parser.y"
    { (yyval.id) = "thread_local"; }
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 4672 "parser.y"
    { (yyval.id) = "typedef"; }
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 4673 "parser.y"
    { (yyval.id) = "static"; }
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 4674 "parser.y"
    { (yyval.id) = "typedef"; }
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 4675 "parser.y"
    { (yyval.id) = "virtual"; }
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 4676 "parser.y"
    { (yyval.id) = "friend"; }
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 4677 "parser.y"
    { (yyval.id) = "explicit"; }
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 4678 "parser.y"
    { (yyval.id) = "constexpr"; }
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 4679 "parser.y"
    { (yyval.id) = "explicit constexpr"; }
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 4680 "parser.y"
    { (yyval.id) = "explicit constexpr"; }
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 4681 "parser.y"
    { (yyval.id) = "static constexpr"; }
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 4682 "parser.y"
    { (yyval.id) = "static constexpr"; }
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 4683 "parser.y"
    { (yyval.id) = "thread_local"; }
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 4684 "parser.y"
    { (yyval.id) = "static thread_local"; }
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 4685 "parser.y"
    { (yyval.id) = "static thread_local"; }
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 4686 "parser.y"
    { (yyval.id) = "extern thread_local"; }
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 4687 "parser.y"
    { (yyval.id) = "extern thread_local"; }
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 4688 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 4695 "parser.y"
    {
                 Parm *p;
		 (yyval.pl) = (yyvsp[(1) - (1)].pl);
		 p = (yyvsp[(1) - (1)].pl);
                 while (p) {
		   Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   p = nextSibling(p);
                 }
               }
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 4706 "parser.y"
    {
                  set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].pl));
                  (yyval.pl) = (yyvsp[(1) - (2)].p);
		}
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 4710 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 4713 "parser.y"
    {
                 set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].pl));
		 (yyval.pl) = (yyvsp[(2) - (3)].p);
                }
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 4717 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 4721 "parser.y"
    {
                   SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		   (yyval.p) = NewParmWithoutFileLineInfo((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).id);
		   Setfile((yyval.p),cparse_file);
		   Setline((yyval.p),cparse_line);
		   if ((yyvsp[(2) - (2)].decl).defarg) {
		     Setattr((yyval.p),"value",(yyvsp[(2) - (2)].decl).defarg);
		   }
		}
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 4731 "parser.y"
    {
                  (yyval.p) = NewParmWithoutFileLineInfo(NewStringf("template<class> %s %s", (yyvsp[(5) - (7)].id),(yyvsp[(6) - (7)].str)), 0);
		  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
                  if ((yyvsp[(7) - (7)].dtype).val) {
                    Setattr((yyval.p),"value",(yyvsp[(7) - (7)].dtype).val);
                  }
                }
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 4739 "parser.y"
    {
		  SwigType *t = NewString("v(...)");
		  (yyval.p) = NewParmWithoutFileLineInfo(t, 0);
		  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
		}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 4747 "parser.y"
    {
                 Parm *p;
		 (yyval.p) = (yyvsp[(1) - (1)].p);
		 p = (yyvsp[(1) - (1)].p);
                 while (p) {
		   if (Getattr(p,"type")) {
		     Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   }
		   p = nextSibling(p);
                 }
               }
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 4760 "parser.y"
    {
                  set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].p));
                  (yyval.p) = (yyvsp[(1) - (2)].p);
		}
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 4764 "parser.y"
    { (yyval.p) = 0; }
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 4767 "parser.y"
    {
                 set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].p));
		 (yyval.p) = (yyvsp[(2) - (3)].p);
                }
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 4771 "parser.y"
    { (yyval.p) = 0; }
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 4775 "parser.y"
    {
		  (yyval.p) = (yyvsp[(1) - (1)].p);
		  {
		    /* We need to make a possible adjustment for integer parameters. */
		    SwigType *type;
		    Node     *n = 0;

		    while (!n) {
		      type = Getattr((yyvsp[(1) - (1)].p),"type");
		      n = Swig_symbol_clookup(type,0);     /* See if we can find a node that matches the typename */
		      if ((n) && (Strcmp(nodeType(n),"cdecl") == 0)) {
			SwigType *decl = Getattr(n,"decl");
			if (!SwigType_isfunction(decl)) {
			  String *value = Getattr(n,"value");
			  if (value) {
			    String *v = Copy(value);
			    Setattr((yyvsp[(1) - (1)].p),"type",v);
			    Delete(v);
			    n = 0;
			  }
			}
		      } else {
			break;
		      }
		    }
		  }

               }
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 4803 "parser.y"
    {
                  (yyval.p) = NewParmWithoutFileLineInfo(0,0);
                  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
		  Setattr((yyval.p),"value",(yyvsp[(1) - (1)].dtype).val);
               }
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 4811 "parser.y"
    { 
                  (yyval.dtype) = (yyvsp[(2) - (2)].dtype); 
		  if ((yyvsp[(2) - (2)].dtype).type == T_ERROR) {
		    Swig_warning(WARN_PARSE_BAD_DEFAULT,cparse_file, cparse_line, "Can't set default argument (ignored)\n");
		    (yyval.dtype).val = 0;
		    (yyval.dtype).rawval = 0;
		    (yyval.dtype).bitfield = 0;
		    (yyval.dtype).throws = 0;
		    (yyval.dtype).throwf = 0;
		    (yyval.dtype).nexcept = 0;
		  }
               }
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 4823 "parser.y"
    { 
		  (yyval.dtype) = (yyvsp[(2) - (5)].dtype);
		  if ((yyvsp[(2) - (5)].dtype).type == T_ERROR) {
		    Swig_warning(WARN_PARSE_BAD_DEFAULT,cparse_file, cparse_line, "Can't set default argument (ignored)\n");
		    (yyval.dtype) = (yyvsp[(2) - (5)].dtype);
		    (yyval.dtype).val = 0;
		    (yyval.dtype).rawval = 0;
		    (yyval.dtype).bitfield = 0;
		    (yyval.dtype).throws = 0;
		    (yyval.dtype).throwf = 0;
		    (yyval.dtype).nexcept = 0;
		  } else {
		    (yyval.dtype).val = NewStringf("%s[%s]",(yyvsp[(2) - (5)].dtype).val,(yyvsp[(4) - (5)].dtype).val); 
		  }		  
               }
    break;

  case 295:

/* Line 1464 of yacc.c  */
#line 4838 "parser.y"
    {
		 skip_balanced('{','}');
		 (yyval.dtype).val = NewString(scanner_ccode);
		 (yyval.dtype).rawval = 0;
                 (yyval.dtype).type = T_INT;
		 (yyval.dtype).bitfield = 0;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
		 (yyval.dtype).nexcept = 0;
	       }
    break;

  case 296:

/* Line 1464 of yacc.c  */
#line 4848 "parser.y"
    { 
		 (yyval.dtype).val = 0;
		 (yyval.dtype).rawval = 0;
		 (yyval.dtype).type = 0;
		 (yyval.dtype).bitfield = (yyvsp[(2) - (2)].dtype).val;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
		 (yyval.dtype).nexcept = 0;
	       }
    break;

  case 297:

/* Line 1464 of yacc.c  */
#line 4857 "parser.y"
    {
                 (yyval.dtype).val = 0;
                 (yyval.dtype).rawval = 0;
                 (yyval.dtype).type = T_INT;
		 (yyval.dtype).bitfield = 0;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
		 (yyval.dtype).nexcept = 0;
               }
    break;

  case 298:

/* Line 1464 of yacc.c  */
#line 4868 "parser.y"
    {
                 (yyval.decl) = (yyvsp[(1) - (2)].decl);
		 (yyval.decl).defarg = (yyvsp[(2) - (2)].dtype).rawval ? (yyvsp[(2) - (2)].dtype).rawval : (yyvsp[(2) - (2)].dtype).val;
            }
    break;

  case 299:

/* Line 1464 of yacc.c  */
#line 4872 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (2)].decl);
	      (yyval.decl).defarg = (yyvsp[(2) - (2)].dtype).rawval ? (yyvsp[(2) - (2)].dtype).rawval : (yyvsp[(2) - (2)].dtype).val;
            }
    break;

  case 300:

/* Line 1464 of yacc.c  */
#line 4876 "parser.y"
    {
   	      (yyval.decl).type = 0;
              (yyval.decl).id = 0;
	      (yyval.decl).defarg = (yyvsp[(1) - (1)].dtype).rawval ? (yyvsp[(1) - (1)].dtype).rawval : (yyvsp[(1) - (1)].dtype).val;
            }
    break;

  case 301:

/* Line 1464 of yacc.c  */
#line 4883 "parser.y"
    {
                 (yyval.decl) = (yyvsp[(1) - (1)].decl);
		 if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		   Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		 } else if (SwigType_isarray((yyvsp[(1) - (1)].decl).type)) {
		   SwigType *ta = SwigType_pop_arrays((yyvsp[(1) - (1)].decl).type);
		   if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		     Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		   } else {
		     (yyval.decl).parms = 0;
		   }
		   SwigType_push((yyvsp[(1) - (1)].decl).type,ta);
		   Delete(ta);
		 } else {
		   (yyval.decl).parms = 0;
		 }
            }
    break;

  case 302:

/* Line 1464 of yacc.c  */
#line 4900 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (1)].decl);
	      if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
	      } else if (SwigType_isarray((yyvsp[(1) - (1)].decl).type)) {
		SwigType *ta = SwigType_pop_arrays((yyvsp[(1) - (1)].decl).type);
		if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		  Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		} else {
		  (yyval.decl).parms = 0;
		}
		SwigType_push((yyvsp[(1) - (1)].decl).type,ta);
		Delete(ta);
	      } else {
		(yyval.decl).parms = 0;
	      }
            }
    break;

  case 303:

/* Line 1464 of yacc.c  */
#line 4917 "parser.y"
    {
   	      (yyval.decl).type = 0;
              (yyval.decl).id = 0;
	      (yyval.decl).parms = 0;
	      }
    break;

  case 304:

/* Line 1464 of yacc.c  */
#line 4925 "parser.y"
    {
              (yyval.decl) = (yyvsp[(2) - (2)].decl);
	      if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (2)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (2)].type);
           }
    break;

  case 305:

/* Line 1464 of yacc.c  */
#line 4933 "parser.y"
    {
              (yyval.decl) = (yyvsp[(3) - (3)].decl);
	      SwigType_add_reference((yyvsp[(1) - (3)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (3)].type);
           }
    break;

  case 306:

/* Line 1464 of yacc.c  */
#line 4942 "parser.y"
    {
              (yyval.decl) = (yyvsp[(3) - (3)].decl);
	      SwigType_add_rvalue_reference((yyvsp[(1) - (3)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (3)].type);
           }
    break;

  case 307:

/* Line 1464 of yacc.c  */
#line 4951 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (1)].decl);
	      if (!(yyval.decl).type) (yyval.decl).type = NewStringEmpty();
           }
    break;

  case 308:

/* Line 1464 of yacc.c  */
#line 4955 "parser.y"
    {
	     (yyval.decl) = (yyvsp[(2) - (2)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_reference((yyval.decl).type);
	     if ((yyvsp[(2) - (2)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
	       Delete((yyvsp[(2) - (2)].decl).type);
	     }
           }
    break;

  case 309:

/* Line 1464 of yacc.c  */
#line 4964 "parser.y"
    {
	     /* Introduced in C++11, move operator && */
             /* Adds one S/R conflict */
	     (yyval.decl) = (yyvsp[(2) - (2)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_rvalue_reference((yyval.decl).type);
	     if ((yyvsp[(2) - (2)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
	       Delete((yyvsp[(2) - (2)].decl).type);
	     }
           }
    break;

  case 310:

/* Line 1464 of yacc.c  */
#line 4975 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();

	     (yyval.decl) = (yyvsp[(3) - (3)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (3)].str));
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = t;
	     }
    break;

  case 311:

/* Line 1464 of yacc.c  */
#line 4986 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(4) - (4)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(2) - (4)].str));
	     SwigType_push((yyvsp[(1) - (4)].type),t);
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (4)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (4)].type);
	     Delete(t);
	   }
    break;

  case 312:

/* Line 1464 of yacc.c  */
#line 4998 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(5) - (5)].decl);
	     SwigType_add_memberpointer((yyvsp[(1) - (5)].type),(yyvsp[(2) - (5)].str));
	     SwigType_add_reference((yyvsp[(1) - (5)].type));
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (5)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (5)].type);
	   }
    break;

  case 313:

/* Line 1464 of yacc.c  */
#line 5008 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(4) - (4)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (4)].str));
	     SwigType_add_reference(t);
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     } 
	     (yyval.decl).type = t;
	   }
    break;

  case 314:

/* Line 1464 of yacc.c  */
#line 5022 "parser.y"
    {
              (yyval.decl) = (yyvsp[(5) - (5)].decl);
	      if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (5)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (5)].type);
           }
    break;

  case 315:

/* Line 1464 of yacc.c  */
#line 5030 "parser.y"
    {
              (yyval.decl) = (yyvsp[(6) - (6)].decl);
	      SwigType_add_reference((yyvsp[(1) - (6)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (6)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (6)].type);
           }
    break;

  case 316:

/* Line 1464 of yacc.c  */
#line 5039 "parser.y"
    {
              (yyval.decl) = (yyvsp[(6) - (6)].decl);
	      SwigType_add_rvalue_reference((yyvsp[(1) - (6)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (6)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (6)].type);
           }
    break;

  case 317:

/* Line 1464 of yacc.c  */
#line 5048 "parser.y"
    {
              (yyval.decl) = (yyvsp[(4) - (4)].decl);
	      if (!(yyval.decl).type) (yyval.decl).type = NewStringEmpty();
           }
    break;

  case 318:

/* Line 1464 of yacc.c  */
#line 5052 "parser.y"
    {
	     (yyval.decl) = (yyvsp[(5) - (5)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_reference((yyval.decl).type);
	     if ((yyvsp[(5) - (5)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(5) - (5)].decl).type);
	       Delete((yyvsp[(5) - (5)].decl).type);
	     }
           }
    break;

  case 319:

/* Line 1464 of yacc.c  */
#line 5061 "parser.y"
    {
	     /* Introduced in C++11, move operator && */
             /* Adds one S/R conflict */
	     (yyval.decl) = (yyvsp[(5) - (5)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_rvalue_reference((yyval.decl).type);
	     if ((yyvsp[(5) - (5)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(5) - (5)].decl).type);
	       Delete((yyvsp[(5) - (5)].decl).type);
	     }
           }
    break;

  case 320:

/* Line 1464 of yacc.c  */
#line 5072 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();

	     (yyval.decl) = (yyvsp[(6) - (6)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (6)].str));
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = t;
	     }
    break;

  case 321:

/* Line 1464 of yacc.c  */
#line 5083 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(7) - (7)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(2) - (7)].str));
	     SwigType_push((yyvsp[(1) - (7)].type),t);
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (7)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (7)].type);
	     Delete(t);
	   }
    break;

  case 322:

/* Line 1464 of yacc.c  */
#line 5095 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(8) - (8)].decl);
	     SwigType_add_memberpointer((yyvsp[(1) - (8)].type),(yyvsp[(2) - (8)].str));
	     SwigType_add_reference((yyvsp[(1) - (8)].type));
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (8)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (8)].type);
	   }
    break;

  case 323:

/* Line 1464 of yacc.c  */
#line 5105 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(8) - (8)].decl);
	     SwigType_add_memberpointer((yyvsp[(1) - (8)].type),(yyvsp[(2) - (8)].str));
	     SwigType_add_rvalue_reference((yyvsp[(1) - (8)].type));
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (8)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (8)].type);
	   }
    break;

  case 324:

/* Line 1464 of yacc.c  */
#line 5115 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(7) - (7)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (7)].str));
	     SwigType_add_reference(t);
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     } 
	     (yyval.decl).type = t;
	   }
    break;

  case 325:

/* Line 1464 of yacc.c  */
#line 5126 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(7) - (7)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (7)].str));
	     SwigType_add_rvalue_reference(t);
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     } 
	     (yyval.decl).type = t;
	   }
    break;

  case 326:

/* Line 1464 of yacc.c  */
#line 5139 "parser.y"
    {
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 (yyval.decl).id = Char((yyvsp[(1) - (1)].str));
		 (yyval.decl).type = 0;
		 (yyval.decl).parms = 0;
		 (yyval.decl).have_parms = 0;
                  }
    break;

  case 327:

/* Line 1464 of yacc.c  */
#line 5146 "parser.y"
    {
                  (yyval.decl).id = Char(NewStringf("~%s",(yyvsp[(2) - (2)].str)));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 328:

/* Line 1464 of yacc.c  */
#line 5154 "parser.y"
    {
                  (yyval.decl).id = Char((yyvsp[(2) - (3)].str));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 329:

/* Line 1464 of yacc.c  */
#line 5170 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(2) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(2) - (4)].type);
                  }
    break;

  case 330:

/* Line 1464 of yacc.c  */
#line 5178 "parser.y"
    {
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(4) - (5)].decl);
		    t = NewStringEmpty();
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (5)].str));
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
		    }
    break;

  case 331:

/* Line 1464 of yacc.c  */
#line 5189 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 332:

/* Line 1464 of yacc.c  */
#line 5200 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 333:

/* Line 1464 of yacc.c  */
#line 5211 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		  }
    break;

  case 334:

/* Line 1464 of yacc.c  */
#line 5230 "parser.y"
    {
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 (yyval.decl).id = Char((yyvsp[(1) - (1)].str));
		 (yyval.decl).type = 0;
		 (yyval.decl).parms = 0;
		 (yyval.decl).have_parms = 0;
                  }
    break;

  case 335:

/* Line 1464 of yacc.c  */
#line 5238 "parser.y"
    {
                  (yyval.decl).id = Char(NewStringf("~%s",(yyvsp[(2) - (2)].str)));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 336:

/* Line 1464 of yacc.c  */
#line 5255 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(2) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(2) - (4)].type);
                  }
    break;

  case 337:

/* Line 1464 of yacc.c  */
#line 5263 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = NewStringEmpty();
		    }
		    SwigType_add_reference((yyval.decl).type);
                  }
    break;

  case 338:

/* Line 1464 of yacc.c  */
#line 5270 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = NewStringEmpty();
		    }
		    SwigType_add_rvalue_reference((yyval.decl).type);
                  }
    break;

  case 339:

/* Line 1464 of yacc.c  */
#line 5277 "parser.y"
    {
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(4) - (5)].decl);
		    t = NewStringEmpty();
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (5)].str));
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
		    }
    break;

  case 340:

/* Line 1464 of yacc.c  */
#line 5288 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 341:

/* Line 1464 of yacc.c  */
#line 5299 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 342:

/* Line 1464 of yacc.c  */
#line 5310 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
                 }
    break;

  case 343:

/* Line 1464 of yacc.c  */
#line 5330 "parser.y"
    {
		    SwigType *t;
                    Append((yyvsp[(1) - (5)].str), " "); /* intervening space is mandatory */
                    Append((yyvsp[(1) - (5)].str), Char((yyvsp[(2) - (5)].id)));
		    (yyval.decl).id = Char((yyvsp[(1) - (5)].str));
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(4) - (5)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(4) - (5)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		  }
    break;

  case 344:

/* Line 1464 of yacc.c  */
#line 5351 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (1)].type);
                    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                  }
    break;

  case 345:

/* Line 1464 of yacc.c  */
#line 5357 "parser.y"
    { 
                     (yyval.decl) = (yyvsp[(2) - (2)].decl);
                     SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		     (yyval.decl).type = (yyvsp[(1) - (2)].type);
		     Delete((yyvsp[(2) - (2)].decl).type);
                  }
    break;

  case 346:

/* Line 1464 of yacc.c  */
#line 5363 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (2)].type);
		    SwigType_add_reference((yyval.decl).type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		  }
    break;

  case 347:

/* Line 1464 of yacc.c  */
#line 5370 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (2)].type);
		    SwigType_add_rvalue_reference((yyval.decl).type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		  }
    break;

  case 348:

/* Line 1464 of yacc.c  */
#line 5377 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (3)].decl);
		    SwigType_add_reference((yyvsp[(1) - (3)].type));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (3)].type);
                  }
    break;

  case 349:

/* Line 1464 of yacc.c  */
#line 5386 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (3)].decl);
		    SwigType_add_rvalue_reference((yyvsp[(1) - (3)].type));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (3)].type);
                  }
    break;

  case 350:

/* Line 1464 of yacc.c  */
#line 5395 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(1) - (1)].decl);
                  }
    break;

  case 351:

/* Line 1464 of yacc.c  */
#line 5398 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(2) - (2)].decl);
		    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_reference((yyval.decl).type);
		    if ((yyvsp[(2) - (2)].decl).type) {
		      SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
		      Delete((yyvsp[(2) - (2)].decl).type);
		    }
                  }
    break;

  case 352:

/* Line 1464 of yacc.c  */
#line 5407 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(2) - (2)].decl);
		    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_rvalue_reference((yyval.decl).type);
		    if ((yyvsp[(2) - (2)].decl).type) {
		      SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
		      Delete((yyvsp[(2) - (2)].decl).type);
		    }
                  }
    break;

  case 353:

/* Line 1464 of yacc.c  */
#line 5416 "parser.y"
    {
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_reference((yyval.decl).type);
                  }
    break;

  case 354:

/* Line 1464 of yacc.c  */
#line 5423 "parser.y"
    {
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_rvalue_reference((yyval.decl).type);
                  }
    break;

  case 355:

/* Line 1464 of yacc.c  */
#line 5430 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
                    SwigType_add_memberpointer((yyval.decl).type,(yyvsp[(1) - (2)].str));
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
      	          }
    break;

  case 356:

/* Line 1464 of yacc.c  */
#line 5437 "parser.y"
    { 
		    SwigType *t = NewStringEmpty();
                    (yyval.decl).type = (yyvsp[(1) - (3)].type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (3)].str));
		    SwigType_push((yyval.decl).type,t);
		    Delete(t);
                  }
    break;

  case 357:

/* Line 1464 of yacc.c  */
#line 5447 "parser.y"
    { 
		    (yyval.decl) = (yyvsp[(4) - (4)].decl);
		    SwigType_add_memberpointer((yyvsp[(1) - (4)].type),(yyvsp[(2) - (4)].str));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (4)].type);
                  }
    break;

  case 358:

/* Line 1464 of yacc.c  */
#line 5458 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 359:

/* Line 1464 of yacc.c  */
#line 5469 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 360:

/* Line 1464 of yacc.c  */
#line 5480 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_array((yyval.decl).type,"");
                  }
    break;

  case 361:

/* Line 1464 of yacc.c  */
#line 5487 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_array((yyval.decl).type,(yyvsp[(2) - (3)].dtype).val);
		  }
    break;

  case 362:

/* Line 1464 of yacc.c  */
#line 5494 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(2) - (3)].decl);
		  }
    break;

  case 363:

/* Line 1464 of yacc.c  */
#line 5497 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
                    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		  }
    break;

  case 364:

/* Line 1464 of yacc.c  */
#line 5514 "parser.y"
    {
                    (yyval.decl).type = NewStringEmpty();
                    SwigType_add_function((yyval.decl).type,(yyvsp[(2) - (3)].pl));
		    (yyval.decl).parms = (yyvsp[(2) - (3)].pl);
		    (yyval.decl).have_parms = 1;
		    (yyval.decl).id = 0;
                  }
    break;

  case 365:

/* Line 1464 of yacc.c  */
#line 5524 "parser.y"
    { 
             (yyval.type) = NewStringEmpty();
             SwigType_add_pointer((yyval.type));
	     SwigType_push((yyval.type),(yyvsp[(2) - (3)].str));
	     SwigType_push((yyval.type),(yyvsp[(3) - (3)].type));
	     Delete((yyvsp[(3) - (3)].type));
           }
    break;

  case 366:

/* Line 1464 of yacc.c  */
#line 5531 "parser.y"
    {
	     (yyval.type) = NewStringEmpty();
	     SwigType_add_pointer((yyval.type));
	     SwigType_push((yyval.type),(yyvsp[(2) - (2)].type));
	     Delete((yyvsp[(2) - (2)].type));
	   }
    break;

  case 367:

/* Line 1464 of yacc.c  */
#line 5537 "parser.y"
    { 
	     (yyval.type) = NewStringEmpty();
	     SwigType_add_pointer((yyval.type));
	     SwigType_push((yyval.type),(yyvsp[(2) - (2)].str));
           }
    break;

  case 368:

/* Line 1464 of yacc.c  */
#line 5542 "parser.y"
    {
	     (yyval.type) = NewStringEmpty();
	     SwigType_add_pointer((yyval.type));
           }
    break;

  case 369:

/* Line 1464 of yacc.c  */
#line 5548 "parser.y"
    {
	          (yyval.str) = NewStringEmpty();
	          if ((yyvsp[(1) - (1)].id)) SwigType_add_qualifier((yyval.str),(yyvsp[(1) - (1)].id));
               }
    break;

  case 370:

/* Line 1464 of yacc.c  */
#line 5552 "parser.y"
    {
		  (yyval.str) = (yyvsp[(2) - (2)].str);
	          if ((yyvsp[(1) - (2)].id)) SwigType_add_qualifier((yyval.str),(yyvsp[(1) - (2)].id));
               }
    break;

  case 371:

/* Line 1464 of yacc.c  */
#line 5558 "parser.y"
    { (yyval.id) = "const"; }
    break;

  case 372:

/* Line 1464 of yacc.c  */
#line 5559 "parser.y"
    { (yyval.id) = "volatile"; }
    break;

  case 373:

/* Line 1464 of yacc.c  */
#line 5560 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 374:

/* Line 1464 of yacc.c  */
#line 5566 "parser.y"
    {
                   (yyval.type) = (yyvsp[(1) - (1)].type);
                   Replace((yyval.type),"typename ","", DOH_REPLACE_ANY);
                }
    break;

  case 375:

/* Line 1464 of yacc.c  */
#line 5572 "parser.y"
    {
                   (yyval.type) = (yyvsp[(2) - (2)].type);
	           SwigType_push((yyval.type),(yyvsp[(1) - (2)].str));
               }
    break;

  case 376:

/* Line 1464 of yacc.c  */
#line 5576 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 377:

/* Line 1464 of yacc.c  */
#line 5577 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (2)].type);
	          SwigType_push((yyval.type),(yyvsp[(2) - (2)].str));
	       }
    break;

  case 378:

/* Line 1464 of yacc.c  */
#line 5581 "parser.y"
    {
		  (yyval.type) = (yyvsp[(2) - (3)].type);
	          SwigType_push((yyval.type),(yyvsp[(3) - (3)].str));
	          SwigType_push((yyval.type),(yyvsp[(1) - (3)].str));
	       }
    break;

  case 379:

/* Line 1464 of yacc.c  */
#line 5588 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type);
                  /* Printf(stdout,"primitive = '%s'\n", $$);*/
               }
    break;

  case 380:

/* Line 1464 of yacc.c  */
#line 5591 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 381:

/* Line 1464 of yacc.c  */
#line 5592 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 382:

/* Line 1464 of yacc.c  */
#line 5593 "parser.y"
    { (yyval.type) = NewStringf("%s%s",(yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].id)); }
    break;

  case 383:

/* Line 1464 of yacc.c  */
#line 5594 "parser.y"
    { (yyval.type) = NewStringf("enum %s", (yyvsp[(2) - (2)].str)); }
    break;

  case 384:

/* Line 1464 of yacc.c  */
#line 5595 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 385:

/* Line 1464 of yacc.c  */
#line 5597 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (1)].str);
               }
    break;

  case 386:

/* Line 1464 of yacc.c  */
#line 5600 "parser.y"
    { 
		 (yyval.type) = NewStringf("%s %s", (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].str));
               }
    break;

  case 387:

/* Line 1464 of yacc.c  */
#line 5603 "parser.y"
    {
                 (yyval.type) = (yyvsp[(1) - (1)].type);
               }
    break;

  case 388:

/* Line 1464 of yacc.c  */
#line 5608 "parser.y"
    {
                 Node *n = Swig_symbol_clookup((yyvsp[(3) - (4)].str),0);
                 if (!n) {
		   Swig_error(cparse_file, cparse_line, "Identifier %s not defined.\n", (yyvsp[(3) - (4)].str));
                   (yyval.type) = (yyvsp[(3) - (4)].str);
                 } else {
                   (yyval.type) = Getattr(n, "type");
                 }
               }
    break;

  case 389:

/* Line 1464 of yacc.c  */
#line 5619 "parser.y"
    {
		 if (!(yyvsp[(1) - (1)].ptype).type) (yyvsp[(1) - (1)].ptype).type = NewString("int");
		 if ((yyvsp[(1) - (1)].ptype).us) {
		   (yyval.type) = NewStringf("%s %s", (yyvsp[(1) - (1)].ptype).us, (yyvsp[(1) - (1)].ptype).type);
		   Delete((yyvsp[(1) - (1)].ptype).us);
                   Delete((yyvsp[(1) - (1)].ptype).type);
		 } else {
                   (yyval.type) = (yyvsp[(1) - (1)].ptype).type;
		 }
		 if (Cmp((yyval.type),"signed int") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("int");
                 } else if (Cmp((yyval.type),"signed long") == 0) {
		   Delete((yyval.type));
                   (yyval.type) = NewString("long");
                 } else if (Cmp((yyval.type),"signed short") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("short");
		 } else if (Cmp((yyval.type),"signed long long") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("long long");
		 }
               }
    break;

  case 390:

/* Line 1464 of yacc.c  */
#line 5644 "parser.y"
    { 
                 (yyval.ptype) = (yyvsp[(1) - (1)].ptype);
               }
    break;

  case 391:

/* Line 1464 of yacc.c  */
#line 5647 "parser.y"
    {
                    if ((yyvsp[(1) - (2)].ptype).us && (yyvsp[(2) - (2)].ptype).us) {
		      Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", (yyvsp[(2) - (2)].ptype).us);
		    }
                    (yyval.ptype) = (yyvsp[(2) - (2)].ptype);
                    if ((yyvsp[(1) - (2)].ptype).us) (yyval.ptype).us = (yyvsp[(1) - (2)].ptype).us;
		    if ((yyvsp[(1) - (2)].ptype).type) {
		      if (!(yyvsp[(2) - (2)].ptype).type) (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
		      else {
			int err = 0;
			if ((Cmp((yyvsp[(1) - (2)].ptype).type,"long") == 0)) {
			  if ((Cmp((yyvsp[(2) - (2)].ptype).type,"long") == 0) || (Strncmp((yyvsp[(2) - (2)].ptype).type,"double",6) == 0)) {
			    (yyval.ptype).type = NewStringf("long %s", (yyvsp[(2) - (2)].ptype).type);
			  } else if (Cmp((yyvsp[(2) - (2)].ptype).type,"int") == 0) {
			    (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
			  } else {
			    err = 1;
			  }
			} else if ((Cmp((yyvsp[(1) - (2)].ptype).type,"short")) == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"int") == 0) {
			    (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"int") == 0) {
			  (yyval.ptype).type = (yyvsp[(2) - (2)].ptype).type;
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"double") == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"long") == 0) {
			    (yyval.ptype).type = NewString("long double");
			  } else if (Cmp((yyvsp[(2) - (2)].ptype).type,"complex") == 0) {
			    (yyval.ptype).type = NewString("double complex");
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"float") == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"complex") == 0) {
			    (yyval.ptype).type = NewString("float complex");
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"complex") == 0) {
			  (yyval.ptype).type = NewStringf("%s complex", (yyvsp[(2) - (2)].ptype).type);
			} else {
			  err = 1;
			}
			if (err) {
			  Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", (yyvsp[(1) - (2)].ptype).type);
			}
		      }
		    }
               }
    break;

  case 392:

/* Line 1464 of yacc.c  */
#line 5701 "parser.y"
    { 
		    (yyval.ptype).type = NewString("int");
                    (yyval.ptype).us = 0;
               }
    break;

  case 393:

/* Line 1464 of yacc.c  */
#line 5705 "parser.y"
    { 
                    (yyval.ptype).type = NewString("short");
                    (yyval.ptype).us = 0;
                }
    break;

  case 394:

/* Line 1464 of yacc.c  */
#line 5709 "parser.y"
    { 
                    (yyval.ptype).type = NewString("long");
                    (yyval.ptype).us = 0;
                }
    break;

  case 395:

/* Line 1464 of yacc.c  */
#line 5713 "parser.y"
    { 
                    (yyval.ptype).type = NewString("char");
                    (yyval.ptype).us = 0;
                }
    break;

  case 396:

/* Line 1464 of yacc.c  */
#line 5717 "parser.y"
    { 
                    (yyval.ptype).type = NewString("wchar_t");
                    (yyval.ptype).us = 0;
                }
    break;

  case 397:

/* Line 1464 of yacc.c  */
#line 5721 "parser.y"
    { 
                    (yyval.ptype).type = NewString("float");
                    (yyval.ptype).us = 0;
                }
    break;

  case 398:

/* Line 1464 of yacc.c  */
#line 5725 "parser.y"
    { 
                    (yyval.ptype).type = NewString("double");
                    (yyval.ptype).us = 0;
                }
    break;

  case 399:

/* Line 1464 of yacc.c  */
#line 5729 "parser.y"
    { 
                    (yyval.ptype).us = NewString("signed");
                    (yyval.ptype).type = 0;
                }
    break;

  case 400:

/* Line 1464 of yacc.c  */
#line 5733 "parser.y"
    { 
                    (yyval.ptype).us = NewString("unsigned");
                    (yyval.ptype).type = 0;
                }
    break;

  case 401:

/* Line 1464 of yacc.c  */
#line 5737 "parser.y"
    { 
                    (yyval.ptype).type = NewString("complex");
                    (yyval.ptype).us = 0;
                }
    break;

  case 402:

/* Line 1464 of yacc.c  */
#line 5741 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int8");
                    (yyval.ptype).us = 0;
                }
    break;

  case 403:

/* Line 1464 of yacc.c  */
#line 5745 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int16");
                    (yyval.ptype).us = 0;
                }
    break;

  case 404:

/* Line 1464 of yacc.c  */
#line 5749 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int32");
                    (yyval.ptype).us = 0;
                }
    break;

  case 405:

/* Line 1464 of yacc.c  */
#line 5753 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int64");
                    (yyval.ptype).us = 0;
                }
    break;

  case 406:

/* Line 1464 of yacc.c  */
#line 5759 "parser.y"
    { /* scanner_check_typedef(); */ }
    break;

  case 407:

/* Line 1464 of yacc.c  */
#line 5759 "parser.y"
    {
                   (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
		   if ((yyval.dtype).type == T_STRING) {
		     (yyval.dtype).rawval = NewStringf("\"%(escape)s\"",(yyval.dtype).val);
		   } else if ((yyval.dtype).type != T_CHAR && (yyval.dtype).type != T_WSTRING && (yyval.dtype).type != T_WCHAR) {
		     (yyval.dtype).rawval = 0;
		   }
		   (yyval.dtype).qualifier = 0;
		   (yyval.dtype).bitfield = 0;
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
		   (yyval.dtype).nexcept = 0;
		   scanner_ignore_typedef();
                }
    break;

  case 408:

/* Line 1464 of yacc.c  */
#line 5773 "parser.y"
    {
		  (yyval.dtype) = (yyvsp[(1) - (1)].dtype);
		}
    break;

  case 409:

/* Line 1464 of yacc.c  */
#line 5789 "parser.y"
    {
		  (yyval.dtype) = (yyvsp[(1) - (1)].dtype);
		}
    break;

  case 410:

/* Line 1464 of yacc.c  */
#line 5792 "parser.y"
    {
		  (yyval.dtype) = (yyvsp[(1) - (1)].dtype);
		}
    break;

  case 411:

/* Line 1464 of yacc.c  */
#line 5798 "parser.y"
    {
		  (yyval.dtype).val = NewString("delete");
		  (yyval.dtype).rawval = 0;
		  (yyval.dtype).type = T_STRING;
		  (yyval.dtype).qualifier = 0;
		  (yyval.dtype).bitfield = 0;
		  (yyval.dtype).throws = 0;
		  (yyval.dtype).throwf = 0;
		  (yyval.dtype).nexcept = 0;
		}
    break;

  case 412:

/* Line 1464 of yacc.c  */
#line 5811 "parser.y"
    {
		  (yyval.dtype).val = NewString("default");
		  (yyval.dtype).rawval = 0;
		  (yyval.dtype).type = T_STRING;
		  (yyval.dtype).qualifier = 0;
		  (yyval.dtype).bitfield = 0;
		  (yyval.dtype).throws = 0;
		  (yyval.dtype).throwf = 0;
		  (yyval.dtype).nexcept = 0;
		}
    break;

  case 413:

/* Line 1464 of yacc.c  */
#line 5825 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 414:

/* Line 1464 of yacc.c  */
#line 5826 "parser.y"
    { (yyval.id) = (char *) 0;}
    break;

  case 415:

/* Line 1464 of yacc.c  */
#line 5829 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 416:

/* Line 1464 of yacc.c  */
#line 5830 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 417:

/* Line 1464 of yacc.c  */
#line 5834 "parser.y"
    {
		 Node *leftSibling = Getattr((yyvsp[(1) - (5)].node),"_last");
		 set_nextSibling(leftSibling,(yyvsp[(4) - (5)].node));
		 Setattr((yyvsp[(1) - (5)].node),"_last",(yyvsp[(4) - (5)].node));
		 (yyval.node) = (yyvsp[(1) - (5)].node);
	       }
    break;

  case 418:

/* Line 1464 of yacc.c  */
#line 5840 "parser.y"
    {
		 (yyval.node) = (yyvsp[(1) - (3)].node);
	       }
    break;

  case 419:

/* Line 1464 of yacc.c  */
#line 5843 "parser.y"
    {
		 Setattr((yyvsp[(2) - (3)].node),"_last",(yyvsp[(2) - (3)].node));
		 (yyval.node) = (yyvsp[(2) - (3)].node);
	       }
    break;

  case 420:

/* Line 1464 of yacc.c  */
#line 5847 "parser.y"
    {
		 (yyval.node) = 0;
	       }
    break;

  case 421:

/* Line 1464 of yacc.c  */
#line 5852 "parser.y"
    {
		   SwigType *type = NewSwigType(T_INT);
		   (yyval.node) = new_node("enumitem");
		   Setattr((yyval.node),"name",(yyvsp[(1) - (1)].id));
		   Setattr((yyval.node),"type",type);
		   SetFlag((yyval.node),"feature:immutable");
		   Delete(type);
		 }
    break;

  case 422:

/* Line 1464 of yacc.c  */
#line 5860 "parser.y"
    {
		   SwigType *type = NewSwigType((yyvsp[(3) - (3)].dtype).type == T_BOOL ? T_BOOL : ((yyvsp[(3) - (3)].dtype).type == T_CHAR ? T_CHAR : T_INT));
		   (yyval.node) = new_node("enumitem");
		   Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
		   Setattr((yyval.node),"type",type);
		   SetFlag((yyval.node),"feature:immutable");
		   Setattr((yyval.node),"enumvalue", (yyvsp[(3) - (3)].dtype).val);
		   Setattr((yyval.node),"value",(yyvsp[(1) - (3)].id));
		   Delete(type);
                 }
    break;

  case 423:

/* Line 1464 of yacc.c  */
#line 5872 "parser.y"
    {
                   (yyval.dtype) = (yyvsp[(1) - (1)].dtype);
		   if (((yyval.dtype).type != T_INT) && ((yyval.dtype).type != T_UINT) &&
		       ((yyval.dtype).type != T_LONG) && ((yyval.dtype).type != T_ULONG) &&
		       ((yyval.dtype).type != T_LONGLONG) && ((yyval.dtype).type != T_ULONGLONG) &&
		       ((yyval.dtype).type != T_SHORT) && ((yyval.dtype).type != T_USHORT) &&
		       ((yyval.dtype).type != T_SCHAR) && ((yyval.dtype).type != T_UCHAR) &&
		       ((yyval.dtype).type != T_CHAR) && ((yyval.dtype).type != T_BOOL)) {
		     Swig_error(cparse_file,cparse_line,"Type error. Expecting an integral type\n");
		   }
                }
    break;

  case 424:

/* Line 1464 of yacc.c  */
#line 5887 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 425:

/* Line 1464 of yacc.c  */
#line 5888 "parser.y"
    {
		 Node *n;
		 (yyval.dtype).val = (yyvsp[(1) - (1)].type);
		 (yyval.dtype).type = T_INT;
		 /* Check if value is in scope */
		 n = Swig_symbol_clookup((yyvsp[(1) - (1)].type),0);
		 if (n) {
                   /* A band-aid for enum values used in expressions. */
                   if (Strcmp(nodeType(n),"enumitem") == 0) {
                     String *q = Swig_symbol_qualified(n);
                     if (q) {
                       (yyval.dtype).val = NewStringf("%s::%s", q, Getattr(n,"name"));
                       Delete(q);
                     }
                   }
		 }
               }
    break;

  case 426:

/* Line 1464 of yacc.c  */
#line 5907 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 427:

/* Line 1464 of yacc.c  */
#line 5908 "parser.y"
    {
		    (yyval.dtype).val = NewString((yyvsp[(1) - (1)].id));
                    (yyval.dtype).type = T_STRING;
               }
    break;

  case 428:

/* Line 1464 of yacc.c  */
#line 5912 "parser.y"
    {
		  SwigType_push((yyvsp[(3) - (5)].type),(yyvsp[(4) - (5)].decl).type);
		  (yyval.dtype).val = NewStringf("sizeof(%s)",SwigType_str((yyvsp[(3) - (5)].type),0));
		  (yyval.dtype).type = T_ULONG;
               }
    break;

  case 429:

/* Line 1464 of yacc.c  */
#line 5917 "parser.y"
    {
		  SwigType_push((yyvsp[(6) - (8)].type),(yyvsp[(7) - (8)].decl).type);
		  (yyval.dtype).val = NewStringf("sizeof...(%s)",SwigType_str((yyvsp[(6) - (8)].type),0));
		  (yyval.dtype).type = T_ULONG;
               }
    break;

  case 430:

/* Line 1464 of yacc.c  */
#line 5922 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 431:

/* Line 1464 of yacc.c  */
#line 5923 "parser.y"
    {
		    (yyval.dtype).val = NewString((yyvsp[(1) - (1)].id));
		    (yyval.dtype).rawval = NewStringf("L\"%s\"", (yyval.dtype).val);
                    (yyval.dtype).type = T_WSTRING;
	       }
    break;

  case 432:

/* Line 1464 of yacc.c  */
#line 5928 "parser.y"
    {
		  (yyval.dtype).val = NewString((yyvsp[(1) - (1)].str));
		  if (Len((yyval.dtype).val)) {
		    (yyval.dtype).rawval = NewStringf("'%(escape)s'", (yyval.dtype).val);
		  } else {
		    (yyval.dtype).rawval = NewString("'\\0'");
		  }
		  (yyval.dtype).type = T_CHAR;
		  (yyval.dtype).bitfield = 0;
		  (yyval.dtype).throws = 0;
		  (yyval.dtype).throwf = 0;
		  (yyval.dtype).nexcept = 0;
	       }
    break;

  case 433:

/* Line 1464 of yacc.c  */
#line 5941 "parser.y"
    {
		  (yyval.dtype).val = NewString((yyvsp[(1) - (1)].str));
		  if (Len((yyval.dtype).val)) {
		    (yyval.dtype).rawval = NewStringf("L\'%s\'", (yyval.dtype).val);
		  } else {
		    (yyval.dtype).rawval = NewString("L'\\0'");
		  }
		  (yyval.dtype).type = T_WCHAR;
		  (yyval.dtype).bitfield = 0;
		  (yyval.dtype).throws = 0;
		  (yyval.dtype).throwf = 0;
		  (yyval.dtype).nexcept = 0;
	       }
    break;

  case 434:

/* Line 1464 of yacc.c  */
#line 5956 "parser.y"
    {
   	            (yyval.dtype).val = NewStringf("(%s)",(yyvsp[(2) - (3)].dtype).val);
		    (yyval.dtype).type = (yyvsp[(2) - (3)].dtype).type;
   	       }
    break;

  case 435:

/* Line 1464 of yacc.c  */
#line 5963 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(4) - (4)].dtype);
		 if ((yyvsp[(4) - (4)].dtype).type != T_STRING) {
		   switch ((yyvsp[(2) - (4)].dtype).type) {
		     case T_FLOAT:
		     case T_DOUBLE:
		     case T_LONGDOUBLE:
		     case T_FLTCPLX:
		     case T_DBLCPLX:
		       (yyval.dtype).val = NewStringf("(%s)%s", (yyvsp[(2) - (4)].dtype).val, (yyvsp[(4) - (4)].dtype).val); /* SwigType_str and decimal points don't mix! */
		       break;
		     default:
		       (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (4)].dtype).val,0), (yyvsp[(4) - (4)].dtype).val);
		       break;
		   }
		 }
 	       }
    break;

  case 436:

/* Line 1464 of yacc.c  */
#line 5980 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (5)].dtype).val,(yyvsp[(3) - (5)].type));
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 437:

/* Line 1464 of yacc.c  */
#line 5987 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_add_reference((yyvsp[(2) - (5)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 438:

/* Line 1464 of yacc.c  */
#line 5994 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_add_rvalue_reference((yyvsp[(2) - (5)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 439:

/* Line 1464 of yacc.c  */
#line 6001 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(6) - (6)].dtype);
		 if ((yyvsp[(6) - (6)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (6)].dtype).val,(yyvsp[(3) - (6)].type));
		   SwigType_add_reference((yyvsp[(2) - (6)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (6)].dtype).val,0), (yyvsp[(6) - (6)].dtype).val);
		 }
 	       }
    break;

  case 440:

/* Line 1464 of yacc.c  */
#line 6009 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(6) - (6)].dtype);
		 if ((yyvsp[(6) - (6)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (6)].dtype).val,(yyvsp[(3) - (6)].type));
		   SwigType_add_rvalue_reference((yyvsp[(2) - (6)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (6)].dtype).val,0), (yyvsp[(6) - (6)].dtype).val);
		 }
 	       }
    break;

  case 441:

/* Line 1464 of yacc.c  */
#line 6017 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("&%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 442:

/* Line 1464 of yacc.c  */
#line 6021 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("&&%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 443:

/* Line 1464 of yacc.c  */
#line 6025 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("*%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 444:

/* Line 1464 of yacc.c  */
#line 6031 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 445:

/* Line 1464 of yacc.c  */
#line 6032 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 446:

/* Line 1464 of yacc.c  */
#line 6033 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 447:

/* Line 1464 of yacc.c  */
#line 6034 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 448:

/* Line 1464 of yacc.c  */
#line 6035 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 449:

/* Line 1464 of yacc.c  */
#line 6036 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 450:

/* Line 1464 of yacc.c  */
#line 6037 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 451:

/* Line 1464 of yacc.c  */
#line 6038 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 452:

/* Line 1464 of yacc.c  */
#line 6041 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s+%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 453:

/* Line 1464 of yacc.c  */
#line 6045 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s-%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 454:

/* Line 1464 of yacc.c  */
#line 6049 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s*%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 455:

/* Line 1464 of yacc.c  */
#line 6053 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s/%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 456:

/* Line 1464 of yacc.c  */
#line 6057 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s%%%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 457:

/* Line 1464 of yacc.c  */
#line 6061 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s&%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 458:

/* Line 1464 of yacc.c  */
#line 6065 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s|%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 459:

/* Line 1464 of yacc.c  */
#line 6069 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s^%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 460:

/* Line 1464 of yacc.c  */
#line 6073 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s << %s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote_type((yyvsp[(1) - (3)].dtype).type);
	       }
    break;

  case 461:

/* Line 1464 of yacc.c  */
#line 6077 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s >> %s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote_type((yyvsp[(1) - (3)].dtype).type);
	       }
    break;

  case 462:

/* Line 1464 of yacc.c  */
#line 6081 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s&&%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 463:

/* Line 1464 of yacc.c  */
#line 6085 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s||%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 464:

/* Line 1464 of yacc.c  */
#line 6089 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s==%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 465:

/* Line 1464 of yacc.c  */
#line 6093 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s!=%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 466:

/* Line 1464 of yacc.c  */
#line 6107 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s >= %s", (yyvsp[(1) - (3)].dtype).val, (yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 467:

/* Line 1464 of yacc.c  */
#line 6111 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s <= %s", (yyvsp[(1) - (3)].dtype).val, (yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 468:

/* Line 1464 of yacc.c  */
#line 6115 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s?%s:%s", (yyvsp[(1) - (5)].dtype).val, (yyvsp[(3) - (5)].dtype).val, (yyvsp[(5) - (5)].dtype).val);
		 /* This may not be exactly right, but is probably good enough
		  * for the purposes of parsing constant expressions. */
		 (yyval.dtype).type = promote((yyvsp[(3) - (5)].dtype).type, (yyvsp[(5) - (5)].dtype).type);
	       }
    break;

  case 469:

/* Line 1464 of yacc.c  */
#line 6121 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("-%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 470:

/* Line 1464 of yacc.c  */
#line 6125 "parser.y"
    {
                 (yyval.dtype).val = NewStringf("+%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 471:

/* Line 1464 of yacc.c  */
#line 6129 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("~%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 472:

/* Line 1464 of yacc.c  */
#line 6133 "parser.y"
    {
                 (yyval.dtype).val = NewStringf("!%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = T_INT;
	       }
    break;

  case 473:

/* Line 1464 of yacc.c  */
#line 6137 "parser.y"
    {
		 String *qty;
                 skip_balanced('(',')');
		 qty = Swig_symbol_type_qualify((yyvsp[(1) - (2)].type),0);
		 if (SwigType_istemplate(qty)) {
		   String *nstr = SwigType_namestr(qty);
		   Delete(qty);
		   qty = nstr;
		 }
		 (yyval.dtype).val = NewStringf("%s%s",qty,scanner_ccode);
		 Clear(scanner_ccode);
		 (yyval.dtype).type = T_INT;
		 Delete(qty);
               }
    break;

  case 474:

/* Line 1464 of yacc.c  */
#line 6153 "parser.y"
    {
	        (yyval.str) = NewString("...");
	      }
    break;

  case 475:

/* Line 1464 of yacc.c  */
#line 6158 "parser.y"
    {
	        (yyval.str) = (yyvsp[(1) - (1)].str);
	      }
    break;

  case 476:

/* Line 1464 of yacc.c  */
#line 6161 "parser.y"
    {
	        (yyval.str) = 0;
	      }
    break;

  case 477:

/* Line 1464 of yacc.c  */
#line 6166 "parser.y"
    {
		 (yyval.bases) = (yyvsp[(1) - (1)].bases);
               }
    break;

  case 478:

/* Line 1464 of yacc.c  */
#line 6171 "parser.y"
    { inherit_list = 1; }
    break;

  case 479:

/* Line 1464 of yacc.c  */
#line 6171 "parser.y"
    { (yyval.bases) = (yyvsp[(3) - (3)].bases); inherit_list = 0; }
    break;

  case 480:

/* Line 1464 of yacc.c  */
#line 6172 "parser.y"
    { (yyval.bases) = 0; }
    break;

  case 481:

/* Line 1464 of yacc.c  */
#line 6175 "parser.y"
    {
		   Hash *list = NewHash();
		   Node *base = (yyvsp[(1) - (1)].node);
		   Node *name = Getattr(base,"name");
		   List *lpublic = NewList();
		   List *lprotected = NewList();
		   List *lprivate = NewList();
		   Setattr(list,"public",lpublic);
		   Setattr(list,"protected",lprotected);
		   Setattr(list,"private",lprivate);
		   Delete(lpublic);
		   Delete(lprotected);
		   Delete(lprivate);
		   Append(Getattr(list,Getattr(base,"access")),name);
	           (yyval.bases) = list;
               }
    break;

  case 482:

/* Line 1464 of yacc.c  */
#line 6192 "parser.y"
    {
		   Hash *list = (yyvsp[(1) - (3)].bases);
		   Node *base = (yyvsp[(3) - (3)].node);
		   Node *name = Getattr(base,"name");
		   Append(Getattr(list,Getattr(base,"access")),name);
                   (yyval.bases) = list;
               }
    break;

  case 483:

/* Line 1464 of yacc.c  */
#line 6201 "parser.y"
    {
		 (yyval.intvalue) = cparse_line;
	       }
    break;

  case 484:

/* Line 1464 of yacc.c  */
#line 6203 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setfile((yyval.node),cparse_file);
		 Setline((yyval.node),(yyvsp[(2) - (4)].intvalue));
		 Setattr((yyval.node),"name",(yyvsp[(3) - (4)].str));
		 Setfile((yyvsp[(3) - (4)].str),cparse_file);
		 Setline((yyvsp[(3) - (4)].str),(yyvsp[(2) - (4)].intvalue));
                 if (last_cpptype && (Strcmp(last_cpptype,"struct") != 0)) {
		   Setattr((yyval.node),"access","private");
		   Swig_warning(WARN_PARSE_NO_ACCESS, Getfile((yyval.node)), Getline((yyval.node)), "No access specifier given for base class '%s' (ignored).\n", SwigType_namestr((yyvsp[(3) - (4)].str)));
                 } else {
		   Setattr((yyval.node),"access","public");
		 }
		 if ((yyvsp[(4) - (4)].str))
		   SetFlag((yyval.node), "variadic");
               }
    break;

  case 485:

/* Line 1464 of yacc.c  */
#line 6219 "parser.y"
    {
		 (yyval.intvalue) = cparse_line;
	       }
    break;

  case 486:

/* Line 1464 of yacc.c  */
#line 6221 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setfile((yyval.node),cparse_file);
		 Setline((yyval.node),(yyvsp[(3) - (6)].intvalue));
		 Setattr((yyval.node),"name",(yyvsp[(5) - (6)].str));
		 Setfile((yyvsp[(5) - (6)].str),cparse_file);
		 Setline((yyvsp[(5) - (6)].str),(yyvsp[(3) - (6)].intvalue));
		 Setattr((yyval.node),"access",(yyvsp[(2) - (6)].id));
	         if (Strcmp((yyvsp[(2) - (6)].id),"public") != 0) {
		   Swig_warning(WARN_PARSE_PRIVATE_INHERIT, Getfile((yyval.node)), Getline((yyval.node)), "%s inheritance from base '%s' (ignored).\n", (yyvsp[(2) - (6)].id), SwigType_namestr((yyvsp[(5) - (6)].str)));
		 }
		 if ((yyvsp[(6) - (6)].str))
		   SetFlag((yyval.node), "variadic");
               }
    break;

  case 487:

/* Line 1464 of yacc.c  */
#line 6237 "parser.y"
    { (yyval.id) = (char*)"public"; }
    break;

  case 488:

/* Line 1464 of yacc.c  */
#line 6238 "parser.y"
    { (yyval.id) = (char*)"private"; }
    break;

  case 489:

/* Line 1464 of yacc.c  */
#line 6239 "parser.y"
    { (yyval.id) = (char*)"protected"; }
    break;

  case 490:

/* Line 1464 of yacc.c  */
#line 6243 "parser.y"
    { 
                   (yyval.id) = (char*)"class"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 491:

/* Line 1464 of yacc.c  */
#line 6247 "parser.y"
    { 
                   (yyval.id) = (char *)"typename"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 492:

/* Line 1464 of yacc.c  */
#line 6251 "parser.y"
    { 
                   (yyval.id) = (char *)"class..."; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 493:

/* Line 1464 of yacc.c  */
#line 6255 "parser.y"
    { 
                   (yyval.id) = (char *)"typename..."; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 494:

/* Line 1464 of yacc.c  */
#line 6261 "parser.y"
    {
                 (yyval.id) = (yyvsp[(1) - (1)].id);
               }
    break;

  case 495:

/* Line 1464 of yacc.c  */
#line 6264 "parser.y"
    { 
                   (yyval.id) = (char*)"struct"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 496:

/* Line 1464 of yacc.c  */
#line 6268 "parser.y"
    {
                   (yyval.id) = (char*)"union"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 499:

/* Line 1464 of yacc.c  */
#line 6278 "parser.y"
    {
                   (yyval.str) = 0;
	       }
    break;

  case 500:

/* Line 1464 of yacc.c  */
#line 6281 "parser.y"
    {
                   (yyval.str) = 0;
	       }
    break;

  case 501:

/* Line 1464 of yacc.c  */
#line 6284 "parser.y"
    {
                   (yyval.str) = 0;
	       }
    break;

  case 502:

/* Line 1464 of yacc.c  */
#line 6287 "parser.y"
    {
                   (yyval.str) = 0;
	       }
    break;

  case 503:

/* Line 1464 of yacc.c  */
#line 6292 "parser.y"
    {
                    (yyval.dtype).throws = (yyvsp[(3) - (4)].pl);
                    (yyval.dtype).throwf = NewString("1");
                    (yyval.dtype).nexcept = 0;
	       }
    break;

  case 504:

/* Line 1464 of yacc.c  */
#line 6297 "parser.y"
    {
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
                    (yyval.dtype).nexcept = NewString("true");
	       }
    break;

  case 505:

/* Line 1464 of yacc.c  */
#line 6302 "parser.y"
    {
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
                    (yyval.dtype).nexcept = 0;
	       }
    break;

  case 506:

/* Line 1464 of yacc.c  */
#line 6307 "parser.y"
    {
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
                    (yyval.dtype).nexcept = NewString("true");
	       }
    break;

  case 507:

/* Line 1464 of yacc.c  */
#line 6312 "parser.y"
    {
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
                    (yyval.dtype).nexcept = (yyvsp[(3) - (4)].dtype).val;
	       }
    break;

  case 508:

/* Line 1464 of yacc.c  */
#line 6319 "parser.y"
    {
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
                    (yyval.dtype).nexcept = 0;
                    (yyval.dtype).qualifier = (yyvsp[(1) - (1)].str);
               }
    break;

  case 509:

/* Line 1464 of yacc.c  */
#line 6325 "parser.y"
    {
		    (yyval.dtype) = (yyvsp[(1) - (1)].dtype);
                    (yyval.dtype).qualifier = 0;
               }
    break;

  case 510:

/* Line 1464 of yacc.c  */
#line 6329 "parser.y"
    {
		    (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                    (yyval.dtype).qualifier = (yyvsp[(1) - (2)].str);
               }
    break;

  case 511:

/* Line 1464 of yacc.c  */
#line 6333 "parser.y"
    { 
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
                    (yyval.dtype).nexcept = 0;
                    (yyval.dtype).qualifier = 0; 
               }
    break;

  case 512:

/* Line 1464 of yacc.c  */
#line 6341 "parser.y"
    { 
                    Clear(scanner_ccode); 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = 0; 
		    (yyval.decl).throws = (yyvsp[(1) - (3)].dtype).throws;
		    (yyval.decl).throwf = (yyvsp[(1) - (3)].dtype).throwf;
		    (yyval.decl).nexcept = (yyvsp[(1) - (3)].dtype).nexcept;
               }
    break;

  case 513:

/* Line 1464 of yacc.c  */
#line 6349 "parser.y"
    { 
                    skip_balanced('{','}'); 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = 0; 
                    (yyval.decl).throws = (yyvsp[(1) - (3)].dtype).throws;
                    (yyval.decl).throwf = (yyvsp[(1) - (3)].dtype).throwf;
                    (yyval.decl).nexcept = (yyvsp[(1) - (3)].dtype).nexcept;
               }
    break;

  case 514:

/* Line 1464 of yacc.c  */
#line 6357 "parser.y"
    { 
                    Clear(scanner_ccode); 
                    (yyval.decl).parms = (yyvsp[(2) - (4)].pl); 
                    (yyval.decl).have_parms = 1; 
                    (yyval.decl).defarg = 0; 
		    (yyval.decl).throws = 0;
		    (yyval.decl).throwf = 0;
		    (yyval.decl).nexcept = 0;
               }
    break;

  case 515:

/* Line 1464 of yacc.c  */
#line 6366 "parser.y"
    {
                    skip_balanced('{','}'); 
                    (yyval.decl).parms = (yyvsp[(2) - (4)].pl); 
                    (yyval.decl).have_parms = 1; 
                    (yyval.decl).defarg = 0; 
                    (yyval.decl).throws = 0;
                    (yyval.decl).throwf = 0;
                    (yyval.decl).nexcept = 0;
               }
    break;

  case 516:

/* Line 1464 of yacc.c  */
#line 6375 "parser.y"
    { 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = (yyvsp[(2) - (3)].dtype).val; 
                    (yyval.decl).throws = 0;
                    (yyval.decl).throwf = 0;
                    (yyval.decl).nexcept = 0;
               }
    break;

  case 517:

/* Line 1464 of yacc.c  */
#line 6382 "parser.y"
    {
                    (yyval.decl).have_parms = 0;
                    (yyval.decl).defarg = (yyvsp[(3) - (4)].dtype).val;
                    (yyval.decl).throws = (yyvsp[(1) - (4)].dtype).throws;
                    (yyval.decl).throwf = (yyvsp[(1) - (4)].dtype).throwf;
                    (yyval.decl).nexcept = (yyvsp[(1) - (4)].dtype).nexcept;
               }
    break;

  case 524:

/* Line 1464 of yacc.c  */
#line 6401 "parser.y"
    {
		  skip_balanced('(',')');
		  Clear(scanner_ccode);
		}
    break;

  case 525:

/* Line 1464 of yacc.c  */
#line 6413 "parser.y"
    {
		  skip_balanced('{','}');
		  Clear(scanner_ccode);
		}
    break;

  case 526:

/* Line 1464 of yacc.c  */
#line 6419 "parser.y"
    { 
                     String *s = NewStringEmpty();
                     SwigType_add_template(s,(yyvsp[(2) - (3)].p));
                     (yyval.id) = Char(s);
		     scanner_last_id(1);
                 }
    break;

  case 527:

/* Line 1464 of yacc.c  */
#line 6425 "parser.y"
    { (yyval.id) = (char*)"";  }
    break;

  case 528:

/* Line 1464 of yacc.c  */
#line 6429 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 529:

/* Line 1464 of yacc.c  */
#line 6430 "parser.y"
    { (yyval.id) = Swig_copy_string("override"); }
    break;

  case 530:

/* Line 1464 of yacc.c  */
#line 6431 "parser.y"
    { (yyval.id) = Swig_copy_string("final"); }
    break;

  case 531:

/* Line 1464 of yacc.c  */
#line 6434 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 532:

/* Line 1464 of yacc.c  */
#line 6435 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].dtype).val; }
    break;

  case 533:

/* Line 1464 of yacc.c  */
#line 6436 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 534:

/* Line 1464 of yacc.c  */
#line 6439 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 535:

/* Line 1464 of yacc.c  */
#line 6440 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 536:

/* Line 1464 of yacc.c  */
#line 6443 "parser.y"
    { 
                  (yyval.str) = 0;
		  if (!(yyval.str)) (yyval.str) = NewStringf("%s%s", (yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str));
      	          Delete((yyvsp[(2) - (2)].str));
               }
    break;

  case 537:

/* Line 1464 of yacc.c  */
#line 6448 "parser.y"
    { 
		 (yyval.str) = NewStringf("::%s%s",(yyvsp[(3) - (4)].str),(yyvsp[(4) - (4)].str));
                 Delete((yyvsp[(4) - (4)].str));
               }
    break;

  case 538:

/* Line 1464 of yacc.c  */
#line 6452 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
   	       }
    break;

  case 539:

/* Line 1464 of yacc.c  */
#line 6455 "parser.y"
    {
		 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 540:

/* Line 1464 of yacc.c  */
#line 6458 "parser.y"
    {
                 (yyval.str) = NewStringf("%s%s",(yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].id));
	       }
    break;

  case 541:

/* Line 1464 of yacc.c  */
#line 6461 "parser.y"
    {
                 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 542:

/* Line 1464 of yacc.c  */
#line 6466 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s%s",(yyvsp[(2) - (3)].str),(yyvsp[(3) - (3)].str));
		   Delete((yyvsp[(3) - (3)].str));
               }
    break;

  case 543:

/* Line 1464 of yacc.c  */
#line 6470 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 544:

/* Line 1464 of yacc.c  */
#line 6473 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 545:

/* Line 1464 of yacc.c  */
#line 6480 "parser.y"
    {
		 (yyval.str) = NewStringf("::~%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 546:

/* Line 1464 of yacc.c  */
#line 6486 "parser.y"
    {
                  (yyval.str) = NewStringf("%s%s",(yyvsp[(1) - (2)].id),(yyvsp[(2) - (2)].id));
		  /*		  if (Len($2)) {
		    scanner_last_id(1);
		    } */
              }
    break;

  case 547:

/* Line 1464 of yacc.c  */
#line 6495 "parser.y"
    {
                  (yyval.str) = 0;
		  if (!(yyval.str)) (yyval.str) = NewStringf("%s%s", (yyvsp[(1) - (2)].id),(yyvsp[(2) - (2)].str));
      	          Delete((yyvsp[(2) - (2)].str));
               }
    break;

  case 548:

/* Line 1464 of yacc.c  */
#line 6500 "parser.y"
    {
		 (yyval.str) = NewStringf("::%s%s",(yyvsp[(3) - (4)].id),(yyvsp[(4) - (4)].str));
                 Delete((yyvsp[(4) - (4)].str));
               }
    break;

  case 549:

/* Line 1464 of yacc.c  */
#line 6504 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].id));
   	       }
    break;

  case 550:

/* Line 1464 of yacc.c  */
#line 6507 "parser.y"
    {
		 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].id));
               }
    break;

  case 551:

/* Line 1464 of yacc.c  */
#line 6510 "parser.y"
    {
                 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
	       }
    break;

  case 552:

/* Line 1464 of yacc.c  */
#line 6513 "parser.y"
    {
                 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 553:

/* Line 1464 of yacc.c  */
#line 6518 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s%s",(yyvsp[(2) - (3)].id),(yyvsp[(3) - (3)].str));
		   Delete((yyvsp[(3) - (3)].str));
               }
    break;

  case 554:

/* Line 1464 of yacc.c  */
#line 6522 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].id));
               }
    break;

  case 555:

/* Line 1464 of yacc.c  */
#line 6525 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 556:

/* Line 1464 of yacc.c  */
#line 6528 "parser.y"
    {
		 (yyval.str) = NewStringf("::~%s",(yyvsp[(2) - (2)].id));
               }
    break;

  case 557:

/* Line 1464 of yacc.c  */
#line 6534 "parser.y"
    { 
                   (yyval.id) = (char *) malloc(strlen((yyvsp[(1) - (2)].id))+strlen((yyvsp[(2) - (2)].id))+1);
                   strcpy((yyval.id),(yyvsp[(1) - (2)].id));
                   strcat((yyval.id),(yyvsp[(2) - (2)].id));
               }
    break;

  case 558:

/* Line 1464 of yacc.c  */
#line 6539 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id);}
    break;

  case 559:

/* Line 1464 of yacc.c  */
#line 6542 "parser.y"
    {
                   (yyval.id) = (char *) malloc(strlen((yyvsp[(1) - (2)].id))+strlen((yyvsp[(2) - (2)].id))+1);
                   strcpy((yyval.id),(yyvsp[(1) - (2)].id));
                   strcat((yyval.id),(yyvsp[(2) - (2)].id));
               }
    break;

  case 560:

/* Line 1464 of yacc.c  */
#line 6554 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id);}
    break;

  case 561:

/* Line 1464 of yacc.c  */
#line 6557 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].id));
               }
    break;

  case 562:

/* Line 1464 of yacc.c  */
#line 6560 "parser.y"
    {
                  skip_balanced('{','}');
		  (yyval.str) = NewString(scanner_ccode);
               }
    break;

  case 563:

/* Line 1464 of yacc.c  */
#line 6564 "parser.y"
    {
		 (yyval.str) = (yyvsp[(1) - (1)].str);
              }
    break;

  case 564:

/* Line 1464 of yacc.c  */
#line 6569 "parser.y"
    {
                  Hash *n;
                  (yyval.node) = NewHash();
                  n = (yyvsp[(2) - (3)].node);
                  while(n) {
                     String *name, *value;
                     name = Getattr(n,"name");
                     value = Getattr(n,"value");
		     if (!value) value = (String *) "1";
                     Setattr((yyval.node),name, value);
		     n = nextSibling(n);
		  }
               }
    break;

  case 565:

/* Line 1464 of yacc.c  */
#line 6582 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 566:

/* Line 1464 of yacc.c  */
#line 6586 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
		 Setattr((yyval.node),"value",(yyvsp[(3) - (3)].id));
               }
    break;

  case 567:

/* Line 1464 of yacc.c  */
#line 6591 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setattr((yyval.node),"name",(yyvsp[(1) - (5)].id));
		 Setattr((yyval.node),"value",(yyvsp[(3) - (5)].id));
		 set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
               }
    break;

  case 568:

/* Line 1464 of yacc.c  */
#line 6597 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"name",(yyvsp[(1) - (1)].id));
	       }
    break;

  case 569:

/* Line 1464 of yacc.c  */
#line 6601 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
                 set_nextSibling((yyval.node),(yyvsp[(3) - (3)].node));
               }
    break;

  case 570:

/* Line 1464 of yacc.c  */
#line 6606 "parser.y"
    {
                 (yyval.node) = (yyvsp[(3) - (3)].node);
		 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
               }
    break;

  case 571:

/* Line 1464 of yacc.c  */
#line 6610 "parser.y"
    {
                 (yyval.node) = (yyvsp[(3) - (5)].node);
		 Setattr((yyval.node),"name",(yyvsp[(1) - (5)].id));
		 set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
               }
    break;

  case 572:

/* Line 1464 of yacc.c  */
#line 6617 "parser.y"
    {
		 (yyval.id) = (yyvsp[(1) - (1)].id);
               }
    break;

  case 573:

/* Line 1464 of yacc.c  */
#line 6620 "parser.y"
    {
                 (yyval.id) = Char((yyvsp[(1) - (1)].dtype).val);
               }
    break;



/* Line 1464 of yacc.c  */
#line 12707 "CParse/parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1684 of yacc.c  */
#line 6627 "parser.y"


SwigType *Swig_cparse_type(String *s) {
   String *ns;
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSETYPE);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}


Parm *Swig_cparse_parm(String *s) {
   String *ns;
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSEPARM);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   Delete(ns);
   return top;
}


ParmList *Swig_cparse_parms(String *s, Node *file_line_node) {
   String *ns;
   char *cs = Char(s);
   if (cs && cs[0] != '(') {
     ns = NewStringf("(%s);",s);
   } else {
     ns = NewStringf("%s;",s);
   }
   Setfile(ns, Getfile(file_line_node));
   Setline(ns, Getline(file_line_node));
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSEPARMS);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}


