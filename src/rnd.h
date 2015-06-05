/**
 * \warning Do NOT use for cryptographic purposes.  Read Internet RFC4086,
 * <http://tools.ietf.org/html/rfc4086>.
 */

/**
 * \file rnd.h
 * \brief Mersenne Twister MT19937 pseudorandom number generator
 * \author Makoto Matsumoto (1997-2002)
 * \author Takuji Nishimura (1997-2002)
 * \author Seiji Nishimura (2008) <seiji1976@gmail.com>
 * \author Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr>
 * \author Pascal Getreuer <getreuer@gmail.com>
 * 
 * This software provides a high-quality pseudorandom number generator, the
 * Mersenne Twister MT19937.  
 * 
 * \par Basic Usage
 * To initialize the generator, call init_randmt_auto() at the beginning of the
 * program.  Example usage:
@code
#include <stdio.h>
#include "randmt.h"

int main(void)
{
    int i;
    init_randmt_auto();
    for(i = 0; i < 20; i++)
        printf("%10.8f\n", rand_unif());
    return 0;
}
@endcode
 * Pseudorandom numbers can be generated for several different distributions:
 * - #rand_unif,       the continuous uniform distribution on (0,1)
 * - #rand_uint32,     32-bit unsigned integers uniformly on [0,0xFFFFFFFF]
 * - #rand_normal,     the standard normal distribution
 * - #rand_exp,        exponential distribution
 * - #rand_gamma,      Gamma distribution
 * - #rand_geometric,  geometric distribution
 * - #rand_poisson,    Poisson distribution
 * 
 * \par Reentrant Versions
 * For use in multithreaded applications, reentrant versions of the functions
 * are also included which have the same name suffixed with "_r."  For these
 * functions, the generator state is passed using a #randmt_t object.
 * - #rand_unif_r,       the continuous uniform distribution on (0,1)
 * - #rand_uint32_r,     32-bit unsigned integers uniformly on [0,0xFFFFFFFF]
 * - #rand_normal_r,     the standard normal distribution
 * - #rand_exp_r,        exponential distribution
 * - #rand_gamma_r,      Gamma distribution
 * - #rand_geometric_r,  geometric distribution
 * - #rand_poisson_r,    Poisson distribution
 */

#ifndef _RANDMT_H_
#define _RANDMT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
/** \brief The constant \f$ \pi \f$ */
#define M_PI        3.14159265358979323846264338327950288419176939937510
#endif

/**
 * \brief An MT19937 pseudo-random number generator 
 * 
 * This object represents the state of an MT19937 pseudo-random number 
 * generator.  Use the following functions to create, initialize, and destroy
 * randmt_t objects:
 * - #new_randmt,          create a new randmt_t
 * - #init_randmt_auto_r,  initialize randmt_t with time and address
 * - #init_randmt_r,       initialize randmt_t with a specified seed value
 * - #free_randmt,         free memory associated with a randmt_t
 * 
 * The randmt_t is encapsulated by forward declaration, and defined in 
 * randmt.c.
 */
typedef struct randmtstruct_t randmt_t;

/** \brief Global randmt_t, used with the global versions of the functions */
extern randmt_t __randmt_global_generator;

/* Samplers using a specified generator */
/**
 * \defgroup randmtlocal randmt: Samplers using a specified generator
 * 
 * These functions use a specified #randmt_t to generate pseudorandom numbers.
 * \{
 */

/**
 * \brief Create a new randmt_t
 * \return a randmt_t pointer, or NULL on failure
 * \see init_randmt_auto_r()
 * \see init_randmt_r()
 * \see free_randmt()
 * 
 * A newly-created randmt_t should be seeded with init_randmt_auto_r() or 
 * init_randmt_r().  After use, call free_randmt() to free memory associated
 * with the randmt_t.
@code
randmt_t *generator = NULL;
int i;

if(!(generator = new_randmt()))
    exit(1);

init_randmt_auto_r(generator);

for(i = 0; i < 10; i++)
    printf("%f\n", rand_unif_r(generator));

free_randmt(generator);
@endcode
 */
