/*
 * Copyright (c) 2004 Stephen Williams (steve@icarus.com)
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
#ident "$Id: vvp_net.cc,v 1.17 2005/02/14 01:50:23 steve Exp $"

# include  "vvp_net.h"
# include  <stdio.h>
# include  <typeinfo>
# include  <assert.h>

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      if (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    return BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      int sum = (int)a + (int)b + (int)c;

      switch (sum) {
	  case 0:
	    return BIT4_0;
	  case 1:
	    c = BIT4_0;
	    return BIT4_1;
	  case 2:
	    c = BIT4_1;
	    return BIT4_0;
	  case 3:
	    c = BIT4_1;
	    return BIT4_1;
	  default:
	    assert(0);
      }
}

bool bit4_is_xz(vvp_bit4_t a)
{
      if (a == BIT4_X)
	    return true;
      if (a == BIT4_Z)
	    return true;
      return false;
}

vvp_bit4_t operator & (vvp_bit4_t a, vvp_bit4_t b)
{
      if (a == BIT4_0)
	    return BIT4_0;
      if (b == BIT4_0)
	    return BIT4_0;
      if (bit4_is_xz(a))
	    return BIT4_X;
      if (bit4_is_xz(b))
	    return BIT4_X;
      return BIT4_1;
}

vvp_bit4_t operator | (vvp_bit4_t a, vvp_bit4_t b)
{
      if (a == BIT4_1)
	    return BIT4_1;
      if (b == BIT4_1)
	    return BIT4_1;
      if (bit4_is_xz(a))
	    return BIT4_X;
      if (bit4_is_xz(b))
	    return BIT4_X;
      return BIT4_0;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      if (bit4_is_xz(a))
	    return BIT4_X;
      if (bit4_is_xz(b))
	    return BIT4_X;
      if (a == BIT4_0)
	    return b;
      if (b == BIT4_0)
	    return a;
      return BIT4_0;
}

vvp_bit4_t operator ~ (vvp_bit4_t a)
{
      switch (a) {
	  case BIT4_0:
	    return BIT4_1;
	  case BIT4_1:
	    return BIT4_0;
	  default:
	    return  BIT4_X;
      }
}

void vvp_send_vec4(vvp_net_ptr_t ptr, vvp_vector4_t val)
{
      while (struct vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next = cur->port[ptr.port()];

	    if (cur->fun)
		  cur->fun->recv_vec4(ptr, val);

	    ptr = next;
      }
}

void vvp_send_vec4_pv(vvp_net_ptr_t ptr, vvp_vector4_t val,
		      unsigned base, unsigned wid, unsigned vwid)
{
      while (struct vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next = cur->port[ptr.port()];

	    if (cur->fun)
		  cur->fun->recv_vec4_pv(ptr, val, base, wid, vwid);

	    ptr = next;
      }
}

void vvp_send_vec8(vvp_net_ptr_t ptr, vvp_vector8_t val)
{
      while (struct vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next = cur->port[ptr.port()];

	    if (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next;
      }
}

void vvp_send_real(vvp_net_ptr_t ptr, double val)
{
      while (struct vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next = cur->port[ptr.port()];

	    if (cur->fun)
		  cur->fun->recv_real(ptr, val);

	    ptr = next;
      }
}

void vvp_send_long(vvp_net_ptr_t ptr, long val)
{
      while (struct vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next = cur->port[ptr.port()];

	    if (cur->fun)
		  cur->fun->recv_long(ptr, val);

	    ptr = next;
      }
}

const unsigned bits_per_word = sizeof (unsigned long) / 2;

vvp_vector4_t::vvp_vector4_t(const vvp_vector4_t&that)
{
      size_ = that.size_;
      if (size_ > bits_per_word) {
	    unsigned words = (size_+bits_per_word-1) / bits_per_word;
	    bits_ptr_ = new unsigned long[words];

	    for (unsigned idx = 0 ;  idx < words ;  idx += 1)
		  bits_ptr_[idx] = that.bits_ptr_[idx];

      } else {
	    bits_val_ = that.bits_val_;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size)
: size_(size)
{
	/* Make a work full of initialized bits. */
      unsigned long initial_value_bits = 0xaa;
      for (unsigned idx = 0 ;  idx < sizeof (unsigned long) ;  idx += 1)
	    initial_value_bits = (initial_value_bits << 8) | 0xaa;

      if (size_ > bits_per_word) {
	    unsigned cnt = (size_ + bits_per_word - 1) / bits_per_word;
	    bits_ptr_ = new unsigned long[cnt];


	    for (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bits_ptr_[idx] = initial_value_bits;

      } else {
	    bits_val_ = initial_value_bits;
      }
}

