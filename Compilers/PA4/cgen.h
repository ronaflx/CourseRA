#include <algorithm>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   std::vector<CgenNodeP> tagged_node;
   ostream& str;
   int stringclasstag;
   int intclasstag;
   int boolclasstag;


// The following methods emit code for
// constants and global declarations.

   void code_global_data();
   void code_global_text();
   void code_bools(int);
   void code_select_gc();
   void code_constants();

   // define by ronaflx
   void code_classobjtab();

   void code_prototype();
   void code_classname();
   void code_disptable();
   void code_initializer();
   void code_classmethod();

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
public:
   CgenClassTable(Classes, ostream& str);
   void code();
   CgenNodeP root();

};


class CgenNode : public class__class {
private: 
   CgenNodeP parentnd;                        // Parent of class
   List<CgenNode> *children;                  // Children of class
   Basicness basic_status;                    // `Basic' if class is basic
                                              // `NotBasic' otherwise
   // define by ronaflx
   int gc_tag;
   int class_tag;
   int child_tag;
   int attrib_size;
   int method_size;
   List<Entry>* method_name;                 // unique method name for each class
   SymbolTable<Symbol, Entry> method_table;   // key: method_name, value: node name
   SymbolTable<Symbol, int>   method_offset;  // key: method_name, value: offset
   SymbolTable<Symbol, int>   attrib_offset;  // key: attrib_name, value: offset
   static int global_class_tag;

public:
   CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP class_table);

   void add_child(CgenNodeP child);
   List<CgenNode> *get_children() { return children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   int basic() { return (basic_status == Basic); }

   // define by ronaflx
   void traversal();  // collect inforamtion of each CgenNode
   void code_prototype(ostream& s);  // Each class has a prototype to call copy().
   void code_prototype_internal(ostream& s);
   void code_attribute(ostream& s);  // Code the attribute section for each class.
   void code_classname(ostream& s);  // Code the classname section for each class.
   void code_disptable(ostream& s);  // Code the disptable section for each class.
   void code_initializer(ostream& s);  // Code initialize function for each class.
   void code_classmethod(ostream& s);  // Code class method for each class.
   void push_children(std::queue<CgenNodeP>* q);

   int lookup_method_offset(Symbol name) { return *method_offset.lookup(name); }
   int lookup_attrib_offset(Symbol name) { return *attrib_offset.lookup(name); }

   int get_class_tag() { return class_tag; }
   int get_child_tag() { return child_tag; }
};
// skip boolclasstag, stringclasstag and intclasstag.
int CgenNode::global_class_tag = 0;

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

