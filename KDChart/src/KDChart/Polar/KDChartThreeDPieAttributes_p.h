/****************************************************************************
**
** This file is part of the KD Chart library.
**
** SPDX-FileCopyrightText: 2001-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDCHARTTHREEDPIEATTRIBUTES_P_H
#define KDCHARTTHREEDPIEATTRIBUTES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Chart API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <KDChartAbstractThreeDAttributes_p.h>

#include <KDABLibFakes>

namespace KDChart {

/**
 * \internal
 */
class ThreeDPieAttributes::Private : public AbstractThreeDAttributes::Private
{
    friend class ThreeDPieAttributes;

public:
    Private();

private:
    bool useShadowColors = true;
};

KDCHART_DERIVED_PRIVATE_FOOTER(ThreeDPieAttributes, AbstractThreeDAttributes)
}

#endif // KDCHARTTHREEDPIEATTRIBUTES_P_H
