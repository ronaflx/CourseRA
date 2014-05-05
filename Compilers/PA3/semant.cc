#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <set>
#include "semant.h"
#include "utilities.h"

extern int semant_debug;
extern char* curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol arg, arg2, Bool, concat, cool_abort, copy, Int, in_int, in_string,
    IO, length, Main, main_meth, No_class, No_type, Object, out_int, out_string,
    prim_slot, self, SELF_TYPE, Str, str_field, substr, type_name, val;

const static int K = 5;

const static Sybmol* basic_class_table[K] = {&Object, &IO, &Int, &Str, &Bool};

// Return true if sym is a basic class of Cool language.
bool is_basic_class(Symbol sym) {
  for (int i = 0; i < K; i++) {
    if (sym == *basic_class_table[i]) {
      return true;
    }
  }
  return false;
}

// Return the symbol of type of sym
Symbol get_real_type(Symbol sym, Class_ c) {
  return sym == SELF_TYPE ? c->get_name() : sym;
}

Symbol get_real_type(Symbol sym, Expression e) {
  return sym == SELF_TYPE ? e->get_type() : sym;
}

// Macro for error report
#define ECHO_ERR1(c, s0) classtable->semant_error(c) << s0 << ".\n"
#define ECHO_ERR2(c, s0, s1) classtable->semant_error(c) << s0 << s1 << ".\n"
#define ECHO_ERR3(c, s0, s1, s2) \
  classtable->semant_error(c) << s0 << s1 << s2 << ".\n"
#define ECHO_ERR4(c, s0, s1, s2, s3) \
  classtable->semant_error(c) << s0 << s1 << s2 << s3 << ".\n"
#define ECHO_ERR5(c, s0, s1, s2, s3, s4) \
  classtable->semant_error(c) << s0 << s1 << s2 << s3 << s4 << ".\n"
#define ECHO_ERR6(c, s0, s1, s2, s3, s4, s5) \
  classtable->semant_error(c) << s0 << s1 << s2 << s3 << s4 << s5 << ".\n"

//
// Initializing the predefined symbols.
//
static void initialize_constants(void) {
  arg = idtable.add_string("arg");
  arg2 = idtable.add_string("arg2");
  Bool = idtable.add_string("Bool");
  concat = idtable.add_string("concat");
  cool_abort = idtable.add_string("abort");
  copy = idtable.add_string("copy");
  Int = idtable.add_string("Int");
  in_int = idtable.add_string("in_int");
  in_string = idtable.add_string("in_string");
  IO = idtable.add_string("IO");
  length = idtable.add_string("length");
  Main = idtable.add_string("Main");
  main_meth = idtable.add_string("main");
  //   _no_class is a symbol that can't be the name of any
  //   user-defined class.
  No_class = idtable.add_string("_no_class");
  No_type = idtable.add_string("_no_type");
  Object = idtable.add_string("Object");
  out_int = idtable.add_string("out_int");
  out_string = idtable.add_string("out_string");
  prim_slot = idtable.add_string("_prim_slot");
  self = idtable.add_string("self");
  SELF_TYPE = idtable.add_string("SELF_TYPE");
  Str = idtable.add_string("String");
  str_field = idtable.add_string("_str_field");
  substr = idtable.add_string("substr");
  type_name = idtable.add_string("type_name");
  val = idtable.add_string("_val");
}

ClassTable::ClassTable(Classes classes)
    : semant_errors(0), error_stream(cerr), classes(classes) {
  sym_table = new SymbolTable<Symbol, Entry>();
}

