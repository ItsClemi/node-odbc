#pragma once

namespace node
{
	template< class TypeName, class M >
	inline v8::Local< TypeName > StrongPersistentToLocal( const v8::Persistent< TypeName, M >& persistent )
	{
		return *reinterpret_cast< v8::Local< TypeName >* >( const_cast< v8::Persistent< TypeName, M >* >( &persistent ) );
	}

	template< class TypeName, class M >
	inline v8::Local< TypeName > WeakPersistentToLocal( v8::Isolate* isolate, const v8::Persistent< TypeName, M >& persistent )
	{
		return v8::Local< TypeName >::New( isolate, persistent );
	}


	template< class TypeName, class M = v8::NonCopyablePersistentTraits< TypeName > >
	inline v8::Local< TypeName > PersistentToLocal( v8::Isolate* isolate, const v8::Persistent< TypeName, M >& persistent )
	{
		if( persistent.IsWeak( ) )
		{
			return WeakPersistentToLocal( isolate, persistent );
		}
		else
		{
			return StrongPersistentToLocal( persistent );
		}
	}

}
