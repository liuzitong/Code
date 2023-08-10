#include <QCoreApplication>
#include <QDirIterator>
#include <QString>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString prefix_dir = QCoreApplication::applicationDirPath();
    QDirIterator  dir_itr (
        prefix_dir + "/Eigen", QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
        QDirIterator::Subdirectories
    );

    // ------------------------------------------------------------------------
    // enum all files in that path
    // ------------------------------------------------------------------------
    QStringList files;
    while ( dir_itr.hasNext() ) {
        dir_itr.next();
        QFileInfo fi = dir_itr.fileInfo();
        if ( fi.isDir()) { continue; }
        QString fn = fi.absoluteFilePath();
        if ( fn.startsWith( prefix_dir + "/" )) {
            QString sub_str = fn.right( fn.size() - prefix_dir.size() - 1 );
            files.append( sub_str );
            //qInfo() << sub_str;
        }
    }

    // ------------------------------------------------------------------------
    // write the pri file
    // ------------------------------------------------------------------------
    QString priv;
    priv.append(
        "! contains( QXPACK_IC, indcom_algor_thr_eigen ) { \n"
        "    QXPACK_IC *= indcom_algor_thr_eigen \n"
        "    CONFIG *= c++11 \n"
        "    INCLUDEPATH *= $$PWD/ \n\n"
        "    QXPACK_IC_HEADERS_ALGOR_3RD_EIGEN += \\  \n"
    );
    auto citr = files.constBegin();
    while ( citr != files.constEnd()) {
        priv.append(
            QString("        $$PWD/%1 \\ \n").arg( *citr ++ )
        );
    }
    priv.append(
        " \n"
        "    QXPACK_IC_HEADERS_COPY += $${QXPACK_IC_HEADERS_ALGOR_3RD_EIGEN} \n"
        "           $$PWD/qxpack_ic_algor_3rd_eigen.pri \n\n"
        "    HEADERS += \\ \n "
        "       # $${QXPACK_IC_HEADERS_ALGOR_3RD_EIGEN}\\ \n\n"
        "    message( qxpack_indcom_algor_thr_eigen include as headers ) \n"
        "} \n"
    );

    QFile priv_fn( QCoreApplication::applicationDirPath() + "/qxpack_ic_algor_3rd_eigen.pri" );
    if ( priv_fn.open( QFile::WriteOnly | QFile::Text)) {
        priv_fn.write( priv.toUtf8().constData() );
        priv_fn.close();
    }

    return 0;
}
