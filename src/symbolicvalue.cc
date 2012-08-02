/*
    Copyrhs 2012 Alex Eubanks (endeavor[at]rainbowsandpwnies.com)

    This file is part of rnp_see ( http://github.com/endeav0r/rnp_see/ )

    rnp_see is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "symbolicvalue.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <z3++.h>

SymbolicValue :: SymbolicValue ()
{
    lhs   = NULL;
    rhs   = NULL;
    type  = SVT_NONE;
    wild  = false;
    value = UInt(0);
    ssa   = SymbolicValueSSA::get().next();
}

SymbolicValue :: SymbolicValue (int bits, uint64_t value64)
{
    lhs   = NULL;
    rhs   = NULL;
    type  = SVT_CONSTANT;
    wild  = false;
    value = UInt(bits, value64);
    ssa   = SymbolicValueSSA::get().next();
}

SymbolicValue :: SymbolicValue (int bits)
{
    lhs   = NULL;
    rhs   = NULL;
    type  = SVT_CONSTANT;
    wild  = true;
    value = UInt(bits);
    ssa   = SymbolicValueSSA::get().next();
}

SymbolicValue :: SymbolicValue (const UInt & value)
{
    lhs   = NULL;
    rhs   = NULL;
    type  = SVT_CONSTANT;
    wild  = false;
    this->value = value;
    ssa   = SymbolicValueSSA::get().next();
}

SymbolicValue :: SymbolicValue (int type,
                                const SymbolicValue & lhss,
                                const SymbolicValue & rhss)
{
    this->type  = type;
    wild  = true;
    value = UInt(lhss.g_bits());
    ssa   = SymbolicValueSSA::get().next();
    this->lhs = new SymbolicValue();
    this->rhs = new SymbolicValue();
    *(this->lhs) = lhss;
    *(this->rhs) = rhss;
}

SymbolicValue :: ~SymbolicValue ()
{
    //std::cerr << "destructor for " << str() << std::endl;
    //if (lhs != NULL)
        //delete lhs;
    //if (rhs != NULL)
        //delete rhs;
}

SymbolicValue & SymbolicValue :: operator = (const SymbolicValue & rhs)
{
    if (this == &rhs) {
        std::cerr << "duplicate SymbolicValue" << std::endl;
        return *this;
    }

    if (this->lhs)
        delete this->lhs;
    if (this->rhs)
        delete this->rhs;

    type  = rhs.type;
    wild  = rhs.wild;
    value = rhs.value;
    ssa   = rhs.ssa;
    if (rhs.lhs) {
        this->lhs = new SymbolicValue();
        this->rhs = new SymbolicValue();
        *(this->lhs) = (const SymbolicValue) *(rhs.lhs);
        *(this->rhs) = (const SymbolicValue) *(rhs.rhs);
    }
    else {
        this->lhs = NULL;
        this->rhs = NULL;
    }
    
    return *this;
}
/*
SymbolicValue & SymbolicValue :: operator = (SymbolicValue & rhs)
{
    *this = (const SymbolicValue) rhs;
    return *this;
}
*/
const std::string SymbolicValue :: str () const
{
    std::stringstream ss;

    if (type == SVT_NONE)
        ss << "(NONE)";
    else if (type == SVT_CONSTANT) {
        ss << "(" << g_bits() << " {" << ssa << "} ";
        if (wild) ss << "wild)";
        else ss << value.str() << ")";
    }
    else if (type == SVT_NOT) {
        ss << "~(" << lhs->str() << ")";
    }
    else {
        ss << "(" << g_bits() << " " << lhs->str();
        switch (type) {
        case SVT_ADD    : ss << " + "; break;
        case SVT_AND    : ss << " & "; break;
        case SVT_CMPLES : ss << " <=S "; break;
        case SVT_CMPLEU : ss << " <=U "; break;
        case SVT_CMPLTS : ss << " <S "; break;
        case SVT_CMPLTU : ss << " <U "; break;
        case SVT_DIV    : ss << " / "; break;
        case SVT_EQ     : ss << " == "; break;
        case SVT_MOD    : ss << " % "; break;
        case SVT_MUL    : ss << " * "; break;
        case SVT_OR     : ss << " | "; break;
        case SVT_SEXT   : ss << " sext "; break;
        case SVT_SHL    : ss << " << "; break;
        case SVT_SHR    : ss << " >> "; break;
        case SVT_SUB    : ss << " - "; break;
        case SVT_XOR    : ss << " ^ "; break;
        default :
            std::stringstream ss;

            ss << "invalid type for SymbolicValue::str() => " << type;
            throw std::runtime_error(ss.str());
        }
        ss << rhs->str() << ")";
    }
    
    return ss.str();
}

