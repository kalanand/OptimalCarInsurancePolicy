 /**********************************************************************
 * Copyright 2014 Kalanand Mishra                                      *
 *                                                                     *
 * Code to predict optimal car insurance policy. See for details:      *
     https://www.kaggle.com/c/allstate-purchase-prediction-challenge   *
 *                                                                     *
 * Uses ROOT - a freely distributed software: http://root.cern.ch      *
 **********************************************************************/

#include "PrintResult.C"

void Analyze() {

// --- convert the csv to ROOT files if needed ------
    makeTree("train");
    makeTree("test");

// --- prepare tmva training ------------
   char* trainFile = "train.root";
   char* testFile = "test.root";
   char* tree = "ntuple";
   char* mycuts = "abs(record_type-1)<0.1"; // signal  
   char*  mycutb = "abs(record_type)<0.1";  // background
 
   char* inputVars[] = { "state_fips", "location", "group_size", "homeowner", "car_age", "car_value_num", "risk_factor", "age_oldest", "age_youngest", "married_couple", "C_previous", "duration_previous", "A", "B", "C", "D", "E", "F", "G", "cost" };
   int size = sizeof(inputVars)/sizeof(inputVars[0]);
// TMVAClassification(trainFile, tree, mycuts, mycutb, inputVars, size); 

// --- compute output and print result in a CSV file ------------
   PrintResult(testFile, inputVars, size, "output.csv");
}




// Read data from an ascii file and create a root file with an ntuple.
void makeTree(char* txtFile) {

   char state[20];
   char car_value[20];
   float state_fips;
   float car_value_num;

   TFile *f = new TFile((TString(txtFile)+TString(".root")).Data(),"RECREATE");
   TTree *tree = new TTree("ntuple","data from ascii file");

   const char* branchDescriptor = "customer_ID/I:shopping_pt/F:record_type:day:time/C:state/C:location/F:group_size:homeowner:car_age:car_value/C:risk_factor/F:age_oldest:age_youngest:married_couple:C_previous:duration_previous:A:B:C:D:E:F:G:cost";

   Long64_t nlines = tree->ReadFile((TString(txtFile)+TString(".csv")).Data(), branchDescriptor);

   printf(" found %d points\n",nlines);


   tree->SetBranchAddress("state", state);
   tree->SetBranchAddress("car_value", car_value);
   TBranch *fips_branch = tree->Branch("state_fips", 
                                       &state_fips,"state_fips/F");
   TBranch *car_branch = tree->Branch("car_value_num", 
                                      &car_value_num,"car_value_num/F");

   Long64_t nentries = tree->GetEntries();

   for (Int_t i = 0; i < nentries; i++) {
      tree->GetEntry(i);

      state_fips = (float) GetStateFIPS(state);
      fips_branch->Fill();
      car_value_num = (float) GetCarValNum(car_value);
      car_branch->Fill();
   }


   tree->Write();
   f->Close();
}




/**********************************************************************************
 * Training and testing of the TMVA classifiers.                                  *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 **********************************************************************************/
 
