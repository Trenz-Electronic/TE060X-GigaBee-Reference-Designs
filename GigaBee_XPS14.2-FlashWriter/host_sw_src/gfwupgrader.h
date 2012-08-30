#ifndef GFWUPGRADER_H
#define GFWUPGRADER_H

#include <QObject>
#include <QtCore/QCoreApplication>
#include <cstdio>

#include <QtNetwork/QUdpSocket>
#include "gfwdevicecommunicator.h"
#include "my_types.h"



class gfwUpgrader : public QObject
{
    Q_OBJECT
public:
    explicit gfwUpgrader(QObject *parent = 0);
    ~gfwUpgrader();
    QByteArray *getTransferProgress();

    int m_discovery_msec_timeout;
    bool m_isFlashFileOK;
    QList<gigabee_device_t> *m_deviceList;

    int m_num_of_devices_being_updated;
    gfwDeviceCommunicator *m_communicators;

private:
    QUdpSocket *m_socket;
    unsigned char *m_flashData;
    int m_flashDataLength;


signals:
    void status(QString & statusString);
    void flashDataOK(bool is);
    void newDeviceList(QList<gigabee_device_t> *newlist);
    void updateProgressSignal();

public slots:
    void doDiscover();
    void loadFlashImage(const QString &fileName);
    void startUpgrade(QList<gigabee_device_t> *chosenDevices);
    void progressSlot(int prog);


};


class DummyMainClass : public QObject {
    Q_OBJECT
public:
    explicit DummyMainClass(gfwUpgrader *gfwupgrader = NULL) : QObject()
    {
        m_gfwupgrader = gfwupgrader;
    }
    gfwUpgrader *m_gfwupgrader;


public slots:
    void exit_app(int code) {
        QCoreApplication::exit(code);
    }

    void writeProgress() {

        if (m_gfwupgrader) {

            QByteArray *prog = m_gfwupgrader->getTransferProgress();
            // print out status
            printf("\r"); // beginning of the line

            for (int i = 0; i < prog->size(); i++) {

                char val = prog->at(i);

                if (val <= 100)
                    printf(" %3.d%% |", val);
                else if (val == 101)
                    printf(" F WR |");
                else if (val == 255)
                    printf(" ERR  |");
                else
                    printf(" ???? |");

            }
            delete prog;
        }

    }


};


#endif // GFWUPGRADER_H
