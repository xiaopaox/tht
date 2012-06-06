#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QNetworkReply>
#include <QByteArray>
#include <QSslError>
#include <QPointer>
#include <QObject>
#include <QString>
#include <QList>

class QNetworkAccessManager;
class QUrl;

class NetworkAccess : public QObject
{
    Q_OBJECT

public:
    explicit NetworkAccess(QObject *parent = 0);
    ~NetworkAccess();

    void get(const QUrl &url);

    QByteArray data() const;

    QNetworkReply::NetworkError error() const;

private:
    void deleteReply();

signals:
    void finished();

private slots:
    void slotNetworkError(QNetworkReply::NetworkError);
    void slotSslErrors(QNetworkReply*, const QList<QSslError> &list);
    void slotNetworkData();
    void slotNetworkDone();

private:
    QNetworkReply::NetworkError m_error;
    QNetworkAccessManager *m_manager;
    QPointer<QNetworkReply> m_reply;
    QByteArray m_data;
};

inline
QNetworkReply::NetworkError NetworkAccess::error() const
{
    return m_error;
}

inline
QByteArray NetworkAccess::data() const
{
    return m_data;
}

#endif // TICKERINFORMATIONFETCHER_H