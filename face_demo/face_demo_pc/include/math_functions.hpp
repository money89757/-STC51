#ifndef CAFFE_UTIL_MATH_FUNCTIONS_H_
#define CAFFE_UTIL_MATH_FUNCTIONS_H_

#include "common.h"
#include <stdint.h>
#include <cmath>  // for std::fabs and std::signbit

extern "C" {
#include "openblas/cblas.h"
}
#include <math.h>


#define DEFINE_VSL_UNARY_FUNC(name, operation) \
  template<typename Dtype> \
  void v##name(const int n, const Dtype* a, Dtype* y) { \
    for (int i = 0; i < n; ++i) { operation; } \
  } \
  inline void vs##name( \
    const int n, const float* a, float* y) { \
    v##name<float>(n, a, y); \
  } \
  inline void vd##name( \
      const int n, const double* a, double* y) { \
    v##name<double>(n, a, y); \
  }

DEFINE_VSL_UNARY_FUNC(Sqr, y[i] = a[i] * a[i]);
DEFINE_VSL_UNARY_FUNC(Exp, y[i] = exp(a[i]));
DEFINE_VSL_UNARY_FUNC(Ln, y[i] = log(a[i]));
DEFINE_VSL_UNARY_FUNC(Abs, y[i] = fabs(a[i]));

// A simple way to define the vsl unary functions with singular parameter b.
// The operation should be in the form e.g. y[i] = pow(a[i], b)
#define DEFINE_VSL_UNARY_FUNC_WITH_PARAM(name, operation) \
  template<typename Dtype> \
  void v##name(const int n, const Dtype* a, const Dtype b, Dtype* y) { \
    for (int i = 0; i < n; ++i) { operation; } \
  } \
  inline void vs##name( \
    const int n, const float* a, const float b, float* y) { \
    v##name<float>(n, a, b, y); \
  } \
  inline void vd##name( \
      const int n, const double* a, const float b, double* y) { \
    v##name<double>(n, a, b, y); \
  }

DEFINE_VSL_UNARY_FUNC_WITH_PARAM(Powx, y[i] = pow(a[i], b));

// A simple way to define the vsl binary functions. The operation should
// be in the form e.g. y[i] = a[i] + b[i]
#define DEFINE_VSL_BINARY_FUNC(name, operation) \
  template<typename Dtype> \
  void v##name(const int n, const Dtype* a, const Dtype* b, Dtype* y) { \
    for (int i = 0; i < n; ++i) { operation; } \
  } \
  inline void vs##name( \
    const int n, const float* a, const float* b, float* y) { \
    v##name<float>(n, a, b, y); \
  } \
  inline void vd##name( \
      const int n, const double* a, const double* b, double* y) { \
    v##name<double>(n, a, b, y); \
  }

DEFINE_VSL_BINARY_FUNC(Add, y[i] = a[i] + b[i]);
DEFINE_VSL_BINARY_FUNC(Sub, y[i] = a[i] - b[i]);
DEFINE_VSL_BINARY_FUNC(Mul, y[i] = a[i] * b[i]);
DEFINE_VSL_BINARY_FUNC(Div, y[i] = a[i] / b[i]);

inline void cblas_saxpby(const int N, const float alpha, const float* X,
	const int incX, const float beta, float* Y,
	const int incY) {
	cblas_sscal(N, beta, Y, incY);
	cblas_saxpy(N, alpha, X, incX, Y, incY);
}
inline void cblas_daxpby(const int N, const double alpha, const double* X,
	const int incX, const double beta, double* Y,
	const int incY) {
	cblas_dscal(N, beta, Y, incY);
	cblas_daxpy(N, alpha, X, incX, Y, incY);
}



// Caffe gemm provides a simpler interface to the gemm functions, with the
// limitation that the data has to be contiguous in memory.
template <typename Dtype>
void caffe_cpu_gemm(const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
    const Dtype alpha, const Dtype* A, const Dtype* B, const Dtype beta,
    Dtype* C);

template <typename Dtype>
void caffe_cpu_gemv(const CBLAS_TRANSPOSE TransA, const int M, const int N,
    const Dtype alpha, const Dtype* A, const Dtype* x, const Dtype beta,
    Dtype* y);

