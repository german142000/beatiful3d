#ifndef AVIR_CIMAGERESIZER_INCLUDED
#define AVIR_CIMAGERESIZER_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace avir {

#define AVIR_VERSION "3.0"

#define AVIR_PI 3.1415926535897932

#define AVIR_PId2 1.5707963267948966

#define AVIR_NOCTOR( ClassName ) \
	private: \
		ClassName( const ClassName& ) { } \
		ClassName& operator = ( const ClassName& ) { return( *this ); }

template< class T >
inline T round( const T d )
{
	return( d < (T) 0 ? -(T) (int) ( (T) 0.5 - d ) :
		(T) (int) ( d + (T) 0.5 ));
}

template< class T >
inline T clamp( const T& Value, const T minv, const T maxv )
{
	if( Value < minv )
	{
		return( minv );
	}
	else
	if( Value > maxv )
	{
		return( maxv );
	}
	else
	{
		return( Value );
	}
}

template< class T >
inline T pow24_sRGB( const T x )
{
	const double x2 = (double) x * x;
	const double x3 = x2 * x;
	const double x4 = x2 * x2;

	return( (T) ( 0.0985766365536824 + 0.839474952656502 * x2 +
		0.363287814061725 * x3 - 0.0125559718896615 /
		( 0.12758338921578 + 0.290283465468235 * x ) -
		0.231757513261358 * x - 0.0395365717969074 * x4 ));
}

template< class T >
inline T pow24i_sRGB( const T x )
{
	const double sx = sqrt( (double) x );
	const double ssx = sqrt( sx );
	const double sssx = sqrt( ssx );

	return( (T) ( 0.000213364515060263 + 0.0149409239419218 * x +
		0.433973412731747 * sx + ssx * ( 0.659628181609715 * sssx -
		0.0380957908841466 - 0.0706476137208521 * sx )));
}

template< class T >
inline T convertSRGB2Lin( const T s )
{
	const T a = (T) 0.055;

	if( s <= (T) 0.04045 )
	{
		return( s / (T) 12.92 );
	}

	return( pow24_sRGB(( s + a ) / ( (T) 1 + a )));
}

template< class T >
inline T convertLin2SRGB( const T s )
{
	const T a = (T) 0.055;

	if( s <= (T) 0.0031308 )
	{
		return( (T) 12.92 * s );
	}

	return(( (T) 1 + a ) * pow24i_sRGB( s ) - a );
}

template< class T1, class T2 >
inline void copyArray( const T1* ip, T2* op, int l,
	const int ipinc = 1, const int opinc = 1 )
{
	while( l > 0 )
	{
		*op = (T2) *ip;
		op += opinc;
		ip += ipinc;
		l--;
	}
}

template< class T1, class T2 >
inline void addArray( const T1* ip, T2* op, int l,
	const int ipinc = 1, const int opinc = 1 )
{
	while( l > 0 )
	{
		*op += *ip;
		op += opinc;
		ip += ipinc;
		l--;
	}
}

template< class T1, class T2 >
inline void replicateArray( const T1* const ip, const int ipl, T2* op, int l,
	const int opinc )
{
	if( ipl == 1 )
	{
		while( l > 0 )
		{
			op[ 0 ] = (T2) ip[ 0 ];
			op += opinc;
			l--;
		}
	}
	else
	if( ipl == 4 )
	{
		while( l > 0 )
		{
			op[ 0 ] = (T2) ip[ 0 ];
			op[ 1 ] = (T2) ip[ 1 ];
			op[ 2 ] = (T2) ip[ 2 ];
			op[ 3 ] = (T2) ip[ 3 ];
			op += opinc;
			l--;
		}
	}
	else
	if( ipl == 3 )
	{
		while( l > 0 )
		{
			op[ 0 ] = (T2) ip[ 0 ];
			op[ 1 ] = (T2) ip[ 1 ];
			op[ 2 ] = (T2) ip[ 2 ];
			op += opinc;
			l--;
		}
	}
	else
	if( ipl == 2 )
	{
		while( l > 0 )
		{
			op[ 0 ] = (T2) ip[ 0 ];
			op[ 1 ] = (T2) ip[ 1 ];
			op += opinc;
			l--;
		}
	}
	else
	{
		while( l > 0 )
		{
			int i;

			for( i = 0; i < ipl; i++ )
			{
				op[ i ] = (T2) ip[ i ];
			}

			op += opinc;
			l--;
		}
	}
}

template< class T >
inline void calcFIRFilterResponse( const T* flt, int fltlen,
	const double th, double& re0, double& im0, const int fltlat = 0 )
{
	const double sincr = 2.0 * cos( th );
	double cvalue1;
	double svalue1;

	if( fltlat == 0 )
	{
		cvalue1 = 1.0;
		svalue1 = 0.0;
	}
	else
	{
		cvalue1 = cos( -fltlat * th );
		svalue1 = sin( -fltlat * th );
	}

	double cvalue2 = cos( -( fltlat + 1 ) * th );
	double svalue2 = sin( -( fltlat + 1 ) * th );

	double re = 0.0;
	double im = 0.0;

	while( fltlen > 0 )
	{
		re += cvalue1 * flt[ 0 ];
		im += svalue1 * flt[ 0 ];
		flt++;
		fltlen--;

		double tmp = cvalue1;
		cvalue1 = sincr * cvalue1 - cvalue2;
		cvalue2 = tmp;

		tmp = svalue1;
		svalue1 = sincr * svalue1 - svalue2;
		svalue2 = tmp;
	}

	re0 = re;
	im0 = im;
}

template< class T >
inline void normalizeFIRFilter( T* const p, const int l, const double DCGain,
	const int pstep = 1 )
{
	double s = 0.0;
	T* pp = p;
	int i = l;

	while( i > 0 )
	{
		s += *pp;
		pp += pstep;
		i--;
	}

	s = DCGain / s;
	pp = p;
	i = l;

	while( i > 0 )
	{
		*pp = (T) ( *pp * s );
		pp += pstep;
		i--;
	}
}

template< class T, typename capint = int >
class CBuffer
{
public:
	CBuffer()
		: Data( NULL )
		, DataAligned( NULL )
		, Capacity( 0 )
		, Alignment( 0 )
	{
	}

	CBuffer( const capint aCapacity, const int aAlignment = 0 )
	{
		allocinit( aCapacity, aAlignment );
	}

	CBuffer( const CBuffer& Source )
	{
		allocinit( Source.Capacity, Source.Alignment );

		if( Capacity > 0 )
		{
			memcpy( DataAligned, Source.DataAligned, Capacity * sizeof( T ));
		}
	}

	~CBuffer()
	{
		freeData();
	}

	CBuffer& operator = ( const CBuffer& Source )
	{
		alloc( Source.Capacity, Source.Alignment );

		if( Capacity > 0 )
		{
			memcpy( DataAligned, Source.DataAligned, Capacity * sizeof( T ));
		}

		return( *this );
	}

	void alloc( const capint aCapacity, const int aAlignment = 0 )
	{
		freeData();
		allocinit( aCapacity, aAlignment );
	}

	void free()
	{
		freeData();
		Data = NULL;
		DataAligned = NULL;
		Capacity = 0;
		Alignment = 0;
	}

	capint getCapacity() const
	{
		return( Capacity );
	}

	void forceCapacity( const capint NewCapacity )
	{
		Capacity = NewCapacity;
	}

	void increaseCapacity( const capint NewCapacity,
		const bool DoDataCopy = true )
	{
		if( NewCapacity < Capacity )
		{
			return;
		}

		if( DoDataCopy )
		{
			const capint PrevCapacity = Capacity;
			T* const PrevData = Data;
			T* const PrevDataAligned = DataAligned;

			allocinit( NewCapacity, Alignment );

			if( PrevCapacity > 0 )
			{
				memcpy( DataAligned, PrevDataAligned,
					PrevCapacity * sizeof( T ));
			}

			:: free( PrevData );
		}
		else
		{
			:: free( Data );
			allocinit( NewCapacity, Alignment );
		}
	}

	void truncateCapacity( const capint NewCapacity )
	{
		if( NewCapacity >= Capacity )
		{
			return;
		}

		Capacity = NewCapacity;
	}

	void updateCapacity( const capint ReqCapacity )
	{
		if( ReqCapacity <= Capacity )
		{
			return;
		}

		capint NewCapacity = Capacity;

		while( NewCapacity < ReqCapacity )
		{
			NewCapacity += NewCapacity / 3 + 1;
		}

		increaseCapacity( NewCapacity );
	}

	operator T* () const
	{
		return( DataAligned );
	}

private:
	T* Data;

	T* DataAligned;

	capint Capacity;

	int Alignment;

	void allocinit( const capint aCapacity, const int aAlignment )
	{
		if( aAlignment == 0 )
		{
			Data = (T*) :: malloc( aCapacity * sizeof( T ));
			DataAligned = Data;
			Alignment = 0;
		}
		else
		{
			Data = (T*) :: malloc( aCapacity * sizeof( T ) + aAlignment );
			DataAligned = alignptr( Data, aAlignment );
			Alignment = aAlignment;
		}

		Capacity = aCapacity;
	}

	void freeData()
	{
		:: free( Data );
	}

	template< class Tp >
	inline Tp alignptr( const Tp ptr, const uintptr_t align )
	{
		return( (Tp) ( (uintptr_t) ptr + align -
			( (uintptr_t) ptr & ( align - 1 ))) );
	}
};

template< class T >
class CStructArray
{
public:
	CStructArray()
		: ItemCount( 0 )
	{
	}

	CStructArray( const CStructArray& Source )
		: ItemCount( 0 )
		, Items( Source.getItemCount() )
	{
		while( ItemCount < Source.getItemCount() )
		{
			Items[ ItemCount ] = new T( Source[ ItemCount ]);
			ItemCount++;
		}
	}

	~CStructArray()
	{
		clear();
	}

	CStructArray& operator = ( const CStructArray& Source )
	{
		clear();

		const int NewCount = Source.ItemCount;
		Items.updateCapacity( NewCount );

		while( ItemCount < NewCount )
		{
			Items[ ItemCount ] = new T( Source[ ItemCount ]);
			ItemCount++;
		}

		return( *this );
	}

	T& operator []( const int Index )
	{
		return( *Items[ Index ]);
	}

	const T& operator []( const int Index ) const
	{
		return( *Items[ Index ]);
	}

	T& add()
	{
		if( ItemCount == Items.getCapacity() )
		{
			Items.increaseCapacity( ItemCount * 3 / 2 + 1 );
		}

		Items[ ItemCount ] = new T();
		ItemCount++;

		return( (*this)[ ItemCount - 1 ]);
	}

	void setItemCount( const int NewCount )
	{
		if( NewCount > ItemCount )
		{
			Items.increaseCapacity( NewCount );

			while( ItemCount < NewCount )
			{
				Items[ ItemCount ] = new T();
				ItemCount++;
			}
		}
		else
		{
			while( ItemCount > NewCount )
			{
				ItemCount--;
				delete Items[ ItemCount ];
			}
		}
	}

	void clear()
	{
		while( ItemCount > 0 )
		{
			ItemCount--;
			delete Items[ ItemCount ];
		}
	}

	int getItemCount() const
	{
		return( ItemCount );
	}

private:
	int ItemCount;

	CBuffer< T* > Items;

};

class CSineGen
{
public:

	CSineGen( const double si, const double ph )
		: svalue1( sin( ph ))
		, svalue2( sin( ph - si ))
		, sincr( 2.0 * cos( si ))
	{
	}

	double generate()
	{
		const double res = svalue1;

		svalue1 = sincr * res - svalue2;
		svalue2 = res;

		return( res );
	}

private:
	double svalue1;

	double svalue2;

	double sincr;

};

class CDSPWindowGenPeakedCosine
{
public:

	CDSPWindowGenPeakedCosine( const double aAlpha, const double aLen2 )
		: Alpha( aAlpha )
		, Len2( aLen2 )
		, Len2i( 1.0 / aLen2 )
		, wn( 0.0 )
		, w1( AVIR_PId2 / Len2, AVIR_PI * 0.5 )
	{
	}

	double generate()
	{
		const double h = pow( wn * Len2i, Alpha );
		wn += 1.0;

		return( w1.generate() * ( 1.0 - h ));
	}

private:
	double Alpha;

	double Len2;

	double Len2i;

	double wn;

	CSineGen w1;

};

class CDSPFIREQ
{
public:

	void init( const double SampleRate, const double aFilterLength,
		const int aBandCount, const double MinFreq, const double MaxFreq,
		const bool IsLogBands, const double WFAlpha )
	{
		FilterLength = aFilterLength;
		BandCount = aBandCount;

		CenterFreqs.alloc( BandCount );

		z = (int) ceil( FilterLength * 0.5 );
		zi = z + ( z & 1 );
		z2 = z * 2;

		CBuffer< double > oscbuf( z2 );
		initOscBuf( oscbuf );

		CBuffer< double > winbuf( z );
		initWinBuf( winbuf, WFAlpha );

		UseFirstVirtBand = ( MinFreq > 0.0 );
		const int k = zi * ( BandCount + ( UseFirstVirtBand ? 1 : 0 ));
		Kernels1.alloc( k );
		Kernels2.alloc( k );

		double m;
		double mo;

		if( IsLogBands )
		{
			m = exp( log( MaxFreq / MinFreq ) / ( BandCount - 1 ));
			mo = 0.0;
		}
		else
		{
			m = 1.0;
			mo = ( MaxFreq - MinFreq ) / ( BandCount - 1 );
		}

		double f = MinFreq;
		double x1 = 0.0;
		double x2;
		int si;

		if( UseFirstVirtBand )
		{
			si = 0;
		}
		else
		{
			si = 1;
			CenterFreqs[ 0 ] = 0.0;
			f = f * m + mo;
		}

		double* kernbuf1 = &Kernels1[ 0 ];
		double* kernbuf2 = &Kernels2[ 0 ];
		int i;

		for( i = si; i < BandCount; i++ )
		{
			x2 = f * 2.0 / SampleRate;
			CenterFreqs[ i ] = x2;

			fillBandKernel( x1, x2, kernbuf1, kernbuf2, oscbuf, winbuf );

			kernbuf1 += zi;
			kernbuf2 += zi;
			x1 = x2;
			f = f * m + mo;
		}

		if( x1 < 1.0 )
		{
			UseLastVirtBand = true;
			fillBandKernel( x1, 1.0, kernbuf1, kernbuf2, oscbuf, winbuf );
		}
		else
		{
			UseLastVirtBand = false;
		}
	}

	int getFilterLength() const
	{
		return( z2 - 1 );
	}

	int getFilterLatency() const
	{
		return( z - 1 );
	}