void ClassTable::install_basic_classes() {

  // The tree package uses these globals to annotate the classes built below.
  // curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

  // The following demonstrates how to create dummy parse trees to
  // refer to basic Cool classes.  There's no need for method
  // bodies -- these are already built into the runtime system.

  // IMPORTANT: The results of the following expressions are
  // stored in local variables.  You will want to do something
  // with those variables at the end of this method to make this
  // code meaningful.

  //
  // The Object class has no parent class. Its methods are
  //        abort() : Object    aborts the program
  //        type_name() : Str   returns a string representation of class name
  //        copy() : SELF_TYPE  returns a copy of the object
  //
  // There is no need for method bodies in the basic classes---these
  // are already built in to the runtime system.

  Class_ Object_class = class_(
      Object, No_class,
      append_Features(
          append_Features(single_Features(method(cool_abort, nil_Formals(),
                                                 Object, no_expr())),
                          single_Features(method(type_name, nil_Formals(), Str,
                                                 no_expr()))),
          single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
      filename);

  //
  // The IO class inherits from Object. Its methods are
  //        out_string(Str) : SELF_TYPE       writes a string to the output
  //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
  //        in_string() : Str                 reads a string from the input
  //        in_int() : Int                      "   an int     "  "     "
  //
  Class_ IO_class = class_(
      IO, Object,
      append_Features(
          append_Features(
              append_Features(single_Features(method(
                                  out_string, single_Formals(formal(arg, Str)),
                                  SELF_TYPE, no_expr())),
                              single_Features(method(
                                  out_int, single_Formals(formal(arg, Int)),
                                  SELF_TYPE, no_expr()))),
              single_Features(
                  method(in_string, nil_Formals(), Str, no_expr()))),
          single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
      filename);

  //
  // The Int class has no methods and only a single attribute, the
  // "val" for the integer.
  //
  Class_ Int_class = class_(
      Int, Object, single_Features(attr(val, prim_slot, no_expr())), filename);

  //
  // Bool also has only the "val" slot.
  //
  Class_ Bool_class = class_(
      Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename);

  //
  // The class Str has a number of slots and operations:
  //       val                                  the length of the string
  //       str_field                            the string itself
  //       length() : Int                       returns length of the string
  //       concat(arg: Str) : Str               performs string concatenation
  //       substr(arg: Int, arg2: Int): Str     substring selection
  //
  Class_ Str_class = class_(
      Str, Object,
      append_Features(
          append_Features(
              append_Features(
                  append_Features(
                      single_Features(attr(val, Int, no_expr())),
                      single_Features(attr(str_field, prim_slot, no_expr()))),
                  single_Features(
                      method(length, nil_Formals(), Int, no_expr()))),
              single_Features(method(concat, single_Formals(formal(arg, Str)),
                                     Str, no_expr()))),
          single_Features(
              method(substr, append_Formals(single_Formals(formal(arg, Int)),
                                            single_Formals(formal(arg2, Int))),
                     Str, no_expr()))),
      filename);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c) {
  return semant_error(c->get_filename(), c);
}

ostream& ClassTable::semant_error(Symbol filename, tree_node* t) {
  error_stream << filename << ":" << t->get_line_number() << ": ";
  return semant_error();
}

ostream& ClassTable::semant_error() {
  semant_errors++;
  return error_stream;
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */

void class__class::check_type(ClassTable* classtable, Class_ c) {
  classtable->sym_table->enterscope();
  ClassInfo::Symbols parents;
  classtable->get_all_parents(name, &parents);
  // Put parent's attr into scope.
  for (size_t i = 1; i < parents.size(); i++) {
    const ClassInfo& class_info = classtable->get_class_info(parents[i]);
    for (ClassInfo::AttrTable::const_iterator i = class_info.attr_table.begin();
         i != class_info.attr_table.end(); ++i) {
      classtable->sym_table->addid(i->first, i->second);
    }
  }
  for (int i = features->first(); features->more(i); i = features->next(i)) {
    features->nth(i)->check_type(classtable, c);
  }
  classtable->sym_table->exitscope();
}

void class__class::collect_feature(ClassInfo* class_info) const {
  for (int i = features->first(); features->more(i); i = features->next(i)) {
    features->nth(i)->collect_feature(class_info);
  }
}

void method_class::check_type(ClassTable* classtable, Class_ c) {
  classtable->sym_table->enterscope();
  if (!classtable->find(return_type)) {
    ECHO_ERR4(c, "Undefined return type ", return_type, "in method ", name);
  }
  pair<Symbol, Symbol> diff;
  FUNC_OVERRIDE_TYPE func_error_type =
      classtable->is_override_func(c->get_name(), name, &diff);
  if (func_error_type == OVERRIDE_RETURN_TYPE ||
      func_error_type == OVERRIDE_PARAMS_TYPE) {
    ECHO_ERR6(c, "In redefined method ", name, ", parameter type ", diff.second,
              " is different from original type ", diff.first);
  } else if (func_error_type == OVERRIDE_NUMS_PARAMS_TYPE) {
    ECHO_ERR3(c, "Incompatible number of formal ",
              "parameters in redefined method ", name);
  }
  std::set<Symbol> formal_names;
  for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
    formals->nth(i)->check_type(classtable, c);
    Symbol name = formals->nth(i)->get_name();
    if (formal_names.find(name) == formal_names.end()) {
      formal_names.insert(name);
    } else {
      ECHO_ERR3(c, "Formal parameter ", name, " is multiply defined");
    }
  }
  expr->check_type(classtable, c);

  Symbol expr_type = get_real_type(expr->get_type(), c);
  Symbol real_return_type = get_real_type(return_type, c);
  if (expr_type != No_type &&
      ((return_type == SELF_TYPE && expr->get_type() != SELF_TYPE) ||
       !classtable->is_sub_type_of(expr_type, real_return_type))) {
    ECHO_ERR6(c, "In redefined method ", name, ", parameter type ",
              expr->get_type(), " is different from original type ",
              return_type);
  }
  classtable->sym_table->exitscope();
}

void method_class::collect_feature(ClassInfo* class_info) const {
  ClassInfo::Symbols formals_sym;
  for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
    formals_sym.push_back(formals->nth(i)->get_type());
  }
  class_info->func_table.insert(
      make_pair(name, make_pair(return_type, formals_sym)));
}

void attr_class::check_type(ClassTable* classtable, Class_ c) {
  init->check_type(classtable, c);
  if (name == self) {
    ECHO_ERR1(c, "'self' cannot be the name of an attribute");
  } else if (classtable->is_override_attr(c->get_name(), name)) {
    ECHO_ERR3(c, "Attribute ", name, " is an attribute of an inherited class");
  } else {
    assert(classtable->sym_table->addid(name, type_decl));
  }
}

void attr_class::collect_feature(ClassInfo* class_info) const {
  class_info->attr_table.insert(make_pair(name, type_decl));
}

void formal_class::check_type(ClassTable* classtable, Class_ c) {
  if (name == self) {
    ECHO_ERR1(c, "self cannot be the name of a formal parameter");
  } else if (type_decl == SELF_TYPE) {
    ECHO_ERR3(c, "Formal parameter ", name, " cannot have type SELF_TYPE");
  } else {
    classtable->sym_table->addid(name, type_decl);
  }
}

void assign_class::check_type(ClassTable* classtable, Class_ c) {
  expr->check_type(classtable, c);
  Symbol name_type = classtable->sym_table->lookup(name);
  Symbol expr_type = expr->get_type();
  if (name == self) {
    ECHO_ERR1(c, "Cannot assign to 'self'");
  } else if (name_type == NULL) {
    ECHO_ERR3(c, "Indentifier ", name_type, "don't exist");
  } else if (!classtable->is_sub_type_of(expr_type, name_type)) {
    ECHO_ERR6(c, "Type ", expr_type,
              " of assigned expression does not conform to declared type ",
              name_type, " of identifier ", name);
  }
  set_type(expr->get_type());
}

void static_dispatch_class::check_type(ClassTable* classtable, Class_ c) {
  expr->check_type(classtable, c);
  // Check type of each actual params.
  ClassInfo::Symbols actual_types;
  for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->check_type(classtable, c);
    actual_types.push_back(actual->nth(i)->get_type());
  }
  size_t n = actual_types.size();

  // Check FuncInfo
  const ClassInfo::FuncInfo& func_info =
      classtable->get_func_info(type_name, name);
  if (func_info == ClassInfo::FuncInfo()) {
    ECHO_ERR2(c, "Dispatch to undefined method ", name);
  } else if (n != func_info.second.size()) {
    ECHO_ERR1(c, "params don't match");
  } else {
    for (size_t i = 0; i < n; i++) {
      Symbol formal_type = func_info.second[i];
      Symbol actual_type = get_real_type(actual_types[i], c);
      // TODO(ronaflx): A litter hack on evaluate-result here.
      // I don't want to rewrite again.
      if (!classtable->is_sub_type_of(actual_type, formal_type)) {
        ECHO_ERR6(c, "In call of method ", name, ", type ", actual_types[i],
                  " of parameter b does not conform to declared type ",
                  formal_type);
      }
    }
  }
  Symbol turn_type = get_real_type(type_name, c);
  Symbol expr_type = get_real_type(expr->get_type(), c);

  // Static dispatch tpye error.
  if (!classtable->is_sub_type_of(expr_type, turn_type)) {
    ECHO_ERR4(c, "Expression type ", expr->get_type(),
              " does not conform to declared static dispatch type ", type_name);
  }

  set_type(get_real_type(func_info.first, c));
}

