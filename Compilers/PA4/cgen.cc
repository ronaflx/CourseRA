
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include <map>
#include <queue>

#include "cgen.h"
#include "cgen_gc.h"

extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;
// Define by ronaflx
// Offset of each params from frame pointer.
static SymbolTable<Symbol, int> frame_offset;
// global label for branch command.
static int static_label = 0;
static int local_variable = 0;
// Current CgenNode to to help identify SELF_TYPE
static CgenNodeP cur_node = NULL;
static std::map<Symbol, CgenNodeP> classnode_table;
const int DEFAULT_FP_OFFSET = 3;

static CgenNodeP LookupCgenNode(Symbol symbol) {
  return classnode_table.find(symbol)->second;
}


//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
       arg,
       arg2,
       Bool,
       concat,
       cool_abort,
       copy,
       Int,
       in_int,
       in_string,
       IO,
       length,
       Main,
       main_meth,
       No_class,
       No_type,
       Object,
       out_int,
       out_string,
       prim_slot,
       self,
       SELF_TYPE,
       Str,
       str_field,
       substr,
       type_name,
       val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
  arg         = idtable.add_string("arg");
  arg2        = idtable.add_string("arg2");
  Bool        = idtable.add_string("Bool");
  concat      = idtable.add_string("concat");
  cool_abort  = idtable.add_string("abort");
  copy        = idtable.add_string("copy");
  Int         = idtable.add_string("Int");
  in_int      = idtable.add_string("in_int");
  in_string   = idtable.add_string("in_string");
  IO          = idtable.add_string("IO");
  length      = idtable.add_string("length");
  Main        = idtable.add_string("Main");
  main_meth   = idtable.add_string("main");
//   _no_class is a symbol that can't be the name of any 
//   user-defined class.
  No_class    = idtable.add_string("_no_class");
  No_type     = idtable.add_string("_no_type");
  Object      = idtable.add_string("Object");
  out_int     = idtable.add_string("out_int");
  out_string  = idtable.add_string("out_string");
  prim_slot   = idtable.add_string("_prim_slot");
  self        = idtable.add_string("self");
  SELF_TYPE   = idtable.add_string("SELF_TYPE");
  Str         = idtable.add_string("String");
  str_field   = idtable.add_string("_str_field");
  substr      = idtable.add_string("substr");
  type_name   = idtable.add_string("type_name");
  val         = idtable.add_string("_val");
}