	void buildFilter( const double* const BandGains, double* const Filter )
	{
		const double* kernbuf1 = &Kernels1[ 0 ];
		const double* kernbuf2 = &Kernels2[ 0 ];
		double x1 = 0.0;
		double y1 = BandGains[ 0 ];
		double x2;
		double y2;

		int i;
		int si;

		if( UseFirstVirtBand )
		{
			si = 1;
			x2 = CenterFreqs[ 0 ];
			y2 = y1;
		}
		else
		{
			si = 2;
			x2 = CenterFreqs[ 1 ];
			y2 = BandGains[ 1 ];
		}

		copyBandKernel( Filter, kernbuf1, kernbuf2, y1 - y2,
			x1 * y2 - x2 * y1 );

		kernbuf1 += zi;
		kernbuf2 += zi;
		x1 = x2;
		y1 = y2;

		for( i = si; i < BandCount; i++ )
		{
			x2 = CenterFreqs[ i ];
			y2 = BandGains[ i ];

			addBandKernel( Filter, kernbuf1, kernbuf2, y1 - y2,
				x1 * y2 - x2 * y1 );

			kernbuf1 += zi;
			kernbuf2 += zi;
			x1 = x2;
			y1 = y2;
		}

		if( UseLastVirtBand )
		{
			addBandKernel( Filter, kernbuf1, kernbuf2, y1 - y2,
				x1 * y2 - y1 );
		}

		for( i = 0; i < z - 1; i++ )
		{
			Filter[ z + i ] = Filter[ z - 2 - i ];
		}
	}

	static int calcFilterLength( const double aFilterLength, int& Latency )
	{
		const int l = (int) ceil( aFilterLength * 0.5 );
		Latency = l - 1;

		return( l * 2 - 1 );
	}

private:
	double FilterLength;

	int z;

	int zi;

	int z2;

	int BandCount;

	CBuffer< double > CenterFreqs;

	CBuffer< double > Kernels1;

	CBuffer< double > Kernels2;

	bool UseFirstVirtBand;

	bool UseLastVirtBand;

	void initOscBuf( double* oscbuf ) const
	{
		int i = z;

		while( i > 0 )
		{
			oscbuf[ 0 ] = 0.0;
			oscbuf[ 1 ] = 1.0;
			oscbuf += 2;
			i--;
		}
	}

	void initWinBuf( double* winbuf, const double Alpha ) const
	{
		CDSPWindowGenPeakedCosine wf( Alpha, FilterLength * 0.5 );
		int i;

		for( i = 1; i <= z; i++ )
		{
			winbuf[ z - i ] = wf.generate();
		}
	}

	void fillBandKernel( const double x1, const double x2, double* kernbuf1,
		double* kernbuf2, double* oscbuf, const double* const winbuf )
	{
		const double s2_incr = AVIR_PI * x2;
		const double s2_coeff = 2.0 * cos( s2_incr );

		double s2_value1 = sin( s2_incr * ( -z + 1 ));
		double c2_value1 = sin( s2_incr * ( -z + 1 ) + AVIR_PI * 0.5 );
		oscbuf[ 0 ] = sin( s2_incr * -z );
		oscbuf[ 1 ] = sin( s2_incr * -z + AVIR_PI * 0.5 );

		int ks;

		for( ks = 1; ks < z; ks++ )
		{
			const int ks2 = ks * 2;
			const double s1_value1 = oscbuf[ ks2 ];
			const double c1_value1 = oscbuf[ ks2 + 1 ];
			oscbuf[ ks2 ] = s2_value1;
			oscbuf[ ks2 + 1 ] = c2_value1;

			const double x = AVIR_PI * ( ks - z );
			const double v0 = winbuf[ ks - 1 ] / (( x1 - x2 ) * x );

			kernbuf1[ ks - 1 ] = ( x2 * s2_value1 - x1 * s1_value1 +
				( c2_value1 - c1_value1 ) / x ) * v0;

			kernbuf2[ ks - 1 ] = ( s2_value1 - s1_value1 ) * v0;

			s2_value1 = s2_coeff * s2_value1 - oscbuf[ ks2 - 2 ];
			c2_value1 = s2_coeff * c2_value1 - oscbuf[ ks2 - 1 ];
		}

		kernbuf1[ z - 1 ] = ( x2 * x2 - x1 * x1 ) / ( x1 - x2 ) * 0.5;
		kernbuf2[ z - 1 ] = -1.0;
	}

	void copyBandKernel( double* outbuf, const double* const kernbuf1,
		const double* const kernbuf2, const double c, const double d ) const
	{
		int ks;

		for( ks = 0; ks < z; ks++ )
		{
			outbuf[ ks ] = c * kernbuf1[ ks ] + d * kernbuf2[ ks ];
		}
	}

	void addBandKernel( double* outbuf, const double* const kernbuf1,
		const double* const kernbuf2, const double c, const double d ) const
	{
		int ks;

		for( ks = 0; ks < z; ks++ )
		{
			outbuf[ ks ] += c * kernbuf1[ ks ] + d * kernbuf2[ ks ];
		}
	}
};

class CDSPPeakedCosineLPF
{
public:
	int fl2;

	int FilterLen;

	CDSPPeakedCosineLPF( const double aLen2, const double aFreq2,
		const double aAlpha )
		: fl2( (int) ceil( aLen2 ) - 1 )
		, FilterLen( fl2 + fl2 + 1 )
		, Len2( aLen2 )
		, Freq2( aFreq2 )
		, Alpha( aAlpha )
	{
	}

	template< class T >
	void generateLPF( T* op, const double DCGain )
	{
		CDSPWindowGenPeakedCosine wf( Alpha, Len2 );
		CSineGen f2( Freq2, 0.0 );

		op += fl2;
		T* op2 = op;
		f2.generate();

		if( DCGain > 0.0 )
		{
			int t = 1;

			*op = (T) ( Freq2 * wf.generate() );
			double s = *op;

			while( t <= fl2 )
			{
				const T v = (T) ( f2.generate() * wf.generate() / t );
				op++;
				op2--;
				*op = v;
				*op2 = v;
				s += v + v;
				t++;
			}

			t = FilterLen;
			s = DCGain / s;

			while( t > 0 )
			{
				*op2 = (T) ( *op2 * s );
				op2++;
				t--;
			}
		}
		else
		{
			int t = 1;

			*op = (T) ( Freq2 * wf.generate() );

			while( t <= fl2 )
			{
				const T v = (T) ( f2.generate() * wf.generate() / t );
				op++;
				op2--;
				*op = v;
				*op2 = v;
				t++;
			}
		}
	}

private:
	double Len2;

	double Freq2;

	double Alpha;

};

class CFltBuffer : public CBuffer< double >
{
public:
	double Len2;

	double Freq;

	double Alpha;

	double DCGain;

	CFltBuffer()
		: CBuffer< double >()
		, Len2( 0.0 )
		, Freq( 0.0 )
		, Alpha( 0.0 )
		, DCGain( 0.0 )
	{
	}

	bool operator == ( const CFltBuffer& b2 ) const
	{
		return( Len2 == b2.Len2 && Freq == b2.Freq && Alpha == b2.Alpha &&
			DCGain == b2.DCGain );
	}
};

template< class fptype >
class CDSPFracFilterBankLin
{
	AVIR_NOCTOR( CDSPFracFilterBankLin );

public:
	CDSPFracFilterBankLin()
		: Order( -1 )
	{
	}

	void copyInitParams( const CDSPFracFilterBankLin& s )
	{
		WFLen2 = s.WFLen2;
		WFFreq = s.WFFreq;
		WFAlpha = s.WFAlpha;
		FracCount = s.FracCount;
		Order = s.Order;
		Alignment = s.Alignment;
		SrcFilterLen = s.SrcFilterLen;
		FilterLen = s.FilterLen;
		FilterSize = s.FilterSize;
		IsSrcTableBuilt = false;
		ExtFilter = s.ExtFilter;
		TableFillFlags.alloc( s.TableFillFlags.getCapacity() );
		int i;

		for( i = 0; i < TableFillFlags.getCapacity(); i++ )
		{
			TableFillFlags[ i ] = (uint8_t) ( s.TableFillFlags[ i ] << 2 );
		}
	}

	bool operator == ( const CDSPFracFilterBankLin& s ) const
	{
		return( Order == s.Order && WFLen2 == s.WFLen2 &&
			WFFreq == s.WFFreq && WFAlpha == s.WFAlpha &&
			FracCount == s.FracCount && ExtFilter == s.ExtFilter );
	}

	void init( const int ReqFracCount, const int ReqOrder,
		const double BaseLen, const double Cutoff, const double aWFAlpha,
		const CFltBuffer& aExtFilter, const int aAlignment = 0,
		const int FltLenAlign = 1 )
	{
		double NewWFLen2 = 0.5 * BaseLen * ReqFracCount;
		double NewWFFreq = AVIR_PI * Cutoff / ReqFracCount;
		double NewWFAlpha = aWFAlpha;

		if( ReqOrder == Order && NewWFLen2 == WFLen2 && NewWFFreq == WFFreq &&
			NewWFAlpha == WFAlpha && ReqFracCount == FracCount &&
			aExtFilter == ExtFilter )
		{
			IsInitRequired = false;
			return;
		}

		WFLen2 = NewWFLen2;
		WFFreq = NewWFFreq;
		WFAlpha = NewWFAlpha;
		FracCount = ReqFracCount;
		Order = ReqOrder;
		Alignment = aAlignment;
		ExtFilter = aExtFilter;

		CDSPPeakedCosineLPF p( WFLen2, WFFreq, WFAlpha );
		SrcFilterLen = ( p.fl2 / ReqFracCount + 1 ) * 2;

		const int ElementSize = ReqOrder + 1;
		FilterLen = SrcFilterLen;

		if( ExtFilter.getCapacity() > 0 )
		{
			FilterLen += ExtFilter.getCapacity() - 1;
		}

		FilterLen = ( FilterLen + FltLenAlign - 1 ) & ~( FltLenAlign - 1 );
		FilterSize = FilterLen * ElementSize;
		IsSrcTableBuilt = false;
		IsInitRequired = true;
	}

	int getFilterLen() const
	{
		return( FilterLen );
	}

	int getFracCount() const
	{
		return( FracCount );
	}

	int getOrder() const
	{
		return( Order );
	}

	const fptype* getFilter( const int i )
	{
		if( !IsSrcTableBuilt )
		{
			buildSrcTable();
		}

		fptype* const Res = &Table[ i * FilterSize ];

		if(( TableFillFlags[ i ] & 2 ) == 0 )
		{
			createFilter( i );
			TableFillFlags[ i ] |= 2;

			if( Order > 0 )
			{
				createFilter( i + 1 );
				const fptype* const Res2 = Res + FilterSize;
				fptype* const op = Res + FilterLen;
				int j;

				for( j = 0; j < FilterLen; j++ )
				{
					op[ j ] = Res2[ j ] - Res[ j ];
				}
			}
		}

		return( Res );
	}

	void createAllFilters()
	{
		int i;

		for( i = 0; i < FracCount; i++ )
		{
			getFilter( i );
		}
	}

	int calcInitComplexity( const CBuffer< uint8_t >& FracUseMap ) const
	{
		const int FltInitCost = 65;

		const int FltUseCost = FilterLen * Order +
			SrcFilterLen * ExtFilter.getCapacity();

		const int ucb[ 2 ] = { 0, FltUseCost };
		int ic;
		int i;

		if( IsInitRequired )
		{
			ic = FracCount * SrcFilterLen * FltInitCost;

			for( i = 0; i < FracCount; i++ )
			{
				ic += ucb[ FracUseMap[ i ]];
			}
		}
		else
		{
			ic = 0;

			for( i = 0; i < FracCount; i++ )
			{
				if( FracUseMap[ i ] != 0 )
				{
					ic += ucb[ TableFillFlags[ i ] == 0 ? 1 : 0 ];
				}
			}
		}

		return( ic );
	}

private:
	static const int InterpPoints = 2;

	double WFLen2;

	double WFFreq;

	double WFAlpha;

	int FracCount;

	int Order;

	int Alignment;

	int SrcFilterLen;

	int FilterLen;

	int FilterSize;

	bool IsInitRequired;

	CBuffer< fptype > Table;

	CBuffer< uint8_t > TableFillFlags;

	CFltBuffer ExtFilter;

	CBuffer< double > SrcTable;

	bool IsSrcTableBuilt;

	void buildSrcTable()
	{
		IsSrcTableBuilt = true;
		IsInitRequired = false;

		CDSPPeakedCosineLPF p( WFLen2, WFFreq, WFAlpha );

		const int BufLen = SrcFilterLen * FracCount + InterpPoints - 1;
		const int BufOffs = InterpPoints / 2 - 1;
		const int BufCenter = SrcFilterLen * FracCount / 2 + BufOffs;

		CBuffer< double > Buf( BufLen );
		memset( Buf, 0, ( BufCenter - p.fl2 ) * sizeof( double ));
		int i = BufLen - BufCenter - p.fl2 - 1;
		memset( &Buf[ BufLen - i ], 0, i * sizeof( double ));

		p.generateLPF( &Buf[ BufCenter - p.fl2 ], 0.0 );

		SrcTable.alloc(( FracCount + 1 ) * SrcFilterLen );
		TableFillFlags.alloc( FracCount + 1 );
		int j;
		double* op0 = SrcTable;

		for( i = FracCount; i >= 0; i-- )
		{
			TableFillFlags[ i ] = 0;
			double* p = Buf + BufOffs + i;

			for( j = 0; j < SrcFilterLen; j++ )
			{
				op0[ 0 ] = p[ 0 ];
				op0++;
				p += FracCount;
			}

			normalizeFIRFilter( op0 - SrcFilterLen, SrcFilterLen, 1.0 );
		}

		Table.alloc(( FracCount + 1 ) * FilterSize, Alignment );
	}

	void createFilter( const int k )
	{
		if( TableFillFlags[ k ] != 0 )
		{
			return;
		}

		TableFillFlags[ k ] |= 1;
		const int ExtFilterLatency = ExtFilter.getCapacity() / 2;
		const int ResLatency = ExtFilterLatency + SrcFilterLen / 2;
		int ResLen = SrcFilterLen;

		if( ExtFilter.getCapacity() > 0 )
		{
			ResLen += ExtFilter.getCapacity() - 1;
		}

		const int ResOffs = FilterLen / 2 - ResLatency;
		fptype* op = &Table[ k * FilterSize ];
		int i;

		for( i = 0; i < ResOffs; i++ )
		{
			op[ i ] = (fptype) 0;
		}

		for( i = ResOffs + ResLen; i < FilterLen; i++ )
		{
			op[ i ] = (fptype) 0;
		}

		op += ResOffs;
		const double* const srcflt = &SrcTable[ k * SrcFilterLen ];

		if( ExtFilter.getCapacity() == 0 )
		{
			for( i = 0; i < ResLen; i++ )
			{
				op[ i ] = (fptype) srcflt[ i ];
			}

			return;
		}

		const double* const extflt = &ExtFilter[ 0 ];
		int j;

		for( j = 0; j < ResLen; j++ )
		{
			int k = 0;
			int l = j - ExtFilter.getCapacity() + 1;
			int r = l + ExtFilter.getCapacity();

			if( l < 0 )
			{
				k -= l;
				l = 0;
			}

			if( r > SrcFilterLen )
			{
				r = SrcFilterLen;
			}

			const double* const extfltb = extflt + k;
			const double* const srcfltb = srcflt + l;
			double s = 0.0;
			l = r - l;

			for( i = 0; i < l; i++ )
			{
				s += extfltb[ i ] * srcfltb[ i ];
			}

			op[ j ] = (fptype) s;
		}
	}
};

