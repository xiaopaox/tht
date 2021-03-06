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

#ifndef TICKERNEIGHBORS_H
#define TICKERNEIGHBORS_H

#include <QKeySequence>
#include <QStringList>
#include <QDialog>
#include <QPoint>
#include <QList>

class QModelIndex;
class QCheckBox;
class QEvent;

namespace Ui
{
    class TickerNeighbors;
}

class TickerNeighbors : public QDialog
{
    Q_OBJECT

public:
    explicit TickerNeighbors(const QString &ticker, QWidget *parent = 0);
    ~TickerNeighbors();

    void showTicker(const QString &);

    virtual void setVisible(bool);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void silentlyCheck(QCheckBox *, bool check);
    void filterAndFetch();
    void resetTabOrders();
    void showNumberOfFilesToCopy(int n);

signals:
    void loadTicker(const QString &);

private slots:
    void slotFetch();
    void slotCopy();
    void slotSelectionChanged();
    void slotActivated(const QModelIndex &);
    void slotCheckboxChanged();
    void slotCopyCombo();

private:
    Ui::TickerNeighbors *ui;
    QStringList m_tickers;
    QObject *m_lastAction;
    QPoint m_pos;
    QKeySequence::StandardKey m_copy;
    QList<QCheckBox *> m_exchanges;
};

#endif // TICKERNEIGHBORS_H
