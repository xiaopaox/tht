#include <QMouseEvent>
#include <QCursor>
#include <QPixmap>
#include <QTimer>

#include "screenshoteditorwidget.h"
#include "screenshotcommentinput.h"
#include "selectablelabel.h"

ScreenshotEditorWidget::ScreenshotEditorWidget(QWidget *parent) :
    QLabel(parent),
    m_editType(None),
    m_wasPress(false)
{
    m_pixmaps[0] = QPixmap(":/images/cursor-long.png");
    m_pixmaps[1] = QPixmap(":/images/cursor-short.png");
    m_pixmaps[2] = QPixmap(":/images/cursor-stop.png");
}

void ScreenshotEditorWidget::setPixmap(const QPixmap &p)
{
    if(!p.isNull())
        setFixedSize(p.size());

    QLabel::setPixmap(p);
}

QPixmap ScreenshotEditorWidget::pixmap()
{
    QList<SelectableLabel *> list = labels();

    foreach(SelectableLabel *l, list)
    {
        l->setSelected(false);
    }

    return QPixmap::grabWidget(this);
}

void ScreenshotEditorWidget::cancel()
{
    if(m_editType != None)
    {
        m_editType = None;
        m_wasPress = false;
        slotResetCursor();
    }
}

void ScreenshotEditorWidget::startLong()
{
    m_editType = Long;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startShort()
{
    m_editType = Short;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startStop()
{
    m_editType = Stop;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startText()
{
    m_editType = Text;
    ScreenshotCommentInput sci(this);

    if(sci.exec() == QDialog::Accepted)
    {
        m_textPixmap = sci.pixmap();

        if(m_textPixmap.isNull())
        {
            cancel();
            return;
        }

        setCursor(m_textPixmap);
    }
}

void ScreenshotEditorWidget::deleteSelected()
{
    QList<SelectableLabel *> list = labels();
    QList<SelectableLabel *> toDelete;

    foreach(SelectableLabel *l, list)
    {
        if(l->selected())
            toDelete.append(l);
    }

    qDeleteAll(toDelete);
}

void ScreenshotEditorWidget::selectAll(bool select)
{
    qDebug("Select all labels: %s", select ? "yes" : "no");

    QList<SelectableLabel *> list = labels();

    foreach(SelectableLabel *l, list)
    {
        l->setSelected(select, false);
    }
}

void ScreenshotEditorWidget::slotResetCursor()
{
    setCursor(Qt::ArrowCursor);
}

void ScreenshotEditorWidget::slotSelected(bool s)
{
    emit selected(qobject_cast<SelectableLabel *>(sender()), s);
}

void ScreenshotEditorWidget::slotDestroyed()
{
    m_labels.removeAll(reinterpret_cast<SelectableLabel *>(sender()));
}

void ScreenshotEditorWidget::mousePressEvent(QMouseEvent *e)
{
    m_wasPress = true;

    QWidget::mousePressEvent(e);
}

void ScreenshotEditorWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    if(m_editType == None || !m_wasPress)
    {
        // deselect all
        selectAll(false);
        return;
    }

    if(m_editType != Text)
        addLabel(e->pos(), m_pixmaps[m_editType]);
    else
    {
        addLabel(e->pos(), m_textPixmap);
        m_textPixmap = QPixmap();
    }

    m_editType = None;
    m_wasPress = false;

    QTimer::singleShot(50, this, SLOT(slotResetCursor()));
}

SelectableLabel *ScreenshotEditorWidget::addLabel(const QPoint &pt, const QPixmap &px)
{
    SelectableLabel *l = new SelectableLabel(px, this);

    connect(l, SIGNAL(selected(bool)), this, SLOT(slotSelected(bool)));
    connect(l, SIGNAL(destroyed()), this, SLOT(slotDestroyed()));

    l->move(pt - cursor().hotSpot() - QPoint(2, 2));
    l->show();

    m_labels.append(l);

    return l;
}