class CImageResizerThreadPool
{
public:
	CImageResizerThreadPool()
	{
	}

	virtual ~CImageResizerThreadPool()
	{
	}

	class CWorkload
	{
	public:
		virtual ~CWorkload()
		{
		}

		virtual void process() = 0;
	};

	virtual int getSuggestedWorkloadCount() const
	{
		return( 1 );
	}

	virtual void addWorkload( CWorkload* const Workload )
	{
	}

	virtual void startAllWorkloads()
	{
	}

	virtual void waitAllWorkloadsToFinish()
	{
	}

	virtual void removeAllWorkloads()
	{
	}
};

struct CImageResizerParams
{
	double CorrFltAlpha;

	double CorrFltLen;

	double IntFltAlpha;

	double IntFltCutoff;

	double IntFltLen;

	double LPFltAlpha;

	double LPFltBaseLen;

	double LPFltCutoffMult;

	CImageResizerParams()
		: HBFltAlpha( 1.94609 )
		, HBFltCutoff( 0.46437 )
		, HBFltLen( 24 )
	{
	}

	double HBFltAlpha;

	double HBFltCutoff;

	double HBFltLen;

};

struct CImageResizerParamsDef : public CImageResizerParams
{
	CImageResizerParamsDef()
	{
		CorrFltAlpha = 0.97946;
		CorrFltLen = 6.4262;
		IntFltAlpha = 6.41341;
		IntFltCutoff = 0.7372;
		IntFltLen = 18;
		LPFltAlpha = 4.76449;
		LPFltBaseLen = 7.55999999999998;
		LPFltCutoffMult = 0.79285;
	}
};

struct CImageResizerParamsULR : public CImageResizerParams
{
	CImageResizerParamsULR()
	{
		CorrFltAlpha = 0.95521;
		CorrFltLen = 5.70774;
		IntFltAlpha = 1.00766;
		IntFltCutoff = 0.74202;
		IntFltLen = 18;
		LPFltAlpha = 1.6801;
		LPFltBaseLen = 6.62;
		LPFltCutoffMult = 0.67821;
	}
};

struct CImageResizerParamsLR : public CImageResizerParams
{
	CImageResizerParamsLR()
	{
		CorrFltAlpha = 1;
		CorrFltLen = 5.865;
		IntFltAlpha = 1.79529;
		IntFltCutoff = 0.74325;
		IntFltLen = 18;
		LPFltAlpha = 1.87597;
		LPFltBaseLen = 6.89999999999999;
		LPFltCutoffMult = 0.69326;
	}
};

struct CImageResizerParamsLow : public CImageResizerParams
{
	CImageResizerParamsLow()
	{
		CorrFltAlpha = 0.99739;
		CorrFltLen = 6.20326;
		IntFltAlpha = 4.6836;
		IntFltCutoff = 0.73879;
		IntFltLen = 18;
		LPFltAlpha = 7.86565;
		LPFltBaseLen = 6.91999999999999;
		LPFltCutoffMult = 0.78379;
	}
};

struct CImageResizerParamsHigh : public CImageResizerParams
{
	CImageResizerParamsHigh()
	{
		CorrFltAlpha = 0.97433;
		CorrFltLen = 6.87893;
		IntFltAlpha = 7.74731;
		IntFltCutoff = 0.73844;
		IntFltLen = 18;
		LPFltAlpha = 4.8149;
		LPFltBaseLen = 8.07999999999996;
		LPFltCutoffMult = 0.79335;
	}
};

struct CImageResizerParamsUltra : public CImageResizerParams
{
	CImageResizerParamsUltra()
	{
		CorrFltAlpha = 0.99705;
		CorrFltLen = 7.42695;
		IntFltAlpha = 1.71985;
		IntFltCutoff = 0.7571;
		IntFltLen = 18;
		LPFltAlpha = 6.71313;
		LPFltBaseLen = 8.27999999999996;
		LPFltCutoffMult = 0.78413;
	}
};

class CImageResizerVars
{
public:
	int ElCount;

	int ElCountIO;

	int fppack;

	int fpalign;

	int elalign;

	int packmode;

	int BufLen[ 2 ];
	int BufOffs[ 2 ];

	double k;

	double o;

	int ResizeStep;

	bool IsResize2;

	double InGammaMult;

	double OutGammaMult;

	double ox;

	double oy;

	CImageResizerThreadPool* ThreadPool;

	bool UseSRGBGamma;

	int BuildMode;

	int RndSeed;

	CImageResizerVars()
		: ox( 0.0 )
		, oy( 0.0 )
		, ThreadPool( NULL )
		, UseSRGBGamma( false )
		, BuildMode( -1 )
		, RndSeed( 0 )
	{
	}
};

template< class fptype, class fptypeatom >
class CImageResizerFilterStep
{
	AVIR_NOCTOR( CImageResizerFilterStep );

public:
	bool IsUpsample;

	int ResampleFactor;

	CBuffer< fptype > Flt;

	CFltBuffer FltOrig;

	double DCGain;

	int FltLatency;

	const CImageResizerVars* Vars;

	int InLen;

	int InBuf;

	int InPrefix;

	int InSuffix;

	int InElIncr;

	int OutLen;

	int OutBuf;

	int OutPrefix;

	int OutSuffix;

	int OutElIncr;

	CBuffer< fptype > PrefixDC;

	CBuffer< fptype > SuffixDC;

	int EdgePixelCount;

	static const int EdgePixelCountDef = 3;

	struct CResizePos
	{
		int SrcPosInt;

		int fti;

		const fptype* ftp;

		fptypeatom x;

		int SrcOffs;

		int fl;

	};

	class CRPosBuf : public CBuffer< CResizePos >
	{
	public:
		double k;

		double o;

		int FracCount;

	};

	class CRPosBufArray : public CStructArray< CRPosBuf >
	{
	public:
		using CStructArray< CRPosBuf > :: add;
		using CStructArray< CRPosBuf > :: getItemCount;

		CRPosBuf& getRPosBuf( const double k, const double o,
			const int FracCount )
		{
			int i;

			for( i = 0; i < getItemCount(); i++ )
			{
				CRPosBuf& Buf = (*this)[ i ];

				if( Buf.k == k && Buf.o == o && Buf.FracCount == FracCount )
				{
					return( Buf );
				}
			}

			CRPosBuf& NewBuf = add();
			NewBuf.k = k;
			NewBuf.o = o;
			NewBuf.FracCount = FracCount;

			return( NewBuf );
		}
	};

	CRPosBuf* RPosBuf;

	CDSPFracFilterBankLin< fptype >* FltBank;

	CImageResizerFilterStep()
	{
	}
};

