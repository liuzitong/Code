// ============================================================================
// author: night wing
// date: 2019/05/24
// to do: check
// ============================================================================
#ifndef  QXPACK_IC_FILEMON_CXX
#define  QXPACK_IC_FILEMON_CXX

#include "qxpack_ic_filemon.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_ncstring.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QReadWriteLock>
#include <QCoreApplication>
#include <QMap>
#include <QStringList>
#include <QString>
#include <QMetaObject>
#include <QThread>
#include <QSemaphore>

namespace  QxPack {

typedef QMap<IcNcString,void*>  IcNcStringVoidMap;

// ////////////////////////////////////////////////////////////////////////////
//
// private file mon worker ( working on a private thread )
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcFileMon_Worker : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcFileMon_Worker )
private:
    IcNcStringVoidMap    m_fn_set;
    QStringList          m_fn_name_filter;
    QFileSystemWatcher  *m_file_watcher;
    QAtomicInt           m_work_path_tag;
    QString              m_work_path;
protected:
    QStringList   firstScanFiles    ( );
            void  ensureFileWatcher ( bool alloc = true );
    Q_SLOT  void  fileWatcher_onFileChanged( const QString & );
    Q_SLOT  void  fileWatcher_onDirChanged ( const QString & );

    QString  fixedPath ( const QString &p )
    {
        QString wk_path = p;
        if ( wk_path.contains( QChar('\\'))) { wk_path = wk_path.replace( QChar('\\'), "/" ); }
        if ( wk_path.size() > 2 && wk_path.at(wk_path.size()-1) == QChar('/')) { wk_path = wk_path.left( wk_path.size() - 1); }
        return wk_path;
    }

public :
    IcFileMon_Worker( const QStringList &file_pattern );
    virtual ~IcFileMon_Worker( );

    int  workPathTag_di() { return m_work_path_tag.loadAcquire(); }
    Q_SLOT   int      workPathTag( ) { return m_work_path_tag.loadAcquire(); }
    Q_SLOT   int      setWorkPath( const QString &, void *fixed_path );
    Q_SLOT   QString  workPath ( ) const { return m_work_path; }
    Q_SLOT   QStringList  addFiles ( const QStringList & );
    Q_SLOT   QStringList  rmvFiles ( const QStringList & );
    Q_SLOT   QStringList  fileList ( );

    Q_SIGNAL void  workPathChanged( const QString &, int tag );
    Q_SIGNAL void  firstFileFound ( const QStringList &, int tag );
    Q_SIGNAL void  fileAdded      ( const QStringList &, int tag );
    Q_SIGNAL void  fileRemoved    ( const QStringList &, int tag );
    Q_SIGNAL void  fileModified   ( const QStringList &, int tag );
};

// ============================================================================
// ctor
// ============================================================================
IcFileMon_Worker :: IcFileMon_Worker ( const QStringList &file_pattern )
                 : QObject( Q_NULLPTR )
{
    m_fn_name_filter = file_pattern;
    m_work_path_tag.storeRelease(0);
    m_file_watcher = Q_NULLPTR;
}

// ============================================================================
// dtor
// ============================================================================
IcFileMon_Worker :: ~IcFileMon_Worker( )
{
    this->ensureFileWatcher( false );
}

// ============================================================================
// return current file list
// ============================================================================
QStringList  IcFileMon_Worker :: fileList( )
{
    QStringList fn_list;
    QMap<IcNcString,void*>::const_iterator c_itr = m_fn_set.cbegin();
    while ( c_itr != m_fn_set.cend()) {
        fn_list.append(  c_itr.key().strRef() ); ++ c_itr;
    }
    return fn_list;
}

// ============================================================================
// first scan files at the directory
// ============================================================================
QStringList  IcFileMon_Worker :: firstScanFiles( )
{
    QStringList fn_list;
    if ( ! m_work_path.isEmpty() ){
        QDir dir( m_work_path, QString(), QDir::Time | QDir::IgnoreCase, QDir::Files );
        dir.setNameFilters( m_fn_name_filter );
        fn_list = dir.entryList( );
    }
    QStringList::const_iterator c_itr = fn_list.cbegin();
    while ( c_itr != fn_list.cend()) {
        m_fn_set.insert( IcNcString( *c_itr ++ ),Q_NULLPTR );
    }

    return fn_list;
}