static char *gc_init_names[] =
  { "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
  { "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream& s)
{
  s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")" 
    << endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream& s)
{
  s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
      << endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, ostream& s)
{
  emit_partial_load_address(dest,s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream& s)
{
  emit_partial_load_address(dest,s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s)
{
  emit_partial_load_address(dest,s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_add(char *dest, char *src1, char *src2, ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addu(char *dest, char *src1, char *src2, ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address,ostream &s)
{ s << JAL << address << endl; }

static void emit_return(ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, ostream &s)
{ s << "label" << l; }

static void emit_protobj_ref(Symbol sym, ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, ostream &s)
{
  emit_label_ref(l,s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_branch(int l, ostream& s)
{
  s << BRANCH;
  emit_label_ref(l,s);
  s << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream& str)
{
  emit_store(reg,0,SP,str);
  emit_addiu(SP,SP,-4,str);
}

static void emit_pop(ostream& str) {
  emit_addiu(SP,SP,4,str);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }


static void emit_test_collector(ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s); // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_addiu(SP,SP,4,s);
  emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, ostream &s)
{
  if (source != (char*)A1) emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}

static void emit_move_protobj(char *dest, Symbol type, ostream& s) {
  emit_partial_load_address(dest, s);
  emit_protobj_ref(type, s);
  s << endl;
}

// emit_ helper function, define by ronaflx
static void emit_load_protobj(char* dest, Symbol type, ostream& s) {
  emit_move_protobj(dest, type, s);

  // Copy copy() to allocate variable in heap and keep address in ACC.
  // Use ::copy here, since copy is also a function of Expression_class.
  s << JAL;
  emit_method_ref(Object, ::copy, s);
  s << endl;
}

static void emit_function_call(Symbol class_name, Symbol method_name, ostream& s) {
  s << JAL;
  emit_method_ref(class_name, method_name, s);
  s << endl;
}

static void emit_init_call(Symbol class_name, ostream& s) {
  s << JAL;
  emit_init_ref(class_name, s);
  s << endl;
}

static void emit_abort(int label, int lineno, Symbol name, char* abort_prog, ostream& s) {
  emit_bne(ACC, ZERO, label, s);
  emit_load_string(ACC, stringtable.lookup_string(name->get_string()), s);
  emit_load_imm(T1, lineno, s);
  emit_jal(abort_prog, s);
}


///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
  s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s  << LABEL                                             // label
      << WORD << stringclasstag << endl                                 // tag
      << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
      << WORD;


 /***** Add dispatch information for class String ******/

  emit_disptable_ref(Str, s);
  s << endl;                                              // dispatch table
  s << WORD;  lensym->code_ref(s);  s << endl;            // string length
  emit_string_constant(s,str);                                // ascii string
  s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s, int stringclasstag)
{
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
  s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                // label
      << WORD << intclasstag << endl                      // class tag
      << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl  // object size
      << WORD; 

 /***** Add dispatch information for class Int ******/

  emit_disptable_ref(Int, s);
  s << endl;                                          // dispatch table
  s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream& s) const
{
  s << BOOLCONST_PREFIX << val;
}
  
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                  // label
      << WORD << boolclasstag << endl                       // class tag
      << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl   // object size
      << WORD;

 /***** Add dispatch information for class Bool ******/

  emit_disptable_ref(Bool, s);
  s << endl;                                            // dispatch table
  s << WORD << val << endl;                             // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
  Symbol main    = idtable.lookup_string(MAINNAME);
  Symbol string  = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc   = idtable.lookup_string(BOOLNAME);

  str << "\t.data\n" << ALIGN;
  //
  // The following global names must be defined first.
  //
  str << GLOBAL << CLASSNAMETAB << endl;
  str << GLOBAL; emit_protobj_ref(main,str);    str << endl;
  str << GLOBAL; emit_protobj_ref(integer,str); str << endl;
  str << GLOBAL; emit_protobj_ref(string,str);  str << endl;
  str << GLOBAL; falsebool.code_ref(str);  str << endl;
  str << GLOBAL; truebool.code_ref(str);   str << endl;
  str << GLOBAL << INTTAG << endl;
  str << GLOBAL << BOOLTAG << endl;
  str << GLOBAL << STRINGTAG << endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  str << INTTAG << LABEL
      << WORD << intclasstag << endl;
  str << BOOLTAG << LABEL 
      << WORD << boolclasstag << endl;
  str << STRINGTAG << LABEL 
      << WORD << stringclasstag << endl;    
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
  str << GLOBAL << HEAP_START << endl
      << HEAP_START << LABEL 
      << WORD << 0 << endl
      << "\t.text" << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Int"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("String"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"),str);
  str << endl << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
  str << endl;
}

void CgenClassTable::code_bools(int boolclasstag)
{
  falsebool.code_def(str,boolclasstag);
  truebool.code_def(str,boolclasstag);
}

void CgenClassTable::code_select_gc()
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
  str << "_MemMgr_INITIALIZER:" << endl;
  str << WORD << gc_init_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
  str << "_MemMgr_COLLECTOR:" << endl;
  str << WORD << gc_collect_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_TEST" << endl;
  str << "_MemMgr_TEST:" << endl;
  str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str,stringclasstag);
  inttable.code_string_table(str,intclasstag);

  code_bools(boolclasstag);
}

static bool SortedByClassTag(const CgenNodeP& a, const CgenNodeP& b) {
  return a->get_class_tag() < b->get_class_tag();
}

CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s)
{
   enterscope();
   if (cgen_debug) cout << "Building CgenClassTable" << endl;
   install_basic_classes();
   install_classes(classes);
   build_inheritance_tree();
   root()->traversal();

   stringclasstag = LookupCgenNode(Str )->get_class_tag();
   intclasstag    = LookupCgenNode(Int )->get_class_tag();
   boolclasstag   = LookupCgenNode(Bool)->get_class_tag();

   // Push all nodes into a vector and sorted by class_tag.
   std::queue<CgenNodeP> node_queue;
   node_queue.push(root());
   while (!node_queue.empty()) {
     CgenNodeP top = node_queue.front();
     node_queue.pop();
     top->push_children(&node_queue);
     tagged_node.push_back(top);
   }
   std::sort(tagged_node.begin(), tagged_node.end(), SortedByClassTag);

   code();
   exitscope();
}

void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
  //curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
  addid(No_class,
	new CgenNode(class_(No_class,No_class,nil_Features(),filename),
			    Basic,this));
  addid(SELF_TYPE,
	new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
			    Basic,this));
  addid(prim_slot,
	new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
			    Basic,this));

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
  install_class(
   new CgenNode(
    class_(Object, 
	   No_class,
	   append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	   filename),
    Basic,this));

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
   install_class(
    new CgenNode(
     class_(IO, 
            Object,
            append_Features(
            append_Features(
            append_Features(
            single_Features(method(out_string, single_Formals(formal(arg, Str)),
                        SELF_TYPE, no_expr())),
            single_Features(method(out_int, single_Formals(formal(arg, Int)),
                        SELF_TYPE, no_expr()))),
            single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
            single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	   filename),	    
    Basic,this));

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
   install_class(
    new CgenNode(
     class_(Int, 
	    Object,
            single_Features(attr(val, prim_slot, no_expr())),
	    filename),
     Basic,this));

//
// Bool also has only the "val" slot.
//
    install_class(
     new CgenNode(
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
      Basic,this));

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
   install_class(
    new CgenNode(
      class_(Str, 
	     Object,
             append_Features(
             append_Features(
             append_Features(
             append_Features(
             single_Features(attr(val, Int, no_expr())),
            single_Features(attr(str_field, prim_slot, no_expr()))),
            single_Features(method(length, nil_Formals(), Int, no_expr()))),
            single_Features(method(concat, 
				   single_Formals(formal(arg, Str)),
				   Str, 
				   no_expr()))),
	    single_Features(method(substr, 
				   append_Formals(single_Formals(formal(arg, Int)), 
						  single_Formals(formal(arg2, Int))),
				   Str, 
				   no_expr()))),
	     filename),
        Basic,this));

}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP nd)
{
  Symbol name = nd->get_name();

  if (probe(name))
    {
      return;
    }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = new List<CgenNode>(nd,nds);
  addid(name,nd);
}

