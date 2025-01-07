#include "cusubmittergui.h"
#include "ui_CUSubmitterGUI.h"
#include <QMessageBox>
#include <QFileDialog>
#include "../chgen/chgen.h"
#include "../transfer/transfer.h"
#include "../submit/submit.h"
#include "../utils/error.h"
#include "../utils/print.h"

CUSubmitterGUI::CUSubmitterGUI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CUSubmitterGUI) {
    ui->setupUi(this);
}

CUSubmitterGUI::~CUSubmitterGUI() {
    delete ui;
}

void CUSubmitterGUI::on_chgenButton_clicked() {
    QString basePath = QFileDialog::getExistingDirectory(this, tr("Select Base Path"));
    QString modifiedPath = QFileDialog::getExistingDirectory(this, tr("Select Modified Path"));

    if (basePath.isEmpty() || modifiedPath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Both paths must be selected."));
        return;
    }

    const auto changelog = chgen::ChangelogGenerator::scan(basePath.toStdString(), modifiedPath.toStdString());
    if (changelog == nullptr) {
        QMessageBox::critical(this, tr("Error"), tr("Could not generate changelog"));
        return;
    }

    chgen::ChangelogGenerator::generate(changelog);
    QMessageBox::information(this, tr("Success"), tr("Changelog generated successfully"));
}

void CUSubmitterGUI::on_transferButton_clicked() {
    QString basePath = QFileDialog::getExistingDirectory(this, tr("Select Base Path"));
    QString fromPath = QFileDialog::getExistingDirectory(this, tr("Select From Path"));
    QString toPath = QFileDialog::getExistingDirectory(this, tr("Select To Path"));

    if (basePath.isEmpty() || fromPath.isEmpty() || toPath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("All paths must be selected."));
        return;
    }

    const auto changelog = transfer::DevbuildTransferer::getTransferChangelog(basePath.toStdString(), fromPath.toStdString());
    if (changelog == nullptr) {
        QMessageBox::critical(this, tr("Error"), tr("Could not generate changelog"));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Transfer"), tr("Confirm transfer?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        QMessageBox::information(this, tr("Info"), tr("Transfer cancelled"));
        return;
    }

    transfer::DevbuildTransferer::transfer(toPath.toStdString());
    transfer::DevbuildTransferer::exportChangelog();
    QMessageBox::information(this, tr("Success"), tr("Transfer completed successfully"));
}

void CUSubmitterGUI::on_submitButton_clicked() {
    QString basePath = QFileDialog::getExistingDirectory(this, tr("Select Base Path"));
    QString modifiedPath = QFileDialog::getExistingDirectory(this, tr("Select Modified Path"));
    QString archivePath = QFileDialog::getSaveFileName(this, tr("Select Archive Path"), "", tr("All Files (*)"));

    if (basePath.isEmpty() || modifiedPath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Both paths must be selected."));
        return;
    }

    const auto changelog = submit::SubmissionBuilder::getSubmissionChangelog(basePath.toStdString(), modifiedPath.toStdString());
    if (changelog == nullptr) {
        QMessageBox::critical(this, tr("Error"), tr("Could not generate changelog"));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Submission"), tr("Confirm submission?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        QMessageBox::information(this, tr("Info"), tr("Submission cancelled"));
        return;
    }

    try {
        if (!archivePath.isEmpty()) {
            submit::SubmissionBuilder::submit(archivePath.toStdString());
        } else {
            submit::SubmissionBuilder::submit();
        }
        QMessageBox::information(this, tr("Success"), tr("Submission completed successfully"));
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Error"), tr(e.what()));
    }
}
