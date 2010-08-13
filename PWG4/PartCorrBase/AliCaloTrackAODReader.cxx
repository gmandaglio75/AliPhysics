
/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
/* $Id: $ */

//_________________________________________________________________________
// Class for reading data (AODs) in order to do prompt gamma
//  or other particle identification and correlations.
// Mixing analysis can be done, input AOD with events
// is opened in the AliCaloTrackReader::Init()
// 
//
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
//#include "Riostream.h"

//---- ANALYSIS system ----
#include "AliCaloTrackAODReader.h" 
#include "AliAODCaloCluster.h"
#include "AliAODTrack.h"
#include "AliESDtrack.h"
#include "AliFiducialCut.h"
#include "AliAODInputHandler.h"
#include "AliMultiEventInputHandler.h"
#include "AliAnalysisManager.h"
#include "AliMixedEvent.h"

ClassImp(AliCaloTrackAODReader)

//____________________________________________________________________________
AliCaloTrackAODReader::AliCaloTrackAODReader() : 
  AliCaloTrackReader()
{
  //Default Ctor
  
  //Initialize parameters
  fDataType=kAOD;
  fReadStack          = kTRUE;
  fReadAODMCParticles = kFALSE;
  //We want tracks fitted in the detectors:
  fTrackStatus=AliESDtrack::kTPCrefit;
  fTrackStatus|=AliESDtrack::kITSrefit;

}
/*
//____________________________________________________________________________
AliCaloTrackAODReader::AliCaloTrackAODReader(const AliCaloTrackAODReader & aodr) :   
  AliCaloTrackReader(aodr), fWriteOutputStdAOD(aodr.fWriteOutputStdAOD)
{
  // cpy ctor
}
*/
//_________________________________________________________________________
//AliCaloTrackAODReader & AliCaloTrackAODReader::operator = (const AliCaloTrackAODReader & source)
//{
//  // assignment operator
//
//  if(&source == this) return *this;
//
//  return *this;
//
//}


//____________________________________________________________________________
void AliCaloTrackAODReader::GetSecondInputAODVertex(Double_t  v[3]) const {
	//Return vertex position of second AOD input
	
	fSecondInputAODEvent->GetPrimaryVertex()->GetXYZ(v);

}

//____________________________________________________________________________
Double_t AliCaloTrackAODReader::GetBField() const {
  //Return magnetic field

  Double_t bfield = fInputEvent->GetMagneticField();

  return bfield;

}

//____________________________________________________________________________
void AliCaloTrackAODReader::SetInputOutputMCEvent(AliVEvent* input, AliAODEvent* aod, AliMCEvent* mc) {
  // Connect the data pointers
  // If input is AOD, do analysis with input, if not, do analysis with the output aod.

	  //printf("AODInputHandler %p, MergeEvents %d \n",aodIH, aodIH->GetMergeEvents());

  Bool_t tesd = kFALSE ; 
  Bool_t taod = kTRUE ; 
  if ( strcmp(input->GetName(), "AliMixedEvent") == 0 ) {
    AliMultiEventInputHandler* multiEH = dynamic_cast<AliMultiEventInputHandler*>((AliAnalysisManager::GetAnalysisManager())->GetInputEventHandler());
    if (multiEH->GetFormat() == 0 ) {
      tesd = kTRUE ; 
    } else if (multiEH->GetFormat() == 1) {
      taod = kTRUE ; 
    }
  }
  if (strcmp(input->GetName(),"AliESDEvent") == 0) {
    tesd = kTRUE ; 
  } else if (strcmp(input->GetName(),"AliAODEvent") == 0) {
    taod = kTRUE ; 
  }
  

  if(tesd)   {
    SetInputEvent(aod);
    SetOutputEvent(aod);
  }
  else if(taod){
    AliAODInputHandler* aodIH = dynamic_cast<AliAODInputHandler*>((AliAnalysisManager::GetAnalysisManager())->GetInputEventHandler());
	  if (aodIH && aodIH->GetMergeEvents()) {
		  //Merged events, use output AOD.
		  SetInputEvent(aod);
		  SetOutputEvent(aod);
	  }
	  else{
		  SetInputEvent(input);
		  SetOutputEvent(aod);
	  }
  }
  else{ 
    AliFatal(Form("AliCaloTrackAODReader::SetInputOutputMCEvent() - STOP : Wrong data format: %s\n",input->GetName()));
  }
  
  SetMC(mc);
  
}

//________________________________________________________________
void AliCaloTrackAODReader::Print(const Option_t * opt) const
{
	
	//Print some relevant parameters set for the analysis
	AliCaloTrackReader::Print(opt);
	
	printf("Write std AOD       =     %d\n", fWriteOutputStdAOD) ;
	
	printf("    \n") ;
} 

