/*
 * Copyright 2016, Simula Research Laboratory
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "sift_constants.h"
#include "sift_conf.h"

#define SUPPORT_ABSOLUTE_SIGMA

namespace popsift {

struct GaussInfo
{
    int required_filter_stages;

    /* These are the 1D Gauss tables for all levels of an octave.
     * The first row is special:
     * - in octave 0 if initial blur is non-zero, contains the
     *   remaining blur that is required to reach sigma0
     * - in octave 0 if initial blur is zero, contains the
     *   filter for sigma0
     * - in all other octaves, row 0 is unused
     */
    float incremental_filter[ GAUSS_ALIGN * GAUSS_LEVELS ];

#ifdef SUPPORT_ABSOLUTE_SIGMA
    /* An experimental 1D Gauss table. The idea is to blur
     * directly level 1. Level 1 is more suitable than level 0
     * because level 0 has special handling in octave 0.
     * For ease of use, level 0 exists in the table but is
     * initialized to 0.
     */
    float from_lvl_1[ GAUSS_ALIGN * GAUSS_LEVELS ];

    float abs_sigma[ GAUSS_LEVELS ];
#endif // SUPPORT_ABSOLUTE_SIGMA

    /* The sigma used to generate the Gauss table for each level.
     * Meaning these are the differences between sigma0 and sigmaN.
     */
    float sigma[ GAUSS_LEVELS ];

    /* The sigma value that is the difference between the assumed
     * initial blur (given blur of the input) and sigma0. Used to
     * generated filter_initial_blur.
     */
    float initial_sigma;

    /* The span of the table that is generated for each level.
     */
    int span[ GAUSS_LEVELS ];

#ifdef SUPPORT_ABSOLUTE_SIGMA
    /* Equivalent to span for from_lvl_1 tables.
     */
    int   abs_span[ GAUSS_LEVELS ];
#endif // SUPPORT_ABSOLUTE_SIGMA

    __host__
    void clearTables( );

    __host__
    void computeBlurTable( int level, int span, float sigma );

#ifdef SUPPORT_ABSOLUTE_SIGMA
    __host__
    void computeAbsBlurTable( int level, int span, float sigma );
#endif // SUPPORT_ABSOLUTE_SIGMA

public:
    __host__
    void setSpanMode( Config::GaussMode m );

    __host__
    int getSpan( float sigma ) const;

private:
    Config::GaussMode _span_mode;

    __host__
    static int vlFeatSpan( float sigma );

    __host__
    static int openCVSpan( float sigma );
};

extern __device__ __constant__ GaussInfo d_gauss;

void init_filter( const Config& conf,
                  float         sigma0,
                  int           levels );

} // namespace popsift