void CgenClassTable::install_classes(Classes cs)
{
  for(int i = cs->first(); cs->more(i); i = cs->next(i))
    install_class(new CgenNode(cs->nth(i),NotBasic,this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for(List<CgenNode> *l = nds; l; l = l->tl())
      set_relations(l->hd());
}

// define by ronaflx
static void SaveActivityRecord(ostream& s) {
  // Push activation record into stack.
  emit_addiu(SP, SP, -3 * WORD_SIZE, s);
  emit_store(FP, 3, SP, s);
  emit_store(SELF, 2, SP, s);
  emit_store(RA, 1, SP, s);
  
  // get frame pointer of current method.
  emit_addiu(FP, SP, WORD_SIZE, s);
  emit_move(SELF, ACC, s);
}

static void LoadActivityRecord(ostream& s) {
  // Load activation record back to register.
  emit_load(FP, 3, SP, s);
  emit_load(SELF, 2, SP, s);
  emit_load(RA, 1, SP, s);
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}

void CgenNode::traversal() {
  Entry* name = get_name();
  classnode_table.insert(std::make_pair(name, this));
  class_tag = global_class_tag++;
  // Object doesn't have a parent class.
  if (parentnd != NULL) {
    attrib_size = parentnd->attrib_size;
    method_size = parentnd->method_size;

    method_name = parentnd->method_name;
    method_table = parentnd->method_table;
    method_offset = parentnd->method_offset;
    attrib_offset = parentnd->attrib_offset;
  }
  // Symbol table is a linked list, this will change the head of smybol table
  // and reuse parent's table.
  attrib_offset.enterscope();
  method_offset.enterscope();
  method_table.enterscope();

  for (int i = features->first(); features->more(i); i = features->next(i)) {
    Symbol feature_name = features->nth(i)->get_name();
    if (features->nth(i)->is_attrib()) {
      attrib_offset.addid(feature_name, new int(DEFAULT_OBJFIELDS + attrib_size));
      attrib_size++;
    } else {
      int * offset = method_offset.lookup(feature_name);
      if (offset == NULL) {
        method_name = new List<Entry>(feature_name, method_name);
        method_offset.addid(feature_name, new int(method_size));
        method_table.addid(feature_name, name);
        method_size++;
      } else {
        method_table.addid(feature_name, name);
      }
    }
  }

  // Do traversal for each child class.
  for (List<CgenNode>* head = children; head != NULL; head = head->tl()) {
    head->hd()->traversal();
  }
  child_tag = global_class_tag - 1;
}

// -1 eye catcher
// class_tag
// object size in word
// dispatch table
// ... field 0
// ... field n
void CgenNode::code_prototype(ostream& s) {
  cur_node = this;
  s << WORD << "-1" << endl;
  // Label of prototype
  emit_protobj_ref(get_name(), s);
  s << LABEL;

  // emit class tag
  s << WORD << class_tag << endl;

  // emit object size (classtag + disptablee + attrib_size + [attribs])
  s << WORD << (DEFAULT_OBJFIELDS + attrib_size) << endl;

  // emit dispatch table
  s << WORD;
  emit_disptable_ref(get_name(), s);
  s << endl;

  // emit attribs
  code_attribute(s);
}

void CgenNode::code_attribute(ostream& s) {
  cur_node = this;
  if (parentnd != NULL) {
    parentnd->code_attribute(s);
  }
  for (int i = features->first(); features->more(i); i = features->next(i)) {
    if (features->nth(i)->is_attrib()) {
      s << WORD;
      Symbol type = features->nth(i)->get_type();
      if (type == Int) {
        inttable.lookup_string("0")->code_ref(s);
      } else if (type == Bool) {
        falsebool.code_ref(s);
      } else if (type == Str) {
        stringtable.lookup_string("")->code_ref(s);
      } else {
        // For normal object the initial ref is null.
        s << 0;
      }
      s << endl;
    }
  }
}

void CgenNode::code_classname(ostream& s) {
  cur_node = this;
  s << WORD;
  stringtable.lookup_string(get_name()->get_string())->code_ref(s);
  s << endl;
}

// Code disptable for this node with all methods including inherited ones.
void CgenNode::code_disptable(ostream& s) {
  cur_node = this;
  std::vector<Symbol> symbols;
  for (List<Entry>* l = method_name; l != NULL; l = l->tl()) {
    symbols.push_back(l->hd());
  }
  int n = static_cast<int>(symbols.size());
  std::reverse(symbols.begin(), symbols.end());
  for (int i = 0; i < n; i++) {
    Symbol method_name = symbols[i];
    Symbol class_node  = method_table.lookup(method_name);
    s << WORD;
    emit_method_ref(class_node, method_name, s);
    s << endl;
  }
}

void CgenNode::code_initializer(ostream& s) {
  cur_node = this;
  Symbol name = get_name();
  emit_init_ref(name, s);
  s << LABEL;
  SaveActivityRecord(s);

  if (name != Object) {
    emit_init_call(parentnd->get_name(), s);
  }
  int offset = DEFAULT_OBJFIELDS + parentnd->attrib_size;
  for (int i = features->first(); features->more(i); i = features->next(i)) {
    Feature feature = features->nth(i);
    if (feature->is_attrib()) {
      feature->code(s, &offset);
      offset++;
    }
  }
  emit_move(ACC, SELF, s);
  LoadActivityRecord(s);
  emit_addiu(SP, SP, 3 * WORD_SIZE, s);
  emit_return(s);
}

void CgenNode::code_classmethod(ostream& s) {
  cur_node = this;
  if (basic()) return;
  for (int i = features->first(); features->more(i); i = features->next(i)) {
    Feature feature = features->nth(i);
    if (feature->is_method()) {
      feature->code(s, this);
    }
  }
}


void CgenNode::push_children(std::queue<CgenNodeP>* q) {
  for (List<CgenNode>* head = children; head != NULL; head = head->tl()) {
    q->push(head->hd());
  }
}

void CgenClassTable::code_classobjtab() {
  str << CLASSOBJTAB << ":" << endl;
  const int n = static_cast<int>(tagged_node.size());
  for (int i = 0; i < n; i++) {
    str << WORD;
    emit_protobj_ref(tagged_node[i]->get_name(), str);
    str << endl;

    str << WORD;
    emit_init_ref(tagged_node[i]->get_name(), str);
    str << endl;
  }
}

void CgenClassTable::code_prototype() {
  const int n = static_cast<int>(tagged_node.size());
  for (int i = 0; i < n; i++) {
    tagged_node[i]->code_prototype(str);
  }
}

void CgenClassTable::code_classname() {
  str << CLASSNAMETAB << LABEL;
  int n = static_cast<int>(tagged_node.size());
  for (int i = 0; i < n; i++) {
    tagged_node[i]->code_classname(str);
  }
}

void CgenClassTable::code_disptable() {
  const int n = static_cast<int>(tagged_node.size());
  for (int i = 0; i < n; i++) {
    emit_disptable_ref(tagged_node[i]->get_name(), str);
    str << LABEL;
    tagged_node[i]->code_disptable(str);
  }
}

void CgenClassTable::code_initializer() {
  const int n = static_cast<int>(tagged_node.size());
  for (int i = 0; i < n; i++) {
    tagged_node[i]->code_initializer(str);
  }
}

void CgenClassTable::code_classmethod() {
  const int n = static_cast<int>(tagged_node.size());
  for (int i = 0; i < n; i++) {
    tagged_node[i]->code_classmethod(str);
  }
}

void CgenClassTable::code()
{
  str << "# start to code global data" << endl;
  code_global_data();

  str << "# start to code choosing gc" << endl;
  code_select_gc();

  str << "# start to code constants" << endl;
  code_constants();

//                 Add your code to emit
//                   - prototype objects
//                   - class_nameTab
//                   - dispatch tables
//
  str << "# start to code class_objtab" <<endl;
  code_classobjtab();
  str << "# start to code classname" << endl;
  code_classname();
  str << "# start to code disptable" << endl;
  code_disptable();
  str << "# start to code prototype" << endl;
  code_prototype();

  if (cgen_debug) cout << "coding global text" << endl;
  str << "# start to code global text" << endl;
  code_global_text();


//                 Add your code to emit
//                   - object initializer
//                   - the class methods
//                   - etc...
  str << "# start to code object initializer" << endl;
  code_initializer();

  str << "# start to code class methods" << endl;
  code_classmethod();
}


CgenNodeP CgenClassTable::root()
{
   return probe(Object);
}


///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
   class__class((const class__class &) *nd),
   parentnd(NULL),
   children(NULL),
   basic_status(bstatus),
   gc_tag(0),
   class_tag(0),
   child_tag(0),
   attrib_size(0),
   method_size(0),
   method_name(NULL)
{ 
   stringtable.add_string(name->get_string());          // Add class name to string table
}


//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

// Hacky code to make emit_* function make be shared with cool-tree code.
// Store to a offset of SELF 
void attr_class::code(ostream& s, void *offset) {
  if (init->get_type() != NULL) {
    init->code(s);
    emit_store(ACC, *(int*)offset, SELF, s);
  }
}

void method_class::code(ostream& s, void* classname) {
  CgenNodeP classnode = (CgenNodeP)classname;
  emit_method_ref(classnode->get_name(), get_name(), s);
  s << LABEL;

  SaveActivityRecord(s);

  int params_len = formals->len();
  frame_offset.enterscope();
  for (int i  = formals->first(); formals->more(i);i = formals->next(i)) {
    frame_offset.addid(formals->nth(i)->get_name(), new int(params_len - i - 1));
  }
  local_variable = 0;
  expr->code(s);
  frame_offset.exitscope();

  LoadActivityRecord(s);
  emit_addiu(SP, SP, (3 + params_len) * WORD_SIZE, s);
  emit_return(s);
}

void assign_class::code(ostream &s) {
  expr->code(s);
  int* frame_pos = frame_offset.lookup(name);
  if (frame_pos != NULL) {
    emit_store(ACC, *frame_pos + DEFAULT_FP_OFFSET, FP, s);
  } else {
    int value = cur_node->lookup_attrib_offset(name);
    emit_store(ACC, value, SELF, s);
  }
}

void static_dispatch_class::code(ostream &s) {
  for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->code(s);
    emit_push(ACC, s);
  }
  expr->code(s);

  const int good_label = static_label++;
  emit_abort(good_label, line_number, cur_node->filename, "_dispatch_abort", s);
  emit_label_def(good_label, s);

  CgenNodeP node = LookupCgenNode(type_name);
  int offset_disptable = node->lookup_method_offset(name);
  emit_move_protobj(T1, type_name, s);
  emit_load(T1, DISPTABLE_OFFSET, T1, s);  // dispatch table.
  emit_load(T1, offset_disptable, T1, s);  // find the entry.
  emit_jalr(T1, s);
}

void dispatch_class::code(ostream &s) {
  for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->code(s);
    emit_push(ACC, s);
  }

  expr->code(s);

  const int not_null_label = static_label++;
  emit_abort(not_null_label, line_number, cur_node->filename, "_dispatch_abort", s);
  emit_label_def(not_null_label, s);

  Symbol expr_type = expr->get_type();
  CgenNodeP node = NULL;
  if (expr_type == SELF_TYPE) {
    node = cur_node;
  } else {
    node = LookupCgenNode(expr_type);
  }

  int offset_disptable = node->lookup_method_offset(name);
  emit_load(T1, DISPTABLE_OFFSET, ACC, s);  // dispatch table.
  emit_load(T1, offset_disptable, T1, s);   // find the entry.
  emit_jalr(T1, s);
}

