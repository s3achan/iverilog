#ifndef __netmisc_H
#define __netmisc_H
/*
 * Copyright (c) 1999-2011 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  "netlist.h"

/*
 * Search for a symbol using the "start" scope as the starting
 * point. If the path includes a scope part, then locate the
 * scope first.
 *
 * The return value is the scope where the symbol was found.
 * If the symbol was not found, return 0. The output arguments
 * get 0 except for the pointer to the object that represents
 * the located symbol.
 *
 * The ex1 and ex2 output arguments are extended results. If the
 * symbol is a parameter (par!=0) then ex1 is the msb expression and
 * ex2 is the lsb expression for the range. If there is no range, then
 * these values are set to 0.
 */
extern NetScope* symbol_search(const LineInfo*li,
                               Design*des,
			       NetScope*start,
                               pform_name_t path,
			       NetNet*&net,       /* net/reg */
			       const NetExpr*&par,/* parameter/expr */
			       NetEvent*&eve,     /* named event */
			       const NetExpr*&ex1, const NetExpr*&ex2);

inline NetScope* symbol_search(const LineInfo*li,
                               Design*des,
			       NetScope*start,
                               const pform_name_t&path,
			       NetNet*&net,       /* net/reg */
			       const NetExpr*&par,/* parameter/expr */
			       NetEvent*&eve      /* named event */)
{
      const NetExpr*ex1, *ex2;
      return symbol_search(li, des, start, path, net, par, eve, ex1, ex2);
}

/*
 * This function transforms an expression by padding the high bits
 * with V0 until the expression has the desired width. This may mean
 * not transforming the expression at all, if it is already wide
 * enough.
 */
extern NetExpr*pad_to_width(NetExpr*expr, unsigned wid, const LineInfo&info);
extern NetNet*pad_to_width(Design*des, NetNet*n, unsigned w,
                           const LineInfo&info);

extern NetNet*pad_to_width_signed(Design*des, NetNet*n, unsigned w,
                                  const LineInfo&info);

/*
 * Generate the nodes necessary to cast an expression (a net) to a
 * real value.
 */
extern NetNet*cast_to_int4(Design*des, NetScope*scope, NetNet*src, unsigned wid);
extern NetNet*cast_to_int2(Design*des, NetScope*scope, NetNet*src, unsigned wid);
extern NetNet*cast_to_real(Design*des, NetScope*scope, NetNet*src);

extern NetExpr*cast_to_int2(NetExpr*expr);

/*
 * Take the input expression and return a variation that assures that
 * the expression is 1-bit wide and logical. This reflects the needs
 * of conditions i.e. for "if" statements or logical operators.
 */
extern NetExpr*condition_reduce(NetExpr*expr);

/*
 * This function transforms an expression by cropping the high bits
 * off with a part select. The result has the width w passed in. This
 * function does not pad, use pad_to_width if padding is desired.
 */
extern NetNet*crop_to_width(Design*des, NetNet*n, unsigned w);

/*
 * These functions generate an equation to normalize an expression using
 * the provided vector/array information.
 */
extern NetExpr*normalize_variable_base(NetExpr *base, long msb, long lsb,
                                       unsigned long wid, bool is_up,
				       long slice_off =0);
extern NetExpr*normalize_variable_base(NetExpr *base,
				       const list<NetNet::range_t>&dims,
				       unsigned long wid, bool is_up);

/*
 * Calculate a canonicalizing expression for a bit select, when the
 * base expression is the last index of an otherwise complete bit
 * select. For example:
 *   reg [3:0][7:0] foo;
 *   ... foo[1][x] ...
 * base is (x) and the generated expression will be (x+8).
 */
extern NetExpr*normalize_variable_bit_base(const list<long>&indices, NetExpr *base,
					   const NetNet*reg);

/*
 * This is similar to normalize_variable_bit_base, but the tail index
 * it a base and width, instead of a bit. This is used for handling
 * indexed part selects:
 *   reg [3:0][7:0] foo;
 *   ... foo[1][x +: 2]
 * base is (x), wid input is (2), and is_up is (true). The output
 * expression is (x+8).
 */
extern NetExpr *normalize_variable_part_base(const list<long>&indices, NetExpr*base,
					     const NetNet*reg,
					     unsigned long wid, bool is_up);
/*
 * Calculate a canonicalizing expression for a slice select. The
 * indices array is less then needed to fully address a bit, so the
 * result is a slice of the packed array. The return value is an
 * expression that gets to the base of the slice, and (lwid) becomes
 * the width of the slice, in bits. For example:
 *   reg [4:1][7:0] foo
 *   ...foo[x]...
 * base is (x) and the generated expression will be (x*8 - 8), with
 * lwid set to (8).
 */
extern NetExpr*normalize_variable_slice_base(const list<long>&indices, NetExpr *base,
					     const NetNet*reg, unsigned long&lwid);

