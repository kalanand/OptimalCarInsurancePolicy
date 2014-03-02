 /**********************************************************************
 * Copyright 2014 Kalanand Mishra                                      *
 *                                                                     *
 * Code to predict optimal car insurance policy. See for details:      *
     https://www.kaggle.com/c/allstate-purchase-prediction-challenge   *
 *                                                                     *
 * Uses ROOT - a freely distributed software: http://root.cern.ch      *
 **********************************************************************/

void PrintResult(char* inFile, char* inputVars[], int size, 
                 char* outFile) {

   ComputeRawOutput(inFile, inputVars, size);

   TFile f((string("new_") + string(inFile).c_str()), "r");
   TTree* t = (TTree*) f.Get("ntuple");
   Long64_t nentries = t->GetEntries();     
  
   int customer_ID;
   char pattern[10];
   float mva;
  
   t->SetBranchAddress("customer_ID", &customer_ID);
   t->SetBranchAddress("pattern", pattern);
   t->SetBranchAddress("mva", &mva);
  
  
   int previousID = 0;
   float previousMVA = -10.;
   char previousPattern[10];

   FILE *file = fopen(outFile,"w");

   for (Int_t i = 0; i < nentries; i++) {
 
      t->GetEntry(i);
      // cout << "customer_ID = " << customer_ID 
      //      << ",  mva = " << mva << ",  pattern = " << pattern << endl;

      if((i>0 && !(customer_ID==previousID)) || i==nentries-1) {
         cout << "output: " << previousID << "," << previousPattern << endl;
         fprintf( file ,"%d,%s\n",previousID, previousPattern);
      }

      if(customer_ID==previousID) {
         if(mva > previousMVA) { 
            previousMVA = mva; 
            sprintf(previousPattern, "%s", pattern);
         }
      }
      else {
         previousMVA = mva; 
         sprintf(previousPattern, "%s", pattern);
      }   
      previousID = customer_ID;
   }
}



void ComputeRawOutput(char* inFile, char* inputVars[], int size)
{
   TFile f(inFile, "r");
   TTree* t = (TTree*) f.Get("ntuple");
   Long64_t nentries = t->GetEntries();     

   TString prefix = "new_";
   TFile fout( prefix + TString(inFile), "recreate");
   t->SetBranchStatus("*", kFALSE); // disable all 
   t->SetBranchStatus("customer_ID", 1); 
   TTree *newtree = t->CloneTree();
   t->SetBranchStatus("*", 1); // enable all back on 


   // declare input variable types
   int customer_ID;
   Float_t theInputVars[size];

   // initialize TMVA reader and tree branches 
   TMVA::Reader* reader = new TMVA::Reader();
   for (int ivar = 0; ivar < size; ++ivar) {
     factory->AddVariable(inputVars[ivar], theInputVars[ivar]);
     t->SetBranchAddress(inputVars[ivar], theInputVars[ivar]);
   }

   reader->BookMVA( "BDT", "Classification_BDT.weights.xml" );
   t->SetBranchAddress("customer_ID", &customer_ID);

   char pattern[10];
   float mva;

   TBranch *patt_branch = newtree->Branch("pattern", pattern,"pattern/C");
   TBranch *mva_branch = newtree->Branch("mva", &mva,"mva/F");
 
   const int nA = 3;
   float mA[nA] = {0, 1, 2};
   const int nB = 2;
   float mB[nB] = {0,1};
   const int nC = 4; 
   float mC[nC] = {1,2,3,4};
   const int nD = 3;
   float mD[nD] = {1,2,3};
   const int nE = 2;
   float mE[nE] = {0,1};
   const int nF=4;
   float mF[nF] = {0,1,2,3};
   const int nG=4;
   float mG[nG] = {1,2,3,4};


//   for (Int_t i = 0; i < 300; i++) {
   for (Int_t i = 0; i < nentries; i++) {

      if(i>0 && i%1000==0) 
         cout << "Completion = " << (float) (i) * 100/ (float) nentries << "%" <<endl;
      t->GetEntry(i);

      float A = theInputVars[12];
      float B = theInputVars[13];
      float C = theInputVars[14];
      float D = theInputVars[15];
      float E = theInputVars[16];
      float F = theInputVars[17];
      float G = theInputVars[18]; 

      TString vect0 = Form ("%d%d%d%d%d%d%d", (int)A, (int)B, (int)C, (int)D, (int)E, (int)F, (int)G);

      TString vect("");
      float mvaMax = -10.;

      for (int iA =0; iA < nA; ++iA) {
         for (int iB =0; iB < nB; ++iB) {
            for (int iC =0; iC < nC; ++iC) {
               for (int iD =0; iD < nD; ++iD) {
                  for (int iE =0; iE < nE; ++iE) {
                     for (int iF =0; iF < nF; ++iF) {
                        for (int iG =0; iG < nG; ++iG) {
                           A = mA[iA];
                           B = mB[iB];
                           C = mC[iC];
                           D = mD[iD];
                           E = mE[iE];
                           F = mF[iF];
                           G = mG[iG];
                           float mvaValue = reader->EvaluateMVA( "BDT" );
                           if(mvaValue > mvaMax) { 
                              mvaMax = mvaValue; 
                              vect = Form ("%d%d%d%d%d%d%d", (int)A, (int)B, (int)C, (int)D, (int)E, (int)F, (int)G);
                           }
                        }
                     }
                  }  
               }
            }
         }
      }


      mva = mvaMax;
      vect = vect + "\0";
 
      sprintf(pattern, "%s",  vect.Data());

      // cout << "customer_ID "<< customer_ID << ", shopping_pt " 
      //      << shopping_pt <<  ", MaxBDT = " << mva 
      //      << ",  pattern = " << pattern << endl;
      
      patt_branch->Fill();
      mva_branch->Fill();
   }

   fout.cd();
   newtree->Write();
   fout.Close();
   f.Close();
}
