#include <stdlib.h>
#include <cassert>
#include <vector>

#include "nn_model_mem.h"
#include "rosetta.h"
#include "txt_class.h"
#include "ros_base.h"

// **************************
// *****GLOBAL VARIABLES*****
// **************************
// binary ANN files converted to hexadecimal arrays:
// less files to install. This stuff stays allocated 
// since it is static, cannot free.
// =================================================
// The folowing # includes are really declarations
// =================================================

#include "tsssc.c"
#include "tssscbd.c"
#include "tssscbdth33.c"
#include "tssscbdth3315.c"
#include "twssc.c"
#include "twsscbd.c"
#include "twsscbdth33.c"
#include "twsscbdth3315.c"
#include "newunsk.c"
#include "txt_rosetta.c"

int ROS_BASE::Init_Rosetta_Models()
  {
    // returns -1 if error (probably memory allocation problem)
    NN_MODEL *nn_model;
    //std::cout << "Allocating ANN retention models:\n";
    nn_model = new NN_MODEL((char *)twssc,sizeof(twssc));
    if (!nn_model) return -1;
    nn_model_ret.push_back(*nn_model); //[0]
    nn_model = new NN_MODEL((char *)twsscbd,sizeof(twsscbd));
    if (!nn_model) return -1;
    nn_model_ret.push_back(*nn_model); //[1]
    nn_model = new NN_MODEL((char *)twsscbdth33,sizeof(twsscbdth33));
    if (!nn_model) return -1;
    nn_model_ret.push_back(*nn_model); //[2]
    nn_model = new NN_MODEL((char *)twsscbdth3315,sizeof(twsscbdth3315));
    if (!nn_model) return -1;
    nn_model_ret.push_back(*nn_model); //[3]
    //std::cout << "done" << std::endl;
    
    //std::cout << "Allocating ANN Ks models:\n";
    nn_model = new NN_MODEL((char *)tsssc,sizeof(tsssc));
    if (!nn_model) return -1;
    nn_model_ks.push_back(*nn_model); //[0]
    nn_model = new NN_MODEL((char *)tssscbd,sizeof(tssscbd));
    if (!nn_model) return -1;
    nn_model_ks.push_back(*nn_model); //[1]
    nn_model = new NN_MODEL((char *)tssscbdth33,sizeof(tssscbdth33));
    if (!nn_model) return -1;
    nn_model_ks.push_back(*nn_model); //[2]
    nn_model = new NN_MODEL((char *)tssscbdth3315,sizeof(tssscbdth3315));
    if (!nn_model) return -1;
    nn_model_ks.push_back(*nn_model); //[3]
    //std::cout << "done" << std::endl;

    //std::cout << "Allocating ANN unsk model:\n";
    nn_model = new NN_MODEL((char *)newunsk,sizeof(newunsk));
    if (!nn_model) return -1;
    nn_model_unsk.push_back(*nn_model); //[0]
    //std::cout << "done" << std::endl;

    txt_class= new TXT_CLASS(txt_rosetta);
    if (!txt_class) return -1;

    return 0; // success
  }

int ROS_BASE::transfer(VECTOR *average_vg4,VECTOR *std_vg4,MATRIX *cmat_vg4,VECTOR *average_ks,VECTOR *std_ks,MATRIX *cmat_ks,RosettaOutput *outputm)
  {
    assert(outputm);
    
    outputm->vgthr=average_vg4->vector[1];  
    outputm->vgths=average_vg4->vector[2];
    outputm->vgalp=average_vg4->vector[3];  
    outputm->vgnpar=average_vg4->vector[4];  
    outputm->stdvgthr=std_vg4->vector[1];  
    outputm->stdvgths=std_vg4->vector[2];
    outputm->stdvgalp=std_vg4->vector[3];  
    outputm->stdvgnpar=std_vg4->vector[4];  
    outputm->ks=average_ks->vector[1];  
    outputm->stdks=std_ks->vector[1];  
    
    return(0);
  }