void cond_class::code(ostream &s) {
  pred->code(s);

  const int false_label = static_label++;
  const int endif_label = static_label++;
  emit_load_bool(T1, falsebool, s);
  emit_beq(T1, ACC, false_label, s);

  then_exp->code(s);
  emit_branch(endif_label, s);

  emit_label_def(false_label, s);
  else_exp->code(s);

  emit_label_def(endif_label, s);
}

void loop_class::code(ostream &s) {
  const int start_label = static_label++;
  const int limit_label = static_label++;

  emit_label_def(start_label, s);
  pred->code(s);

  emit_load_bool(T1, falsebool, s);
  emit_beq(ACC, T1, limit_label, s);

  body->code(s);
  emit_branch(start_label, s);

  emit_label_def(limit_label, s);
  emit_move(ACC, ZERO, s);
}

void typcase_class::code(ostream &s) {
  expr->code(s);
  emit_push(ACC, s);
  local_variable++;

  // Push all case var into local variable table with same address.
  frame_offset.enterscope();
  for (int i = cases->first(); cases->more(i);i = cases->next(i)) {
    const int frame_pos = -local_variable - DEFAULT_FP_OFFSET;
    frame_offset.addid(cases->nth(i)->get_name(), new int(frame_pos));
  }

  // abort the programm if the expr is void.
  const int good_label = static_label++;
  emit_abort(good_label, line_number, cur_node->filename, "_case_abort2", s);
  emit_label_def(good_label, s);

  // Sort all cases node by class_tag.
  std::vector<CgenNodeP> branchs;
  std::map<CgenNodeP, Case_class*> expr_mapping;
  for (int i = cases->first(); cases->more(i);i = cases->next(i)) {
    CgenNodeP node = LookupCgenNode(cases->nth(i)->get_type_decl());
    branchs.push_back(node);
    expr_mapping[node] = cases->nth(i);
  }
  std::sort(branchs.begin(), branchs.end(), SortedByClassTag);

  // Case match algorithm:
  // for each case statement we can have a tag range.
  // compare statement tag range with dynamic class tag.
  Symbol expr_type = expr->get_type();
  const int matched_label = static_label++;
  const int n = static_cast<int>(branchs.size());
  for (int i = n - 1; i >= 0; i--) {
    const int outside_label = static_label++;
    Case_class* case_node = expr_mapping[branchs[i]];
    CgenNodeP cgen_node   = LookupCgenNode(case_node->get_type_decl());

    emit_load_protobj(T2, case_node->get_type_decl(), s);
    emit_load(T2, TAG_OFFSET, T2, s);

    emit_load(T1, 1, SP, s);
    emit_load(T1, TAG_OFFSET, T1, s);
    emit_blti(T1, cgen_node->get_class_tag(), outside_label, s);
    emit_bgti(T1, cgen_node->get_child_tag(), outside_label, s);

    case_node->get_expression()->code(s);
    emit_branch(matched_label, s);
    emit_label_def(outside_label, s);
  }

  // non matched abort.
  emit_load(ACC, 1, SP, s);
  emit_load_imm(T1, line_number, s);
  emit_jal("_case_abort", s);

  emit_label_def(matched_label, s);

  frame_offset.exitscope();
  emit_pop(s);
  local_variable--;
}

