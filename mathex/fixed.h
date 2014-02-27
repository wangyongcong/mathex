#ifndef WYC_HEADER_FIXED
#define WYC_HEADER_FIXED

#include <cstdint>

namespace wyc
{

/******************************************************************************
HELPER FUNCTIONS
	Branch-free arithmetic helper templates used in the remainder computation
	from the book Hacker's Delight, by Henry S. Warren Jr. with thanks.
******************************************************************************/

template <typename R> inline R neg(R r)	{ 
	return r < 0 ? -1 : 0; 
}
template <> inline int32_t neg(int32_t r)	{ return (r >> 31); }
template <> inline int64_t neg(int64_t r)	{ return (r >> 63); }
template <> inline uint32_t neg(uint32_t)	{ return 0; }
// r < 0 ? -1 : 1; 
template <typename R> inline R sign(R r) {
	return neg(r) | 1; 
}
// r < 0 ? -r : r;
template <typename R> inline R absval(R r) { 
	R t = neg(r); 
	return (r ^ t) - t; 
}

template<typename T>
struct extended_t;
template<>
struct extended_t<int8_t> {
	typedef int16_t TYPE;
};
template<>
struct extended_t<uint8_t> {
	typedef uint16_t TYPE;
};
template<>
struct extended_t<int16_t> {
	typedef int32_t TYPE;
};
template<>
struct extended_t<uint16_t> {
	typedef uint32_t TYPE;
};
template<>
struct extended_t<int32_t> {
	typedef int64_t TYPE;
};
template<>
struct extended_t<uint32_t> {
	typedef uint64_t TYPE;
};

// binary fixed point
//	N: number of fraction digits
//	T: underlying integral representation type
template<unsigned N, typename T>
class xfixed_binary
{
public:
	enum {
		SCALE = 1<<N,
		FRACTION = N,
		INTEGER = sizeof(T)-N,
		MASK_S = SCALE-1,
	};
	typedef typename extended_t<T>::TYPE intermed_t;

	static inline T scaled_multiplication (intermed_t f1, intermed_t f2, int s)
	{
		return rounded_division(f1 * f2, s);
	}
	static inline T rounded_division (intermed_t n, intermed_t d)
	{
		T q = (T)(n / d);
		T r = (T)(n % d);
		q  += (T)(sign(n^d) & neg(absval(d) - (absval(r)<<1) - 1));
		return q;
	}

	xfixed_binary(float v) {
		m_rep = v*SCALE+(v>0?0.5f:-0.5f);
	}
	xfixed_binary(int v) {
		m_rep = v<<FRACTION;
	}
	xfixed_binary(unsigned v) {
		m_rep = v<<FRACTION;
	}
	
	template<bool B>
	struct converter;
	template<> struct converter<false>
	{
		template<int OFFSET>
		static inline T exec (T rhs) {
			return rhs<<-OFFSET;
		}
	};
	template<> struct converter<true>
	{
		template<int OFFSET>
		static inline T exec (T rhs) {
			return rhs>>OFFSET;
		}
	};

	xfixed_binary(xfixed_binary rhs) : m_rep(rhs.m_rep) {}
	inline xfixed_binary& operator = (xfixed_binary rhs) {
		this->m_rep = rhs.m_rep;
		return *this;
	}
	template<unsigned D>
	xfixed_binary(xfixed_binary<D,T> rhs) {
		*this = rhs;
	}
	template<unsigned D>
	inline xfixed_binary& operator = (xfixed_binary<D,T> rhs) {
		m_rep = converter<D>N>::exec<D-N>(rhs.m_rep);
		return *this;
	}

	inline xfixed_binary& operator = (float v) {
		m_rep = v*SCALE+(v>0?0.5f:-0.5f);
		return *this;
	}
	inline xfixed_binary& operator = (int v) {
		m_rep = v<<FRACTION;
		return *this;
	}
	inline xfixed_binary& operator = (unsigned int v) {
		m_rep = v<<FRACTION;
		return *this;
	}

	inline operator float() const {
		return static_cast<float>(m_rep)/SCALE; 
	}
	inline operator int() const {
		return static_cast<int>(m_rep>>FRACTION);
	}
	inline operator unsigned int() const {
		return static_cast<unsigned>(m_rep>>FRACTION);
	}

	bool operator==( xfixed_binary rhs ) const {
		return m_rep==rhs.m_rep;
	}
	bool operator!=( xfixed_binary rhs ) const {
		return m_rep!=rhs.m_rep;
	}
	bool operator< ( xfixed_binary rhs ) const {
		return m_rep<rhs.m_rep;
	}
	bool operator<=( xfixed_binary rhs ) const {
		return m_rep<=rhs.m_rep;
	}
	bool operator> ( xfixed_binary rhs ) const {
		return m_rep>rhs.m_rep;
	}
	bool operator>=( xfixed_binary rhs ) const {
		return m_rep>=rhs.m_rep;
	}

	xfixed_binary operator-() const {
		return xfixed_binary(-m_rep);
	}
	
	xfixed_binary& operator+=( xfixed_binary rhs ) {
		m_rep+=rhs.m_rep;
		return *this;
	}
	xfixed_binary& operator-=( xfixed_binary rhs ) {
		m_rep-=rhs.m_rep;
		return *this;
	}
	xfixed_binary& operator*=( xfixed_binary rhs ) {
		m_rep = scaled_multiplication(m_rep, rhs.m_rep, SCALE);
		return *this;
	}
	xfixed_binary& operator/=( xfixed_binary rhs ) {
		m_rep = rounded_division(static_cast<intermed_t>(m_rep)<<FRACTION, rhs.m_rep);
		return *this;
	}

	xfixed_binary& operator*=( int rhs );
	xfixed_binary& operator*=( unsigned int rhs );
	xfixed_binary& operator/=( int rhs );
	xfixed_binary& operator/=( unsigned int rhs );

private:
	T m_rep;
};

/// 20:12 fixed point
typedef xfixed_binary<12,int32_t> xfixed12;
/// 16:16 fixed point
typedef xfixed_binary<16,int32_t> xfixed16;
/// 12:20 fixed point
typedef xfixed_binary<20,int32_t> xfixed20;
/// 8:24 fixed point
typedef xfixed_binary<24,int32_t> xfixed24;


}; // namespace wyc

#endif // WYC_HEADER_FIXED