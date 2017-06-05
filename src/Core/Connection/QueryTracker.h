#pragma once

#ifdef _DEBUG
class CQuery;
class CQueryTracker
{
	typedef CQuery* TQuery;

public:
	CQueryTracker( );
	~CQueryTracker( );

public:
	void AddQuery( TQuery pQuery );
	void RemoveQuery( TQuery pQuery );


private:
	std::mutex m_cs;
	std::vector< TQuery >	m_vecQuery;
	
};
#endif // _DEBUG