randmt_t *new_randmt();

/** 
 * \brief Free a randmt_t
 * \param generator the randmt_t to free
 * \see new_randmt()
 * 
 * Free memory associated to a randmt_t that was created using new_randmt().
 */
void free_randmt(randmt_t *generator);

/**
 * \brief Initialize randmt_t with a seed
 * \param generator the randmt_t
 * \param seed the seed value 
 * \see init_randmt()
 * \see init_randmt_auto_r()
 */
void init_randmt_r(randmt_t *generator, unsigned long seed);

/**
 * \brief Initialize generator with the current time and memory address
 * \param generator the randmt_t
 * \see init_randmt_auto()
 * \see init_randmt_r()
 * 
 * The generator is seeded using the current time added to the memory address
 * of the generator.  The memory address is included so that different 
 * generators are initialized with different seeds.  An array of generators 
 * can be initialized as
@code
randmt_t *generators[16];
int i;

for(i = 0; i < 16; i++)
    init_randmt_auto_r(generators[i]);
@endcode
 */
void init_randmt_auto_r(randmt_t *generator);

/** 
 * \brief Generate a random 32-bit unsigned integer (reentrant)
 * \param generator the randmt_t 
 * \return Pseudo-random integer value uniformly between 0 and 0xFFFFFFFF
 * \see rand_uint32()
 * 
 * Generates a pseudorandom 32-bit integer value uniformly between 0 and 
 * 0xFFFFFFFF using the specified generator.
 */
unsigned long rand_uint32_r(randmt_t *generator);

/**
 * \brief Generate a Gamma-distributed number (reentrant)
 * \param generator the randmt_t
 * \param a shape parameter (positive value)
 * \param b scale parameter (positive value)
 * \return Pseudo-random Gamma-distibuted value
 * \see rand_gamma()
 * 
 * Generates a Gamma-distributed random value using the specified generator.
 */
double rand_gamma_r(randmt_t *generator, double a, double b);

/**
 * \brief Generate a Poisson-distributed number (reentrant)
 * \param generator the randmt_t
 * \param mu the mean parameter of the distribution (positive value)
 * \return Pseudo-random Poisson-distibuted value
 * \see rand_poisson()
 * 
 * A pseudo-random normally distributed number using the specified generator.
 */
double rand_poisson_r(randmt_t *generator, double mu);

/** 
 * \brief Generate a uniform random number on (0,1) (reentrant)
 * \param generator the randmt_t 
 * \return Pseudo-random double value in the open interval (0,1)
 * \see rand_unif()
 * 
 * This routine generates a random number uniformly on the open interval 
 * (0,1) with 53-bit resolution using the specified generator.  The formula
 * used to generate the number is
 *   \f[ \mathtt{rand\_unif} = (a 2^{26} + b + 0.5 - \epsilon) / 2^{53}, \f]
 * where a = integer of 27 random bits, b = integer of 26 random bits.
 */
#define rand_unif_r(generator)  (                   \
    (((rand_uint32_r(generator) >> 5)*67108864.0    \
        + (rand_uint32_r(generator) >> 6))          \
        + 0.4999999999999998) / 9007199254740992.0)

/**
 * \brief Generate a standard normal distributed random number (reentrant)
 * \param generator the randmt_t 
 * \return Pseudo-random double value with standard normal distribution
 * \see rand_normal()
 * 
 * A pseudo-random normally distributed number using the specified generator.
 */
#define rand_normal_r(generator)  (                 \
    sqrt(-2.0*log(rand_unif_r(generator)))          \
        * cos(2.0*M_PI*rand_unif_r(generator)))

/**
 * \brief Generate an exponentially-distributed number (reentrant)
 * \param generator the randmt_t
 * \param mu mean parameter of the distribution (positive value)
 * \return Pseudo-random exponentially-distibuted value with mean mu
 * \see rand_exp()
 * 
 * Generates a pseudo-random exponentially distributed value using the 
 * specified generator.
 */
#define rand_exp_r(generator, mu) (                 \
    -(mu)*log(rand_unif_r(generator)))

