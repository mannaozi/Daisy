// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DaisyUserWidget.h"


void UDaisyUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
