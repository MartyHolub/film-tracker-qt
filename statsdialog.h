#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include "mainwindow.h"

#include <QDialog>
#include <QVector>

class QTextEdit;

class StatsDialog : public QDialog {
    Q_OBJECT

public:
    explicit StatsDialog(const QVector<Film> &films, QWidget *parent = nullptr);
};

#endif // STATSDIALOG_H