void dispatch_class::check_type(ClassTable* classtable, Class_ c) {
  expr->check_type(classtable, c);

  vector<Sybmol> actual_types;
  for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->check_type(classtable, c);
    actual_types.push_back(actual->nth(i)->get_type());
  }
  size_t n = actual_types.size();

  Sybmol expr_type = get_real_type(expr->get_type(), c);
  const ClassInfo::FuncInfo& func_info =
      classtable->get_func_info(expr_type, name);
  if (func_info == ClassInfo::FuncInfo()) {
    ECHO_ERR2(c, "Dispatch to undefined method ", name);
    set_type(Object);
  } else if (n != func_info.second.size()) {
    ECHO_ERR1(c, "params don't match");
    set_type(Object);
  } else {
    for (size_t i = 0; i < n; i++) {
      Symbol formal_type = func_info.second[i];
      Symbol actual_type = get_real_type(actual_types[i], c);
      // TODO(ronaflx): A litter hack on evaluate-result here.
      // I don't want to rewrite again.
      if (!classtable->is_sub_type_of(actual_type, formal_type)) {
        ECHO_ERR6(c, "In call of method ", name, ", type ", actual_types[i],
                  " of parameter b does not conform to declared type ",
                  formal_type);
      }
    }
    set_type(get_real_type(func_info.first, expr));
  }
}

