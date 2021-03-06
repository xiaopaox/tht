/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMutableListIterator>
#include <QApplication>
#include <QMouseEvent>
#include <QTransform>
#include <QPainter>
#include <QCursor>
#include <QPixmap>
#include <QLineF>

#include "screenshoteditorwidget.h"
#include "screenshotcommentinput.h"
#include "selectablelabel.h"
#include "thtsettings.h"
#include "settings.h"
#include "tools.h"

ScreenshotEditorWidget::ScreenshotEditorWidget(QWidget *parent) :
    QLabel(parent),
    m_editType(None),
    m_wasPress(false)
{
    m_pixmaps[Buy] = QPixmap(":/images/cursor-buy.png");
    m_pixmaps[Sale] = QPixmap(":/images/cursor-sale.png");
    m_pixmaps[Stop] = QPixmap(":/images/cursor-stop.png");

    m_colors[Buy] = QColor(29, 199, 0);
    m_colors[Sale] = QColor(244, 49, 49);
    m_colors[Stop] = QColor(9, 98, 191);
}

void ScreenshotEditorWidget::setPixmap(const QPixmap &p)
{
    if(!p.isNull())
        setFixedSize(p.size());

    QLabel::setPixmap(p);
}

QPixmap ScreenshotEditorWidget::renderPixmap()
{
    foreach(SelectableLabel *l, m_labels)
    {
        l->setSelected(false, false);
    }

    return QPixmap::grabWidget(this);
}

void ScreenshotEditorWidget::cancel()
{
    if(m_editType != None)
    {
        m_editType = None;
        m_wasPress = false;
        resetCursor();
    }
}

void ScreenshotEditorWidget::restoreLabels()
{
    qDebug("Restore labels");

    clearLabels();
    m_labels = m_savedLabels;

    foreach(SelectableLabel *l, m_labels)
    {
        l->show();
    }
}

void ScreenshotEditorWidget::saveLabels()
{
    qDebug("Save labels");

    m_savedLabels = m_labels;
}

void ScreenshotEditorWidget::clearLabels()
{
    qDebug("Clear labels");

    QList<SelectableLabel *> toDelete;

    foreach(SelectableLabel *l, m_labels)
    {
        if(m_savedLabels.indexOf(l) < 0)
            toDelete.append(l);
    }

    qDeleteAll(toDelete);
}

void ScreenshotEditorWidget::startBuy()
{
    qDebug("Add buy");

    m_editType = Buy;
}

void ScreenshotEditorWidget::startSale()
{
    qDebug("Add sell");

    m_editType = Sale;
}

void ScreenshotEditorWidget::startStop()
{
    qDebug("Add stop");

    m_editType = Stop;
}

void ScreenshotEditorWidget::startText()
{
    qDebug("Add text");

    m_editType = Text;

    ScreenshotCommentInput sci(this);

    if(sci.exec() == QDialog::Accepted)
    {
        sci.saveSettings();

        m_textPixmap = sci.pixmap();

        if(m_textPixmap.isNull())
        {
            cancel();
            return;
        }

        setCursor(QCursor(m_textPixmap, 0, 0));
    }
    else
        m_editType = None;
}

void ScreenshotEditorWidget::startEllipse()
{
    qDebug("Add ellipse");

    m_editType = Ellipse;
    m_ellipseFillColor = SETTINGS_GET_COLOR(SETTING_ELLIPSE_FILL_COLOR);
}

void ScreenshotEditorWidget::deleteSelected()
{
    QList<SelectableLabel *> toDelete;
    QMutableListIterator<SelectableLabel *> it(m_labels);

    while(it.hasNext())
    {
        SelectableLabel *l = it.next();

        if(l->selected())
        {
            if(m_savedLabels.indexOf(l) < 0)
                toDelete.append(l);
            else
            {
                it.remove();
                l->hide();
                l->setSelected(false, false);
            }
        }
    }

    qDeleteAll(toDelete);
    update();
}

void ScreenshotEditorWidget::selectAll(bool select)
{
    qDebug("Select all labels: %s", select ? "yes" : "no");

    foreach(SelectableLabel *l, m_labels)
    {
        l->setSelected(select, false);
    }
}

void ScreenshotEditorWidget::slotSelected(bool s)
{
    emit selected(qobject_cast<SelectableLabel *>(sender()), s);
}

void ScreenshotEditorWidget::slotDestroyed()
{
    SelectableLabel *l = reinterpret_cast<SelectableLabel *>(sender());

    m_labels.removeAll(l);
    m_savedLabels.removeAll(l);

    update();
}

void ScreenshotEditorWidget::mousePressEvent(QMouseEvent *e)
{
    m_wasPress = true;
    m_startPoint = e->pos();
    m_currentPoint = Tools::invalidQPoint;

    QWidget::mousePressEvent(e);
}

void ScreenshotEditorWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(!m_wasPress || m_editType == None || m_editType == Text)
        return;

    m_currentPoint = e->pos();

    update();
}

void ScreenshotEditorWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    // not our event?
    if(!m_wasPress)
        return;

    m_wasPress = false;

    // regular click, deselect all
    if(m_editType == None)
    {
        selectAll(false);
        return;
    }

    m_currentPoint = e->pos();

    const int margin = 5;

    // outside border?
    if(m_editType != Ellipse && !rect().adjusted(margin, margin, -margin, -margin).contains(m_currentPoint))
    {
        cancel();
        update();
        return;
    }

    if(m_editType == Text)
    {
        addLabel(m_currentPoint, m_currentPoint, m_textPixmap);
        m_textPixmap = QPixmap();
    }
    else if(m_editType == Ellipse)
    {
        QRect rc = QRect(m_startPoint, m_currentPoint).normalized();

        QPixmap px(rc.size());

        px.fill(Qt::transparent);

        QPainter p(&px);
        p.setRenderHints(QPainter::Antialiasing);
        drawEllipse(&p, px.rect());

        addLabel(rc.topLeft(), rc.topLeft(), px);
    }
    else
        addLabel(m_currentPoint, m_startPoint, m_pixmaps[m_editType]);

    m_editType = None;

    update();
    resetCursor();
}

void ScreenshotEditorWidget::paintEvent(QPaintEvent *pe)
{
    QLabel::paintEvent(pe);

    QPainter p(this);

    p.setClipRect(pe->rect());
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    foreach(SelectableLabel *l, m_labels)
    {
        if(l->vectorStart() != l->vectorEnd())
            drawVector(&p, l->vectorColor(), l->vectorStart(), l->vectorEnd());
    }

    if(m_wasPress && m_currentPoint != Tools::invalidQPoint && m_editType != None && m_editType != Text)
    {
        if(m_editType != Ellipse)
        {
            drawVector(&p, m_colors[m_editType], m_currentPoint, m_startPoint);
        }
        else
            drawEllipse(&p, QRect(m_startPoint, m_currentPoint));
    }
}

SelectableLabel *ScreenshotEditorWidget::addLabel(const QPoint &startPoint, const QPoint &endPoint, const QPixmap &px)
{
    if(px.isNull() || m_editType == None)
        return 0;

    QColor elc;

    if(m_editType == Text)
        elc = SETTINGS_GET_COLOR(SETTING_SCREENSHOT_TEXT_COLOR);
    else if(m_editType == Ellipse)
    {
        elc = m_ellipseFillColor;
        elc.setAlpha(120);
    }
    else
        elc = m_colors[m_editType];

    SelectableLabel *l = new SelectableLabel(px, startPoint, endPoint, elc, this);

    connect(l, SIGNAL(selected(bool)), this, SLOT(slotSelected(bool)));
    connect(l, SIGNAL(destroyed()), this, SLOT(slotDestroyed()));

    l->show();

    m_labels.append(l);

    return l;
}

void ScreenshotEditorWidget::drawVector(QPainter *painter, const QColor &color, const QPoint &pt1, const QPoint &pt2)
{
    if(!painter)
        return;

    painter->setPen(QPen(color, 2));
    painter->drawLine(pt1, pt2);

    painter->setBrush(color);
    drawArrow(painter, pt1, pt2);
}

void ScreenshotEditorWidget::drawEllipse(QPainter *painter, const QRect &rc)
{
    if(!painter)
        return;

    painter->setBrush(m_ellipseFillColor);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(rc.normalized().adjusted(1, 1, -1, -1));
}

void ScreenshotEditorWidget::drawArrow(QPainter *painter, const QPoint &pt1, const QPoint &pt2)
{
    if(!painter)
        return;

    // arrow parameters
    const int width = 6;
    const int length = 10;

    // is vector long enough to have an arrow?
    if((pt2 - pt1).manhattanLength() <= length+8)
        return;

    painter->save();

    // arrow coordinates
    const QPoint pts[3] = { QPoint(0, 0), QPoint(-width/2, length), QPoint(width/2, length) };

    QTransform tr;
    tr.translate(pt1.x(), pt1.y());
    tr.rotate(270.0 - QLineF(pt1, pt1+QPoint(10,0)).angleTo(QLineF(pt1, pt2)));

    painter->setWorldTransform(tr);
    painter->drawPolygon(pts, sizeof(pts) / sizeof(pts[0]));

    painter->restore();
}

void ScreenshotEditorWidget::resetCursor()
{
    setCursor(Qt::ArrowCursor);
}