// ============================================================================
// ensure file watcher, drop old one, allocate new one
// ============================================================================
void  IcFileMon_Worker :: ensureFileWatcher( bool alloc )
{
    if ( m_file_watcher != Q_NULLPTR ) {
        m_file_watcher->blockSignals(true);
        m_file_watcher->removePaths( m_file_watcher->files() );
        m_file_watcher->removePaths( m_file_watcher->directories());
        m_file_watcher->deleteLater();
        m_file_watcher = Q_NULLPTR;
    }
    if ( alloc ) {
        m_file_watcher = new QFileSystemWatcher();

        // pre-add the files
        QMap<IcNcString,void*>::const_iterator itr = m_fn_set.cbegin();
        while ( itr != m_fn_set.cend()) {
            m_file_watcher->addPath( QString("%1/%2").arg( m_work_path ).arg( itr.key().strRef() ) );
            ++ itr;
        }

        // setup the directory
        m_file_watcher->addPath ( m_work_path );

        // connect the signal
        QObject::connect( m_file_watcher, SIGNAL(fileChanged( const QString &)),
                          this, SLOT(fileWatcher_onFileChanged( const QString&)));
        QObject::connect( m_file_watcher, SIGNAL(directoryChanged( const QString &)),
                          this, SLOT(fileWatcher_onDirChanged( const QString &)));
    }
}

// ============================================================================
// set the work path
// ============================================================================
int  IcFileMon_Worker :: setWorkPath( const QString &wk, void *fixed_wk )
{
    // setup work path
    QString  wk_path  = this->fixedPath( wk );
    int tag = m_work_path_tag.fetchAndAddOrdered(1) + 1;
    m_work_path = wk_path;
    if ( fixed_wk != Q_NULLPTR ) { *( T_ObjCast( QString*, fixed_wk )) = m_work_path; }

    // clear file name set, and scan the directory, add into fn_set
    m_fn_set = IcNcStringVoidMap();
    QStringList fn_found = this->firstScanFiles( );

    // file watcher create
    this->ensureFileWatcher( !wk.isEmpty() );

    // emit signals   
    emit this->workPathChanged( m_work_path, tag );
    if ( ! fn_found.empty() ) { // nw: 2019/05/10 old is -- if ( fn_found.size() > 0 ) {
        emit this->firstFileFound( fn_found, tag );
    }

    return tag;
}

// ============================================================================
//  add files into file watcher
// ============================================================================
QStringList    IcFileMon_Worker :: addFiles( const QStringList &fn )
{
    IcNcString nc_str; QStringList act_list;
    QStringList::const_iterator c_itr = fn.cbegin( );
    if ( m_file_watcher != Q_NULLPTR ) {
        while ( c_itr != fn.cend( )) {
            nc_str.setString( QDir( (*c_itr ++ ) ).dirName() );
            if ( m_fn_set.contains( nc_str )) { continue; }
            m_file_watcher->addPath( QString("%1/%2").arg( m_work_path ).arg( nc_str.strRef() ));
            m_fn_set.insert( nc_str, Q_NULLPTR );
            act_list.append( nc_str.strRef() );
        }
    }
    return act_list;
}

// ============================================================================
// remove files
// ============================================================================
QStringList  IcFileMon_Worker :: rmvFiles( const QStringList &fn )
{
    IcNcString nc_str; QStringList act_list;
    QStringList::const_iterator c_itr = fn.cbegin();
    if ( m_file_watcher != Q_NULLPTR ) {
        while ( c_itr != fn.end()) {
            nc_str.setString( QDir( (*c_itr ++)).dirName() );
            if ( ! m_fn_set.contains( nc_str )) { continue; }
            m_file_watcher->removePath( QString("%1/%2").arg( m_work_path ).arg( nc_str.strRef()) );
            m_fn_set.remove( nc_str );
            act_list.append( nc_str.strRef());
        }
    }
    return act_list;
}

// ============================================================================
// SLOT: handle the file changed
// ============================================================================
void     IcFileMon_Worker :: fileWatcher_onFileChanged( const QString &fp )
{
    if ( QFile(fp).exists() ) {
        emit this->fileModified( QStringList() << QDir(fp).dirName(), m_work_path_tag );
    } else {
        QStringList act_list = this->rmvFiles( QStringList() << QDir(fp).dirName() );
        if ( ! act_list.empty() ) { // nw: 2019/05/10 old is -- if ( act_list.size() > 0 ) {
            emit this->fileRemoved( act_list, m_work_path_tag );
        }
    }
}

