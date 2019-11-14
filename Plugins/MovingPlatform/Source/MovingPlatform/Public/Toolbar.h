// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"

#include "Toolbar.generated.h"

/**
 * 
 */
UCLASS()
class MOVINGPLATFORM_API UToolbar : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
	UToolbar();
	~UToolbar();

	UFUNCTION(BlueprintCallable)
		void Hello();

	UFUNCTION(BlueprintCallable)
		void PrintText();

	void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Message;
};