void block_class::code(ostream &s) {
  for (int i = body->first();body->more(i); i = body->next(i)) {
    body->nth(i)->code(s);
  }
}

void let_class::code(ostream &s) {
  Symbol init_type = init->get_type();
  if (init_type != NULL) {
    init->code(s);
  } else if (type_decl == Int) {
    emit_load_int(ACC, inttable.lookup_string("0"), s);
  } else if (type_decl == Bool) {
    emit_load_bool(ACC, falsebool, s);
  } else if (type_decl == Str) {
    emit_load_string(ACC, stringtable.lookup_string(""),s);
  } else {
    emit_load_imm(ACC, 0, s);
  }
  local_variable++;
  frame_offset.enterscope();
  frame_offset.addid(identifier, new int(-local_variable - DEFAULT_FP_OFFSET));
  emit_push(ACC, s);
  body->code(s);
  emit_pop(s);
  frame_offset.exitscope();
  local_variable--;
}

// T1 -> e1, T2 -> e2.
#define BINARY_EXPR(cmd) \
  e1->code(s); \
  emit_push(ACC, s); \
  e2->code(s); \
  emit_push(ACC, s); \
  emit_load_protobj(ACC, Int, s); \
  emit_load(T1, 2, SP, s); \
  emit_fetch_int(T1, T1, s); \
  emit_load(T2, 1, SP, s); \
  emit_fetch_int(T2, T2, s); \
  emit_##cmd(T1, T1, T2, s); \
  emit_store_int(T1, ACC, s); \
  emit_addiu(SP, SP, 8, s); \

