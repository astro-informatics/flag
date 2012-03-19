// FLAG package
// Copyright (C) 2012 
// Boris Leistedt & Jason McEwen

#include <flag.h>
#include "mex.h"

/**
 * MATLAB interface: slag_synthesis.
 * This function for internal use only.
 * Compute forward 1D SLAG transform (synthesis).
 *
 * Usage: 
 *   [f, nodes] = ...
 *     slag_synthesis_mex(fn, N, R, nodes);
 *
 */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{

  int n, i, N, fn_m, fn_n, nodes_m, nodes_n;
  double R;
  double *fn_real, *f_real, *nodes_in, *nodes_out, *weights;
  double *fn = NULL, *f = NULL, *nodes = NULL;
  int iin = 0, iout = 0;

  // Check number of arguments
  if(nrhs!=4) {
    mexErrMsgIdAndTxt("slag_synthesis_mex:InvalidInput:nrhs",
          "Require four inputs.");
  }
  if(nlhs!=2) {
    mexErrMsgIdAndTxt("slag_synthesis_mex:InvalidOutput:nlhs",
          "Require two outputs.");
  }

  // Parse harmonic coefficients fn
  iin = 0;
  fn_m = mxGetM(prhs[iin]);
  fn_n = mxGetN(prhs[iin]);
  fn_real = mxGetPr(prhs[iin]);
  fn = (double*)malloc(fn_m * fn_n * sizeof(double));
  for (i=0; i<fn_m*fn_n; i++)
    fn[i] = fn_real[i];
  

 // Parse harmonic band-limit N
  iin = 1;
  if( !mxIsDouble(prhs[iin]) || 
      mxIsComplex(prhs[iin]) || 
      mxGetNumberOfElements(prhs[iin])!=1 ) {
    mexErrMsgIdAndTxt("slag_synthesis_mex:InvalidInput:LbandLimit",
		      "Harmonic band-limit N must be integer.");
  }
  N = (int)mxGetScalar(prhs[iin]);
  if (mxGetScalar(prhs[iin]) > (double)N || N <= 0)
    mexErrMsgIdAndTxt("slag_synthesis_mex:InvalidInput:bandLimitNonInt",
		      "Harmonic band-limit N must be positive integer.");

  if (fn_m * fn_n != N)
    mexErrMsgIdAndTxt("slag_synthesis_mex:InvalidInput:fnSize",
		      "Invalid number of harmonic coefficients.");

  // Parse harmonic band-limit R
  iin = 2;
  if( !mxIsDouble(prhs[iin]) || 
      mxIsComplex(prhs[iin]) || 
      mxGetNumberOfElements(prhs[iin])!=1 ) {
    mexErrMsgIdAndTxt("slag_synthesis_mex:InvalidInput:Rlimit",
          "Radial limit R must be positive real.");
  }
  R = mxGetScalar(prhs[iin]);
  if ( R <= 0)
    mexErrMsgIdAndTxt("slag_synthesis_mex:InvalidInput:RLimitNonInt",
          "Radial limit R must be positive real.");

  nodes = (double*)calloc(N, sizeof(double));

  // Parse nodes
  iin = 3;
  nodes_m = mxGetM(prhs[iin]);
  nodes_n = mxGetN(prhs[iin]);
  if( nodes_m*nodes_n == N ){
    nodes_in = mxGetPr(prhs[iin]);
    for (i=0; i<N; i++)
      nodes[i] = nodes_in[i];
  }else{
    weights = (double*)calloc(N, sizeof(double));
    flag_spherlaguerre_sampling(nodes, weights, R, N);
    free(weights);
  }

  // Run spherical Laguerre synthesis
  f = (double*)calloc(N, sizeof(double));
	flag_spherlaguerre_synthesis(f, fn, nodes, N);

  iout = 0;
  plhs[iout] = mxCreateDoubleMatrix(1, N, mxREAL);
  f_real = mxGetPr(plhs[iout]);
  for(n=0; n<N; n++)
	 f_real[n] = f[n];

  iout = 1;
  plhs[iout] = mxCreateDoubleMatrix(1, N, mxREAL);
  nodes_out = mxGetPr(plhs[iout]);
  for(n=0; n<N; n++)
   nodes_out[n] = nodes[n];

  free(fn);
  free(nodes);
  free(f);

}