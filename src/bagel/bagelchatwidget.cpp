#include "bagelchatwidget.h"
#include "bagelclient.h"
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QDir>

BagelChatWidget::BagelChatWidget(BagelClient *client, QWidget *parent)
    : QWidget(parent)
    , m_client(client)
{
    setupUI();
    
    // Connect BAGEL client signals
    connect(m_client, &BagelClient::chatResponseReceived,
            this, &BagelChatWidget::onChatResponse);
    connect(m_client, &BagelClient::codeGenerated,
            this, &BagelChatWidget::onCodeGenerated);
    connect(m_client, &BagelClient::codeExplained,
            this, &BagelChatWidget::onCodeExplained);
    connect(m_client, &BagelClient::imageGenerated,
            this, &BagelChatWidget::onImageGenerated);
    connect(m_client, &BagelClient::errorOccurred,
            this, &BagelChatWidget::onError);
    
    // Check BAGEL server health on startup
    m_client->checkHealth();
}

void BagelChatWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Status label
    m_statusLabel = new QLabel("Connecting to BAGEL AI...");
    m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
    m_mainLayout->addWidget(m_statusLabel);
    
    // Mode selection
    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(new QLabel("Mode:"));
    
    m_modeCombo = new QComboBox;
    m_modeCombo->addItems({"Chat", "Code Generation", "Code Explanation", "Image Generation"});
    modeLayout->addWidget(m_modeCombo);
    modeLayout->addStretch();
    
    m_mainLayout->addLayout(modeLayout);
    
    // AI Tools buttons
    QHBoxLayout *toolsLayout = new QHBoxLayout;
    
    m_generateCodeButton = new QPushButton("Generate Code");
    m_generateCodeButton->setToolTip("Generate code from description");
    toolsLayout->addWidget(m_generateCodeButton);
    
    m_explainCodeButton = new QPushButton("Explain Code");
    m_explainCodeButton->setToolTip("Explain selected code");
    toolsLayout->addWidget(m_explainCodeButton);
    
    m_generateImageButton = new QPushButton("Generate Image");
    m_generateImageButton->setToolTip("Generate image from description");
    toolsLayout->addWidget(m_generateImageButton);
    
    m_mainLayout->addLayout(toolsLayout);
    
    // Chat display
    m_chatDisplay = new QTextEdit;
    m_chatDisplay->setReadOnly(true);
    m_chatDisplay->setMinimumHeight(300);
    m_chatDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: #1e1e1e;"
        "    color: #ffffff;"
        "    border: 1px solid #3e3e3e;"
        "    border-radius: 4px;"
        "    padding: 8px;"
        "    font-family: 'Consolas', monospace;"
        "}"
    );
    m_mainLayout->addWidget(m_chatDisplay);
    
    // Message input
    QHBoxLayout *inputLayout = new QHBoxLayout;
    
    m_messageInput = new QLineEdit;
    m_messageInput->setPlaceholderText("Type your message or prompt here...");
    m_messageInput->setStyleSheet(
        "QLineEdit {"
        "    background-color: #2d2d2d;"
        "    color: #ffffff;"
        "    border: 1px solid #3e3e3e;"
        "    border-radius: 4px;"
        "    padding: 8px;"
        "    font-size: 12px;"
        "}"
    );
    inputLayout->addWidget(m_messageInput);
    
    m_sendButton = new QPushButton("Send");
    m_sendButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #0e639c;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1177bb;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0d5a8a;"
        "}"
    );
    inputLayout->addWidget(m_sendButton);
    
    m_mainLayout->addLayout(inputLayout);
    
    // Connect signals
    connect(m_sendButton, &QPushButton::clicked, this, &BagelChatWidget::sendMessage);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &BagelChatWidget::sendMessage);
    connect(m_generateCodeButton, &QPushButton::clicked, this, &BagelChatWidget::generateCode);
    connect(m_explainCodeButton, &QPushButton::clicked, this, &BagelChatWidget::explainSelectedCode);
    connect(m_generateImageButton, &QPushButton::clicked, this, &BagelChatWidget::generateImage);
    
    // Add welcome message
    addMessage("BAGEL AI", "Welcome to BAGEL AI Assistant! I can help you with:\n"
                           "• Code generation and explanation\n"
                           "• Image generation\n"
                           "• General programming questions\n"
                           "• Project assistance\n\n"
                           "How can I help you today?");
}

void BagelChatWidget::sendMessage()
{
    QString message = m_messageInput->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    // Add user message to chat
    addMessage("You", message, true);
    m_messageInput->clear();
    
    // Send to BAGEL based on mode
    QString mode = m_modeCombo->currentText();
    if (mode == "Chat") {
        m_client->sendChatMessage(message);
    } else if (mode == "Code Generation") {
        m_client->generateCode(message, "cpp");
    } else if (mode == "Image Generation") {
        m_client->generateImage(message);
    }
    
    m_statusLabel->setText("Processing...");
    m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
}

