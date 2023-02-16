#ifndef USBVIEWERQT_H
#define USBVIEWERQT_H

#include <QtWidgets/QMainWindow>
#include "ui_UsbViewerQt.h"
#include "UsbInterface.h"
#include <QDialog>
#include <QStandardItemModel>
#include <qtimer.h>

class UsbViewerQt : public QDialog
{
	Q_OBJECT

public:
	UsbViewerQt(QWidget *parent = 0);
	~UsbViewerQt();

	// Init treeModel
	void initTreeModel();

	// Refresh the device list
	void refreshDeviceList(DeviceClass deviceClass, QStandardItem* deviceTreeRoot);

	// Retrieve the host name
	QString getHostName();

	// timer
	QTimer* timer;
    QString PID,VID;

public slots:

	// slots: when the tree is clicked
	void getTreeClicked(const QModelIndex index);

	// slots: refresh tree when timer timeout
	void refreshTree();

private slots:
    void on_acceptBtn_clicked();

    void on_cancelBtn_clicked();

private:
	Ui::UsbViewerQtClass ui;

	QTreeView* pTreeView;

	// UI tree root model
	QStandardItemModel* rootModel;

	// Usb device info list 
	QStringList usbInfoList;

    // Usb device info list
    QStringList usbDevInfoList;

	// System device info list 
	QStringList systemInfoList;

	// Mouse device info list
	QStringList mouseInfoList;

	// Net device info list
	QStringList netInfoList;

	// Keyboard device info list
	QStringList keyBoardInfoList;


};

#endif // USBVIEWERQT_H
