#include <stdlib.h>
#include <cassert>
#include <vector>

#include "nn_model_mem.h"
#include "rosetta.h"
#include "txt_class.h"

// Input ERRORs
#define SSC_INVALID 1
#define BD_INVALID  2
#define TH33_INVALID  4
#define TH1500_INVALID  8
#define VG4_INVALID  16
#define MODEL_INVALID 32

class ROS_BASE
{
 public:
  ROS_BASE()
    {
      allocated=Init_Rosetta_Models();
    }
  int Make_Estimate(ANN_MODEL ann_model, RosettaInput& rosinput, RosettaOutput& rosoutput);
  // user can call this to see whether ALL models were allocated succesfully
  bool is_allocated()
  {
    if (allocated==0)
      return true;
    else 
      return false;
  }
  
 protected:
  // vectors that hold the models
  int allocated;
  std::vector<NN_MODEL> nn_model_ret;
  std::vector<NN_MODEL> nn_model_ks;
  std::vector<NN_MODEL> nn_model_unsk;
  TXT_CLASS *txt_class;

  int Init_Rosetta_Models();
  int transfer(VECTOR *average_vg4,VECTOR *std_vg4,MATRIX *cmat_vg4,VECTOR *average_ks,VECTOR *std_ks,MATRIX *cmat_ks,RosettaOutput *outputm);

};
