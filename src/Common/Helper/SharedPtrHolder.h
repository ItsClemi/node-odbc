#pragma once



template< typename T >
class CSharedPtrHolder
{
	CSharedPtrHolder( std::shared_ptr< T > pInstance )
	{ 
		m_pInstance = pInstance;
	}

	~CSharedPtrHolder( )
	{

	}

private:
	std::shared_ptr< T >	m_pInstance;
};
