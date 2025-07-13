#pragma once

namespace Debug
{
	static void Print(const FString& DebugMessage)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, DebugMessage);
		}
	}
}
