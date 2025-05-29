#ifndef BAGELCHATWIDGET_H
#define BAGELCHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QSplitter>
#include <QScrollArea>

class BagelClient;

class BagelChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BagelChatWidget(BagelClient *client, QWidget *parent = nullptr);

private slots:
    void sendMessage();
    void onChatResponse(const QString &response);
    void onCodeGenerated(const QString &code, const QString &explanation);
    void onCodeExplained(const QString &explanation);
    void onImageGenerated(const QString &imageUrl);
    void onError(const QString &error);
    void generateCode();
    void explainSelectedCode();
    void generateImage();

private:
    void setupUI();
    void addMessage(const QString &sender, const QString &message, bool isUser = false);
    void addCodeBlock(const QString &code, const QString &language);
    void addImageBlock(const QString &imageUrl);
    QString getSelectedCodeFromIDE();
    
    BagelClient *m_client;
    
    // UI Components
    QVBoxLayout *m_mainLayout;
    QTextEdit *m_chatDisplay;
    QLineEdit *m_messageInput;
    QPushButton *m_sendButton;
    
    // AI Tools
    QComboBox *m_modeCombo;
    QPushButton *m_generateCodeButton;
    QPushButton *m_explainCodeButton;
    QPushButton *m_generateImageButton;
    
    // Status
    QLabel *m_statusLabel;
};

#endif // BAGELCHATWIDGET_H