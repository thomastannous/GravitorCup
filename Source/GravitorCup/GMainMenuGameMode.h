// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GGameInstance.h"
#include "GPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GMainMenuGameMode.generated.h"

UENUM(BlueprintType)
enum class ESelectedGameMode : uint8
{
	SG_1v1,
	SG_2v2,
	SG_None
};


/**
 *  AGMainMenuGameMode is responsible for most of the functionality in the main menu, e.g. check start conditions and player assignment
 */
UCLASS()
class GRAVITORCUP_API AGMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	//virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;
	//virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
	
	//virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	//virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	//virtual void PostLogin(APlayerController* NewPlayer) override;
	//virtual void InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer) override;

	
public:

	UPROPERTY()
	UGGameInstance* GGameInstance;

	// GameMode as selected in the main menu
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MainMenu")
	ESelectedGameMode SelectedGameMode;

	bool bIsTeamAssignmentActive;

	// Used to make selected data persist through level transitions (From menu to Gameplay or vice versa)
	UFUNCTION(BlueprintCallable)
	void StoreInGameInstance();

	UFUNCTION(BlueprintCallable)
	void RestoreFromGameInstance();

	UFUNCTION(BlueprintCallable, Category="MainMenu")
	bool CanStart1v1();
	
	UFUNCTION(BlueprintCallable, Category="MainMenu")
	bool CanStart2v2();

	UFUNCTION(BlueprintCallable)
	void SetSelectedGameMode(ESelectedGameMode NewSelectedGameMode);

	UFUNCTION(BlueprintCallable)
	void PlayerSelectTeam(class UGLocalPlayer *Player, int32 NewTeam);

	UFUNCTION(BlueprintNativeEvent, Category="MainMenu")
	void PlayerInputRightInTeamSelection();
	UFUNCTION(BlueprintNativeEvent, Category="MainMenu")
	void PlayerInputLeftInTeamSelection();
	

	void PlayerPressedMenuUp(AGPlayerController *PlayerController);
	void PlayerPressedMenuDown(AGPlayerController *PlayerController);
	void PlayerPressedMenuRight(AGPlayerController *PlayerController);
	void PlayerPressedMenuLeft(AGPlayerController *PlayerController);
	void PlayerPressedJoin(AGPlayerController *PlayerController);
	void PlayerPressedLeave(AGPlayerController *PlayerController);

	UFUNCTION(BlueprintCallable, Category="MainMenu")
	void ClearJoinedPlayers();

	UFUNCTION(BlueprintCallable, Category="MainMenu")
	void ChangeTeamForPlayer(UGLocalPlayer *GPL, int32 NewTeam);
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTeamSelectionGUI();
	
	UFUNCTION(BlueprintImplementableEvent)
	void WarnLobbyIsFull();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerJoin(AGPlayerController *GPlayerController);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLeave(AGPlayerController *GPlayerController);
};