const SymbolicValue SymbolicValue :: extend (int bits) const {
    SymbolicValue result = *this;
    result.ssa = SymbolicValueSSA::get().next();
    result.value = result.value.extend(bits);
    return result;
}

const SymbolicValue SymbolicValue :: signExtend (int bits) const
{
    SymbolicValue result = *this;
    result.ssa = SymbolicValueSSA::get().next();
    result.value = result.value.sign_extend(bits);
    return result;
}


SymbolicValue SymbolicValue :: operator~ () const
{
    if (not wild) return SymbolicValue(~value);
    else return SymbolicValue(SVT_NOT, *this, SymbolicValue());
}

#define SVOPERATOR(OPER, ENUM) \
SymbolicValue SymbolicValue :: operator OPER (const SymbolicValue & rhs) const \
{                                                                                 \
    if ((not this->wild) && (not rhs.wild))                                       \
        return SymbolicValue(this->g_value() OPER rhs.g_value());                 \
    else                                                                          \
        return SymbolicValue(ENUM, *this, rhs);                                   \
}

SVOPERATOR(+,  SVT_ADD)
SVOPERATOR(&,  SVT_AND)
SVOPERATOR(/,  SVT_DIV)
SVOPERATOR(%,  SVT_MOD)
SVOPERATOR(*,  SVT_MUL)
SVOPERATOR(|,  SVT_OR)
SVOPERATOR(<<, SVT_SHL)
SVOPERATOR(>>, SVT_SHR)
SVOPERATOR(-,  SVT_SUB)
SVOPERATOR(^,  SVT_XOR)


SymbolicValue SymbolicValue :: operator == (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (this->g_value() == rhs.g_value())
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValue(SVT_EQ, *this, rhs);                                                    \
}

SymbolicValue SymbolicValue :: cmpLes (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (value.cmpLes(rhs.g_value()))
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValue(SVT_CMPLES, *this, rhs);
}

SymbolicValue SymbolicValue :: cmpLeu (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (value <= rhs.value)
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValue(SVT_CMPLEU, *this, rhs);
}

SymbolicValue SymbolicValue :: cmpLts (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (value.cmpLts(rhs.g_value()))
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValue(SVT_CMPLTS, *this, rhs);
}

SymbolicValue SymbolicValue :: cmpLtu (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (value < rhs.value)
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValue(SVT_CMPLTU, *this, rhs);
}


/***********
* z3 logic *
***********/

// missing functions from z3 c++ headers
// where's the love

z3::expr z3mod (z3::expr const & a, z3::expr const & b)
{
    return to_expr(a.ctx(), Z3_mk_bvurem(a.ctx(), a, b));
}

z3::expr z3sext (z3::expr const & a, int bits)
{
    return to_expr(a.ctx(), Z3_mk_sign_ext(a.ctx(), bits, a));
}

z3::expr z3shl (z3::expr const & a, z3::expr const & b)
{
    return to_expr(a.ctx(), Z3_mk_bvshl(a.ctx(), a, b));
}

z3::expr z3shl (z3::expr const & a, int b)
{
    return z3shl(a, a.ctx().num_val(b, a.get_sort()));
}

z3::expr z3shr (z3::expr const & a, z3::expr const & b)
{
    return to_expr(a.ctx(), Z3_mk_bvlshr(a.ctx(), a, b));
}

z3::expr z3shr (z3::expr const & a, int b)
{
    return z3shr(a, a.ctx().num_val(b, a.get_sort()));
}

std::string SymbolicValue :: z3_name () const
{
    std::stringstream ss;
    ss << "symval_" << ssa;
    return ss.str();
}