template <typename Dtype>
void caffe_axpy(const int N, const Dtype alpha, const Dtype* X,
    Dtype* Y);

template <typename Dtype>
void caffe_cpu_axpby(const int N, const Dtype alpha, const Dtype* X,
    const Dtype beta, Dtype* Y);

template <typename Dtype>
void caffe_copy(const int N, const Dtype *X, Dtype *Y);

template <typename Dtype>
void caffe_set(const int N, const Dtype alpha, Dtype *X);

inline void caffe_memset(const size_t N, const int alpha, void* X) {
  memset(X, alpha, N);  // NOLINT(caffe/alt_fn)
}

template <typename Dtype>
void caffe_add_scalar(const int N, const Dtype alpha, Dtype *X);

template <typename Dtype>
void caffe_scal(const int N, const Dtype alpha, Dtype *X);

template <typename Dtype>
void caffe_sqr(const int N, const Dtype* a, Dtype* y);

template <typename Dtype>
void caffe_add(const int N, const Dtype* a, const Dtype* b, Dtype* y);

template <typename Dtype>
void caffe_sub(const int N, const Dtype* a, const Dtype* b, Dtype* y);

template <typename Dtype>
void caffe_mul(const int N, const Dtype* a, const Dtype* b, Dtype* y);

template <typename Dtype>
void caffe_div(const int N, const Dtype* a, const Dtype* b, Dtype* y);

template <typename Dtype>
void caffe_powx(const int n, const Dtype* a, const Dtype b, Dtype* y);


template <typename Dtype>
Dtype caffe_nextafter(const Dtype b);


template <typename Dtype>
void caffe_exp(const int n, const Dtype* a, Dtype* y);

template <typename Dtype>
void caffe_log(const int n, const Dtype* a, Dtype* y);

template <typename Dtype>
void caffe_abs(const int n, const Dtype* a, Dtype* y);

template <typename Dtype>
Dtype caffe_cpu_dot(const int n, const Dtype* x, const Dtype* y);

template <typename Dtype>
Dtype caffe_cpu_strided_dot(const int n, const Dtype* x, const int incx,
    const Dtype* y, const int incy);

// Returns the sum of the absolute values of the elements of vector x
template <typename Dtype>
Dtype caffe_cpu_asum(const int n, const Dtype* x);

// the branchless, type-safe version from
// http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template<typename Dtype>
inline int8_t caffe_sign(Dtype val) {
  return (Dtype(0) < val) - (val < Dtype(0));
}

// The following two macros are modifications of DEFINE_VSL_UNARY_FUNC
//   in include/caffe/util/mkl_alternate.hpp authored by @Rowland Depp.
// Please refer to commit 7e8ef25c7 of the boost-eigen branch.
// Git cherry picking that commit caused a conflict hard to resolve and
//   copying that file in convenient for code reviewing.
// So they have to be pasted here temporarily.


#define DEFINE_CAFFE_CPU_UNARY_FUNC(name, operation) \
  template<typename Dtype> \
  void caffe_cpu_##name(const int n, const Dtype* x, Dtype* y) { \
    CHECK_GT(n, 0); CHECK(x); CHECK(y);\
    for (int i = 0; i < n; ++i) { \
      operation; \
    } \
  }


// output is 1 for the positives, 0 for zero, and -1 for the negatives
DEFINE_CAFFE_CPU_UNARY_FUNC(sign, y[i] = caffe_sign<Dtype>(x[i]));

// This returns a nonzero value if the input has its sign bit set.
// The name sngbit is meant to avoid conflicts with std::signbit in the macro.
// The extra parens are needed because CUDA < 6.5 defines signbit as a macro,
// and we don't want that to expand here when CUDA headers are also included.
DEFINE_CAFFE_CPU_UNARY_FUNC(sgnbit, \
    y[i] = static_cast<bool>((std::signbit)(x[i])));

DEFINE_CAFFE_CPU_UNARY_FUNC(fabs, y[i] = std::fabs(x[i]));

template <typename Dtype>
void caffe_cpu_scale(const int n, const Dtype alpha, const Dtype *x, Dtype* y);



#endif  // CAFFE_UTIL_MATH_FUNCTIONS_H_
