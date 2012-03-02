// FLAG package
// Copyright (C) 2012 
// Boris Leistedt & Jason McEwen

#include "flag.h"
#include <assert.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

double maxerr_cplx(complex double *a, complex double *b, int size)
{
	double value = 0;
	int i;
	for(i = 0; i<size; i++){
		value = MAX( cabs( a[i]-b[i] ), value );
	}
	return value;
}

double maxerr(double *a, double *b, int size)
{
	double value = 0;
	int i;
	for(i = 0; i<size; i++){
		value = MAX( abs( a[i]-b[i] ), value );
	}
	return value;
}

double ran2_dp(int idum) 
{
  int IM1=2147483563,IM2=2147483399,IMM1=IM1-1, 
    IA1=40014,IA2=40692,IQ1=53668,IQ2=52774,IR1=12211,IR2=3791, 
    NTAB=32,NDIV=1+IMM1/NTAB;

  double AM=1./IM1,EPS=1.2e-7,RNMX=1.-EPS;
  int j,k;
  static int iv[32],iy,idum2 = 123456789; 
  // N.B. in C static variables are initialised to 0 by default.

  if (idum <= 0) {
    idum= (-idum>1 ? -idum : 1); // max(-idum,1);
    idum2=idum;
    for(j=NTAB+8;j>=1;j--) {
      k=idum/IQ1;
      idum=IA1*(idum-k*IQ1)-k*IR1;
      if (idum < 0) idum=idum+IM1;
      if (j < NTAB) iv[j-1]=idum;
    }
    iy=iv[0];
  }
  k=idum/IQ1;
  idum=IA1*(idum-k*IQ1)-k*IR1;
  if (idum < 0) idum=idum+IM1;
  k=idum2/IQ2;
  idum2=IA2*(idum2-k*IQ2)-k*IR2;
  if (idum2 < 0) idum2=idum2+IM2;
  j=1+iy/NDIV;
  iy=iv[j-1]-idum2;
  iv[j-1]=idum;
  if(iy < 1)iy=iy+IMM1;
  return (AM*iy < RNMX ? AM*iy : RNMX); // min(AM*iy,RNMX);

}
	
void flag_random_f(complex double *f, int L, int N, int seed)
{
	int i;
	srand( time(NULL) );
	for (i=0; i<flag_f_size(L, N); i++){
		f[i] = (2.0*ran2_dp(seed) - 1.0) + I * (2.0*ran2_dp(seed) - 1.0);
	}
}
	
void flag_random_flmn(complex double *flmn, int L, int N, int seed)
{
	int i;
	srand( time(NULL) );
	for (i=0; i<flag_flmn_size(L, N); i++){
		flmn[i] = (2.0*ran2_dp(seed) - 1.0) + I * (2.0*ran2_dp(seed) - 1.0);//rand()/795079784.0;
	}
}

void print_f(const complex double *f,int L, int N)
{	
	int mapsize = ssht_fr_size(L);
	int n, j;
	for(n=0;n<N;n++){
		printf("\n -- Layer %i -- \n", n);
		for(j=0;j<mapsize;j++){
			printf(" (%f,%f) ",creal(f[n*mapsize+j]),cimag(f[n*mapsize+j]));
		}
	}
	printf("\n");
}

void print_flmn(const complex double *flmn,int L, int N)
{	
	int mapsize = ssht_flm_size(L);
	int n, j;
	for(n=0;n<N;n++){
		printf("\n -- Layer %i -- \n", n);
		for(j=0;j<mapsize;j++){
			printf(" (%f,%f) ",creal(flmn[n*mapsize+j]),cimag(flmn[n*mapsize+j]));
		}
	}
	printf("\n");
}

void flag_sampling_test(int L, int N, double R)
{

	double *rs, *thetas, *phis, *laguweights;

	flag_allocate_sampling(&rs, &thetas, &phis, &laguweights, R, L, N);
	flag_sampling(rs, thetas, phis, laguweights, R, L, N);
	//int i, n;
	//for (n=0; n<N; n++)
	//   printf("r=%f t=%f p=%f \n",rs[n], thetas[n], phis[n]);
	free(rs);
	free(thetas);
	free(phis);
	free(laguweights);
	
}

void flag_spherlaguerre_quadrature_test(int N)
{
	double *roots = (double*)calloc(N, sizeof(double));
	double *weights = (double*)calloc(N, sizeof(double));

	flag_spherlaguerre_quadrature(roots, weights, N);
	/*int n;
	for (n=0; n<N; n++)
		printf("Root %i = %f with weight %f \n",n,roots[n],weights[n]);*/
}