template< class fptype, class fptypeatom >
class CImageResizerFilterStepINL :
	public CImageResizerFilterStep< fptype, fptypeatom >
{
public:
	using CImageResizerFilterStep< fptype, fptypeatom > :: IsUpsample;
	using CImageResizerFilterStep< fptype, fptypeatom > :: ResampleFactor;
	using CImageResizerFilterStep< fptype, fptypeatom > :: Flt;
	using CImageResizerFilterStep< fptype, fptypeatom > :: FltOrig;
	using CImageResizerFilterStep< fptype, fptypeatom > :: FltLatency;
	using CImageResizerFilterStep< fptype, fptypeatom > :: Vars;
	using CImageResizerFilterStep< fptype, fptypeatom > :: InLen;
	using CImageResizerFilterStep< fptype, fptypeatom > :: InPrefix;
	using CImageResizerFilterStep< fptype, fptypeatom > :: InSuffix;
	using CImageResizerFilterStep< fptype, fptypeatom > :: OutLen;
	using CImageResizerFilterStep< fptype, fptypeatom > :: OutPrefix;
	using CImageResizerFilterStep< fptype, fptypeatom > :: OutSuffix;
	using CImageResizerFilterStep< fptype, fptypeatom > :: PrefixDC;
	using CImageResizerFilterStep< fptype, fptypeatom > :: SuffixDC;
	using CImageResizerFilterStep< fptype, fptypeatom > :: RPosBuf;
	using CImageResizerFilterStep< fptype, fptypeatom > :: FltBank;
	using CImageResizerFilterStep< fptype, fptypeatom > :: EdgePixelCount;

	template< class Tin >
	void packScanline( const Tin* ip, fptype* const op0, const int l0 ) const
	{
		const int ElCount = Vars -> ElCount;
		const int ElCountIO = Vars -> ElCountIO;
		fptype* op = op0;
		int l = l0;

		if( !Vars -> UseSRGBGamma )
		{
			if( ElCountIO == 1 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = (fptypeatom) ip[ 0 ];
					op += ElCount;
					ip++;
					l--;
				}
			}
			else
			if( ElCountIO == 4 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = (fptypeatom) ip[ 0 ];
					v[ 1 ] = (fptypeatom) ip[ 1 ];
					v[ 2 ] = (fptypeatom) ip[ 2 ];
					v[ 3 ] = (fptypeatom) ip[ 3 ];
					op += ElCount;
					ip += 4;
					l--;
				}
			}
			else
			if( ElCountIO == 3 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = (fptypeatom) ip[ 0 ];
					v[ 1 ] = (fptypeatom) ip[ 1 ];
					v[ 2 ] = (fptypeatom) ip[ 2 ];
					op += ElCount;
					ip += 3;
					l--;
				}
			}
			else
			if( ElCountIO == 2 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = (fptypeatom) ip[ 0 ];
					v[ 1 ] = (fptypeatom) ip[ 1 ];
					op += ElCount;
					ip += 2;
					l--;
				}
			}
		}
		else
		{
			const fptypeatom gm = (fptypeatom) Vars -> InGammaMult;

			if( ElCountIO == 1 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = convertSRGB2Lin( (fptypeatom) ip[ 0 ] * gm );
					op += ElCount;
					ip++;
					l--;
				}
			}
			else
			if( ElCountIO == 4 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = convertSRGB2Lin( (fptypeatom) ip[ 0 ] * gm );
					v[ 1 ] = convertSRGB2Lin( (fptypeatom) ip[ 1 ] * gm );
					v[ 2 ] = convertSRGB2Lin( (fptypeatom) ip[ 2 ] * gm );
					v[ 3 ] = convertSRGB2Lin( (fptypeatom) ip[ 3 ] * gm );
					op += ElCount;
					ip += 4;
					l--;
				}
			}
			else
			if( ElCountIO == 3 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = convertSRGB2Lin( (fptypeatom) ip[ 0 ] * gm );
					v[ 1 ] = convertSRGB2Lin( (fptypeatom) ip[ 1 ] * gm );
					v[ 2 ] = convertSRGB2Lin( (fptypeatom) ip[ 2 ] * gm );
					op += ElCount;
					ip += 3;
					l--;
				}
			}
			else
			if( ElCountIO == 2 )
			{
				while( l > 0 )
				{
					fptypeatom* v = (fptypeatom*) op;
					v[ 0 ] = convertSRGB2Lin( (fptypeatom) ip[ 0 ] * gm );
					v[ 1 ] = convertSRGB2Lin( (fptypeatom) ip[ 1 ] * gm );
					op += ElCount;
					ip += 2;
					l--;
				}
			}
		}

		const int ZeroCount = ElCount * Vars -> fppack - ElCountIO;
		op = (fptype*) ( (fptypeatom*) op0 + ElCountIO );
		l = l0;

		if( ZeroCount == 1 )
		{
			while( l > 0 )
			{
				fptypeatom* v = (fptypeatom*) op;
				v[ 0 ] = (fptypeatom) 0;
				op += ElCount;
				l--;
			}
		}
		else
		if( ZeroCount == 2 )
		{
			while( l > 0 )
			{
				fptypeatom* v = (fptypeatom*) op;
				v[ 0 ] = (fptypeatom) 0;
				v[ 1 ] = (fptypeatom) 0;
				op += ElCount;
				l--;
			}
		}
		else
		if( ZeroCount == 3 )
		{
			while( l > 0 )
			{
				fptypeatom* v = (fptypeatom*) op;
				v[ 0 ] = (fptypeatom) 0;
				v[ 1 ] = (fptypeatom) 0;
				v[ 2 ] = (fptypeatom) 0;
				op += ElCount;
				l--;
			}
		}
	}

	static void applySRGBGamma( fptype* p, int l,
		const CImageResizerVars& Vars0 )
	{
		const int ElCount = Vars0.ElCount;
		const int ElCountIO = Vars0.ElCountIO;
		const fptypeatom gm = (fptypeatom) Vars0.OutGammaMult;

		if( ElCountIO == 1 )
		{
			while( l > 0 )
			{
				fptypeatom* v = (fptypeatom*) p;
				v[ 0 ] = convertLin2SRGB( v[ 0 ]) * gm;
				p += ElCount;
				l--;
			}
		}
		else
		if( ElCountIO == 4 )
		{
			while( l > 0 )
			{
				fptypeatom* v = (fptypeatom*) p;
				v[ 0 ] = convertLin2SRGB( v[ 0 ]) * gm;
				v[ 1 ] = convertLin2SRGB( v[ 1 ]) * gm;
				v[ 2 ] = convertLin2SRGB( v[ 2 ]) * gm;
				v[ 3 ] = convertLin2SRGB( v[ 3 ]) * gm;
				p += ElCount;
				l--;
			}
		}
		else
		if( ElCountIO == 3 )
		{
			while( l > 0 )
			{
				fptypeatom* v = (fptypeatom*) p;
				v[ 0 ] = convertLin2SRGB( v[ 0 ]) * gm;
				v[ 1 ] = convertLin2SRGB( v[ 1 ]) * gm;
				v[ 2 ] = convertLin2SRGB( v[ 2 ]) * gm;
				p += ElCount;
				l--;
			}
		}
		else
		if( ElCountIO == 2 )
		{
			while( l > 0 )
			{
				fptypeatom* v = (fptypeatom*) p;
				v[ 0 ] = convertLin2SRGB( v[ 0 ]) * gm;
				v[ 1 ] = convertLin2SRGB( v[ 1 ]) * gm;
				p += ElCount;
				l--;
			}
		}
	}

	void convertVtoH( const fptype* ip, fptype* op, const int SrcLen,
		const int SrcIncr ) const
	{
		const int ElCount = Vars -> ElCount;
		int j;

		if( ElCount == 1 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				ip += SrcIncr;
				op++;
			}
		}
		else
		if( ElCount == 4 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				op[ 1 ] = ip[ 1 ];
				op[ 2 ] = ip[ 2 ];
				op[ 3 ] = ip[ 3 ];
				ip += SrcIncr;
				op += 4;
			}
		}
		else
		if( ElCount == 3 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				op[ 1 ] = ip[ 1 ];
				op[ 2 ] = ip[ 2 ];
				ip += SrcIncr;
				op += 3;
			}
		}
		else
		if( ElCount == 2 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				op[ 1 ] = ip[ 1 ];
				ip += SrcIncr;
				op += 2;
			}
		}
	}

	template< class Tout >
	static void unpackScanline( const fptype* ip, Tout* op, int l,
		const CImageResizerVars& Vars0 )
	{
		const int ElCount = Vars0.ElCount;
		const int ElCountIO = Vars0.ElCountIO;

		if( ElCountIO == 1 )
		{
			while( l > 0 )
			{
				const fptypeatom* v = (const fptypeatom*) ip;
				op[ 0 ] = (Tout) v[ 0 ];
				ip += ElCount;
				op++;
				l--;
			}
		}
		else
		if( ElCountIO == 4 )
		{
			while( l > 0 )
			{
				const fptypeatom* v = (const fptypeatom*) ip;
				op[ 0 ] = (Tout) v[ 0 ];
				op[ 1 ] = (Tout) v[ 1 ];
				op[ 2 ] = (Tout) v[ 2 ];
				op[ 3 ] = (Tout) v[ 3 ];
				ip += ElCount;
				op += 4;
				l--;
			}
		}
		else
		if( ElCountIO == 3 )
		{
			while( l > 0 )
			{
				const fptypeatom* v = (const fptypeatom*) ip;
				op[ 0 ] = (Tout) v[ 0 ];
				op[ 1 ] = (Tout) v[ 1 ];
				op[ 2 ] = (Tout) v[ 2 ];
				ip += ElCount;
				op += 3;
				l--;
			}
		}
		else
		if( ElCountIO == 2 )
		{
			while( l > 0 )
			{
				const fptypeatom* v = (const fptypeatom*) ip;
				op[ 0 ] = (Tout) v[ 0 ];
				op[ 1 ] = (Tout) v[ 1 ];
				ip += ElCount;
				op += 2;
				l--;
			}
		}
	}

	void calcScanlineBias( const fptype* p, const int SrcLen,
		fptype* const ElBiases ) const
	{
		const int ElCount = Vars -> ElCount;
		int l = SrcLen;

		if( ElCount == 1 )
		{
			fptype b0 = (fptype) 0;

			while( l > 0 )
			{
				b0 += p[ 0 ];
				p++;
				l--;
			}

			ElBiases[ 0 ] = b0 / (fptype) SrcLen;
		}
		else
		if( ElCount == 4 )
		{
			fptype b0 = (fptype) 0;
			fptype b1 = (fptype) 0;
			fptype b2 = (fptype) 0;
			fptype b3 = (fptype) 0;

			while( l > 0 )
			{
				b0 += p[ 0 ];
				b1 += p[ 1 ];
				b2 += p[ 2 ];
				b3 += p[ 3 ];
				p += 4;
				l--;
			}

			ElBiases[ 0 ] = b0 / (fptype) SrcLen;
			ElBiases[ 1 ] = b1 / (fptype) SrcLen;
			ElBiases[ 2 ] = b2 / (fptype) SrcLen;
			ElBiases[ 3 ] = b3 / (fptype) SrcLen;
		}
		else
		if( ElCount == 3 )
		{
			fptype b0 = (fptype) 0;
			fptype b1 = (fptype) 0;
			fptype b2 = (fptype) 0;

			while( l > 0 )
			{
				b0 += p[ 0 ];
				b1 += p[ 1 ];
				b2 += p[ 2 ];
				p += 3;
				l--;
			}

			ElBiases[ 0 ] = b0 / (fptype) SrcLen;
			ElBiases[ 1 ] = b1 / (fptype) SrcLen;
			ElBiases[ 2 ] = b2 / (fptype) SrcLen;
		}
		else
		if( ElCount == 2 )
		{
			fptype b0 = (fptype) 0;
			fptype b1 = (fptype) 0;

			while( l > 0 )
			{
				b0 += p[ 0 ];
				b1 += p[ 1 ];
				p += 2;
				l--;
			}

			ElBiases[ 0 ] = b0 / (fptype) SrcLen;
			ElBiases[ 1 ] = b1 / (fptype) SrcLen;
		}
	}

	void unbiasScanline( fptype* p, int l,
		const fptype* const ElBiases ) const
	{
		const int ElCount = Vars -> ElCount;

		if( ElCount == 1 )
		{
			const fptype b0 = ElBiases[ 0 ];

			while( l > 0 )
			{
				p[ 0 ] -= b0;
				p++;
				l--;
			}
		}
		else
		if( ElCount == 4 )
		{
			const fptype b0 = ElBiases[ 0 ];
			const fptype b1 = ElBiases[ 1 ];
			const fptype b2 = ElBiases[ 2 ];
			const fptype b3 = ElBiases[ 3 ];

			while( l > 0 )
			{
				p[ 0 ] -= b0;
				p[ 1 ] -= b1;
				p[ 2 ] -= b2;
				p[ 3 ] -= b3;
				p += 4;
				l--;
			}
		}
		else
		if( ElCount == 3 )
		{
			const fptype b0 = ElBiases[ 0 ];
			const fptype b1 = ElBiases[ 1 ];
			const fptype b2 = ElBiases[ 2 ];

			while( l > 0 )
			{
				p[ 0 ] -= b0;
				p[ 1 ] -= b1;
				p[ 2 ] -= b2;
				p += 3;
				l--;
			}
		}
		else
		if( ElCount == 2 )
		{
			const fptype b0 = ElBiases[ 0 ];
			const fptype b1 = ElBiases[ 1 ];

			while( l > 0 )
			{
				p[ 0 ] -= b0;
				p[ 1 ] -= b1;
				p += 2;
				l--;
			}
		}
	}

	void prepareInBuf( fptype* Src ) const
	{
		if( IsUpsample || InPrefix + InSuffix == 0 )
		{
			return;
		}

		const int ElCount = Vars -> ElCount;
		replicateArray( Src, ElCount, Src - ElCount, InPrefix, -ElCount );

		Src += ( InLen - 1 ) * ElCount;
		replicateArray( Src, ElCount, Src + ElCount, InSuffix, ElCount );
	}

	void doUpsample( const fptype* const Src, fptype* const Dst ) const
	{
		const int ElCount = Vars -> ElCount;
		fptype* op0 = &Dst[ -OutPrefix * ElCount ];
		memset( op0, 0, ( OutPrefix + OutLen + OutSuffix ) * ElCount *
			sizeof( fptype ));

		const fptype* ip = Src;
		const int opstep = ElCount * ResampleFactor;
		int l;

		if( FltOrig.getCapacity() > 0 )
		{

			op0 += ( OutPrefix % ResampleFactor ) * ElCount;
			l = OutPrefix / ResampleFactor;

			if( ElCount == 1 )
			{
				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0 += opstep;
					l--;
				}

				l = InLen - 1;

				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0 += opstep;
					ip += ElCount;
					l--;
				}

				l = OutSuffix / ResampleFactor;

				while( l >= 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0 += opstep;
					l--;
				}
			}
			else
			if( ElCount == 4 )
			{
				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0[ 2 ] = ip[ 2 ];
					op0[ 3 ] = ip[ 3 ];
					op0 += opstep;
					l--;
				}

				l = InLen - 1;

				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0[ 2 ] = ip[ 2 ];
					op0[ 3 ] = ip[ 3 ];
					op0 += opstep;
					ip += ElCount;
					l--;
				}

				l = OutSuffix / ResampleFactor;

				while( l >= 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0[ 2 ] = ip[ 2 ];
					op0[ 3 ] = ip[ 3 ];
					op0 += opstep;
					l--;
				}
			}
			else
			if( ElCount == 3 )
			{
				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0[ 2 ] = ip[ 2 ];
					op0 += opstep;
					l--;
				}

				l = InLen - 1;

				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0[ 2 ] = ip[ 2 ];
					op0 += opstep;
					ip += ElCount;
					l--;
				}

				l = OutSuffix / ResampleFactor;

				while( l >= 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0[ 2 ] = ip[ 2 ];
					op0 += opstep;
					l--;
				}
			}
			else
			if( ElCount == 2 )
			{
				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0 += opstep;
					l--;
				}

				l = InLen - 1;

				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0 += opstep;
					ip += ElCount;
					l--;
				}

				l = OutSuffix / ResampleFactor;

				while( l >= 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0[ 1 ] = ip[ 1 ];
					op0 += opstep;
					l--;
				}
			}

			return;
		}

		const fptype* const f = Flt;
		const int flen = Flt.getCapacity();
		fptype* op;
		int i;

		if( ElCount == 1 )
		{
			l = InPrefix;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ i ] += f[ i ] * ip[ 0 ];
				}

				op0 += opstep;
				l--;
			}

			l = InLen - 1;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ i ] += f[ i ] * ip[ 0 ];
				}

				ip += ElCount;
				op0 += opstep;
				l--;
			}

			l = InSuffix;

			while( l >= 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ i ] += f[ i ] * ip[ 0 ];
				}

				op0 += opstep;
				l--;
			}
		}
		else
		if( ElCount == 4 )
		{
			l = InPrefix;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op[ 2 ] += f[ i ] * ip[ 2 ];
					op[ 3 ] += f[ i ] * ip[ 3 ];
					op += 4;
				}

				op0 += opstep;
				l--;
			}

			l = InLen - 1;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op[ 2 ] += f[ i ] * ip[ 2 ];
					op[ 3 ] += f[ i ] * ip[ 3 ];
					op += 4;
				}

				ip += ElCount;
				op0 += opstep;
				l--;
			}

			l = InSuffix;

			while( l >= 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op[ 2 ] += f[ i ] * ip[ 2 ];
					op[ 3 ] += f[ i ] * ip[ 3 ];
					op += 4;
				}

				op0 += opstep;
				l--;
			}
		}
		else
		if( ElCount == 3 )
		{
			l = InPrefix;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op[ 2 ] += f[ i ] * ip[ 2 ];
					op += 3;
				}

				op0 += opstep;
				l--;
			}

			l = InLen - 1;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op[ 2 ] += f[ i ] * ip[ 2 ];
					op += 3;
				}

				ip += ElCount;
				op0 += opstep;
				l--;
			}

			l = InSuffix;

			while( l >= 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op[ 2 ] += f[ i ] * ip[ 2 ];
					op += 3;
				}

				op0 += opstep;
				l--;
			}
		}
		else
		if( ElCount == 2 )
		{
			l = InPrefix;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op += 2;
				}

				op0 += opstep;
				l--;
			}

			l = InLen - 1;

			while( l > 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op += 2;
				}

				ip += ElCount;
				op0 += opstep;
				l--;
			}

			l = InSuffix;

			while( l >= 0 )
			{
				op = op0;

				for( i = 0; i < flen; i++ )
				{
					op[ 0 ] += f[ i ] * ip[ 0 ];
					op[ 1 ] += f[ i ] * ip[ 1 ];
					op += 2;
				}

				op0 += opstep;
				l--;
			}
		}

		op = op0;
		const fptype* dc = SuffixDC;
		l = SuffixDC.getCapacity();

		if( ElCount == 1 )
		{
			for( i = 0; i < l; i++ )
			{
				op[ i ] += ip[ 0 ] * dc[ i ];
			}
		}
		else
		if( ElCount == 4 )
		{
			while( l > 0 )
			{
				op[ 0 ] += ip[ 0 ] * dc[ 0 ];
				op[ 1 ] += ip[ 1 ] * dc[ 0 ];
				op[ 2 ] += ip[ 2 ] * dc[ 0 ];
				op[ 3 ] += ip[ 3 ] * dc[ 0 ];
				dc++;
				op += 4;
				l--;
			}
		}
		else
		if( ElCount == 3 )
		{
			while( l > 0 )
			{
				op[ 0 ] += ip[ 0 ] * dc[ 0 ];
				op[ 1 ] += ip[ 1 ] * dc[ 0 ];
				op[ 2 ] += ip[ 2 ] * dc[ 0 ];
				dc++;
				op += 3;
				l--;
			}
		}
		else
		if( ElCount == 2 )
		{
			while( l > 0 )
			{
				op[ 0 ] += ip[ 0 ] * dc[ 0 ];
				op[ 1 ] += ip[ 1 ] * dc[ 0 ];
				dc++;
				op += 2;
				l--;
			}
		}

		ip = Src;
		op = Dst - InPrefix * opstep;
		dc = PrefixDC;
		l = PrefixDC.getCapacity();

		if( ElCount == 1 )
		{
			for( i = 0; i < l; i++ )
			{
				op[ i ] += ip[ 0 ] * dc[ i ];
			}
		}
		else
		if( ElCount == 4 )
		{
			while( l > 0 )
			{
				op[ 0 ] += ip[ 0 ] * dc[ 0 ];
				op[ 1 ] += ip[ 1 ] * dc[ 0 ];
				op[ 2 ] += ip[ 2 ] * dc[ 0 ];
				op[ 3 ] += ip[ 3 ] * dc[ 0 ];
				dc++;
				op += 4;
				l--;
			}
		}
		else
		if( ElCount == 3 )
		{
			while( l > 0 )
			{
				op[ 0 ] += ip[ 0 ] * dc[ 0 ];
				op[ 1 ] += ip[ 1 ] * dc[ 0 ];
				op[ 2 ] += ip[ 2 ] * dc[ 0 ];
				dc++;
				op += 3;
				l--;
			}
		}
		else
		if( ElCount == 2 )
		{
			while( l > 0 )
			{
				op[ 0 ] += ip[ 0 ] * dc[ 0 ];
				op[ 1 ] += ip[ 1 ] * dc[ 0 ];
				dc++;
				op += 2;
				l--;
			}
		}
	}

	void doFilter( const fptype* const Src, fptype* Dst,
		const int DstIncr ) const
	{
		const int ElCount = Vars -> ElCount;
		const fptype* const f = &Flt[ FltLatency ];
		const int flen = FltLatency + 1;
		const int ipstep = ElCount * ResampleFactor;
		const fptype* ip = Src - EdgePixelCount * ipstep;
		const fptype* ip1;
		const fptype* ip2;
		int l = OutLen;
		int i;

		if( ElCount == 1 )
		{
			while( l > 0 )
			{
				fptype s = f[ 0 ] * ip[ 0 ];
				ip1 = ip;
				ip2 = ip;

				for( i = 1; i < flen; i++ )
				{
					ip1++;
					ip2--;
					s += f[ i ] * ( ip1[ 0 ] + ip2[ 0 ]);
				}

				Dst[ 0 ] = s;
				Dst += DstIncr;
				ip += ipstep;
				l--;
			}
		}
		else
		if( ElCount == 4 )
		{
			while( l > 0 )
			{
				fptype s1 = f[ 0 ] * ip[ 0 ];
				fptype s2 = f[ 0 ] * ip[ 1 ];
				fptype s3 = f[ 0 ] * ip[ 2 ];
				fptype s4 = f[ 0 ] * ip[ 3 ];
				ip1 = ip;
				ip2 = ip;

				for( i = 1; i < flen; i++ )
				{
					ip1 += 4;
					ip2 -= 4;
					s1 += f[ i ] * ( ip1[ 0 ] + ip2[ 0 ]);
					s2 += f[ i ] * ( ip1[ 1 ] + ip2[ 1 ]);
					s3 += f[ i ] * ( ip1[ 2 ] + ip2[ 2 ]);
					s4 += f[ i ] * ( ip1[ 3 ] + ip2[ 3 ]);
				}

				Dst[ 0 ] = s1;
				Dst[ 1 ] = s2;
				Dst[ 2 ] = s3;
				Dst[ 3 ] = s4;
				Dst += DstIncr;
				ip += ipstep;
				l--;
			}
		}
		else
		if( ElCount == 3 )
		{
			while( l > 0 )
			{
				fptype s1 = f[ 0 ] * ip[ 0 ];
				fptype s2 = f[ 0 ] * ip[ 1 ];
				fptype s3 = f[ 0 ] * ip[ 2 ];
				ip1 = ip;
				ip2 = ip;

				for( i = 1; i < flen; i++ )
				{
					ip1 += 3;
					ip2 -= 3;
					s1 += f[ i ] * ( ip1[ 0 ] + ip2[ 0 ]);
					s2 += f[ i ] * ( ip1[ 1 ] + ip2[ 1 ]);
					s3 += f[ i ] * ( ip1[ 2 ] + ip2[ 2 ]);
				}

				Dst[ 0 ] = s1;
				Dst[ 1 ] = s2;
				Dst[ 2 ] = s3;
				Dst += DstIncr;
				ip += ipstep;
				l--;
			}
		}
		else
		if( ElCount == 2 )
		{
			while( l > 0 )
			{
				fptype s1 = f[ 0 ] * ip[ 0 ];
				fptype s2 = f[ 0 ] * ip[ 1 ];
				ip1 = ip;
				ip2 = ip;

				for( i = 1; i < flen; i++ )
				{
					ip1 += 2;
					ip2 -= 2;
					s1 += f[ i ] * ( ip1[ 0 ] + ip2[ 0 ]);
					s2 += f[ i ] * ( ip1[ 1 ] + ip2[ 1 ]);
				}

				Dst[ 0 ] = s1;
				Dst[ 1 ] = s2;
				Dst += DstIncr;
				ip += ipstep;
				l--;
			}
		}
	}

	void doResize( const fptype* SrcLine, fptype* DstLine,
		const int DstLineIncr, const fptype* const ElBiases,
		fptype* const ) const
	{
		const int IntFltLen = FltBank -> getFilterLen();
		const int ElCount = Vars -> ElCount;
		const typename CImageResizerFilterStep< fptype, fptypeatom > ::
			CResizePos* rpos = &(*RPosBuf)[ 0 ];

		const typename CImageResizerFilterStep< fptype, fptypeatom > ::
			CResizePos* const rpose = rpos + OutLen;

#define AVIR_RESIZE_PART1 \
			while( rpos < rpose ) \
			{ \
				const fptype x = (fptype) rpos -> x; \
				const fptype* const ftp = rpos -> ftp; \
				const fptype* const ftp2 = ftp + IntFltLen; \
				const fptype* Src = SrcLine + rpos -> SrcOffs; \
				int i;

#define AVIR_RESIZE_PART1nx \
			while( rpos < rpose ) \
			{ \
				const fptype* const ftp = rpos -> ftp; \
				const fptype* Src = SrcLine + rpos -> SrcOffs; \
				int i;

#define AVIR_RESIZE_PART2 \
				DstLine += DstLineIncr; \
				rpos++; \
			}

		if( FltBank -> getOrder() == 1 )
		{
			if( ElCount == 1 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					sum0 += ( ftp[ i ] + ftp2[ i ] * x ) * Src[ i ];
				}

				DstLine[ 0 ] = sum0;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 4 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];
				fptype sum3 = ElBiases[ 3 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					const fptype xx = ftp[ i ] + ftp2[ i ] * x;
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					sum3 += xx * Src[ 3 ];
					Src += 4;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;
				DstLine[ 3 ] = sum3;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 3 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					const fptype xx = ftp[ i ] + ftp2[ i ] * x;
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					Src += 3;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 2 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					const fptype xx = ftp[ i ] + ftp2[ i ] * x;
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					Src += 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;

				AVIR_RESIZE_PART2
			}
		}
		else
		{
			if( ElCount == 1 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					sum0 += ftp[ i ] * Src[ i ];
				}

				DstLine[ 0 ] = sum0;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 4 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];
				fptype sum3 = ElBiases[ 3 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					const fptype xx = ftp[ i ];
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					sum3 += xx * Src[ 3 ];
					Src += 4;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;
				DstLine[ 3 ] = sum3;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 3 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					const fptype xx = ftp[ i ];
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					Src += 3;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 2 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];

				for( i = 0; i < IntFltLen; i++ )
				{
					const fptype xx = ftp[ i ];
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					Src += 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;

				AVIR_RESIZE_PART2
			}
		}
	}