/**
 * \brief Generate a geometrically-distributed number (reentrant)
 * \param generator the randmt_t
 * \param p probability of success
 * \return Pseudo-random geometrically-distibuted value
 * \see rand_geometric()
 * 
 * Generates a pseudo-random geometrically-distributed value using the 
 * specified generator.
 */
#define rand_geometric_r(generator, p) (            \
    floor(log(rand_unif_r(generator))/log(1 - p)) + 1)

/** \} */


/* Samplers using the global generator */
/**
 * \defgroup randmtglobal randmt: Samplers using the global generator
 * 
 * These functions use the global pseudorandom number generator.
 * \{
 */

/**
 * \brief Initialize the global generator with a seed
 * \param seed the seed value
 * \see init_randmt_auto()
 * \see init_randmt_r()
 * 
 * This routine seeds the global random number generator with an unsigned 
 * 32-bit integer value.  
 * 
 * A constant seed can be used to reproduce the same pseudorandom numbers.
@code
int i;

init_randmt(42);
printf("Ten numbers:\n");
for(i = 0; i < 10; i++)
    printf("%f\n", rand_unif());

init_randmt(42);
printf("The same ten numbers:\n");
for(i = 0; i < 10; i++)
    printf("%f\n", rand_unif());
@endcode
 */
#define init_randmt(seed)     (init_randmt_r(&__randmt_global_generator, seed))

/**
 * \brief Initialize the global generator with the current time
 * \see init_randmt()
 * \see init_randmt_auto_r()
 * 
 * This function seeds the global generator with the current time.  It 
 * should be called once at the beginning of the program so that different
 * pseudorandom values are produced on different runs.
 * 
 * This function only needs to be called once.  Seeding multiple times does
 * not improve the statistical quality of the generator.
 */
#define init_randmt_auto()    (init_randmt_auto_r(&__randmt_global_generator))

/** 
 * \brief Generate a random 32-bit unsigned integer (nonreentrant)
 * \return Pseudo-random integer value uniformly between 0 and 0xFFFFFFFF
 * \see rand_uint32_r()
 * \see rand_unif()
 * 
 * The global generator is used to generate the value.
 */
#define rand_uint32()       (rand_uint32_r(&__randmt_global_generator))

/** 
 * \brief Generate a uniform random number on (0,1) (nonreentrant)
 * \return Pseudo-random double value in the open interval (0,1)
 * \see rand_unif_r()
 * \see rand_uint32()
 * 
 * This routine generates a random number uniformly on the open interval 
 * (0,1) with 53-bit resolution.  The formula used to generate the number is
 *   \f[ \mathtt{rand\_unif} = (a 2^{26} + b + 0.5 - \epsilon) / 2^{53}, \f]
 * where a = integer of 27 random bits, b = integer of 26 random bits.
 * The global generator is used to generate the value.
 * 
 * Distribution properties:
 * - CDF: \f$ \min(\max(x,0),1) \f$
 * - Mean: \f$ \frac{1}{2} \f$
 * - Variance: \f$ \frac{1}{12} \f$
 */
#define rand_unif()         (rand_unif_r(&__randmt_global_generator))

/**
 * \brief Generate a standard normal distributed random number (nonreentrant)
 * \return Pseudo-random double value with standard normal distribution
 * \see rand_normal_r()
 * 
 * A pseudo-random normally distributed number with density 
 *  \f[ f(x) = \frac{1}{\sqrt{2\pi}} \mathrm{e}^{-x^2/2}, \f]
 * generated by the Box-Muller transform.  The global generator is used 
 * to generate the value.
 * 
 * Distribution properties:
 * - CDF: \f$ \frac{1}{2}\bigl( 1 + \mathrm{erf}(x/\sqrt{2})\bigr) \f$
 * - Mean: 0
 * - Variance: 1
 */
#define rand_normal()       (rand_normal_r(&__randmt_global_generator))

