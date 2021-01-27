#pragma once
#include "CoreMinimal.h"
#include "Sockets.h"
#include "Misc/OutputDeviceError.h"

class FCrashTools : public FOutputDeviceError
{
public:
	FCrashTools(FOutputDeviceError* InOldDevice);

	// handle errors
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, const double Time) override;
	virtual void HandleError() override;

	// just reroute 
	virtual void Flush() override;
	virtual void TearDown() override;
	virtual void Dump(class FArchive& Ar) override;
	virtual bool IsMemoryOnly() const override;
	virtual bool CanBeUsedOnAnyThread() const override;

	// implement pure virtual function 
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override {}
private:
	FOutputDeviceError*			OldDevice;
	FSocket*					Socket;
	TSharedPtr<FInternetAddr>	SendTarget;
};
