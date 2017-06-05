#pragma once

#ifdef _DEBUG
class CConnectionPool;
class CConnectionTracker
{
	typedef CConnectionPool* TPool;

public:
	CConnectionTracker( );
	~CConnectionTracker( );

public:
	void AddPool( TPool pPool );
	void RemovePool( TPool pPool );

public:
	void DetachFromJs( );

private:
	std::mutex m_cs;
	std::vector< TPool >	m_vecPool;

};
#endif // _DEBUG