void plus_class::code(ostream &s) {
  BINARY_EXPR(add);
}

void sub_class::code(ostream &s) {
  BINARY_EXPR(sub);
}

void mul_class::code(ostream &s) {
  BINARY_EXPR(mul);
}

void divide_class::code(ostream &s) {
  BINARY_EXPR(div);
}

void neg_class::code(ostream &s) {
  e1->code(s);

  // Copy return value to acc.
  emit_function_call(Object, ::copy, s);

  emit_fetch_int(T1, ACC, s);
  emit_neg(T1, T1, s);
  emit_store_int(T1, ACC, s);
}

#define COMP_EXPR(cmd) \
  const int branch_label = static_label++; \
  const int end_branch_label = static_label++; \
  e1->code(s); \
  emit_push(ACC, s); \
  e2->code(s); \
  emit_load(T1, 1, SP, s); \
  emit_pop(s); \
  emit_fetch_int(T1, T1, s); \
  emit_fetch_int(ACC, ACC, s); \
  emit_##cmd(T1, ACC, branch_label, s); \
  emit_load_bool(ACC, falsebool, s); \
  emit_branch(end_branch_label, s); \
  emit_label_def(branch_label, s); \
  emit_load_bool(ACC, truebool, s); \
  emit_label_def(end_branch_label, s)