#undef AVIR_RESIZE_PART2
#undef AVIR_RESIZE_PART1nx
#undef AVIR_RESIZE_PART1

	void doResize2( const fptype* SrcLine, fptype* DstLine,
		const int DstLineIncr, const fptype* const ElBiases,
		fptype* const ) const
	{
		const int IntFltLen0 = FltBank -> getFilterLen();
		const int ElCount = Vars -> ElCount;
		const typename CImageResizerFilterStep< fptype, fptypeatom > ::
			CResizePos* rpos = &(*RPosBuf)[ 0 ];

		const typename CImageResizerFilterStep< fptype, fptypeatom > ::
			CResizePos* const rpose = rpos + OutLen;

#define AVIR_RESIZE_PART1 \
			while( rpos < rpose ) \
			{ \
				const fptype x = (fptype) rpos -> x; \
				const fptype* const ftp = rpos -> ftp; \
				const fptype* const ftp2 = ftp + IntFltLen0; \
				const fptype* Src = SrcLine + rpos -> SrcOffs; \
				const int IntFltLen = rpos -> fl; \
				int i;

#define AVIR_RESIZE_PART1nx \
			while( rpos < rpose ) \
			{ \
				const fptype* const ftp = rpos -> ftp; \
				const fptype* Src = SrcLine + rpos -> SrcOffs; \
				const int IntFltLen = rpos -> fl; \
				int i;

#define AVIR_RESIZE_PART2 \
				DstLine += DstLineIncr; \
				rpos++; \
			}

		if( FltBank -> getOrder() == 1 )
		{
			if( ElCount == 1 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					sum0 += ( ftp[ i ] + ftp2[ i ] * x ) * Src[ i ];
				}

				DstLine[ 0 ] = sum0;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 4 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];
				fptype sum3 = ElBiases[ 3 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					const fptype xx = ftp[ i ] + ftp2[ i ] * x;
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					sum3 += xx * Src[ 3 ];
					Src += 4 * 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;
				DstLine[ 3 ] = sum3;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 3 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					const fptype xx = ftp[ i ] + ftp2[ i ] * x;
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					Src += 3 * 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 2 )
			{
				AVIR_RESIZE_PART1

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					const fptype xx = ftp[ i ] + ftp2[ i ] * x;
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					Src += 2 * 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;

				AVIR_RESIZE_PART2
			}
		}
		else
		{
			if( ElCount == 1 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					sum0 += ftp[ i ] * Src[ i ];
				}

				DstLine[ 0 ] = sum0;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 4 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];
				fptype sum3 = ElBiases[ 3 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					const fptype xx = ftp[ i ];
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					sum3 += xx * Src[ 3 ];
					Src += 4 * 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;
				DstLine[ 3 ] = sum3;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 3 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];
				fptype sum2 = ElBiases[ 2 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					const fptype xx = ftp[ i ];
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					sum2 += xx * Src[ 2 ];
					Src += 3 * 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;
				DstLine[ 2 ] = sum2;

				AVIR_RESIZE_PART2
			}
			else
			if( ElCount == 2 )
			{
				AVIR_RESIZE_PART1nx

				fptype sum0 = ElBiases[ 0 ];
				fptype sum1 = ElBiases[ 1 ];

				for( i = 0; i < IntFltLen; i += 2 )
				{
					const fptype xx = ftp[ i ];
					sum0 += xx * Src[ 0 ];
					sum1 += xx * Src[ 1 ];
					Src += 2 * 2;
				}

				DstLine[ 0 ] = sum0;
				DstLine[ 1 ] = sum1;

				AVIR_RESIZE_PART2
			}
		}
	}
#undef AVIR_RESIZE_PART2
#undef AVIR_RESIZE_PART1nx
#undef AVIR_RESIZE_PART1
};

template< class fptype >
class CImageResizerDithererDefINL
{
public:

	void init( const int aLen, const CImageResizerVars& aVars,
		const double aTrMul, const double aPkOut )
	{
		Len = aLen;
		Vars = &aVars;
		LenE = aLen * Vars -> ElCount;
		TrMul0 = aTrMul;
		PkOut0 = aPkOut;
	}

	static bool isRecursive()
	{
		return( false );
	}

	void dither( fptype* const ResScanline ) const
	{
		const fptype c0 = (fptype) 0;
		const fptype PkOut = (fptype) PkOut0;
		int j;

		if( TrMul0 == 1.0 )
		{

			for( j = 0; j < LenE; j++ )
			{
				ResScanline[ j ] = clamp( round( ResScanline[ j ]), c0,
					PkOut );
			}
		}
		else
		{
			const fptype TrMul = (fptype) TrMul0;

			for( j = 0; j < LenE; j++ )
			{
				const fptype z0 = round( ResScanline[ j ] / TrMul ) * TrMul;
				ResScanline[ j ] = clamp( z0, c0, PkOut );
			}
		}
	}

protected:
	int Len;

	const CImageResizerVars* Vars;

	int LenE;

	double TrMul0;

	double PkOut0;

};

template< class fptype >
class CImageResizerDithererErrdINL :
	public CImageResizerDithererDefINL< fptype >
{
public:

	void init( const int aLen, const CImageResizerVars& aVars,
		const double aTrMul, const double aPkOut )
	{
		CImageResizerDithererDefINL< fptype > :: init( aLen, aVars, aTrMul,
			aPkOut );

		ResScanlineDith0.alloc( LenE + Vars -> ElCount, sizeof( fptype ));
		ResScanlineDith = ResScanlineDith0 + Vars -> ElCount;
		int i;

		for( i = 0; i < LenE + Vars -> ElCount; i++ )
		{
			ResScanlineDith0[ i ] = (fptype) 0;
		}
	}

	static bool isRecursive()
	{
		return( true );
	}

	void dither( fptype* const ResScanline )
	{
		const int ElCount = Vars -> ElCount;
		const fptype c0 = (fptype) 0;
		const fptype TrMul = (fptype) TrMul0;
		const fptype PkOut = (fptype) PkOut0;
		int j;

		for( j = 0; j < LenE; j++ )
		{
			ResScanline[ j ] += ResScanlineDith[ j ];
			ResScanlineDith[ j ] = (fptype) 0;
		}

		for( j = 0; j < LenE - ElCount; j++ )
		{

			const fptype z0 = round( ResScanline[ j ] / TrMul ) * TrMul;
			const fptype Noise = ResScanline[ j ] - z0;
			ResScanline[ j ] = clamp( z0, c0, PkOut );

			ResScanline[ j + ElCount ] += Noise * (fptype) 0.364842;
			ResScanlineDith[ j - ElCount ] += Noise * (fptype) 0.207305;
			ResScanlineDith[ j ] += Noise * (fptype) 0.364842;
			ResScanlineDith[ j + ElCount ] += Noise * (fptype) 0.063011;
		}

		while( j < LenE )
		{
			const fptype z0 = round( ResScanline[ j ] / TrMul ) * TrMul;
			const fptype Noise = ResScanline[ j ] - z0;
			ResScanline[ j ] = clamp( z0, c0, PkOut );

			ResScanlineDith[ j - ElCount ] += Noise * (fptype) 0.207305;
			ResScanlineDith[ j ] += Noise * (fptype) 0.364842;
			j++;
		}
	}

protected:
	using CImageResizerDithererDefINL< fptype > :: Len;
	using CImageResizerDithererDefINL< fptype > :: Vars;
	using CImageResizerDithererDefINL< fptype > :: LenE;
	using CImageResizerDithererDefINL< fptype > :: TrMul0;
	using CImageResizerDithererDefINL< fptype > :: PkOut0;

	CBuffer< fptype > ResScanlineDith0;

	fptype* ResScanlineDith;

};

template< class afptype, class afptypeatom = afptype,
	class adith = CImageResizerDithererDefINL< afptype > >
class fpclass_def
{
public:
	typedef afptype fptype;

	typedef afptypeatom fptypeatom;

	static const int fppack = sizeof( fptype ) / sizeof( fptypeatom );

	static const int fpalign = sizeof( fptype );

	static const int elalign = 1;

	static const int packmode = 0;

	typedef CImageResizerFilterStepINL< fptype, fptypeatom > CFilterStep;

	typedef adith CDitherer;

};

template< class fpclass = fpclass_def< float > >
class CImageResizer
{
	AVIR_NOCTOR( CImageResizer );

public:

	CImageResizer( const int aResBitDepth = 8, const int aSrcBitDepth = 0,
		const CImageResizerParams& aParams = CImageResizerParamsDef() )
		: Params( aParams )
		, ResBitDepth( aResBitDepth )
	{
		SrcBitDepth = ( aSrcBitDepth == 0 ? ResBitDepth : aSrcBitDepth );

		initFilterBank( FixedFilterBank, 1.0, false, CFltBuffer() );
		FixedFilterBank.createAllFilters();
	}

