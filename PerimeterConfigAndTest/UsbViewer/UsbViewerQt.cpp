#include "UsbViewerQt.h"

#include <QtNetwork\qhostinfo.h>

UsbViewerQt::UsbViewerQt(QWidget *parent)
    : QDialog (parent)
{
	ui.setupUi(this);
    setWindowIcon(QIcon(":/Icon/Token.png"));

    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;
    setWindowFlags(windowFlag);

	rootModel = NULL;
	

	// Init tree view
	initTreeModel();

	// Timer
	this->timer = new QTimer(this);
	//timer->start();
	timer->setInterval(1000);

	connect(ui.treeView, SIGNAL(clicked(const QModelIndex)), this, SLOT(getTreeClicked(const QModelIndex)));
	connect(timer, SIGNAL(timeout()), this, SLOT(refreshTree()));
}

UsbViewerQt::~UsbViewerQt()
{
	
}

void UsbViewerQt::initTreeModel()
{
	QTreeView * tree = ui.treeView;

	// Root Tree
	if (rootModel == NULL)
	{
		this->rootModel = new QStandardItemModel();
		tree->setModel(rootModel);
	}
	else
	{
		rootModel->clear();
	}
	
	QStandardItem * rootStandardItem = rootModel->invisibleRootItem();
	QStandardItem* rootItem = new QStandardItem(getHostName());
	rootStandardItem->appendRow(rootItem);
	
	// USB device Root Tree 
    QStandardItem* usbTreeRoot = new QStandardItem(QIcon(":/Icon/USB.png"), tr("USB"));
	rootItem->appendRow(usbTreeRoot);
	refreshDeviceList(DeviceClass_USB, usbTreeRoot);

    // USB device Root Tree
    QStandardItem* usbDevTreeRoot = new QStandardItem(QIcon(":/Icon/USB.png"), tr("USBDev"));
    rootItem->appendRow(usbDevTreeRoot);
    refreshDeviceList(DeviceClass_USBDEV, usbDevTreeRoot);

	// System device Root Tree 
    QStandardItem* systemDeviceTreeRoot = new QStandardItem(QIcon(":/Icon/SystemDevice.png"), tr("SystemDevice"));
	rootItem->appendRow(systemDeviceTreeRoot);
	refreshDeviceList(DeviceClass_SYSTEM, systemDeviceTreeRoot);

	// Mouse Root Tree 
    QStandardItem* mouseTreeRoot = new QStandardItem(QIcon(":/Icon/Mouse.png"), tr("Mouse"));
	rootItem->appendRow(mouseTreeRoot);
	refreshDeviceList(DeviceClass_MOUSE, mouseTreeRoot);

	// Net Root Tree 
    QStandardItem* netTreeRoot = new QStandardItem(QIcon(":/Icon/Net.png"), tr("Net"));
	rootItem->appendRow(netTreeRoot);
	refreshDeviceList(DeviceClass_NET, netTreeRoot);

	// keyboard Root Tree 
    QStandardItem* keyboardTreeRoot = new QStandardItem(QIcon(":/Icon/Keyboard.png"), tr("Keyboard"));
	rootItem->appendRow(keyboardTreeRoot);
	refreshDeviceList(DeviceClass_KEYBOARD, keyboardTreeRoot);

}

QString UsbViewerQt::getHostName()
{
	QString hostName = QHostInfo::localHostName();
	return hostName;
}

