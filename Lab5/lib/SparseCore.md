# Dual-side Sparse Tensor Core

## Abstract

&emsp;Leveraging sparsity in deep neural network (DNN) models is promising for accelerating model inference.

&emsp;Yet existing GPUs can only leverage the sparsity from weights but not activations, which are dynamic, unpredictable, and hence challenging to exploit.

&emsp;We take a symmetric approach to understand the (dis)advantages of previous sparsity-related architectures and propose a novel, unexplored paradigm that combines outer-product computation primitive and bitmap-based encoding for mat.

&emsp;We propose a set of novel ISA extensions and co-design the matrix-matrix multiplication and convolution algorithms, which are the two dominant computation patterns in today's DNN models, to exploit our new dual-side sparse Tensor Core.

## Introduction

&emsp;Especially, many AI applications have a stringent constraint of service level agreement. Running models at high-scale, low-latency, and high energy efficiency has always been extremely describe.

&emsp;Model compression and sparsification have become ***critical optimizations to reduce the number of parameters as well as arithmetic operations and to improve the computational and energe efficiency on various harware platform.***

&emsp;In particular, sparse Tensor Core, is newly invented to leverage the weight sparsity in DNN models.

&emsp;Besides the weight sparsity, DNN models also exhibit another form of sparsity called *activation sparsity, which is introduced by activation functions and is widely embedded in activation feature maps, for both computer vision and natural language processing.*

&emsp;The wide applicability of GPUs requires the support of both sparse general matrix-matrix multiplication (SpGEMM) and sparse convolution (SpCONV).
&emsp;Those two are key computation kernels in today's DNN models ranging from convolutional neural networks (CNNs), recurrent neural networks (RNN), to attention-based neural networks.

&emsp;In this work, we aim to accerlerate both dual-side SpCONV and SpGEMM on Tensor Core.

&emsp;Sparse Tensor-Core resolves the irregularity of weight sparsity by applying a strutural prunning scheme, which enforces a constant $50\%$ weight sparsity to balance the workload and to exploit parallelism in the dot-product unit.

&emsp;SparTen and Extensor accelerate inner-product by designing dedicated hardware for the inner joint process, which figures out non-zero elements by matching positions in two sparse vectors and accessing those elements.

&emsp; GPUs usually transform a CONV operator into a GEMM operatorr via the im2col method. Sparse Tensor Core, only leverages weight sparsity and the imput remains dense, which requires only dense im2col.

---
The key technical contributions of this work are as follows:

+ We propose a novel method that combines outer product and itmap encoding to accelerate SpGEMM and SpCONV.

</br>

+ We also propose novel instruction set extensions that let us leverage the existing high-performance libraries to accelerate SpGEMM and SpCONV.

## Background and related work

+ Opportunity of sparsity in DNNs
+ Computational kernels
+ Design philosophy and challenges

## bitmap-based SpGEMM

We propose an outer-product based algorithm to accelerate SpGEMM using the bitmap-based sparse encoding format.

+ Overview 

&emsp;To exploit the dual-side sparsity, we propose an efficient SpGEMM algorithm based on outer-product matrix multiplication. 

&emsp;A basic step in outer-product-based matrix multiplication is to compute a cross product between a column of A (sized of $M\times1$) and a row of B (sized of $1\times N$), which leads to the output $M\times N$ partial matrix.

&emsp;To generate the final output, we need to accumulate all those partial results and bias matrix $\mathbb{C}$ with multiple rounds.

&emsp;Each input matrix is represented by a two-tuple encoding of a bitmap, and a collection of non-zero values. The bitmap uses 1's for positions of non-zero values and 0's for zeros. To support outer-product, matrix $\mathbb{A}_v$ is encoded in column-major and $\mathbb{B}_v$ is encoded in row-major.
&emsp;The proposed SpGEMM algorithm has three major operations on the bitmap encoded matrices, which are *multiply-value*, *multiply-bitmap* and *merge* respectively. The *multiply-value* operation computes the cross-product on each vector-vector pair of $\mathbb{A}_v$ and $\mathbb{B}_v$ to generate values of the partial matrices.
&emsp;The output bitmap contains the sparsity information of the corresponding partial matrix, such as $\mathbb{D}1_b$ to $\mathbb{D}3_b$
&emsp;We propose a gather-scatter method to merge the non-zero values from different partial matrices with multiple rounds. Then we propose a SpGEMM algorithm for the outer-product  Tensor Cores in a warp. At last, we extend to the whole device.

+ SpGEMM in a Warp
 Each tensor core can complete a $4\times 4\times 4$ dense matrix multiplication per cycle in a 4-stage pipeline. The basic computation unit in a dense tansor core is a parallel 4-element vector-vector dot product unit that multiplies and accumulates A matrix row and B matrix column.

 To solve the irregular addressing introduced in sparse models, sparse tensor core uses a structural prunning scheme that conducts a 2-out-of-4-prunning in each partitioned sub-vector, which enforces a constant $50\%$ weight sparsity to balance the workload and to exploit parallelism in the dot-product unit, as shown in Figure 3b. 

 ![Sparse　Core]()