void TMVAClassification(char* trainFile, char* tree, 
                        char* mycuts, char*  mycutb, char* inputVars[], int size) 
{
   // this loads the library
   TMVA::Tools::Instance();

   // Create a new root output file.
   TFile* outputFile = TFile::Open( "TMVA.root", "RECREATE" );

   // Create the factory object.
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, 
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D" );
   // ---------- input variables 
   for (int ivar = 0; ivar < size; ++ivar) {
     factory->AddVariable(inputVars[ivar], 'F');
   }

   // read training and test data
   TFile *input = TFile::Open( trainFile);
   TTree *signal     = (TTree*)input->Get(tree);
   TTree *background = (TTree*)input->Get(tree);
   

   // global event weights per tree 
   Double_t signalWeight     = 1.0;
   Double_t backgroundWeight = 1.0;

   // ====== register trees ====================================================
   // you can add an arbitrary number of signal or background trees
   factory->AddSignalTree    ( signal,     signalWeight     );
   factory->AddBackgroundTree( background, backgroundWeight );

   // tell the factory to use all remaining events in the trees after training for testing:
   factory->PrepareTrainingAndTestTree( TCut(mycuts), TCut(mycutb),
                                        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );

   // If no numbers of events are given, half of the events in the tree are used for training, and 
   // the other half for testing:

   // ---- Use BDT: Adaptive Boost
   factory->BookMethod( TMVA::Types::kBDT, "BDT", 
                        "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
 
   // ---- Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();    

   // Save the output
   outputFile->Close();

   std::cout << "==> TMVAClassification is done!" << std::endl;      

   delete factory;
}



int GetStateFIPS(char* state) {
   if(strcmp(state,"AL")==0) return(1);
   if(strcmp(state,"AK")==0) return(2);
   if(strcmp(state,"AZ")==0) return(4);
   if(strcmp(state,"AR")==0) return(5);
   if(strcmp(state,"CA")==0) return(6);
   if(strcmp(state,"CO")==0) return(8);
   if(strcmp(state,"CT")==0) return(9);
   if(strcmp(state,"DE")==0) return(10);
   if(strcmp(state,"FL")==0) return(12);
   if(strcmp(state,"GA")==0) return(13);
   if(strcmp(state,"HI")==0) return(15);
   if(strcmp(state,"ID")==0) return(16);
   if(strcmp(state,"IL")==0) return(17);
   if(strcmp(state,"IN")==0) return(18);
   if(strcmp(state,"IA")==0) return(19);
   if(strcmp(state,"KS")==0) return(20);
   if(strcmp(state,"KY")==0) return(21);
   if(strcmp(state,"LA")==0) return(22);
   if(strcmp(state,"ME")==0) return(23);
   if(strcmp(state,"MD")==0) return(24);
   if(strcmp(state,"MA")==0) return(25);
   if(strcmp(state,"MI")==0) return(26);
   if(strcmp(state,"MN")==0) return(27);
   if(strcmp(state,"MS")==0) return(28);
   if(strcmp(state,"MO")==0) return(29);
   if(strcmp(state,"MT")==0) return(30);
   if(strcmp(state,"NE")==0) return(31);
   if(strcmp(state,"NV")==0) return(32);
   if(strcmp(state,"NH")==0) return(33);
   if(strcmp(state,"NJ")==0) return(34);
   if(strcmp(state,"NM")==0) return(35);
   if(strcmp(state,"NY")==0) return(36);
   if(strcmp(state,"NC")==0) return(37);
   if(strcmp(state,"ND")==0) return(38);
   if(strcmp(state,"OH")==0) return(39);
   if(strcmp(state,"OK")==0) return(40);
   if(strcmp(state,"OR")==0) return(41);
   if(strcmp(state,"PA")==0) return(42);
   if(strcmp(state,"RI")==0) return(44);
   if(strcmp(state,"SC")==0) return(45);
   if(strcmp(state,"SD")==0) return(46);
   if(strcmp(state,"TN")==0) return(47);
   if(strcmp(state,"TX")==0) return(48);
   if(strcmp(state,"UT")==0) return(49);
   if(strcmp(state,"VT")==0) return(50);
   if(strcmp(state,"VA")==0) return(51);
   if(strcmp(state,"WA")==0) return(53);
   if(strcmp(state,"WV")==0) return(54);
   if(strcmp(state,"WI")==0) return(55);
   if(strcmp(state,"WY")==0) return(56);
   if(strcmp(state,"AS")==0) return(60);
   if(strcmp(state,"GU")==0) return(66);
   if(strcmp(state,"MP")==0) return(69);
   if(strcmp(state,"PR")==0) return(72);
   if(strcmp(state,"VI")==0) return(78);
   return 100;
}


int GetCarValNum(char* car_value) {

   if(strcmp(car_value,"a")==0) return 1;
   if(strcmp(car_value,"b")==0) return 2;
   if(strcmp(car_value,"c")==0) return 3;
   if(strcmp(car_value,"d")==0) return 4;
   if(strcmp(car_value,"e")==0) return 5;
   if(strcmp(car_value,"f")==0) return 6;
   if(strcmp(car_value,"g")==0) return 7;
   if(strcmp(car_value,"h")==0) return 8;
   if(strcmp(car_value,"i")==0) return 9;
   return 0;
}


