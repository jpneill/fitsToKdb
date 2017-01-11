/*
 * * =================================================
 * *       Filename:  fitsToKDB.c
 * *
 * *    Description:  Functions for extracting columns from a binary table in a fits file.
 * *                  Returns extracted columns to q as lists
 * *    Unsupported:  Does not support extraction of image HDUs and string columns
 * * 
 * *        Version:  1.0
 * *        Created:  2016.11.01 20:23:25
 * *       Revision:  none
 * *       Compiler:  gcc
 * *
 * *         Author:  Mr. James Neill , jneill@kx.com
 * *        Company:  First Derivatives plc, Newry
 * *
 * * =================================================
 * */
#define KXVER 3
#include<string.h>
#include<stdio.h>
#include"fitsio.h"
#include"k.h"


/********************************************************************
* Name       :  K listHDUs(K x) 
* Description:  Function which prints a list of the number of HDUs and their types
* Parameters :  x - q symbol corresponding to the fits file name `$"example.fits"
*/
K listHDUs(K x)
{
  fitsfile *fptr;
  int i,hdunum,hdutype,status=0;
  ffopen(&fptr,x->s,READONLY,&status);
  ffthdu(fptr,&hdunum,&status);
  printf("Number of HDUs: %d\n",hdunum);
  for(i=1; i<=hdunum; i++)
    {
      ffmahd(fptr,i,&hdutype,&status);
      switch(hdutype)
        {
        case IMAGE_HDU:
          printf("HDU %d: IMAGE_HDU\n",i);
          break;
        case ASCII_TBL:
          printf("HDU %d: ASCII_TBL\n",i);
          break;
        case BINARY_TBL:
          printf("HDU %d: BINARY_TBL\n",i);
          break;
        }
    }
  return ki(0);
}