	template< class Tin, class Tout >
	void resizeImage( const Tin* const SrcBuf, const int SrcWidth,
		const int SrcHeight, int SrcScanlineSize, Tout* const NewBuf,
		const int NewWidth, const int NewHeight, const int ElCountIO,
		const double k, CImageResizerVars* const aVars = NULL ) const
	{
		if( SrcWidth == 0 || SrcHeight == 0 )
		{
			memset( NewBuf, 0, (size_t) NewWidth * NewHeight *
				sizeof( Tout ));

			return;
		}
		else
		if( NewWidth == 0 || NewHeight == 0 )
		{
			return;
		}

		CImageResizerVars DefVars;
		CImageResizerVars& Vars = ( aVars == NULL ? DefVars : *aVars );

		CImageResizerThreadPool DefThreadPool;
		CImageResizerThreadPool& ThreadPool = ( Vars.ThreadPool == NULL ?
			DefThreadPool : *Vars.ThreadPool );

		double kx;
		double ky;
		double ox = Vars.ox;
		double oy = Vars.oy;

		if( k == 0.0 )
		{
			kx = (double) SrcWidth / NewWidth;
			ox += ( kx - 1.0 ) * 0.5;

			ky = (double) SrcHeight / NewHeight;
			oy += ( ky - 1.0 ) * 0.5;
		}
		else
		if( k > 0.0 )
		{
			kx = k;
			ky = k;

			const double ko = ( k - 1.0 ) * 0.5;
			ox += ko;
			oy += ko;
		}
		else
		{
			kx = -k;
			ky = -k;
		}

		const bool IsInFloat = ( (Tin) 0.25 != 0 );
		const bool IsOutFloat = ( (Tout) 0.25 != 0 );
		double OutMul;

		if( Vars.UseSRGBGamma )
		{
			if( IsInFloat )
			{
				Vars.InGammaMult = 1.0;
			}
			else
			{
				Vars.InGammaMult =
					1.0 / ( sizeof( Tin ) == 1 ? 255.0 : 65535.0 );
			}

			if( IsOutFloat )
			{
				Vars.OutGammaMult = 1.0;
			}
			else
			{
				Vars.OutGammaMult = ( sizeof( Tout ) == 1 ? 255.0 : 65535.0 );
			}

			OutMul = 1.0;
		}
		else
		{
			if( IsOutFloat )
			{
				OutMul = 1.0;
			}
			else
			{
				OutMul = ( sizeof( Tout ) == 1 ? 255.0 : 65535.0 );
			}

			if( !IsInFloat )
			{
				OutMul /= ( sizeof( Tin ) == 1 ? 255.0 : 65535.0 );
			}
		}

		const int ElCount = ( ElCountIO + fpclass :: fppack - 1 ) /
			fpclass :: fppack;

		const int NewWidthE = NewWidth * ElCount;

		if( SrcScanlineSize < 1 )
		{
			SrcScanlineSize = SrcWidth * ElCountIO;
		}

		Vars.ElCount = ElCount;
		Vars.ElCountIO = ElCountIO;
		Vars.fppack = fpclass :: fppack;
		Vars.fpalign = fpclass :: fpalign;
		Vars.elalign = fpclass :: elalign;
		Vars.packmode = fpclass :: packmode;

		CDSPFracFilterBankLin< fptype > FltBank;
		CFilterSteps FltSteps;
		typename CFilterStep :: CRPosBufArray RPosBufArray;
		CBuffer< uint8_t > UsedFracMap;

		int UseBuildMode = 1;
		const int BuildModeCount =
			( FixedFilterBank.getOrder() == 0 ? 4 : 2 );

		int m;

		if( Vars.BuildMode >= 0 )
		{
			UseBuildMode = Vars.BuildMode;
		}
		else
		{
			int BestScore = 0x7FFFFFFF;

			for( m = 0; m < BuildModeCount; m++ )
			{
				CDSPFracFilterBankLin< fptype > TmpBank;
				CFilterSteps TmpSteps;
				Vars.k = kx;
				Vars.o = ox;
				buildFilterSteps( TmpSteps, Vars, TmpBank, OutMul, m, true );
				updateFilterStepBuffers( TmpSteps, Vars, RPosBufArray,
					SrcWidth, NewWidth );

				fillUsedFracMap( TmpSteps[ Vars.ResizeStep ], UsedFracMap );
				const int c = calcComplexity( TmpSteps, Vars, UsedFracMap,
					SrcHeight );

				if( c < BestScore )
				{
					UseBuildMode = m;
					BestScore = c;
				}
			}
		}

		Vars.k = kx;
		Vars.o = ox;
		buildFilterSteps( FltSteps, Vars, FltBank, OutMul, UseBuildMode,
			false );

		updateFilterStepBuffers( FltSteps, Vars, RPosBufArray, SrcWidth,
			NewWidth );

		updateBufLenAndRPosPtrs( FltSteps, Vars, NewWidth );

		const int ThreadCount = ThreadPool.getSuggestedWorkloadCount();

		CStructArray< CThreadData< Tin, Tout > > td;
		td.setItemCount( ThreadCount );
		int i;

		for( i = 0; i < ThreadCount; i++ )
		{
			if( i > 0 )
			{
				ThreadPool.addWorkload( &td[ i ]);
			}

			td[ i ].init( i, ThreadCount, FltSteps, Vars );

			td[ i ].initScanlineQueue( td[ i ].sopResizeH, SrcHeight,
				SrcWidth );
		}

		CBuffer< fptype, size_t > FltBuf( (size_t) NewWidthE * SrcHeight,
			fpclass :: fpalign );

		for( i = 0; i < SrcHeight; i++ )
		{
			td[ i % ThreadCount ].addScanlineToQueue(
				(void*) &SrcBuf[ (size_t) i * SrcScanlineSize ],
				&FltBuf[ (size_t) i * NewWidthE ]);
		}

		ThreadPool.startAllWorkloads();
		td[ 0 ].processScanlineQueue();
		ThreadPool.waitAllWorkloadsToFinish();

		const int PrevUseBuildMode = UseBuildMode;

		if( Vars.BuildMode >= 0 )
		{
			UseBuildMode = Vars.BuildMode;
		}
		else
		{
			CImageResizerVars TmpVars( Vars );
			int BestScore = 0x7FFFFFFF;

			for( m = 0; m < BuildModeCount; m++ )
			{
				CDSPFracFilterBankLin< fptype > TmpBank;
				TmpBank.copyInitParams( FltBank );
				CFilterSteps TmpSteps;
				TmpVars.k = ky;
				TmpVars.o = oy;
				buildFilterSteps( TmpSteps, TmpVars, TmpBank, 1.0, m, true );
				updateFilterStepBuffers( TmpSteps, TmpVars, RPosBufArray,
					SrcHeight, NewHeight );

				fillUsedFracMap( TmpSteps[ TmpVars.ResizeStep ],
					UsedFracMap );

				const int c = calcComplexity( TmpSteps, TmpVars, UsedFracMap,
					NewWidth );

				if( c < BestScore )
				{
					UseBuildMode = m;
					BestScore = c;
				}
			}
		}

		Vars.k = ky;
		Vars.o = oy;

		if( UseBuildMode == PrevUseBuildMode && ky == kx )
		{
			if( OutMul != 1.0 )
			{
				modifyCorrFilterDCGain( FltSteps, 1.0 / OutMul );
			}
		}
		else
		{
			buildFilterSteps( FltSteps, Vars, FltBank, 1.0, UseBuildMode,
				false );
		}

		updateFilterStepBuffers( FltSteps, Vars, RPosBufArray, SrcHeight,
			NewHeight );

		updateBufLenAndRPosPtrs( FltSteps, Vars, NewWidth );

		if( IsOutFloat && sizeof( FltBuf[ 0 ]) == sizeof( Tout ) &&
			fpclass :: packmode == 0 )
		{

			for( i = 0; i < ThreadCount; i++ )
			{
				td[ i ].initScanlineQueue( td[ i ].sopResizeV, NewWidth,
					SrcHeight, NewWidthE, NewWidthE );
			}

			for( i = 0; i < NewWidth; i++ )
			{
				td[ i % ThreadCount ].addScanlineToQueue(
					&FltBuf[ (size_t) i * ElCount ],
					(fptype*) &NewBuf[ (size_t) i * ElCount ]);
			}

			ThreadPool.startAllWorkloads();
			td[ 0 ].processScanlineQueue();
			ThreadPool.waitAllWorkloadsToFinish();
			ThreadPool.removeAllWorkloads();

			return;
		}

		CBuffer< fptype, size_t > ResBuf( (size_t) NewWidthE * NewHeight,
			fpclass :: fpalign );

		for( i = 0; i < ThreadCount; i++ )
		{
			td[ i ].initScanlineQueue( td[ i ].sopResizeV, NewWidth,
				SrcHeight, NewWidthE, NewWidthE );
		}

		const int im = ( fpclass :: packmode == 0 ? ElCount : 1 );

		for( i = 0; i < NewWidth; i++ )
		{
			td[ i % ThreadCount ].addScanlineToQueue(
				&FltBuf[ (size_t) i * im ], &ResBuf[ (size_t) i * im ]);
		}

		ThreadPool.startAllWorkloads();
		td[ 0 ].processScanlineQueue();
		ThreadPool.waitAllWorkloadsToFinish();

		if( IsOutFloat )
		{

			for( i = 0; i < ThreadCount; i++ )
			{
				td[ i ].initScanlineQueue( td[ i ].sopUnpackH,
					NewHeight, NewWidth );
			}

			for( i = 0; i < NewHeight; i++ )
			{
				td[ i % ThreadCount ].addScanlineToQueue(
					&ResBuf[ (size_t) i * NewWidthE ],
					&NewBuf[ (size_t) i * NewWidth * ElCountIO ]);
			}

			ThreadPool.startAllWorkloads();
			td[ 0 ].processScanlineQueue();
			ThreadPool.waitAllWorkloadsToFinish();
			ThreadPool.removeAllWorkloads();

			return;
		}

		int TruncBits;
		int OutRange;

		if( sizeof( Tout ) == 1 )
		{
			TruncBits = 8 - ResBitDepth;
			OutRange = 255;
		}
		else
		{
			TruncBits = 16 - ResBitDepth;
			OutRange = 65535;
		}

		const double PkOut = OutRange;
		const double TrMul = ( TruncBits > 0 ?
			PkOut / ( OutRange >> TruncBits ) : 1.0 );

		if( CDitherer :: isRecursive() )
		{
			td[ 0 ].getDitherer().init( NewWidth, Vars, TrMul, PkOut );

			if( Vars.UseSRGBGamma )
			{
				for( i = 0; i < NewHeight; i++ )
				{
					fptype* const ResScanline =
						&ResBuf[ (size_t) i * NewWidthE ];

					CFilterStep :: applySRGBGamma( ResScanline, NewWidth,
						Vars );

					td[ 0 ].getDitherer().dither( ResScanline );

					CFilterStep :: unpackScanline( ResScanline,
						&NewBuf[ (size_t) i * NewWidth * ElCountIO ],
						NewWidth, Vars );
				}
			}
			else
			{
				for( i = 0; i < NewHeight; i++ )
				{
					fptype* const ResScanline =
						&ResBuf[ (size_t) i * NewWidthE ];

					td[ 0 ].getDitherer().dither( ResScanline );

					CFilterStep :: unpackScanline( ResScanline,
						&NewBuf[ (size_t) i * NewWidth * ElCountIO ],
						NewWidth, Vars );
				}
			}
		}
		else
		{
			for( i = 0; i < ThreadCount; i++ )
			{
				td[ i ].initScanlineQueue( td[ i ].sopDitherAndUnpackH,
					NewHeight, NewWidth );

				td[ i ].getDitherer().init( NewWidth, Vars, TrMul, PkOut );
			}

			for( i = 0; i < NewHeight; i++ )
			{
				td[ i % ThreadCount ].addScanlineToQueue(
					&ResBuf[ (size_t) i * NewWidthE ],
					&NewBuf[ (size_t) i * NewWidth * ElCountIO ]);
			}

			ThreadPool.startAllWorkloads();
			td[ 0 ].processScanlineQueue();
			ThreadPool.waitAllWorkloadsToFinish();
		}

		ThreadPool.removeAllWorkloads();
	}

private:
	typedef typename fpclass :: fptype fptype;

	typedef typename fpclass :: CFilterStep CFilterStep;

	typedef typename fpclass :: CDitherer CDitherer;

	CImageResizerParams Params;

	int SrcBitDepth;

	int ResBitDepth;

	CDSPFracFilterBankLin< fptype > FixedFilterBank;

	typedef CStructArray< CFilterStep > CFilterSteps;

	void initFilterBank( CDSPFracFilterBankLin< fptype >& FltBank,
		const double CutoffMult, const bool ForceHiOrder,
		const CFltBuffer& ExtFilter ) const
	{
		const int IntBitDepth = ( ResBitDepth > SrcBitDepth ? ResBitDepth :
			SrcBitDepth );

		const double SNR = -6.02 * ( IntBitDepth + 3 );
		int UseOrder;
		int FracCount;

		if( ForceHiOrder || IntBitDepth > 8 )
		{
			UseOrder = 1;
			FracCount = (int) ceil( 0.23134052 * exp( -0.058062929 * SNR ));
		}
		else
		{
			UseOrder = 0;
			FracCount = (int) ceil( 0.33287686 * exp( -0.11334583 * SNR ));
		}

		if( FracCount < 2 )
		{
			FracCount = 2;
		}

		FltBank.init( FracCount, UseOrder, Params.IntFltLen / CutoffMult,
			Params.IntFltCutoff * CutoffMult, Params.IntFltAlpha, ExtFilter,
			fpclass :: fpalign, fpclass :: elalign );
	}

	static void allocFilter( CBuffer< fptype >& Flt, const int ReqCapacity,
		const bool IsModel = false, int* const FltExt = NULL )
	{
		int UseCapacity = ( ReqCapacity + fpclass :: elalign - 1 ) &
			~( fpclass :: elalign - 1 );

		int Ext = UseCapacity - ReqCapacity;

		if( FltExt != NULL )
		{
			*FltExt = Ext;
		}

		if( IsModel )
		{
			Flt.forceCapacity( UseCapacity );
			return;
		}

		Flt.alloc( UseCapacity, fpclass :: fpalign );

		while( Ext > 0 )
		{
			Ext--;
			Flt[ ReqCapacity + Ext ] = (fptype) 0;
		}
	}

