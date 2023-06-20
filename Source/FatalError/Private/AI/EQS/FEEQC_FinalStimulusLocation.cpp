// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/FEEQC_FinalStimulusLocation.h"
#include "AI/FEAIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UFEEQC_FinalStimulusLocation::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                  FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AActor* OwnerActor = Cast<AActor>((QueryInstance.Owner).Get());
	if(OwnerActor != nullptr)
	{
		AFEAIController* AIController = Cast<AFEAIController>(OwnerActor->GetInstigatorController());
		if(AIController != nullptr)
		{
			IFEAIDetectionInfoInterface* DetectionInfo = Cast<IFEAIDetectionInfoInterface>(AIController);
			if(DetectionInfo != nullptr)
			{
				UEnvQueryItemType_Point::SetContextHelper(ContextData, DetectionInfo->GetFinalStimulusLocation());
			}
		}
	}
}