void cond_class::check_type(ClassTable* classtable, Class_ c) {
  pred->check_type(classtable, c);

  classtable->sym_table->enterscope();
  then_exp->check_type(classtable, c);
  classtable->sym_table->exitscope();

  classtable->sym_table->enterscope();
  else_exp->check_type(classtable, c);
  classtable->sym_table->exitscope();

  Symbol then_class = then_exp->get_type();
  Symbol else_class = else_exp->get_type();
  Symbol lca_class = classtable->get_lca(then_class, else_class);
  set_type(lca_class);
}

void loop_class::check_type(ClassTable* classtable, Class_ c) {
  pred->check_type(classtable, c);
  body->check_type(classtable, c);
  if (pred->get_type() != Bool) {
    ECHO_ERR1(c, "Loop condition does not have type Bool");
  }
  set_type(Object);
}

void typcase_class::check_type(ClassTable* classtable, Class_ c) {
  expr->check_type(classtable, c);
  Symbol expr_type = expr->get_type();
  std::set<Symbol> types;
  Symbol common_type = NULL;
  for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
    cases->nth(i)->check_type(classtable, c);
    Symbol type_decl = cases->nth(i)->get_type_decl();
    Symbol curr_type = cases->nth(i)->get_type();
    if (types.find(type_decl) == types.end()) {
      types.insert(type_decl);
    } else {
      ECHO_ERR3(c, "Duplicate branch ", curr_type, " in case statement");
    }
    if (common_type == NULL) {
      common_type = curr_type;
    } else {
      common_type = classtable->get_lca(common_type, curr_type);
    }
  }
  set_type(common_type);
}

void branch_class::check_type(ClassTable* classtable, Class_ c) {
  classtable->sym_table->enterscope();
  classtable->sym_table->addid(name, type_decl);
  expr->check_type(classtable, c);
  classtable->sym_table->exitscope();
}

void block_class::check_type(ClassTable* classtable, Class_ c) {
  Symbol type = NULL;
  for (int i = body->first(); body->more(i); i = body->next(i)) {
    body->nth(i)->check_type(classtable, c);
    type = body->nth(i)->get_type();
  }
  set_type(type);
}

void let_class::check_type(ClassTable* classtable, Class_ c) {
  if (identifier == self) {
    ECHO_ERR1(c, "'self' cannot be bound in a 'let' expression");
    set_type(Object);
    return;
  }
  classtable->sym_table->enterscope();
  classtable->sym_table->addid(identifier, type_decl);
  init->check_type(classtable, c);
  if (init->get_type() != No_type) {
    Symbol lca = classtable->get_lca(type_decl, init->get_type());
    if (type_decl != SELF_TYPE && type_decl != lca) {
      ECHO_ERR6(c, "Inferred type ", type_decl, " of initialization of ",
                identifier, " does not conform to identifier's declared type ",
                init->get_type());
    }
  }
  body->check_type(classtable, c);
  classtable->sym_table->exitscope();
  set_type(body->get_type());
}

