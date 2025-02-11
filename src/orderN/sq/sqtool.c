/***
 * @file    sqtool.c
 * @brief   This file contains the functions for SQ method.
 *
 * @authors Xin Jing <xjing30@gatech.edu>
 *          Phanish Suryanarayana <phanish.suryanarayana@ce.gatech.edu>
 * 
 * Copyright (c) 2020 Material Physics & Mechanics Group, Georgia Tech.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <mpi.h>

#include "sqtool.h"


#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))


#define TEMP_TOL (1e-14)


void Vec_copy(int *a, int *b, int n) {
    int i;
    
    for (i = 0; i < n; i++)
        a[i] = b[i];
}


void Get_xyz_rs_counts(int *counts, int *layers, int *psize, int *x_counts, int *y_counts, int *z_counts) {
    int shift1 = 0, shift2 = 0;
    Get_reverse(counts, shift1, layers[0], x_counts, shift2);
    x_counts[layers[0]] = psize[0];

    shift1 += layers[0];
    shift2 += layers[0] + 1;

    Get_in_order(counts, shift1, layers[1], x_counts, shift2);

    shift1 += layers[1];
    shift2 = 0;
    Get_reverse(counts, shift1, layers[2], y_counts, shift2);
    y_counts[layers[2]] = psize[1];

    shift1 += layers[2];
    shift2 += layers[2] + 1;

    Get_in_order(counts, shift1, layers[3], y_counts, shift2);

    shift1 += layers[3];
    shift2 = 0;
    Get_reverse(counts, shift1, layers[4], z_counts, shift2);
    z_counts[layers[4]] = psize[2];

    shift1 += layers[4];
    shift2 += layers[4] + 1;
    Get_in_order(counts, shift1, layers[5], z_counts, shift2);
}


void Get_reverse(int *start, int shift1, int n, int *array, int shift2) {
    int i;

    for (i = 0; i < n; i ++){
        array[shift2 + i] = start[shift1 + n - 1 - i];
    }
}

void Get_in_order(int *start, int shift1, int n, int *array, int shift2) {
    int i;

    for (i = 0; i < n; i ++){
        array[shift2 + i] = start[shift1 + i];
    }
}

void Find_size_dir(int rem, int coords, int psize, int *small, int *large) {   
    if (coords < rem){
        *large = psize;
        *small = psize - 1;
    } else {
        *large = psize + 1;
        *small = psize;
    }
}


void VectorDotProduct_local(double ***v1, double ***v2, int Nd[3], double *val) {
    int ii, jj, kk; *val = 0.0;
    for (kk = 0; kk <= 2 * Nd[2]; kk++) {
        for (jj = 0; jj <= 2 * Nd[1]; jj++) {
            for (ii = 0; ii <= 2 * Nd[0]; ii++) { 
                *val = *val + v1[kk][jj][ii] * v2[kk][jj][ii];
            }
        }
    }
}


void Vector2Norm_local(double ***vec, int Nd[3], double *val) {
    int ii, jj, kk; * val = 0.0;
    for (kk = 0; kk <= 2 * Nd[2]; kk++) {
        for (jj = 0; jj <= 2 * Nd[1]; jj++) {
            for (ii = 0; ii <= 2 * Nd[0]; ii++) {
                *val = *val + vec[kk][jj][ii] * vec[kk][jj][ii];
            }
        }
    }
    *val = sqrt( *val);
}


void Vec3Dto1D(double ***vec_in, double *vec_out, int *dims) {
    int i, j, k, count;

    count = 0;
    for (k = 0; k < dims[2]; k++)
        for (j = 0; j < dims[1]; j++)
            for (i = 0; i < dims[0]; i++)
                vec_out[count++] = vec_in[k][j][i];
}

// stress_out length 6, stress_in length 9
void symmetrize_stress_SQ(double *stress_out, double *stress_in) {
#define stress_in(i,j) stress_in[i*3+j]

    stress_out[0] = stress_in(0,0);
    stress_out[1] = (stress_in(0,1) + stress_in(1,0)) / 2;
    stress_out[2] = (stress_in(0,2) + stress_in(2,0)) / 2;;
    stress_out[3] = stress_in(1,1);
    stress_out[4] = (stress_in(1,2) + stress_in(2,1)) / 2;
    stress_out[5] = stress_in(2,2);
#undef stress_in
}