// ============================================================================
// SLOT: handle the dir changed
// ============================================================================
void     IcFileMon_Worker :: fileWatcher_onDirChanged ( const QString &path )
{
    if ( m_work_path.compare( this->fixedPath( path ), Qt::CaseInsensitive ) != 0 )
    { return; }

    // ------------------------------------------------------------------------
    // read file list in the directory
    // ------------------------------------------------------------------------
    QStringList fn_list;
    {
        QDir dir( path, QString(), QDir::Time | QDir::IgnoreCase, QDir::Files | QDir::Modified );
        dir.setNameFilters( m_fn_name_filter );
        fn_list = dir.entryList( );
    }
    if ( fn_list.isEmpty() ) { return; } // no changing.

    // ------------------------------------------------------------------------
    // enum all list and compare in fn_set
    // ------------------------------------------------------------------------
    QStringList fn_added_list; IcNcString nc_str;
    QStringList::const_iterator c_itr = fn_list.cbegin();
    while ( c_itr != fn_list.cend()) {
        nc_str.setString( ( *c_itr ++ ));
        if ( m_fn_set.contains( nc_str )) { continue; }
        fn_added_list << nc_str.strRef();
    }

    // ------------------------------------------------------------------------
    // now check the added files
    // ------------------------------------------------------------------------
    if ( ! fn_added_list.empty() ) { // nw: 2019/05/10 old is -- if ( fn_added_list.size() > 0 ) {
       fn_added_list = this->addFiles( fn_added_list );
    }
    if ( ! fn_added_list.empty() ) { // nw: 2019/05/10 old is -- if ( fn_added_list.size() > 0 ) {
       emit this->fileAdded( fn_added_list, m_work_path_tag );
    }
}


// ////////////////////////////////////////////////////////////////////////////
//
// file monitor private ( work in caller thread )
//
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcFileMonPriv*, o )
class QXPACK_IC_HIDDEN  IcFileMonPriv : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcFileMonPriv )
private:
    IcSysEvtQThreadPriv  m_sys_evt_thread;
    IcFileMon_Worker *m_worker;
    IcFileMon   *m_parent;
    QStringList  m_fn_pattern;
    QString      m_work_path;
    int          m_work_tag;
protected:
    Q_SLOT  void  worker_onFirstFileFound( const QStringList &fn, int tag );
    Q_SLOT  void  worker_onFileAdded     ( const QStringList &fn, int tag );
    Q_SLOT  void  worker_onFileRemoved   ( const QStringList &fn, int tag );
    Q_SLOT  void  worker_onFileModified  ( const QStringList &fn, int tag );
    Q_SLOT  void  worker_workPathChanged ( const QString &, int tag );
public :
    IcFileMonPriv ( const QStringList &fn_pattern, IcFileMon *pa );
    virtual ~IcFileMonPriv ( );
    int     setWorkPath ( const QString & );
    QString  workPath ( ) const { return m_work_path; }
    void     addFiles ( const QStringList &fn_list );
    QStringList  fileList ( ) const;
};

// ============================================================================
// ctor
// ============================================================================
IcFileMonPriv :: IcFileMonPriv (const QStringList &fn_pattern, IcFileMon *pa )
              : m_parent( pa ), m_fn_pattern( fn_pattern )
{
    // create object in remote sysevt thread
    QObject *cr_obj = QxPack::IcRmtObjCreator::createObjInThread(
        m_sys_evt_thread.thread(),
        []( void *ctxt )->QObject* {
            IcFileMonPriv *t_this = static_cast<IcFileMonPriv*>(ctxt);
            return qxpack_ic_new_qobj( IcFileMon_Worker, t_this->m_fn_pattern );
        }, this, false
    );
    m_fn_pattern = QStringList(); // need not to use...
    m_worker = qobject_cast<QxPack::IcFileMon_Worker*>( cr_obj );
    m_work_tag = m_worker->workPathTag_di();

    // make signal handler
    QObject::connect( m_worker, SIGNAL(fileAdded( const QStringList &,int)),
                      this, SLOT(worker_onFileAdded( const QStringList&,int)) );
    QObject::connect( m_worker, SIGNAL(fileRemoved( const QStringList&,int)),
                        this, SLOT(worker_onFileRemoved( const QStringList&,int)));
    QObject::connect( m_worker, SIGNAL(fileModified( const QStringList&,int)),
                        this, SLOT(worker_onFileModified( const QStringList&,int)));
    QObject::connect( m_worker, SIGNAL(firstFileFound( const QStringList&,int)),
                        this, SLOT(worker_onFirstFileFound( const QStringList&,int)));
    QObject::connect( m_worker, SIGNAL(workPathChanged( const QString &,int)),
                        this, SLOT(worker_workPathChanged( const QString &,int)));

}

