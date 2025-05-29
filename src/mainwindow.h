#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;
class QTreeWidget;
class QTextEdit;
class QDockWidget;
class QTreeWidgetItem;
class CodeEditor;
class BagelClient;
class BagelChatWidget;
class ProjectManager;

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
    void closeProject();
    void buildProject();
    void runProject();
    void toggleBagel();
    void showAbout();
    void closeTab(int index);
    
    // Project management slots
    void onProjectOpened(const QString &projectPath);
    void onProjectClosed();
    void onProjectFileAdded(const QString &filePath);
    void onProjectFileDoubleClicked(QTreeWidgetItem *item, int column);
    
    // BAGEL slots
    void onBagelHealthCheck(bool isHealthy);

private:
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupDockWidgets();
    void setupStatusBar();
    void setupBagel();
    void setupProjectManager();
    void applyDarkTheme();
    void createWelcomeTab();
    void openFileInEditor(const QString &fileName);
    void saveFileContent(const QString &fileName, const QString &content);
    void populateProjectTree(QTreeWidgetItem *parentItem, const QString &dirPath);
    
    CodeEditor* getCurrentEditor();
    
    // UI Components
    QTabWidget *m_tabWidget;
    QTreeWidget *m_projectTree;
    QTextEdit *m_outputPanel;
    
    // BAGEL Integration
    BagelClient *m_bagelClient;
    BagelChatWidget *m_bagelWidget;
    QDockWidget *m_bagelDock;
    
    // Project Management
    ProjectManager *m_projectManager;
};

#endif // MAINWINDOW_H