	void assignFilterParams( CFilterStep& fs, const bool IsUpsample,
		const int ResampleFactor, const double FltCutoff, const double DCGain,
		const bool UseFltOrig, const bool IsModel ) const
	{
		double FltAlpha;
		double Len2;
		double Freq;

		if( FltCutoff == 0.0 )
		{
			const double m = 2.0 / ResampleFactor;
			FltAlpha = Params.HBFltAlpha;
			Len2 = 0.5 * Params.HBFltLen / m;
			Freq = AVIR_PI * Params.HBFltCutoff * m;
		}
		else
		{
			FltAlpha = Params.LPFltAlpha;
			Len2 = 0.25 * Params.LPFltBaseLen / FltCutoff;
			Freq = AVIR_PI * Params.LPFltCutoffMult * FltCutoff;
		}

		if( IsUpsample )
		{
			Len2 *= ResampleFactor;
			Freq /= ResampleFactor;
			fs.DCGain = DCGain * ResampleFactor;
		}
		else
		{
			fs.DCGain = DCGain;
		}

		fs.FltOrig.Len2 = Len2;
		fs.FltOrig.Freq = Freq;
		fs.FltOrig.Alpha = FltAlpha;
		fs.FltOrig.DCGain = fs.DCGain;

		CDSPPeakedCosineLPF w( Len2, Freq, FltAlpha );

		fs.IsUpsample = IsUpsample;
		fs.ResampleFactor = ResampleFactor;
		fs.FltLatency = w.fl2;

		int FltExt;

		if( IsModel )
		{
			allocFilter( fs.Flt, w.FilterLen, true, &FltExt );

			if( UseFltOrig )
			{

				fs.FltOrig.alloc( w.FilterLen );
				memset( &fs.FltOrig[ 0 ], 0,
					w.FilterLen * sizeof( fs.FltOrig[ 0 ]));
			}
		}
		else
		{
			fs.FltOrig.alloc( w.FilterLen );

			w.generateLPF( &fs.FltOrig[ 0 ], fs.DCGain );

			allocFilter( fs.Flt, fs.FltOrig.getCapacity(), false, &FltExt );
			copyArray( &fs.FltOrig[ 0 ], &fs.Flt[ 0 ],
				fs.FltOrig.getCapacity() );

			if( !UseFltOrig )
			{
				fs.FltOrig.free();
			}
		}

		if( IsUpsample )
		{
			int l = fs.Flt.getCapacity() - fs.FltLatency - ResampleFactor -
				FltExt;

			allocFilter( fs.PrefixDC, l, IsModel );
			allocFilter( fs.SuffixDC, fs.FltLatency, IsModel );

			if( IsModel )
			{
				return;
			}

			const fptype* ip = &fs.Flt[ fs.FltLatency + ResampleFactor ];
			copyArray( ip, &fs.PrefixDC[ 0 ], l );

			while( true )
			{
				ip += ResampleFactor;
				l -= ResampleFactor;

				if( l <= 0 )
				{
					break;
				}

				addArray( ip, &fs.PrefixDC[ 0 ], l );
			}

			l = fs.FltLatency;
			fptype* op = &fs.SuffixDC[ 0 ];
			copyArray( &fs.Flt[ 0 ], op, l );

			while( true )
			{
				op += ResampleFactor;
				l -= ResampleFactor;

				if( l <= 0 )
				{
					break;
				}

				addArray( &fs.Flt[ 0 ], op, l );
			}
		}
		else
		if( !UseFltOrig )
		{
			fs.EdgePixelCount = fs.EdgePixelCountDef;
		}
	}

	void addCorrectionFilter( CFilterSteps& Steps, const double bw,
		const bool IsPreCorrection, const bool IsModel ) const
	{
		CFilterStep& fs = ( IsPreCorrection ? Steps[ 0 ] : Steps.add() );
		fs.IsUpsample = false;
		fs.ResampleFactor = 1;
		fs.DCGain = 1.0;
		fs.EdgePixelCount = ( IsPreCorrection ? fs.EdgePixelCountDef : 0 );

		if( IsModel )
		{
			allocFilter( fs.Flt, CDSPFIREQ :: calcFilterLength(
				Params.CorrFltLen, fs.FltLatency ), true );

			return;
		}

		const int BinCount = 65;
		const int BinCount1 = BinCount - 1;
		double curbw = 1.0;
		int i;
		int j;
		double re;
		double im;

		CBuffer< double > Bins( BinCount );

		for( j = 0; j < BinCount; j++ )
		{
			Bins[ j ] = 1.0;
		}

		const int si = ( IsPreCorrection ? 1 : 0 );

		for( i = si; i < Steps.getItemCount() - ( si ^ 1 ); i++ )
		{
			const CFilterStep& fs = Steps[ i ];

			if( fs.IsUpsample )
			{
				curbw *= fs.ResampleFactor;

				if( fs.FltOrig.getCapacity() > 0 )
				{
					continue;
				}
			}

			const fptype* Flt;
			int FltLen;

			if( fs.ResampleFactor == 0 )
			{
				Flt = fs.FltBank -> getFilter( 0 );
				FltLen = fs.FltBank -> getFilterLen();
			}
			else
			{
				Flt = &fs.Flt[ 0 ];
				FltLen = fs.Flt.getCapacity();
			}

			const double thm = AVIR_PI * bw / ( curbw * BinCount1 );

			for( j = 0; j < BinCount; j++ )
			{
				calcFIRFilterResponse( Flt, FltLen, j * thm, re, im );

				Bins[ j ] *= fs.DCGain / sqrt( re * re + im * im );
			}

			if( !fs.IsUpsample && fs.ResampleFactor > 1 )
			{
				curbw /= fs.ResampleFactor;
			}
		}

		CDSPFIREQ EQ;
		EQ.init( bw * 2.0, Params.CorrFltLen, BinCount, 0.0, bw, false,
			Params.CorrFltAlpha );

		fs.FltLatency = EQ.getFilterLatency();

		CBuffer< double > Filter( EQ.getFilterLength() );
		EQ.buildFilter( Bins, &Filter[ 0 ]);
		normalizeFIRFilter( &Filter[ 0 ], Filter.getCapacity(), 1.0 );

		allocFilter( fs.Flt, Filter.getCapacity() );
		copyArray( &Filter[ 0 ], &fs.Flt[ 0 ], Filter.getCapacity() );

	}

	static void addSharpenTest( CFilterSteps& Steps, const double bw,
		const bool IsModel )
	{
		if( bw <= 1.0 )
		{
			return;
		}

		const double FltLen = 10.0 * bw;

		CFilterStep& fs = Steps.add();
		fs.IsUpsample = false;
		fs.ResampleFactor = 1;
		fs.DCGain = 1.0;
		fs.EdgePixelCount = 0;

		if( IsModel )
		{
			allocFilter( fs.Flt, CDSPFIREQ :: calcFilterLength( FltLen,
				fs.FltLatency ), true );

			return;
		}

		const int BinCount = 200;
		CBuffer< double > Bins( BinCount );
		int Thresh = (int) round( BinCount / bw * 1.75 );

		if( Thresh > BinCount )
		{
			Thresh = BinCount;
		}

		int j;

		for( j = 0; j < Thresh; j++ )
		{
			Bins[ j ] = 1.0;
		}

		for( j = Thresh; j < BinCount; j++ )
		{
			Bins[ j ] = 256.0;
		}

		CDSPFIREQ EQ;
		EQ.init( bw * 2.0, FltLen, BinCount, 0.0, bw, false, 1.7 );

		fs.FltLatency = EQ.getFilterLatency();

		CBuffer< double > Filter( EQ.getFilterLength() );
		EQ.buildFilter( Bins, &Filter[ 0 ]);
		normalizeFIRFilter( &Filter[ 0 ], Filter.getCapacity(), 1.0 );

		allocFilter( fs.Flt, Filter.getCapacity() );
		copyArray( &Filter[ 0 ], &fs.Flt[ 0 ], Filter.getCapacity() );

	}

	void buildFilterSteps( CFilterSteps& Steps, CImageResizerVars& Vars,
		CDSPFracFilterBankLin< fptype >& FltBank, const double DCGain,
		const int ModeFlags, const bool IsModel ) const
	{
		Steps.clear();

		const bool DoFltAndIntCombo = (( ModeFlags & 1 ) != 0 );

		const bool ForceHiOrderInt = (( ModeFlags & 2 ) != 0 );

		const bool UseHalfband = (( ModeFlags & 4 ) != 0 );

		const double bw = 1.0 / Vars.k;
		const int UpsampleFactor = ( (int) floor( Vars.k ) < 2 ? 2 : 1 );
		double IntCutoffMult;
		CFilterStep* ReuseStep;

		CFilterStep* ExtFltStep;

		bool IsPreCorrection;

		double FltCutoff;
		double corrbw;

		if( Vars.k <= 1.0 )
		{
			IsPreCorrection = true;
			FltCutoff = 1.0;
			corrbw = 1.0;
			Steps.add();
		}
		else
		{
			IsPreCorrection = false;
			FltCutoff = bw;
			corrbw = bw;
		}

		if( UpsampleFactor > 1 )
		{
			CFilterStep& fs = Steps.add();
			assignFilterParams( fs, true, UpsampleFactor, FltCutoff, DCGain,
				DoFltAndIntCombo, IsModel );

			IntCutoffMult = FltCutoff * 2.0 / UpsampleFactor;
			ReuseStep = NULL;
			ExtFltStep = ( DoFltAndIntCombo ? &fs : NULL );
		}
		else
		{
			int DownsampleFactor;

			while( true )
			{
				DownsampleFactor = (int) floor( 0.5 / FltCutoff );
				bool DoHBFltAdd = ( UseHalfband && DownsampleFactor > 1 );

				if( DoHBFltAdd )
				{
					assignFilterParams( Steps.add(), false, DownsampleFactor,
						0.0, 1.0, false, IsModel );

					FltCutoff *= DownsampleFactor;
				}
				else
				{
					if( DownsampleFactor < 1 )
					{
						DownsampleFactor = 1;
					}

					break;
				}
			}

			CFilterStep& fs = Steps.add();
			assignFilterParams( fs, false, DownsampleFactor, FltCutoff,
				DCGain, DoFltAndIntCombo, IsModel );

			IntCutoffMult = FltCutoff / 0.5;

			if( DoFltAndIntCombo )
			{
				ReuseStep = &fs;
				ExtFltStep = &fs;
			}
			else
			{
				IntCutoffMult *= DownsampleFactor;
				ReuseStep = NULL;
				ExtFltStep = NULL;
			}
		}

		CFilterStep& fs = ( ReuseStep == NULL ? Steps.add() : *ReuseStep );

		Vars.ResizeStep = Steps.getItemCount() - 1;
		fs.IsUpsample = false;
		fs.ResampleFactor = 0;
		fs.DCGain = ( ExtFltStep == NULL ? 1.0 : ExtFltStep -> DCGain );

		initFilterBank( FltBank, IntCutoffMult, ForceHiOrderInt,
			( ExtFltStep == NULL ? fs.FltOrig : ExtFltStep -> FltOrig ));

		if( FltBank == FixedFilterBank )
		{
			fs.FltBank = (CDSPFracFilterBankLin< fptype >*) &FixedFilterBank;
		}
		else
		{
			fs.FltBank = &FltBank;
		}

		addCorrectionFilter( Steps, corrbw, IsPreCorrection, IsModel );

	}

	static void extendUpsample( CFilterStep& fs, CFilterStep& NextStep )
	{
		fs.InPrefix = ( NextStep.InPrefix + fs.ResampleFactor - 1 ) /
			fs.ResampleFactor;

		fs.OutPrefix += fs.InPrefix * fs.ResampleFactor;
		NextStep.InPrefix = 0;

		fs.InSuffix = ( NextStep.InSuffix + fs.ResampleFactor - 1 ) /
			fs.ResampleFactor;

		fs.OutSuffix += fs.InSuffix * fs.ResampleFactor;
		NextStep.InSuffix = 0;
	}

	static void fillRPosBuf( CFilterStep& fs, const CImageResizerVars& Vars )
	{
		const int PrevLen = fs.RPosBuf -> getCapacity();

		if( fs.OutLen > PrevLen )
		{
			fs.RPosBuf -> increaseCapacity( fs.OutLen );
		}

		typename CFilterStep :: CResizePos* rpos = &(*fs.RPosBuf)[ PrevLen ];
		const int FracCount = fs.FltBank -> getFracCount();
		const double o = Vars.o;
		const double k = Vars.k;
		int i;

		for( i = PrevLen; i < fs.OutLen; i++ )
		{
			const double SrcPos = o + k * i;
			const int SrcPosInt = (int) floor( SrcPos );
			const double x = ( SrcPos - SrcPosInt ) * FracCount;
			const int fti = (int) x;
			rpos -> x = (typename fpclass :: fptypeatom) ( x - fti );
			rpos -> fti = fti;
			rpos -> SrcPosInt = SrcPosInt;
			rpos++;
		}
	}

	static void updateFilterStepBuffers( CFilterSteps& Steps,
		CImageResizerVars& Vars,
		typename CFilterStep :: CRPosBufArray& RPosBufArray, int SrcLen,
		const int NewLen )
	{
		int upstep = -1;
		int InBuf = 0;
		int i;

		for( i = 0; i < Steps.getItemCount(); i++ )
		{
			CFilterStep& fs = Steps[ i ];

			fs.Vars = &Vars;
			fs.InLen = SrcLen;
			fs.InBuf = InBuf;
			fs.OutBuf = ( InBuf + 1 ) & 1;

			if( fs.IsUpsample )
			{
				upstep = i;
				Vars.k *= fs.ResampleFactor;
				Vars.o *= fs.ResampleFactor;
				fs.InPrefix = 0;
				fs.InSuffix = 0;
				fs.OutLen = fs.InLen * fs.ResampleFactor;
				fs.OutPrefix = fs.FltLatency;
				fs.OutSuffix = fs.Flt.getCapacity() - fs.FltLatency -
					fs.ResampleFactor;

				int l0 = fs.OutPrefix + fs.OutLen + fs.OutSuffix;
				int l = fs.InLen * fs.ResampleFactor +
					fs.SuffixDC.getCapacity();

				if( l > l0 )
				{
					fs.OutSuffix += l - l0;
				}

				l0 = fs.OutLen + fs.OutSuffix;

				if( fs.PrefixDC.getCapacity() > l0 )
				{
					fs.OutSuffix += fs.PrefixDC.getCapacity() - l0;
				}
			}
			else
			if( fs.ResampleFactor == 0 )
			{
				const int FilterLenD2 = fs.FltBank -> getFilterLen() / 2;
				const int FilterLenD21 = FilterLenD2 - 1;

				const int ResizeLPix = (int) floor( Vars.o ) - FilterLenD21;
				fs.InPrefix = ( ResizeLPix < 0 ? -ResizeLPix : 0 );
				const int ResizeRPix = (int) floor( Vars.o +
					( NewLen - 1 ) * Vars.k ) + FilterLenD2 + 1;

				fs.InSuffix = ( ResizeRPix > fs.InLen ?
					ResizeRPix - fs.InLen : 0 );

				fs.OutLen = NewLen;
				fs.RPosBuf = &RPosBufArray.getRPosBuf( Vars.k, Vars.o,
					fs.FltBank -> getFracCount() );

				fillRPosBuf( fs, Vars );
			}
			else
			{
				Vars.k /= fs.ResampleFactor;
				Vars.o /= fs.ResampleFactor;
				Vars.o += fs.EdgePixelCount;

				fs.InPrefix = fs.FltLatency;
				fs.InSuffix = fs.Flt.getCapacity() - fs.FltLatency - 1;

				fs.OutLen = ( fs.InLen + fs.ResampleFactor - 1 ) /
					fs.ResampleFactor + fs.EdgePixelCount;

				fs.InSuffix += ( fs.OutLen - 1 ) * fs.ResampleFactor + 1 -
					fs.InLen;

				fs.InPrefix += fs.EdgePixelCount * fs.ResampleFactor;
				fs.OutLen += fs.EdgePixelCount;
			}

			InBuf = fs.OutBuf;
			SrcLen = fs.OutLen;
		}

		Steps[ Steps.getItemCount() - 1 ].OutBuf = 2;
		Vars.IsResize2 = false;

		if( upstep != -1 )
		{
			extendUpsample( Steps[ upstep ], Steps[ upstep + 1 ]);

			if( Steps[ upstep ].ResampleFactor == 2 &&
				Vars.ResizeStep == upstep + 1 &&
				fpclass :: packmode == 0 &&
				Steps[ upstep ].FltOrig.getCapacity() > 0 )
			{

				Vars.IsResize2 = true;
			}
		}
	}

