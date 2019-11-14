// Fill out your copyright notice in the Description page of Project Settings.


#include "Toolbar.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#define DEBUG_MSG(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}
#define DEBUG_TEXT(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(x, __VA_ARGS__));}

UToolbar::UToolbar()
{

}

UToolbar::~UToolbar()
{

}

void UToolbar::Hello()
{
	DEBUG_MSG("Hello");
}

void UToolbar::PrintText()
{
	DEBUG_TEXT(*this->Message.ToString());
}

void UToolbar::NativePreConstruct()
{
	Super::NativePreConstruct();

	// The default root widget is a canvas panel
	UWidget* RootWidget = GetRootWidget();
	UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(RootWidget);

	// Here we create a text block
	UTextBlock* TextBlock = NewObject<UTextBlock>();
	TextBlock->SetText(FText::FromString("Hello World"));

	// Then a button which contains the text block
	UButton* Button = NewObject<UButton>();
	Button->SetContent(TextBlock);
	Button->OnClicked.AddDynamic(this, &UToolbar::PrintText);

	CanvasPanel->AddChildToCanvas(Button);
}