void lt_class::code(ostream &s) {
  COMP_EXPR(blt);
}

void eq_class::code(ostream &s) {
  const int equal_label = static_label++;
  e1->code(s);
  emit_push(ACC, s);
  e2->code(s);
  emit_load(T1, 1, SP, s);
  emit_pop(s);
  emit_move(T2, ACC, s);
  emit_load_bool(ACC, truebool, s);
  emit_beq(T1, T2, equal_label, s);
  emit_load_bool(A1, falsebool, s);
  emit_jal("equality_test", s);
  emit_label_def(equal_label, s);
}

void leq_class::code(ostream &s) {
  COMP_EXPR(bleq);
}

void comp_class::code(ostream &s) {
  s << "# code comp_class" << endl;
  const int true_label = static_label++;
  const int end_label  = static_label++;

  e1->code(s);
  emit_load_bool(T1, falsebool, s);
  emit_beq(T1, ACC, true_label, s);

  emit_load_bool(ACC, falsebool, s);
  emit_branch(end_label, s);

  emit_label_def(true_label, s);
  emit_load_bool(ACC, truebool, s);
  emit_label_def(end_label, s);
  s << "# end code comp_class" << endl;
}

void int_const_class::code(ostream& s)  
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  emit_load_int(ACC,inttable.lookup_string(token->get_string()),s);
}