void flag_spherlaguerre_tau_test(int N)
{
	const double R = 1.0;
	double tau = flag_spherlaguerre_tau(R,N);
	//printf("Tau = %f",tau);
	assert(tau != 0);
}

void flag_spherlaguerre_sampling_test(int N, double R)
{
	double *nodes = (double*)calloc(N, sizeof(double));
	double *weights = (double*)calloc(N, sizeof(double));

	flag_spherlaguerre_sampling(nodes, weights, R, N);
	assert(nodes != NULL);
	assert(weights != NULL);

	//int n;for (n=0; n<N; n++)
	//	printf("Node %i = %f \n",n,nodes[n]);
	free(nodes);
	free(weights);
}

void flag_spherlaguerre_cmplx_transform_test(int L, int N, double R, int seed)
{	
	clock_t time_start, time_end;
	complex double *f, *fn, *fn_rec;
	int flmsize = ssht_flm_size(L);

	flag_allocate_flmn(&f, L, N);
	flag_allocate_flmn(&fn, L, N);
	flag_allocate_flmn(&fn_rec, L, N);

	flag_random_flmn(fn, L, N, seed);

	double *nodes = (double*)calloc(N, sizeof(double));
	double *weights = (double*)calloc(N, sizeof(double));

 	flag_spherlaguerre_sampling(nodes, weights, R, N);

	time_start = clock();
	flag_mapped_spherlaguerre_synthesis(f, fn, nodes, flmsize, N);
	time_end = clock();
	printf("  - Duration of mapped inverse synthesis : %4.4f  seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);

	time_start = clock();
	flag_mapped_spherlaguerre_analysis(fn_rec, f, nodes, weights, flmsize, N);
	time_end = clock();
	printf("  - Duration of mapped forward analysis  : %4.4f  seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	
	printf("  - Maximum abs error on reconstruction  : %6.5e\n", 
		maxerr_cplx(fn, fn_rec, N));


	free(f);
	free(fn);
	free(fn_rec);
	free(weights);
	free(nodes);
}

void flag_spherlaguerre_transform_test(int N, double R)
{	
	clock_t time_start, time_end;
	double *f = (double*)calloc(N, sizeof(double));
	double *fn = (double*)calloc(N, sizeof(double));
	double *fnrec = (double*)calloc(N, sizeof(double));
	int n;

	srand ( time(NULL) );

	for (n=0; n<N; n++){
		fn[n] = rand()/795079784.0;
	}

	double *nodes = (double*)calloc(N, sizeof(double));
	double *weights = (double*)calloc(N, sizeof(double));

 	flag_spherlaguerre_sampling(nodes, weights, R, N);

	time_start = clock();
	flag_spherlaguerre_synthesis(f, fn, nodes, N);
	time_end = clock();
	printf("  - Duration of 1D inverse synthesis     : %4.0e  seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);

	time_start = clock();
	flag_spherlaguerre_analysis(fnrec, f, nodes, weights, N);
	time_end = clock();
	printf("  - Duration of 1D forward analysis      : %4.0e  seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	
	printf("  - Maximum abs error on reconstruction  : %6.5e\n", 
		maxerr(fn, fnrec, N));

	//printf("\nTau = %f\n",flag_spherlaguerre_tau(1.0, N));
	//for (n=0; n<N; n++){
	   //printf("\nnodes[%i] = %f",n,nodes[n]);
	   //printf("\nweights[%i] = %f",n,weights[n]);
	   //printf("\nfn[%i] = %f",n,fn[n]);   
	//   printf("\nf[%i] = %f - frec[%i] = %f",n,f[n],n,frec[n]);
	//}
	free(f);
	free(fn);
	free(fnrec);
	free(weights);
	free(nodes);
}

void flag_transform_test(int L, int N, int seed)
{
	complex double *f, *flmn, *flmnrec;
	clock_t time_start, time_end;

	flag_allocate_f(&f, L, N);
	flag_allocate_flmn(&flmn, L, N);
	flag_allocate_flmn(&flmnrec, L, N);

    //printf("\n <--- Initial FLMN array ---> \n");
	flag_random_flmn(flmn, L, N, seed);
	//print_flmn(flmn, L, N);

    //printf("\n <--- Resulting field ---> \n");
    time_start = clock();
	flag_synthesis(f, flmn, L, N);
	time_end = clock();
	printf("  - Duration of full 3D synthesis        : %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	//print_f(f, L, N);

	//printf("\n <--- Recovered FLMN array ---> \n");
	time_start = clock();
	flag_analysis(flmnrec, f, L, N);
	time_end = clock();
	printf("  - Duration of full 3D analysis         : %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	//print_flmn(flmnrec, L, N);

	printf("  - Maximum abs error on reconstruction  : %6.5e\n", 
		maxerr_cplx(flmn, flmnrec, flag_flmn_size(L, N)));

	free(f);
	free(flmn);
	free(flmnrec);
}

/*void flag_transform_analysis_test(int L, int N, int seed)
{
	int spin = 0;
	int verbosity = 0;
	clock_t time_start, time_end, t_for, t_back;
	int flmsize = ssht_flm_size(L);
	int frsize = ssht_fr_size(L);
	int i, n, offset_lm, offset_r;
	ssht_dl_method_t dl_method = SSHT_DL_RISBO;

	complex double *f, *flmr, *f_rec, *flmn, *flmr_rec;

	flag_allocate_f(&f, L, N);
	flag_allocate_flmn(&flmr, L, N);
	flag_allocate_f(&f_rec, L, N);

	flag_random_f(f, L, N, seed);

	t_for = 0;
	t_back = 0;
	for (n = 0; n < N; n++){
		offset_lm = n * flmsize;
		offset_r = n * frsize;
		time_start = clock();
		ssht_core_mw_forward_sov_conv_sym(flmr + offset_lm, f + offset_r, L, spin, dl_method, verbosity);
		time_end = clock();
		t_for += (time_end - time_start) / N;
		time_start = clock();
		ssht_core_mw_inverse_sov_sym(f_rec + offset_r, flmr + offset_lm, L, spin, dl_method, verbosity);
		time_end = clock();
		t_back += (time_end - time_start) / N;
	}
	printf("  - SSHT forward average duration        : %4.4f seconds\n", 
		t_for / (double)CLOCKS_PER_SEC);
	printf("  - SSHT backward average duration       : %4.4f seconds\n", 
		t_back / (double)CLOCKS_PER_SEC);
	printf("  - SSHT maximum absolute error          : %6.5e\n", 
		maxerr_cplx(f, f_rec, flag_f_size(L, N)));
	for (n = 0; n < N; n++){
		for (i = 0; i < frsize; i++){
			//printf("n=%i - i=%i - (%f,%f) - (%f,%f)\n",n,i,creal(f[i+n*frsize]),cimag(f[i+n*frsize]),creal(f_rec[i+n*frsize]),cimag(f_rec[i+n*frsize]));
		}
	}

	free(f);
	free(f_rec);
	flag_allocate_flmn(&flmr_rec, L, N);
	flag_allocate_flmn(&flmn, L, N);
	
	double *nodes = (double*)calloc(N, sizeof(double));
	double *weights = (double*)calloc(N, sizeof(double));
	flag_spherlaguerre_quadrature(nodes, weights, N);
	time_start = clock();
	flag_mapped_spherlaguerre_analysis(flmn, flmr, nodes, weights, flmsize, N);
	time_end = clock();
	printf("  - SLAG mapped cmplx analysis duration  : %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	time_start = clock();
	flag_mapped_spherlaguerre_synthesis(flmr_rec, flmn, nodes, flmsize, N);
	time_end = clock();
	printf("  - SLAG mapped cmplx synthesis duration : %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	printf("  - SLAG maximum absolute error          : %6.5e\n", 
		maxerr_cplx(flmr, flmr_rec, flag_flmn_size(L, N)));

	free(flmn);
	free(flmr);
	free(flmr_rec);
}*/

void flag_transform_furter_test(int L, int N, int seed)
{
	int spin = 0;
	int verbosity = 0;
	clock_t time_start, time_end, t_for, t_back;
	int flmsize = ssht_flm_size(L);
	int frsize = ssht_fr_size(L);
	int i, n, offset_lm, offset_r;
	ssht_dl_method_t dl_method = SSHT_DL_RISBO;

	double *nodes = (double*)calloc(N, sizeof(double));
	double *weights = (double*)calloc(N, sizeof(double));
	flag_spherlaguerre_quadrature(nodes, weights, N);

	complex double *flmn, *flmr, *flmn_rec, *f, *flmr_rec;

	flag_allocate_f(&flmn, L, N);
	flag_allocate_flmn(&flmr, L, N);
	flag_allocate_f(&flmn_rec, L, N);
	flag_allocate_flmn(&flmr_rec, L, N);
	flag_allocate_f(&f, L, N);

	flag_random_flmn(flmn, L, N, seed);

	time_start = clock();
	flag_mapped_spherlaguerre_synthesis(flmr, flmn, nodes, flmsize, N);
	time_end = clock();
	printf("  - Synthesis : SLAG 3D synthes duration : %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	time_start = clock();
	flag_mapped_spherlaguerre_analysis(flmn_rec, flmr, nodes, weights, flmsize, N);
	time_end = clock();
	printf("  - Synthesis : SLAG 3D analys duration  : %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	printf("  - SLAG maximum absolute error          : %6.5e\n", 
		maxerr_cplx(flmn, flmn_rec, flag_flmn_size(L, N)));

	t_for = 0;
	t_back = 0;
	for (n = 0; n < N; n++){
		offset_lm = n * flmsize;
		offset_r = n * frsize;
		ssht_core_mw_inverse_sov_sym(f + offset_r, flmr + offset_lm, L, spin, dl_method, verbosity);
		time_end = clock();
		t_back += (time_end - time_start) / N;
		time_start = clock();
		ssht_core_mw_forward_sov_conv_sym(flmr_rec + offset_lm, f + offset_r, L, spin, dl_method, verbosity);
		time_end = clock();
		t_for += (time_end - time_start) / N;
		time_start = clock();
	}
	printf("  - Synthesis : SSHT forward av duration : %4.4f seconds\n", 
		t_for / (double)CLOCKS_PER_SEC);
	printf("  - Analysis : SSHT backward av duration : %4.4f seconds\n", 
		t_back / (double)CLOCKS_PER_SEC);
	printf("  - SSHT maximum absolute error          : %6.5e\n", 
		maxerr_cplx(flmr, flmr_rec, flag_flmn_size(L, N)));
	
	free(flmn_rec);
	flag_allocate_f(&flmn_rec, L, N);
	free(flmr);
	flag_allocate_flmn(&flmr, L, N);

	time_start = clock();
	flag_mapped_spherlaguerre_analysis(flmn_rec, flmr_rec, nodes, weights, flmsize, N);
	time_end = clock();
	printf("  - Analysis : SLAG 3D analysis duration : %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	time_start = clock();
	flag_mapped_spherlaguerre_synthesis(flmr, flmn_rec, nodes, flmsize, N);
	time_end = clock();
	printf("  - Analysis : SLAG 3D synthesis duration: %4.4f seconds\n", 
		(time_end - time_start) / (double)CLOCKS_PER_SEC);
	printf("  - SLAG maximum absolute error          : %6.5e\n", 
		maxerr_cplx(flmr, flmr_rec, flag_flmn_size(L, N)));
	printf("  - Final FLAG maximum absolute error    : %6.5e\n", 
		maxerr_cplx(flmn, flmn_rec, flag_flmn_size(L, N)));

	free(flmn);
	free(flmn_rec);
	free(flmr);
	free(flmr_rec);
}


int main(int argc, char *argv[]) 
{
	const int L = 50;
	const int N = 50;
	const double R = 10.0;
	const int seed = (int)(10000.0*(double)clock()/(double)CLOCKS_PER_SEC);
	printf("==========================================================\n");
	printf("PARAMETERS : ");
	printf("  L = %i   N = %i   R = %4.1f   seed = %i\n", L, N, R, seed);
	printf("----------------------------------------------------------\n");
	printf("> Testing Laguerre quadrature...");
	flag_spherlaguerre_quadrature_test(N);
	printf("OK\n");

	printf("> Testing Laguerre tau calculation...");
	flag_spherlaguerre_tau_test(N);
	printf("OK\n");

	printf("> Testing Laguerre sampling scheme...");
	flag_spherlaguerre_sampling_test(N, R);
	printf("OK\n");

	printf("----------------------------------------------------------\n");

	printf("> Testing 1D Laguerre transform...\n");
	flag_spherlaguerre_transform_test(N, R);
	fflush(NULL);

	printf("----------------------------------------------------------\n");

	printf("> Testing cmplx mapped Laguerre transform...\n");
	flag_spherlaguerre_cmplx_transform_test(L, N, R, seed);
	fflush(NULL);

	printf("----------------------------------------------------------\n");

	printf("> Testing FLAG sampling scheme...");
	flag_sampling_test(L, N, R);
	printf("OK\n");

	printf("> Testing FLAG transform...\n");
	flag_transform_test(L, N, seed);
	fflush(NULL);

	printf("----------------------------------------------------------\n");

	printf("> Testing FLAG in further details...\n");
	flag_transform_furter_test(L, N, seed);
	fflush(NULL);

	printf("==========================================================\n");
	
	return 0;		
}
