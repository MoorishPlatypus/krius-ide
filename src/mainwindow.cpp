#include "mainwindow.h"
#include "editor/codeeditor.h"
#include "bagel/bagelclient.h"
#include "bagel/bagelchatwidget.h"
#include "project/projectmanager.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QKeySequence>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QTreeWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_projectTree(nullptr)
    , m_outputPanel(nullptr)
    , m_bagelClient(nullptr)
    , m_bagelWidget(nullptr)
    , m_projectManager(nullptr)
    , m_bagelDock(nullptr)
{
    setupUI();
    setupMenus();
    setupToolbars();
    setupDockWidgets();
    setupStatusBar();
    setupBagel();
    setupProjectManager();
    
    // Apply dark theme
    applyDarkTheme();
    
    // Set window properties
    setWindowTitle("Krius IDE - C++ Development Environment with BAGEL AI");
    setMinimumSize(1200, 800);
    resize(1400, 900);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Central widget - tabbed editor
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    setCentralWidget(m_tabWidget);
    
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    
    // Create initial welcome tab
    createWelcomeTab();
}

void MainWindow::setupMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    QAction *newAction = fileMenu->addAction("&New File");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    
    QAction *openAction = fileMenu->addAction("&Open File");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    
    QAction *saveAction = fileMenu->addAction("&Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    
    QAction *saveAsAction = fileMenu->addAction("Save &As...");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // Edit menu
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("&Undo")->setShortcut(QKeySequence::Undo);
    editMenu->addAction("&Redo")->setShortcut(QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("Cu&t")->setShortcut(QKeySequence::Cut);
    editMenu->addAction("&Copy")->setShortcut(QKeySequence::Copy);
    editMenu->addAction("&Paste")->setShortcut(QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction("&Find")->setShortcut(QKeySequence::Find);
    editMenu->addAction("Find && &Replace")->setShortcut(QKeySequence::Replace);
    
    // Project menu
    QMenu *projectMenu = menuBar()->addMenu("&Project");
    
    QAction *openProjectAction = projectMenu->addAction("&Open Project Folder");
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::openProject);
    
    QAction *closeProjectAction = projectMenu->addAction("&Close Project");
    connect(closeProjectAction, &QAction::triggered, this, &MainWindow::closeProject);
    
    projectMenu->addSeparator();
    
    QAction *buildAction = projectMenu->addAction("&Build Project");
    buildAction->setShortcut(QKeySequence("Ctrl+B"));
    connect(buildAction, &QAction::triggered, this, &MainWindow::buildProject);
    
    QAction *runAction = projectMenu->addAction("&Run Project");
    runAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(runAction, &QAction::triggered, this, &MainWindow::runProject);
    
    // AI menu
    QMenu *aiMenu = menuBar()->addMenu("&AI");
    
    QAction *toggleBagelAction = aiMenu->addAction("Toggle &BAGEL Assistant");
    toggleBagelAction->setShortcut(QKeySequence("Ctrl+Shift+A"));
    toggleBagelAction->setCheckable(true);
    connect(toggleBagelAction, &QAction::triggered, this, &MainWindow::toggleBagel);
    
    aiMenu->addSeparator();
    aiMenu->addAction("Generate &Code")->setShortcut(QKeySequence("Ctrl+Shift+G"));
    aiMenu->addAction("&Explain Code")->setShortcut(QKeySequence("Ctrl+Shift+E"));
    aiMenu->addAction("Generate &Image")->setShortcut(QKeySequence("Ctrl+Shift+I"));
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Project Explorer");
    viewMenu->addAction("&Output Panel");
    viewMenu->addAction("&BAGEL Assistant");
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = helpMenu->addAction("&About Krius IDE");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupToolbars()
{
    QToolBar *mainToolbar = addToolBar("Main");
    mainToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    QAction *newAction = mainToolbar->addAction("New");
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    
    QAction *openAction = mainToolbar->addAction("Open");
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    
    QAction *saveAction = mainToolbar->addAction("Save");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    
    mainToolbar->addSeparator();
    
    QAction *buildAction = mainToolbar->addAction("Build");
    connect(buildAction, &QAction::triggered, this, &MainWindow::buildProject);
    
    QAction *runAction = mainToolbar->addAction("Run");
    connect(runAction, &QAction::triggered, this, &MainWindow::runProject);
    
    mainToolbar->addSeparator();
    
    QAction *bagelAction = mainToolbar->addAction("BAGEL AI");
    bagelAction->setCheckable(true);
    connect(bagelAction, &QAction::triggered, this, &MainWindow::toggleBagel);
}

void MainWindow::setupDockWidgets()
{
    // Project Explorer
    QDockWidget *projectDock = new QDockWidget("Project Explorer", this);
    projectDock->setObjectName("ProjectDock");
    m_projectTree = new QTreeWidget();
    m_projectTree->setHeaderLabel("Project Files");
    m_projectTree->setRootIsDecorated(true);
    connect(m_projectTree, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onProjectFileDoubleClicked);
    projectDock->setWidget(m_projectTree);
    addDockWidget(Qt::LeftDockWidgetArea, projectDock);
    
    // Output Panel
    QDockWidget *outputDock = new QDockWidget("Output", this);
    outputDock->setObjectName("OutputDock");
    m_outputPanel = new QTextEdit();
    m_outputPanel->setReadOnly(true);
    m_outputPanel->setMaximumHeight(200);
    m_outputPanel->append("Krius IDE Output Panel");
    m_outputPanel->append("Ready for development...");
    outputDock->setWidget(m_outputPanel);
    addDockWidget(Qt::BottomDockWidgetArea, outputDock);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready - Krius IDE with BAGEL AI Integration");
}

void MainWindow::setupBagel()
{
    // Create BAGEL client
    m_bagelClient = new BagelClient(this);
    
    // Create BAGEL chat widget
    m_bagelWidget = new BagelChatWidget(m_bagelClient, this);
    
    // Create BAGEL dock widget (initially hidden)
    m_bagelDock = new QDockWidget("BAGEL AI Assistant", this);
    m_bagelDock->setObjectName("BagelDock");
    m_bagelDock->setWidget(m_bagelWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_bagelDock);
    m_bagelDock->hide();
    
    // Connect BAGEL client signals
    connect(m_bagelClient, &BagelClient::healthCheckResult, this, &MainWindow::onBagelHealthCheck);
}

void MainWindow::setupProjectManager()
{
    m_projectManager = new ProjectManager(this);
    
    connect(m_projectManager, &ProjectManager::projectOpened, this, &MainWindow::onProjectOpened);
    connect(m_projectManager, &ProjectManager::projectClosed, this, &MainWindow::onProjectClosed);
    connect(m_projectManager, &ProjectManager::fileAdded, this, &MainWindow::onProjectFileAdded);
}

void MainWindow::applyDarkTheme()
{
    setStyleSheet(
        "QMainWindow {"
        "    background-color: #2b2b2b;"
        "    color: #ffffff;"
        "}"
        "QMenuBar {"
        "    background-color: #3c3c3c;"
        "    color: #ffffff;"
        "    border-bottom: 1px solid #555555;"
        "    padding: 2px;"
        "}"
        "QMenuBar::item {"
        "    background-color: transparent;"
        "    padding: 6px 12px;"
        "    border-radius: 3px;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #555555;"
        "}"
        "QMenu {"
        "    background-color: #3c3c3c;"
        "    color: #ffffff;"
        "    border: 1px solid #555555;"
        "    padding: 4px;"
        "}"
        "QMenu::item {"
        "    padding: 6px 20px;"
        "    border-radius: 3px;"
        "}"
        "QMenu::item:selected {"
        "    background-color: #555555;"
        "}"
        "QToolBar {"
        "    background-color: #3c3c3c;"
        "    border: none;"
        "    spacing: 4px;"
        "    padding: 4px;"
        "}"
        "QToolBar QToolButton {"
        "    background-color: transparent;"
        "    color: #ffffff;"
        "    padding: 6px;"
        "    border-radius: 3px;"
        "    min-width: 60px;"
        "}"
        "QToolBar QToolButton:hover {"
        "    background-color: #555555;"
        "}"
        "QToolBar QToolButton:pressed {"
        "    background-color: #666666;"
        "}"
        "QTabWidget::pane {"
        "    border: 1px solid #555555;"
        "    background-color: #2b2b2b;"
        "}"
        "QTabBar::tab {"
        "    background-color: #3c3c3c;"
        "    color: #ffffff;"
        "    padding: 8px 16px;"
        "    margin-right: 2px;"
        "    border-top-left-radius: 4px;"
        "    border-top-right-radius: 4px;"
        "}"
        "QTabBar::tab:selected {"
        "    background-color: #2b2b2b;"
        "    border-bottom: 2px solid #0078d4;"
        "}"
        "QTabBar::tab:hover {"
        "    background-color: #4a4a4a;"
        "}"
        "QDockWidget {"
        "    background-color: #2b2b2b;"
        "    color: #ffffff;"
        "    titlebar-close-icon: none;"
        "    titlebar-normal-icon: none;"
        "}"
        "QDockWidget::title {"
        "    background-color: #3c3c3c;"
        "    padding: 6px;"
        "    border-bottom: 1px solid #555555;"
        "}"
        "QTreeWidget {"
        "    background-color: #2b2b2b;"
        "    color: #ffffff;"
        "    border: 1px solid #555555;"
        "    selection-background-color: #0078d4;"
        "}"
        "QTreeWidget::item {"
        "    padding: 4px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: #3c3c3c;"
        "}"
        "QTextEdit {"
        "    background-color: #1e1e1e;"
        "    color: #ffffff;"
        "    border: 1px solid #555555;"
        "    selection-background-color: #0078d4;"
        "}"
        "QStatusBar {"
        "    background-color: #3c3c3c;"
        "    color: #ffffff;"
        "    border-top: 1px solid #555555;"
        "}"
    );
}

void MainWindow::createWelcomeTab()
{
    QTextEdit *welcomeText = new QTextEdit();
    welcomeText->setReadOnly(true);
    welcomeText->setHtml(
        "<h1 style='color: #0078d4;'>Welcome to Krius IDE</h1>"
        "<h3>C++ Development Environment with BAGEL AI Integration</h3>"
        "<p><strong>Features:</strong></p>"
        "<ul>"
        "<li>Advanced C++ code editor with syntax highlighting</li>"
        "<li>Project management and file explorer</li>"
        "<li>Integrated BAGEL AI assistant for code generation and explanation</li>"
        "<li>Build system integration</li>"
        "<li>Dark theme optimized for development</li>"
        "</ul>"
        "<p><strong>Getting Started:</strong></p>"
        "<ul>"
        "<li>Create a new file: <kbd>Ctrl+N</kbd></li>"
        "<li>Open a file: <kbd>Ctrl+O</kbd></li>"
        "<li>Open a project folder: Project → Open Project Folder</li>"
        "<li>Toggle BAGEL AI Assistant: <kbd>Ctrl+Shift+A</kbd></li>"
        "</ul>"
        "<p><strong>BAGEL AI Features:</strong></p>"
        "<ul>"
        "<li>Code generation from natural language descriptions</li>"
        "<li>Code explanation and documentation</li>"
        "<li>Image generation for UI mockups</li>"
        "<li>Interactive chat for programming assistance</li>"
        "</ul>"
        "<p style='color: #888;'>Start by opening a project folder or creating a new file!</p>"
    );
    
    int index = m_tabWidget->addTab(welcomeText, "Welcome");
    m_tabWidget->setCurrentIndex(index);
}

void MainWindow::newFile()
{
    CodeEditor *editor = new CodeEditor();
    editor->setPlainText("// New C++ file\n#include <iostream>\n\nint main() {\n    std::cout << \"Hello, World!\" << std::endl;\n    return 0;\n}\n");
    
    static int fileCounter = 1;
    QString fileName = QString("untitled%1.cpp").arg(fileCounter++);
    
    int index = m_tabWidget->addTab(editor, fileName);
    m_tabWidget->setCurrentIndex(index);
    
    statusBar()->showMessage("New file created", 2000);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Open File", "", 
        "C++ Files (*.cpp *.h *.hpp *.c *.cc *.cxx);;Python Files (*.py);;JavaScript Files (*.js *.ts);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        openFileInEditor(fileName);
    }
}

void MainWindow::saveFile()
{
    CodeEditor *editor = getCurrentEditor();
    if (!editor) {
        statusBar()->showMessage("No file to save", 2000);
        return;
    }
    
    QString fileName = editor->property("fileName").toString();
    if (fileName.isEmpty()) {
        saveAsFile();
        return;
    }
    
    saveFileContent(fileName, editor->toPlainText());
}

void MainWindow::saveAsFile()
{
    CodeEditor *editor = getCurrentEditor();
    if (!editor) {
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save File", "",
        "C++ Files (*.cpp *.h *.hpp);;Python Files (*.py);;JavaScript Files (*.js);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        saveFileContent(fileName, editor->toPlainText());
        editor->setProperty("fileName", fileName);
        
        QFileInfo fileInfo(fileName);
        m_tabWidget->setTabText(m_tabWidget->currentIndex(), fileInfo.fileName());
    }
}

void MainWindow::openProject()
{
    QString projectDir = QFileDialog::getExistingDirectory(this, "Open Project Folder");
    if (!projectDir.isEmpty()) {
        m_projectManager->openProject(projectDir);
    }
}

void MainWindow::closeProject()
{
    m_projectManager->closeProject();
}

void MainWindow::buildProject()
{
    m_outputPanel->append("=== Build Started ===");
    m_outputPanel->append("Build functionality not yet implemented");
    m_outputPanel->append("=== Build Finished ===");
    statusBar()->showMessage("Build completed", 2000);
}

void MainWindow::runProject()
{
    m_outputPanel->append("=== Run Started ===");
    m_outputPanel->append("Run functionality not yet implemented");
    statusBar()->showMessage("Run completed", 2000);
}

void MainWindow::toggleBagel()
{
    if (m_bagelDock->isVisible()) {
        m_bagelDock->hide();
        statusBar()->showMessage("BAGEL AI Assistant hidden", 2000);
    } else {
        m_bagelDock->show();
        statusBar()->showMessage("BAGEL AI Assistant shown", 2000);
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About Krius IDE",
        "<h2>Krius IDE</h2>"
        "<p>Version 1.0.0</p>"
        "<p>A modern C++ development environment with BAGEL AI integration.</p>"
        "<p>Built with Qt5 and powered by BAGEL multimodal AI.</p>"
        "<p>© 2024 Krius IDE Project</p>");
}

void MainWindow::closeTab(int index)
{
    QWidget *widget = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);
    delete widget;
}

