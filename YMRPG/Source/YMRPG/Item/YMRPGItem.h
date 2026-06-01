#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"

#include "YMRPGItem.generated.h"

/*
* 
* Base class for all items in the game. Do not Blueprint directly
*/
UCLASS(abstract, BlueprintType)
class YMRPG_API UYMRPGItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UYMRPGItem()
		:Price(0)
		,MaxCount(1)
		,MaxLevel(1)
	{}


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxCount;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
	bool IsConsumable()const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxLevel;

	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString()const;


	virtual FPrimaryAssetId GetPrimaryAssetId()const override;

};