/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2001 by, Chris J. Myers                                   */
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
/*   specific, prior written permission of University of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE */
/*   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/*   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA */
/*   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* biomap.h                                                                 */
/*****************************************************************************/

#ifndef _biomap_h
#define _biomap_h


#include "biostructure.h"
#include "decomp.h"
#include <map>
#include <vector>

using namespace std;

//#define __BIO_VERBOSE__

void init_bio_map(master_class &bio);
void bio_map(designADT design);

//This function tests if parsing occurred correctly by printing out all
//the parsed gates
void printParsed(hash_map<gate_data> *all_gates, hash_map<promoter> *promoters_set, hash_map<interaction_data> *interactions_set, hash_map<gene> *genes_set);

//This function determines if a vector is satsifiable
bool is_satisfiable(parts_handler& all_parts, int to_sat[]);

//First, use parts that have no affect on solution satisfiability
void use_independent_part(parts_handler &all_parts, int to_sat[]);

//Reduces the problem by using parts who's availability will have no affect on the existence of solution
//void reduce(parts_handler &all_parts, int to_sat[]);
void reduce(parts_handler &all_parts, int to_sat[], bool& done);

//Sorts the promoters into bins
vector<vector<promoter*> > sort_type(parts_handler &all_parts);

//Chooses the best part to use
void choose_best(parts_handler &all_parts, int to_sat[], vector<gene*> inputs, promoter& p);

bool enough_available(parts_handler &all_parts, int to_sat[]);

hash_map<gene*> make_map(vector<gene*> genes);

bool done(int to_sat[]);

void copy_array(int new_to_sat[], int to_sat[]);

//Test function for parts_used
void test_parts_used(master_class* bio, parts_handler* all_parts);
void test_independent_parts(master_class* bio, parts_handler* all_parts, int test[]);
void test_promoters_affected(master_class* bio, parts_handler* all_parts);
#endif    // Ending _biomap_h
