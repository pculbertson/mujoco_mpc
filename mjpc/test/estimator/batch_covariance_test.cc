// Copyright 2023 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <absl/random/random.h>
#include <mujoco/mujoco.h>

#include "gtest/gtest.h"
#include "mjpc/estimators/batch.h"
#include "mjpc/test/load.h"
#include "mjpc/utilities.h"

namespace mjpc {
namespace {

// TEST(Covariance, PriorUpdate) {
//   // ----- set up ----- //
//   // dimensions
//   // int dblock = 20;
//   // int nblock = 3;
//   // int T = 32;

//   int dblock = 2;
//   // int nblock = 3;
//   int T = 5;
//   int ntotal = dblock * T;

//   // conditioning split
//   // int Nprior = 24;
//   int Nprior = 3;
//   int Nposterior = T - Nprior;
//   int nprior = Nprior * dblock;
//   int nposterior = Nposterior * dblock;

//   // create random band matrix
//   std::vector<double> F(ntotal * ntotal);
//   std::vector<double> A_(ntotal * ntotal);
//   std::vector<double> A(ntotal * ntotal);

//   // sample matrix square root
//   absl::BitGen gen_;
//   for (int i = 0; i < ntotal * ntotal; i++) {
//     F[i] = absl::Gaussian<double>(gen_, 0.0, 1.0);
//   }

//   // A = F' F
//   mju_mulMatTMat(A_.data(), F.data(), F.data(), ntotal, ntotal, ntotal);

//   // band Hessian 
//   // DenseToBlockBand(A.data(), A_.data(), ntotal, dblock, nblock);
//   mju_copy(A.data(), A_.data(), ntotal * ntotal);

//   printf("Hessian:\n");
//   mju_printMat(A.data(), ntotal, ntotal);

//   // Hessian factor 
//   std::vector<double> factor(ntotal * ntotal);
//   mju_copy(factor.data(), A.data(), ntotal * ntotal);

//   int rank = mju_cholFactor(factor.data(), ntotal, 0.0);
  
//   printf("rank = %i / %i\n", rank, ntotal);

//   // ----- update ----- //
//   // compute covariance (inverse Hessian)
//   // TODO(taylor): E = [Ea, Eb]
//   std::vector<double> E_(ntotal * ntotal);
//   std::vector<double> E(ntotal * ntotal);

//   std::vector<double> I(ntotal * ntotal);
//   mju_eye(I.data(), ntotal);

//   for (int i = 0; i < ntotal; i++) {
//     mju_cholSolve(E_.data() + ntotal * i, factor.data(), I.data() + ntotal * i, ntotal);
//   }

//   // band covariance 
//   // DenseToBlockBand(E.data(), E_.data(), ntotal, dblock, nblock);
//   mju_copy(E.data(), E_.data(), ntotal * ntotal);

//   printf("covariance: \n");
//   mju_printMat(E.data(), ntotal, ntotal);

//   // unpack E11, E12, E21, E22 
//   std::vector<double> E11(nprior * nprior);
//   std::vector<double> E12(nprior * nposterior);
//   std::vector<double> E21(nposterior * nprior);
//   std::vector<double> E22(nposterior * nposterior);
//   BlockFromMatrix(E11.data(), E.data(), nprior, nprior, ntotal, ntotal, 0, 0);
//   BlockFromMatrix(E12.data(), E.data(), nprior, nposterior, ntotal, ntotal, 0, nprior);
//   BlockFromMatrix(E21.data(), E.data(), nposterior, nprior, ntotal, ntotal, nprior, 0);
//   BlockFromMatrix(E22.data(), E.data(), nposterior, nposterior, ntotal, ntotal, nprior, nprior);

//   printf("E11:\n");
//   mju_printMat(E11.data(), nprior, nprior);

//   printf("E12:\n");
//   mju_printMat(E12.data(), nprior, nposterior);

//   printf("E21:\n");
//   mju_printMat(E21.data(), nposterior, nprior);

//   printf("E22:\n");
//   mju_printMat(E22.data(), nposterior, nposterior);

//   // -- \hat E = E22 - E12' E11^-1 E12 -- // 

//   // factor 
//   rank = mju_cholFactor(E11.data(), nprior, 0.0);
//   printf("rank = %i / %i\n", rank, nprior);

//   // E11^-1 
//   std::vector<double> E11inv(nprior * nprior);
//   mju_eye(I.data(), nprior);

//   for (int i = 0; i < nprior; i++) {
//     mju_cholSolve(E11inv.data() + nprior * i, E11.data(), I.data() + nprior * i, nprior);
//   }

//   // tmp0 = E11^-1 E12
//   std::vector<double> tmp0(nprior * nposterior);
//   mju_mulMatMat(tmp0.data(), E11inv.data(), E12.data(), nprior, nprior, nposterior);

//   // tmp1 = E12' * tmp0 
//   std::vector<double> tmp1(nposterior * nposterior);
//   mju_mulMatTMat(tmp1.data(), E12.data(), tmp0.data(), nprior, nposterior, nposterior);

//   printf("E12' E11^-1 E12:\n");

//   // tmp2 = E11 \ E12
//   std::vector<double> tmp2(nprior * nposterior);
//   for (int i = 0; i < nposterior; i++) {
//     mju_cholSolve(tmp2.data() + nprior * i, E11.data(), E21.data() + nprior * i, nprior);
//   }

//   // tmp3 = E12' tmp2 
//   std::vector<double> tmp3(nposterior * nposterior);
//   mju_mulMatMatT(tmp3.data(), E21.data(), tmp2.data(), nposterior, nprior, nposterior);

//   // compare
//   printf("inverse: E12' E11^-1 E12\n");
//   mju_printMat(tmp1.data(), nposterior, nposterior);

//   printf("factor solve: E12' E11 '\' E12\n");
//   mju_printMat(tmp3.data(), nposterior, nposterior);

//   // E22 - tmp3 
//   std::vector<double> Ehat(nposterior * nposterior);
//   mju_sub(Ehat.data(), E22.data(), tmp3.data(), nposterior * nposterior);

//   printf("update:\n");
//   mju_printMat(Ehat.data(), nposterior, nposterior);

//   // band updated covariance

//   // -- compute prior weight: P = (\hat E)^-1 -- // 

//   // factor 
//   mju_cholFactor(Ehat.data(), nposterior, 0.0);

//   // solve 
//   mju_eye(I.data(), nposterior);

//   std::vector<double> P(nposterior * nposterior);
//   for (int i = 0; i < nposterior; i++) {
//     mju_cholSolve(P.data() + nposterior * i, Ehat.data(), I.data() + nposterior * i, nposterior);
//   }

//   printf("updated weight\n");
//   mju_printMat(P.data(), nposterior, nposterior);
// }

}  // namespace
}  // namespace mjpc
