#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileSystemWatcher>

class ProjectManager : public QObject
{
    Q_OBJECT

public:
    explicit ProjectManager(QObject *parent = nullptr);
    
    bool openProject(const QString &projectPath);
    void closeProject();
    
    QString currentProject() const { return m_currentProjectPath; }
    QStringList projectFiles() const { return m_projectFiles; }
    
    bool isProjectOpen() const { return !m_currentProjectPath.isEmpty(); }

signals:
    void projectOpened(const QString &projectPath);
    void projectClosed();
    void fileAdded(const QString &filePath);
    void fileRemoved(const QString &filePath);
    void fileChanged(const QString &filePath);

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);

private:
    void scanProjectFiles();
    void addFileToProject(const QString &filePath);
    void removeFileFromProject(const QString &filePath);
    bool isSourceFile(const QString &filePath) const;
    
    QString m_currentProjectPath;
    QStringList m_projectFiles;
    QFileSystemWatcher *m_fileWatcher;
};

#endif // PROJECTMANAGER_H