void string_const_class::code(ostream& s)
{
  emit_load_string(ACC,stringtable.lookup_string(token->get_string()),s);
}

void bool_const_class::code(ostream& s)
{
  emit_load_bool(ACC, BoolConst(val), s);
}

void new__class::code(ostream &s) {
  if (type_name == SELF_TYPE) {
    // the first field in class layer is tag.
    // all protobj and init function are sorted by class_tag.
    // the location of proto obj (class_tag << 3)
    // the location of init function is (class_tag << 3) + 4
    emit_load_address(T1, CLASSOBJTAB, s);
    emit_load(T2, TAG_OFFSET, SELF, s);
    emit_sll(T2, T2, 3, s);
    emit_addu(T1, T1, T2, s);

    // Copy SELF to ACC.
    emit_load(ACC, 0, T1, s);
    emit_push(T1, s);
    emit_function_call(Object, ::copy, s);
    emit_load(T1, 1, SP, s);
    emit_pop(s);

    // Call init function
    emit_load(T1, 1, T1, s);
    emit_jalr(T1, s);
  } else {
    // Load proto type to ACC.
    emit_load_protobj(ACC, type_name, s);
    // Jump to initializer method.
    emit_init_call(type_name, s);
  }
}

void isvoid_class::code(ostream &s) {
  const int not_void_label = static_label++;
  const int limit_label = static_label++;
  e1->code(s);
  emit_blti(ACC, 0, not_void_label, s);
  emit_bgti(ACC, 0, not_void_label, s);

  // is void
  emit_load_bool(ACC, truebool, s);
  emit_branch(limit_label, s);

  // is not void
  emit_label_def(not_void_label, s);
  emit_load_bool(ACC, falsebool, s);

  emit_label_def(limit_label, s);
}

void no_expr_class::code(ostream &s) {
  emit_load_imm(ACC, 0, s);
}

void object_class::code(ostream &s) {
  if (name == self) {
    // name could be self, eg dispatch, Method(param0, param1)
    emit_move(ACC, SELF, s);
  } else {
    int* frame_pos = frame_offset.lookup(name);
    if (frame_pos != NULL) {
      emit_load(ACC, *frame_pos + DEFAULT_FP_OFFSET, FP, s);
    } else {
      int value = cur_node->lookup_attrib_offset(name);
      emit_load(ACC, value, SELF, s);
    }
  }
}