vvp_vector4_t::~vvp_vector4_t()
{
      if (size_ > bits_per_word) {
	    delete[] bits_ptr_;
      }
}

vvp_vector4_t& vvp_vector4_t::operator= (const vvp_vector4_t&that)
{
      if (size_ > bits_per_word)
	    delete[] bits_ptr_;

      size_ = that.size_;
      if (size_ > bits_per_word) {
	    unsigned cnt = (size_ + bits_per_word - 1) / bits_per_word;
	    bits_ptr_ = new unsigned long[cnt];

	    for (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bits_ptr_[idx] = that.bits_ptr_[idx];

      } else {
	    bits_val_ = that.bits_val_;
      }

      return *this;
}

vvp_bit4_t vvp_vector4_t::value(unsigned idx) const
{
      if (idx >= size_)
	    return BIT4_X;

      unsigned wdx = idx / bits_per_word;
      unsigned off = idx % bits_per_word;

      unsigned long bits;
      if (size_ > bits_per_word) {
	    bits = bits_ptr_[wdx];
      } else {
	    bits = bits_val_;
      }

      bits >>= (off * 2);

	/* Casting is evil, but this cast matches the un-cast done
	   when the vvp_bit4_t value is put into the vector. */
      return (vvp_bit4_t) (bits & 3);
}

void vvp_vector4_t::set_bit(unsigned idx, vvp_bit4_t val)
{
      assert(idx < size_);

      unsigned wdx = idx / bits_per_word;
      unsigned off = idx % bits_per_word;
      unsigned long mask = 3UL << (2*off);

      if (size_ > bits_per_word) {
	    bits_ptr_[wdx] &= ~mask;
	    bits_ptr_[wdx] |= val << (2*off);
      } else {
	    bits_val_ &= ~mask;
	    bits_val_ |= val << (2*off);
      }
}

char* vvp_vector4_t::as_string(char*buf, size_t buf_len)
{
      char*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      for (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    switch (value(size()-idx-1)) {
		case BIT4_0:
		  *buf++ = '0';
		  break;
		case BIT4_1:
		  *buf++ = '1';
		  break;
		case BIT4_X:
		  *buf++ = 'x';
		  break;
		case BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      return res;
}

bool vector4_to_value(const vvp_vector4_t&vec, unsigned long&val)
{
      unsigned long res = 0;
      unsigned long msk = 1;

      for (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    switch (vec.value(idx)) {
		case BIT4_0:
		  break;
		case BIT4_1:
		  res |= msk;
		  break;
		default:
		  return false;
	    }

	    msk <<= 1UL;
      }

      val = res;
      return true;
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(unsigned long v, unsigned wid)
{
      wid_ = wid;
      const unsigned bits_per_word = 8 * sizeof(vec_[0]);
      const unsigned words = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = new unsigned long[words];
      for (unsigned idx = 0 ;  idx < words ;  idx += 1)
	    vec_[idx] = 0;
}

vvp_vector2_t::vvp_vector2_t(const vvp_vector4_t&that)
{
      wid_ = that.size();
      const unsigned bits_per_word = 8 * sizeof(vec_[0]);
      const unsigned words = (that.size() + bits_per_word-1) / bits_per_word;

      if (words == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    return;
      }

      vec_ = new unsigned long[words];
      for (unsigned idx = 0 ;  idx < words ;  idx += 1)
	    vec_[idx] = 0;

      for (unsigned idx = 0 ;  idx < that.size() ;  idx += 1) {
	    unsigned addr = idx / bits_per_word;
	    unsigned shift = idx % bits_per_word;

	    switch (that.value(idx)) {
		case BIT4_0:
		  break;
		case BIT4_1:
		  vec_[addr] |= 1 << shift;
		  break;
		default:
		  delete[]vec_;
		  vec_ = 0;
		  wid_ = 0;
		  return;
	    }
      }
}

vvp_vector2_t::~vvp_vector2_t()
{
      if (vec_) delete[]vec_;
}

unsigned vvp_vector2_t::size() const
{
      return wid_;
}

int vvp_vector2_t::value(unsigned idx) const
{
      if (idx >= wid_)
	    return 0;

      const unsigned bits_per_word = 8 * sizeof(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      if (vec_[addr] & (1UL<<mask))
	    return 1;
      else
	    return 0;
}

bool vvp_vector2_t::is_NaN() const
{
      return wid_ == 0;
}

/*
 * Multiplication of two vector2 vectors returns a product as wide as
 * the sum of the widths of the input vectors.
 */
vvp_vector2_t operator * (const vvp_vector2_t&a, const vvp_vector2_t&b)
{
      const unsigned bits_per_word = 8 * sizeof(a.vec_[0]);
      vvp_vector2_t r (0, a.size() + b.size());

      assert(sizeof(unsigned long long) >= 2*sizeof(a.vec_[0]));
      unsigned long long word_mask = (1ULL << bits_per_word) - 1ULL;

      unsigned awords = (a.wid_ + bits_per_word - 1) / bits_per_word;
      unsigned bwords = (b.wid_ + bits_per_word - 1) / bits_per_word;
      unsigned rwords = (r.wid_ + bits_per_word - 1) / bits_per_word;

      for (unsigned bdx = 0 ;  bdx < bwords ;  bdx += 1) {
	    unsigned long long tmpb = b.vec_[bdx];
	    if (tmpb == 0)
		  continue;

	    for (unsigned adx = 0 ;  adx < awords ;  adx += 1) {
		  unsigned long long tmpa = a.vec_[adx];
		  unsigned long long tmpr = tmpb * tmpa;

		  for (unsigned sdx = 0
			     ; (adx+bdx+sdx) < rwords && tmpr > 0
			     ;  sdx += 1) {
			unsigned long long sum = r.vec_[adx+bdx+sdx];
			sum += tmpr & word_mask;
			r.vec_[adx+bdx+sdx] = sum & word_mask;
			sum  >>= bits_per_word;
			tmpr >>= bits_per_word;
			tmpr += sum;
		  }
	    }
      }


      return r;
}

vvp_vector4_t vector2_to_vector4(const vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      for (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    if (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      return res;
}

vvp_vector8_t::vvp_vector8_t(const vvp_vector8_t&that)
{
      size_ = that.size_;

      bits_ = new vvp_scaler_t[size_];

      for (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
	    bits_[idx] = that.bits_[idx];

}

vvp_vector8_t::vvp_vector8_t(unsigned size)
: size_(size)
{
      if (size_ == 0) {
	    bits_ = 0;
	    return;
      }

      bits_ = new vvp_scaler_t[size_];
}

vvp_vector8_t::vvp_vector8_t(const vvp_vector4_t&that, unsigned str)
: size_(that.size())
{
      if (size_ == 0) {
	    bits_ = 0;
	    return;
      }

      bits_ = new vvp_scaler_t[size_];

      for (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
	    bits_[idx] = vvp_scaler_t (that.value(idx), str);

}

vvp_vector8_t::~vvp_vector8_t()
{
      if (size_ > 0)
	    delete[]bits_;
}

vvp_vector8_t& vvp_vector8_t::operator= (const vvp_vector8_t&that)
{
      if (size_ != that.size_) {
	    if (size_ > 0)
		  delete[]bits_;
	    size_ = 0;
      }

      if (that.size_ == 0) {
	    assert(size_ == 0);
	    return *this;
      }

      if (size_ == 0) {
	    size_ = that.size_;
	    bits_ = new vvp_scaler_t[size_];
      }

      for (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
	    bits_[idx] = that.bits_[idx];

      return *this;
}

vvp_scaler_t vvp_vector8_t::value(unsigned idx) const
{
      assert(idx < size_);
      return bits_[idx];
}

void vvp_vector8_t::set_bit(unsigned idx, vvp_scaler_t val)
{
      assert(idx < size_);
      bits_[idx] = val;
}

void vvp_vector8_t::dump(FILE*out)
{
      fprintf(out, "C8<");
      for (unsigned idx = 0 ;  idx < size() ;  idx += 1) {
	    vvp_scaler_t tmp = value(size()-idx-1);
	    tmp.dump(out);
      }

      fprintf(out,">");
}

vvp_net_fun_t::vvp_net_fun_t()
{
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

void vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, vvp_vector4_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*this).name());
      assert(0);
}

void vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, vvp_vector4_t,
				 unsigned, unsigned, unsigned)
{
      fprintf(stderr, "internal error: %s: recv_vec4_pv not implemented\n",
	      typeid(*this).name());
      assert(0);
}

void vvp_net_fun_t::recv_vec8(vvp_net_ptr_t, vvp_vector8_t)
{
      fprintf(stderr, "internal error: %s: recv_vec8 not implemented\n",
	      typeid(*this).name());
      assert(0);
}

void vvp_net_fun_t::recv_real(vvp_net_ptr_t, double)
{
      fprintf(stderr, "internal error: %s: recv_real not implemented\n",
	      typeid(*this).name());
      assert(0);
}

void vvp_net_fun_t::recv_long(vvp_net_ptr_t, long)
{
      fprintf(stderr, "internal error: %s: recv_long not implemented\n",
	      typeid(*this).name());
      assert(0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal::vvp_fun_signal(unsigned wid)
: bits4_(wid)
{
      vpi_callbacks = 0;
      continuous_assign_active_ = false;
      force_active_ = false;
}

bool vvp_fun_signal::type_is_vector8_() const
{
      return bits8_.size() > 0;
}

/*
 * Nets simply reflect their input to their output.
 */
void vvp_fun_signal::recv_vec4(vvp_net_ptr_t ptr, vvp_vector4_t bit)
{
      switch (ptr.port()) {
	  case 0: // Normal input (feed from net, or set from process)
	    if (! continuous_assign_active_) {
		  bits4_ = bit;
		  vvp_send_vec4(ptr.ptr()->out, bit);
		  run_vpi_callbacks();
	    }
	    break;

	  case 1: // Continuous assign value
	    continuous_assign_active_ = true;
	    if (type_is_vector8_()) {
		  bits8_ = vvp_vector8_t(bit,6);
		  vvp_send_vec8(ptr.ptr()->out, bits8_);
	    } else {
		  bits4_ = bit;
		  vvp_send_vec4(ptr.ptr()->out, bits4_);
	    }
	    run_vpi_callbacks();
	    break;

	  case 2: // Force value
	    force_active_ = true;
	    force_ = bit;
	    vvp_send_vec4(ptr.ptr()->out, force_);
	    run_vpi_callbacks();
	    break;

	  default:
	    assert(0);
	    break;
      }
}

void vvp_fun_signal::recv_vec4_pv(vvp_net_ptr_t ptr, vvp_vector4_t bit,
				  unsigned base, unsigned wid, unsigned vwid)
{
      assert(bit.size() == wid);
      assert(bits4_.size() == vwid);

      switch (ptr.port()) {
	  case 0: // Normal input
	    if (! continuous_assign_active_) {
		  for (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			if (base+idx >= bits4_.size())
			      break;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  vvp_send_vec4(ptr.ptr()->out, bits4_);
		  run_vpi_callbacks();
	    }
	    break;

	  default:
	    assert(0);
	    break;
      }
}

void vvp_fun_signal::recv_vec8(vvp_net_ptr_t ptr, vvp_vector8_t bit)
{
	// Only port-0 supports vector8_t inputs.
      assert(ptr.port() == 0);

      if (! continuous_assign_active_) {
	    bits8_ = bit;
	    vvp_send_vec8(ptr.ptr()->out, bit);
	    run_vpi_callbacks();
      }
}

void vvp_fun_signal::deassign()
{
      continuous_assign_active_ = false;
}

void vvp_fun_signal::release(vvp_net_ptr_t ptr, bool net)
{
      force_active_ = false;
      if (net) {
	    vvp_send_vec4(ptr.ptr()->out, bits4_);
	    run_vpi_callbacks();
      } else {
	    bits4_ = force_;
      }
}

/*
 * The signal functor takes commands as long values to port-3. This
 * method interprets those commands.
 */
void vvp_fun_signal::recv_long(vvp_net_ptr_t ptr, long bit)
{
      switch (ptr.port()) {
	  case 3: // Command port
	    switch (bit) {
		case 1: // deassign command
		  deassign();
		  break;
		case 2: // release/net
		  release(ptr, true);
		  break;
		case 3: // release/reg
		  release(ptr, false);
		  break;
		default:
		  assert(0);
		  break;
	    }
	    break;

	  default: // Other ports ar errors.
	    assert(0);
	    break;
      }
}

unsigned vvp_fun_signal::size() const
{
      if (force_active_)
	    return force_.size();
      else if (type_is_vector8_())
	    return bits8_.size();
      else
	    return bits4_.size();
}

vvp_bit4_t vvp_fun_signal::value(unsigned idx) const
{
      if (force_active_)
	    return force_.value(idx);
      else if (type_is_vector8_())
	    return bits8_.value(idx).value();
      else
	    return bits4_.value(idx);
}

/* **** vvp_scaler_t methods **** */

/*
 * DRIVE STRENGTHS:
 *
 * The normal functor is not aware of strengths. It generates strength
 * simply by virtue of having strength specifications. The drive
 * strength specification includes a drive0 and drive1 strength, each
 * with 8 possible values (that can be represented in 3 bits) as given
 * in this table:
 *
 *    HiZ    = 0,
 *    SMALL  = 1,
 *    MEDIUM = 2,
 *    WEAK   = 3,
 *    LARGE  = 4,
 *    PULL   = 5,
 *    STRONG = 6,
 *    SUPPLY = 7
 *
 * The vvp_scaler_t value, however, is a combination of value and
 * strength, used in strength-aware contexts.
 *
 * OUTPUT STRENGTHS:
 *
 * The strength-aware values are specified as an 8 bit value, that is
 * two 4 bit numbers. The value is encoded with two drive strengths (0-7)
 * and two drive values (0 or 1). Each nibble contains three bits of
 * strength and one bit of value, like so: VSSS. The high nibble has
 * the strength-value closest to supply1, and the low nibble has the
 * strength-value closest to supply0.
 */

/*
 * A signal value is unambiguous if the top 4 bits and the bottom 4
 * bits are identical. This means that the VSSSvsss bits of the 8bit
 * value have V==v and SSS==sss.
 */
# define UNAMBIG(v)  (((v) & 0x0f) == (((v) >> 4) & 0x0f))

#if 0
# define STREN1(v) ( ((v)&0x80)? ((v)&0xf0) : (0x70 - ((v)&0xf0)) )
# define STREN0(v) ( ((v)&0x08)? ((v)&0x0f) : (0x07 - ((v)&0x0f)) )
#else
# define STREN1(v) (((v)&0x70) >> 4)
# define STREN0(v) ((v)&0x07)
#endif

vvp_scaler_t::vvp_scaler_t(vvp_bit4_t val, unsigned str)
{
      assert(str <= 7);

      switch (val) {
	  case BIT4_0:
	    value_ = str | (str<<4);
	    break;
	  case BIT4_1:
	    value_ = str | (str<<4) | 0x88;
	    break;
	  case BIT4_X:
	    value_ = str | (str<<4) | 0x80;
	    break;
	  case BIT4_Z:
	    value_ = 0;
	    break;
      }
}

vvp_scaler_t::vvp_scaler_t(vvp_bit4_t val, unsigned str0, unsigned str1)
{
      assert(str0 <= 7);
      assert(str1 <= 7);

      switch (val) {
	  case BIT4_0:
	    value_ = str0 | (str0<<4);
	    break;
	  case BIT4_1:
	    value_ = str1 | (str1<<4) | 0x88;
	    break;
	  case BIT4_X:
	    value_ = str0 | (str1<<4) | 0x80;
	    break;
	  case BIT4_Z:
	    value_ = 0x00;
	    break;
      }
}

vvp_scaler_t::vvp_scaler_t()
{
      value_ = 0;
}

vvp_bit4_t vvp_scaler_t::value() const
{
      if (value_ == 0) {
	    return BIT4_Z;

      } else switch (value_ & 0x88) {
	  case 0x00:
	    return BIT4_0;
	  case 0x88:
	    return BIT4_1;
	  default:
	    return BIT4_X;
      }
}

void vvp_scaler_t::dump(FILE*out)
{
      fprintf(out, "%01u%01u", STREN0(value_), STREN1(value_));
      switch (value()) {
	  case BIT4_0:
	    fprintf(out, "0");
	    break;
	  case BIT4_1:
	    fprintf(out, "1");
	    break;
	  case BIT4_X:
	    fprintf(out, "x");
	    break;
	  case BIT4_Z:
	    fprintf(out, "z");
	    break;
      }
}

vvp_scaler_t resolve(vvp_scaler_t a, vvp_scaler_t b)
{
	// If the value is 0, that is the same as HiZ. In that case,
	// resolution is simply a matter of returning the *other* value.
      if (a.value_ == 0)
	    return b;
      if (b.value_ == 0)
	    return a;

      vvp_scaler_t res = a;

      if (UNAMBIG(a.value_) && UNAMBIG(b.value_)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    if (a.value_ == b.value_) {

		    /* values are equal. do nothing. */

	    } else if ((b.value_&0x07) > (res.value_&0x07)) {

		    /* New value is stronger. Take it. */
		  res.value_ = b.value_;

	    } else if ((b.value_&0x77) == (res.value_&0x77)) {

		    /* Strengths are the same. Make value ambiguous. */
		  res.value_ = (res.value_&0x70) | (b.value_&0x07) | 0x80;

	    } else {

		    /* Must be res is the stronger one. */
	    }

      } else if (UNAMBIG(res.value_)) {
	    unsigned tmp = 0;

	    if ((res.value_&0x70) > (b.value_&0x70))
		  tmp |= res.value_&0xf0;
	    else
		  tmp |= b.value_&0xf0;

	    if ((res.value_&0x07) > (b.value_&0x07))
		  tmp |= res.value_&0x0f;
	    else
		  tmp |= b.value_&0x0f;

	    res.value_ = tmp;

      } else if (UNAMBIG(b.value_)) {

	      /* If one of the signals is unambiguous, then it
		 will sweep up the weaker parts of the ambiguous
		 signal. The result may be ambiguous, or maybe not. */

	    unsigned tmp = 0;

	    if ((b.value_&0x70) > (res.value_&0x70))
		  tmp |= b.value_&0xf0;
	    else
		  tmp |= res.value_&0xf0;

	    if ((b.value_&0x07) > (res.value_&0x07))
		  tmp |= b.value_&0x0f;
	    else
		  tmp |= res.value_&0x0f;

	    res.value_ = tmp;

      } else {

	      /* If both signals are ambiguous, then the result
		 has an even wider ambiguity. */

	    unsigned tmp = 0;

	    if (STREN1(b.value_) > STREN1(res.value_))
		  tmp |= b.value_&0xf0;
	    else
		  tmp |= res.value_&0xf0;

	    if (STREN0(b.value_) < STREN0(res.value_))
		  tmp |= b.value_&0x0f;
	    else
		  tmp |= res.value_&0x0f;

	    res.value_ = tmp;
      }


	/* Canonicalize the HiZ value. */
      if ((res.value_&0x77) == 0)
	    res.value_ = 0;

      return res;
}

vvp_vector8_t resolve(const vvp_vector8_t&a, const vvp_vector8_t&b)
{
      assert(a.size() == b.size());

      vvp_vector8_t out (a.size());

      for (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    out.set_bit(idx, resolve(a.value(idx), b.value(idx)));
      }

      return out;
}

vvp_vector4_t reduce4(const vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      for (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      return out;
}

vvp_bit4_t compare_gtge(const vvp_vector4_t&lef, const vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      if (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      if (min_size == 0)
	    return BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      for (unsigned idx = 0 ;  idx < lef.size() ;  idx += 1) {
	    vvp_bit4_t bit = lef.value(idx);
	    if (bit == BIT4_X)
		  return BIT4_X;
	    if (bit == BIT4_Z)
		  return BIT4_X;
      }

	// Check for X/Z in the right operand
      for (unsigned idx = 0 ;  idx < rig.size() ;  idx += 1) {
	    vvp_bit4_t bit = rig.value(idx);
	    if (bit == BIT4_X)
		  return BIT4_X;
	    if (bit == BIT4_Z)
		  return BIT4_X;
      }

      for (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    if (lef.value(idx-1) == BIT4_1)
		  return BIT4_1;
      }

      for (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    if (rig.value(idx-1) == BIT4_1)
		  return BIT4_0;
      }

      for (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    if (lv == rv)
		  continue;

	    if (lv == BIT4_1)
		  return BIT4_1;
	    else
		  return BIT4_0;
      }

      return out_if_equal;
}

vvp_vector4_t operator ~ (const vvp_vector4_t&that)
{
      vvp_vector4_t res (that.size());
      for (unsigned idx = 0 ;  idx < res.size() ;  idx += 1)
	    res.set_bit(idx, ~ that.value(idx));

      return res;
}

vvp_bit4_t compare_gtge_signed(const vvp_vector4_t&a,
			       const vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      assert(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      if (a_sign == BIT4_X)
	    return BIT4_X;
      if (a_sign == BIT4_Z)
	    return BIT4_X;
      if (b_sign == BIT4_X)
	    return BIT4_X;
      if (b_sign == BIT4_Z)
	    return BIT4_Z;

      if (a_sign == b_sign)
	    return compare_gtge(a, b, out_if_equal);

      for (unsigned idx = 0 ;  idx < sign_idx ;  idx += 1) {
	    vvp_bit4_t a_bit = a.value(idx);
	    vvp_bit4_t b_bit = a.value(idx);

	    if (a_bit == BIT4_X)
		  return BIT4_X;
	    if (a_bit == BIT4_Z)
		  return BIT4_X;
	    if (b_bit == BIT4_X)
		  return BIT4_X;
	    if (b_bit == BIT4_Z)
		  return BIT4_Z;
      }

      if(a_sign == BIT4_0)
	    return BIT4_1;
      else
	    return BIT4_0;
}

/*
 * $Log: vvp_net.cc,v $
 * Revision 1.17  2005/02/14 01:50:23  steve
 *  Signals may receive part vectors from %set/x0
 *  instructions. Re-implement the %set/x0 to do
 *  just that. Remove the useless %set/x0/x instruction.
 *
 * Revision 1.16  2005/02/12 06:13:22  steve
 *  Add debug dumps for vectors, and fix vvp_scaler_t make from BIT4_X values.
 *
 * Revision 1.15  2005/02/10 04:54:41  steve
 *  Simplify vvp_scaler strength representation.
 *
 * Revision 1.14  2005/02/07 22:42:42  steve
 *  Add .repeat functor and BIFIF functors.
 *
 * Revision 1.13  2005/02/04 05:13:02  steve
 *  Add wide .arith/mult, and vvp_vector2_t vectors.
 *
 * Revision 1.12  2005/02/03 04:55:13  steve
 *  Add support for reduction logic gates.
 *
 * Revision 1.11  2005/01/30 05:06:49  steve
 *  Get .arith/sub working.
 *
 * Revision 1.10  2005/01/29 17:52:06  steve
 *  move AND to buitin instead of table.
 *
 * Revision 1.9  2005/01/28 05:34:25  steve
 *  Add vector4 implementation of .arith/mult.
 *
 * Revision 1.8  2005/01/22 17:36:15  steve
 *  .cmp/x supports signed magnitude compare.
 *
 * Revision 1.7  2005/01/22 00:58:22  steve
 *  Implement the %load/x instruction.
 *
 * Revision 1.6  2005/01/16 04:19:08  steve
 *  Reimplement comparators as vvp_vector4_t nodes.
 *
 * Revision 1.5  2005/01/09 20:11:16  steve
 *  Add the .part/pv node and related functionality.
 *
 * Revision 1.4  2005/01/01 02:12:34  steve
 *  vvp_fun_signal propagates vvp_vector8_t vectors when appropriate.
 *
 * Revision 1.3  2004/12/31 06:00:06  steve
 *  Implement .resolv functors, and stub signals recv_vec8 method.
 *
 * Revision 1.2  2004/12/15 17:16:08  steve
 *  Add basic force/release capabilities.
 *
 * Revision 1.1  2004/12/11 02:31:30  steve
 *  Rework of internals to carry vectors through nexus instead
 *  of single bits. Make the ivl, tgt-vvp and vvp initial changes
 *  down this path.
 *
 */
