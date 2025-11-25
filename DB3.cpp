#pragma once

#include "DB3.h"
#include "Base.h"
#include "ui_DB3.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <string>

using namespace std;

DB3::DB3(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::DB3)
    , currentDB(nullptr)
{
    ui->setupUi(this);
    enableButtons(false);
    updateStatus("No database opened");
}

DB3::~DB3()
{
    delete currentDB;
    delete ui;
}

void DB3::updateStatus(const QString& message)
{
    ui->statusLabel->setText("Status: " + message);
}

void DB3::clearResults()
{
    ui->resultsText->clear();
}

void DB3::enableButtons(bool enabled)
{
    ui->deleteDB->setEnabled(enabled);
    ui->clearDB->setEnabled(enabled);
    ui->addRecord->setEnabled(enabled);
    ui->deleteByID->setEnabled(enabled);
    ui->deleteByField->setEnabled(enabled);
    ui->searchByID->setEnabled(enabled);
    ui->searchByField->setEnabled(enabled);
    ui->changeRecord->setEnabled(enabled);
    ui->backup->setEnabled(enabled);
    ui->restore->setEnabled(enabled);
    ui->exportCSV->setEnabled(enabled);
    ui->displayAll->setEnabled(enabled);
    ui->showStats->setEnabled(enabled);
}

void DB3::on_createDB_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Create Database", "", "Database Files (*.dat)");
    if (filename.isEmpty()) return;

    if (currentDB) {
        delete currentDB;
        currentDB = nullptr;
    }

    currentDB = new Base(filename.toStdString());
    if (currentDB->create()) {
        currentFilename = filename;
        enableButtons(true);
        updateStatus("Database created: " + filename);
        clearResults();
        ui->resultsText->setText("Database created successfully with 1000 records.");
    }
    else {
        delete currentDB;
        currentDB = nullptr;
        QMessageBox::critical(this, "Error", "Failed to create database");
        updateStatus("Failed to create database");
    }
}

void DB3::on_openDB_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Database", "", "Database Files (*.dat)");
    if (filename.isEmpty()) return;

    if (currentDB) {
        delete currentDB;
        currentDB = nullptr;
    }

    currentDB = new Base(filename.toStdString());
    if (currentDB->open()) {
        currentFilename = filename;
        enableButtons(true);
        updateStatus("Database opened: " + filename);
        clearResults();
        ui->resultsText->setText("Database opened successfully.");
    }
    else {
        delete currentDB;
        currentDB = nullptr;
        QMessageBox::critical(this, "Error", "Failed to open database");
        updateStatus("Failed to open database");
    }
}

void DB3::on_deleteDB_clicked()
{
    if (!currentDB) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete the database?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (currentDB->delete_db()) {
            clearResults();
            ui->resultsText->setText("Database deleted successfully.");
            updateStatus("Database deleted");
            enableButtons(false);
            delete currentDB;
            currentDB = nullptr;
            currentFilename.clear();
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to delete database");
        }
    }
}

void DB3::on_clearDB_clicked()
{
    if (!currentDB) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Clear",
        "Are you sure you want to clear all records?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (currentDB->clear_db()) {
            clearResults();
            ui->resultsText->setText("Database cleared successfully.");
            updateStatus("Database cleared");
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to clear database");
        }
    }
}

void DB3::on_addRecord_clicked()
{
    if (!currentDB) return;

    bool ok;
    int id = ui->idEdit->text().toInt(&ok);
    if (!ok || id < 0 || id >= 1000) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid ID (0-999)");
        return;
    }

    QString title = ui->titleEdit->text();
    if (title.isEmpty() || title.size() > 19) {
        QMessageBox::warning(this, "Input Error", "Title cannot be empty and must be 19 characters or less");
        return;
    }

    QString author = ui->authorEdit->text();
    if (author.isEmpty() || author.size() > 14) {
        QMessageBox::warning(this, "Input Error", "Author cannot be empty and must be 14 characters or less");
        return;
    }

    QString rating = ui->ratingEdit->text();
    if (rating.size() != 1 || !rating[0].isDigit() || rating[0] < '0' || rating[0] > '9') {
        QMessageBox::warning(this, "Input Error", "Rating must be a single digit (0-9)");
        return;
    }

    if (currentDB->addRecord(id, title.toStdString(), author.toStdString(), rating[0].toLatin1())) {
        clearResults();
        ui->resultsText->setText("Record added successfully at ID: " + QString::number(id));
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to add record");
    }
}

void DB3::on_deleteByID_clicked()
{
    if (!currentDB) return;

    bool ok;
    int id = ui->deleteIDEdit->text().toInt(&ok);
    if (!ok || id < 0 || id >= 1000) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid ID (0-999)");
        return;
    }

    if (currentDB->deleteByID(id)) {
        clearResults();
        ui->resultsText->setText("Record with ID " + QString::number(id) + " deleted successfully.");
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to delete record");
    }
}

void DB3::on_deleteByField_clicked()
{
    if (!currentDB) return;

    QString field = ui->deleteFieldCombo->currentText();
    QString value = ui->deleteValueEdit->text();

    if (value.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a value");
        return;
    }

    if (currentDB->deleteByField(field.toStdString(), value.toStdString())) {
        clearResults();
        ui->resultsText->setText("Records deleted successfully where " + field + " = '" + value + "'");
    }
    else {
        QMessageBox::critical(this, "Error", "No records found or failed to delete");
    }
}

void DB3::on_searchByID_clicked()
{
    if (!currentDB) return;

    bool ok;
    int id = ui->searchIDEdit->text().toInt(&ok);
    if (!ok || id < 0 || id >= 1000) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid ID (0-999)");
        return;
    }

    string result = currentDB->searchByID_GUI(id);
    ui->resultsText->setText(QString::fromStdString(result));
}

void DB3::on_searchByField_clicked()
{
    if (!currentDB) return;

    QString field = ui->searchFieldCombo->currentText();
    QString value = ui->searchValueEdit->text();

    if (value.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a value");
        return;
    }

    string result = currentDB->searchByField_GUI(field.toStdString(), value.toStdString());
    ui->resultsText->setText(QString::fromStdString(result));
}

void DB3::on_changeRecord_clicked()
{
    if (!currentDB) return;

    bool ok;
    int id = ui->changeIDEdit->text().toInt(&ok);
    if (!ok || id < 0 || id >= 1000) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid ID (0-999)");
        return;
    }

    QString title = ui->changeTitleEdit->text();
    if (title.isEmpty() || title.size() > 19) {
        QMessageBox::warning(this, "Input Error", "Title cannot be empty and must be 19 characters or less");
        return;
    }

    QString author = ui->changeAuthorEdit->text();
    if (author.isEmpty() || author.size() > 14) {
        QMessageBox::warning(this, "Input Error", "Author cannot be empty and must be 14 characters or less");
        return;
    }

    QString rating = ui->changeRatingEdit->text();
    if (rating.size() != 1 || !rating[0].isDigit() || rating[0] < '0' || rating[0] > '9') {
        QMessageBox::warning(this, "Input Error", "Rating must be a single digit (0-9)");
        return;
    }

    if (currentDB->changeRecord(id, title.toStdString(), author.toStdString(), rating[0].toLatin1())) {
        clearResults();
        ui->resultsText->setText("Record with ID " + QString::number(id) + " updated successfully.");
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to update record");
    }
}

void DB3::on_backup_clicked()
{
    if (!currentDB) return;

    QString filename = QFileDialog::getSaveFileName(this, "Create Backup", "", "Backup Files (*.txt)");
    if (filename.isEmpty()) return;

    if (currentDB->Backup(filename.toStdString())) {
        clearResults();
        ui->resultsText->setText("Backup created successfully: " + filename);
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to create backup");
    }
}

void DB3::on_restore_clicked()
{
    if (!currentDB) return;

    QString filename = QFileDialog::getOpenFileName(this, "Restore Backup", "", "Backup Files (*.txt)");
    if (filename.isEmpty()) return;

    if (currentDB->restoreFromBackup(filename.toStdString())) {
        clearResults();
        ui->resultsText->setText("Backup restored successfully: " + filename);
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to restore backup");
    }
}

void DB3::on_exportCSV_clicked()
{
    if (!currentDB) return;

    QString filename = QFileDialog::getSaveFileName(this, "Export to CSV", "", "CSV Files (*.csv)");
    if (filename.isEmpty()) return;

    if (currentDB->exportToCSV(filename.toStdString())) {
        clearResults();
        ui->resultsText->setText("Data exported to CSV successfully: " + filename);
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to export to CSV");
    }
}

void DB3::on_displayAll_clicked()
{
    if (!currentDB) return;

    string result = currentDB->displayAll_GUI();
    ui->resultsText->setText(QString::fromStdString(result));
}

void DB3::on_showStats_clicked()
{
    if (!currentDB) return;

    string result = currentDB->showStats_GUI();
    ui->resultsText->setText(QString::fromStdString(result));
}