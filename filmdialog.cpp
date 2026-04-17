#include "filmdialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

FilmDialog::FilmDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("Film"));
    resize(520, 520);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout();

    titleEdit_ = new QLineEdit(this);
    form->addRow(tr("Název filmu*"), titleEdit_);

    yearSpin_ = new QSpinBox(this);
    yearSpin_->setRange(1900, 2100);
    yearSpin_->setValue(QDate::currentDate().year());
    form->addRow(tr("Rok vydání"), yearSpin_);

    genreCombo_ = new QComboBox(this);
    genreCombo_->setEditable(true);
    genreCombo_->addItems({
        tr("Akční"), tr("Drama"), tr("Komedie"), tr("Sci-Fi"), tr("Thriller"),
        tr("Horor"), tr("Romantika"), tr("Dobrodružství"), tr("Animace"), tr("Krimi")
    });
    form->addRow(tr("Žánr"), genreCombo_);

    ratingSpin_ = new QDoubleSpinBox(this);
    ratingSpin_->setRange(0.0, 10.0);
    ratingSpin_->setSingleStep(0.1);
    ratingSpin_->setDecimals(1);
    form->addRow(tr("Hodnocení"), ratingSpin_);

    lengthSpin_ = new QSpinBox(this);
    lengthSpin_->setRange(0, 600);
    lengthSpin_->setSuffix(tr(" min"));
    form->addRow(tr("Délka"), lengthSpin_);

    directorEdit_ = new QLineEdit(this);
    form->addRow(tr("Režisér"), directorEdit_);

    descriptionEdit_ = new QTextEdit(this);
    descriptionEdit_->setMinimumHeight(120);
    form->addRow(tr("Popis"), descriptionEdit_);

    auto *checksLayout = new QHBoxLayout();
    watchedCheck_ = new QCheckBox(tr("Sledováno"), this);
    favoriteCheck_ = new QCheckBox(tr("Oblíbené"), this);
    watchlistCheck_ = new QCheckBox(tr("Watchlist"), this);
    checksLayout->addWidget(watchedCheck_);
    checksLayout->addWidget(favoriteCheck_);
    checksLayout->addWidget(watchlistCheck_);
    checksLayout->addStretch();

    layout->addLayout(form);
    layout->addLayout(checksLayout);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (titleEdit_->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Neplatná data"), tr("Název filmu je povinný."));
            return;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

void FilmDialog::setFilm(const Film &film) {
    currentFilm_ = film;
    titleEdit_->setText(film.nazev);
    yearSpin_->setValue(film.rok);
    genreCombo_->setCurrentText(film.zanr);
    ratingSpin_->setValue(film.hodnoceni);
    lengthSpin_->setValue(film.delka);
    directorEdit_->setText(film.reziser);
    descriptionEdit_->setPlainText(film.popis);
    watchedCheck_->setChecked(film.sledovano);
    favoriteCheck_->setChecked(film.oblibene);
    watchlistCheck_->setChecked(film.watchlist);
}

Film FilmDialog::film() const {
    Film out = currentFilm_;
    out.nazev = titleEdit_->text().trimmed();
    out.rok = yearSpin_->value();
    out.zanr = genreCombo_->currentText().trimmed();
    out.hodnoceni = ratingSpin_->value();
    out.delka = lengthSpin_->value();
    out.reziser = directorEdit_->text().trimmed();
    out.popis = descriptionEdit_->toPlainText().trimmed();
    out.sledovano = watchedCheck_->isChecked();
    out.oblibene = favoriteCheck_->isChecked();
    out.watchlist = watchlistCheck_->isChecked();
    return out;
}