void UsbViewerQt::refreshDeviceList(DeviceClass deviceClass, QStandardItem* deviceTreeRoot)
{

	// Retrieve all device info
	char deviceInfo[10000] = { 0 };
	int res = getDeviceProperty(deviceClass, Property_DESCRIPTION, INDEX_ALL, deviceInfo);
	QString DeviceInfoStr = QString::fromLocal8Bit(deviceInfo);
	QStringList deviceInfoList = DeviceInfoStr.split("\n");
	deviceInfoList.removeAt(deviceInfoList.size() - 1);

	QString IconPath;
	switch (deviceClass)
	{
	case DeviceClass_SYSTEM:
        IconPath = ":/Icon/SystemDevice.png";
		this->systemInfoList = deviceInfoList;
		break;
	case DeviceClass_USB:
        IconPath = ":/Icon/USB.png";
		this->usbInfoList = deviceInfoList;
		break;
    case DeviceClass_USBDEV:
        IconPath = ":/Icon/USB.png";
        this->usbDevInfoList = deviceInfoList;
        break;
	case DeviceClass_MOUSE:
        IconPath = ":/Icon/Mouse.png";
		this->mouseInfoList = deviceInfoList;
		break;
	case DeviceClass_NET:
        IconPath = ":/Icon/Net.png";
		this->netInfoList = deviceInfoList;
		break;
	case DeviceClass_KEYBOARD:
        IconPath = ":/Icon/Keyboard.png";
		this->keyBoardInfoList = deviceInfoList;
		break;
	default:
		break;
	}

	for (int index = 0; index <deviceInfoList.count(); index++)
	{
		QString iName = deviceInfoList[index];
		QStandardItem * item = new QStandardItem(QIcon(IconPath), iName);
		deviceTreeRoot->appendRow(item);
	}
	ui.treeView->expandAll();

}

void UsbViewerQt::getTreeClicked(const QModelIndex iIndex)
{

	// 1. Retrieve current selected item
	QString objectName = ui.treeView->model()->itemData(iIndex).values()[0].toString();
	
	// 2. Get the father item to distinguish the device class
	QStandardItem* parentItem = this->rootModel->itemFromIndex(iIndex)->parent();

	QString parentItemText = parentItem->text();
	QByteArray ba_objectName = objectName.toLocal8Bit();
	QByteArray ba_parentItemText = parentItemText.toLatin1();


	DeviceClass deviceClass;
	QStringList deviceInfoList;

	if (parentItemText == "USB"){
		deviceInfoList = this->usbInfoList;
		deviceClass = DeviceClass_USB;
	}
    else if (parentItemText == "USBDev"){
        deviceInfoList = this->usbDevInfoList;
        deviceClass = DeviceClass_USBDEV;
    }
	else if (parentItemText == "SystemDevice"){
		deviceInfoList = this->systemInfoList;
		deviceClass = DeviceClass_SYSTEM;
	}
	else if (parentItemText == "Mouse"){
		deviceInfoList = this->mouseInfoList;
		deviceClass = DeviceClass_MOUSE;
	}
	else if (parentItemText == "Net"){
		deviceInfoList = this->netInfoList;
		deviceClass = DeviceClass_NET;
	}
	else if (parentItemText == "Keyboard"){
		deviceInfoList = this->keyBoardInfoList;
		deviceClass = DeviceClass_KEYBOARD;
	}
	else{
		ui.textEdit->clear();
		return;
	}

	// 3. Retrieve the device index with object name
	int index = 0;
	for (index; index < deviceInfoList.size(); index++){
		if (deviceInfoList[index] == objectName){
			break;
		}
	}

	// 4. Retrieve the device detailed info with the index
	char buffer[1024] = { 0 };
	int ret = getDeviceProperty(deviceClass, Property_DESCRIPTION, index, buffer);
	QString result = QString::fromLocal8Bit(buffer);
    int pos = result.indexOf("VID_");
    if(pos!=-1)
    {
        VID = result.mid(pos+4,4);
    }
    else
    {
        pos = result.indexOf("VID");
        if(pos!=-1)
        {
            VID = result.mid(pos+3,4);
        }
        else{
            VID = "invalid";
        }
    }
    pos=result.indexOf("PID_");
    if(pos!=-1)
    {
         PID=result.mid(pos+4,4);
    }
    else
    {
        pos = result.indexOf("PID");
        if(pos!=-1)
        {
            PID=result.mid(pos+3,4);
        }
        else{
            PID="invalid";
        }
    }

	ui.textEdit->setText(result);
}

void UsbViewerQt::refreshTree()
{
	initTreeModel();
}

void UsbViewerQt::on_acceptBtn_clicked()
{
    accept();
}

void UsbViewerQt::on_cancelBtn_clicked()
{
    reject();
}
