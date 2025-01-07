#ifndef CUSUBMITTERGUI_H
#define CUSUBMITTERGUI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CUSubmitterGUI; }
QT_END_NAMESPACE

class CUSubmitterGUI : public QMainWindow {
    Q_OBJECT

public:
    explicit CUSubmitterGUI(QWidget *parent = nullptr);
    ~CUSubmitterGUI() override;

private slots:
    void on_chgenButton_clicked();
    void on_transferButton_clicked();
    void on_submitButton_clicked();

private:
    Ui::CUSubmitterGUI *ui;
};

#endif // CUSUBMITTERGUI_H
