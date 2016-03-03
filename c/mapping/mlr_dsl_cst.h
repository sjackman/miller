#ifndef MLR_DSL_CST_H
#define MLR_DSL_CST_H

#include "containers/mlr_dsl_ast.h"
#include "rval_evaluators.h"

// xxx update this
// ================================================================
// Concrete syntax tree (CST) derived from an abstract syntax tree (AST).
//
// Statements are of the form:
//
// * Assignment to oosvar (out-of-stream variables, prefixed with @ sigil)
//
// * Assignment to srec (in-stream records, with field names prefixed with $ sigil)
//
// * filter statements: if false, do not pass the current record from the input stream to the output stream
//
// * pattern-action statements: boolean expression with curly-braced statements which are executed only
//   when the boolean evaluates to true.
//
// * bare-boolean statements: no-ops unless they have side effects: namely, the matches/does-not-match
//   operators =~ and !=~ setting regex captures \1, \2, etc.
//
// * emit statements: these place oosvar key-value pairs into the output stream.  These can be of the form 'emit @a;
//   emit @b' which produce separate records such as a=3 and b=4, or of the form 'emit @a, @b' which produce records
//   such as a=3,b=4.
//
// This means that as of the present, the CSTs are just list of left-hand sides and right-hand sides.  The left-hand
// side is an output field name, with an LHS-type flag (srec or oosvar). For filter/bare-boolean these are unused;
// for assignments and emits, the output field names are used. The right-hand sides are lrec-evaluators which take an
// input record and oosvar-map, and produce a mlrval which can be assigned. Additionally, for all but emit, there is a
// single LHS name and single RHS lrec-evaluator per statement.
//
// Further, these statements are organized into three groups:
//
// * begin: executed once, before the first input record is read.
// * main : executed for each input record.
// * end :  executed once, after the last input record is read.
// ================================================================

struct _mlr_dsl_cst_statement_t;

typedef void mlr_dsl_cst_node_evaluator_func_t(
	struct _mlr_dsl_cst_statement_t* pnode,
	mlhmmv_t*        poosvars,
	lrec_t*          pinrec,
	lhmsv_t*         ptyped_overlay,
	string_array_t** ppregex_captures,
	context_t*       pctx,
	int*             pshould_emit_rec,
	sllv_t*          poutrecs);

// Most statements have one item, except multi-oosvar emit.
typedef struct _mlr_dsl_cst_statement_item_t {
	// LHS:
	char* output_field_name;
	sllv_t* poosvar_lhs_keylist_evaluators;
	int all_flag; // for emit all and unset all

	// RHS:
	rval_evaluator_t* prhs_evaluator;

	sllv_t*           pcond_statements;
} mlr_dsl_cst_statement_item_t;

typedef struct _mlr_dsl_cst_statement_t {
	mlr_dsl_cst_node_evaluator_func_t* pevaluator;
	sllv_t* pitems;
} mlr_dsl_cst_statement_t;

typedef struct _mlr_dsl_cst_t {
	sllv_t* pbegin_statements;
	sllv_t* pmain_statements;
	sllv_t* pend_statements;
} mlr_dsl_cst_t;

// ----------------------------------------------------------------
mlr_dsl_cst_t* mlr_dsl_cst_alloc(mlr_dsl_ast_t* past, int type_inferencing);
void mlr_dsl_cst_free(mlr_dsl_cst_t* pcst);

void mlr_dsl_cst_evaluate(
	sllv_t*          pcst_statements, // begin/main/end
	mlhmmv_t*        poosvars,
	lrec_t*          pinrec,
	lhmsv_t*         ptyped_overlay,
	string_array_t** ppregex_captures,
	context_t*       pctx,
	int*             pshould_emit_rec,
	sllv_t*          poutrecs);

#endif // MLR_DSL_CST_H
