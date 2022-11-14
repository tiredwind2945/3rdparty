﻿/****************************************************************************
**
** Copyright (C) 2013 Klaralvdalens Datakonsult AB (KDAB)
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef KDSYSINFO_H
#define KDSYSINFO_H

#include "qruninfo_global.h"

#include <QtCore/QString>

namespace QRunInfo {

class QRUNINFOSHARED_EXPORT VolumeInfo
{
public:
    VolumeInfo();
    static VolumeInfo fromPath(const QString &path);

    QString mountPath() const;
    void setMountPath(const QString &path);

    QString fileSystemType() const;
    void setFileSystemType(const QString &type);

    QString volumeDescriptor() const;
    void setVolumeDescriptor(const QString &descriptor);

    quint64 size() const;
    void setSize(const quint64 &size);

    quint64 availableSize() const;
    void setAvailableSize(const quint64 &available);

    bool operator==(const VolumeInfo &other) const;

private:
    QString m_mountPath;
    QString m_fileSystemType;
    QString m_volumeDescriptor;

    quint64 m_size;
    quint64 m_availableSize;
};

struct QRUNINFOSHARED_EXPORT ProcessInfo
{
    quint32 id;
    QString name;
};

class QRUNINFOSHARED_EXPORT RunInfo
{
public:
    static quint64 installedMemory();
    static QList<VolumeInfo> mountedVolumes();
    static QList<ProcessInfo> runningProcesses();
    static bool killProcess(const ProcessInfo &process, int msecs = 30000);
    static bool pathIsOnLocalDevice(const QString &path);
    static quint32 currentProcessId();
    static quint32 parentProcessId();
};

} // QRUNINFO_NAMEPASE_END


QT_BEGIN_NAMESPACE
class QDebug;
QT_END_NAMESPACE

QDebug operator<<(QDebug dbg, QRunInfo::VolumeInfo volume);
QDebug operator<<(QDebug dbg, QRunInfo::ProcessInfo process);

#endif // KDSYSINFO_H
