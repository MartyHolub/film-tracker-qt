#ifndef FILMDIALOG_H
#define FILMDIALOG_H

#include "mainwindow.h"

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;
class QTextEdit;

class FilmDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilmDialog(QWidget *parent = nullptr);

    void setFilm(const Film &film);
    Film film() const;

private:
    QLineEdit *titleEdit_ = nullptr;
    QSpinBox *yearSpin_ = nullptr;
    QComboBox *genreCombo_ = nullptr;
    QDoubleSpinBox *ratingSpin_ = nullptr;
    QSpinBox *lengthSpin_ = nullptr;
    QLineEdit *directorEdit_ = nullptr;
    QTextEdit *descriptionEdit_ = nullptr;
    QCheckBox *watchedCheck_ = nullptr;
    QCheckBox *favoriteCheck_ = nullptr;
    QCheckBox *watchlistCheck_ = nullptr;
    Film currentFilm_;
};

#endif // FILMDIALOG_H
