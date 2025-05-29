#include "bagelclient.h"
#include <QNetworkRequest>
#include <QJsonArray>
#include <QDebug>

BagelClient::BagelClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:12000")
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &BagelClient::handleNetworkReply);
}

void BagelClient::sendChatMessage(const QString &message)
{
    QJsonObject params;
    params["message"] = message;
    params["conversation_id"] = "ide_chat";
    
    QJsonObject requestData = createRequestData("chat", params);
    sendRequest("/chat", requestData);
}

void BagelClient::generateCode(const QString &prompt, const QString &language)
{
    QJsonObject params;
    params["prompt"] = prompt;
    params["language"] = language;
    params["max_tokens"] = 1000;
    
    QJsonObject requestData = createRequestData("generate_code", params);
    sendRequest("/generate", requestData);
}

void BagelClient::explainCode(const QString &code, const QString &language)
{
    QJsonObject params;
    params["code"] = code;
    params["language"] = language;
    
    QJsonObject requestData = createRequestData("explain_code", params);
    sendRequest("/explain", requestData);
}

void BagelClient::generateImage(const QString &prompt)
{
    QJsonObject params;
    params["prompt"] = prompt;
    params["size"] = "512x512";
    
    QJsonObject requestData = createRequestData("generate_image", params);
    sendRequest("/generate_image", requestData);
}

void BagelClient::checkHealth()
{
    QNetworkRequest request(QUrl(m_baseUrl + "/health"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->get(request);
    m_pendingRequests[reply] = "health";
}

void BagelClient::sendRequest(const QString &endpoint, const QJsonObject &data)
{
    QNetworkRequest request(QUrl(m_baseUrl + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(data);
    QByteArray requestData = doc.toJson();
    
    QNetworkReply *reply = m_networkManager->post(request, requestData);
    m_pendingRequests[reply] = endpoint;
}

QJsonObject BagelClient::createRequestData(const QString &type, const QJsonObject &params)
{
    QJsonObject data;
    data["type"] = type;
    data["params"] = params;
    return data;
}

void BagelClient::handleNetworkReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    QString endpoint = m_pendingRequests.value(reply, "unknown");
    m_pendingRequests.remove(reply);
    
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Network error: %1").arg(reply->errorString());
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }
    
    QByteArray responseData = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        QString errorMsg = QString("JSON parse error: %1").arg(parseError.errorString());
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }
    
    QJsonObject response = doc.object();
    
    // Handle different response types
    if (endpoint == "/health") {
        bool isHealthy = response.value("status").toString() == "healthy";
        emit healthCheckResult(isHealthy);
    }
    else if (endpoint == "/chat") {
        if (response.contains("response")) {
            QString chatResponse = response.value("response").toString();
            emit chatResponseReceived(chatResponse);
        } else if (response.contains("error")) {
            emit errorOccurred(response.value("error").toString());
        }
    }
    else if (endpoint == "/generate") {
        if (response.contains("code")) {
            QString code = response.value("code").toString();
            QString explanation = response.value("explanation").toString();
            emit codeGenerated(code, explanation);
        } else if (response.contains("error")) {
            emit errorOccurred(response.value("error").toString());
        }
    }
    else if (endpoint == "/explain") {
        if (response.contains("explanation")) {
            QString explanation = response.value("explanation").toString();
            emit codeExplained(explanation);
        } else if (response.contains("error")) {
            emit errorOccurred(response.value("error").toString());
        }
    }
    else if (endpoint == "/generate_image") {
        if (response.contains("image_url")) {
            QString imageUrl = response.value("image_url").toString();
            emit imageGenerated(imageUrl);
        } else if (response.contains("error")) {
            emit errorOccurred(response.value("error").toString());
        }
    }
    
    reply->deleteLater();
}