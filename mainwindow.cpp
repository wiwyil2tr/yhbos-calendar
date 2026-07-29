#include "mainwindow.h"
#include "calendarwidget.h"
#include "lunarcalendar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QDate>
#include <QLocale>
#include <QTranslator>
#include <QApplication>
#include <QMenuBar>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QTextEdit>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_translator(new QTranslator(this))
{
    QWidget *central = new QWidget;
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QMenu *settingsMenu = menuBar()->addMenu(tr("Settings"));
    QAction *settingsAction = settingsMenu->addAction(tr("Preferences..."));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);

    QMenu *helpMenu = menuBar()->addMenu(tr("About"));
    QAction *aboutAction = helpMenu->addAction(tr("About YHBOS Calendar"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::openAbout);

    auto *navBar = new QHBoxLayout;

    auto *prevYearBtn = new QPushButton("<<");
    auto *prevMonthBtn = new QPushButton("<");
    m_titleLabel = new QLabel;
    m_titleLabel->setAlignment(Qt::AlignCenter);
    auto *nextMonthBtn = new QPushButton(">");
    auto *nextYearBtn = new QPushButton(">>");

    navBar->addWidget(prevYearBtn);
    navBar->addWidget(prevMonthBtn);
    navBar->addStretch();
    navBar->addWidget(m_titleLabel);
    navBar->addStretch();
    navBar->addWidget(nextMonthBtn);
    navBar->addWidget(nextYearBtn);

    auto *controlBar = new QHBoxLayout;

    m_todayBtn = new QPushButton(tr("Today"));

    m_yearCombo = new QComboBox;
    for (int y = 1900; y <= 2100; ++y)
        m_yearCombo->addItem(QString::number(y), y);

    m_monthCombo = new QComboBox;
    for (int m = 1; m <= 12; ++m)
        m_monthCombo->addItem(QLocale().monthName(m), m);

    m_goBtn = new QPushButton(tr("Go"));

    m_searchInput = new QLineEdit;
    m_searchInput->setPlaceholderText(tr("YYYY-MM-DD"));
    m_jumpBtn = new QPushButton(tr("Jump"));

    controlBar->addWidget(m_todayBtn);
    controlBar->addWidget(m_yearCombo);
    controlBar->addWidget(m_monthCombo);
    controlBar->addWidget(m_goBtn);
    controlBar->addStretch();
    controlBar->addWidget(m_searchInput);
    controlBar->addWidget(m_jumpBtn);

    m_calendar = new CalendarWidget;

    m_infoLabel = new QLabel;
    m_infoLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(navBar);
    mainLayout->addLayout(controlBar);
    mainLayout->addWidget(m_calendar, 1);
    mainLayout->addWidget(m_infoLabel);

    setWindowTitle(tr("YHBOS Calendar"));
    resize(750, 620);

    QDate today = QDate::currentDate();
    m_yearCombo->setCurrentIndex(m_yearCombo->findData(today.year()));
    m_monthCombo->setCurrentIndex(today.month() - 1);

    QString sysLang = QLocale::system().bcp47Name();
    QStringList langs = {"en", "zh_CN", "ru"};
    int langIdx = langs.indexOf(sysLang);
    if (langIdx < 0) {
        sysLang = sysLang.left(2);
        langIdx = langs.indexOf(sysLang);
    }
    if (langIdx < 0)
        langIdx = langs.indexOf("en");
    m_currentLang = langs[langIdx];
    loadLanguage(m_currentLang);

    connect(prevYearBtn, &QPushButton::clicked, this, &MainWindow::goPrevYear);
    connect(prevMonthBtn, &QPushButton::clicked, this, &MainWindow::goPrevMonth);
    connect(nextMonthBtn, &QPushButton::clicked, this, &MainWindow::goNextMonth);
    connect(nextYearBtn, &QPushButton::clicked, this, &MainWindow::goNextYear);
    connect(m_todayBtn, &QPushButton::clicked, this, &MainWindow::goToday);
    connect(m_goBtn, &QPushButton::clicked, this, &MainWindow::onYearMonthChanged);
    connect(m_jumpBtn, &QPushButton::clicked, this, &MainWindow::jumpToDate);
    connect(m_searchInput, &QLineEdit::returnPressed, this, &MainWindow::jumpToDate);
    connect(m_calendar, &CalendarWidget::dateSelected, this, &MainWindow::onDateSelected);

    updateTitle();
    updateInfo(today);
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadLanguage(const QString &langCode)
{
    QApplication::removeTranslator(m_translator);

    if (langCode == "en")
        return;

    QString path = QString(":/translations/yhbos-calendar_%1").arg(langCode);
    if (m_translator->load(path))
        QApplication::installTranslator(m_translator);
}

void MainWindow::retranslateUi()
{
    m_todayBtn->setText(tr("Today"));
    m_goBtn->setText(tr("Go"));
    m_jumpBtn->setText(tr("Jump"));
    m_searchInput->setPlaceholderText(tr("YYYY-MM-DD"));
    setWindowTitle(tr("YHBOS Calendar"));
}

void MainWindow::openSettings()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Settings"));

    auto *layout = new QVBoxLayout(&dlg);

    auto *form = new QFormLayout;
    auto *langCombo = new QComboBox;
    langCombo->addItem("English", "en");
    langCombo->addItem("中文", "zh_CN");
    langCombo->addItem("Русский", "ru");

    QStringList langs = {"en", "zh_CN", "ru"};
    int idx = langs.indexOf(m_currentLang);
    if (idx < 0) idx = 0;
    langCombo->setCurrentIndex(idx);

    form->addRow(tr("Language:"), langCombo);
    layout->addLayout(form);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(buttons);

    if (dlg.exec() == QDialog::Accepted) {
        QString newLang = langCombo->currentData().toString();
        if (newLang != m_currentLang) {
            m_currentLang = newLang;
            loadLanguage(m_currentLang);

            QLocale locale(m_currentLang);
            m_calendar->setLocale(locale);
            m_calendar->setHolidayLanguage(m_currentLang);

            for (int m = 0; m < m_monthCombo->count(); ++m)
                m_monthCombo->setItemText(m, locale.monthName(m + 1));

            retranslateUi();
            updateTitle();
            updateInfo(m_calendar->selectedDate().isValid()
                       ? m_calendar->selectedDate()
                       : QDate::currentDate());
        }
    }
}

