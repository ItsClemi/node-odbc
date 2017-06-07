#include "stdafx.h"
#include "ConnectionTracker.h"

#include "Odbc/Connection/ConnectionPool.h"


#ifdef _DEBUG
CConnectionTracker::CConnectionTracker( )
{
}

CConnectionTracker::~CConnectionTracker( )
{
}

void CConnectionTracker::AddPool( TPool pPool )
{
	std::lock_guard< std::mutex > l( m_cs );

	m_vecPool.push_back( pPool );
}

void CConnectionTracker::RemovePool( TPool pPool )
{
	std::lock_guard< std::mutex > l( m_cs );

	auto it = std::find( m_vecPool.begin( ), m_vecPool.end( ), pPool );
	assert( it != m_vecPool.end( ) );

	m_vecPool.erase( it );
}

void CConnectionTracker::DetachFromJs( )
{
	m_cs.lock( );
	std::vector< TPool > v;
	std::copy( m_vecPool.begin( ), m_vecPool.end( ), std::back_inserter( v ) );
	m_cs.unlock( );

	for( auto& i : v )
	{
		auto pShared = i->GetSharedInstance( );

		pShared._Decref( );

		if( !pShared._Expired( ) )
		{
			pShared._Decref( );
		}
	}
}
#endif // _DEBUG

