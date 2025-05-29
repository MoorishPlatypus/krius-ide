#include "projectmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent)
    , m_fileWatcher(new QFileSystemWatcher(this))
{
    connect(m_fileWatcher, &QFileSystemWatcher::directoryChanged,
            this, &ProjectManager::onDirectoryChanged);
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged,
            this, &ProjectManager::onFileChanged);
}

bool ProjectManager::openProject(const QString &projectPath)
{
    QDir dir(projectPath);
    if (!dir.exists()) {
        return false;
    }
    
    closeProject();
    
    m_currentProjectPath = projectPath;
    m_fileWatcher->addPath(projectPath);
    
    scanProjectFiles();
    
    emit projectOpened(projectPath);
    return true;
}

void ProjectManager::closeProject()
{
    if (!m_currentProjectPath.isEmpty()) {
        m_fileWatcher->removePaths(m_fileWatcher->directories());
        m_fileWatcher->removePaths(m_fileWatcher->files());
        
        m_currentProjectPath.clear();
        m_projectFiles.clear();
        
        emit projectClosed();
    }
}

void ProjectManager::scanProjectFiles()
{
    if (m_currentProjectPath.isEmpty()) {
        return;
    }
    
    m_projectFiles.clear();
    
    QDir dir(m_currentProjectPath);
    QStringList filters;
    filters << "*.cpp" << "*.h" << "*.hpp" << "*.c" << "*.cc" << "*.cxx"
            << "*.py" << "*.js" << "*.ts" << "*.html" << "*.css" << "*.json"
            << "*.txt" << "*.md" << "*.cmake" << "CMakeLists.txt" << "*.pro"
            << "*.qrc" << "*.ui";
    
    QStringList files = dir.entryList(filters, QDir::Files | QDir::Readable, QDir::Name);
    
    foreach (const QString &file, files) {
        QString fullPath = dir.absoluteFilePath(file);
        addFileToProject(fullPath);
    }
    
    // Recursively scan subdirectories
    QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    foreach (const QString &subdir, subdirs) {
        QString subdirPath = dir.absoluteFilePath(subdir);
        m_fileWatcher->addPath(subdirPath);
        
        // Recursively scan subdirectory
        QDir subDir(subdirPath);
        QStringList subFiles = subDir.entryList(filters, QDir::Files | QDir::Readable, QDir::Name);
        foreach (const QString &file, subFiles) {
            QString fullPath = subDir.absoluteFilePath(file);
            addFileToProject(fullPath);
        }
    }
}

void ProjectManager::addFileToProject(const QString &filePath)
{
    if (!m_projectFiles.contains(filePath) && isSourceFile(filePath)) {
        m_projectFiles.append(filePath);
        m_fileWatcher->addPath(filePath);
        emit fileAdded(filePath);
    }
}

void ProjectManager::removeFileFromProject(const QString &filePath)
{
    if (m_projectFiles.contains(filePath)) {
        m_projectFiles.removeAll(filePath);
        m_fileWatcher->removePath(filePath);
        emit fileRemoved(filePath);
    }
}

bool ProjectManager::isSourceFile(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    
    QStringList sourceExtensions;
    sourceExtensions << "cpp" << "h" << "hpp" << "c" << "cc" << "cxx"
                     << "py" << "js" << "ts" << "html" << "css" << "json"
                     << "txt" << "md" << "cmake" << "pro" << "qrc" << "ui";
    
    return sourceExtensions.contains(suffix) || fileInfo.fileName() == "CMakeLists.txt";
}

void ProjectManager::onDirectoryChanged(const QString &path)
{
    Q_UNUSED(path)
    // Rescan project files when directory changes
    scanProjectFiles();
}

void ProjectManager::onFileChanged(const QString &path)
{
    emit fileChanged(path);
}