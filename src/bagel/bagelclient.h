#ifndef BAGELCLIENT_H
#define BAGELCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

class BagelClient : public QObject
{
    Q_OBJECT

public:
    explicit BagelClient(QObject *parent = nullptr);
    
    void sendChatMessage(const QString &message);
    void generateCode(const QString &prompt, const QString &language = "cpp");
    void explainCode(const QString &code, const QString &language = "cpp");
    void generateImage(const QString &prompt);
    void checkHealth();

signals:
    void chatResponseReceived(const QString &response);
    void codeGenerated(const QString &code, const QString &explanation);
    void codeExplained(const QString &explanation);
    void imageGenerated(const QString &imageUrl);
    void healthCheckResult(bool isHealthy);
    void errorOccurred(const QString &error);

private slots:
    void handleNetworkReply();

private:
    void sendRequest(const QString &endpoint, const QJsonObject &data);
    QJsonObject createRequestData(const QString &type, const QJsonObject &params);
    
    QNetworkAccessManager *m_networkManager;
    QString m_baseUrl;
    QMap<QNetworkReply*, QString> m_pendingRequests;
};

#endif // BAGELCLIENT_H