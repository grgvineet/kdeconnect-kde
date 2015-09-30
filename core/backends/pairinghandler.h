/**
 * Copyright 2015 Vineet Garg <grg.vineet@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KDECONNECT_PAIRINGHANDLER_H
#define KDECONNECT_PAIRINGHANDLER_H

#include "networkpackage.h"

#include <QTimer>

class Device;
class DeviceLink;

/*
 * This class separates the pairing interface for each type of link.
 * Since different links can pair via different methods, like for LanLink certificate and public key should be shared,
 * for Bluetooth link they should be paired via bluetooth etc.
 * Each "Device" instance maintains a hash map for these pairing handlers so that there can be single pairing handler per
 * per link type per device.
 * Pairing handler keeps information about device, latest link, and pair status of the link
 * During first pairing process, the pairing process is nearly same as old process.
 * After that if any one of the link is paired, then we can say that device is paired, so new link will pair automatically
 */

class PairingHandler : public QObject
{
    Q_OBJECT
protected:

    enum PairStatus {
        NotPaired,
        Requested,
        RequestedByPeer,
        Paired,
    };

    QTimer m_pairingTimeout;
    Device* m_device;
    DeviceLink* m_deviceLink; // We keep the latest link here, if this is destroyed without new link, linkDestroyed is emitted and device will destroy pairing handler
    PairStatus m_pairStatus;

public:
    PairingHandler(Device* device);
    virtual ~PairingHandler() { }

    void setLink(DeviceLink* dl);
    bool isPaired() const { return m_pairStatus == PairStatus::Paired; };
    bool pairRequested() const { return m_pairStatus == PairStatus::Requested; }

    virtual void createPairPackage(NetworkPackage& np) = 0;
    virtual void packageReceived(const NetworkPackage& np) = 0;
    virtual bool requestPairing() = 0;
    virtual bool acceptPairing() = 0;
    virtual void rejectPairing() = 0;
    virtual void unpair() = 0;

public Q_SLOTS:
    void linkDestroyed(QObject*);
    virtual void pairingTimeout() = 0;

private:
    virtual void setAsPaired() = 0;

Q_SIGNALS:
    void pairingDone();
    void unpairingDone();
    void pairingFailed(const QString& error);
    void linkNull();

};


#endif //KDECONNECT_PAIRINGHANDLER_H
