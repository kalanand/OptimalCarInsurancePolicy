bool tryMyCalc = false;
bool runningOverTest = true;

void drawHists() {

   TFile* train = TFile::Open("train.root");
   TTree* trtrain = (TTree*) train->Get("ntuple"); 

    //////---------- draw them & get ratio ------------
   TH2D* projA;
   TH2D* projB;
   TH2D* projC;
   TH2D* projD;
   TH2D* projE;
   TH2D* projF;
   TH2D* projG;


   if(tryMyCalc) {
      const int nbins = 22;
      double bins[nbins+1] = {-0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 
                              7.5, 8.5, 9.5, 10.5, 11.5, 12.5, 13.5, 
                              14.5, 15.5, 16.5, 17.5, 18.5, 19.5, 20.5, 100.5};
      TProfile2D* profA = new TProfile2D("profA", "", nbins, bins, 102, -1., 101.);
      TProfile2D* profB = new TProfile2D("profB", "", nbins, bins, 102, -1., 101.);
      TProfile2D* profC = new TProfile2D("profC", "", nbins, bins, 102, -1., 101.);
      TProfile2D* profD = new TProfile2D("profD", "", nbins, bins, 102, -1., 101.);
      TProfile2D* profE = new TProfile2D("profE", "", nbins, bins, 102, -1., 101.);
      TProfile2D* profF = new TProfile2D("profF", "", nbins, bins, 102, -1., 101.);
      TProfile2D* profG = new TProfile2D("profG", "", nbins, bins, 102, -1., 101.);

      profA->Sumw2();
      profB->Sumw2();
      profC->Sumw2();
      profD->Sumw2();
      profE->Sumw2();
      profF->Sumw2();
      profG->Sumw2();

      char* refsel = "record_type==0";
      trtrain->Draw("A:car_age:state_fips>>profA", refsel, "goff");
      trtrain->Draw("B:car_age:state_fips>>profB", refsel, "goff");
      trtrain->Draw("C:car_age:state_fips>>profC", refsel, "goff");
      trtrain->Draw("D:car_age:state_fips>>profD", refsel, "goff");
      trtrain->Draw("E:car_age:state_fips>>profE", refsel, "goff");
      trtrain->Draw("F:car_age:state_fips>>profF", refsel, "goff");
      trtrain->Draw("G:car_age:state_fips>>profG", refsel, "goff");

      projA =  profA->ProjectionXY("projA");
      projB =  profB->ProjectionXY("projB");
      projC =  profC->ProjectionXY("projC");
      projD =  profD->ProjectionXY("projD");
      projE =  profE->ProjectionXY("projE");
      projF =  profF->ProjectionXY("projF");
      projG =  profG->ProjectionXY("projG");

      char* opt = "colz";
      TCanvas* canA = new TCanvas();
      projA->Draw(opt);
      TCanvas* canB = new TCanvas();
      projB->Draw(opt);
      TCanvas* canC = new TCanvas();
      projC->Draw(opt);
      TCanvas* canD = new TCanvas();
      projD->Draw(opt);
      TCanvas* canE = new TCanvas();
      projE->Draw(opt);
      TCanvas* canF = new TCanvas();
      projF->Draw(opt);
      TCanvas* canG = new TCanvas();
      projG->Draw(opt);
   }

   char* infilename = "test.root";
   if(!runningOverTest) infilename = "train.root";
   TFile* input = TFile::Open(infilename);
   TTree* tree = (TTree*) input->Get("ntuple"); 
 
   float A, B, C, D, E, F, G, car_age;
   float record_type, shopping_pt, state_fips;
   int customer_ID;

   tree->SetBranchAddress("A", &A);
   tree->SetBranchAddress("B", &B);
   tree->SetBranchAddress("C", &C);
   tree->SetBranchAddress("D", &D);
   tree->SetBranchAddress("E", &E);
   tree->SetBranchAddress("F", &F);
   tree->SetBranchAddress("G", &G);
   tree->SetBranchAddress("car_age", &car_age);
   tree->SetBranchAddress("record_type", &record_type);
   tree->SetBranchAddress("shopping_pt", &shopping_pt);
   tree->SetBranchAddress("customer_ID", &customer_ID);
   tree->SetBranchAddress("state_fips", &state_fips);

   const int nNUM=10;
   int arrayA[nNUM];
   int arrayB[nNUM];
   int arrayC[nNUM];
   int arrayD[nNUM];
   int arrayE[nNUM];
   int arrayF[nNUM];
   int arrayG[nNUM];

   fillArray(arrayA, nNUM);
   fillArray(arrayB, nNUM);
   fillArray(arrayC, nNUM);
   fillArray(arrayD, nNUM);
   fillArray(arrayE, nNUM);
   fillArray(arrayF, nNUM);
   fillArray(arrayG, nNUM);

   int nsize=0;
   int previousID=-1;
   TString truepattern = "";
   int matched = 0; 
   int total = 0;
   int mymatched = 0; 
   int mytotal = 0;

   Long64_t nentries = tree->GetEntries();

   for (Int_t i = nentries-1; i <= (nentries+1); i++) {
      int entry = tree->GetEntry(i);
      if(entry<0) continue;
 
      if(record_type==1) truepattern = Form ("%d%d%d%d%d%d%d",(int)A, 
                                             (int)B, (int)C, (int)D, 
                                             (int)E, (int)F, (int)G);
      else {
         if(customer_ID==previousID) {
            arrayA[nsize] = (int) A;
            arrayB[nsize] = (int) B;
            arrayC[nsize] = (int) C;
            arrayD[nsize] = (int) D;
            arrayE[nsize] = (int) E;
            arrayF[nsize] = (int) F;
            arrayG[nsize] = (int) G;
            nsize++;
         }
         else if(i>0) {
            int modeA = GetMode(arrayA, nsize);
            int modeB = GetMode(arrayB, nsize);
            int modeC = GetMode(arrayC, nsize);
            int modeD = GetMode(arrayD, nsize);
            int modeE = GetMode(arrayE, nsize);
            int modeF = GetMode(arrayF, nsize);
            int modeG = GetMode(arrayG, nsize);

            TString pattern = Form ("%d%d%d%d%d%d%d", (int)modeA, (int)modeB, 
                                    (int)modeC, (int)modeD, (int)modeE, 
                                    (int)modeF, (int)modeG);

            if(pattern.CompareTo(truepattern)==0) matched++;
            total++;

            if(tryMyCalc) {
               int myA = (int)modeA;
               int myB = (int)modeB;
               int myC = (int)modeC;
               int myD = (int)modeD;
               int myE = (int)modeE;
               int myF = (int)modeF;
               int myG = (int)modeG;

               if(shopping_pt<5) {
                  myA = TMath::Nint(projA->Interpolate(state_fips,car_age));
                  myB = TMath::Nint(projB->Interpolate(state_fips,car_age));
                  myC = TMath::Nint(projC->Interpolate(state_fips,car_age));
                  myD = TMath::Nint(projD->Interpolate(state_fips,car_age));
                  myE = TMath::Nint(projE->Interpolate(state_fips,car_age));
                  myF = TMath::Nint(projF->Interpolate(state_fips,car_age));
                  myG = TMath::Nint(projG->Interpolate(state_fips,car_age));
               }

               TString myPattern = Form ("%d%d%d%d%d%d%d",myA,myB,myC,myD,myE,myF,myG);
               if(myPattern.CompareTo(truepattern)==0) mymatched++;
               mytotal++;

               cout << "customerID = " <<  previousID  
                    << ", true pattern = " << truepattern.Data() 
                    << ", mean pattern = " << pattern.Data() 
                    << ", my pattern = " << myPattern.Data() << endl;
            }

            if(!runningOverTest)
               cout << "customerID = " <<  previousID  
                    << ", true pattern = " << truepattern.Data() 
                    << ", pattern = " << pattern.Data() << endl;
            else 
               cout << previousID << "," << pattern.Data() << endl;

            nsize=0;
            fillArray(arrayA, nNUM);
            fillArray(arrayB, nNUM);
            fillArray(arrayC, nNUM);
            fillArray(arrayD, nNUM);
            fillArray(arrayE, nNUM);
            fillArray(arrayF, nNUM);
            fillArray(arrayG, nNUM);
            truepattern = "";
         }
      }
      previousID = customer_ID;
   }

   if(!runningOverTest)
      cout << "matched = " << matched << "\/" << total 
           << " = " << (float) (matched * 100)/ (float) total 
           << "%"<< endl; 
   
   if(!runningOverTest && tryMyCalc)
      cout << "my matched = " << mymatched << "\/" << mytotal 
           << " = " << (float) (mymatched * 100)/ (float) mytotal 
           << "%"<< endl; 
   
}





int GetMode(int daArray[], int iSize) {
    // Allocate an int array of the same size to hold the
    // repetition count
    int* ipRepetition = new int[iSize];
    for (int i = 0; i < iSize; ++i) {
        ipRepetition[i] = 0;
        int j = 0;
        bool bFound = false;
        while ((j < i) && (daArray[i] != daArray[j])) {
            if (daArray[i] != daArray[j]) {
                ++j;
            }
        }
        ++(ipRepetition[j]);
    }
    int iMaxRepeat = 0;
    for (int i = 1; i < iSize; ++i) {
        if (ipRepetition[i] > ipRepetition[iMaxRepeat]) {
            iMaxRepeat = i;
        }
    }
    delete [] ipRepetition;
    return daArray[iMaxRepeat];
}



void fillArray(int array[], int iSize) {

    for (int i = 0; i < iSize; ++i) {
       array[i] = -1;
    }
}
