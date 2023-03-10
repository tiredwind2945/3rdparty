/****************************************************************************
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

#include "kdsysinfo.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>

namespace QRunInfo {

struct PathLongerThan
{
    bool operator()(const VolumeInfo &lhs, const VolumeInfo &rhs) const
    {
        return lhs.mountPath().length() > rhs.mountPath().length();
    }
};

VolumeInfo::VolumeInfo()
    : m_size(0)
    , m_availableSize(0)
{
}

VolumeInfo VolumeInfo::fromPath(const QString &path)
{
    QDir targetPath(QDir::cleanPath(path));
    QList<VolumeInfo> volumes = RunInfo::mountedVolumes();

    // sort by length to get the longest mount point (not just "/") first
    std::sort(volumes.begin(), volumes.end(), PathLongerThan());
    foreach (const VolumeInfo &volume, volumes) {
        const QDir volumePath(volume.mountPath());
        if (targetPath == volumePath)
            return volume;
#ifdef Q_OS_WIN
        if (QDir::toNativeSeparators(path).toLower().startsWith(volume.mountPath().toLower()))
#else
        // we need to take some care here, as canonical path might return an empty string if the target
        // does not exist yet
        if (targetPath.exists()) {
            // the target exist, we can solve the path and if it fits return
            if (targetPath.canonicalPath().startsWith(volume.mountPath()))
                return volume;
            continue;
        }

        // the target directory does not exist yet, we need to cd up till we find the first existing dir
        QStringList parts = targetPath.absolutePath().split(QDir::separator(),QString::SkipEmptyParts);
        while (targetPath.absolutePath() != QDir::rootPath()) {
            if (targetPath.exists())
                break;
            parts.pop_back();
            if (parts.isEmpty())
                targetPath = QDir(QDir::rootPath());
            else
                targetPath = QDir(QLatin1Char('/') + parts.join(QDir::separator()));
        }

        if (targetPath.canonicalPath().startsWith(volume.mountPath()))
#endif
            return volume;
    }
    return VolumeInfo();
}

QString VolumeInfo::mountPath() const
{
    return m_mountPath;
}

void VolumeInfo::setMountPath(const QString &path)
{
    m_mountPath = path;
}

QString VolumeInfo::fileSystemType() const
{
    return m_fileSystemType;
}

void VolumeInfo::setFileSystemType(const QString &type)
{
    m_fileSystemType = type;
}

QString VolumeInfo::volumeDescriptor() const
{
    return m_volumeDescriptor;
}

void VolumeInfo::setVolumeDescriptor(const QString &descriptor)
{
    m_volumeDescriptor = descriptor;
}

quint64 VolumeInfo::size() const
{
    return m_size;
}

void VolumeInfo::setSize(const quint64 &size)
{
    m_size = size;
}

quint64 VolumeInfo::availableSize() const
{
    return m_availableSize;
}

void VolumeInfo::setAvailableSize(const quint64 &available)
{
    m_availableSize = available;
}

bool VolumeInfo::operator==(const VolumeInfo &other) const
{
    return m_volumeDescriptor == other.m_volumeDescriptor;
}

} // QRUNINFO_NAMEPASE_END

QDebug operator<<(QDebug dbg, QRunInfo::VolumeInfo volume)
{
    return dbg << "QRunInfo::Volume(" << volume.mountPath() << ")";
}

QDebug operator<<(QDebug dbg, QRunInfo::ProcessInfo process)
{
    return dbg << "QRunInfo::ProcessInfo(" << process.id << ", " << process.name << ")";
}
