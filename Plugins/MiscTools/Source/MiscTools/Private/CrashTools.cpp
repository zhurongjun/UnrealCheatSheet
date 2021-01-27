#include "CrashTools.h"
#include "SocketSubsystem.h"
#include "SocketSubsystemModule.h"
#include "HAL/PlatformAtomics.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"

FCrashTools::FCrashTools(FOutputDeviceError* InOldDevice): OldDevice(InOldDevice)
{
	check(InOldDevice);

	// build socket 
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	Socket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("Crash Send Socket"), true);

	// build address
	FIPv4Address TargetAddress;
	FIPv4Address::Parse(TEXT("127.0.0.1"), TargetAddress);
	SendTarget = SocketSubsystem->CreateInternetAddr(TargetAddress.Value, 12345);
}

void FCrashTools::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, const double Time)
{
	OldDevice->Serialize(V, Verbosity, Category, Time);

    if(!GIsGuarded)
    {
    	HandleError();
    }
}

void FCrashTools::HandleError()
{
	OldDevice->HandleError();

	// make sure we don't report errors twice
	static int32 CallCount = 0;
	int32 NewCallCount = FPlatformAtomics::InterlockedIncrement(&CallCount);
	if (NewCallCount != 1)
	{
		return;
	}

	// send error info
	GErrorHist[UE_ARRAY_COUNT(GErrorHist)-1]=0;
	int32 SentLen = 0;
	char UTF8Buf[UE_ARRAY_COUNT(GErrorHist) * 2];
	int32 SrcLen = FCString::Strlen(GErrorHist);
	int32 DataLen = FTCHARToUTF8_Convert::ConvertedLength(GErrorHist, SrcLen);
	UTF8Buf[DataLen] = 0;
	FTCHARToUTF8_Convert::Convert(UTF8Buf, UE_ARRAY_COUNT(GErrorHist) * 2, GErrorHist, UE_ARRAY_COUNT(GErrorHist));
	Socket->SendTo((uint8*)UTF8Buf, DataLen + 1, SentLen, *SendTarget);
}

void FCrashTools::Flush()
{
	OldDevice->Flush();
}

void FCrashTools::TearDown()
{
	OldDevice->TearDown();
}

void FCrashTools::Dump(FArchive& Ar)
{
	OldDevice->Dump(Ar);
}

bool FCrashTools::IsMemoryOnly() const
{
	bool bReturnVal = false;
	bReturnVal = OldDevice->IsMemoryOnly();

	return bReturnVal;
}

bool FCrashTools::CanBeUsedOnAnyThread() const
{
	bool bReturnVal = false;
	bReturnVal = OldDevice->CanBeUsedOnAnyThread();

	return bReturnVal;
}
