/*
 *  VS_LinkedListStore.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 17/10/09.
 *  Copyright 2009 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_LINKED_LIST_STORE_H
#define VS_LINKED_LIST_STORE_H

template<class T>
class vsListStoreEntry
{
public:
	T *					m_item;
	
	vsListStoreEntry<T> *	m_next;
	vsListStoreEntry<T> *	m_prev;
	
	vsListStoreEntry() : m_item(NULL), m_next(NULL), m_prev(NULL) {}
	vsListStoreEntry( T *t ) : m_item(t), m_next(NULL), m_prev(NULL) {}
	~vsListStoreEntry() 
	{ 
		Extract(); 
		vsDelete(m_item); 
	}
	
	void	Append( vsListStoreEntry<T> *next )
	{
		next->m_next = m_next;
		next->m_prev = this;
		
		if ( m_next )
		{
			m_next->m_prev = next;
		}
		m_next = next;
	}
	
	void	Prepend( vsListStoreEntry<T> *prev )
	{
		prev->m_next = this;
		prev->m_prev = m_prev;
		
		if ( m_prev )
		{
			m_prev->m_next = prev;
		}
		m_prev = prev;
	}
	
	void	Extract()
	{
		if ( m_next )
			m_next->m_prev = m_prev;
		if ( m_prev )
			m_prev->m_next = m_next;
	}
};

template<class T>
class vsListStoreIterator
{
	vsListStoreEntry<T>	*	m_current;
	
public:
	
	vsListStoreIterator( vsListStoreEntry<T> *initial ): m_current(initial) {}
	
	T *				Get() 
	{ 
		if ( m_current ) 
		{
			return m_current->m_item;
		} 
		return NULL; 
	}
	
	bool			Next() 
	{ 
		m_current = m_current->m_next; 
		return (m_current != NULL); 
	}
	
	bool			Previous() 
	{ 
		m_current = m_current->m_prev; 
		return (m_current != NULL); 
	}

	void	Append( T *item )
	{
		vsListStoreEntry<T> *ent = new vsListStoreEntry<T>( item );
		m_current->Append( ent );
	}

	void	Prepend( T *item )
	{
		vsListStoreEntry<T> *ent = new vsListStoreEntry<T>( item );
		m_current->Prepend( ent );
	}
	
	bool						operator==( const vsListStoreIterator &b ) { return (m_current->m_item == b.m_current->m_item); }
	bool						operator!=( const vsListStoreIterator &b ) { return !((*this)==b); }
	vsListStoreIterator<T>&		operator++() { Next(); return *this; }
	vsListStoreIterator<T>		operator++(int postFix) { vsListStoreIterator<T> other(m_current); Next(); return other; }
	vsListStoreIterator<T>&		operator--() { Previous(); return *this; }
	vsListStoreIterator<T>		operator--(int postFix) { vsListStoreIterator<T> other(m_current); Previous(); return other; }
};

template<class T>T*	operator*(vsListStoreIterator<T> i) { return i.Get(); }

template<class T>
class vsLinkedListStore
{
	vsListStoreEntry<T>		*m_listEntry;
	vsListStoreEntry<T>		*m_tail;
	
	vsListStoreEntry<T> *	FindEntry( T *item )
	{
		vsListStoreEntry<T> *ent = m_listEntry->m_next;
		
		while( ent )
		{
			if ( ent->m_item == item )
			{
				return ent;
			}
			ent = ent->m_next;
		}
		return NULL;
	}
	
public:
	
	typedef vsListStoreIterator<T> Iterator;

	vsLinkedListStore()
	{
		m_listEntry = new vsListStoreEntry<T>;
		m_tail = new vsListStoreEntry<T>;
		
		m_listEntry->m_next = m_tail;
		m_tail->m_prev = m_listEntry;
	}
	
	~vsLinkedListStore()
	{
		while ( m_listEntry->m_next )
		{
			vsListStoreEntry<T> *toDelete = m_listEntry->m_next;
			vsDelete( toDelete );
		}
		vsDelete( m_listEntry );
		m_tail = NULL;
	}
	
	void	Clear()
	{
		while ( m_listEntry->m_next != m_tail )
		{
			vsListStoreEntry<T> *toDelete = m_listEntry->m_next;
			vsDelete( toDelete );
		}
	}
	
	void	AddItem( T *item )
	{
		vsListStoreEntry<T> *ent = new vsListStoreEntry<T>( item );
		
		m_tail->Prepend( ent );
	}
	
	void	RemoveItem( T *item )
	{
		vsListStoreEntry<T> *ent = FindEntry(item);
		if ( ent )
		{
			ent->Extract();
			vsDelete(ent);
			return;
		}
		
		vsAssert(0, "Error: couldn't find item??");
	}
	
	bool	Contains( T *item )
	{
		vsListStoreEntry<T> *ent = FindEntry(item);
		return (ent != NULL);
	}
	
	bool	IsEmpty()
	{
		vsListStoreIterator<T> it(m_listEntry);
		
		return ( ++it == m_tail );
	}
	
	int		ItemCount()
	{
		int count = 0;
		
		vsListStoreIterator<T> it(m_listEntry);
		
		while(++it != m_tail)
		{
			count++;
		}
		
		return count;
	}
	
	vsListStoreIterator<T>	Begin() const
	{
		return 		++vsListStoreIterator<T>(m_listEntry);
	}

	vsListStoreIterator<T>	End() const
	{
		return 		vsListStoreIterator<T>(m_tail);
	}
	
	T *	operator[](int n)
	{
		for( vsListStoreIterator<T> iter = Begin(); iter != End(); iter++ )
		{
			if ( n == 0 )
				return *iter;
			n--;
		}
		
		return NULL;
	}
	
	vsLinkedListStore<T>& operator=( vsLinkedListStore<T> &o )
	{
		Clear();
		
		for( vsListStoreIterator<T> iter = o.Begin(); iter != o.End(); iter++ )
		{
			T* copy = new T(*(*iter));
			AddItem(copy);
		}
		return *this;
	}
};

#endif // VS_LINKED_LIST_STORE_H

