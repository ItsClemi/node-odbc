#pragma once



enum class EForegroundResult : size_t
{
	eDiscard, eReschedule,
};



struct IUvOperation
{
	virtual void ProcessBackground( )
	{
		;
	}

	virtual EForegroundResult ProcessForeground( v8::Isolate* isolate )
	{
		return EForegroundResult::eDiscard;
	}
};
