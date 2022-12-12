#include <stdlib.h>
#include <atlstr.h>
#include <cassert>
#include <vector>
#include <fstream>


//#include <iostream>
#include <iomanip>
#include <sstream>  // needed for setw
//#include <string>
using namespace std;
#include "nn_model_mem.h"
#include "rosetta.h"
#include "txt_class.h"
#include "ros_base.h"
//input pass a file on the command line with sand silt clay bd and om information
// the first two are dummy numbers (from the k-nearest program
/*
layer sand silt clay bd om 
1   0.07     0.62     0.31     1.2    6.0 

9/9/2014 Modified to output new soil file structure that has texture and bulk density in the file as well.
There are some more write statements in nn_model_mem.cc, matrix and rose_base that I have to get rid of. EIther write to a log file
or comment them. The best thing to do will be to open and close the error file as I need to write. I will have to
add these later.
3/13/2018 the method to split off the extension by finding the '.' in the string did not work when the path
had a '.' in it. I replaced the find statement with _splitpath and _makepath, two intrinsic functions defined explicitly for that purpose
1/27/2019  added InitType to the layer file. This will be output to the nodal file and indicates if the water is input
  as head or theta (InitType='w' 'h')

*/

// The output file name uses and extension of .soi and the same root. 

int main(int argc, char* argv[])
{
	RosettaInput rosinput;
	RosettaOutput rosoutput;
	// need these for splitpath, variables in brackets are defined in libraries

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	int res;
	char* outFile= (char*)calloc(256,sizeof(char));
	char* inFile = (char*)calloc(256, sizeof(char));
	char* buffer = (char*)calloc(256, sizeof(char));
	CString errFile;
	int layerCount;
	float OM;
	string InitType;
	int Layer;
	layerCount=0;
	string line;
	inFile=argv[1];
	_splitpath(inFile, drive, dir, fname, ext);
	_makepath(outFile, drive, dir, fname, ".soi");
	errFile = "Rosetta.log";
	
	ofstream HProperties(outFile,ios::out);
	ofstream errorOut(errFile, ios::out);
	ifstream textureData(argv[1],ios::in);
	errorOut <<"hi there we are now in rosetta" <<std::endl;
	errorOut << "infile:" << inFile << std::endl;
	errorOut << "file name:" << outFile << "  Drive:" << drive << "  DIR:" << dir << "  fname:"<< fname;
	if (textureData)
	{

		//write headers
		HProperties <<   "           *** Material information ****                                                                   g/g  " << std::endl;
		HProperties << "   thr       ths         tha       thm      Alfa      n        Ks         Kk       thk       BulkD     OM    Sand    Silt    InitType" << std::endl;

		textureData.getline(buffer, 250, '\n');


		//Loop here over all layers
		while (!textureData.eof())
		{
			//getline(textureData,line);
			//sscanf(line.c_str(),"%i,%i,%d,%d,%d,%d,%d,$d"  dummy,dummy,rosinput.sand, rosinput.silt, rosinput.clay,
			//        rosinput.bd, dummy, dummy;,
			textureData >> Layer >> rosinput.sand >> rosinput.silt >> rosinput.clay >> rosinput.bd >> OM >>rosinput.th33 >> rosinput.th1500 >> InitType;
			if (!textureData.eof())
			{
				rosinput.sand=rosinput.sand*100;
				rosinput.silt=rosinput.silt*100;
				rosinput.clay= rosinput.clay*100;


				// allocate models
				ROS_BASE rosetta;

				// this is date from some source (user input, database, file)
				//rosinput.sand=07.0;
				//rosinput.silt=62.0;
				//rosinput.clay=31.0;
				//rosinput.bd=1.2;

				//ANN_MODEL ann_model=SSCBD;
				ANN_MODEL ann_model=SSCBDTH3315;
				if (rosinput.th1500 < 0 && rosinput.th33 > 0)
				{
					ann_model=SSCBDTH33;
				}

				if (rosinput.th1500 <0 && rosinput.th33 <0)
				{
					ann_model= SSCBD;
				}

				res=rosetta.Make_Estimate(ann_model, rosinput, rosoutput);
				if (res)  // anything not 0 is an error, thus rosoutput is bogus
				{
					errorOut <<"res : " << res << std::endl; 
					errorOut << "Something went wrong in making estimates" << std::endl;
					// handle this error
				}
				double vgths, vgthm, vgthk, vgkk, m;
				double Qee, Qs, Qs_temp, FH, FH1;
				int count_fit = 0;
				layerCount++;
				//DT 9-22-2022 based on a paper by Vogel et al.
				// Advances in Water Resources 24 (2001) 133±144
				// Eect of the shape of the soil hydraulic functions near saturation on
				// variably - saturated ¯ow predictions

				Qs_temp = 0.0;
				
				if (rosoutput.vgnpar < 2.0 && rosoutput.vgnpar>1.0) 
				{
					vgths = rosoutput.vgths;
					vgthk = rosoutput.vgths - 0.004;
					vgkk = rosoutput.ks  - (0.10 * rosoutput.ks);
					Qs = 0.0;
					m = 1.0 - 1.0 / rosoutput.vgnpar;
				//find ths value such that hSat will be in the range of -5 to -8 cm
					for (Qee = 0.930; Qee <= 1.0; Qee = Qee + .00001)
					{
						FH1 = pow(Qee, -1.0 / m) - 1.;
						FH = (-1.0 / rosoutput.vgalp) * pow(FH1, 1. / rosoutput.vgnpar);

						Qs_temp = ((rosoutput.vgths - rosoutput.vgthr)/Qee + 
							rosoutput.vgthr)* (FH < -2.0)* (FH > -5.0);
						Qs = Qs + Qs_temp;
						count_fit = count_fit + (Qs_temp > 0);
						//cout << "Qee, FH= Qs= " << Qee << " " << FH << " " << Qs_temp << endl;
					}
					vgthm = Qs / (double) count_fit;

				}
				else
				{
					vgthm = rosoutput.vgths;
					vgths = rosoutput.vgths;
					vgthk = rosoutput.vgths;
					vgkk = rosoutput.ks ;

				}
				//std::cout << "theta_r " << rosoutput.vgthr << std::endl;
				//std::cout << "theta_s " << rosoutput.vgths << std::endl;
				//std::cout << "alpha   " << rosoutput.vgalp << std::endl;
				//std::cout << "n par   " << rosoutput.vgnpar << std::endl;
				//std::cout << "Ks      " << rosoutput.ks << std::endl;
				//std::cout << "Ko      " << rosoutput.unsks << std::endl;
				//std::cout << "l par   " << rosoutput.unsl << "count->" << count << std::endl;
				HProperties 
					<< setiosflags(ios::right)
					<< setiosflags(ios::fixed)
					<<  setw(9) << setprecision (4)   << rosoutput.vgthr  
					<<  setw(9) << setprecision (4)  << vgths
					<<  setw(9) << setprecision (4)  << rosoutput.vgthr 
					<<  setw(9) << setprecision (4)  << vgthm
					<<  setw(11) << setprecision (5)  << rosoutput.vgalp
					<<  setw(11) << setprecision (5)  << rosoutput.vgnpar
					<<  setw(9) << setprecision (3)  << rosoutput.ks 
					<<  setw(9) << setprecision (4)  << vgkk
					<<  setw(9) << setprecision (4)  << vgthk 
					<<  setw(8) << setprecision (2)  << rosinput.bd
					<<  setw(8) << setprecision (5)  << OM
					<<  setw(8) << setprecision (2)  << rosinput.sand/100.0
					<<  setw(8) << setprecision (2)  << rosinput.silt/100.0
					<<  setw(3) << "   " << InitType
					<< std::endl;
			}

		}
		textureData.close();
		HProperties
		<< std::endl;
		HProperties.close();
		errorOut.close();
	}
	else
	{
		errorOut << "file open error";
		errorOut.close();
	}
	
}
