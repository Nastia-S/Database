#pragma once

#include <QMainWindow>
#include "Base.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class DB3;
}
QT_END_NAMESPACE

class DB3 : public QMainWindow
{
    Q_OBJECT

public:
    DB3(QWidget* parent = nullptr);
    ~DB3();

private slots:
    void on_createDB_clicked();
    void on_openDB_clicked();
    void on_deleteDB_clicked();
    void on_clearDB_clicked();
    void on_addRecord_clicked();
    void on_deleteByID_clicked();
    void on_deleteByField_clicked();
    void on_searchByID_clicked();
    void on_searchByField_clicked();
    void on_changeRecord_clicked();
    void on_backup_clicked();
    void on_restore_clicked();
    void on_exportCSV_clicked();
    void on_displayAll_clicked();
    void on_showStats_clicked();

private:
    Ui::DB3* ui;
    Base* currentDB;
    QString currentFilename;

    void updateStatus(const QString& message);
    void clearResults();
    void enableButtons(bool enabled);
};