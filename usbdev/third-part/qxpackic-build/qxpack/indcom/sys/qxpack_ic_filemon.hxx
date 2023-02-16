#ifndef QXPACK_IC_FILEMON_HXX
#define QXPACK_IC_FILEMON_HXX

#include <qxpack/indcom/common/qxpack_ic_def.h>
#include <QObject>
#include <QString>
#include <QStringList>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief   file monitor
 * @details this class used to monitor the specified path. this class inner \n
 *     used a QThread worker.
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcFileMon : public QObject {
    Q_OBJECT
public:
    /*!
     * @brief ctor
     * @param file_pattern  [in] like "rpt_*.json", "*.fdt
     * @param pa  [in] QObject tree, parent object
     */
    IcFileMon ( const QStringList &file_pattern, QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcFileMon ( ) Q_DECL_OVERRIDE;

    /*!
     * @brief set the work directory full Path
     * @param [in] work directory full path
     * @return a tag value means this work path
     * @note  if passed a empty string as work directory, the monitor \n
     *        will be stopped.
     */
    int  setWorkPath( const QString & );

    /*!
     * @return the current work directory full path
     */
    QString      workPath ( ) const;

    /*!
     * @return current monitored file list
     */
    QStringList  fileList ( ) const;

    /*!
     * @brief add files that want to monitor
     * @param [in] fn, the file list
     */
    void         addFiles ( const QStringList &fn );

    //! emited while work directory changed
    Q_SIGNAL void  workPathChanged   ( const QString & );

    //! only emited once while first or change the work directory
    Q_SIGNAL void  fileFirstFound( const QStringList & );

    //! while the monitored file are removed
    Q_SIGNAL void  fileRemoved   ( const QStringList & );

    //! while the new files created in the work directory
    Q_SIGNAL void  fileAdded     ( const QStringList & );

    //! while the monitored files are modified
    Q_SIGNAL void  fileModified  ( const QStringList & );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcFileMon )
};


}

#endif
