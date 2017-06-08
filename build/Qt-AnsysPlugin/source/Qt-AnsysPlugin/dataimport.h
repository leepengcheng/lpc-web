#ifndef DATAIMPORT_H
#define DATAIMPORT_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <matconfigure.h>

namespace Ui {
class DataImport;
}

class DataImport : public QDialog
{
    Q_OBJECT

public:
    explicit DataImport(MatConfigure *parent = 0);
    ~DataImport();

private:
    Ui::DataImport *ui;
    MatConfigure *main;
    QString xmlfile;
    QString msgfile;
    void InitializeDialog();
private slots:
    void LoadTreeData(QAbstractButton*);
};

#endif // DATAIMPORT_H
