#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTreeView>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QFileSystemModel>

class CodeEditor;
class BagelChatWidget;
class BagelClient;
class ProjectManager;
class SideBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void openProject();
    void toggleBagelChat();
    void showAbout();
    void onFileDoubleClicked(const QModelIndex &index);

private:
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    void setupDockWidgets();
    void connectSignals();
    
    // UI Components
    QWidget *m_centralWidget;
    QSplitter *m_mainSplitter;
    QTabWidget *m_editorTabs;
    
    // Docks
    QDockWidget *m_projectDock;
    QDockWidget *m_bagelDock;
    
    // Project Explorer
    QTreeView *m_projectTree;
    QFileSystemModel *m_fileSystemModel;
    
    // BAGEL Integration
    BagelChatWidget *m_bagelChat;
    BagelClient *m_bagelClient;
    
    // Managers
    ProjectManager *m_projectManager;
    
    // Actions
    QAction *m_newFileAction;
    QAction *m_openFileAction;
    QAction *m_saveFileAction;
    QAction *m_saveAsFileAction;
    QAction *m_openProjectAction;
    QAction *m_toggleBagelAction;
    QAction *m_aboutAction;
    QAction *m_exitAction;
    
    // Current file
    QString m_currentFile;
    QString m_currentProject;
};

#endif // MAINWINDOW_H