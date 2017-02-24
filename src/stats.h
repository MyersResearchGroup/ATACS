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
#include "struct.h"
#include "CPdf.h"

#define _ALL_IS_WELL_      0
#define _NOT_ENOUGH_DATA_  1
#define _ZERO_VARIANCE_    2
#define _BAD_FILE_POINTER_ 3
#define _NULL_POINTER_     4
#define _BAD_FILE_         5

typedef CPdf::real_type real_type;

/////////////////////////////////////////////////////////////////////////////
// Returns the precision of real_type arithmatic for this archetecture
//          eps: smallest postive number that, added to 1.0, does ot
//               equal 1.0
//       negeps: smallest positive number that, subtracted from 1.0,
//               if not equal to 1.0
/////////////////////////////////////////////////////////////////////////////
void machar( real_type& eps, real_type& epsneg );

/////////////////////////////////////////////////////////////////////////////
// Returns true iff exponent of a == the exponent of b AND
// the fabs( fraction( a ) - fraction( b ) ) <= prec.
/////////////////////////////////////////////////////////////////////////////
bool real_type_compare( const real_type& a, 
			const real_type& b, 
			const real_type& prec );

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
int dump_stats( designADT design, real_type* data, 
		unsigned int n, 
		unsigned int reps );


/////////////////////////////////////////////////////////////////////////////
// Returns the half length of the confidence interval (half_length) for the
// given number of observations (n).  The alpha variable is the confidence
// indicator.  If a %95 interval is desired, then alpha is set to 
// 1-0.95 = 0.5.
/////////////////////////////////////////////////////////////////////////////
int confidence_interval( const real_type& stdev, 
			 const unsigned int& n,
			 const real_type& alpha,
		         real_type& half_length );

/////////////////////////////////////////////////////////////////////////////
// Returns the value in data[0..n-1] that has an equal number of elements
// above and below the value.  The function will partially sort data using
// the nth_element algorithm.  If data were sorted, then the median is
//
//                   /
//          median = | data[(N-1)/2)]              N odd
//                   | 0.5 * (X(N/2) + X((N/2)-1)) N even
//              
/////////////////////////////////////////////////////////////////////////////
int median( real_type* data, const unsigned int& n, real_type& med );

/////////////////////////////////////////////////////////////////////////////
// moment
//
// Description: This function is taken from "Numerical Recipies in C"
// William H. Press, Cambridge University Press 1992.  This function
// takes the aray of data[1...n] and computes the following statistical
// descriptors:
//
//      ave : mean average
//     adev : average deviation
//     sdev : standard deviation
//      var : variance
//     skew : skewness
//     curt : kurtosis
//
/////////////////////////////////////////////////////////////////////////////
int moment( const real_type* data, const unsigned int& n,
	    real_type& ave,
	    real_type& adev,
	    real_type& sdev,
	    real_type& var,
	    real_type& skew,
	    real_type& curt );
