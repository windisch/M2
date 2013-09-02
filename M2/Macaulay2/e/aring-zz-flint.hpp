// Copyright 2013 Michael E. Stillman

#ifndef _aring_zz_flint_hpp_
#define _aring_zz_flint_hpp_

#include "aring.hpp"
#include "buffer.hpp"
#include "ringelem.hpp"
#include <iosfwd>
#include "exceptions.hpp"

#if not defined(HAVE_FLINT)  
namespace M2 {
  class ARingZZ : public DummyRing
  {
  public:
    static const RingID ringID = ring_ZZFlint;
    typedef M2::ARingZZ ring_type ;
    
    ARingZZ() {};
  };
};
#else

#include <flint/arith.h>

namespace M2 {
  /**
     @ingroup rings
     
     @brief wrapper for the flint fmpz_t integer representation
  */
  
  class ARingZZ : public RingInterface
  {
  public:
    static const RingID ringID = ring_ZZFlint;
    
    typedef fmpz ElementType;
    typedef ElementType elem;

    ARingZZ();
    ~ARingZZ();
  public:
    // ring informational
    size_t characteristic() const { return 0; }
    
    size_t cardinality() const { return static_cast<size_t>(-1); }

    unsigned long computeHashValue(const ElementType& a) const 
    { 
      return fmpz_get_ui(&a);
    }
    
    /** @name properties
        @{     
    */
    
    bool is_unit(const ElementType& f) const {return fmpz_is_one(&f) or fmpz_cmp_si(&f,-1) == 0;}
    bool is_zero(const ElementType& f) const {return fmpz_is_zero(&f);}
    
    /** @} */
    
    
    /** @name operators
        @{ */
    
    bool is_equal(const ElementType& f,const ElementType& g) const {return fmpz_equal(&f,&g);}
    int compare_elems(const ElementType& f,const ElementType& g) const {return fmpz_cmp(&f,&g);}
    /** @} */
    
    /** @name init_set
        @{ */
    
    void init_set(ElementType& result, const ElementType& a) const {fmpz_init_set(&result, &a);}

    void init(ElementType& result) const {fmpz_init(&result);}

    void clear(ElementType& result) const {fmpz_clear(&result);}
    
    void set(ElementType& result, const ElementType& a) const {fmpz_set(&result, &a);}
    
    void set_zero(ElementType& result) const {fmpz_set_si(&result, 0);}
    
    void set_from_int(ElementType& result, int a) const {fmpz_set_si(&result, a);}
    
    void set_from_mpz(ElementType& result,const mpz_ptr a) const {
      //printf("ARingZZ::calling set_from_mpz\n");
      fmpz_set_mpz(&result, a);
    }
    
    void set_from_mpq(ElementType& result,const mpq_ptr a) const {M2_ASSERT(false);}
    
    bool set_from_BigReal(ElementType& result, gmp_RR a) const {return false;}
    
    void set_var(ElementType& result, int v) const {fmpz_set_si(&result,1);}
    
    /** @} */
    
    
    /** @name arithmetic
        @{ */
    void negate(ElementType& result,const ElementType& a) const {fmpz_neg(&result,&a);}
    
    void invert(ElementType& result,const ElementType& a) const {if (is_unit(a)) set(result,a); else set_zero(result);}
    
    void add(ElementType& result, const ElementType& a,const ElementType& b) const {fmpz_add(&result,&a,&b);}
    
    void subtract(ElementType& result,const  ElementType& a,const  ElementType& b) const {fmpz_sub(&result,&a,&b);}
    
    void subtract_multiple(ElementType& result,const  ElementType& a,const  ElementType& b) const {fmpz_submul(&result,&a,&b);}
    
    void mult(ElementType& result,const  ElementType& a,const  ElementType& b) const {fmpz_mul(&result,&a,&b);}
    
    ///@brief test doc
    bool divide(ElementType& result,const  ElementType& a,const  ElementType& b) const {
      if (fmpz_divisible(&a,&b)) {
        fmpz_divexact(&result,&a,&b); 
        return true;
      } else
        return false;
    }
    
    void power(ElementType& result,const  ElementType& a,const unsigned long  n) const {
      M2_ASSERT(n >= 0);
      return fmpz_pow_ui(&result,&a,n);
    }
    
    void power_mpz(ElementType& result,const  ElementType& a,const  mpz_ptr n) const {
      if (mpz_fits_ulong_p(n))
        fmpz_pow_ui(&result,&a,mpz_get_ui(n));
      else
        throw exc::engine_error("attempted to take a power of an integer to too large of a power");
    }
    
    void syzygy(const ElementType& a, const ElementType& b,
                ElementType& x, ElementType& y) const;
    /** @} */
    
    
    /** @name misc
        @{ */
    void swap(ElementType& a, ElementType& b) const {fmpz_swap(&a,&b);}
    
    void random(ElementType& result) const {fmpz_randm(&result, mRandomState, mMaxHeight);}
    /** @} */


    /** @name IO
        @{ 
    */
    void text_out(buffer &o) const { o << "ZZFlint"; }
    
    void elem_text_out(buffer &o, 
                       const ElementType& a,
                       bool p_one=true, 
                       bool p_plus=false, 
                       bool p_parens=false) const;
    /** @} */

    /** @name translation functions
        @{ */
    
    void to_ring_elem(ring_elem &result, const ElementType& a) const
    {
      fmpz b;
      fmpz_init_set(&b,&a);
      result.poly_val = reinterpret_cast<Nterm*>(b);
    }
    
    void from_ring_elem(ElementType& result, const ring_elem &a) const
    {
      fmpz t = reinterpret_cast<fmpz>(const_cast<Nterm*>(a.poly_val));
      fmpz_set(&result, &t);
    }
    
    /** @} */
    
    bool promote(const Ring *Rf, const ring_elem f, ElementType& result) const {return false;}
    
    bool lift(const Ring *Rg, const ElementType& f, ring_elem &result) const {return false;}
    
    // map : this --> target(map)
    //       primelem --> map->elem(first_var)
    // evaluate map(f)
    void eval(const RingMap *map, const ElementType& f, int first_var, ring_elem &result) const;
    
  private:
    mutable flint_rand_t mRandomState;
    fmpz_t mMaxHeight;
  };
};


#endif
#endif

// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e  "
// indent-tabs-mode: nil
// End: