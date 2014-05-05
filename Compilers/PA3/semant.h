#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

using std::make_pair;
using std::map;
using std::pair;
using std::vector;

class ClassTable;
typedef ClassTable* ClassTableP;

class Entry;
typedef Entry* Sybmol;
// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassInfo {
 public:
  Symbol name, parent;
  typedef vector<Sybmol> Symbols;
  // <name, return_type, formals>
  typedef map<Symbol, pair<Symbol, Symbols> > FuncTable;
  typedef FuncTable::mapped_type FuncInfo;
  // <name, type>
  typedef map<Symbol, Symbol> AttrTable;

  // Return true if has FuncInfo of function f, false otherwise.
  bool has_func_info(Symbol func) const {
    return func_table.find(func) != func_table.end();
  }

  // Get const reference FuncInfo of function f.
  const FuncInfo& get_func_info(Symbol func) const {
    FuncTable::const_iterator i = func_table.find(func);
    assert(i != func_table.end());
    return i->second;
  }

  // Return true if has Attr attr, false otherwise
  bool has_attr_info(Symbol attr_sym) const {
    return attr_table.find(attr_sym) != attr_table.end();
  }

  // Return Symbo of attr
  Symbol get_attr_info(Symbol attr_sym) const {
    AttrTable::const_iterator i = attr_table.find(attr_sym);
    assert(i != attr_table.end());
    return i->second;
  }
  FuncTable func_table;
  AttrTable attr_table;
};

// Functio override error type.
enum FUNC_OVERRIDE_TYPE {
  NO_ERROR_TYPE,
  OVERRIDE_RETURN_TYPE,
  OVERRIDE_PARAMS_TYPE,
  OVERRIDE_NUMS_PARAMS_TYPE
};

class ClassTable {
 private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;
  Classes classes;
  typedef map<Symbol, ClassInfo> ClassMap;
  ClassMap class_name;

 public:
  SymbolTable<Symbol, Entry>* sym_table;  // id -> type table

  // Return true if find class c.
  bool find(Symbol c) const { return class_name.find(c) != class_name.end(); }

  // Insert ClasInfo of class c.
  void insert(Symbol c, const ClassInfo& class_info) {
    class_name.insert(make_pair(c, class_info));
  }

  // Return const refrence ClassInfo of class c.
  const ClassInfo& get_class_info(Symbol c) const {
    ClassMap::const_iterator i = class_name.find(c);
    assert(i != class_name.end());
    return i->second;
  }

  // Return parent of c.
  Symbol get_parent(Symbol c) const { return get_class_info(c).parent; }

  // Return const Reference FuncTable::value_type of class c, function f.
  const ClassInfo::FuncInfo& get_func_info(Symbol c, Symbol f) const {
    Symbol cur_type = c, pre_type = NULL;
    while (pre_type != cur_type) {
      const ClassInfo& class_info = get_class_info(cur_type);
      if (class_info.has_func_info(f)) {
        return class_info.get_func_info(f);
      } else {
        pre_type = cur_type;
        cur_type = class_info.parent;
      }
    }
    static ClassInfo::FuncInfo empty_func_info;
    return empty_func_info;
  }

  // Return all parents of a include itself.
  void get_all_parents(Symbol c, ClassInfo::Symbols* parents) const {
    Symbol cur_type = c, pre_type = NULL;
    while (pre_type != cur_type) {
      parents->push_back(cur_type);
      pre_type = cur_type;
      cur_type = get_parent(cur_type);
    }
  }

  // Return true if a is subtype of b, false otherwise.
  bool is_sub_type_of(Symbol a, Symbol b) const {
    Symbol cur_type = a, pre_type = NULL;
    while (pre_type != cur_type) {
      if (cur_type == b) return true;
      pre_type = cur_type;
      cur_type = get_parent(cur_type);
    }
    return false;
  }

  // Return true if a override attr of it parents, false otherwise.
  bool is_override_attr(Symbol a, Symbol attr) const {
    Symbol cur_type = a, pre_type = NULL;
    while (pre_type != cur_type) {
      pre_type = cur_type;
      cur_type = get_parent(cur_type);
      const ClassInfo& class_info = get_class_info(cur_type);
      if (class_info.has_attr_info(attr)) return true;
    }
    return false;
  }

  // Return true if a redefine function f in it parents, false otherwise.
  FUNC_OVERRIDE_TYPE is_override_func(Symbol a, Symbol func,
                                      pair<Symbol, Symbol>* diff = NULL) const {
    const ClassInfo::FuncInfo& desc = get_class_info(a).get_func_info(func);
    const ClassInfo::Symbols& cur_syms = desc.second;
    Symbol cur_type = a, pre_type = NULL;
    while (cur_type != pre_type) {
      const ClassInfo& class_info = get_class_info(cur_type);
      if (class_info.has_func_info(func)) {
        const ClassInfo::FuncInfo& pdesc = class_info.get_func_info(func);
        const ClassInfo::Symbols& syms = pdesc.second;
        if (pdesc.first != desc.first) {
          if (diff != NULL) *diff = make_pair(pdesc.first, desc.first);
          return OVERRIDE_RETURN_TYPE;
        } else if (cur_syms.size() != syms.size()) {
          return OVERRIDE_NUMS_PARAMS_TYPE;
        } else if (cur_syms != syms) {
          for (int i = 0; i < int(cur_syms.size()); i++) {
            if (syms[i] != cur_syms[i]) {
              if (diff != NULL) *diff = make_pair(syms[i], cur_syms[i]);
              return OVERRIDE_PARAMS_TYPE;
            }
          }
        }
      }
      pre_type = cur_type;
      cur_type = get_parent(cur_type);
    }
    return NO_ERROR_TYPE;
  }

  // Return the Symbol of LCA(a, b).
  Symbol get_lca(Symbol a, Symbol b) const {
    vector<Symbol> n1, n2;
    get_all_parents(a, &n1);
    get_all_parents(b, &n2);
    for (vector<Symbol>::iterator i = n1.begin(); i != n1.end(); ++i) {
      vector<Symbol>::iterator j = std::find(n2.begin(), n2.end(), *i);
      if (j != n2.end()) return *j;
    }
    return NULL;
  }

  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node* t);
};

#endif
