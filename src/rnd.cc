/**
 * \warning Do NOT use for cryptographic purposes.  Read Internet RFC4086,
 * <http://tools.ietf.org/html/rfc4086>.
 */

/**
 * \file rnd.cc
 * \brief Mersenne Twister MT19937 pseudorandom number generator
 * \author Makoto Matsumoto (1997-2002)
 * \author Takuji Nishimura (1997-2002)
 * \author Seiji Nishimura (2008) <seiji1976@gmail.com>
 * \author Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr>
 * \author Pascal Getreuer <getreuer@gmail.com>
 * 
 * \par History
 * - This code uses the Mersenne Twister MT19937 code by Makoto Matsumoto and
 *   Takuji Nishimura (2002/1/26), the original is available at
 *   <http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html>
 * - Seiji Nishimura (2008) wrote a reentrant modification of MT19937, where
 *   a generator struct is passed to the pseudorandom sampler functions,
 *   <http://www.k3.dion.ne.jp/~sn1976/site/Software.html>
 * - Nicolas Limare wrote a version with Doxygen documentation, simpler
 *   interface, and automatic initialization using the time, 
 *   <http://dev.ipol.im/git/?p=nil/mt.git;a=summary>
 * - Pascal Getreuer added samplers for normal, exponential, Gamma, 
 *   geometric, and Poisson distributions and test program with Kolmogorov-
 *   Smirnov and chi-squared testing.
 * 
 * 
 * Copyright (C) 1997-2002, Makoto Matsumoto and Takuji Nishimura \n
 * Copyright (C) 2008, Seiji Nishimura <seiji1976@gmail.com> \n
 * Copyright (C) 2010-2011 Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr> \n
 * Copyright (C) 2011, Pascal Getreuer <getreuer@gmail.com> \n
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 
 *   3. The names of its contributors may not be used to endorse or promote 
 *      products derived from this software without specific prior written 
 *      permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER 
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "rnd.h"

/* Get the high-precision timer specified by POSIX.1-2001 if available */
#if defined(USE_GETTIMEOFDAY) || defined(unix) || defined(__unix__) || defined(__unix)
#include <unistd.h>
#if defined(USE_GETTIMEOFDAY) || (defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200112L))
#include <sys/time.h>
#define USE_GETTIMEOFDAY
#endif
#endif

#ifndef M_LOGSQRT2PI
/** \brief The constant \f$ \log \sqrt{2\pi} \f$ */
#define M_LOGSQRT2PI    0.9189385332046727417803297
#endif

/* Period parameters */
#define MT_N           624             /**< length of the state vector    */
#define MT_M           397
#define MT_MATRIX_A    0x9908B0DFUL    /**< constant vector a             */
#define MT_UPPER_MASK  0x80000000UL    /**< most  significant w-r bits    */
#define MT_LOWER_MASK  0x7FFFFFFFUL    /**< least significant   r bits    */


/** \brief An MT19937 pseudo-random number generator */
typedef struct randmtstruct_t
{    
    unsigned long mt[MT_N];    /**< the array for the state vector       */    
    int mti;                   /**< current position in the state vector */
} randmttype_t;

/** \brief Global randmt_t, used with the global versions of the functions */
randmt_t __randmt_global_generator = {{0}, MT_N + 1};


/* Create a new randmt_t */
randmt_t *new_randmt(void)
{
    randmt_t *generator;

    if((generator = (randmt_t *)calloc(1, sizeof(randmt_t))))
    {
        /* mti == MT_N+1 means mt[MT_N] is not initialized */
        generator->mti = MT_N + 1;
    }

    return generator;
}


/* Free a randmt_t */
void free_randmt(randmt_t *generator)
{
    if(generator)
        free(generator);
    return;
}


/* Initialize randmt_t with a seed */
void init_randmt_r(randmt_t *generator, unsigned long seed)
{
    int mti;
    unsigned long *mt;

    if(!generator)
        return;
    
#if ULONG_MAX > 0xFFFFFFFFUL
    seed &= 0xFFFFFFFFUL;           /* for WORDSIZE > 32bit */
#endif
    (mt = generator->mt)[0] = seed;

    for(mti = 1; mti < MT_N; mti++) 
    {
        mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
         * In the previous versions, MSBs of the seed affect
         * only MSBs of the array mt[].
         * 2002/01/09 modified by Makoto Matsumoto
         */
#if ULONG_MAX > 0xFFFFFFFFUL
        mt[mti] &= 0xFFFFFFFFUL;    /* for WORDSIZE > 32bit machines */
#endif
    }

    generator->mti = MT_N;
    return;
}


/* Initialize generator with the current time and memory address */
void init_randmt_auto_r(randmt_t *generator)
{
    unsigned long int seed;

    /* The basic (weak) initialization uses the current time, in seconds */
    seed = (unsigned long int) time(NULL);
    /* Add to the generator's memory address so that different generators 
       use different seeds. */
    seed += ((unsigned long int) generator);

#if defined(USE_GETTIMEOFDAY)
    /* gettimeofday() provides microsecond resolution */
    {
        struct timeval tp;
        (void) gettimeofday(&tp, NULL);

        seed *= 1000000;
        seed += tp.tv_usec;
    }
#endif

    init_randmt(seed);
    return;
}


