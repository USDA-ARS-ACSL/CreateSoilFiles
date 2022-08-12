// data.h: 
#ifndef DATA_H
#define DATA_H

#include <list>
#include <stdio.h>
#include <string.h>

#include "rosetta.h"

#define MINBD	0.5
#define MAXBD	2.0
#define MINGRAVEL	0.0
#define MAXGRAVEL	100.0
#define MINSUM	99
#define MAXSUM	101
#define MINOC	0.0
#define MAXOC	100.0
#define MINTH33	0.0
#define MAXTH33 1.0
#define MINTH15 0.0
#define MAXTH15 1.0



#define MISSING -9.9
#define NDES	129

#include <ostream>
#include <stdlib.h>

typedef char DESCRIPTION[129]; 

class RosettaInput 
{
 public:
  //TXT_CLASS *txt_class;
  long long code;
  DESCRIPTION description;
  double bd;
  double gravel;
  double sand;
  double silt;
  double clay;
  double th33;
  double th1500;
 
  // input for unsatk prediction
  double vgthr;
  double vgths;
  double vgalp;
  double vgnpar;

  int ReadInputData(FILE *inputfile);

 public:
  int is_valid_ssc();
  int is_valid_bd();
  int is_valid_th33();
  int is_valid_th1500();
  int is_valid_gravel();
  int is_valid_VG4();

 public:
  RosettaInput()
    {
      initialize();
    }		
  ~RosettaInput()
    {
    }

  void Invalid()
  {
    initialize();
  }
  
 protected:

  void initialize()
    {
      code=0;
      strcpy_s(description,"none");
      sand=MISSING;
      silt=MISSING;
      clay=MISSING;
      bd=MISSING;
      gravel=MISSING;
      th33=MISSING;
      th1500=MISSING;
 
    }


std::ostream& operator<<(std::ostream& output)
{
  output << this->code;
  output << "   " << this->description;
  output << "   " << this->bd;
  output << "   " << this->gravel;
  output << "   " << this->sand;
  output << "   " << this->silt;
  output << "   " << this->clay;
  return output;
}

};

class RosettaOutput 
{
 public:
  int code;
  char description[NDES];
  ANN_MODEL ann_model; 
  
  double vgthr;
  double vgths;
  double vgalp;
  double vgnpar;
  double ks;
  double stdvgthr;
  double stdvgths;
  double stdvgalp;
  double stdvgnpar;
  double stdks;

  double unsks;
  double unsl;
  double stdunsks;
  double stdunsl;


  void initialize()
  {
    code=0;
    strcpy_s(description,"none");
    vgthr=MISSING;
    vgths=MISSING;
    vgalp=MISSING;
    vgnpar=MISSING;
    ks=MISSING;
    stdvgthr=MISSING;
    stdvgths=MISSING;
    stdvgalp=MISSING;
    stdvgnpar=MISSING;
    stdks=MISSING;
  }
  
 public:
  RosettaOutput()
    {
      initialize();
    }

  void Invalid()
    {
      initialize();
    }

};


class RosettaData
{
 public:
  RosettaInput Input;
  RosettaOutput Output;
 public:
  RosettaData(RosettaInput& in, RosettaOutput& out)
    {
      Input=in;
      Output=out;
    }

  RosettaData& operator=(const RosettaData &rhs)
    {
      this->Input = rhs.Input;
      this->Output = rhs.Output;
      return *this;
    }
  

};

//typedef std::list<RosettaData> DM_SOIL;

#endif