	static void updateBufLenAndRPosPtrs( CFilterSteps& Steps,
		CImageResizerVars& Vars, const int ResElIncr )
	{
		int MaxPrefix[ 2 ] = { 0, 0 };
		int MaxLen[ 2 ] = { 0, 0 };
		int i;

		for( i = 0; i < Steps.getItemCount(); i++ )
		{
			CFilterStep& fs = Steps[ i ];
			const int ib = fs.InBuf;

			if( fs.InPrefix > MaxPrefix[ ib ])
			{
				MaxPrefix[ ib ] = fs.InPrefix;
			}

			int l = fs.InLen + fs.InSuffix;

			if( l > MaxLen[ ib ])
			{
				MaxLen[ ib ] = l;
			}

			fs.InElIncr = fs.InPrefix + l;

			if( fs.OutBuf == 2 )
			{
				break;
			}

			const int ob = fs.OutBuf;

			if( fs.IsUpsample )
			{
				if( fs.OutPrefix > MaxPrefix[ ob ])
				{
					MaxPrefix[ ob ] = fs.OutPrefix;
				}

				l = fs.OutLen + fs.OutSuffix;

				if( l > MaxLen[ ob ])
				{
					MaxLen[ ob ] = l;
				}
			}
			else
			{
				if( fs.OutLen > MaxLen[ ob ])
				{
					MaxLen[ ob ] = fs.OutLen;
				}
			}
		}

		for( i = 0; i < Steps.getItemCount(); i++ )
		{
			CFilterStep& fs = Steps[ i ];

			if( fs.OutBuf == 2 )
			{
				fs.OutElIncr = ResElIncr;
				break;
			}

			CFilterStep& fs2 = Steps[ i + 1 ];

			if( fs.IsUpsample )
			{
				fs.OutElIncr = fs.OutPrefix + fs.OutLen + fs.OutSuffix;

				if( fs.OutElIncr > fs2.InElIncr )
				{
					fs2.InElIncr = fs.OutElIncr;
				}
				else
				{
					fs.OutElIncr = fs2.InElIncr;
				}
			}
			else
			{
				fs.OutElIncr = fs2.InElIncr;
			}
		}

		for( i = 0; i < 2; i++ )
		{
			Vars.BufLen[ i ] = MaxPrefix[ i ] + MaxLen[ i ];
			Vars.BufOffs[ i ] = MaxPrefix[ i ];

			if( Vars.packmode == 0 )
			{
				Vars.BufOffs[ i ] *= Vars.ElCount;
			}

			Vars.BufLen[ i ] *= Vars.ElCount;
		}

		CFilterStep& fs = Steps[ Vars.ResizeStep ];
		typename CFilterStep :: CResizePos* rpos = &(*fs.RPosBuf)[ 0 ];
		const int em = ( fpclass :: packmode == 0 ? Vars.ElCount : 1 );
		const int fl = fs.FltBank -> getFilterLen();
		const int FilterLenD21 = fl / 2 - 1;

		if( Vars.IsResize2 )
		{
			for( i = 0; i < fs.OutLen; i++ )
			{
				const int p = rpos -> SrcPosInt - FilterLenD21;
				const int fo = p & 1;
				rpos -> SrcOffs = ( p + fo ) * em;
				rpos -> ftp = fs.FltBank -> getFilter( rpos -> fti ) + fo;
				rpos -> fl = fl - fo;
				rpos++;
			}
		}
		else
		{
			for( i = 0; i < fs.OutLen; i++ )
			{
				rpos -> SrcOffs = ( rpos -> SrcPosInt - FilterLenD21 ) * em;
				rpos -> ftp = fs.FltBank -> getFilter( rpos -> fti );
				rpos++;
			}
		}
	}

	void modifyCorrFilterDCGain( CFilterSteps& Steps, const double m ) const
	{
		CBuffer< fptype >* Flt;
		const int z = Steps.getItemCount() - 1;

		if( !Steps[ z ].IsUpsample && Steps[ z ].ResampleFactor == 1 )
		{
			Flt = &Steps[ z ].Flt;
		}
		else
		{
			Flt = &Steps[ 0 ].Flt;
		}

		int i;

		for( i = 0; i < Flt -> getCapacity(); i++ )
		{
			(*Flt)[ i ] = (fptype) ( (double) (*Flt)[ i ] * m );
		}
	}

	static void fillUsedFracMap( const CFilterStep& fs,
		CBuffer< uint8_t >& UsedFracMap )
	{
		const int FracCount = fs.FltBank -> getFracCount();
		UsedFracMap.increaseCapacity( FracCount, false );
		memset( &UsedFracMap[ 0 ], 0, FracCount * sizeof( UsedFracMap[ 0 ]));

		typename CFilterStep :: CResizePos* rpos = &(*fs.RPosBuf)[ 0 ];
		int i;

		for( i = 0; i < fs.OutLen; i++ )
		{
			UsedFracMap[ rpos -> fti ] |= 1;
			rpos++;
		}
	}

	static int calcComplexity( const CFilterSteps& Steps,
		const CImageResizerVars& Vars, const CBuffer< uint8_t >& UsedFracMap,
		const int ScanlineCount )
	{
		int fcnum;
		int fcdenom;

		if( Vars.packmode != 0 )
		{
			fcnum = 1;
			fcdenom = 1;
		}
		else
		{

			fcnum = 3;
			fcdenom = 4;
		}

		int s = 0;
		int s2 = 0;
		int i;

		for( i = 0; i < Steps.getItemCount(); i++ )
		{
			const CFilterStep& fs = Steps[ i ];

			s2 += 65 * fs.Flt.getCapacity();

			if( fs.IsUpsample )
			{
				if( fs.FltOrig.getCapacity() > 0 )
				{
					continue;
				}

				s += ( fs.Flt.getCapacity() *
					( fs.InPrefix + fs.InLen + fs.InSuffix ) +
					fs.SuffixDC.getCapacity() + fs.PrefixDC.getCapacity() ) *
					Vars.ElCount;
			}
			else
			if( fs.ResampleFactor == 0 )
			{
				s += fs.FltBank -> getFilterLen() *
					( fs.FltBank -> getOrder() + Vars.ElCount ) * fs.OutLen;

				if( i == Vars.ResizeStep && Vars.IsResize2 )
				{
					s >>= 1;
				}

				s2 += fs.FltBank -> calcInitComplexity( UsedFracMap );
			}
			else
			{
				s += fs.Flt.getCapacity() * Vars.ElCount * fs.OutLen *
					fcnum / fcdenom;
			}
		}

		return( s + s2 / ScanlineCount );
	}

	template< class Tin, class Tout >
	class CThreadData : public CImageResizerThreadPool :: CWorkload
	{
	public:
		virtual void process()
		{
			processScanlineQueue();
		}

		enum EScanlineOperation
		{
			sopResizeH,

			sopResizeV,

			sopDitherAndUnpackH,

			sopUnpackH

		};

		void init( const int aThreadIndex, const int aThreadCount,
			const CFilterSteps& aSteps, const CImageResizerVars& aVars )
		{
			ThreadIndex = aThreadIndex;
			ThreadCount = aThreadCount;
			Steps = &aSteps;
			Vars = &aVars;
		}

		void initScanlineQueue( const EScanlineOperation aOp,
			const int TotalLines, const int aSrcLen, const int aSrcIncr = 0,
			const int aResIncr = 0 )
		{
			const int l = Vars -> BufLen[ 0 ] + Vars -> BufLen[ 1 ];

			if( Bufs.getCapacity() < l )
			{
				Bufs.alloc( l, fpclass :: fpalign );
			}

			BufPtrs[ 0 ] = Bufs + Vars -> BufOffs[ 0 ];
			BufPtrs[ 1 ] = Bufs + Vars -> BufLen[ 0 ] + Vars -> BufOffs[ 1 ];

			int j;
			int ml = 0;

			for( j = 0; j < Steps -> getItemCount(); j++ )
			{
				const CFilterStep& fs = (*Steps)[ j ];

				if( fs.ResampleFactor == 0 &&
					ml < fs.FltBank -> getFilterLen() )
				{
					ml = fs.FltBank -> getFilterLen();
				}
			}

			TmpFltBuf.alloc( ml, fpclass :: fpalign );
			ScanlineOp = aOp;
			SrcLen = aSrcLen;
			SrcIncr = aSrcIncr;
			ResIncr = aResIncr;
			QueueLen = 0;
			Queue.increaseCapacity(( TotalLines + ThreadCount - 1 ) /
				ThreadCount, false );
		}

		void addScanlineToQueue( void* const SrcBuf, void* const ResBuf )
		{
			Queue[ QueueLen ].SrcBuf = SrcBuf;
			Queue[ QueueLen ].ResBuf = ResBuf;
			QueueLen++;
		}

		void processScanlineQueue()
		{
			int i;

			switch( ScanlineOp )
			{
				case sopResizeH:
				{
					for( i = 0; i < QueueLen; i++ )
					{
						resizeScanlineH( (Tin*) Queue[ i ].SrcBuf,
							(fptype*) Queue[ i ].ResBuf );
					}

					break;
				}

				case sopResizeV:
				{
					for( i = 0; i < QueueLen; i++ )
					{
						resizeScanlineV( (fptype*) Queue[ i ].SrcBuf,
							(fptype*) Queue[ i ].ResBuf );
					}

					break;
				}

				case sopDitherAndUnpackH:
				{
					if( Vars -> UseSRGBGamma )
					{
						for( i = 0; i < QueueLen; i++ )
						{
							CFilterStep :: applySRGBGamma(
								(fptype*) Queue[ i ].SrcBuf, SrcLen, *Vars );

							Ditherer.dither( (fptype*) Queue[ i ].SrcBuf );

							CFilterStep :: unpackScanline(
								(fptype*) Queue[ i ].SrcBuf,
								(Tout*) Queue[ i ].ResBuf, SrcLen, *Vars );
						}
					}
					else
					{
						for( i = 0; i < QueueLen; i++ )
						{
							Ditherer.dither( (fptype*) Queue[ i ].SrcBuf );

							CFilterStep :: unpackScanline(
								(fptype*) Queue[ i ].SrcBuf,
								(Tout*) Queue[ i ].ResBuf, SrcLen, *Vars );
						}
					}

					break;
				}

				case sopUnpackH:
				{
					if( Vars -> UseSRGBGamma )
					{
						for( i = 0; i < QueueLen; i++ )
						{
							CFilterStep :: applySRGBGamma(
								(fptype*) Queue[ i ].SrcBuf, SrcLen, *Vars );

							CFilterStep :: unpackScanline(
								(fptype*) Queue[ i ].SrcBuf,
								(Tout*) Queue[ i ].ResBuf, SrcLen, *Vars );
						}
					}
					else
					{
						for( i = 0; i < QueueLen; i++ )
						{
							CFilterStep :: unpackScanline(
								(fptype*) Queue[ i ].SrcBuf,
								(Tout*) Queue[ i ].ResBuf, SrcLen, *Vars );
						}
					}

					break;
				}
			}
		}

		CDitherer& getDitherer()
		{
			return( Ditherer );
		}

	private:
		int ThreadIndex;

		int ThreadCount;

		const CFilterSteps* Steps;

		const CImageResizerVars* Vars;

		CBuffer< fptype > Bufs;

		fptype* BufPtrs[ 3 ];

		CBuffer< fptype > TmpFltBuf;

		EScanlineOperation ScanlineOp;

		int SrcLen;

		int SrcIncr;

		int ResIncr;

		CDitherer Ditherer;

		struct CQueueItem
		{
			void* SrcBuf;

			void* ResBuf;

		};

		CBuffer< CQueueItem > Queue;

		int QueueLen;

		void resizeScanlineH( const Tin* const SrcBuf, fptype* const ResBuf )
		{
			const CFilterStep& fs0 = (*Steps)[ 0 ];

			fs0.packScanline( SrcBuf, BufPtrs[ 0 ], SrcLen );
			BufPtrs[ 2 ] = ResBuf;

			fptype ElBiases[ 4 ];
			fs0.calcScanlineBias( BufPtrs[ 0 ], SrcLen, ElBiases );
			fs0.unbiasScanline( BufPtrs[ 0 ], SrcLen, ElBiases );

			int j;

			for( j = 0; j < Steps -> getItemCount(); j++ )
			{
				const CFilterStep& fs = (*Steps)[ j ];
				fs.prepareInBuf( BufPtrs[ fs.InBuf ]);
				const int DstIncr =
					( Vars -> packmode == 0 ? Vars -> ElCount : 1 );

				if( fs.ResampleFactor != 0 )
				{
					if( fs.IsUpsample )
					{
						fs.doUpsample( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ]);
					}
					else
					{
						fs.doFilter( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ], DstIncr );
					}
				}
				else
				{
					if( Vars -> IsResize2 )
					{
						fs.doResize2( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ], DstIncr, ElBiases,
							TmpFltBuf );
					}
					else
					{
						fs.doResize( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ], DstIncr, ElBiases,
							TmpFltBuf );
					}
				}
			}
		}

		void resizeScanlineV( const fptype* const SrcBuf,
			fptype* const ResBuf )
		{
			const CFilterStep& fs0 = (*Steps)[ 0 ];

			fs0.convertVtoH( SrcBuf, BufPtrs[ 0 ], SrcLen, SrcIncr );
			BufPtrs[ 2 ] = ResBuf;

			fptype ElBiases[ 4 ];
			fs0.calcScanlineBias( BufPtrs[ 0 ], SrcLen, ElBiases );
			fs0.unbiasScanline( BufPtrs[ 0 ], SrcLen, ElBiases );

			int j;

			for( j = 0; j < Steps -> getItemCount(); j++ )
			{
				const CFilterStep& fs = (*Steps)[ j ];
				fs.prepareInBuf( BufPtrs[ fs.InBuf ]);
				const int DstIncr = ( fs.OutBuf == 2 ? ResIncr :
					( Vars -> packmode == 0 ? Vars -> ElCount : 1 ));

				if( fs.ResampleFactor != 0 )
				{
					if( fs.IsUpsample )
					{
						fs.doUpsample( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ]);
					}
					else
					{
						fs.doFilter( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ], DstIncr );
					}
				}
				else
				{
					if( Vars -> IsResize2 )
					{
						fs.doResize2( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ], DstIncr, ElBiases,
							TmpFltBuf );
					}
					else
					{
						fs.doResize( BufPtrs[ fs.InBuf ],
							BufPtrs[ fs.OutBuf ], DstIncr, ElBiases,
							TmpFltBuf );
					}
				}
			}
		}
	};
};

#undef AVIR_PI
#undef AVIR_PId2
#undef AVIR_NOCTOR

}

#endif