/* Generate a random 32-bit unsigned integer (reentrant) */
unsigned long rand_uint32_r(randmt_t *generator)
{
    /* mag01[x] = x * MT_MATRIX_A  for x=0,1 */
    const unsigned long mag01[2] = { 0x0UL, MT_MATRIX_A };
    unsigned long y, *mt;
    int mti;
    
    if(!generator)
        return 0;

    mt = generator->mt;
    mti = generator->mti;

    if (mti >= MT_N)   /* generate MT_N words at one time */
    {    
        int kk;
        
        /* If init_randmt_r() has not been called, use a default seed. */
        if (mti == MT_N + 1)   
            init_randmt_r(generator, 5489UL);
        
        for(kk = 0; kk < MT_N - MT_M; kk++) 
        {
            y = (mt[kk] & MT_UPPER_MASK) | (mt[kk + 1] & MT_LOWER_MASK);
            mt[kk] = mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }        
        for(; kk < MT_N - 1; kk++) 
        {
            y = (mt[kk] & MT_UPPER_MASK) | (mt[kk + 1] & MT_LOWER_MASK);
            mt[kk] = mt[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        
        y = (mt[MT_N - 1] & MT_UPPER_MASK) | (mt[0] & MT_LOWER_MASK);
        mt[MT_N - 1] = mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
        mti = 0;
    }

    y = mt[mti++];
    
    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9D2C5680UL;
    y ^= (y << 15) & 0xEFC60000UL;    
    y ^= (y >> 18);

    generator->mti = mti;

    return y;
}


/* Generate a Gamma-distributed number (reentrant) */
double rand_gamma_r(randmt_t *generator, double a, double b)
{
    const double d = a - 1.0/3.0;
    const double c = 1.0/sqrt(9*d);
    double x, v, u;
    
    if(a >= 1)
    {
        do
        {
            do
            {
                x = rand_normal_r(generator);
                v = 1 + c*x;
            }while(v <= 0);
            
            v = v*v*v;
            u = rand_unif_r(generator);
            x = x*x;
        }while(u >= 1 - 0.0331*x*x 
            && log(u) >= x/2 + d*(1 - v + log(v)));
                
        return b*d*v;
    }
    else if(a > 0)
        return rand_gamma_r(generator, 1 + a, b) 
            * pow(rand_unif_r(generator), 1/a);
    else
        return 0;
}


/* Compute the natural logarithm of the factorial (used by rand_poisson_r) */
static double logfactorial(double n)
{
    /* Look-up table for log(n!) for n = 2, ..., 11 */
    static const double Table[10] = {
        0.69314718055994530942, 1.7917594692280550008,
        3.1780538303479456197, 4.7874917427820459943,
        6.5792512120101009951, 8.5251613610654143002,
        10.604602902745250228, 12.801827480081469611,
        15.104412573075515295, 17.502307845873885839};
    /* Asymptotic approximation coefficients */
    static const double C[7] = { -1.910444077728e-03, 
        8.4171387781295e-04, -5.952379913043012e-04, 
        7.93650793500350248e-04, -2.777777777777681622553e-03, 
        8.333333333333333331554247e-02, 5.7083835261e-03};
    double x, xsq, res, corr;
    int i;
    
    if(n > 11)  /* Asymptotic approximation, based on the public domain */
    {           /* NETLIB (Fortran) code by W. J. Cody and L. Stoltz    */
        x = 1 + floor(n + 0.5);
        
        if(x <= 2.25e76)
        {
            res = C[6];
            xsq = x*x;
            
            for(i = 0; i < 6; i++)
                res = res/xsq + C[i];
        }
        else
            res = 0;
        
        corr = log(x);
        return res/x + M_LOGSQRT2PI - corr/2 + x*(corr - 1);
    }
    else        /* Use look-up table */
    {
        i = (int)floor(n + 0.5);
        return (i >= 2) ? Table[i - 2] : 0; 
    }
}


/* Generate a Poisson-distributed number (reentrant) */
double rand_poisson_r(randmt_t *generator, double mu)
{
    double k, b, a, U, V, us, vr, invalpha, logmu;
    
    if(mu < 10)
    {   /* Use simple direct algorthm for small mu */
        vr = exp(-mu);
        k = -1;
        V = 1;
        
        do
        {
            k += 1;
            V *= rand_unif_r(generator);
        }while(V > vr);
    }
    else
    {   /* Use the "PTRS" algorithm of Hormann */
        b = 0.931 + 2.53*sqrt(mu);
        a = -0.059 + 0.02483*b;
        vr = 0.9277 - 3.6224/(b - 2);
        invalpha = 1.1239 + 1.1328/(b - 3.4);
        logmu = log(mu);
        
        do
        {
            U = rand_unif_r(generator) - 0.5;
            V = rand_unif_r(generator);
            us = 0.5 - fabs(U);
            k = floor((2*a/us + b)*U + mu + 0.43);
        }while((us < 0.07 || V > vr)
            && (k < 0 || (us < 0.013 && V > us)
                || log(V*invalpha/(a/(us*us) + b)) 
                        > -mu + k*logmu - logfactorial(k)));
    }
    
    return k;
}