z3::expr SymbolicValue :: contextCmp (z3::context & c, z3::expr && cond) const
{
    z3::solver s(c);

    s.add(cond);

    if (s.check() == z3::sat) return c.bv_val(1, g_bits());
    else return c.bv_val(0, g_bits());
}

z3::expr SymbolicValue :: extend (z3::expr expr, int target_size) const
{
    int expr_size = expr.get_sort().bv_size();

    if (expr_size < target_size) {
        return to_expr(expr.ctx(), Z3_mk_zero_ext(expr.ctx(), target_size - expr_size, expr));
    }
    else if (expr_size > target_size) {

    }
    return expr;
}

z3::expr SymbolicValue :: context (z3::context & c) const
{
    switch (type) {
    case SVT_ADD    :
        return extend(lhs->context(c) + rhs->context(c), g_bits());
    case SVT_AND    :
        return extend(lhs->context(c) & rhs->context(c), g_bits());
    case SVT_CMPLES :
        return extend(contextCmp(c, lhs->context(c) <= rhs->context(c)), g_bits());
    case SVT_CMPLEU :
        return extend(contextCmp(c, ule(lhs->context(c), rhs->context(c))), g_bits());
    case SVT_CMPLTS :
        return extend(contextCmp(c, lhs->context(c) < rhs->context(c)), g_bits());
    case SVT_CMPLTU :
        return extend(contextCmp(c, ult(lhs->context(c), rhs->context(c))), g_bits());
    case SVT_DIV    :
        return extend(lhs->context(c) / rhs->context(c), g_bits());
    case SVT_EQ     :
        return extend(contextCmp(c, lhs->context(c) == rhs->context(c)), g_bits());
    case SVT_MOD    :
        return extend(z3mod(lhs->context(c), rhs->context(c)), g_bits());
    case SVT_MUL    :
        return extend(lhs->context(c) * rhs->context(c), g_bits());
    case SVT_NOT    :
        return extend(~(lhs->context(c)), g_bits());
    case SVT_OR     :
        return extend(lhs->context(c) | rhs->context(c), g_bits());
    case SVT_SEXT   :
        if (rhs->g_wild())
            throw std::runtime_error("tried to sign-extend by wild bits");
        return z3sext(lhs->context(c), rhs->g_uint64());
    case SVT_SHL    :
        return extend(z3shl(lhs->context(c), rhs->context(c)), g_bits());
    case SVT_SHR    :
        return extend(z3shr(lhs->context(c), rhs->context(c)), g_bits());
    case SVT_SUB    :
        return extend(lhs->context(c) - rhs->context(c), g_bits());
    case SVT_XOR    :
        return extend(lhs->context(c) | rhs->context(c), g_bits());
    }

    if (not wild) {
        if (g_bits() <= 64) {
            return c.bv_val((__uint64) g_uint64(), g_bits());
        }
        else if (g_bits() == 128) {
            z3::expr lower64 = c.bv_val((__uint64) g_uint64(), 64);
            z3::expr upper64 = c.bv_val((__uint64) (value >> UInt(8, 64)).g_value64(), 64);
            return (z3shl(upper64, 64) | lower64).simplify();
        }
        else
            throw std::runtime_error("invalid bits for SymbolicValue::context");
    }
    return c.bv_const(z3_name().c_str(), g_bits());
}


bool SymbolicValue :: sv_assert (const SymbolicValue && value) const {
    z3::context c;

    z3::expr this_value   = context(c);
    z3::expr target_value = value.context(c);

    z3::solver s(c);
    s.add(this_value == target_value);

    if (s.check() == z3::sat) return true;
    return false;
}

// drunk coding leads to regrets, but FOSS so what the hell
bool SymbolicValue :: sv_assert (const SymbolicValue && value,
                                std::list <std::pair <SymbolicValue, SymbolicValue>> assertions)
const
{
    z3::context c;

    z3::expr this_value   = context(c);
    z3::expr target_value = value.context(c);

    z3::solver s(c);

    std::list <std::pair <SymbolicValue, SymbolicValue>> :: iterator it;
    for (it = assertions.begin(); it != assertions.end(); it++) {
        s.add(it->first.context(c) == it->second.context(c));
    }

    s.add(this_value == target_value);

    if (s.check() == z3::sat) return true;
    else return false;
}