void MainWindow::openFileInEditor(const QString &fileName)
{
    // Check if file is already open
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget *widget = m_tabWidget->widget(i);
        if (widget->property("fileName").toString() == fileName) {
            m_tabWidget->setCurrentIndex(i);
            return;
        }
    }
    
    // Load file content
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", QString("Could not open file: %1").arg(fileName));
        return;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    // Create new editor
    CodeEditor *editor = new CodeEditor();
    editor->setPlainText(content);
    editor->setProperty("fileName", fileName);
    editor->setCurrentFile(fileName);
    
    QFileInfo fileInfo(fileName);
    int index = m_tabWidget->addTab(editor, fileInfo.fileName());
    m_tabWidget->setCurrentIndex(index);
    
    statusBar()->showMessage(QString("Opened: %1").arg(fileName), 2000);
}

void MainWindow::saveFileContent(const QString &fileName, const QString &content)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", QString("Could not save file: %1").arg(fileName));
        return;
    }
    
    QTextStream out(&file);
    out << content;
    file.close();
    
    statusBar()->showMessage(QString("Saved: %1").arg(fileName), 2000);
}

CodeEditor* MainWindow::getCurrentEditor()
{
    return qobject_cast<CodeEditor*>(m_tabWidget->currentWidget());
}

void MainWindow::onProjectOpened(const QString &projectPath)
{
    m_projectTree->clear();
    
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(m_projectTree);
    QFileInfo projectInfo(projectPath);
    rootItem->setText(0, projectInfo.fileName());
    rootItem->setData(0, Qt::UserRole, projectPath);
    
    populateProjectTree(rootItem, projectPath);
    rootItem->setExpanded(true);
    
    statusBar()->showMessage(QString("Project opened: %1").arg(projectPath), 3000);
}

void MainWindow::onProjectClosed()
{
    m_projectTree->clear();
    statusBar()->showMessage("Project closed", 2000);
}

void MainWindow::onProjectFileAdded(const QString &filePath)
{
    // Refresh project tree
    if (m_projectManager->isProjectOpen()) {
        onProjectOpened(m_projectManager->currentProject());
    }
}

void MainWindow::onProjectFileDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    
    QString filePath = item->data(0, Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isFile()) {
        openFileInEditor(filePath);
    }
}

void MainWindow::onBagelHealthCheck(bool isHealthy)
{
    if (isHealthy) {
        statusBar()->showMessage("BAGEL AI server is healthy and ready", 3000);
    } else {
        statusBar()->showMessage("BAGEL AI server is not responding", 3000);
    }
}

void MainWindow::populateProjectTree(QTreeWidgetItem *parentItem, const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    
    foreach (const QString &entry, entries) {
        QString fullPath = dir.absoluteFilePath(entry);
        QFileInfo fileInfo(fullPath);
        
        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
        item->setText(0, entry);
        item->setData(0, Qt::UserRole, fullPath);
        
        if (fileInfo.isDir()) {
            populateProjectTree(item, fullPath);
        }
    }
}