/**
 * \brief Generate an exponentially-distributed number (nonreentrant)
 * \param mu mean parameter of the distribution (positive value)
 * \return Pseudo-random exponentially-distibuted value with mean mu
 * \see rand_exp_r()
 * 
 * Generates a pseudo-random value distributed with probability density
 *   \f[ f(x;\mu) = \begin{cases} 
 *       \frac{1}{\mu} \mathrm{e}^{-x/\mu}, & x \ge 0, \\
 *       0,                                 & x < 0,
 *     \end{cases} \f]
 * generated by inversion.  The global generator is used to generate 
 * the value.
 * 
 * Distribution properties:
 * - CDF: \f$ 1 - \exp(-x/\mu) \f$
 * - Mean: \f$ \mu \f$
 * - Variance: \f$ \mu^2 \f$
 */
#define rand_exp(mu)        (rand_exp_r(&__randmt_global_generator, mu))

/**
 * \brief Generate a Gamma-distributed number (nonreentrant)
 * \param a shape parameter (positive value)
 * \param b scale parameter (positive value)
 * \return Pseudo-random Gamma-distibuted value
 * \see rand_gamma_r()
 * 
 * Generates a Gamma-distributed random value with density 
 *  \f[ f(x;a,b) = x^{a-1} \frac{\exp(-x/b)}{\Gamma(a)b^k}, \quad x \ge 0, \f]
 * where \f$ \Gamma(a) \f$ is the Gamma function, using the method of
 * Marsaglia and Tsang, 2000.  The global generator is used to generate
 * the value.
 * 
 * Distribution properties:
 * - CDF: \f$ \gamma(a,x/b)/\Gamma(a) \f$, where \f$ \gamma \f$ is the
 *   lower incomplete gamma function
 * - Mean: \f$ ab \f$
 * - Variance: \f$ ab^2 \f$
 */
#define rand_gamma(a, b)    (rand_gamma_r(&__randmt_global_generator, a, b))

/**
 * \brief Generate a Poisson-distributed number (nonreentrant)
 * \param mu the mean parameter of the distribution (positive value)
 * \return Pseudo-random Poisson-distibuted value
 * \see rand_poisson_r()
 * 
 * A pseudo-random normally distributed number with probability mass function
 *  \f[ \mathrm{P}(X = k) = \frac{\mu^k \mathrm{e}^{-\mu}}{k!}, 
 *      \quad k = 0, 1,\ldots, \f]
 * using a simple direct algorthm for mu < 10 and the "PTRS" algorthm of
 * Hormann for larger mu.  The global generator is used to generate 
 * the value.
 * 
 * Reference:
 *    Wolfgang Hormann, "The transformed rejection method for generating 
 *    Poisson random variables," Insurance: Mathematics and Economics 12, 
 *    pp. 39-45, 1993.
 * 
 * Distribution properties:
 * - CDF: \f$ 1 - \gamma(\lfloor k+1 \rfloor, \mu)/\lfloor k \rfloor ! \f$,
 *   where \f$ \gamma \f$ is the lower incomplete Gamma function
 * - Mean: \f$ \mu \f$
 * - Variance: \f$ \mu \f$
 */
#define rand_poisson(mu)    (rand_poisson_r(&__randmt_global_generator, mu))

/**
 * \brief Generate a geometrically-distributed number (nonreentrant)
 * \param p probability of success
 * \return Pseudo-random geometrically-distibuted value
 * \see rand_geometric_r()
 * 
 * Generates a pseudo-random value distributed with probability mass fuction
 *   \f[ \mathrm{P}(X = k) = (1 - p)^{k-1} p, \quad k = 1, 2, \ldots, \f]
 * where \f$ 0 < p \le 1 \f$, generated by inversion.  The global generator
 * is used to generate the value.
 * 
 * Distribution properties:
 * - CDF: \f$ 1 - (1 - p)^{\lfloor k \rfloor} \f$
 * - Mean: \f$ 1/p \f$
 * - Variance: \f$ (1 - p)/p^2 \f$
 */
#define rand_geometric(p)   (rand_geometric_r(&__randmt_global_generator, p))

#define rnd() rand_unif()
#define rnd_int() rand_uint32()

/** \} */

#ifdef __cplusplus
}
#endif
#endif /* _RANDMT_H_ */