void BagelChatWidget::generateCode()
{
    QString prompt = m_messageInput->text().trimmed();
    if (prompt.isEmpty()) {
        QMessageBox::information(this, "Generate Code", 
                               "Please enter a description of the code you want to generate.");
        return;
    }
    
    addMessage("You", QString("Generate code: %1").arg(prompt), true);
    m_client->generateCode(prompt, "cpp");
    m_messageInput->clear();
    
    m_statusLabel->setText("Generating code...");
    m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
}

void BagelChatWidget::explainSelectedCode()
{
    QString code = getSelectedCodeFromIDE();
    if (code.isEmpty()) {
        QMessageBox::information(this, "Explain Code", 
                               "Please select some code in the editor first.");
        return;
    }
    
    addMessage("You", QString("Explain this code:\n```cpp\n%1\n```").arg(code), true);
    m_client->explainCode(code, "cpp");
    
    m_statusLabel->setText("Explaining code...");
    m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
}

void BagelChatWidget::generateImage()
{
    QString prompt = m_messageInput->text().trimmed();
    if (prompt.isEmpty()) {
        QMessageBox::information(this, "Generate Image", 
                               "Please enter a description of the image you want to generate.");
        return;
    }
    
    addMessage("You", QString("Generate image: %1").arg(prompt), true);
    m_client->generateImage(prompt);
    m_messageInput->clear();
    
    m_statusLabel->setText("Generating image...");
    m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
}

void BagelChatWidget::onChatResponse(const QString &response)
{
    addMessage("BAGEL AI", response);
    m_statusLabel->setText("Ready");
    m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
}

void BagelChatWidget::onCodeGenerated(const QString &code, const QString &explanation)
{
    addMessage("BAGEL AI", explanation);
    addCodeBlock(code, "cpp");
    m_statusLabel->setText("Ready");
    m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
}

void BagelChatWidget::onCodeExplained(const QString &explanation)
{
    addMessage("BAGEL AI", explanation);
    m_statusLabel->setText("Ready");
    m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
}

void BagelChatWidget::onImageGenerated(const QString &imageUrl)
{
    addMessage("BAGEL AI", "Image generated successfully!");
    addImageBlock(imageUrl);
    m_statusLabel->setText("Ready");
    m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
}

void BagelChatWidget::onError(const QString &error)
{
    addMessage("System", QString("Error: %1").arg(error));
    m_statusLabel->setText("Error occurred");
    m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
}

void BagelChatWidget::addMessage(const QString &sender, const QString &message, bool isUser)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString color = isUser ? "#4a9eff" : "#ffffff";
    QString senderColor = isUser ? "#4a9eff" : "#00ff88";
    
    QString html = QString(
        "<div style='margin-bottom: 10px;'>"
        "<span style='color: %1; font-weight: bold;'>%2</span> "
        "<span style='color: #888888; font-size: 10px;'>%3</span><br>"
        "<span style='color: %4;'>%5</span>"
        "</div>"
    ).arg(senderColor, sender, timestamp, color, message.toHtmlEscaped());
    
    m_chatDisplay->append(html);
    
    // Scroll to bottom
    QTextCursor cursor = m_chatDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_chatDisplay->setTextCursor(cursor);
}

void BagelChatWidget::addCodeBlock(const QString &code, const QString &language)
{
    QString html = QString(
        "<div style='margin: 10px 0; background-color: #2d2d2d; border: 1px solid #3e3e3e; border-radius: 4px; padding: 10px;'>"
        "<div style='color: #888888; font-size: 10px; margin-bottom: 5px;'>Generated %1 Code:</div>"
        "<pre style='color: #ffffff; font-family: Consolas, monospace; margin: 0; white-space: pre-wrap;'>%2</pre>"
        "<div style='margin-top: 5px;'>"
        "<button onclick='navigator.clipboard.writeText(\"%3\")' style='background-color: #0e639c; color: white; border: none; padding: 4px 8px; border-radius: 2px; cursor: pointer;'>Copy</button>"
        "</div>"
        "</div>"
    ).arg(language.toUpper(), code.toHtmlEscaped(), code.toHtmlEscaped());
    
    m_chatDisplay->append(html);
}

void BagelChatWidget::addImageBlock(const QString &imageUrl)
{
    QString html = QString(
        "<div style='margin: 10px 0; text-align: center;'>"
        "<img src='%1' style='max-width: 300px; max-height: 300px; border-radius: 4px;' />"
        "</div>"
    ).arg(imageUrl);
    
    m_chatDisplay->append(html);
}

QString BagelChatWidget::getSelectedCodeFromIDE()
{
    // This would need to be connected to the main window's current editor
    // For now, return empty string
    // TODO: Implement connection to main window's active editor
    return QString();
}