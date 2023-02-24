/****************************************************************************
**
** This file is part of the KD Chart library.
**
** SPDX-FileCopyrightText: 2001-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "mainwindow.h"

#include <KDChartAbstractCoordinatePlane>
#include <KDChartChart>
#include <KDChartDataValueAttributes>
#include <KDChartGridAttributes>
#include <KDChartLegend>
#include <KDChartMarkerAttributes>
#include <KDChartPolarDiagram>
#include <KDChartTextAttributes>

#include <QDebug>
#include <QPainter>

using namespace KDChart;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    // instantiate the KD Chart classes
    initKDChartClasses();

    // insert the KDChart::Chart into Qt's layout
    auto *chartLayout = new QHBoxLayout(chartFrame);
    m_chart->setGlobalLeading(2, 2, 2, 2);
    chartLayout->addWidget(m_chart);

    // wire up the KD Chart classes
    wireUpKDChartClasses();

    // initialize the ItemModel and fill in some data
    m_model.insertRows(0, 40);
    m_model.insertColumns(0, 5);
    setItemModelData();
}

void MainWindow::initKDChartClasses()
{
    m_chart = new Chart();
    m_diagram = new PolarDiagram();
    m_polarPlane = new PolarCoordinatePlane();
}

void MainWindow::wireUpKDChartClasses()
{
    m_chart->replaceCoordinatePlane(m_polarPlane);
    // note: We need to set a valid item model to the diagram,
    //       before we can add it to the coordinate plane.
    m_diagram->setModel(&m_model);
    m_chart->coordinatePlane()->replaceDiagram(m_diagram);
}

void MainWindow::setItemModelData()
{
    // For a change we do not read data from a resource file here,
    // but we just fill in the cells manually
    int value = 0;
    for (int column = 0; column < m_model.columnCount(); ++column) {
        for (int row = 0; row < m_model.rowCount(); ++row) {
            QModelIndex index = m_model.index(row, column);
            m_model.setData(index, QVariant(value++));
        }
    }
}

void MainWindow::on_startPositionSB_valueChanged(double pos)
{
    const int intValue = static_cast<int>(pos);
    startPositionSL->blockSignals(true);
    startPositionSL->setValue(intValue);
    startPositionSL->blockSignals(false);
    // note: We use the global getter method here, it will fall back
    //       automatically to return the default settings.
    static_cast<PolarCoordinatePlane *>(m_chart->coordinatePlane())->setStartPosition(pos);
    update();
}

void MainWindow::on_startPositionSL_valueChanged(int pos)
{
    auto qrealValue = static_cast<qreal>(pos);
    startPositionSB->blockSignals(true);
    startPositionSB->setValue(qrealValue);
    startPositionSB->blockSignals(false);
    // note: We use the global getter method here, it will fall back
    //       automatically to return the default settings.
    static_cast<PolarCoordinatePlane *>(m_chart->coordinatePlane())->setStartPosition(pos);
    update();
}

void MainWindow::on_circularGridCB_toggled(bool toggle)
{
    GridAttributes attrs(m_polarPlane->gridAttributes(true));
    attrs.setGridVisible(toggle);
    m_polarPlane->setGridAttributes(true, attrs);
    update();
}
void MainWindow::on_sagittalGridCB_toggled(bool toggle)
{
    GridAttributes attrs(m_polarPlane->gridAttributes(false));
    attrs.setGridVisible(toggle);
    m_polarPlane->setGridAttributes(false, attrs);
    update();
}
/* planned for future release:
void MainWindow::on_circularAxisCB_toggled( bool toggle )
{
    Q_UNUSED( toggle );
    update();
}
void MainWindow::on_sagittalAxisCB_toggled( bool toggle )
{
    Q_UNUSED( toggle );
    update();
}
*/
