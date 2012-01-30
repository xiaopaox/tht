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

#ifndef THT_H
#define THT_H

#include <QWidget>
#include <QList>

#include <windows.h>

namespace Ui
{
    class THT;
}

class QGridLayout;
class QTimer;
class QMenu;

class List;

class THT : public QWidget
{
    Q_OBJECT
    
public:
    explicit THT(QWidget *parent = 0);
    ~THT();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    void sendKey(int vkey, bool extended = false) const;
    void sendString(const QString &str) const;
    void rebuildUi();
    void checkWindows();
    void loadNextWindow();

private slots:
    void slotCheckActive();
    void slotAbout();
    void slotAboutQt();
    void slotOptions();
    void slotMoveLeft(const QString &ticker);
    void slotMoveRight(const QString &ticker);
    void slotLoadTicker(const QString &ticker);
    void slotLoadToNextWindow();

private:
    Ui::THT *ui;
    QList<HWND> m_windows;
    int m_currentWindow;
    bool m_running;
    QTimer *m_timerCheckActive;
    QTimer *m_timerLoadToNextWindow;
    uint m_loadToNextInterval;
    qint64 m_startupTime;
    QMenu *m_menu;
    QList<List *> m_lists;
    QGridLayout *m_layout;
    QString m_ticker;
};

#endif // THT_H