/********************************************************************
* Name       :  K num_rows(K x,K y)
* Description:  Function which returns number of rows in the first table from a fits file
* Parameters :  x - q symbol corresponding to the fits file name `$"exampleTable.fits"
*               y - int representing the HDU of the binary table
*/               
K num_rows(K x, K y)
{
  fitsfile *fptr;
  long nrows;
  int status = 0,hdunum,hdutype;
  fits_open_file(&fptr,x->s,READONLY,&status); // x->s converts q sym to c char*
  fits_movabs_hdu(fptr,y->i,&hdutype,&status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_num_rows(fptr, &nrows, &status);
  fits_close_file(fptr,&status);
  return ki(nrows);
}

/********************************************************************
* Name       :  K num_cols(K x, K y)
* Description:  Function which returns number of columns in the first table from a fits file
* Parameters :  x - q symbol corresponding to the fits file name `$"exampleTable.fits"
*               y - int representing the HDU of the binary table
*/               
K num_cols(K x, K y)
{
  fitsfile *fptr;
  int status = 0,hdunum,hdutype,ncols;
  fits_open_file(&fptr,x->s,READONLY,&status);
  fits_movabs_hdu(fptr,y->i,&hdutype,&status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_num_cols(fptr,&ncols,&status);
  fits_close_file(fptr,&status);
  return ki(ncols);
}

/********************************************************************
* Name       :  K cols(K x, K y)
* Description:  Function which returns a list of columns in a binary table in a fits file
* Parameters :  x - q symbol corresponding to the fits file name `$"example.fits"
*               y - int representing the HDU we're looking for
*/               
K cols(K x, K y)
{
  fitsfile *fptr;
  char colname[FLEN_VALUE],keyname[FLEN_KEYWORD];
  K cols=ktn(KS,0);
  int status=0,colnum,hdunum,hdutype,ncols;
  fits_open_file(&fptr,x->s,READONLY,&status);
  fits_movabs_hdu(fptr,y->i,&hdutype,&status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_num_cols(fptr,&ncols,&status);
  for(colnum=1; colnum<=ncols; colnum++)
    {
      fits_make_keyn("TTYPE",colnum,keyname,&status);
      fits_read_key(fptr,TSTRING,keyname,colname,NULL,&status);
      js(&cols,ss(colname));
    }
  fits_close_file(fptr,&status);
  return cols;
}

/********************************************************************
* Name       :  K getColType(K x, K y, K z)
* Description:  Function which returns a column type from a binary table in a fits file
* Parameters :  x - q symbol corresponding to the name of fits file 
*               y - q symbol corresponding to column name
*               z - q int representing the HDU we're looking at
*/               
K getColType(K x, K y, K z)
{
  fitsfile *fptr;
  char keyname[FLEN_KEYWORD],coltype[FLEN_VALUE];
  int status=0,hdunum=z->i,hdutype,ncols;
  int colnum=0;
  fits_open_file(&fptr,x->s,READONLY,&status);
  fits_movabs_hdu(fptr, hdunum, &hdutype, &status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_colnum(fptr,CASEINSEN,y->s,&colnum,&status);
  fits_make_keyn("TFORM",colnum,keyname,&status);
  fits_read_key(fptr,TSTRING,keyname,coltype,NULL,&status);
  fits_close_file(fptr,&status);
  return ks(coltype);
}


/********************************************************************
* Name       :  K readLongCol(K x,K y,K z,K h)
*               K readIntCol(K x,K y,K z,K h)
*               K readDoubleCol(K x,K y,K z,K h)
*               K readSymCol(K x,K y,K z,K h)
* Description:  Column extraction functions for different data types
*               Columns types extracted are:  c long->q long,c int->q int,c real and c float->q float
* Parameters:
* 		x - q symbol for fits file name
* 		y - q symbol for column name
* 		z - q long for number of rows to extract
* 		h - q int representing the number of the HDU we're looking at
*/
K readLongCol(K x,K y,K z,K h)
{
  fitsfile *fptr;
  long long arr[z->j];
  int colnum=0;
  int status=0,hdunum,hdutype,anynulls,i;
  float nullval;
  K list=ktn(KJ,0);
  fits_open_file(&fptr,x->s,READONLY,&status);
  fits_movabs_hdu(fptr,h->i,&hdutype,&status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_colnum(fptr,CASEINSEN,y->s,&colnum,&status);
  fits_read_col(fptr,TLONGLONG,colnum,1,1,z->j,&nullval,&arr,&anynulls,&status);
  for(i=0; i<z->i; i++)
    {
      ja(&list,&arr[i]);
    }
  return list;
}

K readIntCol(K x,K y,K z,K h)
{
  fitsfile *fptr;
  int arr[z->i];
  int colnum=0;
  int status=0,hdunum,hdutype,anynulls,i;
  float nullval;
  K list=ktn(KI,0);
  fits_open_file(&fptr,x->s,READONLY,&status);
  fits_movabs_hdu(fptr,h->i,&hdutype,&status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_colnum(fptr,CASEINSEN,y->s,&colnum,&status);
  fits_read_col(fptr,TINT,colnum,1,1,z->j,&nullval,&arr,&anynulls,&status);
  for(i=0; i<z->i; i++)
    {
      ja(&list,&arr[i]);
    }
  return list;
}

K readDoubleCol(K x,K y,K z,K h)
{
  fitsfile *fptr;
  double arr[z->i];
  int colnum=0;
  int status=0,hdunum,hdutype,anynulls,i;
  float nullval;
  K list=ktn(KF,0);
  fits_open_file(&fptr,x->s,READONLY,&status);
  fits_movabs_hdu(fptr,h->i,&hdutype,&status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_colnum(fptr,CASEINSEN,y->s,&colnum,&status);
  fits_read_col(fptr,TDOUBLE,colnum,1,1,z->j,&nullval,&arr,&anynulls,&status);
  for(i=0; i<z->i; i++)
    {
      ja(&list,&arr[i]);
    }
  return list;
}

// sym col reader is slower than the above as it reads inside the for loop
// finding a way to take it out of the loop should improve perf significantly
K readSymCol(K x,K y,K z,K h)
{
  fitsfile *fptr;
  char *val,value[1000],nullstr[]="*";
  int colnum=0;
  int status=0,hdunum,hdutype,anynul,i;
  float nullval;
  val=value;
  K list=ktn(KS,0);
  fits_open_file(&fptr,x->s,READONLY,&status);
  fits_movabs_hdu(fptr,h->i,&hdutype,&status);
  if(hdutype!=BINARY_TBL)
    {
      printf("Selected HDU is not accessible to this function.");
      return ki(0);
    }; // if hdu we're looking at isn't a binary table exit
  fits_get_colnum(fptr,CASEINSEN,y->s,&colnum,&status);
  for(i=1; i<=z->i; i++)
    {
      fits_read_col_str(fptr,colnum,i,1,1,nullstr,&val,&anynul,&status);
      js(&list,ss(value));
    }
  return list;
}
