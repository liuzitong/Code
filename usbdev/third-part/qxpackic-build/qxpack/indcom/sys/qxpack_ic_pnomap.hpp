#ifndef QXPACK_IC_PNOMAP_HPP
#define QXPACK_IC_PNOMAP_HPP

#include <QString>
#include <QMap>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
   \class  IcPnoMap
   \brief  The IcPnoMap class is a template map that manage fuzzy name object
*/
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class  IcPnoMap {
private:
    // ========================================================================
    // Innser class, the name string, override the compare method for QMap
    // ========================================================================
    class  NameStr {
    public : enum  CmpMode { CmpMode_Accur = 0, CmpMode_Prefix };
    private: QString  m_str; const CmpMode *m_cmp_mode_ptr; bool  m_is_skey;
    public :
        NameStr () :
            m_cmp_mode_ptr( Q_NULLPTR ), m_is_skey( false ) { }

        NameStr ( const QString &s, const CmpMode &cm,  bool is_skey ) :
            m_str( s ), m_cmp_mode_ptr( &cm ), m_is_skey( is_skey ) { }

        ~NameStr() { }

        NameStr( const NameStr &other ) :
            m_str( other.m_str ), m_cmp_mode_ptr( other.m_cmp_mode_ptr ), m_is_skey( other.m_is_skey ) { }

        NameStr &  operator = ( const NameStr &other )
        {  m_str = other.m_str; m_cmp_mode_ptr = other.m_cmp_mode_ptr; m_is_skey = other.m_is_skey; return *this; }


#if  __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1900 )
        // C++11 support
        NameStr( NameStr &&other ) :
            m_str( other.m_str ), m_cmp_mode_ptr( other.m_cmp_mode_ptr ), m_is_skey( other.m_is_skey )
        { other.m_str = QString(); }

        NameStr &  operator = ( NameStr &&other )
        {
            if ( this != &other ) { // fixed since 20201230
                m_str = other.m_str; m_cmp_mode_ptr = other.m_cmp_mode_ptr; m_is_skey = other.m_is_skey;
                other.m_str = QString();
            }
            return *this;
        }
#endif

        inline QString&  strRef() { return m_str;  }


        // ====================================================================
        // compare method, called by QMap
        // ====================================================================
        bool    operator < ( const NameStr &other ) const
        {
            // ----------------------------------------------------------------
            // HINT: QString::compare( "abc", "abcd" ) return -1 ( means "abc" < "abcd" )
            // ----------------------------------------------------------------
            Q_ASSERT( ! m_str.isEmpty() && ! other.m_str.isEmpty() );

            if ( *m_cmp_mode_ptr == CmpMode_Prefix ) {
                // ------------------------------------------------------------
                // Just compare search item whether if it start with map item.
                // If not, use normal compare
                // ------------------------------------------------------------
                if ( ! m_is_skey ) {
                    return ( other.m_str.startsWith( m_str ) ? false : ( m_str.compare( other.m_str ) < 0 ));
                } else {
                    return ( m_str.startsWith( other.m_str ) ? false : ( m_str.compare( other.m_str ) < 0 ));
                }
            } else {  // insert mode
                return ( m_str.compare(  other.m_str ) < 0 );
            }
        }
    };


private:
    QMap<NameStr,T>  m_map; typename NameStr::CmpMode  m_cmp_mode;
protected:
    inline  void  ensureCmpMode( typename NameStr::CmpMode m ) const
    {
        IcPnoMap<T>* t_this = const_cast<IcPnoMap<T>*>( this );
        if ( m_cmp_mode != m ) { t_this->m_cmp_mode = m; }
    }

public :
    IcPnoMap ( ) { }
    ~IcPnoMap( ) { }

    inline IcPnoMap ( const IcPnoMap &other ) { m_map = other.m_map; m_cmp_mode = NameStr::CmpMode_Accur; }
    inline IcPnoMap&  operator = ( const IcPnoMap &other )
    {  m_map = other.m_map; m_cmp_mode = other.m_cmp_mode; return *this; }

#if  __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1900 )
    inline IcPnoMap ( IcPnoMap  && other ) {  m_map = other.m_map; m_cmp_mode = other.m_cmp_mode; other.m_map = QMap<NameStr,T>(); }
    inline IcPnoMap &  operator = ( IcPnoMap &&other )
    {  
        if ( this != &other ) { // fixed since 20201230
            m_map = other.m_map; m_cmp_mode = other.m_cmp_mode; other.m_map = QMap<NameStr,T>();
        }
        return *this; 
    }
#endif

    inline int   size() const { return m_map.size(); }

    // ========================================================================
    // the iterator
    // ========================================================================
    typedef typename QMap<NameStr,T>::const_iterator  const_iterator;
    typedef typename QMap<NameStr,T>::iterator        iterator;

    inline const_iterator   constEnd()   const { return m_map.constEnd();   }
    inline const_iterator   constBegin() const { return m_map.constBegin(); }
    inline const_iterator   constFind( const QString &nm )  const
    {
        ensureCmpMode( NameStr::CmpMode_Accur );
        return m_map.constFind( NameStr( nm, m_cmp_mode, true ));
    }

    inline const_iterator   constPrefixFind( const QString &nm ) const
    {
        ensureCmpMode( NameStr::CmpMode_Prefix );
        return m_map.constFind( NameStr( nm, m_cmp_mode, true ) );
    }

    inline iterator         end()   { return m_map.end();   }
    inline iterator         begin() { return m_map.begin(); }
    inline iterator         erase( iterator itr ) { return m_map.erase( itr ); }
    inline iterator         find( const QString &nm )
    {
        ensureCmpMode( NameStr::CmpMode_Accur );
        return m_map.find( NameStr( nm, m_cmp_mode, true ));
    }

    inline iterator         prefixFind( const QString &nm )
    {
        ensureCmpMode( NameStr::CmpMode_Prefix );
        return m_map.find( NameStr( nm, m_cmp_mode, true ));
    }

    // ========================================================================
    /*!
       \fn void clear()

       clear all members in the map
     */
    // ========================================================================
    void   clear( ) { m_map.clear();  }


    // ========================================================================
    /*!
       \fn void insert( const QString  & nm, const T  & val )

       Insert a pair that name is \a nm, value is \a val.
    */
    // ========================================================================
    void   insert( const QString & nm, const T &val )
    {
        ensureCmpMode( NameStr::CmpMode_Accur );
        m_map.insert( NameStr( nm, m_cmp_mode, false ), val );
    }

    // ========================================================================
    /*!
       \fn int    remove( const QString & nm )

       Remove the pair that name equal to \a nm.
    */
    // ========================================================================
    int    remove( const QString & nm )
    {
        ensureCmpMode( NameStr::CmpMode_Accur );
        return m_map.remove( NameStr( nm, m_cmp_mode, false ) );
    }

    // ========================================================================
    /*!
        \fn T      findValue ( const QString  & nm )

        Find pair that name equal to \a nm by using accurate compare mode.
        Return the pair value. If not found, return the T default value.
    */
    // ========================================================================
    T      findValue ( const QString &nm ) const
    {
        const_iterator citr = constFind( nm );
        return ( citr != constEnd() ? citr.value() : T() );
    }

    // ========================================================================
    /*!
       \fn T      prefixFindValue ( const QString   & nm )

       Find pair that name is the prefix of the \a nm.
     */
    // =========================================================================
    T      prefixFindValue( const QString &nm ) const
    {
        const_iterator citr = constPrefixFind( nm );
        return ( citr != constEnd() ?  citr.value() : T() );
    }
};


}



#endif