void MainWindow::openAbout()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("About YHBOS Calendar"));
    dlg.resize(500, 350);

    auto *layout = new QVBoxLayout(&dlg);

    auto *titleLabel = new QLabel(
        QString("<h2>YHBOS Calendar v1</h2>"));
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    auto *authorLabel = new QLabel(
        QString("<b>%1</b><br>%2 &lt;wiwyil2tr@ya.ru&gt;")
            .arg(tr("Author:"), "wiwyil2tr"));
    authorLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(authorLabel);

    layout->addWidget(new QLabel(QString("© 2026 wiwyil2tr. %1").arg(tr("All rights reserved."))));

    auto *licenseLabel = new QLabel(QString("<b>%1</b>").arg(tr("License: GNU General Public License v2")));
    licenseLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(licenseLabel);

    auto *gplNotice = new QTextEdit;
    gplNotice->setReadOnly(true);
    gplNotice->setPlainText(
        tr("This program is free software; you can redistribute it and/or "
           "modify it under the terms of the GNU General Public License "
           "as published by the Free Software Foundation; either version 2 "
           "of the License, or (at your option) any later version.\n\n"
           "This program is distributed in the hope that it will be useful, "
           "but WITHOUT ANY WARRANTY; without even the implied warranty of "
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
           "GNU General Public License for more details.\n\n"
           "You should have received a copy of the GNU General Public License "
           "along with this program; if not, write to the Free Software "
           "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  "
           "02110-1301, USA."));
    layout->addWidget(gplNotice, 1);

    auto *gplLink = new QLabel(
        "<a href=\"https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\">"
        "https://www.gnu.org/licenses/old-licenses/gpl-2.0.html</a>");
    gplLink->setOpenExternalLinks(true);
    gplLink->setAlignment(Qt::AlignCenter);
    layout->addWidget(gplLink);

    auto *closeBtn = new QPushButton(tr("Close"));
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(closeBtn);

    dlg.exec();
}