extern NetExpr*normalize_variable_array_base(NetExpr *base, long offset,
                                             unsigned count);

/*
 * This function takes as input a NetNet signal and adds a constant
 * value to it. If the val is 0, then simply return sig. Otherwise,
 * return a new NetNet value that is the output of an addition.
 *
 * Not currently used.
 */
#if 0
extern NetNet*add_to_net(Design*des, NetNet*sig, long val);
#endif
extern NetNet*sub_net_from(Design*des, NetScope*scope, long val, NetNet*sig);

/*
 * Make a NetEConst object that contains only X bits.
 */
extern NetEConst*make_const_x(unsigned long wid);
extern NetEConst*make_const_0(unsigned long wid);
extern NetEConst*make_const_val(unsigned long val);

/*
 * Make A const net
 */
extern NetNet* make_const_x(Design*des, NetScope*scope, unsigned long wid);

/*
 * In some cases the lval is accessible as a pointer to the head of
 * a list of NetAssign_ objects. This function returns the width of
 * the l-value represented by this list.
 */
extern unsigned count_lval_width(const class NetAssign_*first);

/*
 * This function elaborates an expression, and tries to evaluate it
 * right away. If the expression can be evaluated, this returns a
 * constant expression. If it cannot be evaluated, it returns whatever
 * it can. If the expression cannot be elaborated, return 0.
 *
 * The context_width is the width of the context where the expression is
 * being elaborated, or -1 if the expression is self-determined, or -2
 * if the expression is lossless self-determined (this last option is
 * treated as standard self-determined if the gn_strict_expr_width flag
 * is set).
 */
class PExpr;

extern NetExpr* elab_and_eval(Design*des, NetScope*scope,
			      PExpr*pe, int context_width,
                              bool need_const =false);

/*
 * This function is a variant of elab_and_eval that elaborates and
 * evaluates the arguments of a system task.
 */
extern NetExpr* elab_sys_task_arg(Design*des, NetScope*scope,
                                  perm_string name, unsigned arg_idx,
                                  PExpr*pe, bool need_const =false);
/*
 * This function elaborates an expression as if it is for the r-value
 * of an assignment, The lv_type and lv_width are the type and width
 * of the l-value, and the expr is the expression to elaborate. The
 * result is the NetExpr elaborated and evaluated. (See elab_expr.cc)
 */
extern NetExpr* elaborate_rval_expr(Design*des, NetScope*scope,
				    ivl_variable_type_t lv_type,
				    unsigned lv_width, PExpr*expr,
                                    bool need_const =false);

/*
 * This procedure evaluates an expression and if the evaluation is
 * successful the original expression is replaced with the new one.
 */
void eval_expr(NetExpr*&expr, int context_width =-1);

/*
 * Get the long integer value for the passed in expression, if
 * possible. If it is not possible (the expression is not evaluated
 * down to a constant) then return false and leave value unchanged.
 */
bool eval_as_long(long&value, NetExpr*expr);
bool eval_as_double(double&value, NetExpr*expr);

/*
 * Evaluate the component of a scope path to get an hname_t value. Do
 * the evaluation in the context of the given scope.
 */
extern hname_t eval_path_component(Design*des, NetScope*scope,
				   const name_component_t&comp);

/*
 * Evaluate an entire scope path in the context of the given scope.
 */
extern std::list<hname_t> eval_scope_path(Design*des, NetScope*scope,
					  const pform_name_t&path);

/*
 * Return true if the data type is a type that is normally available
 * in vector for. IVL_VT_BOOL and IVL_VT_LOGIC are vectorable,
 * IVL_VT_REAL is not.
 */
extern bool type_is_vectorable(ivl_variable_type_t type);

/*
 * Return a human readable version of the operator.
 */
const char *human_readable_op(const char op, bool unary = false);

/*
 * Is the expression a constant value and if so what is its logical
 * value.
 *
 * C_NON - the expression is not a constant value.
 * C_0   - the expression is constant and it has a false value.
 * C_1   - the expression is constant and it has a true value.
 * C_X   - the expression is constant and it has an 'bX value.
 */
enum const_bool { C_NON, C_0, C_1, C_X };
const_bool const_logical(const NetExpr*expr);

extern bool dly_used_no_timescale;
extern bool dly_used_timescale;
extern bool display_ts_dly_warning;

/*
 * When scaling a real value to a time we need to do some standard
 * processing.
 */
extern uint64_t get_scaled_time_from_real(Design*des,
                                          NetScope*scope,
                                          NetECReal*val);

extern void collapse_partselect_pv_to_concat(Design*des, NetNet*sig);

extern bool evaluate_index_prefix(Design*des, NetScope*scope,
				  list<long>&prefix_indices,
				  const list<index_component_t>&indices);
#endif