// ============================================================================
// dtor
// ============================================================================
    IcFileMonPriv :: ~IcFileMonPriv ( )
{
    QObject::disconnect( m_worker, Q_NULLPTR, this, Q_NULLPTR );
    IcRmtObjDeletor::deleteObjInThread(
        m_sys_evt_thread.thread(),
        []( void*, QObject *del_obj ) {
            qxpack_ic_delete_qobj_later( del_obj );
        }, this, m_worker
    );
}

// ============================================================================
// setup the work path, return the tag
// ============================================================================
int  IcFileMonPriv :: setWorkPath( const QString &wk )
{
    bool call_ret = false;

    if ( m_worker->thread() != QThread::currentThread()) {
        call_ret = QMetaObject::invokeMethod(
            m_worker, "setWorkPath",  Qt::BlockingQueuedConnection,
            Q_RETURN_ARG( int, m_work_tag ), Q_ARG( const QString &, wk ),
            Q_ARG( void*, reinterpret_cast<void *>( & m_work_path ))
        );
    } else {
        m_work_tag = m_worker->setWorkPath( wk, & m_work_path );
    }
    return ( call_ret ? m_work_tag : m_work_tag ); // .... what value if failed to call ?
}

// ============================================================================
// setup pre-added files
// ============================================================================
void    IcFileMonPriv :: addFiles ( const QStringList &fn_list )
{
    QStringList fn_fixed_list;

    if ( m_worker->thread() != QThread::currentThread()) {
        bool call_ret = QMetaObject::invokeMethod(
            m_worker, "addFiles", Qt::BlockingQueuedConnection,
            Q_RETURN_ARG( QStringList, fn_fixed_list ), Q_ARG( const QStringList &, fn_list )
        );
        Q_UNUSED( call_ret )
    } else {
        fn_fixed_list = m_worker->addFiles( fn_list );
    }
}

// ============================================================================
// return current file list that monitored
// ============================================================================
QStringList  IcFileMonPriv :: fileList ( ) const
{
    QStringList fn_list;

    if ( m_worker->thread() != QThread::currentThread()) {
        bool call_ret = QMetaObject::invokeMethod(
            m_worker, "fileList", Qt::BlockingQueuedConnection,
            Q_RETURN_ARG( QStringList, fn_list )
        );
        Q_UNUSED( call_ret )
    } else {
        fn_list = m_worker->fileList();
    }
    return fn_list;
}

// ============================================================================
// slot filter
// ============================================================================
void   IcFileMonPriv :: worker_onFirstFileFound( const QStringList &fn, int tag )
{
    if ( tag == m_work_tag ) { emit m_parent->fileFirstFound( fn ); }
}

void   IcFileMonPriv :: worker_onFileAdded ( const QStringList &fn, int tag )
{
    if ( tag == m_work_tag ) { emit m_parent->fileAdded( fn ); }
}

void   IcFileMonPriv :: worker_onFileRemoved  ( const QStringList &fn, int tag )
{
    if ( tag == m_work_tag ) { emit m_parent->fileRemoved( fn ); }
}

void   IcFileMonPriv :: worker_onFileModified ( const QStringList &fn, int tag )
{
    if ( tag == m_work_tag ) { emit m_parent->fileModified( fn ); }
}

void   IcFileMonPriv :: worker_workPathChanged( const QString &wk, int tag )
{
    if ( tag == m_work_tag ) { emit m_parent->workPathChanged( wk ); }
}



// ////////////////////////////////////////////////////////////////////////////
//
//                  File Monitor
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
        IcFileMon :: IcFileMon ( const QStringList &file_pattern, QObject *pa )
                   : QObject( pa )
{
    m_obj = qxpack_ic_new( IcFileMonPriv, file_pattern, this );
}

// ============================================================================
// dtor
// ============================================================================
        IcFileMon :: ~IcFileMon (  )
{
    if ( m_obj != Q_NULLPTR ) {
        qxpack_ic_delete( m_obj, IcFileMonPriv );
    }
}

// ============================================================================
// setup the working path
// ============================================================================
int        IcFileMon :: setWorkPath( const QString &wk )
{   return T_PrivPtr( m_obj )->setWorkPath( wk ); }

// ============================================================================
// return the work path
// ============================================================================
QString    IcFileMon :: workPath( ) const
{   return T_PrivPtr( m_obj )->workPath( ); }

// ============================================================================
// add the files, note we alwasy use the name without path.
// ============================================================================
void       IcFileMon :: addFiles ( const QStringList &fn )
{   T_PrivPtr( m_obj )->addFiles( fn ); }

// ============================================================================
// return current files that monitor
// ============================================================================
QStringList  IcFileMon :: fileList( ) const
{   return T_PrivPtr( m_obj )->fileList();  }

}
#include "qxpack_ic_filemon.moc"
#endif
