/*
 * Copyright (c) 2002-2011 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "netlist.h"
# include  "netenum.h"
# include  "compiler.h"
# include  "netmisc.h"
# include  <iostream>

/*
 * the grand default data type is a logic vector.
 */
ivl_variable_type_t NetExpr::expr_type() const
{
      return IVL_VT_LOGIC;
}

netenum_t*NetExpr::enumeration() const
{
      return 0;
}

/*
 * Create an add/sub node from the two operands.
 */
NetEBAdd::NetEBAdd(char op__, NetExpr*l, NetExpr*r, unsigned wid, bool signed_flag)
: NetEBinary(op__, l, r, wid, signed_flag)
{
}

NetEBAdd::~NetEBAdd()
{
}

ivl_variable_type_t NetEBAdd::expr_type() const
{
      if (left_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      if (right_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      return IVL_VT_LOGIC;
}

/*
 * Create a comparison operator with two sub-expressions.
 */
NetEBComp::NetEBComp(char op__, NetExpr*l, NetExpr*r)
: NetEBinary(op__, l, r, 1, false)
{
}

NetEBComp::~NetEBComp()
{
}

bool NetEBComp::has_width() const
{
      return true;
}

ivl_variable_type_t NetEBComp::expr_type() const
{
	// Case compare always returns BOOL
      if (op() == 'E' || op() == 'N')
	    return IVL_VT_BOOL;

      if (left()->expr_type() == IVL_VT_LOGIC)
	    return IVL_VT_LOGIC;

      if (right()->expr_type() == IVL_VT_LOGIC)
	    return IVL_VT_LOGIC;

      return IVL_VT_BOOL;
}

NetEBDiv::NetEBDiv(char op__, NetExpr*l, NetExpr*r, unsigned wid, bool signed_flag)
: NetEBinary(op__, l, r, wid, signed_flag)
{
}

NetEBDiv::~NetEBDiv()
{
}

ivl_variable_type_t NetEBDiv::expr_type() const
{
      if (left_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      if (right_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      return IVL_VT_LOGIC;
}

NetEBMinMax::NetEBMinMax(char op__, NetExpr*l, NetExpr*r, unsigned wid, bool signed_flag)
: NetEBinary(op__, l, r, wid, signed_flag)
{
}

NetEBMinMax::~NetEBMinMax()
{
}

ivl_variable_type_t NetEBMinMax::expr_type() const
{
      if (left_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;
      if (right_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      return IVL_VT_LOGIC;
}

NetEBMult::NetEBMult(char op__, NetExpr*l, NetExpr*r, unsigned wid, bool signed_flag)
: NetEBinary(op__, l, r, wid, signed_flag)
{
}

NetEBMult::~NetEBMult()
{
}

ivl_variable_type_t NetEBMult::expr_type() const
{
      if (left_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      if (right_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      return IVL_VT_LOGIC;
}

NetEBPow::NetEBPow(char op__, NetExpr*l, NetExpr*r, unsigned wid, bool signed_flag)
: NetEBinary(op__, l, r, wid, signed_flag)
{
}

NetEBPow::~NetEBPow()
{
}

ivl_variable_type_t NetEBPow::expr_type() const
{
      if (right_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;
      if (left_->expr_type() == IVL_VT_REAL)
	    return IVL_VT_REAL;

      return IVL_VT_LOGIC;
}

NetEBShift::NetEBShift(char op__, NetExpr*l, NetExpr*r, unsigned wid, bool signed_flag)
: NetEBinary(op__, l, r, wid, signed_flag)
{
}

NetEBShift::~NetEBShift()
{
}

bool NetEBShift::has_width() const
{
      return left_->has_width();
}

NetEConcat::NetEConcat(unsigned cnt, unsigned r)
: parms_(cnt), repeat_(r)
{
      expr_width(0);
}

NetEConcat::~NetEConcat()
{
      for (unsigned idx = 0 ;  idx < parms_.count() ;  idx += 1)
	    delete parms_[idx];
}

bool NetEConcat::has_width() const
{
      return true;
}

void NetEConcat::set(unsigned idx, NetExpr*e)
{
      assert(idx < parms_.count());
      assert(parms_[idx] == 0);
      parms_[idx] = e;
      expr_width( expr_width() + repeat_ * e->expr_width() );
}

NetEConstEnum::NetEConstEnum(NetScope*s, perm_string n, netenum_t*eset, const verinum&v)
: NetEConst(v), scope_(s), enum_set_(eset), name_(n)
{
      assert(has_width());
}

NetEConstEnum::~NetEConstEnum()
{
}

netenum_t*NetEConstEnum::enumeration() const
{
      return enum_set_;
}

NetECReal::NetECReal(const verireal&val)
: value_(val)
{
      expr_width(1);
      cast_signed_base_(true);
}

NetECReal::~NetECReal()
{
}

const verireal& NetECReal::value() const
{
      return value_;
}

bool NetECReal::has_width() const
{
      return true;
}

ivl_variable_type_t NetECReal::expr_type() const
{
      return IVL_VT_REAL;
}

NetECRealParam::NetECRealParam(NetScope*s, perm_string n, const verireal&v)
: NetECReal(v), scope_(s), name_(n)
{
}

NetECRealParam::~NetECRealParam()
{
}

perm_string NetECRealParam::name() const
{
      return name_;
}

const NetScope* NetECRealParam::scope() const
{
      return scope_;
}


NetENetenum::NetENetenum(netenum_t*s)
: netenum_(s)
{
}

NetENetenum::~NetENetenum()
{
}

netenum_t* NetENetenum::netenum() const
{
      return netenum_;
}

NetESelect::NetESelect(NetExpr*exp, NetExpr*base, unsigned wid,
                       ivl_select_type_t sel_type)
: expr_(exp), base_(base), sel_type_(sel_type)
{
      expr_width(wid);
}

NetESelect::~NetESelect()
{
      delete expr_;
      delete base_;
}

const NetExpr*NetESelect::sub_expr() const
{
      return expr_;
}

const NetExpr*NetESelect::select() const
{
      return base_;
}

ivl_select_type_t NetESelect::select_type() const
{
      return sel_type_;
}

bool NetESelect::has_width() const
{
      return true;
}

NetESFunc::NetESFunc(const char*n, ivl_variable_type_t t,
		     unsigned width, unsigned np)
: name_(0), type_(t), enum_type_(0), parms_(np)
{
      name_ = lex_strings.add(n);
      expr_width(width);
}

NetESFunc::NetESFunc(const char*n, netenum_t*enum_type, unsigned np)
: name_(0), type_(enum_type->base_type()), enum_type_(enum_type), parms_(np)
{
      name_ = lex_strings.add(n);
      expr_width(enum_type->base_width());
}

NetESFunc::~NetESFunc()
{
      for (unsigned idx = 0 ;  idx < parms_.size() ;  idx += 1)
	    if (parms_[idx]) delete parms_[idx];

	/* name_ string ls lex_strings allocated. */
}

const char* NetESFunc::name() const
{
      return name_;
}

unsigned NetESFunc::nparms() const
{
      return parms_.size();
}

void NetESFunc::parm(unsigned idx, NetExpr*v)
{
      assert(idx < parms_.size());
      if (parms_[idx])
	    delete parms_[idx];
      parms_[idx] = v;
}

const NetExpr* NetESFunc::parm(unsigned idx) const
{
      assert(idx < parms_.size());
      return parms_[idx];
}

NetExpr* NetESFunc::parm(unsigned idx)
{
      assert(idx < parms_.size());
      return parms_[idx];
}

ivl_variable_type_t NetESFunc::expr_type() const
{
      return type_;
}

netenum_t* NetESFunc::enumeration() const
{
      return enum_type_;
}

NetEAccess::NetEAccess(NetBranch*br, ivl_nature_t nat)
: branch_(br), nature_(nat)
{
}

NetEAccess::~NetEAccess()
{
}

ivl_variable_type_t NetEAccess::expr_type() const
{
      return IVL_VT_REAL;
}
