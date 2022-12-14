#ifndef NN_MODEL_H
#define NN_MODEL_H

#include <cassert>
#include <deque>
#include <iostream>
#include <math.h>

//#include "rosetta.h"
#include "memory_file.h"
#include "matrix.h"
#include "data.h"



//transfer functions
enum TRANSFER_FUNCTION
  {
    SIGMOID=1,
    TANSIG=2,
    LINEAR=3
  };

enum INPUT_PARAMETERS
  {
    SAND=1,
    SILT=2,
    CLAY=3,
    BD=4,
    TH33=5,
    TH15=6,
    GRAVEL=7,
    VGTHR=11,
    VGTHS=12,
    VGALP=13,
    VGNPAR=14
  };

enum OUTPUT_PARAMETERS
  {
    VGTHRIN=1,
    VGTHSIN=2,
    VGALPIN=3,
    VGNPARIN=4,
    KS=5,
    KUS=11,
    LPAR=12
  };

enum SCALE_IN
  {
    ASIS=1,
    DIVISION=2,
    LOGTR=3
  };

enum SCALE_OUT
  {
    ASISOUT=1,
    POW10=2,
    DIVOUT=3
  };




/* definitions for NN structures */
typedef struct
{
  int nin;
  int nlayer;
  int s1;
  int s2;
  int nout;
} NN_CONTROL; 


class WEIGHTS 
{
 public:
  MATRIX 	*w;
  VECTOR	*b;
 public:
  WEIGHTS();
  virtual ~WEIGHTS();
};


class NN_WEIGHTS 
{
 public:
  int index;
  std::deque<WEIGHTS> weightslist;
 public:
  NN_WEIGHTS();

};

class LAYER 
{
 public:
  int nnodes;
  int ttype;

 public:

  void tfunc(int transfer_type, VECTOR *in, VECTOR *out)
  {
    switch(transfer_type)
      {
      case SIGMOID:
	{
	  sigmoid(in,out);
	  break;
	}
      case TANSIG:
	{
	  tansig(in,out);
	  break;
	}
      case LINEAR:
	{
	  linear(in,out);
	  break;
	}

      default:
	{
	  std::cout << "Unknown transfer function function in read_net()" << std::endl;
	}
      }
  }

  void sigmoid(VECTOR *in, VECTOR *out)
  {
    for (int i=1;i<=in->rows;i++){
      out->vector[i]=1.0/(1.0+exp(-in->vector[i]));
    }
  }

  void tansig(VECTOR *in, VECTOR *out)
  {
    for (int i=1;i<=in->rows;i++)
      {
	out->vector[i]=-1.0+2.0/(1.0+exp(-2*in->vector[i]));
      }
  }

  void linear(VECTOR *in, VECTOR *out)
  {
    for (int i=1;i<=in->rows;i++)
      {
	out->vector[i]=in->vector[i];
      }
  }


};

class INOUT 
{
 public:
  int type;
  int scalenr_out,scalenr_in;
  //void		(INOUT::*scale)(double *,VECTOR *,double *); /* scaling function */
  VECTOR		*scalpar;
 public:
  INOUT();
  virtual ~INOUT();

  void scale_in(int scalenr, double *in,VECTOR *scalepar, double *out)
  {
    switch (scalenr)
      {
      case ASIS: 
	{
	  *out=*in;
	  break;
	}
      case DIVISION: 
	{
	  *out=*in/scalepar->vector[1];
	  break;
	}
      case LOGTR: 
	{
	  *out=log10(*in)/scalepar->vector[1]+scalepar->vector[2];
	  break;
	}
      default: 
	{
	  std::cout << "Unknown input scaling function in read_net()" << std::endl;
	  exit(1);
	}
      }
  };  
  
  void scale_out(int scalenr, double *in,VECTOR *scalepar, double *out)
  {
    switch (scalenr)
      {	
      case ASISOUT: 
	{
	  *out=*in;
	  break;
	}
      case POW10: 
	{
	  *out=scalepar->vector[1]*((*in)+scalepar->vector[2]);
	  break;
	}
      case DIVOUT: // I guess this is not really division
	{
	  *out=scalepar->vector[1]*((*in)+scalepar->vector[2]);
	  break;
	}
      default: 
	{
	  std::cout << "Unknown output scaling function nr: "  << scalenr << " in read_net()" << std::endl;	  
	  exit(1);
	}
      }
  }  
};



class NN_CONFIG 
{
 public:
  int netid;
  int nboot;
  int nlayer;
  std::deque<LAYER> layerlist;
  int nin;
  std::deque<INOUT> inputlist;
  int nout;
  std::deque<INOUT> outputlist;
 public:
  NN_CONFIG();
  virtual ~NN_CONFIG();
  int Get_nboot() { return(nboot);}
};

class NN_MODEL 
{
 public:
  
  NN_MODEL(char* memory_model, size_t size)
    {
      int res;
      mem_file = new Memory_File(memory_model, size);
      res=read_net();
    }
  
  ~NN_MODEL()
    {
    }

 protected:
  Memory_File* mem_file; 
  NN_CONFIG config;
  std::deque<NN_WEIGHTS> nn_netlist;
	
 public:
  MATRIX* nn_forward(RosettaInput *inputm);
  int calc_avg(MATRIX *output,VECTOR *average,VECTOR *stddev,MATRIX *cmat);
  int Get_nboot() {return(config.Get_nboot());}
 protected:
  int read_net();
  int read_nn_control(NN_CONTROL *control);


};




#endif
