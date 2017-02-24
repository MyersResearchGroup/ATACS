/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1996 by, Eric G Mercer                                    */
/*   Electrical Engineering Department                                       */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Univeristy of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL U. OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE LIABLE  */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <numeric>

#include "stats.h"
#include "cpu_time_tracker.h"

#define CONV(i) ((real_type)(i))

#ifndef _NO_STATS_
extern "C" {
  extern real_type stdtri( int, real_type );
  extern real_type ndtri( int, real_type );
}
#endif

/////////////////////////////////////////////////////////////////////////////
// See Numerical Recipies in C page 892
// Determining Machine Properties
/////////////////////////////////////////////////////////////////////////////
void machar( real_type& eps, real_type& epsneg ) {
  int i, itemp, irnd, it, ibeta, negep, machep;
  real_type a, b, beta, betah, betain, one, temp, temp1, tempa, two, zero;
  
  one = CONV(1);
  two = one + one;
  zero = one - one;
  a = one;
  do {
    a += a;
    temp = a + one;
    temp1 = temp - a;
  } while( temp1 - one == zero );
  b = one;
  do {
    b += b;
    temp = a + b;
    itemp = (int)( temp - a );
  } while( itemp == 0 );
  ibeta = itemp;
  beta = CONV( ibeta );
  it = 0;
  b = one;
  do {
    ++it;
    b *= beta;
    temp = b + one;
    temp1 = temp - b;
  } while ( temp1 - one == zero );
  irnd = 0;
  betah = beta / two;
  temp = a + betah;
  if ( temp - a != zero ) irnd = 1;
  tempa = a + beta;
  temp = tempa + betah;
  if ( irnd == 0 && temp - tempa != zero ) irnd = 2;
  negep = it + 3;
  betain = one / beta;
  a = one;
  for ( i = 1 ; i < negep ; i++ ) a *= betain;
  b = a;
  for ( ; ; ) {
    temp = one - a;
    if ( temp - one != zero ) break;
    a *= beta;
    --negep;
  }
  negep = -negep;
  epsneg = a;
  machep = -it - 3;
  a = b;
  for( ; ; ) {
    temp = one + a;
    if ( temp - one != zero ) break;
    a *= beta;
    ++machep;
  }
  eps = a;
}

bool real_type_compare( const real_type& a, 
			const real_type& b, 
			const real_type& prec ) {
  int exp_a = 0, exp_b = 0;
  real_type frac_a = frexp( a, &exp_a );
  real_type frac_b = frexp( b, &exp_b );
  if ( exp_a != exp_b ) return( false );
  return( fabs( frac_a - frac_b ) <= prec );
}