void MainWindow::goPrevMonth()
{
    int y = m_calendar->year();
    int m = m_calendar->month() - 1;
    if (m < 1) { m = 12; --y; }
    m_calendar->setYearMonth(y, m);
    updateTitle();
    syncYearMonthCombo();
}

void MainWindow::goNextMonth()
{
    int y = m_calendar->year();
    int m = m_calendar->month() + 1;
    if (m > 12) { m = 1; ++y; }
    m_calendar->setYearMonth(y, m);
    updateTitle();
    syncYearMonthCombo();
}

void MainWindow::goPrevYear()
{
    m_calendar->setYearMonth(m_calendar->year() - 1, m_calendar->month());
    updateTitle();
    syncYearMonthCombo();
}

void MainWindow::goNextYear()
{
    m_calendar->setYearMonth(m_calendar->year() + 1, m_calendar->month());
    updateTitle();
    syncYearMonthCombo();
}

void MainWindow::goToday()
{
    QDate today = QDate::currentDate();
    m_calendar->setYearMonth(today.year(), today.month());
    m_calendar->setSelectedDate(today);
    m_yearCombo->setCurrentIndex(m_yearCombo->findData(today.year()));
    m_monthCombo->setCurrentIndex(today.month() - 1);
    updateTitle();
    updateInfo(today);
}

void MainWindow::onYearMonthChanged()
{
    int y = m_yearCombo->currentData().toInt();
    int m = m_monthCombo->currentData().toInt();
    m_calendar->setYearMonth(y, m);
    updateTitle();
}

void MainWindow::onDateSelected(const QDate &date)
{
    updateInfo(date);
}

void MainWindow::jumpToDate()
{
    QString text = m_searchInput->text().trimmed();
    if (text.isEmpty()) return;
    QDate d = QDate::fromString(text, "yyyy-MM-dd");
    if (!d.isValid())
        d = QDate::fromString(text, "yyyy-M-d");
    if (!d.isValid())
        d = QDate::fromString(text, "yyyyMMdd");
    if (!d.isValid()) return;

    m_calendar->setYearMonth(d.year(), d.month());
    m_yearCombo->setCurrentIndex(m_yearCombo->findData(d.year()));
    m_monthCombo->setCurrentIndex(d.month() - 1);
    updateTitle();
    updateInfo(d);
}

void MainWindow::updateTitle()
{
    QFont tf = m_titleLabel->font();
    tf.setBold(true);
    m_titleLabel->setFont(tf);
    QLocale loc = m_calendar->locale();
    m_titleLabel->setText(QString("%1 %2")
        .arg(m_calendar->year())
        .arg(loc.monthName(m_calendar->month())));
}

void MainWindow::updateInfo(const QDate &date)
{
    LunarCalendar lc;
    lc.setLanguage(m_currentLang);
    lc.setSolarDate(date.year(), date.month(), date.day());

    QStringList parts;
    parts << lc.lunarMonth() + QString::fromUtf8("月") + lc.lunarDay();
    parts << lc.bazi();
    parts << lc.zodiac();
    parts << tr(lc.constellation().toUtf8().constData());
    QString h = lc.holiday();
    if (!h.isEmpty()) parts << h;
    QString info = parts.join("  |  ");

    m_infoLabel->setText(info);
}

void MainWindow::syncYearMonthCombo()
{
    m_yearCombo->setCurrentIndex(m_yearCombo->findData(m_calendar->year()));
    m_monthCombo->setCurrentIndex(m_calendar->month() - 1);
}
