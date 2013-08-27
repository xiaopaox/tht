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

#include <QToolButton>
#include <QMetaType>
#include <QLayout>
#include <QHash>
#include <QIcon>
#include <QDir>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "QXmppLogger.h"

#include "chatsettings.h"
#include "chatwindow.h"
#include "chatplugin.h"
#include "chattools.h"
#include "settings.h"
#include "roominfo.h"
#include "tools.h"

ChatPlugin::ChatPlugin() :
    Plugin(),
    m_window(0)
{
    THT_PLUGIN_INTERFACE_IMPLEMENTATION

    qRegisterMetaTypeStreamOperators<RoomInfo>("RoomInfo");
    qRegisterMetaTypeStreamOperators<QList<RoomInfo> >("QList<RoomInfo>");

    QHash<QString, QVariant> defaltValues;

    defaltValues.insert(SETTING_CHAT_POSITION, Tools::invalidQPoint);
    defaltValues.insert(SETTING_CHAT_FONT_SIZE, 8);
    defaltValues.insert(SETTING_CHAT_AUTO_LOGIN, true);
    defaltValues.insert(SETTING_CHAT_SAVE_ROOMS, true);
    defaltValues.insert(SETTING_CHAT_AUTO_LOGIN_TO_ROOMS, true);

    Settings::instance()->addDefaultValues(defaltValues);

    ChatTools::init();

    // NOTE log to file?
    QXmppLogger::getLogger()->setLoggingType(QXmppLogger::NoLogging);
    QXmppLogger::getLogger()->setLogFilePath(
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                                             QStandardPaths::writableLocation(QStandardPaths::TempLocation)
#else
                                             QDesktopServices::storageLocation(QDesktopServices::TempLocation)
#endif
                                             + QDir::separator()
                                             + "tht-chat-qxmpp.log");
}

ChatPlugin::~ChatPlugin()
{
    delete m_window;
}

bool ChatPlugin::init()
{
    m_button = new QToolButton;

    if(!m_button)
    {
        qWarning("Cannot create QPushButton object");
        return false;
    }

    m_button->setFixedSize(22, 22);
    m_button->setIcon(ChatTools::chatIcon().pixmap(16, 16));
    m_button->setToolTip(tr("Chat"));

    connect(m_button, SIGNAL(clicked()), this, SLOT(slotClicked()));

    return true;
}

void ChatPlugin::delayedInit()
{
    QWidget *c = containerLeft();

    if(!c)
        return;

    m_button->setParent(c);
    c->layout()->addWidget(m_button);

    if(SETTINGS_GET_BOOL(SETTING_CHAT_RESTORE_AT_STARTUP))
        m_button->click();
}

void ChatPlugin::slotClicked()
{
    if(!m_window)
    {
        m_window = new ChatWindow;

        if(!m_window)
            return;

        m_window->setWindowIcon(ChatTools::chatIcon());
        connect(m_window, SIGNAL(openTicker(QString)), this, SIGNAL(openTicker(QString)));
    }

    Tools::raiseWindow(m_window);
    m_window->activateWindow();
}

PLUGIN_CONSTRUCTOR(ChatPlugin)
