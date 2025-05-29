#include "mainwindow.h"
#include "editor/codeeditor.h"
#include "bagel/bagelchatwidget.h"
#include "bagel/bagelclient.h"
#include "project/projectmanager.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QCloseEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainSplitter(nullptr)
    , m_editorTabs(nullptr)
    , m_projectDock(nullptr)
    , m_bagelDock(nullptr)
    , m_projectTree(nullptr)
    , m_fileSystemModel(nullptr)
    , m_bagelChat(nullptr)
    , m_bagelClient(nullptr)
    , m_projectManager(nullptr)
{
    setWindowTitle("Krius IDE - Multimodal AI-Powered Development Environment");
    setWindowIcon(QIcon(":/icons/krius-logo.png"));
    resize(1400, 900);
    
    // Initialize components
    m_bagelClient = new BagelClient(this);
    m_projectManager = new ProjectManager(this);
    
    setupUI();
    setupMenus();
    setupToolbars();
    setupStatusBar();
    setupDockWidgets();
    connectSignals();
    
    // Load settings
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    // Save settings
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal);
    
    // Create editor tabs
    m_editorTabs = new QTabWidget;
    m_editorTabs->setTabsClosable(true);
    m_editorTabs->setMovable(true);
    
    // Add editor tabs to splitter
    m_mainSplitter->addWidget(m_editorTabs);
    
    // Set up central widget layout
    QHBoxLayout *layout = new QHBoxLayout(m_centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_mainSplitter);
    
    // Connect tab close signal
    connect(m_editorTabs, &QTabWidget::tabCloseRequested, [this](int index) {
        QWidget *widget = m_editorTabs->widget(index);
        m_editorTabs->removeTab(index);
        widget->deleteLater();
    });
}

void MainWindow::setupMenus()
{
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    m_newFileAction = new QAction("&New File", this);
    m_newFileAction->setShortcut(QKeySequence::New);
    m_newFileAction->setStatusTip("Create a new file");
    fileMenu->addAction(m_newFileAction);
    
    m_openFileAction = new QAction("&Open File", this);
    m_openFileAction->setShortcut(QKeySequence::Open);
    m_openFileAction->setStatusTip("Open an existing file");
    fileMenu->addAction(m_openFileAction);
    
    m_openProjectAction = new QAction("Open &Project", this);
    m_openProjectAction->setShortcut(QKeySequence("Ctrl+Shift+O"));
    m_openProjectAction->setStatusTip("Open a project folder");
    fileMenu->addAction(m_openProjectAction);
    
    fileMenu->addSeparator();
    
    m_saveFileAction = new QAction("&Save", this);
    m_saveFileAction->setShortcut(QKeySequence::Save);
    m_saveFileAction->setStatusTip("Save the current file");
    fileMenu->addAction(m_saveFileAction);
    
    m_saveAsFileAction = new QAction("Save &As...", this);
    m_saveAsFileAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsFileAction->setStatusTip("Save the current file with a new name");
    fileMenu->addAction(m_saveAsFileAction);
    
    fileMenu->addSeparator();
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Exit the application");
    fileMenu->addAction(m_exitAction);
    
    // AI Menu
    QMenu *aiMenu = menuBar()->addMenu("&AI");
    
    m_toggleBagelAction = new QAction("Toggle &BAGEL Chat", this);
    m_toggleBagelAction->setShortcut(QKeySequence("Ctrl+Shift+B"));
    m_toggleBagelAction->setStatusTip("Toggle BAGEL AI chat panel");
    m_toggleBagelAction->setCheckable(true);
    aiMenu->addAction(m_toggleBagelAction);
    
    // Help Menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = new QAction("&About Krius IDE", this);
    m_aboutAction->setStatusTip("Show information about Krius IDE");
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolbars()
{
    QToolBar *fileToolBar = addToolBar("File");
    fileToolBar->addAction(m_newFileAction);
    fileToolBar->addAction(m_openFileAction);
    fileToolBar->addAction(m_saveFileAction);
    fileToolBar->addSeparator();
    fileToolBar->addAction(m_toggleBagelAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

void MainWindow::setupDockWidgets()
{
    // Project Explorer Dock
    m_projectDock = new QDockWidget("Project Explorer", this);
    m_projectDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    m_projectTree = new QTreeView;
    m_fileSystemModel = new QFileSystemModel;
    m_fileSystemModel->setRootPath(QDir::homePath());
    m_projectTree->setModel(m_fileSystemModel);
    m_projectTree->setRootIndex(m_fileSystemModel->index(QDir::homePath()));
    m_projectTree->hideColumn(1); // Size
    m_projectTree->hideColumn(2); // Type
    m_projectTree->hideColumn(3); // Date Modified
    
    m_projectDock->setWidget(m_projectTree);
    addDockWidget(Qt::LeftDockWidgetArea, m_projectDock);
    
    // BAGEL Chat Dock
    m_bagelDock = new QDockWidget("BAGEL AI Assistant", this);
    m_bagelDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    
    m_bagelChat = new BagelChatWidget(m_bagelClient);
    m_bagelDock->setWidget(m_bagelChat);
    addDockWidget(Qt::RightDockWidgetArea, m_bagelDock);
    
    // Initially hide BAGEL dock
    m_bagelDock->hide();
}

void MainWindow::connectSignals()
{
    // File actions
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(m_openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_saveFileAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_saveAsFileAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(m_openProjectAction, &QAction::triggered, this, &MainWindow::openProject);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    
    // AI actions
    connect(m_toggleBagelAction, &QAction::triggered, this, &MainWindow::toggleBagelChat);
    
    // Help actions
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    
    // Project tree
    connect(m_projectTree, &QTreeView::doubleClicked, this, &MainWindow::onFileDoubleClicked);
}

void MainWindow::newFile()
{
    CodeEditor *editor = new CodeEditor;
    int index = m_editorTabs->addTab(editor, "Untitled");
    m_editorTabs->setCurrentIndex(index);
    editor->setFocus();
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open File", QDir::homePath(),
        "All Files (*.*);;C++ Files (*.cpp *.h);;Python Files (*.py);;JavaScript Files (*.js);;Text Files (*.txt)");
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            
            CodeEditor *editor = new CodeEditor;
            editor->setPlainText(content);
            editor->setCurrentFile(fileName);
            
            QFileInfo fileInfo(fileName);
            int index = m_editorTabs->addTab(editor, fileInfo.fileName());
            m_editorTabs->setCurrentIndex(index);
            
            statusBar()->showMessage(QString("Opened: %1").arg(fileName), 2000);
        }
    }
}

void MainWindow::saveFile()
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(m_editorTabs->currentWidget());
    if (editor) {
        if (editor->currentFile().isEmpty()) {
            saveAsFile();
        } else {
            QFile file(editor->currentFile());
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << editor->toPlainText();
                statusBar()->showMessage(QString("Saved: %1").arg(editor->currentFile()), 2000);
            }
        }
    }
}