void dump_error( FILE* fp, unsigned int ERROR ) {
  switch( ERROR ) {
  case _NOT_ENOUGH_DATA_: 
    fprintf( fp, "Note enough data to generate sumary statistics.\n" );
    break;
  case _ZERO_VARIANCE_:
    fprintf( fp, "Zero variace detected in sumary statistics.\n" );
    break;
  default:
    fprintf( fp, "Unknown error %d\n", ERROR );
  }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
int dump_stats( FILE* fp, real_type* data, unsigned int n, 
		unsigned int reps ) {
  int tmp = 0;
  real_type ave = 0.0, adev = 0.0, sdev = 0.0, var = 0.0;
  real_type skew = 0.0, curt = 0.0, half_length = 0.0, med = 0.0;
 
  fprintf( fp, " -------------+---------------------------\n" );
  fprintf( fp, "|   Index   |        Obeservation         |\n" );
  fprintf( fp, " -------------+---------------------------\n" );
  
  for ( tmp = 0 ; (unsigned int)tmp < n ; tmp++ )
    fprintf( fp, "%8d%26.10f\n", tmp+1, data[tmp] );

  tmp = moment( data, n, ave, adev, sdev, var, skew, curt );
  if ( tmp != _ALL_IS_WELL_ ) {
    dump_error( fp, tmp );
    return( tmp );
  }
  median( data, n, med );

  confidence_interval( sdev, n, 0.05, half_length );
  
  fprintf( fp, "\n ----------------------------------------\n" );
  fprintf( fp, "|           Summary Statistics           |\n" );
  fprintf( fp, " ----------------------------------------\n" );
  fprintf( fp, " cycles observed: %15d\n" , reps );
  fprintf( fp, "    replications: %15d\n", n );
  fprintf( fp, "confidence level: %19.3f\n", 0.95 );
  fprintf( fp, "            mean: %19.10f\n", ave );
  fprintf( fp, "          median: %19.10f\n", med );
  fprintf( fp, "     half-length: %19.10f\n", half_length );
  fprintf( fp, "             std: %19.10f\n", sdev );
  fprintf( fp, "         avg std: %19.10f\n", adev );
  fprintf( fp, "             var: %19.10f\n", var );
  fprintf( fp, "            skew: %19.3f\n", skew );
  fprintf( fp, "            curt: %19.3f", curt );
  
  return( _ALL_IS_WELL_ );
}

int dump_stats( designADT design, real_type* data, 
		unsigned int n, unsigned int reps ) {
  if ( design->filename == NULL ) {
    fprintf( stdout, "No file loaded or file name is not specified.\n" );
    return( _NULL_POINTER_ );
  }
  char outfile[strlen( design->filename ) + 7];
  strcpy( outfile, design->filename );
  strcat( outfile, ".stats" );
  FILE* fp = fopen( outfile, "w" );
  if ( fp == NULL ) {
    fprintf( stdout, "Failed to open '%s' for writing", outfile );
    return( _BAD_FILE_ );
  }
  cpu_time_tracker time;
  fprintf(stdout,"Finding summary statistics and storing to: %s ...",outfile );
  dump_stats( fp, data, n, reps );
  time.mark();
  fprintf( stdout, "done: " );
  time.print( stdout );
  fprintf( stdout, "\n" );
  fclose( fp );
  return( _ALL_IS_WELL_ );
}

int confidence_interval( const real_type& stdev, 
			 const unsigned int& n,
			 const real_type& alpha,
		         real_type& half_length ) {
#ifndef _NO_STATS_
  if ( n < 1 ) return( _NOT_ENOUGH_DATA_ );
  half_length = ( n < 120 ) ? 
                  stdtri( (int)(n - 1), (real_type)(1 - alpha / 2) ) * 
                  stdev / sqrt( (double)n ) 
                :
                  ndtri( (int)(n - 1), (real_type)(1 - alpha / 2) ) * 
                  stdev / sqrt( (double)n );
#else
  half_length = 0;
#endif
  return( _ALL_IS_WELL_ );
}

int median( real_type* data, const unsigned int& n, real_type& med ) {
  if ( n < 1 ) return( _NOT_ENOUGH_DATA_ );
  bool odd = (bool)( n % 2 );
  real_type* mptr = data + ( (odd) ? (n-1)/2 : n/2  );
  if ( odd ) {
    nth_element( data, mptr, data + n );
    med =  *mptr;
    return( _ALL_IS_WELL_ );
  }
  nth_element( data, (mptr-1), data+n );
  med = ( 0.5 * ( *(mptr) + *(mptr-1) ) );
  return( _ALL_IS_WELL_ );
}

/////////////////////////////////////////////////////////////////////////////
// NOTE: The variance and mean calculation should be extracted into function
// calls to allow them to be computed by other functions.
/////////////////////////////////////////////////////////////////////////////
int moment( const real_type* data, const unsigned int& n,
	    real_type& ave,
	    real_type& adev,
	    real_type& sdev,
	    real_type& var,
	    real_type& skew,
	    real_type& curt ) {
  real_type ep = 0.0, s = 0.0, p = 0.0;
  // You must have at least 2 observations in data to compute a 
  // moment, return 
  if ( n < 1 ) return( _NOT_ENOUGH_DATA_ );
  ave = accumulate( data, data+n, 0.0 ) / n;
  adev = var = skew = curt = 0.0;
  for ( unsigned int j = 0 ; j < n ; j++ ) {
    adev += fabs( s = data[j] - ave );
    ep += s;
    var += ( p = s * s );
    skew += ( p *= s );
    curt += ( p *= s );
  }
  adev /= n;
  ep += s;
  var = ( var - ep * ep / n ) / ( n - 1 );
  sdev = sqrt( var );
  if ( var != 0 ) {
    skew /= ( n * var * sdev );
    curt = curt / ( n * var * var ) - 3.0;
  }
  else return( _ZERO_VARIANCE_ );
  return( _ALL_IS_WELL_ );
}