#define ARTH_CHECK_TYPE(op)                                                  \
  e1->check_type(classtable, c);                                             \
  e2->check_type(classtable, c);                                             \
  if (!(e1->get_type() == Int && e2->get_type() == Int)) {                   \
    ECHO_ERR4(c, "non-Int arguments: ", e1->get_type(), op, e2->get_type()); \
  }                                                                          \
  set_type(e1->get_type());

void plus_class::check_type(ClassTable* classtable, Class_ c) {
  ARTH_CHECK_TYPE(" + ");
}

void sub_class::check_type(ClassTable* classtable, Class_ c) {
  ARTH_CHECK_TYPE(" - ")
}

void mul_class::check_type(ClassTable* classtable, Class_ c) {
  ARTH_CHECK_TYPE(" * ");
}

void divide_class::check_type(ClassTable* classtable, Class_ c) {
  ARTH_CHECK_TYPE(" / ");
}

void neg_class::check_type(ClassTable* classtable, Class_ c) {
  e1->check_type(classtable, c);
  Symbol e1_type = e1->get_type();
  if (e1_type != Bool && e1_type != Int) {
    ECHO_ERR1(c, "neg_expr is not  Boolean or Int");
  }
  set_type(e1->get_type());
}

#define COMPARE_CHECK_TYPE                                    \
  e1->check_type(classtable, c);                              \
  e2->check_type(classtable, c);                              \
  Symbol e1_type = e1->get_type();                            \
  Symbol e2_type = e2->get_type();                            \
  if (is_basic_class(e1_type) && is_basic_class(e1_type)) {   \
    if (e1_type != e2_type) {                                 \
      ECHO_ERR1(c, "Illegal comparison with a basic type");   \
    }                                                         \
  } else {                                                    \
    Symbol lca_class = classtable->get_lca(e1_type, e2_type); \
  }                                                           \
  set_type(Bool);

void lt_class::check_type(ClassTable* classtable, Class_ c) {
  COMPARE_CHECK_TYPE;
}

void eq_class::check_type(ClassTable* classtable, Class_ c) {
  COMPARE_CHECK_TYPE;
}

void leq_class::check_type(ClassTable* classtable, Class_ c) {
  COMPARE_CHECK_TYPE;
}

void comp_class::check_type(ClassTable* classtable, Class_ c) {
  e1->check_type(classtable, c);
  set_type(e1->get_type());
}

void int_const_class::check_type(ClassTable* classtable, Class_ c) {
  set_type(Int);
}

void bool_const_class::check_type(ClassTable* classtable, Class_ c) {
  set_type(Bool);
}

void string_const_class::check_type(ClassTable* classtable, Class_ c) {
  set_type(Str);
}

void new__class::check_type(ClassTable* classtable, Class_ c) {
  if (type_name == SELF_TYPE) {
    set_type(SELF_TYPE);
  } else if (classtable->find(type_name)) {
    set_type(type_name);
  } else {
    ECHO_ERR2(c, "'new' used with undefined class ", type_name);
    set_type(Object);
  }
}

void isvoid_class::check_type(ClassTable* classtable, Class_ c) {
  e1->check_type(classtable, c);
  set_type(e1->get_type());
}

void no_expr_class::check_type(ClassTable* classtable, Class_ c) {
  set_type(No_type);
}

void object_class::check_type(ClassTable* classtable, Class_ c) {
  Symbol type_sym = classtable->sym_table->lookup(name);
  if (name == self) {
    set_type(SELF_TYPE);
  } else if (!type_sym) {
    ECHO_ERR2(c, "Undeclared identifier ", name);
    set_type(No_type);
  } else {
    set_type(type_sym);
  }
}

static int in_basic_class(Symbol name, Symbol basic_class[], int n) {
  if (name == NULL) return -1;
  for (int i = 0; i < n; i++) {
    if (name == basic_class[i]) {
      return i;
    }
  }
  return -1;
}

static ClassInfo::Symbols build_symbols() { return ClassInfo::Symbols(); }
static ClassInfo::Symbols build_symbols(Symbol sym) {
  ClassInfo::Symbols vec;
  vec.push_back(sym);
  return vec;
}
static ClassInfo::Symbols build_symbols(Symbol sym0, Symbol sym1) {
  ClassInfo::Symbols vec;
  vec.push_back(sym0);
  vec.push_back(sym1);
  return vec;
}

