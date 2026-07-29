#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QDate>
#include <QLocale>

class QGridLayout;

class CalendarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarWidget(QWidget *parent = nullptr);

    void setYearMonth(int year, int month);
    int year() const { return m_year; }
    int month() const { return m_month; }
    void setSelectedDate(const QDate &date);
    QDate selectedDate() const { return m_selectedDate; }
    QDate todayDate() const { return m_today; }
    void setLocale(const QLocale &locale);
    QLocale locale() const { return m_locale; }
    void setHolidayLanguage(const QString &lang);

signals:
    void dateSelected(const QDate &date);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void rebuild();

    QGridLayout *m_grid;
    int m_year;
    int m_month;
    QDate m_today;
    QDate m_selectedDate;
    QLocale m_locale;
    QString m_holidayLang;
};

#endif