int ROS_BASE::Make_Estimate(ANN_MODEL ann_model, RosettaInput& rosinput, RosettaOutput& rosoutput)
{
  
  if (ann_model==TXT)
    {
      // simple lookup table
      TXT_INDEX class_index;
      
      if (rosinput.is_valid_ssc())
	{
	  // use the actual numbers rather than the combobox
	  class_index=txt_class->determine_texture_class(rosinput.sand, rosinput.silt, rosinput.clay);
	  
	  rosoutput.vgthr=txt_class->Get_Value(class_index,TXT_THR);
	  rosoutput.vgths=txt_class->Get_Value(class_index,TXT_THS);
	  rosoutput.vgalp=txt_class->Get_Value(class_index,TXT_ALPHA);
	  rosoutput.vgnpar=txt_class->Get_Value(class_index,TXT_NPAR);
	  rosoutput.ks=txt_class->Get_Value(class_index,TXT_KS);
	  rosoutput.stdvgthr=txt_class->Get_Value(class_index,TXT_THR_STD);
	  rosoutput.stdvgths=txt_class->Get_Value(class_index,TXT_THS_STD);
	  rosoutput.stdvgalp=txt_class->Get_Value(class_index,TXT_ALPHA_STD);
	  rosoutput.stdvgnpar=txt_class->Get_Value(class_index,TXT_NPAR_STD);
	  rosoutput.stdks=txt_class->Get_Value(class_index,TXT_KS_STD);		
	  rosoutput.ann_model=ann_model;
	  // do not return because we still most compute unsaturated K
	}
      else
	{
	  return (SSC_INVALID);
	}
    }
  else
    // the above block was moved out of the switch because some specialized ANN stuff happens
    // this switch has a __FALL_THROUGH__ structure (no 'break' statements!). So all four 
    // cases are checked for SSCBDTH1500, 3 for SSCBDTH33, etc.
    {     
      int res=0;

      switch (ann_model)
	{
	case SSCBDTH3315:
	  {
	    if (!rosinput.is_valid_th1500()) res|=TH1500_INVALID;
	  }
	case SSCBDTH33:
	  {
	    if (!rosinput.is_valid_th33()) res|=TH33_INVALID;
	  }
	case SSCBD:
	  {
	    if (!rosinput.is_valid_bd()) res|=BD_INVALID;
	  }
	case SSC:
	  {
	    if (!rosinput.is_valid_ssc()) res|=SSC_INVALID;
	    // break here, otherwise we go through the default
	    break;
	  }	
	default:
	  {
	    return (MODEL_INVALID);
	  }  
	}
      
      if (res) 
	{
	  // we do not have valid input data
	  return(res);
	}
      else
	{
	  int ann_index=ann_model-1; // this is because TXT model is #0

	  MATRIX *bootstrap_output_vg4=NULL;
	  VECTOR *average_vg4=new VECTOR(4);
	  VECTOR *std_vg4=new VECTOR(4);
	  MATRIX *corr_vg4=new MATRIX(4,4);
	  MATRIX *bootstrap_output_ks=NULL;
	  VECTOR *average_ks=new VECTOR(1);
	  VECTOR *std_ks=new VECTOR(1);
	  MATRIX *corr_ks=new MATRIX(1,1);

	  // this is where it all happens, first retention, then ks
	  bootstrap_output_vg4=nn_model_ret[ann_index].nn_forward(&rosinput);
	  nn_model_ret[ann_index].calc_avg(bootstrap_output_vg4,average_vg4,std_vg4,corr_vg4);
	  bootstrap_output_ks=nn_model_ks[ann_index].nn_forward(&rosinput);
	  nn_model_ks[ann_index].calc_avg(bootstrap_output_ks,average_ks,std_ks,corr_ks);
	  transfer(average_vg4,std_vg4,corr_vg4,average_ks,std_ks,corr_ks,&rosoutput);
	  rosoutput.ann_model=ann_model;

	  delete corr_vg4;
	  delete bootstrap_output_vg4;
	  delete average_vg4;
	  delete std_vg4;
	  delete bootstrap_output_ks;
	  delete average_ks;
	  delete std_ks;
	  delete corr_ks;
	}
    }// else for ANN models

  // normalize alpha, n,ks
  rosoutput.vgalp=pow(10.0,rosoutput.vgalp);
  rosoutput.vgnpar=pow(10.0,rosoutput.vgnpar);
  rosoutput.ks=pow(10.0,rosoutput.ks);

  rosinput.vgthr=rosoutput.vgthr;
  rosinput.vgths=rosoutput.vgths;
  // don't forget the antilog for these parameters
  //  rosinput.vgalp=pow(10,rosoutput.vgalp);
  //rosinput.vgnpar=pow(10,rosoutput.vgnpar);
  rosinput.vgalp=rosoutput.vgalp;
  rosinput.vgnpar=rosoutput.vgnpar;
 
  // needed for unsatK estimate
  if (rosinput.is_valid_VG4())
    {  
      //run ANN model for unsat-K
      MATRIX *bootstrap_output_unsk;
      VECTOR *average_unsk=new VECTOR(2);
      VECTOR *std_unsk=new VECTOR(2);
      MATRIX *corr_unsk=new MATRIX(2,2);

      bootstrap_output_unsk=nn_model_unsk[0].nn_forward(&rosinput);
      nn_model_unsk[0].calc_avg(bootstrap_output_unsk,average_unsk,std_unsk,corr_unsk);
      
      rosoutput.unsks=average_unsk->vector[1];
      rosoutput.unsl=average_unsk->vector[2];
      rosoutput.stdunsks=std_unsk->vector[1];
      rosoutput.stdunsl=std_unsk->vector[2];

      delete bootstrap_output_unsk;
      delete average_unsk;
      delete std_unsk;
      delete corr_unsk;
    }
  else
    {
      return VG4_INVALID;
    }
  // normalize unsks
  rosoutput.unsks=pow(10.0,rosoutput.unsks);

  // success
  return(0);
}
