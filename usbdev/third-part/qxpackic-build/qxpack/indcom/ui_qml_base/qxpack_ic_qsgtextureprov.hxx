/* ///////////////////////////////////////////////////////////////////////// */
/*! 
  @file    icqsgtextureprov.hxx
  @author  night wing
  @date    2018/11
  @brief   The file is QSG texture Provider class
  @par     History
  @verbatim
          <author>   <time>   <version>  <desc>                                   
         nightwing   2018/11   0.1.0     port this module
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////////// */
#ifndef  QXPACK_IC_QSGTEXTUREPROV_HXX
#define  QXPACK_IC_QSGTEXTUREPROV_HXX


#include <stdint.h>
#include <qxpack/indcom/common/qxpack_ic_def.h>
#include <QObject>
#include <QSGTextureProvider>
#include <QSGTexture>

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief TextureProv
 * @details this class used to provide the Texture to QML
 * @note  this object should live in the QuickItem.
 */
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {

class QXPACK_IC_API IcQSGTextureProv : public QSGTextureProvider {
    Q_OBJECT
public:
    //! ctor
    IcQSGTextureProv ( );

    //! dtor
    virtual ~IcQSGTextureProv( ) Q_DECL_OVERRIDE;
    
    /*!
     * @brief set the texture
     * @param [in] tex, the texture pointer
     * @param [in] force_dirty, means emit the texturechange signal
     */
    void setTexture( QSGTexture *tex, bool force_dirty = true );

    //! return the current hold texture.
    virtual QSGTexture *  texture( ) const Q_DECL_OVERRIDE;

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQSGTextureProv )
};
    


}

#endif
