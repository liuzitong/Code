#ifndef  PERM_IMPROV_CXX
#define  PERM_IMPROV_CXX

#include "perm/common/perm_memcntr.hxx"
#include "perm_improv.hxx"
#include <QMutex>
#include <QCoreApplication>
#include <QString>
#include <QMap>
#include <functional> //C++11


#define T_PrivPtr( o )       perm_objcast( PermImProvPriv *, o )

namespace FcPerm {

// /////////////////////////////////////////////////////////////////////
//
//       PermImProvPriv_FileNameImagePair
//
// /////////////////////////////////////////////////////////////////////
class  PERM_HIDDEN  PermImProvPriv_FileNameImagePair {
private: QString m_file_name; QImage *m_im_ptr;
public :
    PermImProvPriv_FileNameImagePair( ) { m_im_ptr = Q_NULLPTR; }
    PermImProvPriv_FileNameImagePair( const QString &nm, QImage *ptr ) { m_file_name = nm; m_im_ptr = ptr; }
    ~PermImProvPriv_FileNameImagePair( ) { }
    PermImProvPriv_FileNameImagePair &  operator = ( const PermImProvPriv_FileNameImagePair & other )
    {  m_file_name = other.m_file_name; m_im_ptr = other.m_im_ptr; return *this; }
    QString   fileName( ) const { return m_file_name; }
    QImage *  imagePtr( ) const { return m_im_ptr;  }
};

// /////////////////////////////////////////////////////////////////////
// 
//        PermImProvPriv
//
// /////////////////////////////////////////////////////////////////////
class  PERM_HIDDEN  PermImProvPriv {
private:
    QMutex  m_locker;
    QMap< QString, PermImProvPriv_FileNameImagePair >    m_im_name_map;
	
