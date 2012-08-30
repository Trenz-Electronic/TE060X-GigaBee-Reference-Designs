#include "gfwupgrader.h"
#include <iostream>
#include <QFile>
#include <QTime>
#include <QtConcurrentRun>


gfwUpgrader::gfwUpgrader(QObject *parent) :
    QObject(parent)
{
    m_isFlashFileOK = false;
    m_deviceList = new QList<gigabee_device_t>();
    m_deviceList->clear();

    m_flashData = NULL;
    m_flashDataLength = 0;

    m_discovery_msec_timeout = 1000; // wait 1 sec for discovery.

    m_num_of_devices_being_updated = 0;
    m_communicators = NULL;
}


gfwUpgrader::~gfwUpgrader()
{

    delete m_deviceList;
    delete m_flashData;
    delete [] m_communicators; // delete communicators.

}

QByteArray *gfwUpgrader::getTransferProgress()
{
    QByteArray *a = new QByteArray();

    a->resize(m_num_of_devices_being_updated);

    for (int i = 0; i < m_num_of_devices_being_updated; i++) {
        if (m_communicators)
            (*a)[i] = m_communicators[i].m_transferPercent;
        else
            (*a)[i] = 0;
    }

    return a;
}


// discovers the devices
void gfwUpgrader::doDiscover()
{
    //std::cout << "doDiscover!!" << std::endl;

    // perform discovery

    m_deviceList->clear();

    // broadcast discovery ping
    m_socket = new QUdpSocket(this);
    m_socket->bind(QHostAddress::Any, 5000); // for listening only

    gfw_datagram_t gfww;
    memset(&gfww, 0, sizeof(gfw_datagram_t));

    gfww.magic1 = 'G';
    gfww.magic2 = 'F';
    gfww.magic3 = 'W';
    gfww.op = GFW_OP_PING_CMD;

    //m_socket->connectToHost(QHostAddress::Broadcast,5000);
    while (m_socket->writeDatagram((char *)&gfww, sizeof(gfw_datagram_t),QHostAddress::Broadcast,5000) == -1);

    QTime time;
    time.start();

    char *recvbuf;

    while (time.elapsed() < m_discovery_msec_timeout)
    {
        if (m_socket->pendingDatagramSize() != -1) {
            int datagram_size = m_socket->pendingDatagramSize();
            //std::cout << "Datagram received, size " << datagram_size << std::endl;

            recvbuf = new char[datagram_size];
            m_socket->readDatagram(recvbuf,datagram_size);

            if (datagram_size > ( sizeof(gfw_datagram_t))) {
                gfw_datagram_t *gfww = (gfw_datagram_t *)recvbuf;

                if (gfww->op == GFW_OP_PING_ACK) {
                    // we got a pong.. add the device
                    gigabee_device_t *dev = new gigabee_device_t;
                    memset(dev,0,sizeof(gigabee_device_t));
                    memcpy(dev->mac, &gfww->data[0], 6);
                    memcpy(&dev->ip, &gfww->data[6], 4);
                    memcpy(&dev->test_result, &gfww->data[10], 4);

                    m_deviceList->append(*dev);

                    std::cout << ".";


                }


            }


            delete recvbuf;
        }

    }
    //std::cout << "Discovery Elapsed " << time.elapsed() << std::endl;


    m_socket->close();
    delete m_socket;
    m_socket = NULL;


    emit newDeviceList(m_deviceList);

}

void gfwUpgrader::loadFlashImage(const QString &fileName)
{

    QByteArray flash_file;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly) == true) {
        flash_file = file.readAll();
        file.close();

        // allocate
        if (m_flashData != NULL) {
            delete m_flashData;
            m_flashData = NULL;
        }
        m_flashData = new unsigned char[flash_file.length()];
        memcpy(m_flashData, flash_file.constData(), flash_file.length());
        m_flashDataLength = flash_file.length();

        std::cout << "Filelength: " << flash_file.length() << " bytes" << std::endl;

        if (flash_file.length() > (8 * 1024 * 1024)) {

            m_isFlashFileOK = false;
            std::cout << "ERROR: File length exceeds 8 Mb (flash size)." << std::endl;
            QString stat("File  not OK");
            emit flashDataOK(false);
            emit status(stat);
            return;

        }

        m_isFlashFileOK = true;
        QString stat("File OK");
        emit flashDataOK(true);
        emit status(stat);

    } else {
        m_isFlashFileOK = false;
        std::cout << "Cannot open file." << std::endl;
        QString stat("File  not OK");
        emit flashDataOK(false);
        emit status(stat);
    }


}



void gfwUpgrader::startUpgrade(QList<gigabee_device_t> *chosenDevices)
{
    // ok send the flash image...
    if ((m_isFlashFileOK == false) || (m_flashData == NULL))
    {
        QString a("Flash file not loaded.");
        emit status(a);
        return;
    }

    if (chosenDevices->size() < 1) {
        std::cout << "No devices to upgrade, quitting." << std::endl;
        return;
    }

    //std::cout << "Start transfer!" << std::endl;
    // start sending


    // create instances of gfwDeviceCommunicator.. 1 for each selected
    // device

    int num_of_devices = chosenDevices->size();
    m_num_of_devices_being_updated = num_of_devices;
    m_communicators = new gfwDeviceCommunicator[num_of_devices];
    QFuture<int> *futures = new QFuture<int>[num_of_devices];
    for (int i = 0; i < num_of_devices; i++) {

        m_communicators[i].setup(chosenDevices->value(i).mac,
                                 chosenDevices->value(i).ip,
                                 m_flashData, m_flashDataLength);

        //comm[i].startTransmission();
        //m_communicators[i].startSimulatedTransmission();

        futures[i] = QtConcurrent::run(&m_communicators[i],
                                       &gfwDeviceCommunicator::startTransmission);

        connect(&m_communicators[i],SIGNAL(signalTransmissionProgress(int)),
                this, SLOT(progressSlot(int)),
                Qt::QueuedConnection);


    }

    bool future_running = true;
    while (future_running) {

        QCoreApplication::processEvents();

        // if any future is running, keep looping
        future_running = false;
        for (int j = 0; j < num_of_devices; j ++) {
            if (futures[j].isFinished() == false)
                future_running = true;
        }


    }


    delete [] futures;


    return;
}

void gfwUpgrader::progressSlot(int prog)
{

    //printf("Progress! %d\n", prog);


    emit updateProgressSignal();

}