void install_basic_classinfo(ClassTable* classtable) {
  // Object
  {
    ClassInfo class_info;
    class_info.name = Object;
    class_info.parent = Object;
    // Object copy
    class_info.func_table.insert(
        make_pair(copy, make_pair(SELF_TYPE, build_symbols())));

    // Object abort
    class_info.func_table.insert(
        make_pair(cool_abort, make_pair(Object, build_symbols())));
    classtable->insert(Object, class_info);
  }

  // IO
  {
    ClassInfo class_info;
    class_info.name = IO;
    class_info.parent = Object;

    // out_int
    class_info.func_table.insert(
        make_pair(out_int, make_pair(SELF_TYPE, build_symbols(Int))));
    // out_string
    class_info.func_table.insert(
        make_pair(out_string, make_pair(SELF_TYPE, build_symbols(Str))));
    // in_int
    class_info.func_table.insert(
        make_pair(in_int, make_pair(Int, build_symbols())));

    // in_string
    class_info.func_table.insert(
        make_pair(in_string, make_pair(Str, build_symbols())));
    classtable->insert(IO, class_info);
  }

  // Str
  {
    ClassInfo class_info;
    class_info.name = Str;
    class_info.parent = Object;
    class_info.func_table.insert(
        make_pair(length, make_pair(Int, build_symbols())));
    class_info.func_table.insert(
        make_pair(substr, make_pair(Str, build_symbols(Int, Int))));
    class_info.func_table.insert(
        make_pair(concat, make_pair(Str, build_symbols(Str))));
    classtable->insert(Str, class_info);
  }

  // Int
  {
    ClassInfo class_info;
    class_info.name = Int;
    class_info.parent = Object;
    classtable->insert(Int, class_info);
  }

  // Bool
  {
    ClassInfo class_info;
    class_info.name = Bool;
    class_info.parent = Object;
    classtable->insert(Bool, class_info);
  }

  // SELF_TYPE
  {
    ClassInfo class_info;
    class_info.name = SELF_TYPE;
    class_info.parent = Object;
    classtable->insert(SELF_TYPE, class_info);
  }
}

static void __semant(ClassTable* classtable, Classes classes) {
  Symbol builtin_name_sym[] = {Object, IO, Int, Str, Bool, SELF_TYPE};
  int n = sizeof(builtin_name_sym) / sizeof(builtin_name_sym[0]);
  bool found_main = false;
  for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
    Class_ class_ = classes->nth(i);
    Symbol class_name = class_->get_name();

    // init class_info
    ClassInfo class_info;
    class_info.name = class_->get_name();
    class_info.parent = class_->get_parent();
    class_->collect_feature(&class_info);

    int id = in_basic_class(class_name, builtin_name_sym, n);
    if (classtable->find(class_name)) {
      // Redefine class
      ECHO_ERR3(class_, "Class ", class_name, " was previously defined");
      return;
    } else if (id != -1) {
      // Redefine basic class
      ECHO_ERR2(class_, "Redefinition of basic class ", class_name);
      return;
    } else {
      classtable->insert(class_name, class_info);
    }
  }

  // Parent class is missing.
  for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
    Class_ class_ = classes->nth(i);
    Symbol class_name = class_->get_name();
    Symbol parent_name = class_->get_parent();
    int pid = in_basic_class(parent_name, builtin_name_sym, n);
    if (pid > 1) {
      ECHO_ERR4(class_, "Class ", class_name, " cannot inherit class ",
                parent_name);
      return;
    } else if (pid == -1 && !classtable->find(parent_name)) {
      ECHO_ERR4(class_, "Class ", class_name,
                " inherits from an undefined class ", parent_name);
      return;
    }
  }

  // No Main class.
  if (!classtable->find(Main)) {
    classtable->semant_error() << "Class Main is not defined.\n";
    return;
  }

  install_basic_classinfo(classtable);

  // Check type for each class.
  for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
    Class_ class_ = classes->nth(i);
    class_->check_type(classtable, class_);
  }
}

// ** NOTICE **: all symbol are in same idtable.
// Two token shared same symbol if they are same type.
// So they can compare with each other by != or ==.
void program_class::semant() {
  initialize_constants();

  ClassTable* classtable = new ClassTable(classes);

  __semant(classtable, classes);
  if (classtable->errors()) {
    cerr << "Compilation halted due to static semantic errors." << endl;
    exit(1);
  }
}