    QImage  m_sys_eye_logo_default, m_sys_eye_logo_128_default;
    QImage  m_username_default,     m_password_default;
    QImage  m_pwd_hide_default,     m_pwd_show_default;
    QImage  m_keyboard_default,     m_keyboard_close_default;
    QImage  m_close_default,        m_default_login;

protected:
    void   loadImByName( const QString &nm, QImage *mem_im );

public :
    PermImProvPriv (  );
    ~PermImProvPriv (  );
    QImage  requestImage( const QString &id, QSize *size, const QSize &reqSize );
   
};

// =====================================================================
// CTOR
// =====================================================================
        PermImProvPriv :: PermImProvPriv (  )
{
    m_im_name_map.insert( QStringLiteral("default/keyboard"),       PermImProvPriv_FileNameImagePair( QStringLiteral("default/keyboard.svg"),        &m_keyboard_default ));
    m_im_name_map.insert( QStringLiteral("default/keyboardClose"),  PermImProvPriv_FileNameImagePair( QStringLiteral("default/keyboard_close.svg"),  &m_keyboard_close_default ));
    m_im_name_map.insert( QStringLiteral("default/username"),       PermImProvPriv_FileNameImagePair( QStringLiteral("default/username.svg"),        &m_username_default ));
    m_im_name_map.insert( QStringLiteral("default/password"),       PermImProvPriv_FileNameImagePair( QStringLiteral("default/password.svg"),        &m_password_default ));
    m_im_name_map.insert( QStringLiteral("default/pwdHide"),        PermImProvPriv_FileNameImagePair( QStringLiteral("default/pwd_hide.svg"),        &m_pwd_hide_default ));
    m_im_name_map.insert( QStringLiteral("default/pwdShow"),        PermImProvPriv_FileNameImagePair( QStringLiteral("default/pwd_show.svg"),        &m_pwd_show_default ));
    m_im_name_map.insert( QStringLiteral("default/sysEyeLogo"),     PermImProvPriv_FileNameImagePair( QStringLiteral("default/syseye_logo.svg"),     &m_sys_eye_logo_default ));
    m_im_name_map.insert( QStringLiteral("default/sysEyeLogo128"),  PermImProvPriv_FileNameImagePair( QStringLiteral("default/syseye_logo_128.svg"), &m_sys_eye_logo_128_default ));
    m_im_name_map.insert( QStringLiteral("default/close"),          PermImProvPriv_FileNameImagePair( QStringLiteral("default/close.svg"),           &m_close_default ));
    m_im_name_map.insert( QStringLiteral("default/defaultLogin"),   PermImProvPriv_FileNameImagePair( QStringLiteral("default/default_login.svg"),   &m_default_login ));
}

// =====================================================================
// DTOR
// =====================================================================
        PermImProvPriv :: ~PermImProvPriv (  )
{
    
}

// =====================================================================
// request image
// =====================================================================
QImage  PermImProvPriv :: requestImage ( const QString &id, QSize *size, const QSize &reqSize )
{
    QImage  im;  

    // --------------------------------------------------------
    // lookup the specified name
    // --------------------------------------------------------
    QMap<QString,PermImProvPriv_FileNameImagePair>::const_iterator c_itr = m_im_name_map.find( id );
    if ( c_itr == m_im_name_map.cend()) { return im; }

    // --------------------------------------------------------
    // try load the image by name
    // --------------------------------------------------------
    PermImProvPriv_FileNameImagePair pair = c_itr.value();
    if ( pair.fileName().isEmpty() || pair.fileName().isNull() || pair.imagePtr() == Q_NULLPTR ) { return im; }

    m_locker.lock();

    if ( pair.imagePtr()->isNull() ) {
        this->loadImByName( pair.fileName(), pair.imagePtr() );
    }
    im = *( pair.imagePtr());

    m_locker.unlock();

    // -------------------------------------------------------
    // scale the image into request size
    // -------------------------------------------------------
    if ( ! im.isNull( )) {
        if ( size != Q_NULLPTR ) { *size = im.size( ); }
        if ( im.size( ) != reqSize  &&  ! reqSize.isEmpty()) {
            if ( im.width( ) > reqSize.width( )  &&  im.height( ) > reqSize.height( )) {
                im = im.scaled( reqSize, Qt::KeepAspectRatio, Qt::FastTransformation  );
            } else {
                im = im.scaled( reqSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
            }
        }
    } else {
        if ( size != Q_NULLPTR ) { *size = QSize( 0, 0 ); }
    }

    return im;
}

// ====================================================================
// load by name
// ====================================================================
void     PermImProvPriv :: loadImByName( const QString &nm, QImage *mem_im )
{
    if ( mem_im != Q_NULLPTR ) {
        QString aa =  QString("%1/resource/perm/%2").arg(QCoreApplication::applicationDirPath()).arg( nm );
        *mem_im = QImage( QString("%1/resource/perm/%2").arg(QCoreApplication::applicationDirPath()).arg( nm ));
    }
}





// /////////////////////////////////////////////////////////////////////
//
//        PermImProv
//
// /////////////////////////////////////////////////////////////////////
// =====================================================================
//! CTOR
// =====================================================================
        PermImProv :: PermImProv ( ) :
                       QQuickImageProvider( QQuickImageProvider::Image )
{
    m_obj = perm_new( PermImProvPriv );
}

// =====================================================================
//! DTOR
// =====================================================================
        PermImProv :: ~PermImProv (  )
{
    if ( m_obj != Q_NULLPTR ) {
        perm_delete( m_obj, PermImProvPriv );
        m_obj = Q_NULLPTR;
    }
}

// =====================================================================
//! return the request image
/*!
  @param  id  [ in ] the identify string that 
*/
// =====================================================================
QImage  PermImProv :: requestImage( const QString &id, QSize *size, const QSize &reqSize )
{
    return T_PrivPtr( m_obj )->requestImage( id, size, reqSize );
}


}

/*!
  @}
*/

/*!
  @}
*/

/*!
  @}
*/

#endif