void MainWindow::saveAsFile()
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(m_editorTabs->currentWidget());
    if (editor) {
        QString fileName = QFileDialog::getSaveFileName(this,
            "Save File", QDir::homePath(),
            "All Files (*.*);;C++ Files (*.cpp *.h);;Python Files (*.py);;JavaScript Files (*.js);;Text Files (*.txt)");
        
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << editor->toPlainText();
                editor->setCurrentFile(fileName);
                
                QFileInfo fileInfo(fileName);
                m_editorTabs->setTabText(m_editorTabs->currentIndex(), fileInfo.fileName());
                
                statusBar()->showMessage(QString("Saved: %1").arg(fileName), 2000);
            }
        }
    }
}

void MainWindow::openProject()
{
    QString projectDir = QFileDialog::getExistingDirectory(this,
        "Open Project Directory", QDir::homePath());
    
    if (!projectDir.isEmpty()) {
        m_fileSystemModel->setRootPath(projectDir);
        m_projectTree->setRootIndex(m_fileSystemModel->index(projectDir));
        m_currentProject = projectDir;
        
        QFileInfo dirInfo(projectDir);
        setWindowTitle(QString("Krius IDE - %1").arg(dirInfo.baseName()));
        
        statusBar()->showMessage(QString("Opened project: %1").arg(projectDir), 2000);
    }
}

void MainWindow::toggleBagelChat()
{
    if (m_bagelDock->isVisible()) {
        m_bagelDock->hide();
        m_toggleBagelAction->setChecked(false);
    } else {
        m_bagelDock->show();
        m_toggleBagelAction->setChecked(true);
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About Krius IDE",
        "<h2>Krius IDE</h2>"
        "<p>Version 1.0.0</p>"
        "<p>A modern, AI-powered integrated development environment with BAGEL multimodal AI integration.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Advanced code editing with syntax highlighting</li>"
        "<li>BAGEL AI assistant for code generation and explanation</li>"
        "<li>Multimodal AI capabilities (text, code, images)</li>"
        "<li>Project management and file explorer</li>"
        "<li>Modern dark theme interface</li>"
        "</ul>"
        "<p>Built with Qt5 and C++</p>");
}

void MainWindow::onFileDoubleClicked(const QModelIndex &index)
{
    if (m_fileSystemModel->isDir(index)) {
        return; // Don't open directories
    }
    
    QString fileName = m_fileSystemModel->filePath(index);
    QFile file(fileName);
    
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        
        CodeEditor *editor = new CodeEditor;
        editor->setPlainText(content);
        editor->setCurrentFile(fileName);
        
        QFileInfo fileInfo(fileName);
        int index = m_editorTabs->addTab(editor, fileInfo.fileName());
        m_editorTabs->setCurrentIndex(index);
        
        statusBar()->showMessage(QString("Opened: %1").arg(fileName), 2000);
    }
}