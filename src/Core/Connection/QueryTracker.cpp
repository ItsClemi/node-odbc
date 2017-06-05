#include "stdafx.h"
#include "QueryTracker.h"

#ifdef _DEBUG
CQueryTracker::CQueryTracker( )
{

}

CQueryTracker::~CQueryTracker( )
{

}

void CQueryTracker::AddQuery( TQuery pQuery )
{
	std::lock_guard< std::mutex > l( m_cs );

	m_vecQuery.push_back( pQuery );
}

void CQueryTracker::RemoveQuery( TQuery pQuery )
{
	std::lock_guard< std::mutex > l( m_cs );

	auto it = std::find( m_vecQuery.begin( ), m_vecQuery.end( ), pQuery );
	assert( it != m_vecQuery.end( ) );

	m_vecQuery.erase( it );
}

#endif // _DEBUG
