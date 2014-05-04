#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
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
using std::set;
using std::string;
using std::vector;

class ClassTable;
typedef ClassTable* ClassTableP;

class Entry;
typedef Entry* Sybmol;
// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

struct ClassInfo {
  Symbol name, parent;
  typedef vector<Sybmol> Symbols;
  // <name, return_type, formals>
  typedef map<string, pair<Symbol, Symbols> > FuncTable;
  typedef FuncTable::mapped_type FuncInfo;
  // <name, type>
  typedef map<string, Symbol> AttrTable;
  FuncTable func_table;
  AttrTable attr_table;

  // Return true if has FuncInfo of function f, false otherwise.
  bool has_func_info(const string& f) const {
    FuncTable::const_iterator i = func_table.find(f);
    return i != func_table.end();
  }
  // Get const reference FuncInfo of function f.
  const FuncInfo& get_func_info(const string& f) const {
    FuncTable::const_iterator i = func_table.find(f);
    assert(i != func_table.end());
    return i->second;
  }
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
  typedef map<string, ClassInfo> ClassMap;
  map<string, ClassInfo> class_name;

 public:
  SymbolTable<string, Entry>* sym_table;  // id -> type table

  // Return const refrence ClassInfo of class c.
  const ClassInfo& get_class_info(const string& c) const {
    ClassMap::const_iterator i = class_name.find(c);
    assert(i != class_name.end());
    return i->second;
  }

  // Return const Reference FuncTable::value_type of class c, function f.
  const ClassInfo::FuncInfo& get_func_info(const string& c,
                                           const string& f) const {
    string ptype = c;
    while (!is_object(ptype)) {
      const ClassInfo& class_info = get_class_info(ptype);
      if (class_info.has_func_info(f)) {
        return class_info.get_func_info(f);
      } else {
        ptype = class_info.parent->get_string();
      }
    }
    // Object
    const ClassInfo& class_info = get_class_info(ptype);
    if (class_info.has_func_info(f)) {
      return class_info.get_func_info(f);
    }
    static ClassInfo::FuncInfo empty_func_info;
    return empty_func_info;
  }

  // Return true if find class c.
  bool find(const string& c) const {
    return class_name.find(c) != class_name.end();
  }

  // Insert ClasInfo of class c.
  void insert(const string& c, const ClassInfo& class_info) {
    class_name.insert(make_pair(c, class_info));
  }

  // Return all parents of a include itself.
  void get_all_parents(const string& a, ClassInfo::Symbols* parents) const {
    string ptype = a;
    while (!is_object(ptype)) {
      const ClassInfo& class_info = get_class_info(ptype);
      parents->push_back(class_info.name);
      ptype = class_info.parent->get_string();
    }
    // Object
    const ClassInfo& class_info = get_class_info(ptype);
    parents->push_back(class_info.name);
  }

  // Return true if a is subtype of b, false otherwise.
  bool is_sub_type_of(const string& a, const string& b) const {
    if (a == b) return true;
    string ptype = a;
    do {
      const ClassInfo& class_info = get_class_info(ptype);
      ptype = class_info.parent->get_string();
      if (ptype == b) return true;
    } while (!is_object(ptype));
    return false;
  }

  // Symbol params version.
  bool is_sub_type_of(Symbol a, Symbol b) const {
    return is_sub_type_of(a->get_string(), b->get_string());
  }

  // Return true if a override attr of it parents, false otherwise.
  bool is_override_attr(const string& a, const string& attr) const {
    string ptype = a;
    do {
      const ClassInfo& class_info = get_class_info(ptype);
      ptype = class_info.parent->get_string();
      const ClassInfo& pclass_info = get_class_info(ptype);
      if (pclass_info.attr_table.find(attr) != pclass_info.attr_table.end())
        return true;
    } while (!is_object(ptype));
    return false;
  }

  // Return true if a redefine function f in it parents, false otherwise.
  FUNC_OVERRIDE_TYPE is_override_func(const string& a, const string& func,
                                      pair<Symbol, Symbol>* diff = NULL) const {
    const ClassInfo::FuncInfo& desc =
        get_class_info(a).func_table.find(func)->second;
    string ptype = a;
    do {
      const ClassInfo& class_info = get_class_info(ptype);
      ClassInfo::FuncTable::const_iterator k = class_info.func_table.find(func);
      if (k != class_info.func_table.end()) {
        const ClassInfo::FuncInfo override_desc = k->second;
        if (override_desc.first != desc.first) {
          if (diff != NULL) {
            *diff = make_pair(k->second.first, desc.first);
          }
          return OVERRIDE_RETURN_TYPE;
        } else if (override_desc.second.size() != desc.second.size()) {
          return OVERRIDE_NUMS_PARAMS_TYPE;
        } else if (override_desc.second != desc.second) {
          for (int i = 0; i < int(desc.second.size()); i++) {
            if (k->second.second[i] != desc.second[i] && diff != NULL) {
              *diff = make_pair(k->second.second[i], desc.second[i]);
              return OVERRIDE_PARAMS_TYPE;
            }
          }
        }
      }
      ptype = class_info.parent->get_string();
    } while (!is_object(ptype));
    return NO_ERROR_TYPE;
  }

  // Return true if c is a basic class, false otherwise.
  bool is_object(const string& c) const {
    const ClassInfo& class_info = get_class_info(c);
    return class_info.parent->get_string() == c;
  }

  // Retturn the Symbol of LCA(a, b).
  Symbol get_lca(const string& a, const string& b) const {
    vector<Symbol> n1, n2;
    get_all_parents(a, &n1);
    get_all_parents(b, &n2);
    for (vector<Symbol>::const_iterator i = n1.begin(); i != n1.end(); ++i) {
      for (vector<Symbol>::const_iterator j = n2.begin(); j != n2.end(); ++j) {
        if (*i == *j) return *i;
      }
    }
    return NULL;
  }

  // Symbol version
  Symbol get_lca(Symbol a, Symbol b) const {
    return get_lca(a->get_string(), b->get_string());
  }

  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node* t);
};

#endif
